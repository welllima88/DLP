
/***************************************************************************/
/*  c3api.h              Fichier Header C3                                 */
/*  (c)   copyright 07/07/1997                                             */
/*        CKD-MoneyLine                                                    */
/*        All rights reserved                                              */
/*                                                                         */
/***************************************************************************/

#ifndef  C3API_H
#define  C3API_H


#ifdef VERSION_PRT
   /* static library */
   extern void c3 (struct tpvMessIn *, struct tpvMessOut *, void *lastop);
   /* dynamic library */
   extern void c3dll (struct tpvMessIn *, struct tpvMessOut *,
                     int   (*)(),      /* GetSecurity() */
                     int   (*)(),      /* GetKey       */
                     int   (*)(),      /* GetStringFromKeyboard */
                     void  (*)(),      /* pos_display() */
                     void * pCtx);
#else
   /* static library */
   extern void c3 (struct tpvMessIn *, struct tpvMessOut *);
   /* dynamic library */
   extern void  c3dllAuto (struct tpvMessIn *, struct tpvMessOut *);
   #ifdef __cplusplus
      extern "C" void c3dll (struct tpvMessIn *, struct tpvMessOut *,
                     int   (*)(void),                    /* GetSecurity() */
                     int   (*)(void),                    /* GetKey       */
                     int   (*)(char *, int, char *),     /* GetStringFromKeyboard */
                     void  (*)(char *, int, short),      /* pos_display() */
	                 void  (*)(char *));                 /* PrintTicket*/
   #else
      extern void c3dll (struct tpvMessIn *, struct tpvMessOut *,
                     int   (*)(),      /* GetSecurity() */
                     int   (*)(),      /* GetKey       */
                     int   (*)(),      /* GetStringFromKeyboard */
                     void  (*)(),      /* pos_display() */
	                 void  (*)());     /* PrintTicket() */
   #endif
#endif

extern char Automate; /*'1' Mode Automate '0' Mode normal */


/* ----------- POS / C3  interface  ----------------------- */

/*  messages  operationels    */
struct tpvMessIn {
   char cAmount         [12]; /*   0  main amount     */
   char cCurrencyCode    [3]; /*  12  main curency    */
   char cAmount2        [12]; /*  15  second amount   */
   char cCurrencyCode2   [3]; /*  27  second curency  */
   char cOperation       [1]; /*  30  type de message */
   char cTenderType      [2]; /*  31  type de client  */
   char cReaderType      [2]; /*  33  type de saisie: '0' Inside , '1' Manuelle , '2' External Only , '3' Inside First */
   char cCustomerPresent [1]; /*  35  0x31 client present,  0x30 client absent ( vad, parking ...)  */
   char cTermNum         [8]; /*  36  n° de terminal 00000 + */
   char cCashNum         [8]; /*  44  n° de caissiere 0000 + */
   char cTrnsNum         [4]; /*  52  n° sequentiel de transaction */
   char cPan            [19]; /*  56  ex: ???4900000000000000 */
   char cCryptoVAD       [3]; /*  75  Crytogramme VAD (V5) */
   char cDateInitValidite[4]; /*  78  aamm  */
   char cDateFinValidite [4]; /*  82  aamm  */
   char cIso2           [40]; /*  86  lecteur externe */
   char cAutoType        [1]; /* 126  Type d'autorisation (Bancaire) */
   char cNumContexte     [8]; /* 127  contexte , reprise suspension XXhhmmss */
   char cNumAuto         [9]; /* 135  auto, reprise suspension */
   char cCtrlCheque      [1]; /* 144  '1'= FNCI, '2'=GARANTIE, '3'= GREEN */ 
   char cIdentCheque     [1]; /* 145  '0' : aucune ,'1' : CNI , '2' : Permis, '3' : Passeport, '4' : Autres */
   char cCmc7           [35]; /* 146  piste CMC7 utilisée pour la saisie manuelle */
   char cUserData1      [32]; /* 181  Generic Data      */
   char cUserData2      [32]; /* 213  Generic Data      */
   char cOption          [8]; /* 245  Paiement Option   */
   //Sup SJ cb52 - char FFU            [131]; /* 253  Pad à 384 */
   // SJ - Début Ajout cb52.
   char cTutorialMode    [1]; /* 253  mode école ('1') ou non ('0') */   
   char cNumTicket       [8]; /* 254  n° de ticket encaissement */
   char cNumDossier     [12]; /* 262 N° de dossier de préauto */
   char cTypeFacture     [1]; /* 274 Type de facture préauto '1': init, '2': fact. aditionnelle '3': cloture, '4': init. fact. prolongation, '5': cloture fact. prolongation, '6': fact. complement, '7': Suppression dossier, '8': fact. no-show */
   char FFU            [109]; /* 262  Pad à 384 */
   // SJ - Fin Ajout cb52.
   };



