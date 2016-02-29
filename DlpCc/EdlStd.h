
/***************************************************************************/
/*  EdlStd.h             Fichier Header edl-std                            */
/*  (c)   copyright 26/01/2007                                             */
/*        DLRP                                                             */
/*                                                                         */
/***************************************************************************/




/* ----------- dll / ON/2  interface  ----------------------- */


/*  messages  operationels    */

struct EdlStdIn {
   char IcaCreditPlan    [1]; /*   0  */
   char IcaSourceTerminal [2]; /*   1  */
   char IcaDestination   [2]; /*   3  */
   char IcaCreditFlag    [3]; /*   5  */
   char IcaSequenceNbr   [2]; /*   8  */
   char IcaFs2FuncCode   [2]; /*  10  */
   char IcaForcePostAuth [6]; /*  12  */
   char IcaTerminalID    [1]; /*  18  */
   char IcaSystemNbr     [4]; /*  19  */
   char IcaLocationNbr   [4]; /*  23  */
   char IcaHotelCode     [2]; /*  27  */
   char IcaCheckNbr      [6]; /*  29  */
   char IcaSign          [1]; /*  35  */
   char IcaAmount        [9]; /*  36  */
   char IcaTransNbr      [4]; /*  45  */
   char IcaForcePostFlag [1]; /*  49  */
   char IcaSldNbr        [4]; /*  50  */
   char IcaTranDateTime [10]; /*  54  */
   char IcaSmartCardType [1]; /*  64  */
   char IcaCaiValeur     [8]; /*  65  */
   char IcaCurrencyCode  [3]; /*  73  */
   char IcaDateCAI       [6]; /*  76  */
   char IcaTimeCAI       [6]; /*  82  */
   char IcaCashierNbr    [6]; /*  88 CashierNbr + ServerNbr */
   char IcaTipAmount    [10]; /*  94  */   
   char IcaTaxAmouunt   [10]; /* 104  */
   char IcaTrackAcctNbr [19]; /* 114  */
   char IcaTrackSeparator [1];/* 133 */
   char IcaTrackCountry  [3]; /* 134 */
   char IcaTrackExpDate  [4]; /* 137 */
   char IcaTrackSerCode  [3]; /* 141 */
   char IcaFreeZone      [7]; /* 144 */
   char IcaTrackFiller   [3]; /* 151 */
   char IcaCAI          [16]; /* 154 */
   char IcaIcaCAIAdresse [4]; /* 170 */
   char IcaEndOfMessage  [1]; /* 174 */
   char IcaMpxExtraData [20]; /* 175 */
   }; /* Lg totale 195 */



struct EdlStdOut {
   char OcaCreditPlan       [1]; /*   0  */
   char OcaSourceTerminal   [2]; /*   1  */
   char OcaDestination      [2]; /*   3  */
   char OcaCreditFlag       [3]; /*   5  */
   char OcaSequenceNbr      [2]; /*   8  */
   char OcaStatusCode       [1]; /*  10  */
   char OcaRequestFs2Code   [1]; /*  11  */
   char OcaMessageType      [1]; /*  12  */
   char OcaSystemNbr        [4]; /*  13  */
   char OcaTerminalID       [1]; /*  17  */
   char OcaMessageIndicator [1]; /*  18  */
   char OcaCrtRespMsgNbr    [2]; /*  19  */
   char Filler              [6]; /*  21  */
   char OcaTransNbr         [6]; /*  27  */
   char OcaRespMsgText     [18]; /*  33  */
   char OcaAuthCode         [6]; /*  51  */
   char OcaCardAcctNbr     [10]; /*  57  */
   char OcaAmount           [5]; /*  67  */
   char OcaEndOfMessage     [1]; /*  72  */
   }; /* Lg totale 73 */



/* Constantes edl-tcp */
#define  NEG_AMOUNT           '-'
#define  FORCE_POST_1         '1'
#define  FORCE_POST_2         '2'
#define  FORCE_POST_3         '3'
#define  TRAINING_MODE        'T'


                            


