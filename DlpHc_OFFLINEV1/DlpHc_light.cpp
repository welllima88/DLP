// Dlp_Hc.cpp : Defines the entry point for the DLL application.
#include<memory.h>


#include "stdafx.h"
#include <fcntl.h>
#include <process.h>
#include <io.h>



LPVOID  CreateMappingFile();
void    DestroyMappingFile(LPVOID MPtr); 

#pragma warning(disable : 4800)
#pragma warning(disable : 4018)
#pragma warning(disable : 4550)
#pragma warning(disable : 4101)
#pragma warning(disable : 4700)


//DllExport void  __stdcall Dlp_Hc(S1 &,
extern "C" DllExport void  __cdecl Dlp_Hc(S1 &,
								S2 &,
								int (*pGetSecurity)(void),
								int (*pGetKey)(void),
								int (*pGetStringFromKeyboard)(char *, int, char *),
								void(*pPos_display)(char *, int, short));



/*-----------------------------------------------------------*/
/*     prototypes des fonctions utilisées pour les tests     */
/*-----------------------------------------------------------*/
void	setResponse(char, char *, S2 *);
void	initSQM(S3 *);

bool	TestMsgSend(S1 *, S2 *);
bool	SetHcAuthorizationMsg(S1 *, S3 *);
bool	Abort();
bool	Close();
bool	Discard(DWORD);
bool	DecodeHeader(LPCSTR,char&,WORD&,DWORD&);
bool	BuildHeader(LPSTR,char,WORD,DWORD);
bool	Send(LPCSTR, DWORD,int);
bool	StrSend(LPCSTR, DWORD);
bool	Receive(DWORD,LPSTR,DWORD,int);
bool	StrReceive(DWORD,LPSTR,DWORD);
WORD	GetNextMessageNumber();
void    CreateHcCtxDlp(char *fConfig, char *fNewConfig, MsgFromStratus Msg);
bool    GetStratusCom(char *fConfig);


/*-----------------------------------------------------------*/
/*     création de la socket et des variables utilisées      */
/*-----------------------------------------------------------*/
static SOCKET	m_hSocket;
static WORD		m_wMessageNumber;
static int		(*GetSecurity)(void);
static int		(*GetKey)(void);
static int		(*GetStringFromKeyboard)(char *, int, char *);
static void		(*Pos_display)(char *, int, short);

WORD			/*wRemotePort,*/
				wMessageNumber = GetNextMessageNumber();
DWORD			dwTimeout,
				dwlendata,
				dwLenData,
				dwNonBlockingMode = 1; //socket en mode non-bloquant. 0 si mode bloquant.
DWORD			longueur;
LPSTR			lpdata,
				lpszHeader;
LPCSTR			lpData,
				wRemotePort,
				lpszRemoteAddress;
bool			m_bStartedUp;
char			szHeader[SOCKON2_HEADER_LEN + 1],
				HcMsg[174],
				HcRep[73],
				HcRepInit[63],
				ConfigHost[32],
				ConfigSocket[5];
S2 *			hpmr;



/*-----------------------------------------------------------*/

BOOL APIENTRY DllMain( HANDLE, DWORD  ul_reason_for_call, LPVOID)
{
		switch (ul_reason_for_call)
	{
		case DLL_PROCESS_ATTACH:{}
								// PAS DE break;
		case DLL_THREAD_ATTACH:	{return TRUE;}
								break;
		case DLL_THREAD_DETACH:	{return TRUE;}
								break;
		case DLL_PROCESS_DETACH:{return TRUE;}
								break;
		default:				{return FALSE;}
								break;
	}
		
}/* end BOOL APIENTRY... */ 

/*** Dlp_Hc module **********************************************************/
/*                                                                         */
/*   Modulede traitement du message applicatif                             */
/*                                                                         */
/*                                                                         */
/*                                                                         */
/***************************************************************************/

extern "C" DllExport void __cdecl Dlp_Hc(S1 &HcPosMsgSend,
					 S2 &HcPosMsgRecv,
					 int (*pGetSecurity)(void),
					 int (*pGetKey)(void),
					 int (*pGetStringFromKeyboard)(char *, int, char *),
					 void(*pPos_display)(char *, int, short))