struct tpvMessOut {
   char cAmount         [12]; /*   0  main amount     */
   char cCurrencyCode    [3]; /*  12  main curency    */
   char cAmount2        [12]; /*  15  second amount   */
   char cCurrencyCode2   [3]; /*  27  Second Currency Code  */
   char cReponseCode     [4]; /*  30  Response code from host */
   char cC3Error         [4]; /*  34  Main error code */
   char cticketAvailable [1]; /*  38  Ticket avalable : 0x30:no,  0x31:yes */
   char cPan            [19]; /*  39  PAN : Primary Account Number */
   char cNumAuto         [9]; /*  58  Authorization Number */
   char cSignatureDemande[1]; /*  67  Ticket signature :  0x30 , 0x31  */
   char cTermNum         [8]; /*  68  Terminal Number 00000 + */
   char cTypeMedia       [1]; /*  76  Media type (FFU) */
   char cIso2           [38]; /*  77  ISO2 */
   char cCmc7           [35]; /* 115  Check */
   char cCardType        [1]; /* 150  Card Type : Bnc, Amx, Dcf ... */
   char cSSCarte         [1]; /* 151  Sub-card Type */
   char cTimeLoc         [6]; /* 152  Current Time */
   char cDateFinValidite [4]; /* 158  Expire Date */
   char cCodeService     [3]; /* 162  Service Code */
   char cTypeForcage     [1]; /* 165  Application specific */
   char cTenderIdent     [2]; /* 166  Customer Type */
   char cCertificatVA   [13]; /* 168  Trusted sign */
   char cDateTrns        [6]; /* 181  Transaction date "DDMMYY" */
   char cHeureTrns       [6]; /* 187  Transaction time "hhmmss" */
   char cPisteK          [1]; /* 193  K value for French cards */
   char cDepassPuce      [1]; /* 194  Smart card flag */
   char cIncidentCam     [3]; /* 195  Smart card information */
   char cCondSaisie      [3]; /* 198  Input mode information */
   char cOptionChoisie   [8]; /* 201  General for cards */
   char cOptionLibelle  [16]; /* 209  General for cards */
   char cNumContexte     [8]; /* 225  contexte, suspension XXhhmmss */
   char cCodeRejet      [36]; /* 233  Reject error code */
   char cCAI_Emetteur   [28]; /* 269  CAI_Emetteur (16) + Data_CAI_Em (12) */
   char cCAI_Auto       [52]; /* 297  CAI_Acquereur(16) + Data_CAI_Acq(36) */
  //Sup SJ cb52 - char FFU            [163]; /* 349  Pad à 512 */
  // SJ - Début Ajout cb52.
   char cTrnsNum         [4]; /* 349  n° sequentiel de transaction */
   char cNumFile         [2]; /* 353  transaction file number */
   char cUserData1      [32]; /* 355  Generic Data */
   char cUserData2      [32]; /* 387  Generic Data */
   char cNumDossier     [12]; /* 419  N° de dossier de préauto */
   char cTypeFacture     [1]; /* 431  Type de facture préauto '1': init, '2': fact. aditionnelle '3': cloture, '4': init. fact. prolongation, '5': cloture fact. prolongation, '6': fact. complement, '7': Suppression dossier, '8': fact. no-show */
   char cAxis            [1]; /* 432  N° du serveur Axis ayant enregisté la transaction */
   char FFU             [79]; /* 433  Pad à 512 */
   
  // SJ - Fin Ajout cb52.
   };


/* Champ cOperation[0] : type de traitement */
#define  TRAITEMENT_CARTE        0x01  /* internal value */
#define  TRAITEMENT_NOT_ALLOWED  0x02  /* internal value */

