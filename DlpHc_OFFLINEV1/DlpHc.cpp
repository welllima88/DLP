// Dlp_Hc.cpp : Defines the entry point for the DLL application.

/*--------------------------------------------------------------------------------------------------------
H I S T O R I Q U E   D E S   M O D I F I C A T I O N S
--------------------------------------------------------------------------------------------------------
:  Version :   Date   :Auteur  :                          Commentaires                                           :
--------------------------------------------------------------------------------------------------------
:   4.3    : 05/01/07 :   SJU  :  Gestion du Forcage (bForce) et de l'appel phonie réception (bAuto)
:          : 08/01/07 :   SJU  :  Modification de la structure aller du msg d'init
:   4.4    : 07/03/07 :   SJU  :  Dlp_Hc_Revelation, deuxième point d'entrée pour les messages de 2TPOS             
:   4.5    : 26/06/07 :   SJU  :  Dlp_Hc_Revelation, Tous les terminaux sont renseignés à 01
:   4.6    : 03/07/07 :   SJU  :  Implémentation du mode offline, actif si bforce == 2
:   4.7    : 20/11/07 :   SJU  :  Dlp_Hc_Revelation, Tous les terminaux sont renseignés avec le register
:   4.8    : 02/06/08 :   SJU  :  Ajouter Point d'entrée EncoDeco pour cryptage / décryptage
:   4.9    : 07/08/15     NM   :  Ajout du C3
:----------:----------:--------:------------------------------------------------------------------------
--------------------------------------------------------------------------------------------------------*/
#include <memory.h>
#include "apiDLPc3.h"
#include "stdafx.h"
#include <fcntl.h>
#include <process.h>
#include <io.h>


//#include "afx.h"

LPVOID  CreateMappingFile();
void    DestroyMappingFile(LPVOID MPtr); 

#pragma warning(disable : 4800)
#pragma warning(disable : 4018)
#pragma warning(disable : 4550)
#pragma warning(disable : 4101)
#pragma warning(disable : 4700)

//#define DLPCCDLL_API __declspec(dllexport) //Add, pour test Gateway
//#define DLPCCDLL_API __cdecl(dllexport) //Add, pour Gateway issue 19061

//DllExport void  __stdcall Dlp_Hc(S1 &,
extern "C" DllExport void  __cdecl Dlp_Hc(S1 &,
										  S2 &,
										  int (*pGetSecurity)(void),
										  int (*pGetKey)(void),
										  int (*pGetStringFromKeyboard)(char *, int, char *),
										  void(*pPos_display)(char *, int, short));

// Ajout 4.4
extern "C" DllExport void  __cdecl Dlp_Hc_Revelation(S5 &,
													 S2 &,
													 int (*pGetSecurity)(void),
													 int (*pGetKey)(void),
													 int (*pGetStringFromKeyboard)(char *, int, char *),
													 void(*pPos_display)(char *, int, short));

// Ajout 4.8, 
extern "C" DllExport void  __cdecl EncoDeco(S6);

extern "C" DllExport bool GetHCIDFromTPERevelation(S5* msgsend, int ordre,  void (*pPos_display)(char *, int, short));
extern "C" DllExport bool GetHCIDFromTPE(S1* msgsend, int ordre,  void (*pPos_display)(char *, int, short));




/*** DLP_GetSecurity *******************************************************/
/*                                                                         */
/*   Gestion de la clef superviseur                                        */
/*                                                                         */
/*                                                                         */
/*                                                                         */
/***************************************************************************/
int DLP_GetSecurity()
{ 
	return true;
}

/*** DLP_GetKey ************************************************************/
/*                                                                         */
/*   Gestion du clavier                                                    */
/*                                                                         */
/*                                                                         */
/*                                                                         */
/***************************************************************************/
int DLP_GetKey() 
{
	HANDLE MHdle;
	LPVOID MPtr;
	int Ret;

	//Ret = PostMessage(FindWindow(NULL,"Monetique"), WM_USER+2, 0, 0); //Active les pushBtn de choix.
	Ret = SendMessage(FindWindow(NULL,"Monetique"), WM_USER+2, 0, 0); //Active les pushBtn de choix.

	MHdle = OpenFileMapping(FILE_MAP_READ | FILE_MAP_WRITE, TRUE, "Fic_Map");
	MPtr = MapViewOfFile(MHdle, FILE_MAP_WRITE | FILE_MAP_READ,0, 0, 128);

	if (MPtr == NULL || MHdle == NULL) return 0x00; 

	if (memcmp("CANCEL",(char *)MPtr,6) == 0)
	{
		SendMessage (FindWindow(NULL,"Monetique"), WM_USER+4, 0L, 0L);
		sprintf ((char *) MPtr, "      \0");
		DestroyMappingFile(MPtr);
		return 0x41;
	}
	if (memcmp("ACCEPT",(char *)MPtr,6) == 0)
	{
		SendMessage (FindWindow(NULL,"Monetique"), WM_USER+4, 0L, 0L);
		sprintf ((char *) MPtr, "      \0");
		DestroyMappingFile(MPtr);
		return 13;
	}

	DestroyMappingFile(MPtr);
	Sleep (1);
	return 0x00;

}

/////////////////////////////////////////////////////////////////////////////
//    Mapping mémoire                                                      //
/////////////////////////////////////////////////////////////////////////////
LPVOID CreateMappingFile() 
{
	HANDLE MHdle;

	MHdle = CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE,0,128, "Fic_Map");

	if (MHdle == NULL) return NULL;

	return MapViewOfFile (MHdle, FILE_MAP_WRITE | FILE_MAP_READ, 0, 0, 128);

}


/////////////////////////////////////////////////////////////////////////////
//    Destruction Mapping mémoire                                          //
/////////////////////////////////////////////////////////////////////////////
void DestroyMappingFile(LPVOID MPtr) 
{
	HANDLE MHdle;

	MHdle = OpenFileMapping(FILE_MAP_READ | FILE_MAP_WRITE, TRUE, "Fic_Map");
	//sprintf ((char *) MPtr, "      \0");
	if (MPtr)
	{
		UnmapViewOfFile(MPtr);
		MPtr = NULL;
	}

	if (MHdle )
	{
		CloseHandle(MHdle);
		MHdle = NULL;
	}

}



/*** DLP_GetStringFromKeyboard *********************************************/
/*                                                                         */
/*   Lecture d'une string sur le clavier                                   */
/*                                                                         */
/*                                                                         */
/*                                                                         */
/***************************************************************************/
int DLP_GetStringFromKeyboard (char *stringEntered, int MaxlentOfStringEntered, char *messageToDisplay)
{
	HANDLE MHdle;
	LPVOID MPtr;
	char szRep[255], szCode[255];
	int Ret;

	DLP_Pos_display(messageToDisplay, MaxlentOfStringEntered, 0);
	memset (szRep, '\0', 255);
	memset (szCode, '\0', 255);
	SendMessage (FindWindow(NULL,"Monetique"), WM_USER+3, 0L, 0L);

	MHdle = OpenFileMapping(FILE_MAP_READ | FILE_MAP_WRITE, TRUE, "Fic_Map");
	MPtr = MapViewOfFile(MHdle, FILE_MAP_WRITE | FILE_MAP_READ,0, 0, 128);

	while (TRUE)
	{
		if (memcmp("CANCEL",(char *)MPtr,6) == 0)
		{
			SendMessage (FindWindow(NULL,"Monetique"), WM_USER+4, 0L, 0L);
			sprintf ((char *) MPtr, "      \0");
			DestroyMappingFile(MPtr);
			return 0x41;
		}
		if (memcmp("ACCEPT",(char *)MPtr,6) == 0)
		{
			Ret = sscanf((char *) MPtr,"%s %s", szRep, szCode);
			sprintf(stringEntered, szCode); 
			SendMessage (FindWindow(NULL,"Monetique"), WM_USER+4, 0L, 0L);
			sprintf ((char *) MPtr, "      \0");
			DestroyMappingFile(MPtr);
			return 0x12;
		}
	}

	return 0x41;
}

/*** DLP_Pos_display *******************************************************/
/*                                                                         */
/*   Affichage des messages de caisse                                      */
/*                                                                         */
/*                                                                         */
/*                                                                         */
/***************************************************************************/
void DLP_Pos_display (char * messageToDisplay, int lentOfMessageToDisplay, short confirm)

{
	LPVOID Mptr;

	lentOfMessageToDisplay = 0; //Pour éviter warning C4100 à la compil.

	if (Mptr = CreateMappingFile() )
	{

		memset((char *) Mptr, '\0', sizeof (Mptr));	

		strcpy ((char *) Mptr, messageToDisplay);

		SendMessage (FindWindow(NULL,"Monetique"), WM_USER+1, 0L, 0L);
		sprintf ((char *) Mptr, "      \0");
		DestroyMappingFile(Mptr);

		switch(confirm)
		{
		case WAIT_KEY:
			//Ret = MessageBox(NULL,(char *) Mptr,"Confirmer",MB_OK);
			break;

		case WAIT_1_SEC :
			//case WAIT_KEY   :
			Sleep(1000);
			break;
		}
	}
}

/*** DLP_PrintTicket *******************************************************/
/*                                                                         */
/*   Imprime le ticket                                                     */
/*                                                                         */
/*                                                                         */
/*                                                                         */
/***************************************************************************/

void DLP_PrintTicket (char * FileToPrint)
{
	return;
}

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
char *  GetParameter(LPTSTR);
void    CreateHcCtxDlp(char *fConfig, char *fNewConfig, MsgFromStratus Msg);
bool    GetStratusCom(char *fConfig);
bool    SetHcAuthorizationRevelation(S5 *, S3 *);
bool    AddTrnInFileOffLine(LPCSTR Msg);
void    GestionDuForcage(S3 *);
bool	CheckKeyDlpHC(LPSTR);

