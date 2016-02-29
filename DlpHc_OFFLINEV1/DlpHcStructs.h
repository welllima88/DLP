#include "stdafx.h"
#include "apiDLPc3.h"
#define DLPHCDLL_API __declspec(dllexport)

struct S1	/* message envoyé au POS */
{
	char aOperation;
	char aAmount[12];
	char aCurrencyCode[3];
	char aCtrlNum[4];
	char aRegister;
	char aTermNum[3];
	char aCashNum[7];
	char aTrnsNum[6];
	char aCustomerPresent;
	char aChargeNum[9];		/*char aHotelCode[2];char aFolioNumber[7];*/
};

struct S2	/* message retourné au POS */
{
	char bResponseCode;
	char bExplanation[18];
};

struct S3	/* message envoyé au stratus */
{
	char CreditPlan;			/* blank */
	char SourceTerminal[2];		/* = aTermNum */
	char DestinationTerminal[2];/* blank */
	char CreditFlag[3];			/* blank */
	char SequenceNumber[2];		/* blank */
	char FS2;					/* Hex'29' */
	char ForcePostAuthCode[6];	/* blank of returned value from GetSecurity() */
	char TerminalIdentification;/* blank */
	char SystemNumber[4];		/* = aCtrlNum */
	char LocationNumber[4];		/* filled with 0 */
	char HotelCode[2];			/* returned value from GetNumHc() */
	char GuestCheck[6];			/* filled with 0 */
	char AmountOfCharges[10];	/* = aAmount */
	char TransactionNumber[4];	/* = aTrnsNum */
	char ForcePostFlag;			/* Code : '0' or '1' or '2' */
	char SLDNumber[4];			/* filled with 0 */
	char RoomNumber[5];			/* blank */
	char GuestID[2];			/* blank */
	char LineNumber[2];			/* blank */
	char FolioNumber[7];		/* = aChargeNum */
	char Filler1[4];				/* blank */
	char CurrencyCode[3];		/* 250 or 978 */
	char Filler2[7];			    /* blank */
	char NumberOfPersons[3];	/* filled with 0 */
	char MenuNumber[2];			/* filled with 0 */
	char CashierNumber[3];		/* = aCashNum[1-3] */
	char ServerNumber[3];		/* = aCashNum[3-6] */
	char ChargeTipAmount[10];	/* filled with 0 */
	char TaxAmount[10];			/* filled with 0 */
	char Itemizer1[10];			/* filled with 0 */
	char Itemizer2[10];			/* filled with 0 */
	char Itemizer3[10];			/* filled with 0 */
	char Itemizer4[10];			/* filled with 0 */
	char DiscountItemizer1[10];	/* filled with 0 */
	char DiscountItemizer2[10];	/* filled with 0 */
	char EndOfMessage;			/* Hex'0D' */
};
	
struct S4	/* message retourné par le stratus */
{
	char rCreditPlan;				/* field #1 of the request */
	char rSourceTerminal[2];		/* field #3 of the request */
	char rDestinationTerminal[2];	/* field #2 of the request */
	char rCreditFlag[3];			/* field #4 of the request */
	char rSequenceNumber[2];		/* field #5 of the request */
	char rStatusCode;				/* details in Credit Response */
	char rFS2;						/* Hex'29' */
	char rMessageType;				/* '2' ou '4' */
	char rFiller[4];				/* blank */
	char rTerminalIdentification;	/* field #8 of the request */
	char rMessageIndicator;			/* '1' */
	char rCRTResponseMessage[2];	/* details in Credit Response */
	char rFolioNumber[7];			/* field #20 of the request */
	char rTransactionNumbre[5];		/* field #14 of the request */
	char rResponseMessageText[18];	/* details in Credit Response */
	char rFiller_2[12];				/* blank */
	char rFolioNumberPacked[4];		/* field #20 of the request, packed decimal */
	char rAmount[5];				/* field #13 of the request */
	char rEndOfMessage;				/* Hex'0D' */
};


struct S5	/* message envoyé par Revelation */
{
	char rOperation;
	char rAmount[12];
	char rCurrencyCode[3];
	char rCtrlNum[4];
	char rRegister;
	char rTermNum[3];
	char rCashNum[7];
	char rTrnsNum[6];
	char rCustomerPresent;
	char rChargeNum[9];		/*char aHotelCode[2];char aFolioNumber[7];*/
	char rNumTransUnique[8];
	char rItem1[10];
	char rItem2[10];
	char rItem3[10];
	char rItem4[10];
	char rFiller1[20];
	char rFiller2[20];
	char rFiller3[20];
};


struct S6	/* message EncoDeco */
{
	char cOperation;
	char cInfoAller[512];
	char cInfoRetour[512];
	int  cCodeRetour;
	char cfiller[128];

};	


struct MsgToStratus /* Msg d'init dll -> on/2) */
{
	char    sTypeOperation[1];	
//	4.3  	char    sTermNum[3];
//	4.3     char    sType_init[1];
	char    sCtrlNum[4];
    char    sRegister[1];
	char    sTermNum[3];  // 4.3
	char    sType_init[1]; // 4.3
	
    
} ;

struct MsgFromStratus  /* Msg d'init on/2 -> dll) */
{
	char    riTypeOperation[1];	
	char    riNumCom[10];
	char    riNumTerm[3];
	char    riNom[16];
    char    riAdresse[24];
	char    riType_init[1];
	char    riSocket[4];
	char    riSocketCB[4];
	char    riForcage[1];
	char    riTypeAnnulation[1];
    char    riDleEtx[2];
} ;

typedef struct
{
	char    cTermNum[8];  // SJ Modif CB52 de 3 à 8
    char    cCashNum[8];  // SJ Modif CB52 de 7 à 8
} CcPosInitSend, FAR* lpCcPosInitSend;

LPVOID CreateMappingFile();
void DestroyMappingFile(LPVOID MPtr); 
int DLP_GetSecurity();
int DLP_GetKey(); 
int DLP_GetStringFromKeyboard (char *stringEntered, int MaxlentOfStringEntered, char *messageToDisplay);
void DLP_Pos_display (char * messageToDisplay, int lentOfMessageToDisplay, short confirm);
bool GetHCIDFromTPE(S1* msg, int ordre);
bool GetHCIDFromTPERevelation(S5* msgsend, int ordre);
void InitLecture(lpCcPosInitSend Revelation, struct tpvMessIn *c3);
void InitC3Admin(lpCcPosInitSend Revelation, struct tpvMessIn *c3);
BOOL CheckContext(S1* msg_test,MsgToStratus s);
int IsContextExist (char *CTX_NAME);
int DLP_GetSecurity();
int DLP_GetKey(); 
int DLP_GetStringFromKeyboard (char *stringEntered, int MaxlentOfStringEntered, char *messageToDisplay);
void DLP_Pos_display (char * messageToDisplay, int lentOfMessageToDisplay, short confirm);
void CloseMonetic(int Etat);
bool Chiffre(char *EnClair, char *EnChiffre); 
bool Dechiffre(char *EnClair, char *EnChiffre); 
void InitTransSMEDRevelation(S5* Revelation, struct tpvMessIn *c3);
void InitTransSMED(S1* Galaxy, struct tpvMessIn *c3);
int IsContextValid (char *CTX_NAME);
int todigit(char* stringdigit);
int GetTermFromON2(lpCcPosInitSend Galaxy, MsgToStratus s);
bool Close_IP();
bool GetAxisCom(char *fConfig);
void MajSocketInC3Config (char *fConfig, char *Socket);
int todigit(char* stringdigit, int size);
bool IsForceMonetic (char *fConfig);