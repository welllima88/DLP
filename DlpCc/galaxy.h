/*****
* DlpCc - Inteface Galaxy - C3
* Header commun privé des messages avec GALAXY
* SJU - 26 avril 2001
*****
* Pos.h
*****/

#define DLPCCDLL_API __declspec(dllexport)
#define WINAPI       __stdcall




#define SOCKON2_HEADER_FORMAT			"%c%04u%04u"
#define SOCKON2_HEADER_LEN				9
#define SOCKON2_HEADERTYPE_CONVERSATION	'C'
#define SOCKON2_HEADERTYPE_ACKNOWLEDGE	'A'
#define SOCKON2_HEADERTYPE_KEEPALIVE	'K'
#define SOCKON2_HEADERNUMBER_MAX		9999
#define SOCKON2_HEADERLENGTH_MAX		9999
#define SOCKON2_TIMEOUT_ACKNOWLEDGE		10000
#define SOCKON2_TIMEOUT_DISCARD			1000
#define SOCKON2_DISCARDBUFFER_LEN		64



/*****
* Structures
*****/

typedef struct
{
	char    cTermNum[8];  // SJ Modif CB52 de 3 à 8
    char    cCashNum[8];  // SJ Modif CB52 de 7 à 8
} CcPosInitSend, FAR* lpCcPosInitSend;


typedef struct
{
	char    aOperation[1];
	char    aAmount[12];
	char    aCurrencyCode[3];
	char    aCtrlNum[4];
	char    aRegister[1];
	char    aTermNum[3];
    char    aCashNum[7];
	char    aTrnsNum[6];
	char    aTrnsRef[10];
	char    aCustomerPresent[1];
	char    aReadingMode[1];
	char    aPanNum[19];
	char    aEndValid[4];
	char    aCvv2[3];
} CcPosMsgSend, FAR* lpCcPosMsgSend;

typedef struct
{
	char    aOperation[1];
} CcTest, FAR* lpCcTest;


typedef struct
{
	char    bResponseCode[1];
	char    bExplanation[18];
	char    bPanNum[19];
	char    bBbank[2];
	char    bSignature[1];
} CcPosMsgRecv, FAR* lpCcPosMsgRecv;


typedef struct
{
	char    aOperation[1];
	char    aAmount[12];
	char    aCurrencyCode[3];
	char    aCtrlNum[4];
	char    aRegister[1];
	char    aTermNum[3];
    char    aCashNum[7];
	char    aTrnsNum[6];
	char    aCustomerPresent[1];
	char    aChargeNum[9];
} HcPosMsgSend, FAR* lpHcPosMsgSend;


typedef struct
{
	char    bResponseCode[1];
	char    bExplanation[18];
} HcPosMsgRecv, FAR* lpHcPosMsgRecv;


typedef struct
{
	char    bResponseCode[1];
	char    bExplanation[18];
	char    bPanNum[19];
	char    bNumAuto [9];
	char    bBbank[2];
	char    bSignature[1];
} StmPosMsgRecv, FAR* lpStmPosMsgRecv;



struct MsgToStratus
{
	char    sTypeOperation[1];	
	char    sTermNum[8];
	char    sType_init[1];
} ;


struct MsgFromStratus
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
	char    rOperation[1];
	char    rAmount[12];
	char    rCurrencyCode[3];
	char    rCtrlNum[4];
	char    rRegister[1];
	char    rTermNum[3];
    char    rCashNum[7];
	char    rTrnsNum[6];
	char    rTrnsRef[10];
	char    rCustomerPresent[1];
	char    rReadingMode[1];
	char    rPanNum[19];
	char    rEndValid[4];
	char    rCvv2[3];
	char    rNumTransUnique[8];
	char    rFiller[20];
} CcRevMsgSend, FAR* lpCcRevMsgSend;



typedef struct
{
	char    cResponseCode[1];
	char    cExplanation[18];
	char    cPanNum[19];
	char    cBbank[2];
	char    cSignature[1];
    char    cFiller[20];
} RevPosMsgRecv, FAR* lpRevPosMsgRecv;



/*****
* Fonctions
*****/

#ifdef __cplusplus
extern "C" {
#endif



/* Point d'entrée de l'API DlpCc 
 #ifdef __cplusplus
   DLPCCDLL_API BOOL WINAPI DLP_CC(lpCcPosMsgSend,
								   lpCcPosMsgRecv,
								   char *,
								   char *,
								   int  (*pGetSecurity)(void),
								   int  (*pGetKey)(void),
								   int  (*pGetStringFromKeyboard)(char *, int, char *),
								   void (*pPos_display)(char *, int, short) );
 #else
   DLPCCDLL_API BOOL WINAPI DLP_CC(lpCcPosMsgSend,
								   lpCcPosMsgRecv,
								   char *,
								   char *,
								   int  (*)(),
								   int  (*)(),
								   int  (*)(),
								   void (*)() );
 #endif

*/
#ifdef __cplusplus
}
#endif


