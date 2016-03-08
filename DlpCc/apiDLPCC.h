/*****
* STM - Station de Travail Monétique
* API STM - Utilitaires STM
* Couche API exportée par la DLL
* Portage DLL Windows 95 - Patrice BURIEZ (PBu) - Février 1998
*****
* apiSTM.h
*****/

#ifndef __APIDLP_H__
#define __APIDLP_H__


/*****
* Constantes
*****/

//SJ début pour le type de banque, en attendant mieux !
#define DLP_FRANCE                  "FR"
#define DLP_ETRANGERE               "ET"
#define DLP_AMEX                    "AM"
#define DLP_DINERS                  "DC"
#define DLP_JCB                     "JC"
//SJ Fin pour le type de banque.

#define STM_SYSTEM_MAX				9999
#define STM_TERMINAL_MAX			9999
#define STM_TRANSACTION_MAX			9999
#define STM_CURRENCY_MAX			999
#define STM_AMOUNT_MAX				99999999

#define STM_CARDTRACK2DATA_SIZE		(40 + 1)
#define STM_CARDCERTIFICATE_SIZE	(8 + 1)
#define STM_CARDSTATUS_TRACK		0
#define STM_CARDSTATUS_CHIP			1
#define STM_CARDSTATUS_PHONE		2
#define STM_CARDSTATUS_ALMOSTFULL	4
#define STM_CARDSTATUS_FULL_TRANSAC	5
#define STM_CARDSTATUS_FULL_PIN		8
#define STM_CARDSTATUS_VPP_FAILURE1	0xE
#define STM_CARDSTATUS_VPP_FAILUREN	0xF
#define STM_CARDSTATUS_MAX			0xF
#define STM_CARDCALL_NONE			0
#define STM_CARDCALL_LIMIT_REACHED	1
#define STM_CARDCALL_FULL_TRANSAC	3
#define STM_CARDCALL_FULL_PIN		4
#define STM_CARDCALL_MAX			4

#define STM_MERCHANT_SIZE			(13 + 1)
#define STM_APPROVALCODE_SIZE		(6 + 1)

/*****
* Structures
*****/

typedef struct
{
	WORD	wYear;
	BYTE	byMonth;
	BYTE	byDay;
	BYTE	byHour;
	BYTE	byMin;
	BYTE	abyAlign[2];
} DLP_DATETIME, FAR* DLP_LPDATETIME;

/*****
* Fonctions
*****/

LPVOID CreateMappingFile();
void DestroyMappingFile(LPVOID MPtr); 
int DLP_GetSecurity();
int DLP_GetKey(); 
int DLP_GetStringFromKeyboard (char *stringEntered, int MaxlentOfStringEntered, char *messageToDisplay);
void DLP_Pos_display (char * messageToDisplay, int lentOfMessageToDisplay, short confirm);
void InitDebit(lpCcPosMsgSend Galaxy, struct tpvMessIn *c3);
void InitCredit(lpCcPosMsgSend Galaxy, struct tpvMessIn *c3);
void InitPreAuto(lpCcPosMsgSend Galaxy, struct tpvMessIn *c3);
void InitCarte(lpCcPosInitSend Galaxy, struct tpvMessIn *c3);
void InitVersion(lpCcPosInitSend Galaxy, struct tpvMessIn *c3);
void InitLecture(lpCcPosMsgSend Galaxy, struct tpvMessIn *c3);
void InitC3Admin(lpCcPosInitSend Galaxy, struct tpvMessIn *c3);
void InitDuplicata(lpCcPosMsgSend Galaxy, struct tpvMessIn *c3);
void InitLecturePiste(lpCcPosMsgSend Galaxy, struct tpvMessIn *c3);
void InitGetStrFrPin(lpCcPosMsgSend Galaxy, struct tpvMessIn *c3);
void InitPersoSMED(lpCcPosMsgSend Galaxy, struct tpvMessIn *c3);
void InitTransSMED(lpCcPosMsgSend Galaxy, struct tpvMessIn *c3);
void VersionNumber(struct tpvMessIn *c3);
int ReadTicket (char *fTicket, char *pTicket, struct tpvMessIn *c3, struct tpvMessOut *retourc3);
int ReadTicketDLRP (char *fTicket, char *pTicket, CcPosMsgSend *lpGalxSend, CcPosMsgRecv *lpGalxRecv);
void FormateChaine(char szNb[12], long nb);
int IsContextExist (char *CTX_NAME);
int IsContextValid (char *CTX_NAME);
//void CheckContext(lpCcPosMsgSend Galaxy, struct tpvMessIn *c3);
void CheckContext(lpCcPosMsgSend lpGalxSend, struct tpvMessIn *c3, void (*pPos_display)(char *, int, short));
void MajSocketInC3Config (char *fConfig, char *Socket);
bool IsPosteVAD (char *fConfig);
bool IsProtocoleResa  (char *fConfig);
bool IsPosteSTM (char *fConfig);
bool IsPostePKG (char *fConfig);
bool IsDebugActivated (char *fConfig);
bool IsTicketActivated (char *fConfig);
bool IsForceMonetic (char *fConfig);
bool TraiteVad(char *Msg, lpCcPosMsgSend Galaxy);
bool TraitePkg(char *Msg, lpCcPosMsgSend Galaxy);
int  CreateLastInit();
bool Close_IP();
bool Abort();
bool Discard(DWORD);
bool Send(LPCSTR, DWORD,int);
bool GetAxisCom(char *fConfig);
bool StrSend(LPCSTR, DWORD);
bool Receive(DWORD,LPSTR,DWORD,int);
bool StrReceive(DWORD,LPSTR,DWORD);
int GetTermFromON2 (lpCcPosInitSend Galaxy,MsgToStratus s);
WORD GetNextMessageNumber();
bool DecodeHeader(LPCSTR,char&,WORD&,DWORD&);
bool BuildHeader(LPSTR,char,WORD,DWORD);
double NATOF (char *num, int n);
void dateL (char *jjmmaaaa);
void timeL (char *hhmmss);
bool LectureCarte(char *Msg, lpCcPosMsgSend Galaxy);


extern "C" DLPCCDLL_API void Test(void);

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
}
#endif


/* Lecture de la date et de l'heure système */
DLPCCDLL_API BOOL WINAPI DLP_GetDateTime(DLP_LPDATETIME lpDateTime);

/* Point d'entrée STM */
DLPCCDLL_API BOOL WINAPI Dlp_Stm(lpCcPosMsgSend lpGalxSend,lpCcPosMsgRecv lpGalxRecv) ;

/* Point d'entrée HIS */
DLPCCDLL_API BOOL WINAPI Dlp_His(lpCcPosMsgSend lpGalxSend,lpCcPosMsgRecv lpGalxRecv) ;

DLPCCDLL_API BOOL WINAPI Dlp_His_Canceled_Card(char trame[], char numfolio[]) ;

/* Point d'entrée Resa */
//DLPCCDLL_API BOOL WINAPI Protocole_Revelation(lpCcRevMsgSend lpRevSend,lpCcPosMsgRecv lpGalxRecv) ;

#endif /* __APIDLP_H__ */

