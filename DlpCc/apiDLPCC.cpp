//*****
//* DlpCc - Interface Galaxy - C3
//* Initialisation et terminaison de DLPCCDLL.DLL
//* SJU - 26 avril 2001 
//* Dernière livraison : 4 juillet 2014
//* Version 9.13   Parking / HIS
//* Version 9.14a  Proximité
//* Version 9.14   VAD / HIS
//* Version 9.15   Impression Alias
//* Version 9.17   Déclencher la lecteur carte par TPE dans les hôtels
//*****
//* apiDLP.cpp
//*****

#include "DlpCcDLLpriv.h"
#include "apiDLPc3.h"
#include "galaxy.h"
#include "apiDLPCC.h"
#include "apiON2.h"
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <conio.h>
#include <ctype.h>
#include <io.h>
#include <fcntl.h>
#include <process.h>
#include <time.h>
#include <math.h>
#include <Winuser.h>

#define ABSENT       "0"
#define PRESENT      "1"


/////////////////////////////////////////////////////////////////////////////
//                      Variables globales                                 //
/////////////////////////////////////////////////////////////////////////////

char szNumAuto[9];
bool bDebug = FALSE;
bool bRev = FALSE;
bool bHis = FALSE;
bool bTicket = FALSE;
bool bNePasAttendre = FALSE;
char szNumTransRev[9];
char szDate[9], szTime[6];



char ConfigHost[32],ConfigSocket[5],HcRepInit[63], HcRep[73];



/*-----------------------------------------------------------*/
/*     création de la socket et des variables utilisées      */
/*-----------------------------------------------------------*/
static SOCKET	m_hSocket;

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
char			szHeader[SOCKON2_HEADER_LEN + 1];







/////////////////////////////////////////////////////////////////////////////
//    Gestion de la clef superviseur                                       //
/////////////////////////////////////////////////////////////////////////////
int DLP_GetSecurity()
{ 
	//int Ret;

	//Ret = MessageBox(0,"Retour 0","DLP_GetSecurity",MB_OK);

	DLP_Pos_display("Fonction DLP_GetSecurity sollicitée",0,0);
	return 0x12; // SJ pour CB5.2 return 0; 
}