/*-----------------------------------------------------------*/
/*     création de la socket et des variables utilisées      */
/*-----------------------------------------------------------*/
static SOCKET	m_hSocket;
static WORD		m_wMessageNumber;
static HWND     hMonetic = NULL;
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
bool			m_bStartedUp, bAuto, bForce;
char			szHeader[SOCKON2_HEADER_LEN + 1],
	HcMsg[174],
	HcRep[73],
	HcRepInit[67],
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
//DllExport void __stdcall Dlp_Hc(S1 &HcPosMsgSend,
extern "C" DllExport void __cdecl Dlp_Hc(S1 &HcPosMsgSend,
										 S2 &HcPosMsgRecv,
										 int (*pGetSecurity)(void),
										 int (*pGetKey)(void),
										 int (*pGetStringFromKeyboard)(char *, int, char *),
										 void(*pPos_display)(char *, int, short))

										 //Sup et remplace : Gateway issue 19061
										 //extern "C" DLPCCDLL_API void Dlp_Hc(S1 &HcPosMsgSend,  //Add, Pour test Gateway
										 /*extern DLPCCDLL_API void Dlp_Hc(S1 &HcPosMsgSend,  //Add, Pour test Gateway
										 S2 &HcPosMsgRecv,
										 int (*pGetSecurity)(void),
										 int (*pGetKey)(void),
										 int (*pGetStringFromKeyboard)(char *, int, char *),
										 void(*pPos_display)(char *, int, short))
										 */
{
	S1 * hpms = &HcPosMsgSend;
	hpmr = &HcPosMsgRecv;
	FILE *fp ;
	lpCcPosInitSend msg; // test C3 NM
	char line [18], hostName [13];
	char Port[5];
	char *args[2], Prog[80];
	char   szPan[20], szMsg[255]; 
	char *limit;
	int Ret, getkey;
	static char   sztmp[4]; 
	char Msg[255], szMontantE[11], szMontantD[3];
	char szC3Cfg[32]; // test C3 NM
	bool dlphcc3 = false;

	S3 * stratusQueryMsg = new S3;
	S4 * stratusReplyMsg = new S4;

	struct MsgToStratus s;

	static char   szGen[16];

	memset (szC3Cfg,'\0',sizeof(szC3Cfg)); // test C3 NM
	sprintf (szC3Cfg, "%s.", "c3Config"); // test C3 NM

	memset(&msg, 0x00, sizeof(msg)); // test C3 NM
	memset(HcMsg,'\0',sizeof(HcMsg));
	memset(HcRep,'\0',sizeof(HcRep));
	memset(ConfigSocket,'\0',sizeof(ConfigSocket));
	memset(ConfigHost,'\0',sizeof(ConfigHost));

	//*pGetSecurity = NULL;
	//*pGetKey = NULL;
	//*pGetStringFromKeyboard = NULL; 
	//*pPos_display = NULL;

	if (IsForceMonetic(szC3Cfg)){
		pGetSecurity = NULL;
		pGetKey = NULL;
		pGetStringFromKeyboard = NULL; 
		pPos_display = NULL;
	}

	if (*pPos_display == NULL)
	{

		hMonetic = FindWindow(NULL,"Monetique");
		if (hMonetic == NULL)
		{
			memset(Prog,'\0',sizeof(Prog));

			memcpy(Prog,"Monetic.exe",11);
			args[0] = "Monetic.exe";
			args[1] = NULL;
			_spawnv(_P_NOWAIT, Prog, args);  //Ouvre l'application Monétique.
		}
		else
		{
			Ret = PostMessage(FindWindow(NULL,"Monetique"), WM_USER+8, 0, 0); //Init de la fenêtre déjà existante.
		}

		while (hMonetic == NULL) //Pour attendre que l'appli soit chargée
		{
			hMonetic = FindWindow(NULL,"Monetique");
		}

		memset(Msg,0x00,sizeof(Msg));
		memset(szPan,0x00,sizeof(szPan));
		memset(szMontantD,0x00,sizeof(szMontantD));
		memset(szMontantE,0x00,sizeof(szMontantE));
		memset(&s, 0x00, sizeof(s));


		strncpy (szMontantE, hpms->aAmount, sizeof(hpms->aAmount)-2);
		strncpy (szMontantD, &hpms->aAmount[sizeof(hpms->aAmount)-2], 2);

		// Pour tester les fonctionnalitées du C3 
		// Cas SMED appel de C3 pour récuperer le numero HC
		// effectuer la condtion sur la variable customerpresent



		sprintf (Msg, "N° Hotel Charge : %s  -  Montant : %d,%s \0", szPan, atoi(szMontantE), szMontantD);
		DLP_Pos_display(Msg,strlen(Msg),0);

		//sprintf (Msg, "Montant : %d,%s \0", atoi(szMontantE), szMontantD);
		//DLP_Pos_display(Msg,0,0);

		Ret = PostMessage(FindWindow(NULL,"Monetique"), WM_USER+5, 0, 0); //Donne l'état en cours.
	}


	/* test des paramètres d'entrée dans la DLL */
	if	(*pGetSecurity == NULL) GetSecurity = DLP_GetSecurity;
	else GetSecurity = *pGetSecurity;
	if	(*pGetKey == NULL) GetKey = DLP_GetKey;
	else GetKey = *pGetKey;
	if	(*pGetStringFromKeyboard == NULL) GetStringFromKeyboard = DLP_GetStringFromKeyboard;
	else GetStringFromKeyboard = *pGetStringFromKeyboard;
	if	(*pPos_display == NULL) Pos_display = DLP_Pos_display;
	else Pos_display = *pPos_display;

	fflush(0);
	Pos_display("Hotel Charge v6.1 - 27 janvier 2016",35,0);

	if (!(CheckContext(hpms,s))) 

	{
		setResponse('1',"Erreur récupération du port à partir des fichiers de configuration",hpmr);	
		CloseMonetic(0);
		Close(); //5.2
		Pos_display("Erreur socket dans config",25,0);
		return;
	}	

	if( strcmp(hpms->aChargeNum, "000000000") == 0  ){
		hpms->aCustomerPresent = 'P';
		dlphcc3 = GetHCIDFromTPE(hpms, 'z', Pos_display);
		if (dlphcc3 == FALSE){
			return;
		}
		hpms->aCustomerPresent = '1';

	}


	//strncpy (szPan, hpms->aChargeNum, sizeof(hpms->aChargeNum));

	memset (szGen,'\0',sizeof(szGen)); 
	sprintf (szGen, "%s.", "./HcCtxDlp");
	//strncat (szGen, hpms->aTermNum, 3);


	/* récupération des paramètres de connexion dans le registre */
	/*------------------------------------------------*/
	/*wRemotePort = GetParameter(SERVER_PORT);		  */
	/*lpszRemoteAddress = GetParameter(SERVER_IP);	  */
	/*------------------------------------------------*/


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
				setResponse('1',"Erreur d'initialisation du socket",hpmr);
				delete stratusQueryMsg;
				delete stratusReplyMsg;
				fflush(0);
				Ret = PostMessage(hMonetic, WM_USER+7, 0, 0); //Traitement refusé.
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
			CloseMonetic(0);
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
			Close(); //SJ050606
			CloseMonetic(0);
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
			CloseMonetic(0);
			Close(); //SJ070307
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
			CloseMonetic(0);
			Close(); //SJ070307
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
			CloseMonetic(0);
			Close(); //SJ070307
			return;
			//exit(666);
		}
		else 
		{
			memcpy(stratusReplyMsg,HcRep,sizeof(HcRep));
#ifdef _DEBUG
			//Pos_display(&stratusReplyMsg->rCreditPlan,0,0);
#endif

			switch(stratusReplyMsg->rStatusCode)
			{
			case '0':	/* transaction acceptée */

				Pos_display("transaction acceptee...",23,0);
				setResponse(stratusReplyMsg->rStatusCode,(char*) stratusReplyMsg->rResponseMessageText,hpmr);
				break;

			case '1':	/* transaction refusée et non forcable */

				Pos_display("transaction refusee...",22,0);

				setResponse(stratusReplyMsg->rStatusCode,(char*) stratusReplyMsg->rResponseMessageText,hpmr);
				CloseMonetic(0);
				delete stratusQueryMsg;
				delete stratusReplyMsg;
				fflush(0);
				Close(); //SJ050606
				return;
				break;			

			case '2' :	/* transaction refusée mais forcable */

				//Pos_display("transaction refusee. Forcage...",31,0);

				if (bAuto)
				{
					char m_numauto[6];
					int k;
					char szTmp[255];

					memset(szTmp, 0x00, sizeof(szTmp));
					memcpy(szTmp, stratusReplyMsg->rResponseMessageText,sizeof(stratusReplyMsg->rResponseMessageText));
					Pos_display(szTmp,strlen(szTmp),0);

					switch(GetStringFromKeyboard(m_numauto,sizeof(m_numauto),"saisie du code d'autorisation : "))
					{
					case '\x41':	/* transaction abandonnée */
						Pos_display("transaction abandonnee",22,0);
						setResponse('1',"Trans. abandonnee",hpmr);
						CloseMonetic(0);
						delete stratusQueryMsg;
						delete stratusReplyMsg;
						fflush(0);
						Close(); 
						return;
						break;

					case '\x12':	/* transaction acceptée */

						for(k=0;k<sizeof(stratusQueryMsg->ForcePostAuthCode);k++)

							if (isdigit(m_numauto[k]))
								stratusQueryMsg->ForcePostAuthCode[k] = m_numauto[k];
							else
								stratusQueryMsg->ForcePostAuthCode[k] = 0x20;


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
							CloseMonetic(0);
							Close(); //SJ070307
							return;		
						}

						Pos_display("Forçage enregistré.",23,0);

						setResponse('0',"Trans. acceptee",hpmr);
						break;

					default :		/* erreur dans tout autre cas */
						Pos_display("erreur code autorisation",24,0);
						setResponse('1',"Trans. refusee",hpmr);
						break;
					} //end switch
				}
				else // (!bAuto)
				{
					Pos_display("Transaction refusée",19,0);
					// setResponse('1',"Trans. refusee",hpmr);
					setResponse(stratusReplyMsg->rStatusCode,(char*) stratusReplyMsg->rResponseMessageText,hpmr);
				}
				//}
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
				CloseMonetic(0);
				Close(); //5.2
				return;
				//exit(667);
				break;
			}

			delete stratusQueryMsg;
			delete stratusReplyMsg;
			fflush(0);
			CloseMonetic(1);
			Close();
		}
	}
	else
	{	
		delete stratusQueryMsg;
		delete stratusReplyMsg;
		fflush(0);
		Pos_display("Transaction refusée... Mauvais paramètres caisse",49,0);
		CloseMonetic(0);
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
	//strcpy(hpmr->bExplanation,e);
	memcpy(hpmr->bExplanation, e, sizeof(hpmr->bExplanation)-1);
	//printf("sortie du programme \n");
	//Modification OTHMAN le 04/10/01
	//exit(1);
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

	numTest=1;

	do{
		switch(numTest)
		{
		case 1:
			//Pos_display("1",1,2);
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
			//Pos_display("2",1,2);
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
			//Pos_display("3",1,2);
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
			//Pos_display("4",1,2);
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
			//Pos_display("5",1,2);
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
			//Pos_display("6",1,2);
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
			//Pos_display("7",1,2);
			if( isdigit(hpms->aCashNum[0]) && hpms->aCashNum[0] =='0' &&
				isdigit(hpms->aCashNum[1]) &&
				isdigit(hpms->aCashNum[2]) &&
				isdigit(hpms->aCashNum[3]) &&
				isdigit(hpms->aCashNum[4]) &&
				isdigit(hpms->aCashNum[5]) &&
				isdigit(hpms->aCashNum[6]) ) numTest++;
			else
			{
				printf("saisie incorrecte du numero de caissier \n");
				setResponse('1',"aca Error", hpmr);
				Pos_display("saisie incorrecte du numero de caissier",39,2);
				numTest = 66;			
			}
			break;
		case 8:
			//Pos_display("8",1,2);
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
			//Pos_display("9",1,2);
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
			//Pos_display("10",2,2);
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
			//Pos_display("11",2,2);
			if( isdigit(hpms->aChargeNum[2]) &&
				isdigit(hpms->aChargeNum[3]) &&
				isdigit(hpms->aChargeNum[4]) &&
				isdigit(hpms->aChargeNum[5]) &&
				isdigit(hpms->aChargeNum[6]) &&
				isdigit(hpms->aChargeNum[7]) &&
				isdigit(hpms->aChargeNum[8]) ) numTest = 12;  //&&
			//!isalnum(hpms->aChargeNum[9])) numTest = 12;
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
			//Pos_display("break",5,2);
			break;
		}
	}while(numTest!=66 && numTest != 12);

	/*création d'une réponse positive*/
	if(numTest == 12) 
	{
		//Pos_display("Message Caisse analysé : OK",27,2);
		return (true);;
	}
	else 
	{
		/*char tmp[3];
		itoa(numTest,tmp,3);
		setResponse('1',tmp,hpmr);*/
		Pos_display("Message Caisse analysé : K0",27,2);
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
	//SJ if(::setsockopt( m_hSocket,SOL_SOCKET,SO_LINGER,(LPCSTR)&lingerAbort,sizeof(lingerAbort) ) != 0) return FALSE;

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

		//DWORD dwHeaderLenData;

		if(!StrReceive(SOCKON2_TIMEOUT_ACKNOWLEDGE,lpData, dwLenData))
		{
			return FALSE;
		}

		memcpy(szHeader,lpData,SOCKON2_HEADER_LEN);


		if (flag==1)
		{
			for(int i=0;i<sizeof(HcRep);i++)
				//	SJ0211	HcRep[i] = lpData[i+SOCKON2_HEADER_LEN];
					HcRep[i] = lpData[i]; //SJ0211
		}

		if (flag==0)
		{
			for(int j=0;j<sizeof(HcRepInit);j++)
				//	SJ0211	HcRepInit[j] = lpData[j+SOCKON2_HEADER_LEN];
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



bool SendA(LPCSTR lpData, DWORD dwLenData,int flag)
{
	// Vérifier les paramètres
	if (lpData == NULL) 
	{
		DLP_Pos_display("err sur Send : lpData == NULL",0,0);
		return false;
	}


	char envoi[183];
	memset(envoi,'\0',183);

	strcat(envoi,lpData);

	// Emettre les données du message
	if(!StrSend(envoi,dwLenData))
	{
		DLP_Pos_display("Send : erreur pendant l'envoi du message",0,0);

		return false;
	}



	return true;
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
	//if (flag==1) strcat(envoi,szHeader);	//auto
	// //SJ0211  strcat(envoi,szHeader);
	strcat(envoi,lpData);

	// Emettre les données du message
	//SJ0211 if(!StrSend(envoi,dwLenData+SOCKON2_HEADER_LEN))
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


		if (iLenRead == -1  && longueur !=0) break;

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




/*** GetParameter **********************************************************/
/*                                                                         */
/*   Récupération des paramètres stockés dans la base de registre:		   */
/*	 Deux valeurs possibles pour le paramètre                              */
/*		- ServerIP :   adresse IP du serveur monétique                     */
/*      - ServerPort : numéro du port auquel se connecter sur le serveur   */
/*                                                                         */
/***************************************************************************/
char * GetParameter(LPTSTR lpSubKey)
{
	static HKEY phkResult;		// handle to key to be opened
	HKEY hKey;           // handle to key to query
	LPTSTR lpValueName;  // address of name of value to query
	LPDWORD lpReserved;  // reserved
	LPDWORD lpType;      // address of buffer for value type
	LPBYTE lpData;       // address of data buffer
	LPDWORD lpcbData;    // address of data buffer size

	unsigned long ult = REG_SZ;

	/* init */
	lpValueName = lpSubKey;
	lpReserved = NULL;
	lpType = &ult;//REG_SZ;

	/* vérification de la valeur du paramètre */
	if(lpSubKey!=SERVER_IP && lpSubKey!=SERVER_PORT)
	{
		/* code a rajouter en cas d'erreur de paramètrage de la fonction */
		exit(699);
	}

	/* ouverture de la clé dans le registre */
	int cr;
	cr = RegOpenKeyEx(HKEY_LOCAL_MACHINE,
		DLPHC_PARAM_PATH,
		0,
		KEY_QUERY_VALUE,
		&phkResult);
	if(cr != ERROR_SUCCESS)
	{
		/* code a rajouter en cas d'erreur de clé */
		RegCloseKey(phkResult);
		exit(700);
	}

	hKey = phkResult;
	/* la clé est ouverte, récupération du paramètre */
	cr = RegQueryValueEx(hKey,
		lpValueName,
		lpReserved,
		lpType,
		lpData,
		lpcbData);
	if(cr != ERROR_SUCCESS)
	{
		/* Code a rajouter en cas d'erreur de récupération des données */
		RegCloseKey(phkResult);
		exit(701);
	}

	/* fermeture de la clé du registre */
	if(!RegCloseKey(hKey))
	{
		/* Code a rajouter en cas d'erreur de fermeture de la clé */
		exit(702);
	}

	/* retour de la valeur du paramètre */
	LPSTR valeur;
	memcpy(valeur,lpData,*lpcbData);
	return(valeur);


} /* fin de la fonction GetParameter */



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
	char line [18],  szBuf[50], lineNew[128];
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
	memset (lineNew,'\0',sizeof(lineNew));

	sprintf (szGen, "%s\0", "./HcCtxDlp");
	sprintf (szHcCfg, "%s.", "./HcConfig");
	//strncat (szGen, hpms->aTermNum, 3);

	//récupérer et mettre le répertoire courant comme répertoire de travail 
	//GetCurrentDirectory(50,buff);
	//SetCurrentDirectory(buff);   


	//Test présence du fichier "HcCtxDlp.xxx"
	hSearch1 = FindFirstFile(szGen, &FileData1); 
	if (hSearch1 != INVALID_HANDLE_VALUE) return TRUE;

	if (hSearch1 == INVALID_HANDLE_VALUE)
	{
		sprintf (lineNew, "Recherche contexte du terminal %s \n", szGen ); 
		Pos_display(lineNew,strlen(lineNew),0);
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

		//	/* test de l'adresse du serveur */
		//	if ((addrRemote.sin_addr.s_addr = ::inet_addr(lpszRemoteAddress)) == INADDR_NONE) 
		//	return FALSE;

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


		//création du fichier dlphc.ini
		/*	Pas besoin ?	if ( ! (CreateFile("dlphc.ini",GENERIC_READ|GENERIC_WRITE,
		FILE_SHARE_READ|FILE_SHARE_WRITE,NULL,
		CREATE_NEW,NULL,NULL)))
		return FALSE;
		*/	

		if (isdigit(rs_init.riSocket[0]) && isdigit(rs_init.riSocket[1]) &&
			isdigit(rs_init.riSocket[2]) && isdigit(rs_init.riSocket[3]) )
		{

			CreateHcCtxDlp(szHcCfg, szGen, rs_init);
		}
		else
		{
			DLP_Pos_display("ERREUR : Socket non numérique", 0, 0); 
			return false;
		}

		//CreateHcCtxDlp(szHcCfg, szGen, rs_init);

	}
	Close();
	return TRUE;
}



int IsContextExist (char *CTX_NAME)
	///////////////////////////////////////////////////////////////////////////////////////////
	//   IsContextExist : Recherche si le fichier existe.
	//
	//               Retour   OUI : Le fichier existe.
	//                        NON : Le fichier n'existe pas.
	//
	//  SJU, 10/12/2003.
	//
	///////////////////////////////////////////////////////////////////////////////////////////

{
	int fd;



	if ((fd = _open (CTX_NAME, _O_RDWR | _O_BINARY)) < 0)
	{
		return FALSE ;
	}
	else
	{
		_close (fd);
	}

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
			//strncpy(lineNew, line, strlen(line));
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
	char line [128], szBuf[50], CibleCOM[12], CibleAUTO[13],CibleFORCE[8];
	char devTcp [32];
	int  lg, ret;


	memset(CibleCOM, '\0', sizeof(CibleCOM));
	memset(CibleAUTO, '\0', sizeof(CibleAUTO));
	memset(CibleFORCE, '\0', sizeof(CibleFORCE));
	memset(szBuf, '\0', sizeof(szBuf));
	bAuto = false; 
	bForce = false;

	strncpy (CibleCOM, "STRATUS_COM",11);
	strncpy (CibleAUTO, "AUTORISATION",12);
	strncpy (CibleFORCE, "FORCAGE",7);

	//lg= strlen(CibleCOM);


	if( ! (fp = fopen(fConfig, "r" )))
		return  FALSE;

	while ( fgets (line , sizeof line , fp) ) 
	{
		lg= strlen(CibleCOM);
		if (!memcmp(CibleCOM ,line , lg) )  
		{
			/* line   ->  Interface:hostName:hostPort */
			strcpy(szBuf, line+lg+1);
			ret = sscanf(szBuf ,"%s %s %s", devTcp,ConfigHost, ConfigSocket);
			//break;
		}

		lg= strlen(CibleAUTO);
		if (!memcmp(CibleAUTO ,line , lg) )  
		{
			/* line   ->  Autorisation*/
			strcpy(szBuf, line+lg+1);
			ret = sscanf(szBuf ,"%i", &bAuto);
			//break;
		}

		lg= strlen(CibleFORCE);
		if (!memcmp(CibleFORCE ,line , lg) )  
		{
			/* line   ->  Autorisation*/
			strcpy(szBuf, line+lg+1);
			ret = sscanf(szBuf ,"%i", &bForce);
			//break;
		}
	}	 

	fclose(fp);

	return TRUE;

}

void CloseMonetic(int Etat)
{
	int Ret;

	if (hMonetic != NULL) 
	{
		if (Etat == 0)
		{
			Ret = PostMessage(hMonetic, WM_USER+7, 0, 0); //Traitement refusé.
		}
		else
		{
			Ret = PostMessage(hMonetic, WM_USER+6, 0, 0); //Traitement autorisé.
		}

		Pos_display("        ",8,0);	  
		Pos_display("- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -",69,0);	  
		Pos_display("           ***       CLIQUEZ SUR OK       ***",45,0);	  
		Pos_display("- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -",69,0);	  

		while (DLP_GetKey() == 0x00)
		{
			Sleep(500);
		}

		Pos_display("Fermeture de l'application",26,0);	  
		Ret = PostMessage(hMonetic, WM_USER+9, 0, 0); //Fermer l'application Monétique.
	}
	return;
}



// Ajout version 4.4
extern "C" DllExport void __cdecl Dlp_Hc_Revelation(S5 &HcRevelationMsgSend,
													S2 &HcPosMsgRecv,
													int (*pGetSecurity)(void),
													int (*pGetKey)(void),
													int (*pGetStringFromKeyboard)(char *, int, char *),
													void(*pPos_display)(char *, int, short))
{
	S5 * hpms = &HcRevelationMsgSend;
	hpmr = &HcPosMsgRecv;
	FILE *fp ;
	char line [18], hostName [13];
	char Port[5];
	char *args[2], Prog[80];
	char   szPan[20], szMsg[255], szC3Cfg[32]; 
	char *limit;
	int Ret;
	static char   sztmp[4]; 
	char Msg[255], szMontantE[11], szMontantD[3];
	bool dlphcc3;	

	S1 shpms;
	S1 *hpms1 = &shpms;

	S3 * stratusQueryMsg2 = new S3;
	S4 * stratusReplyMsg2 = new S4;


	memcpy (hpms1, hpms, sizeof(S1));

	struct MsgToStratus s;

	static char   szGen[16];


	memset(HcMsg,'\0',sizeof(HcMsg));
	memset(HcRep,'\0',sizeof(HcRep));
	memset(ConfigSocket,'\0',sizeof(ConfigSocket));
	memset(ConfigHost,'\0',sizeof(ConfigHost));


	memset (szC3Cfg,'\0',sizeof(szC3Cfg)); // test C3 NM
	sprintf (szC3Cfg, "%s.", "c3Config"); // test C3 NM

	hMonetic = NULL;
	if (IsForceMonetic(szC3Cfg)){
		pGetSecurity = NULL;
		pGetKey = NULL;
		pGetStringFromKeyboard = NULL; 
		pPos_display = NULL;
	}


	if (*pPos_display == NULL)
	{
		hMonetic = FindWindow(NULL,"Monetique");
		if (hMonetic == NULL)
		{
			memset(Prog,'\0',sizeof(Prog));

			memcpy(Prog,"Monetic.exe",11);
			args[0] = "Monetic.exe";
			args[1] = NULL;
			_spawnv(_P_NOWAIT, Prog, args);  //Ouvre l'application Monétique.
		}
		else
		{
			Ret = PostMessage(FindWindow(NULL,"Monetique"), WM_USER+8, 0, 0); //Init de la fenêtre déjà existante.
		}

		while (hMonetic == NULL) //Pour attendre que l'appli soit chargée
		{
			hMonetic = FindWindow(NULL,"Monetique");
		}

		memset(Msg,0x00,sizeof(Msg));
		memset(szPan,0x00,sizeof(szPan));
		memset(szMontantD,0x00,sizeof(szMontantD));
		memset(szMontantE,0x00,sizeof(szMontantE));
		memset(&s, 0x00, sizeof(s));


		strncpy (szMontantE, hpms->rAmount, sizeof(hpms->rAmount)-2);
		strncpy (szMontantD, &hpms->rAmount[sizeof(hpms->rAmount)-2], 2);


		sprintf (Msg, "N° Hotel Charge : %s  -  Montant : %d,%s \0", szPan, atoi(szMontantE), szMontantD);
		DLP_Pos_display(Msg,strlen(Msg),0);

		Ret = PostMessage(FindWindow(NULL,"Monetique"), WM_USER+5, 0, 0); //Donne l'état en cours.
	}

	

	/* test des paramètres d'entrée dans la DLL */
	if	(*pGetSecurity == NULL) GetSecurity = DLP_GetSecurity;
	else GetSecurity = *pGetSecurity;
	if	(*pGetKey == NULL) GetKey = DLP_GetKey;
	else GetKey = *pGetKey;
	if	(*pGetStringFromKeyboard == NULL) GetStringFromKeyboard = DLP_GetStringFromKeyboard;
	else GetStringFromKeyboard = *pGetStringFromKeyboard;
	if	(*pPos_display == NULL) Pos_display = DLP_Pos_display;
	else Pos_display = *pPos_display;

	fflush(0);

	Pos_display("Hotel Charge Revelation v5.4 - 27 janvier 2016",46,0);
	
	if (!(CheckContext(hpms1,s))) 

	{
		setResponse('1',"Erreur récupération du port à partir des fichiers de configuration",hpmr);	
		CloseMonetic(0);
		Pos_display("Erreur socket dans config",25,2);
		return;
	}			


	// Cas SMED appel de C3 pour récuperer le numero HC
	// ajouter par NMI 
	if(hpms->rCustomerPresent == '9' ){
		//DLP_Pos_display("Cas Smart Media",15,0);
		hpms->rCustomerPresent = 'P';
		dlphcc3 = GetHCIDFromTPERevelation(hpms, 'z', Pos_display);
		if (dlphcc3 == FALSE){
			return;
		}
		hpms->rCustomerPresent = '1';
		hpms1->aCustomerPresent = '1';
		memcpy(hpms1->aChargeNum, hpms->rChargeNum, sizeof(hpms->rChargeNum));
	}


	// fin d'ajout

	strncpy (szPan, hpms->rChargeNum, sizeof(hpms->rChargeNum));

	memset (szGen,'\0',sizeof(szGen)); 
	sprintf (szGen, "%s", "./HcCtxDlp");
	// Sup 4.6  strncat (szGen, hpms->rTermNum, 3);



	if(TestMsgSend(hpms1, hpmr)) 	//si le message passé en paramètre est valide
	{
		initSQM(stratusQueryMsg2);	//init de la structure passée au stratus
		if(SetHcAuthorizationRevelation(hpms,stratusQueryMsg2))	//création du message pour le stratus

		{
			memcpy(HcMsg,stratusQueryMsg2,sizeof(HcMsg));

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
				setResponse('1',"Erreur d'initialisation du socket",hpmr);
				//delete hpms1;
				delete stratusQueryMsg2;
				delete stratusReplyMsg2;
				fflush(0);
				Ret = PostMessage(hMonetic, WM_USER+7, 0, 2); //Traitement refusé.
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
			// Sup pour 4.6 CloseMonetic(0);
			Pos_display("Pas de hostname",15,2);

			//4.6, début
			if (bForce)
			{
				GestionDuForcage(stratusQueryMsg2);
				delete stratusQueryMsg2;
				delete stratusReplyMsg2;
				fflush(0);
				Close(); 
			}

			CloseMonetic(0);
			//4.6, Fin
			return ;
		}

		wRemotePort=ConfigSocket; 

		memset(&addrRemote, 0, sizeof(addrRemote));
		memcpy  ( & ( addrRemote.sin_addr) ,nHost->h_addr, sizeof addrRemote.sin_addr );
		addrRemote.sin_family = AF_INET;
		addrRemote.sin_port = ::htons(atoi(wRemotePort));


		// Ouvrir la socket 
		Sleep(300);
		if ((m_hSocket = ::socket(PF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET)
		{
			Close();
			Pos_display("DLL : Socket deja ouvert",24,0);
			//modification de OTHMAN le 08/10/01
			setResponse('1',"Socket deja ouvert",hpmr);
			//delete hpms1;
			delete stratusQueryMsg2;
			delete stratusReplyMsg2;
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
			// Sup 4.6 setResponse('1',"Connexion au serveur impossible",hpmr);
			Pos_display("DLL : Connexion au serveur impossible",37,2);

			//4.6, Début
			if (bForce)
			{
				GestionDuForcage(stratusQueryMsg2);
			}
			else setResponse('1',"Connexion au serveur impossible",hpmr);

			//4.6, Fin

			//delete hpms1;
			delete stratusQueryMsg2;
			delete stratusReplyMsg2;
			fflush(0);	
			Close(); //SJ050606
			CloseMonetic(0);
			return;
			//exit(663);
		}

		/* Passer le socket en mode non-bloquant */
		if (::ioctlsocket(m_hSocket, FIONBIO, &dwNonBlockingMode) != 0)
		{
			::closesocket(m_hSocket);
			m_hSocket = INVALID_SOCKET;
			Pos_display("DLL : paramètrage en mode NON BLOQUANT impossible",48,2);
			//modification de OTHMAN le 08/10/01
			setResponse('1',"paramètrage en mode NON BLOQUANT impossible",hpmr);
			//delete hpms1;
			delete stratusQueryMsg2;
			delete stratusReplyMsg2;
			fflush(0);	
			CloseMonetic(0);
			Close(); //SJ070307
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
			Pos_display("Echec d'emission des données",28,2);
			//delete hpms1;
			delete stratusQueryMsg2;
			delete stratusReplyMsg2;
			fflush(0);
			CloseMonetic(0);
			Close(); //SJ070307
			return;		
			//exit(665);
		}


		/* jusqu'à ce point tout fonctionne a priori correctement */
		/* reception du message renvoyé par le serveur monétique */
		char hcrep[83];
		memset(hcrep,'\0',sizeof(hcrep));

		if(!Receive(dwTimeout, hcrep, sizeof(HcRep)+SOCKON2_HEADER_LEN,1))
		{
			Abort();
			Pos_display("DLL : Echec de Reception",24,0);

			//modification de OTHMAN le 08/10/01	
			setResponse('1',"Echec de Reception",hpmr);
			Pos_display("Echec de réception des données",30,2);
			//delete hpms1;
			delete stratusQueryMsg2;
			delete stratusReplyMsg2;
			fflush(0);		
			CloseMonetic(0);
			Close(); //SJ070307
			return;
			//exit(666);
		}
		else 
		{
			memcpy(stratusReplyMsg2,HcRep,sizeof(HcRep));
#ifdef _DEBUG
			//Pos_display(&stratusReplyMsg->rCreditPlan,0,0);
#endif

			switch(stratusReplyMsg2->rStatusCode)
			{
			case '0':	/* transaction acceptée */

				Pos_display("transaction acceptee...",23,2);
				setResponse(stratusReplyMsg2->rStatusCode,(char*) stratusReplyMsg2->rResponseMessageText,hpmr);
				break;

			case '1':	/* transaction refusée et non forcable */

				Pos_display("transaction refusee...",22,2);

				setResponse(stratusReplyMsg2->rStatusCode,(char*) stratusReplyMsg2->rResponseMessageText,hpmr);
				CloseMonetic(0);
				//delete hpms1;
				delete stratusQueryMsg2;
				delete stratusReplyMsg2;
				fflush(0);
				Close(); //SJ050606
				return;
				break;			

			case '2' :	/* transaction refusée mais forcable */
				char szTmp[255];

				Pos_display("transaction refusee. Forcage...",31,0);

				if (bAuto)
				{
					char m_numauto[6];
					int k;

					memset(szTmp, 0x00, sizeof(szTmp));
					memcpy(szTmp, stratusReplyMsg2->rResponseMessageText,sizeof(stratusReplyMsg2->rResponseMessageText));
					Pos_display(szTmp,strlen(szTmp),0);

					switch(GetStringFromKeyboard(m_numauto,sizeof(m_numauto),"saisie du code d'autorisation : "))
					{
					case '\x41':	/* transaction abandonnée */
						Pos_display("transaction abandonnee",22,2);
						setResponse('1',"Trans. abandonnee",hpmr);
						CloseMonetic(0);
						//delete hpms1;
						delete stratusQueryMsg2;
						delete stratusReplyMsg2;
						fflush(0);
						Close(); 
						return;
						break;

					case '\x12':	/* transaction acceptée */

						for(k=0;k<sizeof(stratusQueryMsg2->ForcePostAuthCode);k++)

							if (isdigit(m_numauto[k]))
								stratusQueryMsg2->ForcePostAuthCode[k] = m_numauto[k];
							else
								stratusQueryMsg2->ForcePostAuthCode[k] = 0x20;


						stratusQueryMsg2->ForcePostFlag='1';

						memcpy(HcMsg,stratusQueryMsg2,sizeof(HcMsg));

						/* envoi du message avec le num_auto pour le stratus */
						if(!Send(HcMsg,sizeof(HcMsg),1))
						{
							Abort();

							//modification de OTHMAN le 08/10/01
							setResponse('1',"Echec d'emission des données",hpmr);
							//delete hpms1;
							delete stratusQueryMsg2;
							delete stratusReplyMsg2;
							fflush(0);
							Pos_display("echec d'emission des données avec num d'auto",44,2);
							CloseMonetic(0);
							Close(); //SJ070307
							return;		
						}

						Pos_display("Forçage enregistré.",23,2);

						setResponse('0',"Trans. acceptee",hpmr);
						break;

					default :		/* erreur dans tout autre cas */
						Pos_display("erreur code autorisation",24,0);
						//setResponse('1',"Trans. refusee",hpmr);
						setResponse(stratusReplyMsg2->rStatusCode,(char*) stratusReplyMsg2->rResponseMessageText,hpmr);
						memset(szTmp, 0x00, sizeof(szTmp));
						memcpy(szTmp, stratusReplyMsg2->rResponseMessageText,sizeof(stratusReplyMsg2->rResponseMessageText));
						Pos_display(szTmp,strlen(szTmp),0);
						break;
					} //end switch
				}
				else // (!bAuto)
				{
					Pos_display("Transaction refusée",19,0);
					//setResponse('1',"Trans. refusee",hpmr);
					setResponse(stratusReplyMsg2->rStatusCode,(char*) stratusReplyMsg2->rResponseMessageText,hpmr);
					/*memset(szTmp, 0x00, sizeof(szTmp));
					memcpy(szTmp, stratusReplyMsg2->rResponseMessageText,sizeof(stratusReplyMsg2->rResponseMessageText));
					Pos_display(szTmp,strlen(szTmp),0);*/
				}
				//}
				break;

			default:	/* erreur pour toute autre valeur */
#ifdef _DEBUG
				Pos_display("erreur code reponse",19,0);
#endif
				setResponse('1',"Trans. refusee",hpmr);
				//delete hpms1;
				delete stratusQueryMsg2;
				delete stratusReplyMsg2;
				fflush(0);		
				Pos_display("Transaction refusée...",22,2);
				fflush(0); //5.2
				CloseMonetic(0);
				Close(); //5.2
				return;
				//exit(667);
				break;
			}

			delete stratusQueryMsg2;
			delete stratusReplyMsg2;
			fflush(0);
			CloseMonetic(1);
			Close();
		}
	}
	else
	{	
		//delete hpms1;
		delete stratusQueryMsg2;
		delete stratusReplyMsg2;
		fflush(0);
		Pos_display("Transaction refusée... Mauvais paramètres caisse",49,2);
		CloseMonetic(0);
		return;
	}


	return;
}/* end Dlp_Hc_Revelation... */



extern "C" DllExport void __cdecl EncodeDecode(S6 &MsgCrypt)
{

	int Ret;
	char sztmp[512];
	char * Ligne, *Resultat;


	memset(sztmp,'\0',sizeof(sztmp));
	memset(MsgCrypt.cInfoRetour,0x00, sizeof(MsgCrypt));
	strncpy(Ligne, (LPSTR) MsgCrypt.cInfoAller, strlen(MsgCrypt.cInfoAller));
	Ligne[strlen(MsgCrypt.cInfoAller)] = 0x00;

	MsgCrypt.cCodeRetour=0;

	switch (MsgCrypt.cOperation)
	{
	case 'D' : // Décode
		if (Dechiffre(Ligne, Resultat))
		{
		}
		break;



	case 'E' : // Encode
		if (Chiffre(Ligne, Resultat))
		{
		}
		break;

	default :
		MsgCrypt.cCodeRetour= -1;


	}



}



/*** SetHcAuthorizationMsg *************************************************/
/*                                                                         */
/*   Affectations du message à envoyer au stratus                          */
/*                                                                         */
/*                                                                         */
/*                                                                         */
/***************************************************************************/
bool SetHcAuthorizationRevelation(S5 * hpms, S3 * stratusQueryMsg)
{
	int i=0;


	//	4.5 for(i=0;i<sizeof(stratusQueryMsg->SourceTerminal);i++)
	//	4.5	stratusQueryMsg->SourceTerminal[i] = hpms->rTermNum[i+1];

	stratusQueryMsg->SourceTerminal[0] = '0'; //Ajout 4.5
	// Sup 4.7  stratusQueryMsg->SourceTerminal[1] = '1'; //Ajout 4.5
	stratusQueryMsg->SourceTerminal[1] = hpms->rCtrlNum[4]; //Ajout 4.7
	stratusQueryMsg->FS2 = '\x29';

	memcpy(stratusQueryMsg->SystemNumber,hpms->rCtrlNum,4);  //controleur

	stratusQueryMsg->LocationNumber[3] = hpms->rCtrlNum[4];  // register


	if(hpms->rOperation == 'd') stratusQueryMsg->AmountOfCharges[0] = ' ';
	else stratusQueryMsg->AmountOfCharges[0] = '-';

	for(i=0;i<(sizeof(stratusQueryMsg->AmountOfCharges)-1);i++)
		stratusQueryMsg->AmountOfCharges[i+1] = hpms->rAmount[i+3];	/* troncage de 3 à gauche */

	for(i=0;i<sizeof(stratusQueryMsg->TransactionNumber);i++)
		stratusQueryMsg->TransactionNumber[i] = hpms->rTrnsNum[i+2];/* troncage de 2 à gauche */

	memcpy(stratusQueryMsg->HotelCode,hpms->rChargeNum,2);

	for(i=0;i<sizeof(stratusQueryMsg->FolioNumber);i++)
		stratusQueryMsg->FolioNumber[i] = hpms->rChargeNum[i+2];

	for(i=0; i< (sizeof(hpms->rNumTransUnique)); i++)
		stratusQueryMsg->ChargeTipAmount[i] = hpms->rNumTransUnique[i];


	for(i=0;i<sizeof(stratusQueryMsg->CurrencyCode);i++)
		stratusQueryMsg->CurrencyCode[i] = hpms->rCurrencyCode[i];

	for(i=0;
		i< (sizeof(stratusQueryMsg->CashierNumber) + sizeof(stratusQueryMsg->ServerNumber));
		i++)
		stratusQueryMsg->CashierNumber[i] = hpms->rCashNum[i+1];

	for(i=0; i< (sizeof(stratusQueryMsg->Itemizer1)); i++)
		stratusQueryMsg->Itemizer1[i] = hpms->rItem1[i];

	for(i=0; i< (sizeof(stratusQueryMsg->Itemizer2)); i++)
		stratusQueryMsg->Itemizer2[i] = hpms->rItem2[i];

	for(i=0; i< (sizeof(stratusQueryMsg->Itemizer3)); i++)
		stratusQueryMsg->Itemizer3[i] = hpms->rItem3[i];

	for(i=0; i< (sizeof(stratusQueryMsg->Itemizer4)); i++)
		stratusQueryMsg->Itemizer4[i] = hpms->rItem4[i];

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

}


/**********************************************************************************
*  SJU, le 03/07/2007
*
*  Fonction : Enregistre la transaction déjà formatée dans le fichier MsgHcOff.dat
*
*  Retour   : TRUE  si l'opération s'est déroulée correctement
*             FALSE en cas de problème
*
***********************************************************************************/
bool AddTrnInFileOffLine(LPCSTR Msg)
{
	FILE *fpNew ;
	int  lg, ret ;
	char *EnChiffre;


	Pos_display("Attente traitement off-line",27,0);

	//	if (!Chiffre ((char*)Msg, EnChiffre))
	//	{	
	//		Pos_display("Echec de sécurisation",21,0);
	//        return false ;
	//	}

	if( ! (fpNew = fopen("MsgHcOff.dat", "a+" )))
	{
		Pos_display("Fichier off-line non trouvé",27,0);
		return false ;
	}

	Pos_display("Forçage en cours d'enregistrement",33,0);

	fputs(Msg, fpNew);
	//fputs(EnChiffre, fpNew);
	fputs("\n", fpNew);

	fclose(fpNew);

	return true;
}


/**********************************************************************************
*  SJU, le 03/07/2007
*
*  Fonction : Traite le off-line 
*
*  Retour   : Aucun
*
***********************************************************************************/
void GestionDuForcage(S3 *Msg)
{
	char m_numauto[6];
	int k;
	char szFolio[10];


	memset(szFolio, 0x00, sizeof(szFolio));

	memcpy(szFolio,Msg->HotelCode,2); 
	memcpy(&szFolio[2],Msg->FolioNumber ,7);

	if (!CheckKeyDlpHC(szFolio) )
	{
		setResponse('1',"Refus folio erroné",hpmr);
		Pos_display("REFUS n° folio EasyPass non valide",34,0);
		return;	
	}

	Pos_display("Pas de réseau, Forcage ?",24,0);
	switch(GetStringFromKeyboard(m_numauto,sizeof(m_numauto),"saisie du code d'autorisation : "))
	{
	case '\x41':	// offline abandonné 
		Pos_display("transaction abandonnee",22,2);
		setResponse('1',"Trans. abandonnee",hpmr);
		break;

	case '\x12':	// offline accepté 
		for(k=0;k<sizeof(Msg->ForcePostAuthCode);k++)
			if (isdigit(m_numauto[k]))
				Msg->ForcePostAuthCode[k] = m_numauto[k];
			else
				Msg->ForcePostAuthCode[k] = 0x20;


		Msg->ForcePostFlag='2';

		memcpy(HcMsg,Msg,sizeof(HcMsg));

		if (AddTrnInFileOffLine(HcMsg) == true)
		{
			Pos_display("Forçage enregistré.",19,2);
			setResponse('0',"Trans. enregis.",hpmr);
		}
		//CloseMonetic(0);

		break;
	}

	return;
}


/**********************************************************************************
*  SJU, le 03/07/2007
*
*  Fonction : Check le n° folio entré par le CM
*
*  Retour   : TRUE  si clé correcte
*			  FALSE si clé incorrecte
*
***********************************************************************************/

bool CheckKeyDlpHC(LPSTR Folio)
{
	int i,total,ires, icle;
	char szi[2];

	total=0; icle=0;

	for (i=0 ; i < 9 ; i++)
		if (!isdigit(Folio[i])) return false;
		else
		{
			memset(szi, 0x00, 2);
			memcpy(szi, &Folio[i], 1);

			switch (i)
			{
			case 0: 
				icle =  atoi(szi);
				break;

			case 1:
				ires = 9 * atoi(szi);
				while (ires > 9) ires=(ires/10) + ires%10;
				total+=ires;
				break;

			default:
				ires = i * atoi(szi);
				while (ires > 9) ires=(ires/10) + ires%10;
				total+=ires;
				break;
			}
		}
		return (icle==total%9);
}



/**********************************************************************************
*  SJU, le 02/06/2008
*
*  Fonction : Chiffre une chaine de caractère
*
*  Retour   : TRUE  si le chiffre est techniquement correct
*			  FALSE si chiffre est techniquement incorrect
*
***********************************************************************************/
bool Chiffre(char *EnClair, char *EnChiffre)
{
	int i, ret;
	char *sztmp;

	ret = sprintf(sztmp,"%s",EnClair);

	return true;
}


/**********************************************************************************
*  SJU, le 02/06/2008
*
*  Fonction : Déchiffre une chaine de caractère
*
*  Retour   : TRUE  si le déchiffrage est techniquement correct
*			  FALSE si chiffre est techniquement incorrect
*
***********************************************************************************/
bool Dechiffre(char *EnClair, char *EnChiffre)
{
	int i, ret;
	char *sztmp;

	ret = sprintf(sztmp,"%o",EnClair);

	return true;
}



/**********************************************************************************/
/*																				  */
/*							Fonction C3											  */
/*																				  */
/**********************************************************************************/



/**********************************************************************************/
/*                                                                                */
/*       Initialise les structures du C3 pour utiliser l'ordre A                  */
/*                                                                                */
/**********************************************************************************/
void InitCarte(CcPosInitSend send, struct tpvMessIn *c3)
{

	memset(c3,0x20,sizeof(tpvMessIn));
	c3->cOperation[0] = INIT_CARTE;

	memcpy (c3->cTermNum, send.cTermNum, 8); 
	memcpy (c3->cCashNum, send.cCashNum, 7); 



	//Fin InitCarte
}


int CreateLastInit()
	///////////////////////////////////////////////////////////////////////////////////////////
	//   CreateLastInit : Créé le fichier LastInit.
	//
	//               Retour   OUI : Le fichier est créé.
	//                        NON : Le fichier n'est pas créé.
	//
	//  SJU, 10/12/2003.
	//
	///////////////////////////////////////////////////////////////////////////////////////////
{
	FILE *fp;
	char szName [30];
	char jjmmaaaa[15];
	time_t time_c;
	struct tm *dat_j;


	memset (szName,0x00, sizeof(szName));
	memset (jjmmaaaa,0x00, sizeof(jjmmaaaa));
	sprintf (szName, "%s.", "LastInit");

	if (!(fp = fopen (szName,"w")))
	{
		DLP_Pos_display("LastInit non créé", 0, 0); //SJ pour debug
		return FALSE ;
	}
	else
	{
		/* Appel fonction date systeme */
		time (&time_c);
		dat_j = localtime (&time_c);
		sprintf (jjmmaaaa, "%02d%02d%04d", dat_j->tm_mday, dat_j->tm_mon+1, dat_j->tm_year+1900);
		fputs(jjmmaaaa, fp );
		fclose (fp);
	}

	return TRUE;
}


void CheckInit(S1* msgSend, struct tpvMessIn *c3, void (*pPos_display)(char *, int, short))
	//void CheckContext(lpCcPosMsgSend lpGalxSend, struct tpvMessIn *c3)
	///////////////////////////////////////////////////////////////////////////////////////////
	//   CheckContext : Regarde si les fichiers contextuels AMX, CB et GEN existent
	//                  OUI : ne demande pas l'init
	//                  NON : demande d'initialisation
	//                        Init correct : NO TAF
	//                        Init échoue  : les fichiers contextuels partiels
	//                                       du terminal sont effacés.
	//  SJU, 02/08/2001.
	//  SJU, 01/08/2003 : Modif CB5.2 (Noté cb52)

	///////////////////////////////////////////////////////////////////////////////////////////
{
	struct tpvMessOut out;
	static char   sztmp[5], szGen[32], szC3Cfg[32], szLastInit[32];
	int iCtrl;
	CcPosInitSend lpGalxInit;
	struct MsgToStratus s;
	int Ret;
	Ret = FALSE;
	memset(&s, 0x20, sizeof(s));
	memset(sztmp,0x00, sizeof(sztmp));
	strncpy(sztmp,msgSend->aCtrlNum,4);
	iCtrl = 0;


	printf ("Recherche contextuelle \n");
	memset (szGen,'\0',sizeof(szGen));
	memset (szC3Cfg,'\0',sizeof(szC3Cfg));
	memset (szLastInit,'\0',sizeof(szLastInit));


	//sprintf (szGen, "%s.", "c3CtxGen");
	sprintf (szGen, "c3CtxGen.%-4.4d\0",atoi(sztmp) );
	sprintf (szC3Cfg, "%s.", "c3Config");
	sprintf (szLastInit, "%s.", "LastInit");
	strncat (szGen, &msgSend->aRegister, sizeof(msgSend->aRegister)); // SJ, Add CB52
	strncat (szGen, msgSend->aTermNum, 3);

	if (IsContextValid(szLastInit) == FALSE )
	{

		printf ("Rechargement du contexte, demande auto init \n");
		//DLP_Pos_display("Defaut de contexte, demande auto init", 0, 0); 
		Pos_display("Rechargement du contexte, demande auto init.", 44, 0);
		memset (&lpGalxInit, 0x20, sizeof lpGalxInit);	    
		memset (lpGalxInit.cCashNum, '0',sizeof(lpGalxInit.cCashNum)); //SJ, Add CB52
		memcpy (&lpGalxInit.cCashNum[1], msgSend->aCashNum, sizeof(msgSend->aCashNum)); //SJ Modif CB52
		memcpy (lpGalxInit.cTermNum, msgSend->aTermNum , sizeof(msgSend->aTermNum) );
		memcpy (lpGalxInit.cTermNum, msgSend->aCtrlNum, sizeof(msgSend->aCtrlNum) ); // Add SJ CB52 
		memcpy (&lpGalxInit.cTermNum[4], &msgSend->aRegister, sizeof(msgSend->aRegister) ); // Add SJ CB52 
		memcpy (&lpGalxInit.cTermNum[5], msgSend->aTermNum , sizeof(msgSend->aTermNum) ); // Add SJ CB52 



		if (! CreateLastInit())
		{
			//DLP_Pos_display("Enregistrement Init terminé", 0, 0); 
			Pos_display("Enregistrement Init terminé", 27, 0); 
		}

		InitCarte(lpGalxInit, c3);

		c3dll (c3, &out, GetSecurity, GetKey, GetStringFromKeyboard, Pos_display, DLP_PrintTicket);
		//  c3dll (c3, &out, pGetSecurity, pGetKey, pGetStringFromKeyboard, pPos_display);


		//Test de la réponse Init.
		if (memcmp (out.cC3Error, "0000", 4) != 0)
		{
			Pos_display("Init Echouée, Destruction des fichiers contextuels", 50, 0); //SJ pour debug

			if (remove(szGen) != 0 )
			{
			}

		}
		else //Init OK, on peut modifier dynamiquement le c3config
		{
			Pos_display("Init terminée", 13, 0); //SJ pour debug
			if (! CreateLastInit())
			{
				Pos_display("Enregistrement Init terminé", 27, 0); //SJ pour debug
			}
		}
	}
}


void CheckInit(S5* msgSend, struct tpvMessIn *c3, void (*pPos_display)(char *, int, short))
	//void CheckContext(lpCcPosMsgSend lpGalxSend, struct tpvMessIn *c3)
	///////////////////////////////////////////////////////////////////////////////////////////
	//   CheckContext : Regarde si les fichiers contextuels AMX, CB et GEN existent
	//                  OUI : ne demande pas l'init
	//                  NON : demande d'initialisation
	//                        Init correct : NO TAF
	//                        Init échoue  : les fichiers contextuels partiels
	//                                       du terminal sont effacés.
	//  SJU, 02/08/2001.
	//  SJU, 01/08/2003 : Modif CB5.2 (Noté cb52)

	///////////////////////////////////////////////////////////////////////////////////////////
{
	struct tpvMessOut out;
	static char   sztmp[5], szGen[32], szC3Cfg[32], szLastInit[32];
	int iCtrl;
	CcPosInitSend lpGalxInit;
	struct MsgToStratus s;
	int Ret;

	Ret = FALSE;
	memset(&s, 0x20, sizeof s );
	memset(sztmp,0x00, sizeof(sztmp));
	strncpy(sztmp,msgSend->rCtrlNum,4);
	iCtrl = 0;

	Pos_display("Recherche contextuelle", 22, 0);
	memset (szC3Cfg,'\0',sizeof(szC3Cfg));
	memset (szLastInit,'\0',sizeof(szLastInit));


	//sprintf (szGen, "%s.", "c3CtxGen");
	sprintf (szC3Cfg, "%s.", "c3Config");
	sprintf (szLastInit, "%s.", "LastInit");
	strncat (szGen, &msgSend->rRegister, sizeof(msgSend->rRegister)); // SJ, Add CB52
	strncat (szGen, msgSend->rTermNum, 3);

	if (IsContextValid(szLastInit) == FALSE )
	{
		Pos_display("Rechargement du contexte, demande auto init.", 44, 0);
		memset (&lpGalxInit, 0x20, 16);	    
		memset (lpGalxInit.cCashNum, '0',sizeof(lpGalxInit.cCashNum)); //SJ, Add CB52
		memcpy (&lpGalxInit.cCashNum[1], msgSend->rCashNum, sizeof(msgSend->rCashNum)); //SJ Modif CB52
		memcpy (lpGalxInit.cTermNum, msgSend->rTermNum , sizeof(lpGalxInit.cTermNum) );
		memcpy (lpGalxInit.cTermNum, msgSend->rCtrlNum, sizeof(msgSend->rCtrlNum) ); // Add SJ CB52 
		memcpy (&lpGalxInit.cTermNum[4], &msgSend->rRegister, sizeof(msgSend->rRegister) ); // Add SJ CB52 
		memcpy (&lpGalxInit.cTermNum[5], msgSend->rTermNum , sizeof(msgSend->rTermNum) ); // Add SJ CB52 


		if (! CreateLastInit())
		{
			//DLP_Pos_display("Enregistrement Init terminé", 0, 0); 
			Pos_display("Enregistrement Init terminé", 27, 0); 
		}

		InitCarte(lpGalxInit, c3);

		c3dll (c3, &out, GetSecurity, GetKey, GetStringFromKeyboard, Pos_display, DLP_PrintTicket);
		//  c3dll (c3, &out, pGetSecurity, pGetKey, pGetStringFromKeyboard, pPos_display);


		//Test de la réponse Init.
		if (memcmp (out.cC3Error, "0000", 4) != 0)
		{
			//printf ("Init Echouée, Destruction des fichiers contextuels \n");
			Pos_display("Init Echouée, Destruction des fichiers contextuels", 50, 0); //SJ pour debug

			if (remove(szGen) != 0 )
			{
			}

		}
		else //Init OK, on peut modifier dynamiquement le c3config
		{
			Pos_display("Init terminée", 13, 0); //SJ pour debug
			if (! CreateLastInit())
			{
				Pos_display("Enregistrement Init terminé", 27, 0); //SJ pour debug
			}
		}
	}
}


int IsContextValid (char *CTX_NAME)
	///////////////////////////////////////////////////////////////////////////////////////////
	//   IsContextValid : Lit le fichier LastInit pour connaitre la dernière date d'init.
	//
	//               Retour   OUI : L'init a été créé dans la journée.
	//                        NON : L'init date de plus d'un jour.
	//
	//  SJU, 10/12/2003.
	//
	///////////////////////////////////////////////////////////////////////////////////////////
{
	FILE *fp;
	char line [128];
	char jjmmaaaa[15];
	time_t time_c;
	struct tm *dat_j;
	bool InitDuJour;


	memset(jjmmaaaa, 0x00, sizeof(jjmmaaaa));


	InitDuJour = false;

	if (!(fp = fopen (CTX_NAME,"r")))
	{

		return FALSE ;
	}
	else
	{


		while ( fgets (line , sizeof line , fp) ) 
		{
			/* Appel fonction date systeme */
			time (&time_c);
			dat_j = localtime (&time_c);
			sprintf (jjmmaaaa, "%02d%02d%04d", dat_j->tm_mday, dat_j->tm_mon+1, dat_j->tm_year+1900);

			if (memcmp(jjmmaaaa,(char *) line,8) == 0)
			{ 
				InitDuJour = true;
			}
		}
		fclose (fp);
	}

	return InitDuJour;
}

/**********************************************************************************/
/*                                                                                */
/*       Initialise les structures du C3 pourla personnalisation                  */
/*                                                                                */
/**********************************************************************************/

void PersoCarte(S1 *send, struct tpvMessIn *c3)
{

	memset(c3,0x00,sizeof(tpvMessIn));
	c3->cOperation[0] = 'z';

	memcpy (c3->cTermNum, send->aTermNum, 8); 
	memcpy (c3->cCashNum, send->aCashNum, 8); 
	memcpy(c3->cReaderType, "0 ", 2);
	memcpy(c3->cTenderType, "+p", 2);

	memset(c3->cPan, 0x00, sizeof(c3->cPan));
	memset(c3->cPan, '0',sizeof(c3->cPan));

	memset(c3->cUserData1, '0', sizeof(c3->cUserData1));    
	memset(c3->cUserData2, '0', sizeof(c3->cUserData2)); 

	//Fin Personalisation Carte
}



extern "C" DllExport int __cdecl PersonnalisationCarte(S1 &send,int (*pGetSecurity)(void), int (*pGetKey)(void), int (*pGetStringFromKeyboard)(char *, int, char *), void(*pPos_display)(char *, int, short)){
	struct tpvMessIn in;
	struct tpvMessOut out;
	int    Ret;
	char *args[2];
	char   sztmp[4],retMsg[255],Prog[11];

	memset(retMsg, 0x00, 255);
	Ret = FALSE;


	// charge la fenetre monetic.exe
	if (*pPos_display == NULL)
	{
		hMonetic = FindWindow(NULL,"Monetique");
		if (hMonetic == NULL)
		{
			memset(Prog,'\0',sizeof(Prog));

			memcpy(Prog,"monetic.exe",11);
			args[0] = "monetic.exe";
			args[1] = NULL;
			_spawnv(_P_NOWAIT, args[0], args);  //Ouvre l'application Monétique.
		}
		else
		{
			Ret = PostMessage(FindWindow(NULL,"Monetique"), WM_USER+8, 0, 0); //Init de la fenêtre déjà existante.
		}

		while (hMonetic == NULL) //Pour attendre que l'appli soit chargée
		{
			hMonetic = FindWindow(NULL,"Monetique");
		}
	}


	if	(*pGetSecurity == NULL) GetSecurity = DLP_GetSecurity;
	else GetSecurity = *pGetSecurity;
	if	(*pGetKey == NULL) GetKey = DLP_GetKey;
	else GetKey = *pGetKey;
	if	(*pGetStringFromKeyboard == NULL) GetStringFromKeyboard = DLP_GetStringFromKeyboard;
	else GetStringFromKeyboard = *pGetStringFromKeyboard;
	if	(*pPos_display == NULL) Pos_display = DLP_Pos_display;
	else Pos_display = *pPos_display;

	memset (&in, 0x20, sizeof in);	    
	PersoCarte(&send, &in);

	c3dll (&in, &out, DLP_GetSecurity, DLP_GetKey, DLP_GetStringFromKeyboard, DLP_Pos_display, DLP_PrintTicket);

	if (memcmp (out.cC3Error, "0000", 4) != 0)
	{   
		sprintf (retMsg,"c3Error = %4.4s c3ResponseCode = %4.4s", out.cC3Error, out.cReponseCode);
		Pos_display(retMsg,strlen(retMsg),0);
		//sscanf( out.cC3Error, "%4d", &Ret );
		fflush(0); //5.2
		CloseMonetic(0);
		Close(); 
		return -1;
	}    
	else
	{
		fflush(0); //5.2
		CloseMonetic(0);
		Close(); 
		return atoi("0000");
	}

	return -1;
}

void InitTransSMED(S1* Galaxy, struct tpvMessIn *c3)
{
	char tag[5];
	tag[0] = 0;
	memcpy (c3->cOperation, "z", 1);
	memcpy (c3->cAmount, Galaxy->aAmount, 12); 
	memcpy (c3->cCurrencyCode, Galaxy->aCurrencyCode, 3);
	if ( Galaxy->aOperation == 'c' ){
		memcpy(tag, "R5CA0", 5);
	}
	else{
		memcpy(tag, "R5AA0", 5);
	}

	memcpy (c3->cTenderType, "+P", 2);	//typage ppour déterminer la personnalisation ou la transaction
	memcpy (c3->cReaderType, "0 ", 2);	// Lecture effectuée par C3, correspondant a un type de carte

	memset(c3->cTermNum, '0', sizeof(c3->cTermNum));
	memcpy (&c3->cTermNum[0], Galaxy->aCtrlNum, 4);     // n° de controleur
	memcpy (&c3->cTermNum[4], &Galaxy->aRegister, 1);     // n° de register
	memcpy (&c3->cTermNum[5], Galaxy->aTermNum, 3);     // n° de terminal

	memset(c3->cCashNum, '0', sizeof(c3->cCashNum));    
	memcpy (&c3->cCashNum[1], Galaxy->aCashNum, 7);		// n° caissière

	//memcpy (c3->cPan, 0x00, sizeof(c3->cPan));

	//-------------------------
	// Début Particularités DLP
	//-------------------------
	//memset(c3->cUserData1, '0', sizeof(c3->cUserData1));    
	//memset(c3->cUserData2, '0', sizeof(c3->cUserData2)); 

	// Tag de lecture correspondant à l'HC ( cf table des tags)
	memcpy(c3->cUserData1, tag, 5);
	//-----------------------
	// Fin Particularités DLP
	//-----------------------


	//Fin InitTransSMED
}



void InitTransSMEDRevelation(S5* Revelation, struct tpvMessIn *c3)
{
	char tag[5];
	tag[0] = 0;
	memcpy (c3->cOperation, "z", 1);
	memcpy (c3->cAmount, Revelation->rAmount, sizeof(Revelation->rAmount)); 
	if ( Revelation->rOperation == 'c' ){
		memcpy(tag, "R5CA0", 5);
	}
	else{
		memcpy(tag, "R5AA0", 5);
	}
	memcpy (c3->cCurrencyCode, Revelation->rCurrencyCode, 3);

	memcpy (c3->cTenderType, "+P", 2);	  			    //typage ppour déterminer la transaction
	memcpy (c3->cReaderType, "0 ", 2);	// Lecture effectuée par C3

	memset(c3->cTermNum, '0', sizeof(c3->cTermNum));
	memcpy (&c3->cTermNum[0], Revelation->rCtrlNum, 4);     // n° de controleur
	memcpy (&c3->cTermNum[4], &Revelation->rRegister, 1);     // n° de register
	memcpy (&c3->cTermNum[5], Revelation->rTermNum, 3);     // n° de terminal

	memset(c3->cCashNum, '0', sizeof(c3->cCashNum));    
	memcpy (&c3->cCashNum[1], Revelation->rCashNum, 7);		// n° caissière

	//memcpy (c3->cPan, 0x00, sizeof(c3->cPan));

	//-------------------------
	// Début Particularités DLP
	//-------------------------
	//memset(c3->cUserData1, '0', 32);    
	//memset(c3->cUserData2, 0x20, sizeof(c3->cUserData2));  

	// Tag de lecture correspondant à l'HC ( cf table des tags)
	strncpy(c3->cUserData1, tag, 5);

	//-----------------------
	// Fin Particularités DLP
	//-----------------------


	//Fin InitTransSMED
}


/**********************************************************************************/
/*  NM, le 07/08/2015															  */
/*																				  */
/*  Fonction : Activation du TPE pour récuperer l'id à partir du TPE		      */
/*																				  */
/*  Retour   : TRUE or FALSE
/*																				  */
/**********************************************************************************/

extern "C" DllExport bool GetHCIDFromTPE(S1* msgsend, int ordre,  void (*pPos_display)(char *, int, short))
{
	struct tpvMessIn in;
	struct tpvMessOut out;
	int    Ret, szMsg, size;
	char   sztmp[4], sztoread[2];
	char retMsg[255];

	memset(retMsg, 0x00, 255);

	//if (*pPos_display == NULL) pPos_display = DLP_Pos_display;

	Ret = TRUE;
	CheckInit(msgsend , &in, Pos_display);

	switch (ordre)
	{
	case 'z' :  /* SMED */
		memset (&in , 0x20, sizeof in);
		switch(msgsend->aCustomerPresent)
		{
		case 'P' :
			InitTransSMED(msgsend, &in);
			break;
		}
		break;
	default:
		return FALSE;
	}

	
	c3dll (&in, &out, GetSecurity, GetKey, GetStringFromKeyboard, Pos_display, DLP_PrintTicket);
	
	//Ret = MessageBox(NULL, (char *) &in, "Vers C3", MB_OK);//SJ debug

	//Ret = MessageBox(NULL, (char *) &out, "Retour C3", MB_OK); //SJ debug

	//  Test de la réponse
	if (memcmp (out.cC3Error, "0000", 4) != 0)
	{   
		sprintf (retMsg,"c3Error = %4.4s c3ResponseCode = %4.4s", out.cC3Error, out.cReponseCode);
		Pos_display(retMsg,strlen(retMsg),0);
		// carte bloquée 
		if ( (memcmp(out.cC3Error, "0111", 4) == 0) && (memcmp(out.cReponseCode, "0017", 4) == 0)){
			memset(retMsg, 0x00, sizeof(retMsg));
			sprintf (retMsg,"CARTE BLOQUEE");
			Pos_display(retMsg,strlen(retMsg),0);
			Sleep(2000);
		}
		
		//sscanf( out.cC3Error, "%4d", &Ret );
		fflush(0); //5.2
		CloseMonetic(0);
		Close(); 
		return FALSE;
	}    
	else
	{
		Ret = TRUE;
	}

	// Recupérer numéro HC grâce au TAG dans cUserData
	sztoread[0] = out.cUserData1[4];
	sztoread[1] = out.cUserData1[5];

	if( (size = todigit(sztoread, 2)) != -1){
		memcpy(msgsend->aChargeNum, &out.cUserData1[6], size);
	}


	memset(sztmp,'\0',4);
	memcpy (sztmp, out.cC3Error, 4);

	return Ret;

}

extern "C" DllExport bool GetHCIDFromTPERevelation(S5* msgsend, int ordre,  void (*pPos_display)(char *, int, short))
{
	struct tpvMessIn in;
	struct tpvMessOut out;
	int    Ret, szMsg, size;
	char   sztmp[4], sztoread[2];
	char retMsg[255];


	memset(sztoread, 0x00, 2);
	memset(retMsg, 0x00, 255);
	Ret = TRUE;
	CheckInit(msgsend , &in, Pos_display);


	switch (ordre)
	{
	case 'z' :  /* SMED */
		memset (&in , 0x20, sizeof in);
		switch(msgsend->rCustomerPresent)
		{
		case 'P' :
			InitTransSMEDRevelation(msgsend, &in);
			break;
		}
		break;
	default:
		return FALSE;
	}

	c3dll (&in, &out, GetSecurity, GetKey, GetStringFromKeyboard, Pos_display, DLP_PrintTicket);
	
	
	//Ret = MessageBox(NULL, (char *) &in, "Vers C3", MB_OK);//SJ debug

	//Ret = MessageBox(NULL, (char *) &out, "Retour C3", MB_OK); //SJ debug

	//  Test de la réponse
	if (memcmp (out.cC3Error, "0000", 4) != 0)
	{   
		sprintf (retMsg,"c3Error = %4.4s c3ResponseCode = %4.4s", out.cC3Error, out.cReponseCode);
		Pos_display(retMsg,strlen(retMsg),0);
		//sscanf( out.cC3Error, "%4d", &Ret );
		CloseMonetic(0);
		Close(); 
		return FALSE;
	}    
	else
	{

		Ret = TRUE;
	}

	// Recupérer numéro HC grâce au TAG dans cUserData

	// memcpy(msgsend->aChargeNum, out.cUserData1, sizeof msgsend->aChargeNum);
	sztoread[0] = out.cUserData1[4];
	sztoread[1] = out.cUserData1[5];

	if( (size = todigit(sztoread, 2)) != -1){
		memcpy(msgsend->rChargeNum, &out.cUserData1[6], size);
	}


	memset(sztmp,'\0',4);
	memcpy (sztmp, out.cC3Error, 4);

	return Ret;

}

int todigit(char* stringdigit, int size){
	int result = 0;
	int i =0;

	for(i=0; i < size; i++){

		if (!isdigit(stringdigit[i])){

			return -1;
		}

	}

	return atoi(stringdigit);
}

bool IsForceMonetic (char *fConfig)
{
	FILE *fp;
	char line [128], szBuf[50], Cible[15], sztmp[15];
	int  lg, ret ;


	memset(Cible, '\0', sizeof(Cible));
	memset(szBuf, '\0', sizeof(szBuf));
	memset(sztmp, '\0', sizeof(sztmp));



	strncpy (Cible, "DLP_MONETIC",11);

	lg= strlen(Cible);
	ret = -1 ;

	if( ! (fp = fopen(fConfig, "r" )))
		return  false;

	while ( fgets (line , sizeof line , fp) ) 
	{
		if (!memcmp(Cible ,line , lg) )  
		{
			fclose(fp);
			return true; //DLP_MONETIC présent dans C3Config
		}
	}

	fclose(fp);

	return false;
}