#define  INIT_CARTE           'A'
#define  TOTAL_CARTE          'B'
#define  DEBIT_CARTE          'C'
#define  CREDIT_CARTE         'D'
#define  ANTICIPATION         'E'
#define  RESTART_ACTION       'F'
#define  DEBIT_CHQ            'G'
#define  ANNUL_CHQ            'H'
#define  LECTURE_CARTE        'I'
#define  ANNUL_LAST_OP        'J'
#define  DUPLICATA_LAST_OP    'K'
#define  GET_VERSION_NUMBER   'L'
#define  CHECK_IN             'M'   /* préautorisation */
#define  TRAITEMENT_SPU       'N'   /* traitement defini dans cTenderType[0] */
#define  TRAITEMENT_EDL       'O'   /* traitement defini dans cTenderType[0] */
#define  C3_PORTUGAL          'P'   /* traitement defini dans cTenderType[0] */
#define  CHECK_READER         'Q'
#define  TRAITEMENT_FNF       'R'   /* traitement defini dans cTenderType[0] */
#define  TRAITEMENT_LECTEUR   'S'   /* operation sur le lecteur (traitement defini dans cTenderType[0]) */
#define  CHECK_OUT            'T'   /* validation préautorisation */
#define  ANNUL_CARTE          'U'   /* BNC V5, annulation d'une carte */
#define  C3_ADMIN             'V'   /* pour fonction administrateur : telechargement de logiciels, autotest, etc. */
#define  TRAITEMENT_PME       'W'   /* traitement defini dans cTenderType[0] */




/* tpvMessIn.cTenderType  + tpvMessIn.cTenderType */
#define  NO_TENDER_TYPE       '0'
#define  BNC_TYPE             '1'
#define  COF_TYPE             '2'
#define  PAS_TYPE             '3'
#define  BBL_TYPE             '4'
#define  PRT_TYPE             '5'
#define  AMX_TYPE             '6'
#define  DCF_TYPE             '7'
#define  CTM_TYPE             '8'
#define  FFI_TYPE             '9'
#define  FFI_TYPE_PROV        'A'
#define  SCO_TYPE             'B'
#define  ACC_TYPE             'C'
#define  JCB_TYPE             'D'
#define  FNC_TYPE             'E'
#define  ITC_TYPE             'E'
#define  FNF_TYPE             'F'
#define  SPU_TYPE             'G'
#define  EDL_TYPE             'H'
#define  BON_CUSTOM           'I'   /* Carrefour Bon d' Achat */
#define  SKY_TYPE             'J'
#define  KTL_TYPE             'K'
#define  CADEAUX_CUSTOM       'L'   /* Carrefour Bon d' Achat */
//#define  TEST_TYPE            'M'   /* Champ cCardType : Type de Cheque */
#define  CHQ_TYPE             'N'   /* Champ cCardType : Type de Cheque */
#define  CHQ_CCP_TYPE         'O'   /* Champ cCardType : Type de Cheque */
#define  CHQ_BNC_TYPE         'P'   /* Champ cCardType : Type de Cheque */
#define  CAF_TYPE             'Q'   
#define  CHECK_TYPE           'R'   /* préautorisation (Auchan) */
#define  PME_TYPE             'S'
#define  EMV_TYPE             'T'
#define  MTS_TYPE             'U'
#define  CBA_TYPE             'V'
#define  PNF_TYPE             'W'
#define  ELF_TYPE             'X'
#define  SML_TYPE             'Y'
#define  MCB_TYPE             'Z'
#define  PMS_TYPE             'a'




/* tpvMessIn.cTenderType Utilise UNIQUEMENT pour le traitement de la CarteU */
/* cOperation[0] == TRAITEMENT_SPU */
#define  TRANSACTION_SPU      'A'   /* traitement du total et du passage magasin */
/*#define  COUPON_SPU           'B'   traitement des coupons */
#define  CADEAUX_SPU          'C'   /* traitement des cadeaux */
#define  RENOUVEL_SPU         'D'   /* traitement du renouvellement de la carteU */
#define  POINTS_SPU           'E'   /* ticket avec le nbr de points de la carteU */
/*#define  ANNULATION_SPU       'F'   annulation d'un coupon */
#define  ADD_PTS_SPU          'G'   /* Ajout de points dans la carte */
#define  DELETE_PTS_SPU       'H'   /* Suppression de points de la carte */
#define  DUPLICATA_SPU        'I'   /* duplicata du dernier ticket (non implementé...) */
#define  LECTURE_SPU          'J'   /* lecture de la carteU */