/////////////////////////////////////////////////////////////////////////////
//    Gestion du clavier                                                   //
/////////////////////////////////////////////////////////////////////////////
int DLP_GetKey() 
{

	HANDLE MHdle;
	LPVOID MPtr;
	int Ret;


	//Ret = PostMessage(FindWindow(NULL,"monetique"), WM_USER+2, 0, 0); //Active les pushBtn de choix.
	Ret = SendMessage(FindWindow(NULL,"monetique"), WM_USER+2, 0, 0); //Active les pushBtn de choix.

	MHdle = OpenFileMapping(FILE_MAP_READ | FILE_MAP_WRITE, TRUE, "Fic_Map");
	MPtr = MapViewOfFile(MHdle, FILE_MAP_WRITE | FILE_MAP_READ,0, 0, 128);

	if (MPtr == NULL || MHdle == NULL) return 0x00; 

	if (bNePasAttendre)
	{
		bNePasAttendre = !bNePasAttendre;
		SendMessage (FindWindow(NULL,"monetique"), WM_USER+4, 0L, 0L);
		sprintf ((char *) MPtr, "      \0");
		DestroyMappingFile(MPtr);
		return 13;
	}

	if (memcmp("CANCEL",(char *)MPtr,6) == 0)
	{
		SendMessage (FindWindow(NULL,"monetique"), WM_USER+4, 0L, 0L);
		sprintf ((char *) MPtr, "      \0");
		DestroyMappingFile(MPtr);
		return 0x41;
	}
	if (memcmp("ACCEPT",(char *)MPtr,6) == 0) 
	{
		SendMessage (FindWindow(NULL,"monetique"), WM_USER+4, 0L, 0L);
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

/////////////////////////////////////////////////////////////////////////////
//    Lecture d'une string sur le clavier                                  //
/////////////////////////////////////////////////////////////////////////////
int DLP_GetStringFromKeyboard (char *stringEntered, int MaxlentOfStringEntered, char *messageToDisplay)
{
	HANDLE MHdle;
	LPVOID MPtr;
	char szRep[255], szCode[255];
	int Ret;

	//memset(messageToDisplay, '\0', sizeof(messageToDisplay));		

	DLP_Pos_display(messageToDisplay, MaxlentOfStringEntered, 0);
	memset (szRep, '\0', 255);
	memset (szCode, '\0', 255);
	SendMessage (FindWindow(NULL,"monetique"), WM_USER+3, 0L, 0L);

	MHdle = OpenFileMapping(FILE_MAP_READ | FILE_MAP_WRITE, TRUE, "Fic_Map");
	MPtr = MapViewOfFile(MHdle, FILE_MAP_WRITE | FILE_MAP_READ,0, 0, 128);

	while (TRUE)
	{
		//MHdle = OpenFileMapping(FILE_MAP_READ | FILE_MAP_WRITE, TRUE, "Fic_Map");
		//MPtr = MapViewOfFile(MHdle, FILE_MAP_WRITE | FILE_MAP_READ,0, 0, 128);
		if (memcmp("CANCEL",(char *)MPtr,6) == 0)
		{
			SendMessage (FindWindow(NULL,"monetique"), WM_USER+4, 0L, 0L);
			sprintf ((char *) MPtr, "      \0");
			DestroyMappingFile(MPtr);
			return 0x41;
		}
		if (memcmp("ACCEPT",(char *)MPtr,6) == 0)
		{
			Sleep(200);
			Ret = sscanf((char *) MPtr,"%s %s", szRep, szCode);
			sprintf(stringEntered, szCode); 
			SendMessage (FindWindow(NULL,"monetique"), WM_USER+4, 0L, 0L);
			sprintf ((char *) MPtr, "      \0");
			DestroyMappingFile(MPtr);
			return 0x12;
		}
	}

	return 0x41;
}


/////////////////////////////////////////////////////////////////////////////
//    Affichage des messages de caisse                                     //
/////////////////////////////////////////////////////////////////////////////
/*
#pragma data_seg(".SHRSEC")
char shrMsg[255]="\0";
HGLOBAL hMem;
char *lpMem;
#pragma data_seg()

#pragma comment (linker, "/SECTION:.SHRSEC,RWS")
*/
void DLP_Pos_display (char * messageToDisplay, int lentOfMessageToDisplay, short confirm)
{

	LPVOID Mptr;

	lentOfMessageToDisplay = 0; //Pour éviter warning C4100 à la compil.

	if (Mptr = CreateMappingFile() )
	{

		memset((char *) Mptr, '\0', sizeof (Mptr));	

		strcpy ((char *) Mptr, messageToDisplay);

		if (bHis)
		{
			if (memcmp("LECTURE PUCE",messageToDisplay,12) == 0)
			{
				bNePasAttendre = true;
			}
		}


		SendMessage (FindWindow(NULL,"monetique"), WM_USER+1, 0L, 0L);
		sprintf ((char *) Mptr, "      \0");
		DestroyMappingFile(Mptr);

		switch(confirm)
		{
		case WAIT_KEY:
			//Ret = MessageBox(NULL,(char *) Mptr,"Confirmer",MB_OK);
			break;

		case WAIT_1_SEC :
			//case WAIT_KEY   :
			Sleep(100); // 1000
			break;
		}
	}

}

void DLP_PrintTicket (char * FileToPrint)
{
	return;
}



void Bcd2A ( char * src , char * dst, int n )
{
int i , j ;

       i=j=0 ;

       if ( n % 2 ) /* impaire */
       {
          dst[j++] = (src[i++] & 0xF ) | 0x30;
          n++ ;
       }

        for ( n/=2  ; i < n ; i++,j+=2)
          {
              dst[j  ] = ( (src[i] >> 4) &0xF ) | 0x30  ;
              dst[j+1] = ( src[i] &0xF ) | 0x30  ;
          }
        dst[j ] =  0 ;

}

int A2Bcd (char *dst , char *src , int lent )
{
int  i = 0, j = 0 ;

   if( lent % 2 )  /* impaire */
   {
      dst[lent-1] = 0 ;
      dst[lent-1] += ( src[i] & 0X0F );
      i ++;
   }

   j = i;
   while ( j < lent) 
   {
	  if ( src[j] > '9')
		dst[i] = (( (src[j]-55) << 4 ) & 0XF0 ) ;
	  else
		dst[i] = (( src[j] << 4 ) & 0XF0 ) ;
	  
	  if ( src[j+1] > '9')
		dst[i] += ( (src[j+1]-55) & 0X0F) ;
	  else
		dst[i] += ( src[j+1] & 0X0F) ;
	/*dst[i] = (( src[j] << 4 ) & 0XF0 ) ;
	dst[i] += ( src[j+1] & 0X0F) ;*/
      j += 2 ; i ++ ;
   }

   return i ;
}



void AsciiTODcb(char* in, char * out, int n){
	int i, j;
	char tmp;
	i = j = 0;

	if (n % 2 != 0){
		out[j] = 0;
		out[j] += (in[j] & 0x0F);
		j++;
	}

	for (i = j; i < n ; i+=2){
		tmp = in[i];
		if(in[i] > '9'){
			tmp -=55;
		}
		out[j] = (tmp << 4);

		tmp = in[i + 1];
		if(in[i + 1] > '9')
			tmp -= 55;
		out[j] = out[j] | (tmp & 0x0f);
		j++;
	}


}

/////////////////////////////////////////////////////////////////////////////
// Couche API exportée par la DLL

// Lecture de la date et de l'heure système
DLPCCDLL_API BOOL WINAPI GetDateTime(DLP_LPDATETIME lpDateTime)
{
	// Vérifier les paramètres
	if (lpDateTime == NULL)
	{
		return FALSE;
	}

	// Lire la date et l'heure système
	time_t timeNow = ::time(NULL);
	tm* ptmNow = ::localtime(&timeNow);
	if (ptmNow == NULL)
	{
		return FALSE;
	}

	// Retourner la date et l'heure système
	char buffer[50];

	lpDateTime->wYear = short(ptmNow->tm_year + 1900);
	lpDateTime->byMonth = char(itoa(ptmNow->tm_mon + 1, buffer, 10));
	lpDateTime->byDay = char(itoa(ptmNow->tm_mday, buffer, 10));
	lpDateTime->byHour = char(itoa(ptmNow->tm_hour, buffer, 10));
	lpDateTime->byMin = char(itoa(ptmNow->tm_min, buffer, 10));
	return TRUE;
}




////////////////////////////////////
//    Point d'entrée de DlpCc.
////////////////////////////////////

extern "C" DLPCCDLL_API int Dlp_Cc(lpCcPosMsgSend lpGalxSend,
								   lpCcPosMsgRecv lpGalxRecv,
								   char * p1,
								   char * p2,
								   int (*pGetSecurity)(void), 
								   int (*pGetKey)(void) ,
								   int (*pGetStringFromKeyboard)(char *, int, char *) ,
								   void (*pPos_display)(char *, int, short) ) 
{

	struct tpvMessIn in, EnAttente;
	struct tpvMessOut out;

	int    Ret, i, j; 
	char *args[2], Prog[80];
	char   szPan[20], szMsg[255]; 
	char *limit;
	static char   sztmp[4]; 
	char Msg[255], szMontantE[11], szMontantD[3];
	HWND hMonetic = NULL;
	short Delim;
	char szBin[25], szDateExp[5];
	bool fin;

	//HWND hWndTop = NULL;
	// HWND hChild = NULL;
	//WINDOWPLACEMENT *lpwndpl;
	//char *titre = new char(255);

	char szC3Cfg[32], szLastInit[32];


	//if (p1 == NULL) Ret = MessageBox(NULL, "dlpcc", "p1 vide", MB_OK);
	//if (p2 == NULL) Ret = MessageBox(NULL, "dlpcc", "p2 vide", MB_OK); 
	memset (szC3Cfg,'\0',sizeof(szC3Cfg));
	sprintf (szC3Cfg, "%s.", "c3Config");

	memset (szLastInit,'\0',sizeof(szLastInit));
	sprintf (szLastInit, "%s.", "LastInit");

	memset (&EnAttente,'\0',sizeof(EnAttente));

	Ret = FALSE;

	if (IsForceMonetic(szC3Cfg) || IsProtocoleResa(szC3Cfg))
	{
		pGetSecurity = NULL;
		pGetKey = NULL;
		pGetStringFromKeyboard = NULL;
		pPos_display = NULL;
	}



	if (IsDebugActivated (szC3Cfg)) bDebug = TRUE;
	if (IsTicketActivated (szC3Cfg)) bTicket = TRUE;

	if (bDebug) Ret = MessageBox(NULL, (char *)lpGalxSend, "Recvmsg", MB_OK);

	/*
	if (bRev) 
	{

	hPolo = FindWindow(NULL,"InfDlpIF"); 

	if (hPolo != NULL)
	{
	//DLP_Pos_display("On trouve InfDlpIF",0,0);
	//SendMessage (hPolo, WM_ENABLE, (WPARAM)FALSE, 0L);

	//SetForegroundWindow(hMonetic);
	hChild = GetForegroundWindow();
	SendMessage (hChild, WM_QUIT, 0L, 0L);
	//ShowWindow(hPolo,SW_MAXIMIZE);
	//ShowWindow(HWND_BROADCAST,SW_MINIMIZE);

	}
	}*/

	if (*pPos_display == NULL || IsPosteVAD(szC3Cfg)) //IsPosteVAD() : UNIQUEMENT POUR VAD NEWTON
	{ 
		hMonetic = FindWindow(NULL,"monetique");

		if (hMonetic == NULL)
		{
			memset(Prog,'\0',sizeof(Prog));

			if (bDebug) Ret = MessageBox(NULL, "Dlp_Cc", "hMonetic = NULL", MB_OK);

			memcpy(Prog,"Monetic.exe",11);
			args[0] = "Monetic.exe"; 
			args[1] = NULL;
			_spawnv(_P_NOWAIT, Prog, args);  //Ouvre l'application Monétique.
			if (bDebug) Ret = MessageBox(NULL, "Dlp_Cc", "Monetic.exe lance", MB_OK);
		}
		else
		{
			if (hMonetic != NULL)
			{
				Ret = PostMessage(FindWindow(NULL,"monetique"), WM_USER+8, 0, 0); //Init de la fenêtre déjà existante.
			}
		}

		if (bDebug) Ret = MessageBox(NULL, "Dlp_Cc", "Attente du chargement Monetic.exe", MB_OK);
		while (hMonetic == NULL) //Pour attendre que l'appli soit chargée
		{
			hMonetic = FindWindow(NULL,"monetique");
			Sleep(200);
		}


		if (bDebug) Ret = MessageBox(NULL, "Dlp_Cc", "Monetic.exe pret", MB_OK);

		/*
		if (bRev) 
		{
		if (IsWindowVisible (hMonetic) == false) 
		{
		//ShowWindow(HWND_BROADCAST,SW_MINIMIZE);
		//ShowWindow(hMonetic, SW_MAXIMIZE);
		//BringWindowToTop(hMonetic);
		// DLP_Pos_display("Appel au premier plan",0,0);	  
		//SwitchToThisWindow(hMonetic,true);
		//ShowOwnedPopups(HWND_BROADCAST,true);
		// GetWindowText(GetParent(GetFocus()),Msg,50);
		// Ret = MessageBox(NULL, Msg,"Focus sur la fenêtre", MB_OK);

		}
		}


		GetWindowText(GetParent(GetFocus()),Msg,50);
		Ret = MessageBox(NULL, Msg,"Focus sur la fenêtre", MB_OK); 
		*/ 

		/*char trame[409];
		memset(trame, 0x00, 409);
		memcpy(trame ,"M0502039510014757337", 23);
		memcpy(&trame[294] , "2", 1);
		Dlp_His_Canceled_Card(trame, "267642610");*/

		memset(Msg,0x00,sizeof(Msg));
		memset(szMsg,0x20,sizeof(szMsg)); 
		memset(szPan,0x00,sizeof(szPan));
		memset(szMontantD,0x00,sizeof(szMontantD));
		memset(szMontantE,0x00,sizeof(szMontantE));

		if ((memcmp(lpGalxSend->aCustomerPresent, ABSENT, 1) == 0) || IsPosteVAD(szC3Cfg) )
		{
			if (bDebug) Ret = MessageBox(NULL, "Dlp_Cc", "Traitement PAN", MB_OK);
			for (i=0 ; i < sizeof(lpGalxSend->aPanNum) ; i++) 
			{
				if (memcmp(&lpGalxSend->aPanNum[i],"?",1) != 0)
				{
					strncat(szPan, &lpGalxSend->aPanNum[i], 1);
				}
			}
			if (bDebug) Ret = MessageBox(NULL, "Dlp_Cc", "Traitement DateValidite", MB_OK);
			sprintf (Msg, "Carte : %s  -  Fin Validite : \0", szPan);
			strncat (Msg, lpGalxSend->aEndValid, sizeof(lpGalxSend->aEndValid));
			strncat (Msg, "\0", 1);
			DLP_Pos_display(Msg,0,0);

			if (szPan[0] != 0x20 && szPan[0] != 0x00 && szPan[0] != 0x30) 
			{
				sprintf (szMsg, "%s \0", szPan);
			}
			else
			{
				sprintf (szMsg, "NOPAN \0");
				if (bDebug) Ret = MessageBox(NULL, "Dlp_Cc", "Pas de PAN en paramètre", MB_OK);
			}


			if (lpGalxSend->aEndValid[0] != 0x20 && lpGalxSend->aEndValid[0] != 0x00 
				&& memcmp(&lpGalxSend->aEndValid[0],"0000",4) != 0)
			{
				strncat (szMsg, lpGalxSend->aEndValid, sizeof(lpGalxSend->aEndValid));
			}
			else
			{
				strncat (szMsg, "NODATE\0", 6);
				if (bDebug) Ret = MessageBox(NULL, "Dlp_Cc", "Pas de datevalidite en parametre", MB_OK);
			}

			strncpy (szMontantE, lpGalxSend->aAmount, sizeof(lpGalxSend->aAmount)-2);
			strncpy (szMontantD, &lpGalxSend->aAmount[sizeof(lpGalxSend->aAmount)-2], 2);
			sprintf (Msg, "Montant : %d,%s \0", atoi(szMontantE), szMontantD);
			DLP_Pos_display(Msg,0,0);


			if (IsProtocoleResa(szC3Cfg))
			{
				DLP_Pos_display("Monetic Tools DLRP V9.17 vad / hotel du 24/11/2014 ",0,0);	  
				if (lpGalxSend->aOperation[0] == LECTURE_CARTE)
				{
					InitLecturePiste (lpGalxSend, &in);
					if (*pGetSecurity == NULL) pGetSecurity = DLP_GetSecurity;
					if (*pGetKey == NULL) pGetKey = DLP_GetKey;
					if (*pGetStringFromKeyboard == NULL) pGetStringFromKeyboard = DLP_GetStringFromKeyboard;
					if (*pPos_display == NULL) pPos_display = DLP_Pos_display;

					c3dll (&in, &out, pGetSecurity, pGetKey, pGetStringFromKeyboard, pPos_display, DLP_PrintTicket);

					if (memcmp (out.cC3Error, "0000", 4) != 0)
					{
						Ret = PostMessage(hMonetic, WM_USER+7, 0, 0); //Traitement refusé.
						memset(szMsg,'\0',sizeof(szMsg));
						sprintf (szMsg, "Erreur Traitement c3 : cC3Error (%4.4s), cReponseCode (%4.4s)\n", out.cC3Error, out.cReponseCode);
						pPos_display (szMsg,0,0);
						memcpy(lpGalxRecv->bResponseCode,"1",1);

						if (hMonetic != NULL) 
						{
							DLP_Pos_display("        ",0,0);	  
							DLP_Pos_display("- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -",0,0);	  
							DLP_Pos_display("           ***       CLIQUEZ SUR OK       ***",0,0);	  
							DLP_Pos_display("- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -",0,0);	  

							while (DLP_GetKey() == 0x00)
							{
								Sleep(500);
							}

							DLP_Pos_display("Fermeture de l'application",0,0);	

							Ret = PostMessage(hMonetic, WM_USER+9, 0, 0); //Fermer l'application Monétique.
							
						}
						return FALSE;
					}


				
					Delim=0; fin=false; j=0;

					memset (szBin, 0x00, sizeof(szBin));
					memset (szDateExp, 0x00, sizeof(szDateExp));

					for (i=0;i < (short)strlen(out.cIso2) && !fin ;i++) 
					{

						if (out.cIso2[i] == 0x3D ) // = 
							Delim ++;

						switch(Delim)
						{
						case 0 :
							if (out.cIso2[i] >= 0x30 && out.cIso2[i] <= 0x39)
							{
								strncat(szBin, &out.cIso2[i],1);
								j++;
							}
							break;

						case 1 :
							if (out.cIso2[i+1] >= 0x30 && out.cIso2[i+1] <= 0x39)
								strncat(szDateExp, &out.cIso2[i+1],1);

							if (out.cIso2[i+2] >= 0x30 && out.cIso2[i+2] <= 0x39)
								strncat(szDateExp, &out.cIso2[i+2],1);

							if (out.cIso2[i+3] >= 0x30 && out.cIso2[i+3] <= 0x39)
								strncat(szDateExp, &out.cIso2[i+3],1);

							if (out.cIso2[i+4] >= 0x30 && out.cIso2[i+4] <= 0x39)
								strncat(szDateExp, &out.cIso2[i+4],1);

							fin = true;

							break;

						}

					}

					memcpy (lpGalxSend->aOperation, "D",1);
					memset (lpGalxSend->aPanNum, 0x30, sizeof(lpGalxSend->aPanNum));

					if (j==19)
						memcpy (&lpGalxSend->aPanNum, szBin, sizeof(lpGalxSend->aPanNum));
					else
						memcpy (&lpGalxSend->aPanNum[sizeof(lpGalxSend->aPanNum)-j], szBin, strlen(szBin));

					memcpy (lpGalxSend->aEndValid, szDateExp, sizeof(lpGalxSend->aEndValid));



				}
				else if (memcmp(lpGalxSend->aOperation, "z", 1) == 0 )
				{
					InitPersoSMED (lpGalxSend, &in);
					if (*pGetSecurity == NULL) pGetSecurity = DLP_GetSecurity;
					if (*pGetKey == NULL) pGetKey = DLP_GetKey;
					if (*pGetStringFromKeyboard == NULL) pGetStringFromKeyboard = DLP_GetStringFromKeyboard;
					if (*pPos_display == NULL) pPos_display = DLP_Pos_display;

					c3dll (&in, &out, pGetSecurity, pGetKey, pGetStringFromKeyboard, pPos_display, DLP_PrintTicket);

					if (memcmp (out.cC3Error, "0000", 4) != 0)
					{
						Ret = PostMessage(hMonetic, WM_USER+7, 0, 0); //Traitement refusé.
						memset(szMsg,'\0',sizeof(szMsg));
						sprintf (szMsg, "Erreur Traitement c3 : cC3Error (%4.4s), cReponseCode (%4.4s)\n", out.cC3Error, out.cReponseCode);
						pPos_display (szMsg,0,0);
						memcpy(lpGalxRecv->bResponseCode,"1",1);

						if (hMonetic != NULL) 
						{
							DLP_Pos_display("        ",0,0);	  
							DLP_Pos_display("- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -",0,0);	  
							DLP_Pos_display("           ***       CLIQUEZ SUR OK       ***",0,0);	  
							DLP_Pos_display("- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -",0,0);	  

							while (DLP_GetKey() == 0x00)
							{
								Sleep(500);
							}

							DLP_Pos_display("Fermeture de l'application",0,0);	

							Ret = PostMessage(hMonetic, WM_USER+9, 0, 0); //Fermer l'application Monétique.
						}
						return FALSE;
					}
					if (hMonetic != NULL) 
					{
						DLP_Pos_display("        ",0,0);	  
						DLP_Pos_display("- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -",0,0);	  
						DLP_Pos_display("           ***       CLIQUEZ SUR OK       ***",0,0);	  
						DLP_Pos_display("- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -",0,0);	  

						while (DLP_GetKey() == 0x00)
						{
							Sleep(500);
						}

						DLP_Pos_display("Fermeture de l'application",0,0);	

						Ret = PostMessage(hMonetic, WM_USER+9, 0, 0); //Fermer l'application Monétique.
					}
					return true;
				}
				else
				{
					if ( !TraiteResa_EcranMonetique(szMsg, lpGalxSend) ) 
					{
						memcpy(lpGalxRecv->bResponseCode,"1",1);
						Ret = PostMessage(hMonetic, WM_USER+7, 0, 0); //Traitement refusé.
						return FALSE;
					}
				}
				if (TraiteResa(lpGalxSend, lpGalxRecv) == false)
				{
					DLP_Pos_display("Problème dans la fonction TraiteResa",0,0);	  
					Ret = PostMessage(hMonetic, WM_USER+7, 0, 0); //Traitement refusé.
					memcpy (lpGalxRecv->bResponseCode, "1", 1);
				}
				if (!bHis) memset(&lpGalxRecv->bPanNum[10],'?',sizeof(lpGalxRecv->bPanNum)-6 );
				memset(&lpGalxSend->aPanNum, '0', sizeof(lpGalxSend->aPanNum));


				if (bDebug) DLP_Pos_display("Test hMonetic",0,0);	  
				if (bDebug)  Ret = MessageBox(NULL, (char *) &out, "Retour C3", MB_OK);//SJ debug
				if (bDebug) DLP_Pos_display((char *) &out,0,0); //debug, SJ

				if (hMonetic != NULL) 
				{
					DLP_Pos_display("        ",0,0);	  
					DLP_Pos_display("- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -",0,0);	  
					DLP_Pos_display("           ***       CLIQUEZ SUR OK       ***",0,0);	  
					DLP_Pos_display("- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -",0,0);	  

					while (DLP_GetKey() == 0x00)
					{
						Sleep(500);
					}

					DLP_Pos_display("Fermeture de l'application",0,0);	

					Ret = PostMessage(hMonetic, WM_USER+9, 0, 0); //Fermer l'application Monétique.
				}
				return true;
			}



			if (IsPostePKG(szC3Cfg))
			{
				DLP_Pos_display("Monetic Tools DLRP V9.17b pkg / HIS du 30/10/2015 ",0,0);	  
				if (!TraitePkg(szMsg,lpGalxSend)) //Acquisition des données piste
				{
					DLP_Pos_display("Problème dans la fonction TraiteParking",0,0);	  
					Ret = PostMessage(hMonetic, WM_USER+7, 0, 0); //Traitement refusé.
					memcpy(lpGalxRecv->bResponseCode, "1", 1);
					if (hMonetic != NULL) 
					{
						SendMessage (FindWindow(NULL,"monetique"), WM_USER+23, 0L, 0L);
						DLP_Pos_display("        ",0,0);	  
						DLP_Pos_display("- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -",0,0);	  
						DLP_Pos_display("           ***       CLIQUEZ SUR OK       ***",0,0);	  
						DLP_Pos_display("- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -",0,0);	   

						while (DLP_GetKey() == 0x00)
						{
							Sleep(250);
						}

						Ret = PostMessage(hMonetic, WM_USER+25, 0, 0); //Cacher l'application Monétique.
					}
					return false;
				}
				else // Lecture piste par TPE OK
				{
					if (TraiteResa(lpGalxSend, lpGalxRecv) == false)
					{
						DLP_Pos_display("Problème dans la fonction TraiteResa Pkg",0,0);	  
						Ret = PostMessage(hMonetic, WM_USER+7, 0, 0); //Traitement refusé.
						memcpy(lpGalxRecv->bResponseCode, "1", 1);
						memcpy(lpGalxRecv->bExplanation, "PROBLEME COMMUNICATION", 22);
						Ret = PostMessage(hMonetic, WM_USER+25, 0, 0); //Cacher l'application Monétique.
						return false;
					}
				}



				char * CcCustTicket = NULL;  //Pour Test ticket
				char * CcDlpTicket = NULL;   //Pour Test ticket

				if (bTicket)
				{
					CcCustTicket = (char *) malloc(1024);  //Pour Test ticket
					CcDlpTicket = (char *) malloc(1024);   //Pour Test ticket
					p1 = CcCustTicket; //Pour Test ticket
					p2 = CcDlpTicket; //Pour Test ticket
				}


				if (memcmp(lpGalxRecv->bResponseCode,"0",1)==0) //Transaction autorisée, on génère les tickets
				{
					dateL(szDate);
					timeL(szTime);


					for (i=0; !memcmp("0", &lpGalxSend->aPanNum[i], 1); i++);

					if (!memcmp("35", &lpGalxSend->aPanNum[i] , 2))  
					{
						memcpy (lpGalxRecv->bBbank, DLP_JCB, 2); 
						Ret = ReadTicketDLRP(F_TICKET_JC_DLRP, p1, lpGalxSend, lpGalxRecv);	
						Ret = ReadTicketDLRP(F_TICKETCOM_JC_DLRP, p2, lpGalxSend, lpGalxRecv);	
					}
					else
					{
						if (!memcmp("34", &lpGalxSend->aPanNum[i] , 2) || !memcmp("37", &lpGalxSend->aPanNum[i], 2))  
						{
							memcpy (lpGalxRecv->bBbank, DLP_AMEX, 2);
							Ret = ReadTicketDLRP(F_TICKET_AM_DLRP, p1, lpGalxSend, lpGalxRecv);	
							Ret = ReadTicketDLRP(F_TICKETCOM_AM_DLRP, p2, lpGalxSend, lpGalxRecv);	
						}
						else
						{
							memcpy (lpGalxRecv->bBbank, DLP_FRANCE, 2);  
							Ret = ReadTicketDLRP(F_TICKET_CB_DLRP, p1, lpGalxSend, lpGalxRecv);	
							Ret = ReadTicketDLRP(F_TICKETCOM_CB_DLRP, p2, lpGalxSend, lpGalxRecv);	
						}

					}
				}

				if (bDebug) MessageBox(NULL, (char *) p1, "Ticket Client", MB_OK);
				if (bDebug) MessageBox(NULL, (char *) p2, "Ticket Commerçant", MB_OK);
				if (bDebug) DLP_Pos_display("Test hMonetic",0,0);	  
				if (bDebug) Ret = MessageBox(NULL, (LPSTR) &out, "Retour C3", MB_OK);//SJ debug
				if (bDebug) DLP_Pos_display((char *) &out,0,0); //debug, SJ

				if (bTicket)
				{
					free(CcCustTicket); //Pour Test ticket
					free(CcDlpTicket); //Pour Test ticket
				}

				if (hMonetic != NULL) 
				{
					SendMessage (FindWindow(NULL,"monetique"), WM_USER+23, 0L, 0L);
					DLP_Pos_display("        ",0,0);	  
					DLP_Pos_display("- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -",0,0);	  
					DLP_Pos_display("           ***       CLIQUEZ SUR OK       ***",0,0);	  
					DLP_Pos_display("- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -",0,0);	  

					while (DLP_GetKey() == 0x00)
					{
						Sleep(250);
					}

					DLP_Pos_display("Fermeture de l'application",0,0);	

					//Ret = PostMessage(hMonetic, WM_USER+9, 0, 0); //Fermer l'application Monétique.

					Ret = PostMessage(hMonetic, WM_USER+25, 0, 0); //Cacher l'application Monétique.

				}
				return true;
			}


			if ( !TraiteVad(szMsg, lpGalxSend) ) 
			{
				//pPos_display("Pb VAD * Appel HOT-LINE *",0,0);
				return FALSE;
			}

			if (bDebug) DLP_Pos_display("Test hMonetic",0,0);	  
			if (bDebug)  Ret = MessageBox(NULL, (char *) &out, "Retour C3", MB_OK);//SJ debug


			if (hMonetic != NULL) 
			{
				DLP_Pos_display("        ",0,0);	  
				DLP_Pos_display("- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -",0,0);	  
				DLP_Pos_display("           ***       CLIQUEZ SUR OK       ***",0,0);	  
				DLP_Pos_display("- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -",0,0);	  

				while (DLP_GetKey() == 0x00)
				{
					Sleep(500);
				}

				if (pPos_display != NULL) pPos_display("Fermeture de l'application",0,0);	
				Ret = PostMessage(hMonetic, WM_USER+9, 0, 0); //Fermer l'application Monétique.
			}
		} // Fin du traitement sans C3



		Ret = PostMessage(FindWindow(NULL,"monetique"), WM_USER+5, 0, 0); //Donne l'état en cours.
	} //Fin Pos_Display == NULL ou poste VAD.


	if (*pGetSecurity == NULL) pGetSecurity = DLP_GetSecurity;
	if (*pGetKey == NULL) pGetKey = DLP_GetKey;
	if (*pGetStringFromKeyboard == NULL) pGetStringFromKeyboard = DLP_GetStringFromKeyboard;
	if (*pPos_display == NULL) pPos_display = DLP_Pos_display;


	if (lpGalxRecv == NULL) 
	{
		pPos_display("RECV NULL",0,0);
		return FALSE;
	}
	if (lpGalxSend == NULL) 
	{
		pPos_display("SEND NULL",0,0);
		return FALSE;
	}

	pPos_display("Monetic Tools DLRP V9.16A proxi du 08/03/2016 ",0,0);	  
	CheckContext(lpGalxSend , &in, pPos_display);

	switch (lpGalxSend->aOperation[0])
	{
	case  'D' :   /* Débit */
		memset (&in, 0x20, sizeof in);	  
		InitDebit(lpGalxSend, &in);
		break;

	case  'C' :   /* Crédit */
		memset (&in, 0x20, sizeof in);	  
		InitCredit(lpGalxSend, &in);
		break;

	case  'L' :   /* Get Version Number */
		memset (&in, 0x20, sizeof in);	  
		VersionNumber(&in);
		break;

	case  'M' :   /* Pré-autorisation */
		memset (&in, 0x20, sizeof in);	  
		InitPreAuto (lpGalxSend, &in);
		break;

	case  'K' :   /* Duplicata */
		memset (&in, 0x20, sizeof in);	  
		InitDuplicata (lpGalxSend, &in);
		break;

	case  'I' :   /* Lecture piste */
		memset (&in, 0x20, sizeof in);	  
		InitLecturePiste (lpGalxSend, &in);
		break;

	case 'z' :  /* SMED */
		memset (&in , 0x20, sizeof in);
		switch(lpGalxSend->aCvv2[0])
		{
		case 'p' : 
			InitPersoSMED(lpGalxSend, &in);
			break;

		case 'P' :
			InitTransSMED(lpGalxSend, &in);
			break;
		}
		break;

	default:
		pPos_display("Opération inconnue",0,0);
		return FALSE;
	}

	//if (bRev) memcpy(in.cNumTicket, szNumTransRev,sizeof(in.cNumTicket));
	if (bRev) memcpy(in.cUserData1, szNumTransRev,sizeof(in.cNumTicket));

	if (bDebug) Ret = MessageBox(NULL, (char *) &in, "Vers C3", MB_OK);//SJ debug

	c3dll (&in, &out, pGetSecurity, pGetKey, pGetStringFromKeyboard, pPos_display, DLP_PrintTicket);

	if (bDebug)  Ret = MessageBox(NULL, (char *) &out, "Retour C3", MB_OK);//SJ debug
	if (bDebug) DLP_Pos_display((char *) &out,0,0); //debug, SJ


	if ( memcmp (out.cticketAvailable, "1", 1) == 0)
	{
		if (bDebug) DLP_Pos_display("Ticket généré",0,0);
		Ret = ReadTicket(F_TICKET, p1, &in, &out);	
		if (Ret == FALSE) 
		{
			sprintf (szMsg, "Erreur lecture fichier (%11.11s) : , cReponseCode (%4.4s)\n", out.cC3Error, out.cReponseCode);
			DLP_Pos_display("Pb lecture ticket client",0,0);
		}
		//Ret = ReadTicket(F_TICKETCOM, p1, &in);	

		Ret = ReadTicket(F_TICKETCOM, p2, &in, &out);	
		if (Ret == FALSE) if (Ret == FALSE) DLP_Pos_display("Pb lecture ticket commercant",0,0);
		//Ret = ReadTicket(F_TICKET, p2, &in);	


	}

	if (bDebug) i = MessageBox(NULL, (char *) p1, "TicketCom", MB_OK);
	if (bDebug) i = MessageBox(NULL, (char *) p2, "TicketCli", MB_OK);

	memset(szPan,'\0',sizeof(szPan) );
	strncpy(szPan, out.cPan, sizeof(out.cPan) );
	limit = (char *)memchr(out.cPan, '?', sizeof(out.cPan) );

	if (limit == 0x00)
	{
		memcpy (lpGalxRecv->bPanNum, out.cPan, sizeof(out.cPan));
		memset(&lpGalxRecv->bPanNum[10],'?',sizeof(lpGalxRecv->bPanNum)-6 );
	}
	else
	{
		memset(lpGalxRecv->bPanNum,'?',sizeof(lpGalxRecv->bPanNum) );
		Ret = limit - out.cPan ;
		memcpy (&lpGalxRecv->bPanNum[sizeof(out.cPan) - Ret], out.cPan, Ret);
		memset(&lpGalxRecv->bPanNum[sizeof(out.cPan) - Ret+10],'?',sizeof(lpGalxRecv->bPanNum)-6 );
	}

	//  Test de la réponse

	if (memcmp (out.cC3Error, "0000", 4) != 0)
	{
		Ret = PostMessage(hMonetic, WM_USER+7, 0, 0); //Traitement refusé.
		memset(szMsg,'\0',sizeof(szMsg));
		sprintf (szMsg, "Erreur Traitement c3 : cC3Error (%4.4s), cReponseCode (%4.4s)\n", out.cC3Error, out.cReponseCode);
		pPos_display (szMsg,0,0);
		if (bDebug) 
		{
			memset(szMsg,'\0',sizeof(szMsg));
			sprintf (szMsg, "Stratus : \0");
			strncat (szMsg, out.cOptionLibelle, sizeof(out.cOptionLibelle));
			pPos_display (szMsg,0,0);
		}
		sscanf( out.cC3Error, "%4d", &Ret );
		memcpy (lpGalxRecv->bResponseCode, "1", 1);
		memset (lpGalxRecv->bExplanation, '0', sizeof(lpGalxRecv->bExplanation));
		memcpy (lpGalxRecv->bExplanation, out.cOptionLibelle, sizeof(out.cOptionLibelle)) ;
		memset (lpGalxRecv->bBbank, '?', sizeof(lpGalxRecv->bBbank));
		memcpy (lpGalxRecv->bSignature, "0", 1);
	}    
	else
	{
		Ret = PostMessage(hMonetic, WM_USER+6, 0, 0); //Traitement autorisé.
		printf ("Traitement c3 OK\n");
		Ret = TRUE;
		memcpy (lpGalxRecv->bResponseCode, "0", 1);
		memset (lpGalxRecv->bExplanation, '0', sizeof(lpGalxRecv->bExplanation));
		memcpy (lpGalxRecv->bExplanation, out.cOptionLibelle, sizeof(out.cOptionLibelle) );

		memset (szNumAuto, 0x30, sizeof(szNumAuto));            //Spécifique STM
		memcpy (szNumAuto, out.cNumAuto, sizeof(out.cNumAuto)); //Spécifique STM

		memset (lpGalxSend->aEndValid, '0', sizeof(lpGalxSend->aEndValid)); //Spécifique HIS
		memcpy (lpGalxSend->aEndValid, out.cDateFinValidite, sizeof(out.cDateFinValidite)); //HIS


		switch(out.cCardType[0] )
		{                               
		case '6' : memcpy (lpGalxRecv->bBbank, DLP_AMEX, 2);  
			break;

		case '7' : memcpy (lpGalxRecv->bBbank, DLP_DINERS, 2);  
			break;

		case 'D' : memcpy (lpGalxRecv->bBbank, DLP_JCB, 2);  
			break;

		case '1' : if (out.cSSCarte[0] == '0')  //carte Française
				   {
					   memcpy (lpGalxRecv->bBbank, DLP_FRANCE, 2);  
				   }
				   else
				   {
					   memcpy (lpGalxRecv->bBbank, DLP_ETRANGERE, 2);  
				   }
				   break;

		default : memcpy (lpGalxRecv->bBbank, DLP_ETRANGERE, 2);  
		}
		memcpy (lpGalxRecv->bSignature, out.cSignatureDemande, 1);
	} 

	if (bDebug) pPos_display("Test hMonetic",0,0);	  
	if (hMonetic != NULL) 
	{
		pPos_display("        ",0,0);	  
		pPos_display("- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -",0,0);	  
		pPos_display("           ***       CLIQUEZ SUR OK       ***",0,0);	  
		pPos_display("- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -",0,0);	  

		while (DLP_GetKey() == 0x00)
		{
			Sleep(500);
		}

		pPos_display("Fermeture de l'application",0,0);	

		Ret = PostMessage(hMonetic, WM_USER+9, 0, 0); //Fermer l'application Monétique.
	}

	if (bDebug) i = MessageBox(NULL, (char *) lpGalxRecv, "Retour DLL", MB_OK);
	
	if (bDebug){
		memset(sztmp,'\0',4);
		memcpy (sztmp, out.cC3Error, 4);
		i = MessageBox(NULL, sztmp, "Valeur de retour", MB_OK);
	}
	return Ret;

}



////////////////////////////////////
//    Point d'entrée de DlpStm.
////////////////////////////////////
DLPCCDLL_API BOOL WINAPI Dlp_Stm(lpCcPosMsgSend lpGalxSend,
								 lpStmPosMsgRecv lpGalxRecv) 

{
	char * CcCustTicket = NULL;
	char * CcDlpTicket = NULL;
	CcPosMsgRecv  szCcPosMsgRecv;



	CcCustTicket = (char *) malloc(1024);
	CcDlpTicket = (char *) malloc(1024);

	memset(&szCcPosMsgRecv, 0x20, sizeof(szCcPosMsgRecv));

	Dlp_Cc(lpGalxSend, &szCcPosMsgRecv, CcCustTicket, CcDlpTicket, NULL, NULL, NULL, NULL);


	memcpy (lpGalxRecv->bBbank , szCcPosMsgRecv.bBbank, sizeof(lpGalxRecv->bBbank));
	memcpy (lpGalxRecv->bExplanation , szCcPosMsgRecv.bExplanation, sizeof(lpGalxRecv->bExplanation));
	memcpy (lpGalxRecv->bPanNum , szCcPosMsgRecv.bPanNum, sizeof(lpGalxRecv->bPanNum));
	memset(&lpGalxRecv->bPanNum[10],'?',sizeof(lpGalxRecv->bPanNum)-11 ); 
	//memset(lpGalxRecv->bPanNum,'?',sizeof(lpGalxRecv->bPanNum) );
	memcpy (lpGalxRecv->bResponseCode , szCcPosMsgRecv.bResponseCode, sizeof(lpGalxRecv->bResponseCode));
	memcpy (lpGalxRecv->bSignature , szCcPosMsgRecv.bSignature, sizeof(lpGalxRecv->bSignature));

	memset (lpGalxRecv->bNumAuto, 0x20, sizeof(lpGalxRecv->bNumAuto));
	memcpy (lpGalxRecv->bNumAuto, szNumAuto, sizeof(lpGalxRecv->bNumAuto));

	free(CcCustTicket);
	free(CcDlpTicket);

	return true;
}

////////////////////////////////////
//    Point d'entrée de DlpHis.
////////////////////////////////////

DLPCCDLL_API BOOL WINAPI Dlp_His(lpCcPosMsgSend lpGalxSend,
								 lpStmPosMsgRecv HISGalxRecv) 

{
	//char * CcCustTicket = NULL;
	//char * CcDlpTicket = NULL;
	CcPosMsgRecv  szCcPosMsgRecv;


	
	//int Ret = MessageBox(NULL, "DlpHIS", "Vers C3", MB_OK);//SJ debug
	bHis = true;
	memset(HISGalxRecv, 0x00, sizeof(CcPosMsgRecv));
	memset(&szCcPosMsgRecv, 0x00, sizeof(CcPosMsgRecv));
	if( memcmp(lpGalxSend->aOperation, "I", 1) == 0 ){
		memcpy (lpGalxSend->aCustomerPresent, lpGalxSend->aCustomerPresent, 1);
	}

	else{
		memcpy (lpGalxSend->aCustomerPresent,lpGalxSend->aCustomerPresent, 1);
	}
	//Dlp_Cc(lpGalxSend, &szCcPosMsgRecv, CcCustTicket, CcDlpTicket, NULL, NULL, NULL, NULL);
	bHis = Dlp_Cc(lpGalxSend, &szCcPosMsgRecv, NULL, NULL, NULL, NULL, NULL, NULL);

	

	memcpy (HISGalxRecv->bBbank , szCcPosMsgRecv.bBbank, sizeof(szCcPosMsgRecv.bBbank));
	memcpy (HISGalxRecv->bExplanation , szCcPosMsgRecv.bExplanation, sizeof(szCcPosMsgRecv.bExplanation));
	memcpy (HISGalxRecv->bPanNum , szCcPosMsgRecv.bPanNum, sizeof(szCcPosMsgRecv.bPanNum));
	//memset(&lpGalxRecv->bPanNum[10],'?',sizeof(lpGalxRecv->bPanNum)-11 );
	//memset(lpGalxRecv->bPanNum,'?',sizeof(lpGalxRecv->bPanNum) );
	memcpy (HISGalxRecv->bResponseCode , szCcPosMsgRecv.bResponseCode, sizeof(szCcPosMsgRecv.bResponseCode));
	memcpy (HISGalxRecv->bSignature , szCcPosMsgRecv.bSignature, sizeof(szCcPosMsgRecv.bSignature));


	memset(lpGalxSend, 0x00, sizeof(CcPosMsgSend));


	return bHis;
}





extern "C" DLPCCDLL_API int DlpCc_init(lpCcPosInitSend lpGalxInit)
{
	struct tpvMessIn in;
	struct tpvMessOut out;
	int    Ret, szMsg;
	char   sztmp[4];



	Ret = FALSE;


	if (lpGalxInit == NULL) return FALSE;


	//szMsg=MessageBox(NULL, (char *)lpGalxInit,"From TPV", MB_OK);
	memset (&in, 0x20, sizeof in);	    
	InitCarte(lpGalxInit, &in);

	//Ret = MessageBox(NULL, (char *) &in, "Vers C3", MB_OK);//SJ debug

	c3dll (&in, &out, DLP_GetSecurity, DLP_GetKey, DLP_GetStringFromKeyboard, DLP_Pos_display, DLP_PrintTicket);



	//Ret = MessageBox(NULL, (char *) &out, "Retour C3", MB_OK); //SJ debug

	//  Test de la réponse
	if (memcmp (out.cC3Error, "0000", 4) != 0)
	{   
		printf ("Erreur Traitement c3 : cC3Error (%4.4s), cReponseCode (%4.4s)\n", out.cC3Error, out.cReponseCode);
		sscanf( out.cC3Error, "%4d", &Ret );

	}    
	else
	{
		printf ("Traitement c3 OK\n");
		Ret = TRUE;
	}

	memset(sztmp,'\0',4);
	memcpy (sztmp, out.cC3Error, 4);

	return Ret;

}


extern "C" DLPCCDLL_API int DlpCc_C3Admin(lpCcPosInitSend lpGalxInit, int ordre)
{
	struct tpvMessIn in;
	struct tpvMessOut out;
	int    Ret, szMsg;
	char   sztmp[4];


	Ret = FALSE;


	if (lpGalxInit == NULL) return FALSE;


	//szMsg=MessageBox(NULL, (char *)lpGalxInit,"From TPV", MB_OK);
	memset (&in, 0x20, sizeof in);	    

	switch (ordre)
	{
		/*	  case 'I':
		InitLecture(lpGalxSend, &in);
		break;
		*/
	case 'L':
		InitVersion(lpGalxInit, &in);
		break;

	case 'V':
		Ret = MessageBox(NULL, "Ordre V", "DlpCc", MB_OK);//SJ debug
		InitC3Admin(lpGalxInit, &in);
		break;


	default:
		printf ("Ordre DlpCc inconnu");
		return 1;
		break;
	}


	//Ret = MessageBox(NULL, (char *) &in, "Vers C3", MB_OK);//SJ debug

	c3dll (&in, &out, DLP_GetSecurity, DLP_GetKey, DLP_GetStringFromKeyboard, DLP_Pos_display, DLP_PrintTicket);



	//Ret = MessageBox(NULL, (char *) &out, "Retour C3", MB_OK); //SJ debug

	//  Test de la réponse
	if (memcmp (out.cC3Error, "0000", 4) != 0)
	{   
		printf ("Erreur Traitement c3 : cC3Error (%4.4s), cReponseCode (%4.4s)\n", out.cC3Error, out.cReponseCode);
		sscanf( out.cC3Error, "%4d", &Ret );

	}    
	else
	{
		printf ("Traitement c3 OK\n");
		switch (ordre)
		{
		case 'I':
			printf ("ISO2 : (%38.38s)\n", out.cIso2);
			printf ("TypeCarte : (%1.1s)\n", out.cCardType);
			printf ("Validite : (%4.4s)\n", out.cDateFinValidite);
			printf ("CodeService : (%3.3s)\n", out.cCodeService);
			printf ("ConditionSaisie : (%3.3s)\n", out.cCondSaisie);
			break;

		case 'L':
			printf ("Info : (%38.38s)\n", out.cIso2);
			break;


		}

		Ret = TRUE;
	}

	memset(sztmp,'\0',4);
	memcpy (sztmp, out.cC3Error, 4);

	return Ret;

}



/*extern "C" DLPCCDLL_API DlpCc_Version(void)
{
struct tpvMessIn in;
struct tpvMessOut out;
int    Ret, szMsg;
char   sztmp[4];

CcPosMsgSend lpGalxSend;
CcPosMsgRecv lpGalxRecv;


Ret = FALSE;


if (lpGalxInit == NULL) return FALSE;


memset (&in, 0x20, sizeof in);	    
InitVersion(lpGalxInit, &in);

//Ret = MessageBox(NULL, (char *) &in, "Vers C3", MB_OK);//SJ debug

c3dll (&in, &out, DLP_GetSecurity, DLP_GetKey, DLP_GetStringFromKeyboard, DLP_Pos_display);





memset(sztmp,'\0',4);
memcpy (sztmp, out.cC3Error, 4);

return Ret;

}
*/

//////////////////////////////////////////////////
//       Initialisation de la structure C3      //
//////////////////////////////////////////////////


void InitCarte(lpCcPosInitSend Galaxy, struct tpvMessIn *c3)
{

	memset(c3,0x00,sizeof(tpvMessIn));
	c3->cOperation[0] = INIT_CARTE;

	memcpy (c3->cTermNum, Galaxy->cTermNum, 8); // SJ Ajout CB52
	memcpy (c3->cCashNum, Galaxy->cCashNum, 8); // SJ Ajout CB52



	//Fin InitCarte
}



void InitVersion(lpCcPosInitSend Galaxy, struct tpvMessIn *c3)
{

	memset(c3,0x00,sizeof(tpvMessIn));
	c3->cOperation[0] = GET_VERSION_NUMBER;
	memcpy (c3->cTermNum, "00000001", 8); 
	memcpy (c3->cCashNum, "00000001", 8); 

	//Fin InitVersion
}

void InitLecture(lpCcPosMsgSend Galaxy, struct tpvMessIn *c3)
{

	memset(c3,0x00,sizeof(tpvMessIn));
	c3->cOperation[0] = LECTURE_CARTE;
	memcpy (c3->cTermNum, Galaxy->aTermNum, 8);
	memcpy (c3->cCashNum, Galaxy->aCashNum, 8);
	memcpy (c3->cAmount, Galaxy->aAmount, 12); 
	memcpy (c3->cCurrencyCode, Galaxy->aCurrencyCode, 3);

	memcpy (c3->cTenderType, "0 ", 2);	  			    // pas de typage
	memcpy (c3->cReaderType, Galaxy->aReadingMode, 1);	// Lecture effectuée par C3
	memcpy (c3->cCustomerPresent, Galaxy->aCustomerPresent, 0);	


	//Fin InitVersion
}


//////////////////////////////////////////////////
//       Initialisation de la structure C3      //
//////////////////////////////////////////////////


void InitC3Admin(lpCcPosInitSend Galaxy, struct tpvMessIn *c3)
{
	memset(c3,0x20,sizeof(tpvMessIn));

	memcpy (c3->cOperation, "V", 1);
	memcpy (c3->cTermNum, "00000001", 8); 
	memcpy (c3->cCashNum, "00000001", 8); 
	//memcpy (c3->cTenderType, "03", 2); // Chargement local
	memcpy (c3->cTenderType, "02", 2); // Chargement FTP


	//Fin InitC3Admin
}



void InitDebit(lpCcPosMsgSend Galaxy, struct tpvMessIn *c3)
{
	long AmountEuro;	
	char szTmp[13]; 


	AmountEuro = 0L; 
	memset(szTmp, '\0', sizeof(szTmp));

	memcpy (c3->cOperation, "C", 1);
	memcpy (c3->cAmount, Galaxy->aAmount, 12); 
	memcpy (c3->cCurrencyCode, Galaxy->aCurrencyCode, 3);

	memcpy (c3->cTenderType, "0 ", 2);	  			    // pas de typage
	memcpy (c3->cReaderType, Galaxy->aReadingMode, 1);	// Lecture effectuée par C3
	memcpy (c3->cCustomerPresent, Galaxy->aCustomerPresent, 1);	

	memset(c3->cTermNum, '0', sizeof(c3->cTermNum));
	memcpy (&c3->cTermNum[0], Galaxy->aCtrlNum, 4);     // n° de controleur
	memcpy (&c3->cTermNum[4], Galaxy->aRegister, 1);     // n° de register
	memcpy (&c3->cTermNum[5], Galaxy->aTermNum, 3);     // n° de terminal

	memset(c3->cTrnsNum, '0', sizeof(c3->cTrnsNum));
	memcpy (c3->cTrnsNum, &Galaxy->aTrnsNum[2], 4);        // n° de transaction

	memset(c3->cCashNum, '0', sizeof(c3->cCashNum));    
	memcpy (&c3->cCashNum[1], Galaxy->aCashNum, 7);		// n° caissière

	memcpy (c3->cPan, Galaxy->aPanNum, sizeof(c3->cPan));
	memcpy (&c3->cDateFinValidite[2], Galaxy->aEndValid, 2 );
	memcpy (&c3->cDateFinValidite[0], &Galaxy->aEndValid[2], 2 );
	memcpy (c3->cCryptoVAD, Galaxy->aCvv2, sizeof(c3->cCryptoVAD));

	//-------------------------
	// Début Particularités DLP
	//-------------------------
	memset(c3->cUserData1, '0', sizeof(c3->cUserData1));    
	memset(c3->cUserData2, '0', sizeof(c3->cUserData2));    

	memcpy (c3->cUserData1, Galaxy->aCashNum, 7);		// n° caissière  pour ON/2
	memcpy (&c3->cUserData1[7], Galaxy->aCtrlNum, 4);	// n° contrôleur pour ON/2
	memcpy (&c3->cUserData1[11], Galaxy->aRegister, 1);	// n° Register   pour ON/2
	memcpy (&c3->cUserData1[12], Galaxy->aTrnsRef, 1);	// n° Résa pour STM

	//-----------------------
	// Fin Particularités DLP
	//-----------------------


	//Fin InitDebit
}


void InitCredit(lpCcPosMsgSend Galaxy, struct tpvMessIn *c3)
{
	long AmountEuro;	
	char szTmp[13]; 

	AmountEuro = 0L; 
	memset(szTmp, '\0', sizeof(szTmp));


	memcpy (c3->cOperation, "D", 1);
	memcpy (c3->cAmount, Galaxy->aAmount, 12); 
	memcpy (c3->cCurrencyCode, Galaxy->aCurrencyCode, 3);


	strncpy (szTmp, c3->cAmount, 12);

	memcpy (c3->cTenderType, "0", 1);	  			    // pas de typage
	memcpy (c3->cReaderType, Galaxy->aReadingMode, 1);	// Lecture effectué par C3
	memcpy (c3->cCustomerPresent, Galaxy->aCustomerPresent, 1);	// client présent/absent

	memset(c3->cTermNum, '0', sizeof(c3->cTermNum));
	memset(c3->cTermNum, '0', sizeof(c3->cTermNum));
	memcpy (&c3->cTermNum[0], Galaxy->aCtrlNum, 4);     // n° de controleur
	memcpy (&c3->cTermNum[4], Galaxy->aRegister, 1);     // n° de register
	memcpy (&c3->cTermNum[5], Galaxy->aTermNum, 3);     // n° de terminal

	memset(c3->cTrnsNum, '0', sizeof(c3->cTrnsNum));
	memcpy (c3->cTrnsNum, &Galaxy->aTrnsNum[2], 4);        // n° de transaction

	memset(c3->cCashNum, '0', sizeof(c3->cCashNum));    
	memcpy (&c3->cCashNum[1], Galaxy->aCashNum, 7);		// n° caissière

	memcpy (c3->cPan, Galaxy->aPanNum, sizeof(c3->cPan));

	memcpy (&c3->cDateFinValidite[2], Galaxy->aEndValid, 2 );
	memcpy (&c3->cDateFinValidite[0], &Galaxy->aEndValid[2], 2 );


	//-------------------------
	// Début Particularités DLP
	//-------------------------
	memset(c3->cUserData1, '0', sizeof(c3->cUserData1));    
	memset(c3->cUserData2, '0', sizeof(c3->cUserData2));    

	memcpy (c3->cUserData1, Galaxy->aCashNum, 7);		// n° caissière  pour ON/2
	memcpy (&c3->cUserData1[7], Galaxy->aCtrlNum, 4);	// n° contrôleur pour ON/2
	memcpy (&c3->cUserData1[11], Galaxy->aRegister, 1);	// n° Register   pour ON/2
	//-----------------------
	// Fin Particularités DLP
	//-----------------------


	//Fin InitCredit
}


void InitPreAuto(lpCcPosMsgSend Galaxy, struct tpvMessIn *c3)
{
	long AmountEuro;	
	char szTmp[13]; 

	AmountEuro = 0L; 
	memset(szTmp, '\0', sizeof(szTmp));


	memcpy (c3->cOperation, "M", 1);
	memcpy (c3->cAmount, Galaxy->aAmount, 12); 
	memcpy (c3->cCurrencyCode, Galaxy->aCurrencyCode, 3);

	strncpy (szTmp, c3->cAmount, 12);


	memcpy (c3->cTenderType, "0", 1);	  			    // pas de typage
	memcpy (c3->cReaderType, Galaxy->aReadingMode, 1);	// Lecture effectué par C3
	memcpy (c3->cCustomerPresent, Galaxy->aCustomerPresent, 1);	// client présent/absent

	memset(c3->cTermNum, '0', sizeof(c3->cTermNum));
	//memcpy (&c3->cTermNum[5], Galaxy->aTermNum, 3);     // n° de terminal
	memcpy (&c3->cTermNum[0], Galaxy->aCtrlNum, 4);     // n° de controleur
	memcpy (&c3->cTermNum[4], Galaxy->aRegister, 1);     // n° de register
	memcpy (&c3->cTermNum[5], Galaxy->aTermNum, 3);     // n° de terminal


	memset(c3->cTrnsNum, '0', sizeof(c3->cTrnsNum));
	memcpy (c3->cTrnsNum, &Galaxy->aTrnsNum[2], 4);        // n° de transaction

	memset(c3->cCashNum, '0', sizeof(c3->cCashNum));    
	memcpy (&c3->cCashNum[1], Galaxy->aCashNum, 7);		// n° caissière

	memcpy (c3->cPan, Galaxy->aPanNum, sizeof(c3->cPan));

	memcpy (&c3->cDateFinValidite[2], Galaxy->aEndValid, 2 );
	memcpy (&c3->cDateFinValidite[0], &Galaxy->aEndValid[2], 2 );

	memcpy (c3->cCryptoVAD, Galaxy->aCvv2, sizeof(c3->cCryptoVAD));
	memcpy (c3->cTypeFacture, "1", 1);            // 1 pour création de la pré-auto
	memcpy (c3->cNumDossier, "000000000001", 12); // 1 pour création de la pré-auto


	//-------------------------
	// Début Particularités DLP
	//-------------------------
	memset(c3->cUserData1, '0', sizeof(c3->cUserData1));    
	memset(c3->cUserData2, '0', sizeof(c3->cUserData2));    

	memcpy (c3->cUserData1, Galaxy->aCashNum, 7);		// n° caissière  pour ON/2
	memcpy (&c3->cUserData1[7], Galaxy->aCtrlNum, 4);	// n° contrôleur pour ON/2
	memcpy (&c3->cUserData1[11], Galaxy->aRegister, 1);	// n° Register   pour ON/2
	memcpy (&c3->cUserData1[12], Galaxy->aTrnsRef, 1);	// n° Résa pour STM
	//-----------------------
	// Fin Particularités DLP
	//-----------------------


	//Fin InitPreAuto
}


void InitDuplicata(lpCcPosMsgSend Galaxy, struct tpvMessIn *c3)
{

	memset(c3,0x00,sizeof(tpvMessIn));

	c3->cOperation[0] = DUPLICATA_LAST_OP;
	memcpy (&c3->cTenderType[0], "0", 1);


	memcpy (&c3->cTermNum[0], Galaxy->aCtrlNum, 4);     // n° de controleur
	memcpy (&c3->cTermNum[4], Galaxy->aRegister, 1);     // n° de register
	memcpy (&c3->cTermNum[5], Galaxy->aTermNum, 3);     // n° de terminal

	memcpy (&c3->cCashNum[1], Galaxy->aCashNum, 7);		// n° caissière	


	//Fin InitDuplicata
}


void InitLecturePiste(lpCcPosMsgSend Galaxy, struct tpvMessIn *c3)
{

	memset(c3,0x00,sizeof(tpvMessIn));

	c3->cOperation[0] = LECTURE_CARTE;
	memcpy (&c3->cTenderType[0], "0", 1);


	memcpy (&c3->cTermNum[0], Galaxy->aCtrlNum, 4);     // n° de controleur
	memcpy (&c3->cTermNum[4], Galaxy->aRegister, 1);     // n° de register
	memcpy (&c3->cTermNum[5], Galaxy->aTermNum, 3);     // n° de terminal

	memcpy (&c3->cCashNum[1], Galaxy->aCashNum, 7);		// n° caissière	
	memcpy (c3->cAmount, Galaxy->aAmount, 12); 
	memcpy (c3->cCurrencyCode, Galaxy->aCurrencyCode, 3);

	memcpy (c3->cTenderType, "0 ", 2);	  			    // pas de typage
	// memcpy (c3->cReaderType, Galaxy->aReadingMode, 1);	// Lecture effectuée par C3
	memcpy (c3->cReaderType, "0 ", 2);	// Lecture effectuée par C3
	memcpy (c3->cCustomerPresent, Galaxy->aCustomerPresent, 0);	



	//Fin InitDuplicata
}


void InitPersoSMED(lpCcPosMsgSend Galaxy, struct tpvMessIn *c3)
{
	// ajout 04/02
	char Tag[4],TLV[16];
	int i = 0, j = 0;
	int tagpresent = 0;
	memset(Tag, 0x00, sizeof(Tag));
	memset(TLV, 0x00, sizeof(TLV));

	FILE *fp;
	char line [128], Cible[15], sztmp[15];
	int  lg, ret ;
	// char  TypeLecteur [32];


	memset(Cible, '\0', sizeof(Cible));
	memset(sztmp, '\0', sizeof(sztmp));



	strncpy (Cible, "DLP_TAG:",8);

	lg= strlen(Cible);
	ret = -1 ;

	if( ! (fp = fopen("c3Config", "r" )))
		return;

	while ( fgets (line , sizeof line , fp) ) 
	{
		if (!memcmp(Cible ,line , lg) )  
		{
			memcpy(Tag, &line[8] , 4);
			tagpresent = 1;
		}
	}

	fclose(fp);
	
	if ( tagpresent == 0 ){
		
		return;
	}

	// fin d'ajout

	memcpy (c3->cOperation, "z", 1);
	memcpy (c3->cCurrencyCode, Galaxy->aCurrencyCode, 3);

	memcpy (c3->cTenderType, "+p", 2);	  			    //typage pour déterminer la personnalisation ou la transaction
	memcpy (c3->cReaderType, Galaxy->aReadingMode, 1);	// Lecture effectuée par C3

	memset(c3->cTermNum, '0', sizeof(c3->cTermNum));
	memcpy (&c3->cTermNum[0], Galaxy->aCtrlNum, 4);     // n° de controleur
	memcpy (&c3->cTermNum[4], Galaxy->aRegister, 1);     // n° de register
	memcpy (&c3->cTermNum[5], Galaxy->aTermNum, 3);     // n° de terminal

	memset(c3->cCashNum, '0', sizeof(c3->cCashNum));    
	memcpy (&c3->cCashNum[1], Galaxy->aCashNum, 7);		// n° caissière
	
	if( strcmp(Galaxy->aReadingMode, "1") == 0 )
		memcpy (c3->cPan, Galaxy->aPanNum , sizeof(c3->cPan));

	//-------------------------
	// Début Particularités DLP
	//-------------------------
	memset(c3->cUserData1, 0x00, sizeof(c3->cUserData1));    
	memset(c3->cUserData2, 0x00, sizeof(c3->cUserData2));    
	
	memcpy(TLV, Tag, sizeof(Tag));
	memcpy(&TLV[4], "09", 2);
	memcpy(&TLV[6], Galaxy->aTrnsRef, sizeof(Galaxy->aTrnsRef));
	memcpy(c3->cUserData1, TLV, sizeof(TLV));
	
	
	// fin d'ajout


	//-----------------------
	// Fin Particularités DLP
	//-----------------------


	//Fin InitPersoSMED
}


void InitTransSMED(lpCcPosMsgSend Galaxy, struct tpvMessIn *c3)
{

	memcpy (c3->cOperation, "z", 1);
	memcpy (c3->cAmount, Galaxy->aAmount, 12); 
	memcpy (c3->cCurrencyCode, Galaxy->aCurrencyCode, 3);

	memcpy (c3->cTenderType, "+P", 2);	  			    //typage ppour déterminer la personnalisation ou la transaction
	memcpy (c3->cReaderType, Galaxy->aReadingMode, 1);	// Lecture effectuée par C3
	memcpy (c3->cCustomerPresent, Galaxy->aCustomerPresent, 1);	

	memset(c3->cTermNum, '0', sizeof(c3->cTermNum));
	memcpy (&c3->cTermNum[0], Galaxy->aCtrlNum, 4);     // n° de controleur
	memcpy (&c3->cTermNum[4], Galaxy->aRegister, 1);     // n° de register
	memcpy (&c3->cTermNum[5], Galaxy->aTermNum, 3);     // n° de terminal

	memset(c3->cCashNum, '0', sizeof(c3->cCashNum));    
	memcpy (&c3->cCashNum[1], Galaxy->aCashNum, 7);		// n° caissière

	memcpy (c3->cPan, Galaxy->aPanNum, sizeof(c3->cPan));

	//-------------------------
	// Début Particularités DLP
	//-------------------------
	memset(c3->cUserData1, '0', sizeof(c3->cUserData1));    
	memset(c3->cUserData2, '0', sizeof(c3->cUserData2));    



	//-----------------------
	// Fin Particularités DLP
	//-----------------------


	//Fin InitTransSMED
}

/////////////////////////////////////////
//    Point d'entrée de DlpRevelation. /
///////////////////////////////////////
extern "C" DLPCCDLL_API BOOL WINAPI Protocole_Revelation(lpCcRevMsgSend lpRevSend,
														 lpRevPosMsgRecv lpGalxRecv,
														 char * p1,
														 char * p2,
														 int (*pGetSecurity)(void), 
														 int (*pGetKey)(void) ,
														 int (*pGetStringFromKeyboard)(char *, int, char *) ,
														 void (*pPos_display)(char *, int, short) ) 

{

	char szC3Cfg[32];
	int    Ret;
	char *sztmp;

	memset (szC3Cfg,'\0',sizeof(szC3Cfg));
	sprintf (szC3Cfg, "%s.", "c3Config");
	bDebug = FALSE; 

	if (IsDebugActivated (szC3Cfg)) bDebug = TRUE;
	if (bDebug) Ret = MessageBox(NULL, (char *)lpRevSend, "Entrée dans Protocole_Revelation", MB_OK);


	bRev = true;
	memcpy(szNumTransRev,lpRevSend->rNumTransUnique, sizeof(lpRevSend->rNumTransUnique));

	CcPosMsgSend szGalxSend;
	CcPosMsgSend *lpszGalxSend = &szGalxSend;

	CcPosMsgRecv szGalxRecv;
	CcPosMsgRecv *lpszGalxRecv = &szGalxRecv;

	memcpy(lpszGalxSend, lpRevSend, sizeof(CcPosMsgSend));

	if (bDebug) Ret = MessageBox(NULL, (char *)lpszGalxSend, "Appel DLP_CC", MB_OK);

	Dlp_Cc(lpszGalxSend, lpszGalxRecv, p1, p2, pGetSecurity, pGetKey, pGetStringFromKeyboard, pPos_display);


	if (bDebug) Ret = MessageBox(NULL, (char *)lpszGalxRecv, "Retour DLP_CC", MB_OK);

	memcpy(lpGalxRecv, lpszGalxRecv, sizeof(CcPosMsgRecv));
	memset (lpGalxRecv->cFiller, 0x20, sizeof(lpGalxRecv->cFiller));

	if (bDebug) Ret = MessageBox(NULL, (char *)lpGalxRecv, "Sortie de Protocole_Revelation", MB_OK);


	if (IsProtocoleResa(szC3Cfg))
	{
		memset (p1, '0x20',1);
		memset (p2, '0x20',1);
	}

	return true;
}






extern DLPCCDLL_API void Test()
{
	MessageBox(NULL, " Test", "DLPCC" , 1);
	printf("Test");
}

void VersionNumber(struct tpvMessIn *c3)
{

	memset (c3, 0x0020, sizeof(c3));
	memcpy (c3->cOperation, "L", 1);
}	



int ReadTicket (char *fTicket, char *pTicket, struct tpvMessIn *c3, struct tpvMessOut *retourc3)
{
	FILE *fp;
	char line[WIDE_TICKET], Cible[12];
	int Ret, lg;

	if (pTicket == 0x00) 
	{
		if (bDebug)  Ret = MessageBox(NULL, "pointeur vide","Readticket", MB_OK);//SJ debug
		return FALSE;
	}

	memset (pTicket, '\0',1);
	memset(Cible, '\0', sizeof(Cible));
	strncpy (Cible, "ticket1.com",11);
	lg= strlen(Cible);

	fp = fopen(fTicket, "r");

	if (fp == 0x00) 
	{
		if (bDebug)  Ret = MessageBox(NULL, "Ouverture impossible","Readticket", MB_OK);//SJ debug
		return FALSE;
	}


	line[WIDE_TICKET - 1] = 0x00;
	while (fgets (line, sizeof(line), fp)) 
	{
		strncat(pTicket, line, sizeof(line) );
	}
	fclose(fp);

	memset(line, 0x00, WIDE_TICKET );
	sprintf(line, "lbr %.8s cm %.8s",&c3->cTermNum ,&c3->cCashNum );
	line[WIDE_TICKET - 1] = 0x00;
	strncat(pTicket,  line,  sizeof(line) ); 

	if (!memcmp(Cible ,fTicket , lg) )  
	{
		memset(line, 0x00, WIDE_TICKET );
		sprintf(line, "TKND# %.19s" ,&retourc3->cPan );
		line[WIDE_TICKET - 1] = 0x00;
		strncat(pTicket,  line,  sizeof(line) ); 
	}


	return TRUE;

}


int ReadTicketDLRP (char *fTicket, char *pTicket, CcPosMsgSend *lpGalxSend, CcPosMsgRecv *lpGalxRecv)
{
	FILE *fp;
	char line[WIDE_TICKET];
	char szline[128], Cible[12];
	double Mnt = 0;
	int pos,lg, len, lgCible;


	if (pTicket == 0x00) 
	{
		return FALSE;
	}

	memset (pTicket, '\0',1);
	memset(Cible, '\0', sizeof(Cible));
	strncpy (Cible, "ticket1.com",11);
	lgCible= strlen(Cible);
	fp = fopen(fTicket, "r");

	if (fp == 0x00) 
	{
		return FALSE;
	}


	line[WIDE_TICKET - 1] = 0x00;
	while (fgets (line, sizeof(line), fp)) 
	{
		for (lg = 0; line[lg] != '%' && lg < 125; lg++);

		if (lg < 124)
		{
			if (!memcmp("%horodatage%",&line[lg],12)) 
			{
				memcpy(&line[lg],"000000000000", 12);
				len = sprintf (line, "le : %2.2s/%2.2s/%4.4s a : %2.2s:%2.2s:%2.2s",
					szDate, szDate+2, szDate+4, szTime, szTime+2, szTime+4);
			}

			if (!memcmp("%transaction%",&line[lg],13)) 
			{
				memcpy(&line[lg],"             ", 13);
				memcpy(&line[lg], lpGalxSend->aTrnsNum, sizeof(lpGalxSend->aTrnsNum));
			}


			if (!memcmp("%pan%",&line[lg],5))
			{
				memcpy(&line[lg],"     ", 5);
				memcpy(&line[lg],lpGalxSend->aPanNum, sizeof(lpGalxSend->aPanNum));
			}

			if (!memcmp("%pan_partiel%",&line[lg],13))
			{
				memcpy(&line[lg],"             ", 13);
				memcpy(&line[lg],lpGalxSend->aPanNum, sizeof(lpGalxSend->aPanNum));
				memcpy(&line[lg+9],"*********", 9);
			}

			if (!memcmp("%date_valid%",&line[lg],12)) // + code service pour le ticket com
			{
				len=sprintf(line, " %4.4s  101\n", lpGalxSend->aEndValid);
			}


			if (!memcmp("%codeservice%",&line[lg],13)) 
			{
				memcpy(&line[lg],"             ", 13);
				memcpy(&line[lg],"101", 3);
			}


			if (!memcmp("%montant_euro%",&line[lg],14))  
			{ 
				Mnt=NATOF (lpGalxSend->aAmount, 12);
				len = sprintf (line, "  %0.2lf EUR\n", Mnt/=100);

			}

			if (!memcmp("%montant_frf%",&line[lg],13))  
			{
				Mnt=NATOF (lpGalxSend->aAmount, 12) * 6.55957;
				len = sprintf (line, "  %0.2lf FRF\n", Mnt/=100);

			}

		}

		strncat(pTicket, line, sizeof(line) );
	}
	fclose(fp);

	memset(line, 0x00, WIDE_TICKET );
	sprintf(line, "lbr %.3s cm %.7s",&lpGalxSend->aTermNum ,&lpGalxSend->aCashNum);
	line[WIDE_TICKET - 1] = 0x00;
	strncat(pTicket,  line,  sizeof(line) ); 

	if (!memcmp(Cible ,fTicket , lgCible) )  
	{
		memset(line, 0x00, WIDE_TICKET );
		sprintf(line, "lbr %.19s ",&lpGalxRecv->bPanNum);
		line[WIDE_TICKET - 1] = 0x00;
		strncat(pTicket,  line,  sizeof(line) ); 
	}

	return TRUE;

}


void FormateChaine(char szNb[12], long nb)
{
	int lg, decimal, signe;
	char *sztmp;


	sztmp = _fcvt(nb, 0, &decimal, &signe );
	lg = strlen(sztmp);

	memset(szNb,0x30,12);
	memcpy(&szNb[12-lg],sztmp,lg);

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

	if (bDebug) DLP_Pos_display(CTX_NAME, 0, 0); //SJ pour debug

	if ((fd = _open (CTX_NAME, _O_RDWR | _O_BINARY)) < 0)
	{
		if (bDebug) DLP_Pos_display("pas trouvé", 0, 0); //SJ pour debug
		return FALSE ;
	}
	else
	{
		if (bDebug) DLP_Pos_display("trouvé", 0, 0); //SJ pour debug
		_close (fd);
	}

	return TRUE;
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

	if (bDebug) DLP_Pos_display(CTX_NAME, 0, 0); //SJ pour debug

	memset(jjmmaaaa, 0x00, sizeof(jjmmaaaa));


	InitDuJour = false;

	if (!(fp = fopen (CTX_NAME,"r")))
	{
		if (bDebug) DLP_Pos_display("LastInit non trouvé", 0, 0); //SJ pour debug
		return FALSE ;
	}
	else
	{
		if (bDebug) DLP_Pos_display("LastInit trouvé", 0, 0); //SJ pour debug

		while ( fgets (line , sizeof line , fp) ) 
		{
			/* Appel fonction date systeme */
			time (&time_c);
			dat_j = localtime (&time_c);
			sprintf (jjmmaaaa, "%02d%02d%04d", dat_j->tm_mday, dat_j->tm_mon+1, dat_j->tm_year+1900);

			if (memcmp(jjmmaaaa,(char *) line,8) == 0)
			{ 
				InitDuJour = true;
				if (bDebug) DLP_Pos_display("Maj des tables", 0, 0); //SJ pour debug
			}
		}
		fclose (fp);
	}

	return InitDuJour;
}


void CheckContext(lpCcPosMsgSend lpGalxSend, struct tpvMessIn *c3, void (*pPos_display)(char *, int, short))
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
	static char   sztmp[5], szGen[32], szBnc[32], szAmex[32], szC3Cfg[32], szLastInit[32];
	int iCtrl;
	CcPosInitSend lpGalxInit;
	struct MsgToStratus s;
	int Ret;

	memset(sztmp,0x00, sizeof(sztmp));
	strncpy(sztmp,lpGalxSend->aCtrlNum,4);
	iCtrl = 0;

	if ( (lpGalxSend->aOperation[0] == 'D') || 
		(lpGalxSend->aOperation[0] == 'C') ||
		(lpGalxSend->aOperation[0] == 'M') ||
		(lpGalxSend->aOperation[0] == 'z') ) 

	{
		printf ("Recherche contextuelle \n");
		memset (szGen,'\0',sizeof(szGen));
		memset (szBnc,'\0',sizeof(szBnc));
		memset (szAmex,'\0',sizeof(szAmex));
		memset (szC3Cfg,'\0',sizeof(szC3Cfg));
		memset (szLastInit,'\0',sizeof(szLastInit));


		//sprintf (szGen, "%s.", "c3CtxGen");
		sprintf (szGen, "c3CtxGen.%-4.4d\0",atoi(sztmp) );
		sprintf (szBnc, "%s", "c3CtxBnc");
		sprintf (szAmex, "%s.", "c3CtxAmx");
		sprintf (szC3Cfg, "%s.", "c3Config");
		sprintf (szLastInit, "%s.", "LastInit");
		strncat (szAmex, lpGalxSend->aCtrlNum, sizeof(lpGalxSend->aCtrlNum)); // SJ, Add CB52
		strncat (szGen, lpGalxSend->aRegister, sizeof(lpGalxSend->aRegister)); // SJ, Add CB52
		strncat (szAmex, lpGalxSend->aRegister, sizeof(lpGalxSend->aRegister));  // SJ, Add CB52
		strncat (szGen, lpGalxSend->aTermNum, 3);
		strncat (szAmex, lpGalxSend->aTermNum, 3);

		if ( IsContextExist(szGen) == FALSE ||
			IsContextValid(szLastInit) == FALSE )
		{
			printf ("Rechargement du contexte, demande auto init \n");
			//DLP_Pos_display("Defaut de contexte, demande auto init", 0, 0); 
			pPos_display("Rechargement du contexte, demande auto init.", 0, 0);
			memset (&lpGalxInit, 0x20, sizeof lpGalxInit);	    
			memset (lpGalxInit.cCashNum, '0',sizeof(lpGalxInit.cCashNum)); //SJ, Add CB52
			memcpy (&lpGalxInit.cCashNum[1], lpGalxSend->aCashNum, sizeof(lpGalxInit.cCashNum)); //SJ Modif CB52
			memcpy (lpGalxInit.cTermNum, lpGalxSend->aTermNum , sizeof(lpGalxInit.cTermNum) );
			memcpy (lpGalxInit.cTermNum, lpGalxSend->aCtrlNum, sizeof(lpGalxSend->aCtrlNum) ); // Add SJ CB52 
			memcpy (&lpGalxInit.cTermNum[4], lpGalxSend->aRegister, sizeof(lpGalxSend->aRegister) ); // Add SJ CB52 
			memcpy (&lpGalxInit.cTermNum[5], lpGalxSend->aTermNum , sizeof(lpGalxSend->aTermNum) ); // Add SJ CB52 


			if (!GetTermFromON2 (&lpGalxInit,s))
			{
				//DLP_Pos_display("ATTENTION : Socket ON/2 non récupérée.", 0, 0); 
				pPos_display("ATTENTION : Socket ON/2 non récupérée.", 0, 0); 
			}

			//DLP_Pos_display("Init terminée", 0, 0); 
			pPos_display("Init terminée", 0, 0); 

			if (! CreateLastInit())
			{
				//DLP_Pos_display("Enregistrement Init terminé", 0, 0); 
				pPos_display("Enregistrement Init terminé ", 0, 0); 
			}

			InitCarte(&lpGalxInit, c3) ;

			if (bDebug) pPos_display("Type d'opération", 0, 0); //SJ pour debug
			if (bDebug) pPos_display((char *)c3->cOperation, 0, 0); //SJ pour debug
			if (bDebug) pPos_display("N° Terminal", 0, 0); //SJ pour debug
			if (bDebug) pPos_display((char *)c3->cTermNum, 0, 0); //SJ pour debugc3->cTermNum, 0, 0); //SJ pour debug

			 c3dll (c3, &out, DLP_GetSecurity, DLP_GetKey, DLP_GetStringFromKeyboard, pPos_display, DLP_PrintTicket);
			//  c3dll (c3, &out, pGetSecurity, pGetKey, pGetStringFromKeyboard, pPos_display);

			if (bDebug) Ret = MessageBox(NULL, (char *) &out, "Retour C3", MB_OK); //SJ debug

			//Test de la réponse Init.
			if (memcmp (out.cC3Error, "0000", 4) != 0)
			{
				printf ("Init Echouée, Destruction des fichiers contextuels \n");
				pPos_display("Init Echouée, Destruction des fichiers contextuels", 0, 0); //SJ pour debug

				if (remove(szGen) != 0 )
				{
					printf( "Destruction impossible '%s'\n", szGen );
					if (bDebug) pPos_display("Destruction impossible", 0, 0); //SJ pour debug
				}
				remove(szLastInit);
			}
			else //Init OK, on peut modifier dynamiquement le c3config
			{
				pPos_display("Init terminée", 0, 0); //SJ pour debug
				if (! CreateLastInit())
				{
					pPos_display("Enregistrement Init terminé", 0, 0); //SJ pour debug
				}
			}
		}
	}

}

void MajSocketInC3Config (char *fConfig, char *Socket)
{
	FILE *fp, *fpNew ;
	char line [128], lineNew[128], szBuf[50], Cible[10], NewC3Cfg[16];
	int  lg, ret ;
	char  devTcp [32], hostName [32];
	char szSoc[5];
	int short hostPort ;




	memset(Cible, '\0', sizeof(Cible));
	memset(szBuf, '\0', sizeof(szBuf));
	memset(NewC3Cfg, '\0', sizeof(NewC3Cfg));
	memset(szSoc, '\0', sizeof(szSoc));
	strncpy (szSoc, Socket,4);  


	strncpy (Cible, "AXIS_COM",8);
	sprintf (NewC3Cfg, "%s.", "./c3Config.new");

	lg= strlen(Cible);
	ret = -1 ;

	if( ! (fp = fopen(fConfig, "r" )))
		return  ;

	if( ! (fpNew = fopen(NewC3Cfg, "w" )))
		return  ;

	while ( fgets (line , sizeof line , fp) ) 
	{
		memset(lineNew,'\0', sizeof(lineNew));
		if (!memcmp(Cible ,line , lg) )  
		{
			/* line   ->  Interface:hostName:hostPort */
			strcpy(szBuf, line+lg+1);
			ret = sscanf(szBuf ,"%s %s %d", devTcp,hostName, & hostPort);
			sprintf (lineNew, "AXIS_COM=%s %s %s \n", devTcp, hostName, szSoc ); 

			if (hostPort == atoi(szSoc) ) //Rien à changer. On ferme et on s'en va !
			{
				printf ("No Change on C3Config \n");
				fclose(fp);
				fclose(fpNew);

				if (remove(NewC3Cfg) != 0 )
				{
					printf( "Destruction impossible '%s'\n", NewC3Cfg );
				}
				return;
			}
		}
		else
		{
			strncpy(lineNew, line, strlen(line));
		}
		fputs(lineNew, fpNew);
	}

	fclose(fp);
	fclose(fpNew);

	if (remove(fConfig) != 0 )
	{
		printf( "Destruction impossible '%s'\n", fConfig );
	}

	if ( rename(NewC3Cfg, fConfig ) != 0 )
	{
		printf( "Rename impossible '%s'\n", NewC3Cfg );
	}

	return ;
}

bool IsPosteVAD (char *fConfig)
{
	FILE *fp;
	char line [128], szBuf[50], Cible[15], sztmp[15];
	int  lg, ret ;
	char  TypeLecteur [32];


	memset(Cible, '\0', sizeof(Cible));
	memset(szBuf, '\0', sizeof(szBuf));
	memset(TypeLecteur, '\0', sizeof(TypeLecteur));
	memset(sztmp, '\0', sizeof(sztmp));



	strncpy (Cible, "LECTEUR_CB",10);
	strncpy (sztmp, "VAD",3);

	lg= strlen(Cible);
	ret = -1 ;

	if( ! (fp = fopen(fConfig, "r" )))
		return  false;

	while ( fgets (line , sizeof line , fp) ) 
	{
		if (!memcmp(Cible ,line , lg) )  
		{

			strcpy(szBuf, line+lg+1);
			ret = sscanf(szBuf ,"%s", TypeLecteur);
			if (!memcmp(TypeLecteur ,sztmp , strlen(TypeLecteur)) )  
			{
				return true;
			}
		}
	}

	fclose(fp);

	return false;
}


bool IsPostePKG (char *fConfig)
{
	FILE *fp;
	char line [128], szBuf[50], Cible[15], sztmp[15];
	int  lg, ret ;


	memset(Cible, '\0', sizeof(Cible));
	memset(szBuf, '\0', sizeof(szBuf));
	memset(sztmp, '\0', sizeof(sztmp));



	strncpy (Cible, "DLP_PKG",13);

	lg= strlen(Cible);
	ret = -1 ;

	if( ! (fp = fopen(fConfig, "r" )))
		return  false;

	while ( fgets (line , sizeof line , fp) ) 
	{
		if (!memcmp(Cible ,line , lg) )  
		{
			fclose(fp);
			return true; //DLP_PKG présent dans C3Config
		}
	}

	fclose(fp);

	return false;
}



bool IsDebugActivated (char *fConfig)
{
	FILE *fp;
	char line [128], szBuf[50], Cible[15], sztmp[15];
	int  lg, ret ;
	// char  TypeLecteur [32];


	memset(Cible, '\0', sizeof(Cible));
	memset(szBuf, '\0', sizeof(szBuf));
	memset(sztmp, '\0', sizeof(sztmp));



	strncpy (Cible, "DLP_DEBUG",9);

	lg= strlen(Cible);
	ret = -1 ;

	if( ! (fp = fopen(fConfig, "r" )))
		return  false;

	while ( fgets (line , sizeof line , fp) ) 
	{
		if (!memcmp(Cible ,line , lg) )  
		{
			fclose(fp);
			return true; //DLP_DEBUG présent dans C3Config
		}
	}

	fclose(fp);

	return false;
}


bool IsTicketActivated (char *fConfig)
{
	FILE *fp;
	char line [128], szBuf[50], Cible[15], sztmp[15];
	int  lg, ret ;


	memset(Cible, '\0', sizeof(Cible));
	memset(szBuf, '\0', sizeof(szBuf));
	memset(sztmp, '\0', sizeof(sztmp));



	strncpy (Cible, "DLP_TICKET",10);

	lg= strlen(Cible);
	ret = -1 ;

	if( ! (fp = fopen(fConfig, "r" )))
		return  false;

	while ( fgets (line , sizeof line , fp) ) 
	{
		if (!memcmp(Cible ,line , lg) )  
		{
			fclose(fp);
			return true; //DLP_TICKET présent dans C3Config
		}
	}

	fclose(fp);

	return false;
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


bool IsProtocoleResa (char *fConfig)
{
	FILE *fp;
	char line [128], szBuf[50], Cible[15], sztmp[15];
	int  lg, ret ;


	memset(Cible, '\0', sizeof(Cible));
	memset(szBuf, '\0', sizeof(szBuf));
	memset(sztmp, '\0', sizeof(sztmp));



	strncpy (Cible, "DLP_PROTOCOLE",13);

	lg= strlen(Cible);
	ret = -1 ;

	if( ! (fp = fopen(fConfig, "r" )))
		return  false;

	while ( fgets (line , sizeof line , fp) ) 
	{
		if (!memcmp(Cible ,line , lg) )  
		{
			fclose(fp);
			return true; //DLP_PROTOCOLE présent dans C3Config
		}
	}

	fclose(fp);

	return false;
}



bool IsPosteSTM (char *fConfig)
{
	FILE *fp;
	char line [128], szBuf[50], Cible[15], sztmp[15];
	int  lg, ret ;


	memset(Cible, '\0', sizeof(Cible));
	memset(szBuf, '\0', sizeof(szBuf));
	memset(sztmp, '\0', sizeof(sztmp));



	strncpy (Cible, "DLP_STM",13);

	lg= strlen(Cible);
	ret = -1 ;

	if( ! (fp = fopen(fConfig, "r" )))
		return  false;

	while ( fgets (line , sizeof line , fp) ) 
	{
		if (!memcmp(Cible ,line , lg) )  
		{
			fclose(fp);
			return true; //DLP_STM présent dans C3Config
		}
	}

	fclose(fp);

	return false;
}



bool TraiteVad(char *Msg, lpCcPosMsgSend Galaxy)
{
	HANDLE MHdle;
	LPVOID MPtr;
	char szRep[255], szCode[255], szBin[20], szMoisExp[3], szAnExp[3], szCvx2[4];
	int Ret, i, lg;

	memset(szRep,0x00,sizeof(szRep));
	memset(szCode,0x00,sizeof(szCode));
	memset(szBin,0x00,sizeof(szBin));
	memset(szMoisExp,0x00,sizeof(szMoisExp));
	memset(szAnExp,0x00,sizeof(szAnExp));
	memset(szCvx2,0x00,sizeof(szCvx2));
	i = 0;
	lg = 0;



	if (MPtr = CreateMappingFile() )
	{

		memset((char *) MPtr, '\0', sizeof (MPtr));	

		strcpy ((char *) MPtr, Msg);

		SendMessage (FindWindow(NULL,"monetique"), WM_USER+20, 0L, 0L);
		sprintf ((char *) MPtr, "                                     \0");
		DestroyMappingFile(MPtr);
	}



	MHdle = OpenFileMapping(FILE_MAP_READ | FILE_MAP_WRITE, TRUE, "Fic_Map");
	MPtr = MapViewOfFile(MHdle, FILE_MAP_WRITE | FILE_MAP_READ,0, 0, 128);


	while (TRUE)
	{
		if (memcmp("VADCANCEL",(char *)MPtr,9) == 0) //Bouton Cancel VAD
		{
			SendMessage (FindWindow(NULL,"monetique"), WM_USER+9, 0L, 0L);
			sprintf ((char *) MPtr, "                             \0");
			DestroyMappingFile(MPtr);
			return FALSE;
		}
		if (memcmp("VADACCEPT",(char *)MPtr,9) == 0) //Bouton OK VAD
		{
			Ret = sscanf((char *) MPtr,"%s %s %s %s %s", szRep, szBin, szMoisExp, szAnExp, szCvx2);
			SendMessage (FindWindow(NULL,"monetique"), WM_USER+21, 0L, 0L);
			sprintf ((char *) MPtr, "                             \0");
			DestroyMappingFile(MPtr);
			if (!bHis) memset(Galaxy->aCustomerPresent, '0', 1);
			memcpy (Galaxy->aPanNum, szBin, sizeof(Galaxy->aPanNum));
			memcpy (Galaxy->aEndValid, szMoisExp, 2 );
			memcpy (&Galaxy->aEndValid[2], szAnExp, 2 );
			memcpy (Galaxy->aCvv2, szCvx2, 3 );
			memset (Galaxy->aReadingMode, '1', 1 );

			memset (Galaxy->aPanNum,'?', sizeof(Galaxy->aPanNum));
			lg = sizeof(Galaxy->aPanNum) - strlen(szBin);
			memcpy (&Galaxy->aPanNum[lg],szBin, strlen(szBin));

			return TRUE;
		}
	}

	return FALSE;
}


bool TraitePkg(char *Msg, lpCcPosMsgSend Galaxy)
{
	HANDLE MHdle;
	LPVOID MPtr;
	char szRep[255], szCode[255], szBin[20], szMoisExp[3], szAnExp[3], szCvx2[4];
	int Ret, i, lg;


	i = 0;
	lg = 0;


	// Lecture de la piste par ordre c3 Ingénico (SJ 03-07-2015)
	//-------------------------------------------

	struct tpvMessIn in;
	struct tpvMessOut out;
	short Delim;
	char szDateExp[5], szMsg[255];
	bool fin;
	int j;

	memset(szRep,0x00,sizeof(szRep));
	memset(szCode,0x00,sizeof(szCode));
	memset(szBin,0x00,sizeof(szBin));
	memset(szMoisExp,0x00,sizeof(szMoisExp));
	memset(szAnExp,0x00,sizeof(szAnExp));
	memset(szCvx2,0x00,sizeof(szCvx2));
	memset(szMsg,0x20,sizeof(szMsg)); 

	//SendMessage (FindWindow(NULL,"monetique"), WM_USER+22, 0L, 0L);  //Affichage des champs de saisie carte
	InitLecturePiste (Galaxy, &in);
	//		   if (*pGetSecurity == NULL) *pGetSecurity = DLP_GetSecurity;
	//           if (*pGetKey == NULL) *pGetKey = DLP_GetKey;
	//           if (*pGetStringFromKeyboard == NULL) *pGetStringFromKeyboard = DLP_GetStringFromKeyboard;
	//           if (*pPos_display == NULL) *pPos_display = DLP_Pos_display;


	c3dll (&in, &out, DLP_GetSecurity, DLP_GetKey, DLP_GetStringFromKeyboard, DLP_Pos_display, DLP_PrintTicket);

	if (memcmp (out.cC3Error, "0000", 4) != 0)
	{
		Ret = PostMessage(FindWindow(NULL,"monetique"), WM_USER+7, 0, 0); //Traitement refusé.
		memset(szMsg,'\0',sizeof(szMsg));
		sprintf (szMsg, "Erreur Traitement c3 : cC3Error (%4.4s), cReponseCode (%4.4s)\n", out.cC3Error, out.cReponseCode);
		DLP_Pos_display (szMsg,0,0);
		//memcpy(lpGalxRecv->bResponseCode,"1",1);

		/*if (hMonetic != NULL) 
		{
		DLP_Pos_display("        ",0,0);	  
		DLP_Pos_display("- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -",0,0);	  
		DLP_Pos_display("           ***       CLIQUEZ SUR OK       ***",0,0);	  
		DLP_Pos_display("- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -",0,0);	  

		while (DLP_GetKey() == 0x00)
		{
		Sleep(500);
		}

		DLP_Pos_display("Fermeture de l'application",0,0);	

		Ret = PostMessage(hMonetic, WM_USER+9, 0, 0); //Fermer l'application Monétique.
		}*/
		return FALSE;
	}

	Delim=0; fin=false; j=0;

	memset (szBin, 0x00, sizeof(szBin));
	memset (szDateExp, 0x00, sizeof(szDateExp));


	for (i=0;i < (short)strlen(out.cIso2) && !fin ;i++) 
	{

		if (out.cIso2[i] == 0x3D ) // = 
			Delim ++;

		switch(Delim)
		{
		case 0 :
			if (out.cIso2[i] >= 0x30 && out.cIso2[i] <= 0x39)
			{
				strncat(szBin, &out.cIso2[i],1);
				j++;
			}
			break;

		case 1 :
			if (out.cIso2[i+1] >= 0x30 && out.cIso2[i+1] <= 0x39)
				strncat(szDateExp, &out.cIso2[i+1],1);

			if (out.cIso2[i+2] >= 0x30 && out.cIso2[i+2] <= 0x39)
				strncat(szDateExp, &out.cIso2[i+2],1);

			if (out.cIso2[i+3] >= 0x30 && out.cIso2[i+3] <= 0x39)
				strncat(szDateExp, &out.cIso2[i+3],1);

			if (out.cIso2[i+4] >= 0x30 && out.cIso2[i+4] <= 0x39)
				strncat(szDateExp, &out.cIso2[i+4],1);

			fin = true;

			break;

		}

	}



	memcpy (Galaxy->aOperation, "D",1);
	memset (Galaxy->aPanNum, 0x30, sizeof(Galaxy->aPanNum));

	if (j==19)
		memcpy (&Galaxy->aPanNum, szBin, sizeof(Galaxy->aPanNum));
	else
		memcpy (&Galaxy->aPanNum[sizeof(Galaxy->aPanNum)-j], szBin, strlen(szBin));

	memcpy (Galaxy->aEndValid, szDateExp, sizeof(Galaxy->aEndValid));


	if (!bHis) memset(Galaxy->aCustomerPresent, '0', 1);
	//memcpy (Galaxy->aPanNum, szBin, sizeof(Galaxy->aPanNum));
	//memcpy (Galaxy->aEndValid, szMoisExp, 2 );
	//memcpy (&Galaxy->aEndValid[2], szAnExp, 2 );
	memcpy (Galaxy->aCvv2, "000", 3 );
	memset (Galaxy->aReadingMode, '1', 1 );

	return true;



	// Fin Lecture de la piste par ordre c3 Ingénico (SJ 03-07-2015)
	//----------------------------------------------


	if (MPtr = CreateMappingFile() )
	{

		memset((char *) MPtr, '\0', sizeof (MPtr));	

		strcpy ((char *) MPtr, Msg);

		SendMessage (FindWindow(NULL,"monetique"), WM_USER+22, 0L, 0L);
		sprintf ((char *) MPtr, "                                     \0");
		DestroyMappingFile(MPtr);
	}



	MHdle = OpenFileMapping(FILE_MAP_READ | FILE_MAP_WRITE, TRUE, "Fic_Map");
	MPtr = MapViewOfFile(MHdle, FILE_MAP_WRITE | FILE_MAP_READ,0, 0, 128);

	DLP_Pos_display("Attente lecture piste", 0 ,0);

	while (TRUE)
	{
		if (memcmp("PKGCANCEL",(char *)MPtr,9) == 0) //Bouton Cancel PKG
		{
			SendMessage (FindWindow(NULL,"monetique"), WM_USER+9, 0L, 0L);
			sprintf ((char *) MPtr, "                             \0");
			DestroyMappingFile(MPtr);
			return FALSE;
		}
		if (memcmp("PKGACCEPT",(char *)MPtr,9) == 0) //Piste lue onIsoChange
		{
			Sleep(200); //500
			Ret = sscanf((char *) MPtr,"%s %s %s %s %s", szRep, szBin, szMoisExp, szAnExp, szCvx2);
			//SendMessage (FindWindow(NULL,"monetique"), WM_USER+24, 0L, 0L);
			sprintf ((char *) MPtr, "                             \0");
			DestroyMappingFile(MPtr);
			if (!bHis) memset(Galaxy->aCustomerPresent, '0', 1);
			memcpy (Galaxy->aPanNum, szBin, sizeof(Galaxy->aPanNum));
			memcpy (Galaxy->aEndValid, szMoisExp, 2 );
			memcpy (&Galaxy->aEndValid[2], szAnExp, 2 );
			memcpy (Galaxy->aCvv2, szCvx2, 3 );
			memset (Galaxy->aReadingMode, '1', 1 );

			memset (Galaxy->aPanNum,'0', sizeof(Galaxy->aPanNum));
			lg = sizeof(Galaxy->aPanNum) - strlen(szBin);
			memcpy (&Galaxy->aPanNum[lg],szBin, strlen(szBin));

			return TRUE;
		}
	}

	return FALSE;
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


	if (bDebug) DLP_Pos_display(szName, 0, 0); //SJ pour debug

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


/*** Close *****************************************************************/
/*                                                                         */
/*   Fermeture du socket                                                   */
/*                                                                         */
/*                                                                         */
/*                                                                         */
/***************************************************************************/
bool Close_IP()
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
		DLP_Pos_display("StrSend : error lpData == NULL",30,0);
		return FALSE;
	}

	// Le socket doit être ouvert
	if (m_hSocket == INVALID_SOCKET)
	{
		DLP_Pos_display("erreur StrSend : la socket doit etre ouverte",37,0);
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
		DLP_Pos_display("err sur Receive : lpData == NULL",0,0);
		return FALSE;
	}

	// Attendre le message

	char szMsg[100];


	if(!StrReceive(SOCKON2_TIMEOUT_ACKNOWLEDGE,lpData, dwLenData))
	{
		DLP_Pos_display("Receive : erreur pendant la réception du message",0,0);
		return FALSE;
	}

	memcpy(szMsg,lpData,strlen(lpData));



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
		DLP_Pos_display("StrReceive : error lpData == NULL",33,0);
		return FALSE;
	}

	// Le socket doit être ouvert
	if (m_hSocket == INVALID_SOCKET)
	{
		DLP_Pos_display("erreur StrReceive : la socket doit etre ouverte",40,0);
		return FALSE;
	}

	longueur=0;
	DWORD argp;
	// Lire les données en plusieurs passes si elles ne sont pas immédiatement disponibles

	Sleep(300); //500 On commence par temporiser (le process de Philippe est Corse !!)
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
				DLP_Pos_display("StrReceive : error no incoming data",35,0);
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


int GetTermFromON2(lpCcPosInitSend Galaxy, MsgToStratus s)
	///////////////////////////////////////////////////////////////////////////////////////////
	//   GetTermFromON2 : Ouvre la communication vers ON/2 et récupère la socket de travail
	//                  OUI : L'opération s'est déroulée correctement.
	//                  NON : Problème.

	//  SJU, 07/07/2006.
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

	BOOL fFinished = FALSE; 

	static char   sztmp[4], szC3cfg[16], szSocket[5];


	memset (szC3cfg,'\0',sizeof(szC3cfg));
	memset(ConfigSocket,'\0',sizeof(ConfigSocket));
	memset(ConfigHost,'\0',sizeof(ConfigHost));
	memset(szBuf, '\0', sizeof(szBuf));


	sprintf (szC3cfg, "%s.", "./C3Config");



	ret = -1 ;

	// Initialiser la DLL WinSock lors de la première ouverture d'une socket
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


	/* La socket ne doit pas être déjà ouverte */
	if (m_hSocket == INVALID_SOCKET)
	{
		Close_IP();
	}

	GetAxisCom(szC3cfg);


	wRemotePort=ConfigSocket; 


	//Préparation de la structure du message init à envoyer à ON/2
	memset (&s, 0x20, sizeof s);

	//Renseignement de la structure de la demande d'initialisation
	s.sTypeOperation[0]='A';
	s.sType_init[0]='H';
	strncpy ( s.sTermNum, Galaxy->cTermNum, 8 );

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
		Close_IP();
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
	//if(!Receive(dwTimeout, recv_init, sizeof(HcRepInit),0))
	if(!Receive(dwTimeout, HcRepInit, sizeof(HcRepInit),0))
	{
		Abort();
		return FALSE;
	} 


	struct MsgFromStratus  rs_init;

	memset (&rs_init, 0x20, sizeof rs_init);
	memcpy((char*)&rs_init,HcRepInit,sizeof(HcRepInit));

	Close_IP();

	if (memcmp(rs_init.riTypeOperation, "F", 1) ==0)
	{
		DLP_Pos_display("ERREUR : Récupération de socket échouée", 0, 0); 
		return false;
	}
	else
	{
		if (isdigit(rs_init.riSocketCB[0]) && isdigit(rs_init.riSocketCB[1]) &&
			isdigit(rs_init.riSocketCB[2]) && isdigit(rs_init.riSocketCB[3]) )
		{
			if (memcmp(rs_init.riSocketCB, wRemotePort, 4) != 0)
				MajSocketInC3Config(szC3cfg, rs_init.riSocketCB);
		}
		else
		{
			DLP_Pos_display("ERREUR : Socket non numérique", 0, 0); 
			return false;
		}

	}

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


bool GetAxisCom(char *fConfig)
	///////////////////////////////////////////////////////////////////////////////////////////
	//   GetAxisCom : Ouvre le fichier C3Config et récupère le host et la socket de connexion
	//                  OUI : L'opération s'est déroulée correctement.
	//                  NON : Problème.

	//  SJU, 07/07/2006.
	///////////////////////////////////////////////////////////////////////////////////////////
{
	FILE *fp;
	char line [128], szBuf[50], Cible[12];
	char devTcp [32];
	int  lg, ret;


	memset(Cible, '\0', sizeof(Cible));
	memset(szBuf, '\0', sizeof(szBuf));

	strncpy (Cible, "AXIS_COM",11);

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


/***************************************************************************/
/*   CONVERSION D'UNE CHAINE EN UN FLOTANT (double)                        */
/***************************************************************************/
double NATOF (char *num, int n)
{
	double ret = 0;

	while (--n >= 0)
	{
		if (*num >= 0x30  &&  *num <= 0x39)
			ret = ret * 10 + *num -0x30;
		num++;
	}

	return ret;
}


/***************************************************************************/
/*   RECUPERE LA DATE COURANTE (format jjmmaaaa)                           */
/***************************************************************************/
void dateL (char *jjmmaaaa)
{
	time_t time_c;
	struct tm *dat_j;

	/* Appel fonction date systeme */
	time (&time_c);
	dat_j = localtime (&time_c);
	sprintf (jjmmaaaa, "%02d%02d%04d", dat_j->tm_mday, dat_j->tm_mon+1, dat_j->tm_year+1900);
}

/***************************************************************************/
/*   RECUPERE L'HEURE COURANTE (format hhmmss)                             */
/***************************************************************************/
void timeL (char *hhmmss)
{
	time_t time_c;
	struct tm *dat_j;

	/* Appel fonction date systeme */
	time (&time_c);
	dat_j = localtime (&time_c);
	sprintf (hhmmss, "%02d%02d%02d", dat_j->tm_hour, dat_j->tm_min, dat_j->tm_sec);
}


// permet l'annulation des cartes dans la base de données smocs
DLPCCDLL_API int WINAPI Dlp_His_Canceled_Card(char trame[409], char NumFolio[9]){
	
	char szC3Cfg[9];
	char Response[34];
	char HCid[15], Tag[4];
	char HCtosend[8];
	char errorMsg[255];
	int tagpresent = 0;
	memset(errorMsg, 0x00, sizeof(errorMsg)); 
	memset (szC3Cfg,'\0',sizeof(szC3Cfg));
	sprintf (szC3Cfg, "%s.", "c3Config");
	
	

	// particularitée envoi de HC à SMOCS
	FILE *fp;
	char line [128], Cible[15], sztmp[15];
	int  lg, ret ;
	// char  TypeLecteur [32];


	memset(Cible, '\0', sizeof(Cible));
	memset(sztmp, '\0', sizeof(sztmp));


	strncpy (Cible, "DLP_TAG:",8);

	lg= strlen(Cible);
	ret = -1 ;

	if( ! (fp = fopen("c3Config", "r" )))
		return -1;

	while ( fgets (line , sizeof line , fp) ) 
	{
		if (!memcmp(Cible ,line , lg) )  
		{
			memcpy(Tag, &line[8] , 4);
			tagpresent = 1;
		}
	}

	fclose(fp);

	// si le tag n'est pas present dans le c3config
	if (tagpresent == 0){
		sprintf(errorMsg,"DLP_TAG absent du c3Config");
		MessageBox(0, errorMsg, "Error", 0);
	}

	// On place le numero de folio dans la trame qui doit être envoyée
	memcpy(HCid, Tag, sizeof(Tag));
	memcpy(&HCid[4], "09", 2);
	memcpy(&HCid[6], NumFolio, 9);
	memcpy(&trame[295], HCid, 15);

	// fin particulartitée 

	// Initialiser la DLL WinSock lors de la première ouverture d'une socket
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

	if (!m_bStartedUp) return -1;


	/* La socket ne doit pas être déjà ouverte */
	if (m_hSocket == INVALID_SOCKET)
	{
		Close_IP();
	}

	GetAxisCom(szC3Cfg);


	wRemotePort=ConfigSocket;

	//ouverture du socket récupérer dans le c3config
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
		Close_IP();
		sprintf(errorMsg,"Connexion a la socket %s impossible",ConfigSocket);
		MessageBox(0, errorMsg, "Error", 0);
		return -1;

	}

	/* Connecter le socket à la machine distante */
	if (::connect(m_hSocket, (LPSOCKADDR)&addrRemote, sizeof(addrRemote)) != 0)
	{
		::closesocket(m_hSocket);
		m_hSocket = INVALID_SOCKET;
		sprintf(errorMsg,"Connexion au stratus impossible",ConfigSocket);
		MessageBox(0, errorMsg, "Error", 0);
		return -1;

	}

	/* Passer le socket en mode non-bloquant */
	if (::ioctlsocket(m_hSocket, FIONBIO, &dwNonBlockingMode) != 0)
	{
		::closesocket(m_hSocket);
		m_hSocket = INVALID_SOCKET;
		return -1;

	}

	// envoi de la trame avec les données reçues de ITIS88
	if(!StrSend(trame, 409))
	{
		Abort();
		sprintf(errorMsg,"Erreur lors de l'envoi des donnees au serveur");
		MessageBox(0, errorMsg, "Error", 0);
		::closesocket(m_hSocket);
		m_hSocket = INVALID_SOCKET;
		return -1;
	}

	memset(Response,0x00,sizeof(Response));

	//réception de la réponse
	if(!Receive(dwTimeout, Response, sizeof(Response),0))
	{
		Abort();
		sprintf(errorMsg,"Erreur lors de la reception des donnees du serveur");
		MessageBox(0, errorMsg, "Error", 0);
		return -1;
	} 

	Close_IP();
	if( memcmp(&Response[30], "00", 2) == 0){
		return 0;
	}
	else
	{
		return 99;
	}
	

	
}