{
	S1 * hpms = &HcPosMsgSend;
	hpmr = &HcPosMsgRecv;
	FILE *fp ;
	char line [18], hostName [13];
	char Port[5];
	char *args[2], Prog[80];
    char   szPan[20], szMsg[255]; 
    char *limit;
	int Ret;
    static char   sztmp[4]; 
    char Msg[255], szMontantE[11], szMontantD[3];
    	
	
	S3 * stratusQueryMsg = new S3;
	S4 * stratusReplyMsg = new S4;

	struct MsgToStratus s;

	static char   szGen[16];


	memset(HcMsg,'\0',sizeof(HcMsg));
	memset(HcRep,'\0',sizeof(HcRep));
	memset(ConfigSocket,'\0',sizeof(ConfigSocket));
	memset(ConfigHost,'\0',sizeof(ConfigHost));

    
	fflush(0);
	
	Pos_display("Hotel Charge v4.2 - 5 juin 2006",17,0);
	if (!(CheckContext(hpms,s))) 
		
	{
		setResponse('1',"Erreur récupération du port à partir des fichiers de configuration",hpmr);	
		Pos_display("Erreur socket dans config",25,0);
		return;
	}			
	
    memset (szGen,'\0',sizeof(szGen)); 
    sprintf (szGen, "%s.", "./HcCtxDlp");
	strncat (szGen, hpms->aTermNum, 3);

	


	if(TestMsgSend(hpms, hpmr)) 	//si le message passé en paramètre est valide
	{
		initSQM(stratusQueryMsg);	//init de la structure passée au stratus
		if(SetHcAuthorizationMsg(hpms,stratusQueryMsg))	//création du message pour le stratus

		{
			memcpy(HcMsg,stratusQueryMsg,sizeof(HcMsg));

			// Initialiser la DLL WinSock lors de la première ouverture d'un socket
			if (!m_bStartedUp)
			{	
				WORD wVersionRequested = MAKEWORD(1,1);
				WSADATA wsaData;
				if(::WSAStartup(wVersionRequested, &wsaData) == 0)
				{
					// Vérifier la version supportée par la DLL WinSock
					if (wsaData.wVersion == wVersionRequested) m_bStartedUp = TRUE;
				}
				else ::WSACleanup();
			}
			if (!m_bStartedUp)
			{
				Pos_display("DLL : erreur d'initialisation du socket",39,0);
				//modification de OTHMAN le 08/10/01
				delete stratusQueryMsg;
				delete stratusReplyMsg;
				fflush(0);
				return;
				//exit(660);
			}
		}
			
		
		/* Le socket ne doit pas être déjà ouvert */
		if (m_hSocket == INVALID_SOCKET)
		{
			Close();
		}

				

		/* Construire l'adresse de la machine distante */
		SOCKADDR_IN addrRemote;
		struct   hostent  FAR *nHost;

        GetStratusCom(szGen);
		if ( (nHost = gethostbyname( ConfigHost) ) == 0 ) 
		{
		  return ;
		}

        wRemotePort=ConfigSocket; 

        memset(&addrRemote, 0, sizeof(addrRemote));
        memcpy  ( & ( addrRemote.sin_addr) ,nHost->h_addr, sizeof addrRemote.sin_addr );
		addrRemote.sin_family = AF_INET;
		addrRemote.sin_port = ::htons(atoi(wRemotePort));


		// Ouvrir la socket 
		Sleep(500);
		if ((m_hSocket = ::socket(PF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET)
		{
			Close();
			Pos_display("DLL : Socket deja ouvert",24,0);
			//modification de OTHMAN le 08/10/01
			setResponse('1',"Socket deja ouvert",hpmr);
			delete stratusQueryMsg;
			delete stratusReplyMsg;
			fflush(0);		
			//exit(662);
		}

		/* Connecter le socket à la machine distante */
		if (::connect(m_hSocket, (LPSOCKADDR)&addrRemote, sizeof(addrRemote)) != 0)
		{
			::closesocket(m_hSocket);
			m_hSocket = INVALID_SOCKET;
#ifdef _DEBUG
			Pos_display("DLL : Connexion au serveur impossible",37,0);
#endif
			//modification de OTHMAN le 08/10/01
			setResponse('1',"Connexion au serveur impossible",hpmr);
			delete stratusQueryMsg;
			delete stratusReplyMsg;
			fflush(0);	
			return;
			//exit(663);
		}
		
		/* Passer le socket en mode non-bloquant */
		if (::ioctlsocket(m_hSocket, FIONBIO, &dwNonBlockingMode) != 0)
		{
			::closesocket(m_hSocket);
			m_hSocket = INVALID_SOCKET;
			Pos_display("DLL : paramètrage en mode NON BLOQUANT impossible",48,0);
			//modification de OTHMAN le 08/10/01
			setResponse('1',"paramètrage en mode NON BLOQUANT impossible",hpmr);
			delete stratusQueryMsg;
			delete stratusReplyMsg;
			fflush(0);	
			return;
			//exit(664);
		}

		/* Ouverture du socket terminée */
		/* envoi du message pour le stratus */
		if(!Send(HcMsg,sizeof(HcMsg),1))
		{
			Abort();
			Pos_display("DLL : echec d'emission des données",34,0);
			//modification de OTHMAN le 08/10/01
			setResponse('1',"Echec d'emission des données",hpmr);
			delete stratusQueryMsg;
			delete stratusReplyMsg;
			fflush(0);
			return;		
			//exit(665);
		}
		

		/* jusqu'à ce point tout fonctionne a priori correctement */
		/* reception du message renvoyé par le serveur monétique */
		char hcrep[83];
		if(!Receive(dwTimeout, hcrep, sizeof(HcRep)+SOCKON2_HEADER_LEN,1))
		{
			Abort();
			Pos_display("DLL : Echec de Reception",24,0);
			
			//modification de OTHMAN le 08/10/01	
			setResponse('1',"Echec de Reception",hpmr);
			delete stratusQueryMsg;
			delete stratusReplyMsg;
			fflush(0);		
			return;
			//exit(666);
		}
		else 
		{
			memcpy(stratusReplyMsg,HcRep,sizeof(HcRep));
			#ifdef _DEBUG
			//Pos_display(&stratusReplyMsg->rCreditPlan,0,0);
			#endif
			/* test de la reponse retournée par le stratus */
			//cout<<"code retour stratus "<<stratusReplyMsg->rStatusCode;
			switch(stratusReplyMsg->rStatusCode)
			{
			case '0':	/* transaction acceptée */

				Pos_display("transaction acceptee...",23,0);
				setResponse(stratusReplyMsg->rStatusCode,(char*) stratusReplyMsg->rResponseMessageText,hpmr);
				break;

			case '1':	/* transaction refusée et non forcable */

				Pos_display("transaction refusee...",22,0);

				setResponse(stratusReplyMsg->rStatusCode,(char*) stratusReplyMsg->rResponseMessageText,hpmr);
				delete stratusQueryMsg;
		        delete stratusReplyMsg;
				fflush(0);
				Close(); //SJ050606
				return;
				break;			
			
			case '2':	/* transaction refusée mais forcable */

				Pos_display("transaction refusee. Forcage...",31,0);

				if(!GetSecurity())
				{ 
					Pos_display("transaction refusee... ",23,0);
			    	setResponse('1',"transaction refusee ",hpmr);
					Close(); //SJ050606
					delete stratusQueryMsg;
		            delete stratusReplyMsg;
					fflush(0);
					return;
					break;
				}
				else
				{
					char m_numauto[6];
					int k;
					char szTmp[255];

					memset(szTmp, 0x00, sizeof(szTmp));
					memcpy(szTmp, stratusReplyMsg->rResponseMessageText,sizeof(stratusReplyMsg->rResponseMessageText));
                    Pos_display(szTmp,strlen(szTmp),0);
					//cout<<"code2 "<<GetStringFromKeyboard<<endl;
					switch(GetStringFromKeyboard(m_numauto,sizeof(m_numauto),"saisie du code d'autorisation : "))
					{
					case '\x41':	/* transaction abandonnée */
						Pos_display("transaction abandonnee",22,0);
						setResponse('1',"Trans. abandonnee",hpmr);
						delete stratusQueryMsg;
		                delete stratusReplyMsg;
						fflush(0);
						return;
						break;
					case '\x12':	/* transaction acceptée */
					
						for(k=0;k<sizeof(stratusQueryMsg->ForcePostAuthCode);k++)
						stratusQueryMsg->ForcePostAuthCode[k] = m_numauto[k];
						
						stratusQueryMsg->ForcePostFlag='1';

						memcpy(HcMsg,stratusQueryMsg,sizeof(HcMsg));

						/* envoi du message avec le num_auto pour le stratus */
					if(!Send(HcMsg,sizeof(HcMsg),1))
					{
						Abort();

						//modification de OTHMAN le 08/10/01
						setResponse('1',"Echec d'emission des données",hpmr);
						delete stratusQueryMsg;
						delete stratusReplyMsg;
						fflush(0);
						Pos_display("echec d'emission des données avec num d'auto",44,0);
						return;		
					}

						Pos_display("transaction acceptee...",23,0);

						setResponse('0',"Trans. acceptee",hpmr);
						break;
						
					default :		/* erreur dans tout autre cas */
						Pos_display("erreur code autorisation",24,0);
						setResponse('1',"Trans. refusee",hpmr);
						break;
					}
				}
				break;

			default:	/* erreur pour toute autre valeur */
#ifdef _DEBUG
				Pos_display("erreur code reponse",19,0);
#endif
				setResponse('1',"Trans. refusee",hpmr);
				delete stratusQueryMsg;
				delete stratusReplyMsg;
				fflush(0);		
				Pos_display("Transaction refusée...",22,0);
				return;
				//exit(667);
				break;
			}

            delete stratusQueryMsg;
		    delete stratusReplyMsg;
		    fflush(0);
			Close();
		}
	}
	else
	{	
		delete stratusQueryMsg;
		delete stratusReplyMsg;
		fflush(0);
		Pos_display("Transaction refusée... Mauvais paramètres caisse",49,0);
		return;
	}
        
		return;
}/* end Dlp_Hc... */




/*** setResponse ***********************************************************/
/*                                                                         */
/*   Affectation à la structure de réponse applicative                     */
/*                                                                         */
/*                                                                         */
/*                                                                         */
/***************************************************************************/
void setResponse(char c, char * e, S2 * hpmr)
{

	memset (hpmr->bExplanation, 0x00, sizeof(hpmr->bExplanation));
	hpmr->bResponseCode = c;
	memcpy(hpmr->bExplanation, e, sizeof(hpmr->bExplanation)-1);
}/* end void setResponse */



/*** TestMsgSend ***********************************************************/
/*                                                                         */
/*   Test du message applicatif reçu                                       */
/*                                                                         */
/*                                                                         */
/*                                                                         */
/***************************************************************************/
bool TestMsgSend(S1 * hpms, S2 * hpmr)
{

	static int numTest=1, i=0;
	
	/*tests de validité du message envoyé par le POS*/
	do{
	switch(numTest)
	{
	case 1:
		if(hpms->aOperation=='d' ||
		   hpms->aOperation=='c' ) numTest++;
		else
		{
			printf("saisie incorrecte de l'operation \n");
			setResponse('1',"aop Error", hpmr);
			Pos_display("saisie incorrecte de l'operation",32,2);
			numTest = 66;
		}
		break;
	case 2:
		if(	isdigit(hpms->aAmount[0])	&& hpms->aAmount[0] == '0' &&
			isdigit(hpms->aAmount[1])	&& hpms->aAmount[1] == '0' &&
			isdigit(hpms->aAmount[2])	&& hpms->aAmount[2] == '0' &&
			isdigit(hpms->aAmount[3])	&&
			isdigit(hpms->aAmount[4])	&&
			isdigit(hpms->aAmount[5])	&&
			isdigit(hpms->aAmount[6])	&&
			isdigit(hpms->aAmount[7])	&&
			isdigit(hpms->aAmount[8])	&&
			isdigit(hpms->aAmount[9])	&&
			isdigit(hpms->aAmount[10])	&&
			isdigit(hpms->aAmount[11])  
			)	numTest++;
		else
		{
			printf("saisie incorrecte du montant \n");
			setResponse('1',"aam Error", hpmr);
			Pos_display("saisie incorrecte du montant",28,2);
			numTest = 66;			
		}
		break;
	case 3:
		if(	strncmp(hpms->aCurrencyCode,CURRENCY_FRANCS,strlen(CURRENCY_FRANCS))==0 ||
			strncmp(hpms->aCurrencyCode,CURRENCY_EURO,strlen(CURRENCY_EURO))==0 )
			numTest++;
		else
		{
			printf("saisie incorrecte de la devise \n");
			setResponse('1',"acc Error", hpmr);
			Pos_display("saisie incorrecte de la devise",30,2);
			numTest = 66;			
		}		
		break;
	case 4:
		if( isdigit(hpms->aCtrlNum[0]) &&
			isdigit(hpms->aCtrlNum[1]) &&
			isdigit(hpms->aCtrlNum[2]) &&
			isdigit(hpms->aCtrlNum[3]) ) numTest++;
		else
		{
			printf("saisie incorrecte du numero de controleur \n");
			setResponse('1',"act Error", hpmr);
			Pos_display("saisie incorrecte du numero de controleur",41,2);
			numTest = 66;			
		}
		break;
	case 5:
		if(	isdigit(hpms->aRegister)) numTest++;
		else
		{
			printf("saisie incorrecte du numero de register \n");
			setResponse('1',"are Error", hpmr);
			Pos_display("saisie incorrecte du numero de register",39,2);
			numTest = 66;			
		}
		break;
	case 6:
		if( isdigit(hpms->aTermNum[0]) &&
			isdigit(hpms->aTermNum[1]) &&
			isdigit(hpms->aTermNum[2]) ) numTest++;
		else
		{
			printf("saisie incorrecte du numero du terminal \n");
			setResponse('1',"ate Error", hpmr);
			Pos_display("saisie incorrecte du numero de terminal",39,2);
			numTest = 66;			
		}
		break;
	case 7:
		if( isdigit(hpms->aCashNum[0]) && hpms->aCashNum[0] =='0' &&
			isdigit(hpms->aCashNum[1]) &&
			isdigit(hpms->aCashNum[2]) &&
			isdigit(hpms->aCashNum[3]) &&
			isdigit(hpms->aCashNum[4]) &&
			isdigit(hpms->aCashNum[5]) &&
			isdigit(hpms->aCashNum[6]) ) numTest++;
		else
		{
			printf("saisie incorrecte du numero de caisse \n");
			setResponse('1',"aca Error", hpmr);
			Pos_display("saisie incorrecte du numero de caisse",37,2);
			numTest = 66;			
		}
		break;
	case 8:
		if( isdigit(hpms->aTrnsNum[0]) &&
			isdigit(hpms->aTrnsNum[1]) &&
			isdigit(hpms->aTrnsNum[2]) &&
			isdigit(hpms->aTrnsNum[3]) &&
			isdigit(hpms->aTrnsNum[4]) &&
			isdigit(hpms->aTrnsNum[5])  ) numTest++;
		else
		{
			printf("saisie incorrecte du numero de transaction \n");
			setResponse('1',"atr Error", hpmr);
			Pos_display("saisie incorrecte du numero de transaction",42,2);
			numTest = 66;			
		}
		break;
	case 9:
		if( hpms->aCustomerPresent == '0' ||
			hpms->aCustomerPresent == '1' ) numTest++;
		else
		{
			printf("saisie incorrecte du code-presence-client \n");
			setResponse('1',"acu Error", hpmr);
			Pos_display("saisie incorrecte du code-presence-client",41,2);
			numTest = 66;			
		}
		break;
	case 10:
		if( isdigit(hpms->aChargeNum[0]) &&
			isdigit(hpms->aChargeNum[1]) ) numTest++;
		else
		{
			printf("saisie incorrecte du numero de l'hotel charge \n");
			setResponse('1',"ach1 Error", hpmr);
			Pos_display("saisie incorrecte du numero de l'hotel",38,2);
			numTest = 66;
		}
		break;
	case 11:
		if( isdigit(hpms->aChargeNum[2]) &&
			isdigit(hpms->aChargeNum[3]) &&
			isdigit(hpms->aChargeNum[4]) &&
			isdigit(hpms->aChargeNum[5]) &&
			isdigit(hpms->aChargeNum[6]) &&
			isdigit(hpms->aChargeNum[7]) &&
			isdigit(hpms->aChargeNum[8]) &&
			!isalnum(hpms->aChargeNum[9])) numTest = 12;
		else
		{
			printf("saisie incorrecte du numero de l'hotel charge \n");
			setResponse('1',"ach2 Error", hpmr);
			Pos_display("saisie incorrecte du numero de l'hotel charge",45,2);
			numTest = 66;
		}
		break;
	default:
		numTest=66;
		break;
	}
	}while(numTest!=66 && numTest != 12);

	/*création d'une réponse positive*/
	if(numTest == 12) 
	{
		return (true);;
	}
	else 
	{
		return (false);;
	}

}/* end function */



/*** SetHcAuthorizationMsg *************************************************/
/*                                                                         */
/*   Affectations du message à envoyer au stratus                          */
/*                                                                         */
/*                                                                         */
/*                                                                         */
/***************************************************************************/
bool SetHcAuthorizationMsg(S1 * hpms, S3 * stratusQueryMsg)
{
	int i=0;
	
	for(i=0;i<sizeof(stratusQueryMsg->SourceTerminal);i++)
		stratusQueryMsg->SourceTerminal[i] = hpms->aTermNum[i+1];
	stratusQueryMsg->FS2 = '\x29';

	memcpy(stratusQueryMsg->SystemNumber,hpms->aCtrlNum,4);  //controleur

	stratusQueryMsg->LocationNumber[3] = hpms->aCtrlNum[4];  // register


	if(hpms->aOperation == 'd') stratusQueryMsg->AmountOfCharges[0] = ' ';
	else stratusQueryMsg->AmountOfCharges[0] = '-';

	for(i=0;i<(sizeof(stratusQueryMsg->AmountOfCharges)-1);i++)
		stratusQueryMsg->AmountOfCharges[i+1] = hpms->aAmount[i+3];	/* troncage de 3 à gauche */
	
	for(i=0;i<sizeof(stratusQueryMsg->TransactionNumber);i++)
		stratusQueryMsg->TransactionNumber[i] = hpms->aTrnsNum[i+2];/* troncage de 2 à gauche */
	
	memcpy(stratusQueryMsg->HotelCode,hpms->aChargeNum,2);

	for(i=0;i<sizeof(stratusQueryMsg->FolioNumber);i++)
		stratusQueryMsg->FolioNumber[i] = hpms->aChargeNum[i+2];

	for(i=0;i<sizeof(stratusQueryMsg->CurrencyCode);i++)
		stratusQueryMsg->CurrencyCode[i] = hpms->aCurrencyCode[i];

	for(i=0;
	    i< (sizeof(stratusQueryMsg->CashierNumber) + sizeof(stratusQueryMsg->ServerNumber));
		i++)
		stratusQueryMsg->CashierNumber[i] = hpms->aCashNum[i+1];
	stratusQueryMsg->EndOfMessage = '\x0D';
	return(true);

	/***** A FAIRE **************************************************************/
	/*																			*/
	/* must be extended to accept cases '1' and '2'								*/
	/* --->  stratusQueryMsg->ForcePostFlag, initialization : '0';				*/
	/*																			*/
	/* must be modifed to contain the returned value from function GetSecurity()*/
	/* ---> stratusQueryMsg->ForcePostAuthCode, initialization blank			*/
	/*																			*/
	/* must be modified to contain he returned value from function GetNumHC()	*/
	/* ---> stratusQueryMsg->HotelCode, initialization : blank					*/
	/*																			*/
	/****************************************************************************/


} /* end of function */


/*** initSQM ***************************************************************/
/*                                                                         */
/*   Initialisation du message à envoyer au stratus                        */
/*                                                                         */
/*                                                                         */
/*                                                                         */
/***************************************************************************/
void initSQM(S3 * stratusQueryMsg)
{
	stratusQueryMsg->CreditPlan = ' ';
	stratusQueryMsg->TerminalIdentification = ' ';
	stratusQueryMsg->FS2 = '0';
	stratusQueryMsg->ForcePostFlag = '0';	/* default value. must be extended to accept other cases */
	stratusQueryMsg->EndOfMessage = '0';

	memset(stratusQueryMsg->SourceTerminal,'a',2);
	memset(stratusQueryMsg->DestinationTerminal,' ',2);
	memset(stratusQueryMsg->CreditFlag,' ',3);
	memset(stratusQueryMsg->SequenceNumber,' ',2);
	memset(stratusQueryMsg->ForcePostAuthCode,' ',6); /* must be modifed to contain the returned value from function GetSeurity() */
	memset(stratusQueryMsg->SystemNumber,'0',4);
	memset(stratusQueryMsg->LocationNumber,'0',4);
	memset(stratusQueryMsg->HotelCode,' ',2);	/* must be modified to contain he returned value from function GetNumHC()*/
	memset(stratusQueryMsg->GuestCheck,'0',6);
	memset(stratusQueryMsg->AmountOfCharges,'0',10);	/* must be modified to match size */
	memset(stratusQueryMsg->TransactionNumber,'0',4);
	memset(stratusQueryMsg->SLDNumber,'0',4);
	memset(stratusQueryMsg->RoomNumber,' ',5);
	memset(stratusQueryMsg->GuestID,' ',2);
	memset(stratusQueryMsg->LineNumber,' ',2);
	memset(stratusQueryMsg->FolioNumber,'0',7);/*must be modified to match size */
	memset(stratusQueryMsg->Filler1,' ',4);
	memset(stratusQueryMsg->CurrencyCode,' ',3);
	memset(stratusQueryMsg->Filler2,' ',7);
	memset(stratusQueryMsg->NumberOfPersons,'0',3);
	memset(stratusQueryMsg->MenuNumber,'0',2);
	memset(stratusQueryMsg->CashierNumber,'0',3);/*servernumber is initialized with this line */
	memset(stratusQueryMsg->ServerNumber,'0',3);
	memset(stratusQueryMsg->ChargeTipAmount,'0',10);
	memset(stratusQueryMsg->TaxAmount,'0',10);
	memset(stratusQueryMsg->Itemizer1,'0',10);
	memset(stratusQueryMsg->Itemizer2,'0',10);
	memset(stratusQueryMsg->Itemizer3,'0',10);
	memset(stratusQueryMsg->Itemizer4,'0',10);
	memset(stratusQueryMsg->DiscountItemizer1,'0',10);
	memset(stratusQueryMsg->DiscountItemizer2,'0',10);

} /* end of function */


/*** Close *****************************************************************/
/*                                                                         */
/*   Fermeture du socket                                                   */
/*                                                                         */
/*                                                                         */
/*                                                                         */
/***************************************************************************/
bool Close()
{
	/* Le socket doit être ouvert */
	if (m_hSocket == INVALID_SOCKET) return FALSE;

	/* Fermer le socket */
	if (::closesocket(m_hSocket) != 0) return FALSE;

	/* Fermeture du socket terminée */
	m_hSocket = INVALID_SOCKET;
	return TRUE;
}



/*** Abort *****************************************************************/
/*                                                                         */
/*   Terminaison brutale de la communication                               */
/*                                                                         */
/*                                                                         */
/*                                                                         */
/***************************************************************************/
bool Abort()
{
	/* Le socket doit être ouvert */
	if (m_hSocket == INVALID_SOCKET) return FALSE;

	/* Demander la fermeture brutale de la connexion */
	LINGER lingerAbort;
	lingerAbort.l_onoff = 1;
	lingerAbort.l_linger = 0;

	/* Fermer le socket */
	if (::closesocket(m_hSocket) != 0) return FALSE;

	/* Fermeture brutale du socket terminée */
	m_hSocket = INVALID_SOCKET;
	return TRUE;
}


/*** GetNextMessageNumber **************************************************/
/*                                                                         */
/*   Récupération du numéro du message transmit                            */
/*                                                                         */
/*                                                                         */
/*                                                                         */
/***************************************************************************/
WORD GetNextMessageNumber()
{
	// Ramener le numéro séquentiel de message entre 0 et (SOCKON2_HEADERNUMBER_MAX - 1)
	m_wMessageNumber %= SOCKON2_HEADERNUMBER_MAX;

	// Retourner un numéro séquentiel de message compris entre 1 et SOCKON2_HEADERNUMBER_MAX
	return ++m_wMessageNumber;
}


/*** BuildHeader ***********************************************************/
/*                                                                         */
/*   Construction de l'entête ON/2                                         */
/*                                                                         */
/*                                                                         */
/*                                                                         */
/***************************************************************************/
bool BuildHeader(	LPSTR lpszHeader,
					char cHeaderType,
					WORD wMessageNumber,
					DWORD dwLenData /*=0*/ )
{
	// Vérifier les paramètres
	if (   (lpszHeader == NULL)
		|| (wMessageNumber > SOCKON2_HEADERNUMBER_MAX)
		|| (dwLenData > (SOCKON2_HEADERLENGTH_MAX - SOCKON2_HEADER_LEN)) )
	{
		return FALSE;
	}

	// Construire l'en-tête
	return (::wsprintfA(	lpszHeader,
							SOCKON2_HEADER_FORMAT,
							cHeaderType,
							wMessageNumber,
							dwLenData + SOCKON2_HEADER_LEN ) == SOCKON2_HEADER_LEN);
};



/*** DecodeHeader **********************************************************/
/*                                                                         */
/*   Décodage de l'entête ON/2 reçue                                       */
/*                                                                         */
/*                                                                         */
/*                                                                         */
/***************************************************************************/
bool DecodeHeader(	LPCSTR lpszHeader,
					char& rcHeaderType,
					WORD& rwMessageNumber,
					DWORD& rdwLenData )
{
	// Vérifier les paramètres
	if (lpszHeader == NULL)
	{
		return FALSE;
	}

	// Décoder le type de l'en-tête
	if (   (lpszHeader[0] != SOCKON2_HEADERTYPE_CONVERSATION)
		&& (lpszHeader[0] != SOCKON2_HEADERTYPE_ACKNOWLEDGE)
		&& (lpszHeader[0] != SOCKON2_HEADERTYPE_KEEPALIVE) )
	{
		return FALSE;
	}
	rcHeaderType = lpszHeader[0];

	// Décoder le numéro séquentiel du message
	if (   (lpszHeader[1] < '0') || (lpszHeader[1] > '9')
		|| (lpszHeader[2] < '0') || (lpszHeader[2] > '9')
		|| (lpszHeader[3] < '0') || (lpszHeader[3] > '9')
		|| (lpszHeader[4] < '0') || (lpszHeader[4] > '9') )
	{
		return FALSE;
	}
	char buffer[50];
	rwMessageNumber	= /*char(itoa(*/(lpszHeader[1] - '0') * 1000
					+ (lpszHeader[2] - '0') * 100
					+ (lpszHeader[3] - '0') * 10
					+ (lpszHeader[4] - '0')/*,buffer,10))*/;

	// Décoder la longueur des données du message
	if (   (lpszHeader[5] < '0') || (lpszHeader[5] > '9')
		|| (lpszHeader[6] < '0') || (lpszHeader[6] > '9')
		|| (lpszHeader[7] < '0') || (lpszHeader[7] > '9')
		|| (lpszHeader[8] < '0') || (lpszHeader[8] > '9') )
	{
		return FALSE;
	}
	rdwLenData	= (lpszHeader[5] - '0') * 1000
				+ (lpszHeader[6] - '0') * 100
				+ (lpszHeader[7] - '0') * 10
				+ (lpszHeader[8] - '0');
	if (rdwLenData < SOCKON2_HEADER_LEN)
	{
		return FALSE;
	}
	rdwLenData -= SOCKON2_HEADER_LEN;

	// Décodage de l'en-tête terminé
	return TRUE;
}


/*** Discard ***************************************************************/
/*                                                                         */
/*   Suppression des données du buffer de réception                        */
/*                                                                         */
/*                                                                         */
/*                                                                         */
/***************************************************************************/
bool Discard(DWORD dwLenData)
{
	// Eliminer les données indésirables en plusieurs passes
	char szDiscard[SOCKON2_DISCARDBUFFER_LEN];
	while (dwLenData != 0)
	{
		// Déterminer la longueur des données à éliminer
		DWORD dwLenToDiscard = dwLenData;
		if (dwLenToDiscard > sizeof(szDiscard))
		{
			dwLenToDiscard = sizeof(szDiscard);
		}

		// Eliminer le bloc courant
		if (!Receive(SOCKON2_TIMEOUT_DISCARD, szDiscard, dwLenToDiscard,1))
		{
			return FALSE;
		}

		// Passer au bloc suivant
		dwLenData -= dwLenToDiscard;
	}

	// Elimination des données indésirables terminée
	return TRUE;
}


/*** Receive ***************************************************************/
/*                                                                         */
/*   Réception de données en provenance du stratus                         */
/*                                                                         */
/*                                                                         */
/*                                                                         */
/***************************************************************************/
bool Receive(DWORD dwTimeout, LPSTR lpData, DWORD dwLenData,int flag)
{

	
	// Vérifier les paramètres
	if (lpData == NULL)
	{
		return FALSE;
	}

	// Attendre le message
	WORD wHeaderNumber;
	DWORD dwHeaderLenData;
	char szHeader[SOCKON2_HEADER_LEN+1];
	
	char cHeaderType;
	while (TRUE)
	{
		/* Recevoir un en-tête */
		/* Décoder l'en-tête reçu */


		if(!StrReceive(SOCKON2_TIMEOUT_ACKNOWLEDGE,lpData, dwLenData))
		{
			return FALSE;
		}

		memcpy(szHeader,lpData,SOCKON2_HEADER_LEN);
		
		
		if (flag==1)
		{
		for(int i=0;i<sizeof(HcRep);i++)
			HcRep[i] = lpData[i]; //SJ0211
		}

		if (flag==0)
		{
		for(int j=0;j<sizeof(HcRepInit);j++)
		            HcRepInit[j] = lpData[j];//	SJ0211
		}
        
		return true;  //SJ0211 On sort ...

		// decodage de l'entete recue 
		if(!DecodeHeader(szHeader, cHeaderType, wHeaderNumber, dwHeaderLenData))
			return FALSE;

	
		/* L'en-tête reçu est-il celui du message attendu ? */
		if (   (cHeaderType == SOCKON2_HEADERTYPE_CONVERSATION)
			&& (dwHeaderLenData+SOCKON2_HEADER_LEN == /*dwLenData*/longueur) )
		{
			break;
		}

		// Eliminer les éventuelles données du message associé au mauvais en-tête reçu
			if ((dwHeaderLenData != 0) && !Discard(dwHeaderLenData))
		{
			return FALSE;
		}
	}

	 //Construire l'en-tête ON/2 d'acquittement du message reçu
	 //Emettre l'acquittement du message reçu 
	char szAck[SOCKON2_HEADER_LEN + 1];
	if (   !BuildHeader(szAck, SOCKON2_HEADERTYPE_ACKNOWLEDGE, wHeaderNumber, SOCKON2_HEADER_LEN)
		|| !StrSend(szAck, SOCKON2_HEADER_LEN) )
	{
		return FALSE;
	}

	if (longueur!= dwLenData) return FALSE;
	// Réception du message terminée
	return TRUE;
}



/*** Send ******************************************************************/
/*                                                                         */
/*   Envoi de données vers les stratus                                     */
/*                                                                         */
/*                                                                         */
/*                                                                         */
/***************************************************************************/
bool Send(LPCSTR lpData, DWORD dwLenData,int flag)
{
	int nbH = 0, nbM = 0;

	// Vérifier les paramètres
	if (lpData == NULL) 
	{
#ifdef _DEBUG
		Pos_display("err : lpData == NULL",20,0);
#endif
		return false;
	}

	// Construire l'en-tête ON/2 spécifique aux communications TCP/IP
		if(!BuildHeader(szHeader, SOCKON2_HEADERTYPE_CONVERSATION, wMessageNumber, dwLenData))
		{
#ifdef _DEBUG
			Pos_display("Send : error while building the header",38,0);
#endif
			return false;
		}

		char envoi[183];
		memset(envoi,'\0',183);
		strcat(envoi,lpData);

		         if(!StrSend(envoi,dwLenData))//SJ0211
		{
#ifdef _DEBUG
			Pos_display("Send : error while sending the message",38,0);
#endif
			return false;
		}

    return true ;    //SJ0211 On sort ...


	// Attendre l'acquittement du message
	while (TRUE)
	{
		// Recevoir un en-tête
		// Décoder l'en-tête reçu
		char szAck[SOCKON2_HEADER_LEN];
		char cAckType;
		WORD wAckNumber;
		DWORD dwAckLenData;
		int nbr = 0;

		if(!StrReceive(SOCKON2_TIMEOUT_ACKNOWLEDGE, szAck, SOCKON2_HEADER_LEN))
		{
			Pos_display("Send : error while receiving the header",39,0);
			return false;
		}
			
		if(!DecodeHeader(szAck, cAckType, wAckNumber, dwAckLenData))
		{
#ifdef _DEBUG
			Pos_display("Send : error while decoding the header",38,0);
#endif
			return false;
		}
		
		// L'en-tête reçu est-il l'acquittement du message ?
		if (   (cAckType == SOCKON2_HEADERTYPE_ACKNOWLEDGE)
			&& (wAckNumber == wMessageNumber)
			&& (dwAckLenData == 0) )
			break;
		
		// Eliminer les éventuelles données du message associé au mauvais en-tête reçu
		if ((dwAckLenData != 0) && !Discard(dwAckLenData))
		{
			return false;
		}
	}/*fin du while*/

	// Emmission du message terminée
	
	
	return true;
}

/*** StrSend ***************************************************************/
/*                                                                         */
/*   envoi des données vers le stratus, couche bas-niveau                  */
/*                                                                         */
/*                                                                         */
/*                                                                         */
/***************************************************************************/

bool StrSend(LPCSTR lpData, DWORD dwLenData)
{
	// Vérifier les paramètres
	if (lpData == NULL)
	{
		Pos_display("StrSend : error lpData == NULL",30,0);
		return FALSE;
	}
	
	// Le socket doit être ouvert
	if (m_hSocket == INVALID_SOCKET)
	{
		Pos_display("StrSend : error socket must be opened",37,0);
		return FALSE;
	}

	// Emettre en plusieurs passes si il y a trop de données
	while (dwLenData != 0)
	{
		// Emettre la portion courante
		int iLenWritten = ::send(m_hSocket, lpData, dwLenData, 0);
		if(iLenWritten != dwLenData)
		{
			// Peut-on continuer ?
			if (::WSAGetLastError() != WSAEWOULDBLOCK)
			{
				return FALSE;
			}

			// Attendre que l'émission soit de nouveau possible
			fd_set fdsWrite;
			FD_ZERO(&fdsWrite);
			FD_SET(m_hSocket, &fdsWrite);
			timeval tvTimeout;
			tvTimeout.tv_sec  = 1;
			tvTimeout.tv_usec = 0;
			if (::select(0, NULL, &fdsWrite, NULL, &tvTimeout) != 1)
			{
				return FALSE;
			}
			continue;
		}

		// Passer à la portion suivante
		lpData += iLenWritten;
		dwLenData -= iLenWritten;
	}

	// Emission des données terminée

	return TRUE;
}

/*** StrReceive ************************************************************/
/*                                                                         */
/*   Réception des données du stratus, couche bas-niveau                   */
/*                                                                         */
/*                                                                         */
/*                                                                         */
/***************************************************************************/
bool StrReceive(DWORD dwTimeout, LPSTR lpData, DWORD dwLenData)
{
	// Vérifier les paramètres
	if (lpData == NULL)
	{
		Pos_display("StrReceive : error lpData == NULL",33,0);
		return FALSE;
	}

	// Le socket doit être ouvert
	if (m_hSocket == INVALID_SOCKET)
	{
		Pos_display("StrReceive : error socket must be opened",40,0);
		return FALSE;
	}

	longueur=0;
	DWORD argp;
	// Lire les données en plusieurs passes si elles ne sont pas immédiatement disponibles

	Sleep(500); //On commence par temporiser (le process de Philippe est Corse !!)
	while (dwLenData != 0)
	{
		// Lire la portion courante
		int iLenRead = ::recv(m_hSocket, lpData, dwLenData,0);
		if (iLenRead == 0)
		{
			// La connection a été terminée (proprement) par la machine distante !
			return FALSE;
		}

		
		if (iLenRead == -1) break;

 		if (iLenRead == SOCKET_ERROR)
		{
			// Peut-on continuer ?
			int numerr;
			if (( numerr = ::WSAGetLastError()) != WSAEWOULDBLOCK)
			{
				return FALSE;
			}

			// Attendre que des données soient de nouveau disponibles
			fd_set fdsRead;
			FD_ZERO(&fdsRead);
			FD_SET(m_hSocket, &fdsRead);
			timeval tvTimeout;
			tvTimeout.tv_sec  = dwTimeout / 1000;
			tvTimeout.tv_usec = (dwTimeout % 1000) * 1000;

			if (::select(0, &fdsRead, NULL, NULL, &tvTimeout)!= 1)
			{
				Pos_display("StrReceive : error no incoming data",35,0);
				return FALSE;
			}
			continue;
		}

		// Passer à la portion suivante
		lpData += iLenRead;
		dwLenData -= iLenRead;

		longueur+=iLenRead;

		int i=::ioctlsocket(m_hSocket,SIOCATMARK,&argp);
		if ((i==0) && (!argp)) break;

	}

	// Lecture des données terminée

	return TRUE;
}






BOOL CheckContext(S1* hpms, MsgToStratus s)
///////////////////////////////////////////////////////////////////////////////////////////
//   CheckContext : Regarde si les fichier contextuel HC existe
//                  OUI : ne demande pas l'init
//                  NON : demande d'initialisation
//                        Init correct : NO TAF
//                        Init échoue  : les fichiers contextuels partiels
//                                       du terminal sont effacés.
//  SJU, 02/08/2001.
///////////////////////////////////////////////////////////////////////////////////////////
{
char InitMsg[10];
char recv_init[73];

	FILE *fp ;
	FILE *fp1 ;
	char line [18],  szBuf[50];
	int  ret ;
	char hostName [13];
	char * szhostName ;
	char Port[5];
	int short hostPort ;
	LPSTR	wRemotePort,
			lpszRemoteAddress;
	char buff[50];

	WIN32_FIND_DATA FileData1;   
	WIN32_FIND_DATA FileData2;  
 
	HANDLE hSearch1; 
	HANDLE hSearch2;
	BOOL fFinished = FALSE; 

	static char   sztmp[4], szGen[16], szHcCfg[16], szSocket[5];


    printf ("Recherche contextuelle \n");
	memset (szGen,'\0',sizeof(szGen)); 
    memset (szHcCfg,'\0',sizeof(szHcCfg));
	memset (szSocket,'\0',sizeof(szSocket));
	
    sprintf (szGen, "%s.", "./HcCtxDlp");
	sprintf (szHcCfg, "%s", "./HcConfig");
	//Sup 4.6 strncat (szGen, hpms->aTermNum, 3);
	

	//Test présence du fichier "HcCtxDlp.xxx"
	hSearch1 = FindFirstFile(szGen, &FileData1); 
    if (hSearch1 != INVALID_HANDLE_VALUE) return TRUE;

	if (hSearch1 == INVALID_HANDLE_VALUE)
	{
		Pos_display ("Defaut de contexte, demande auto init",37,0);
			
		// Test présence du fichier "HcConfig"                        
		hSearch2 = FindFirstFile(szHcCfg, &FileData2); 
		if (hSearch2 == INVALID_HANDLE_VALUE) 
		{
		fFinished=TRUE;
		return FALSE;
		}
		

	
		memset(szBuf, '\0', sizeof(szBuf));
	    
		ret = -1 ;


		// Initialiser la DLL WinSock lors de la première ouverture d'un socket
			if (!m_bStartedUp)
			{	
				WORD wVersionRequested = MAKEWORD(1,1);
				WSADATA wsaData;
				if(::WSAStartup(wVersionRequested, &wsaData) == 0)
				{
					// Vérifier la version supportée par la DLL WinSock
					if (wsaData.wVersion == wVersionRequested) m_bStartedUp = TRUE;
				}
				else ::WSACleanup();
			}

			if (!m_bStartedUp) return FALSE;

	
			/* Le socket ne doit pas être déjà ouvert */
		if (m_hSocket == INVALID_SOCKET)
		{
			Close();
		}

	    GetStratusCom(szHcCfg);
        
	  
        //memcpy  ( & ( addrRemote.sin_addr) ,nHost->h_addr, sizeof addrRemote.sin_addr );
		wRemotePort=ConfigSocket; 
		//lpszRemoteAddress=hostName;
		
		
		FindClose(hSearch1);
		FindClose(hSearch2);
		
		//Préparation de la structure du message init à envoyer pour le status
        memset (&s, 0x20, sizeof s);
		
		//Renseignement de la structure de la demande d'initialisation
		s.sTypeOperation[0]='A';
		s.sType_init[0]='H';
		strncpy ( s.sTermNum, hpms->aTermNum, sizeof(s.sTermNum) );
        s.sRegister[0] = hpms->aRegister;
	    strncpy ( s.sCtrlNum,   hpms->aCtrlNum, sizeof(s.sCtrlNum) );
		
		memset(InitMsg,'\0',sizeof(InitMsg));
		memcpy (InitMsg,&s,sizeof(InitMsg));
		
		//ouverture du socket
		SOCKADDR_IN addrRemote;
		struct   hostent  FAR *nHost;

		memset(&addrRemote, 0, sizeof(addrRemote));
        if ( (nHost = gethostbyname( ConfigHost) ) == 0 ) return FALSE ;
        memcpy  ( & ( addrRemote.sin_addr) ,nHost->h_addr, sizeof addrRemote.sin_addr );
		addrRemote.sin_family = AF_INET;
		addrRemote.sin_port = ::htons(atoi(wRemotePort));


		/* Ouvrir la socket */
		if ((m_hSocket = ::socket(PF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET)
		{
			Close();
			return FALSE;

		}

		/* Connecter le socket à la machine distante */
		if (::connect(m_hSocket, (LPSOCKADDR)&addrRemote, sizeof(addrRemote)) != 0)
		{
			::closesocket(m_hSocket);
			m_hSocket = INVALID_SOCKET;
			return FALSE;
		
		}
		
		/* Passer le socket en mode non-bloquant */
		if (::ioctlsocket(m_hSocket, FIONBIO, &dwNonBlockingMode) != 0)
		{
			::closesocket(m_hSocket);
			m_hSocket = INVALID_SOCKET;
			return FALSE;
			
		}

		// envoi du message init pour le stratus 
		if(!Send(InitMsg,sizeof(InitMsg),0))
		{
			Abort();
			return FALSE;
		}
		
		memset(HcRepInit,'\0',sizeof(HcRepInit));

		//réception de la réponse
		if(!Receive(dwTimeout, recv_init, sizeof(HcRepInit)+SOCKON2_HEADER_LEN,0))
		{
			Abort();
			return FALSE;
		} 
		

		struct MsgFromStratus  rs_init;
		
		memset (&rs_init, 0x20, sizeof rs_init);
		memcpy((char*)&rs_init,HcRepInit,sizeof(HcRepInit));

		
		CreateHcCtxDlp(szHcCfg, szGen, rs_init);

	}
    Close();
	return TRUE;
}



int IsContextExist (char *CTX_NAME)

{
	int fd;

	if ((fd = _open (CTX_NAME, _O_RDWR | _O_BINARY)) < 0)
		return FALSE ;
	_close (fd);
	return TRUE;
}


void CreateHcCtxDlp(char *fConfig, char *fNewConfig, MsgFromStratus Msg)
{
 FILE *fp, *fpNew ;
 char line [128], lineNew[128], szBuf[50], Cible[12], HcCtxDlp[16];
 int  lg, ret ;
 char  devTcp [32], hostName [32];
 int short hostPort ;

      Pos_display("Création du contexte",20,0);
      memset(Cible, '\0', sizeof(Cible));
	  memset(szBuf, '\0', sizeof(szBuf));

      strncpy (Cible, "STRATUS_COM",11);
	  	  
      lg= strlen(Cible);
      ret = -1 ;

      if( ! (fp = fopen(fConfig, "r" )))
         return  ;

      if( ! (fpNew = fopen(fNewConfig, "w" )))
         return  ;

    Pos_display("Ouverture des fichiers",22,0);

    while ( fgets (line , sizeof line , fp) ) 
	{
      memset(lineNew,'\0', sizeof(lineNew));
      if (!memcmp(Cible ,line , lg) )  
	  {
		/* line   ->  Interface:hostName:hostPort */
        Pos_display("Récupération  de la socket",26,0);
        strcpy(szBuf, line+lg+1);
        ret = sscanf(szBuf ,"%s %s %d", devTcp,hostName, & hostPort);
        sprintf (lineNew, "STRATUS_COM=%s %s %4.4s \n", devTcp, hostName, Msg.riSocket ); 
		Pos_display(lineNew,strlen(lineNew),0);
	  }
      else
	  {
        strncpy(lineNew, line, strlen(line));
	  }

	  fputs(lineNew, fpNew);
	}
     sprintf (lineNew, "AUTORISATION=%1.1s \n",  Msg.riTypeAnnulation ); 
     fputs(lineNew, fpNew);
     sprintf (lineNew, "FORCAGE=%1.1s \n",  Msg.riForcage ); 
     fputs(lineNew, fpNew);

     fclose(fp);
     fclose(fpNew);
     Pos_display("Contexte créé",13,0);
     return ;
}

bool GetStratusCom(char *fConfig)
{
 FILE *fp;
 char line [128], szBuf[50], Cible[12];
 char devTcp [32];
 int  lg, ret;
 

      memset(Cible, '\0', sizeof(Cible));
	  memset(szBuf, '\0', sizeof(szBuf));
	  
      strncpy (Cible, "STRATUS_COM",11);
	  
      lg= strlen(Cible);
      

      if( ! (fp = fopen(fConfig, "r" )))
         return  FALSE;

     while ( fgets (line , sizeof line , fp) ) 
	{
      if (!memcmp(Cible ,line , lg) )  
	  {
		/* line   ->  Interface:hostName:hostPort */
        strcpy(szBuf, line+lg+1);
		ret = sscanf(szBuf ,"%s %s %s", devTcp,ConfigHost, ConfigSocket);
        break;
	  }
	}	 

     fclose(fp);
    
     return TRUE;

}


}