/* tpvMessIn.cTenderType Utilise UNIQUEMENT pour le traitement de la Carte Edel */
/* cOperation[0] == TRAITEMENT_EDL */
#define  EDL_CTRL_CODE_PIN    'A'   /* Saisie & Controle du code PIN */
#define  EDL_FIRSTS_TRNS      'B'   /* Traitement des premieres transactions*/
#define  EDL_LAST_TRNS        'C'   /* Traitement de la derniere transaction*/
#define  EDL_ONE_STEP         'D'   /* Traitement en une passe */


/* tpvMessIn.cTenderType Utilise UNIQUEMENT pour le traitement SIBS */
/* cOperation[0] == C3_PORTUGAL */
#define C3_CLOSE              'A'
#define DEBIT_PME             'B'
#define C3_MANAGEMENT         'C'
#define C3_OPEN               'D'

/* tpvMessIn.cTenderType Utilise UNIQUEMENT pour le traitement Cheque */
/* cOperation[0] == DEBIT_CHEQUE */
#define CHECK_NORMAL          '0'
#define CHECK_READ            'A'  
#define CHECK_PRINT           'B'
#define CHECK_EJECT           'C'
#define CHECK_ANNUL           'D'


/* tpvMessIn.cTenderType Utilise UNIQUEMENT pour le traitement Finaref & Fnac */
/* cOperation[0] == TRAITEMENT_FNF */
#define  DEMANDE_FONDS        'A'   /* Demande de fonds */
#define  DEPOT_CHEQUE         'B'   /* Dépot cheque */
/*#define  DEBIT_CARTE        'C'
  #define  CREDIT_CARTE       'D' */
#define  REMBT_FONDS          'E'   /* Remboursement demande de fonds */
#define  REMBT_CHEQUE         'F'   /* Remboursement dépot cheque */
#define  STATUT_TRNS          'G'   /* Interrogation Statut */
#define  REGUL_DEBIT          'H'   /* Regularisation Debit FNAC */
#define  REGUL_CREDIT         'I'   /* Regularisation Credit FNAC */

/* tpvMessIn.cTenderType Utilise UNIQUEMENT pour le traitement de MONEO*/
/* cOperation[0] == TRAITEMENT_PME */
#define  CLOTURE_PME          'A'   /* Cloture de caisse */
#define  ARRET_PME            'B'   /* Cloture de caisse */
#define  RECHARGEMENT_PME     'C'   /* Rechargement de caisse */








/* tpvMessIn.cTenderType Utilise UNIQUEMENT pour les operations lecteur */
/* cOperation[0] == TRAITEMENT_LECTEUR */
#define  STRING_FROM_PINPAD   'A'   /* saisie d'une chaine de caractères */
#define  TEST_PRESENCE        'B'   /* test la présence de la carte */
#define  EJECTION_CARTE       'C'   /* éjection de la carte */
#define  CAPTURE_CARTE        'D'   /* capture de la carte */


/* tpvMessIn.cTenderType[2]  Utilise UNIQUEMENT pour les operations d'administrattion */
/* cOperation[0] == C3_ADMIN */
#define  TLC_AX_L3  		  "01"  /* telechargement axis & c3     	*/
#define  TLC_AX		  		  "02"  /* telechargement axis uniquement    */
#define  TLC_L3 			  "03"  /* telechargement c3   uniquement  */
#define  AUTO_TEST_C3		  "04"  /* auto test c3: fonctions externes + comm axis + comm lecteur */

