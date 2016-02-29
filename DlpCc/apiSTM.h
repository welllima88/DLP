/*****
* STM - Station de Travail Monétique
* API STM - Utilitaires STM
* Couche API exportée par la DLL
* Portage DLL Windows 95 - Patrice BURIEZ (PBu) - Février 1998
*****
* apiSTM.h
*****/

#ifndef __APISTM_H__
#define __APISTM_H__

/*****
* Constantes
*****/

#define STM_SYSTEM_MAX				9999
#define STM_TERMINAL_MAX			9999
#define STM_TRANSACTION_MAX			9999
#define STM_CURRENCY_MAX			999
#define STM_AMOUNT_MAX				99999999

#define STM_CARDTRACK2DATA_SIZE		(40 + 1)
#define STM_NUM_AUTO           		(10 + 1) //SJ070118
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
} STM_DATETIME, FAR* STM_LPDATETIME;

/*****
* Fonctions
*****/

#ifdef __cplusplus
extern "C" {
#endif

/* Lecture de la date et de l'heure système */
STM95DLL_API BOOL WINAPI STM_GetDateTime(STM_LPDATETIME lpDateTime);

#ifdef __cplusplus
}
#endif

#endif /* __APISTM_H__ */