#define  F_TICKET	          "ticket1"  /* Fichier du ticket 9.14a, ancien : ticket2*/
#define  F_TICKETCOM          "ticket1.com"  /* Fichier du ticket 9.14a, ancien : ticket2*/
#define  F_TICKET_CB_DLRP     "cb_dlrp"      /* Fichier du ticket */
#define  F_TICKETCOM_CB_DLRP  "cb_dlrp.com"  /* Fichier du ticket Toll Plaza VAD */
#define  F_TICKET_AM_DLRP     "am_dlrp"      /* Fichier du ticket Toll Plaza VAD */
#define  F_TICKETCOM_AM_DLRP  "am_dlrp.com"  /* Fichier du ticket Toll Plaza VAD */
#define  F_TICKET_JC_DLRP     "jc_dlrp"      /* Fichier du ticket Toll Plaza VAD */
#define  F_TICKETCOM_JC_DLRP  "jc_dlrp.com"  /* Fichier du ticket Toll Plaza VAD */


/* Type de carte (Table de Bin) */
#define BIN_BNC         0x30
#define BIN_TEST        0x31
#define BIN_PAS         0x33
#define CHQ             0x34
#define BIN_COF         0x35
#define BIN_AMX         0x36
#define BIN_DCF         0x37
#define BIN_PRT         0x38
#define BIN_BBL         0x39
#define BIN_FFI         0x3A
#define BIN_CTM         0x3B
#define BIN_SPU         0x3C
#define BIN_EDL         0x3D
#define CHQ_CCP         0x3E
#define CHQ_BNC         0x3F
#define BIN_SCO         0x40
#define BIN_ACC         0x41
#define BIN_ITC         0x42
#define BIN_FNF         0x43
#define BIN_JCB         0x44
#define BIN_CAF         0x45
#define BIN_SKY         0x46


                            

/* Champ cReaderType : Type de lecture */
#define C3_INPUT              '0'   /* Lecture effectuee par C3 */
#define MANUAL_INPUT          '1'   /* Saisie manuelle piste ISO2/CMC7 */
#define EXTERNAL_INPUT        '2'   /* Lecture effectuee par un lectueur externe */
#define C3_AND_EXTERNAL_INPUT '3'   /* Lecture effectuee par C3, si erreur alors utilise les paramètres passés par le lectueur externe */
#define CARTE_SYNCHRONE       '4'   /* Gestion des cartes synchrones */


/*posOut  sReponseCode */
#define  SUSPENSION           'A'


/* definition  Operation Reversal  different de DEBIT_CARTE, CREDIT_CARTE */
#define  REVERSAL             'A'

/* Largeur du ticket */
#define WIDE_TICKET           40




/* differentes valeurs de cCustomerPresent */
#define CLIENT_ABSENT         '0'   /* client absent ( vad, parking, ...) */
#define CLIENT_PRESENT        '1'   /* client present */

/* differentes valeurs de cIso2[0] (card type) */
#define MAGNETIC_CARD         '1'   /* carte magnétique */
#define SMART_CARD            '2'   /* carte à puce */


/* Timeout keyboard */
#define  NO_WAIT_KEY       0
#define  WAIT_KEY          1
#define  WAIT_1_SEC        2

#define  ABANDON_POS           0x41          /* A */
#define  ANTICIPATION_POS      0x42          /* B */
#define  SUSPEND_POS           0x42          /* B */
#define  NO_KEY_POS            0x43          /* C */
#define  RAPPEL_POS            0x44          /* D */
#define  CORRECTION_POS        0x45          /* E */
#define  VALIDATION_POS        0x12          /* ctrl+R */


/*  Status outPut sC3Error  */
#define TRNS_ABOUTIE                      0
#define TIME_OUT_L10                    100 
#define BAD_OPEN_L10                    101 
#define BAD_READ_L10                    102 
#define PISTE_ILLISIBLE                 110 
#define CARTE_BLOQUEE                   111 
#define CHEQUE_ILLISIBLE                112 
#define LECT_CHQ_INCONNU                120
#define LECT_CHQ_ABS                    121
#define BAD_LECT_CHEQUE                 122
#define BAD_ECR_CHEQUE                  123
/* c3 read/write  error codes */
#define BAD_AXIS_CONN                   201
#define BAD_AXIS_READ                   202
#define BAD_AXIS_WRITE                  203
#define EN_COURS                        204
#define BAD_RSPN_MSG_FROM_AXIS          205
#define NAK_FROM_AXIS                   206
#define TIME_OUT_AXIS                   207   /* refus définitif (nack) de la part d'Axis */
#define NAK_AXIS_NOT_RUNNING            208   /* Nack de la part de LocalAxis -> sauvegarde offline */



/* pos error codes */
#define INIT_POS_CTX_IMPOSSIBLE         300
#define GET_POS_CTX_IMPOSSIBLE          301
#define SET_POS_CTX_IMPOSSIBLE          302
#define GET_BIN_TABLE_IMPOSSIBLE        303
#define SET_BIN_TABLE_IMPOSSIBLE        304
#define ABANDON_OPERATOR                310
#define TRNS_NON_ABOUTIE                311
#define TRNS_SUSPENDUE                  312
#define CARD_NO_TENDER                  313

/*  Status outPut cReponseCode  */
/* Code erreur detaille en cas de Refus de la transaction */
#define RESPONSE_CODE_OK                  0  /* acceptee */                        
#define RESPONSE_CODE_REFUSE              1  /* refusee forcable */                
#define RESPONSE_CODE_INTERDITE           2  /* interdite */                       
#define RESPONSE_CODE_APPELER             3  /* appeler */                         
#define RESPONSE_CODE_REFUSE_NF           4  /* refuse non forcable */             
#define RESPONSE_CODE_DERNIER_ACHAT       4  /* Dernier achat <=> accepte */       
#define RESPONSE_CODE_EPUISEE             5  /* refuse non forcable */             
#define RESPONSE_CODE_NON_DISPO           6  /* forcage ou relance appel */        
#define RESPONSE_CODE_ERREUR_COM          7  /* Erreur Centre Auto apres COM */    
#define RESPONSE_CODE_SECRET              8  /* Erreur Code secret */              
#define INCORRECT_AMOUNT                 13  /* BAD AMOUNT */                      
#define RESPONSE_CODE_14                 14  /* TRT non AUTORISE */                
#define RESPONSE_CODE_15                 15  /* BIN INCONNU */                     
#define RESPONSE_CODE_16                 16  /* TPV INCONNU */                     
#define RESPONSE_CODE_18                 18  /* TPV non INITIALISE */              
#define RESPONSE_CODE_20                 20  /* APPLI AXIS NON INIT/ACTIVE/APPEL */
#define RESPONSE_CODE_21                 21  /* Fichier Transactions Plein */      
#define RESPONSE_CODE_22                 22  /* Bordereau Chq attendu */           
#define RESPONSE_CODE_23                 23  /* Bordereau Chq non attendu */       
#define RESPONSE_CODE_24                 24  /* Devise non supportee */       
#define RESPONSE_CODE_FORMAT             30  /* erreur format */                   
#define RESPONSE_CODE_ABOUTI             31  /* non abouti */                      
#define INCORRECT_EXPIRY_DATE          1030
#define INCORRECT_PAN                  1005


/*=======================================*/
/* c3.exe  fichiers d'echange            */ 
/*=======================================*/

#define C3_INIT_FILE    "c3init"      /* parametres de init c3 ligth */
#define C3_CMDE_FILE    "c3cmde"      /* fichier de commande  TPV --> zc3 */ 
#define C3_RSPN_FILE    "c3rspn"      /* fichier compte rendu zc3 --> TPV */
#define C3_EOT_FILE     "c3eot"       /* fichier End Of Transmission */
#define C3_BAD_FILE     "c3error"     /* fichier de commande c3cmde en erreur TPV --> zc3 */ 
#define C3_AX_FILE      "c3tlc"          /* fichier de telechargement  par default a partir d'axis   */ 


/*======================================*/
/*        Definitions EURO              */
/*======================================*/

/* Code monnaie */
#define EUR_N     "978"
#define EUR_F     "999"
#define EUR_S     "EUR"
#define FRF_N     "250"
#define FRF_S     "FRF"

/*
#define EUR_N		"978"
#define EUR_A		"999"

#define EUR_Z		 978
#define EUR_F		 999
#define EUR_S		"EUR"

#define FRF_N		"250"
#define FRF_Z		 250
#define FRF_S		"FRF"
*/

/* Valeur de l'euro */
#define EURO_N    655957
#define EURO_V    6.55957
#define EURO_S   "655957"
    
#endif

