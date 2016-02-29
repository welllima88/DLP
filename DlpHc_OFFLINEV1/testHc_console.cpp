// testHc_console.cpp : Defines the entry point for the console application.
//


#include "stdafx.h"
#include "DlpHcStructs.h"
#include <iostream>
#undef far
#define far
#undef FAR
#define FAR far
typedef int BOOL;

/***************************************************************************/
/*    Gestion de la clef superviseur                                       */
/***************************************************************************/
int GetSecurity()
{ 
	return 0;
}


/***************************************************************************/
/*    Gestion du clavier                                                   */
/***************************************************************************/
int GetKey() 
{
//	Sleep (1);
	if (_kbhit())
		return (getchar());
  
	return 0;
}


/***************************************************************************/
/*    Lecture d'une string sur le clavier                                  */
/***************************************************************************/
int GetStringFromKeyboard (char *stringEntered, int MaxlentOfStringEntered, char *messageToDisplay)
{
	puts (messageToDisplay);
	gets (stringEntered);

	return 0;
}


/***************************************************************************/
/*    Affichage des messages de caisse                                     */
/***************************************************************************/
void Pos_display (char *messageToDisplay, int lentOfMessageToDisplay, short confirm)
{
	puts (messageToDisplay);
}


 //__declspec(dllimport) void __stdcall Dlp_Hc(S1 &,
extern "C" __declspec(dllimport) void __cdecl Dlp_Hc(S1 &,
										   S2 &,
										   int (*GetSecurity)(void),
										   int (*GetKey)(void),
										   int (*GetStringFromKeyboard)(char *, int, char *),
										   void(*Pos_display)(char *, int, short));


extern "C" __declspec(dllimport) void __cdecl Dlp_Hc_Revelation(S5 &,
										   S2 &,
										   int (*GetSecurity)(void),
										   int (*GetKey)(void),
										   int (*GetStringFromKeyboard)(char *, int, char *),
										   void(*Pos_display)(char *, int, short));



void main ()
{
	using namespace std;
 	int choix =0;
    S1 Send;
    S2 Recv;
	S5 SendRevelation;

	memset (&Send, 0x20, sizeof Send);
	memset (&Recv, 0x20, sizeof Recv);
	memset (&SendRevelation, 0x20, sizeof SendRevelation);

cout<<"entrer le numero de l'hotel charge"<<endl;
cin>>Send.aChargeNum;

	printf("-----------------------------------------------------------------\n");
	printf(" 1. Test DEBIT Hotel Charge avec message non valide\n");
	printf(" 2. Test DEBIT Hotel Charge avec message valide\n");
	printf(" 3. Test CREDIT Hotel Charge avec message non valide\n");
	printf(" 4. Test CREDIT Hotel Charge avec message valide\n");
	printf(" 5. Test DEBIT Hotel Charge avec Revelation non valide\n");
	printf(" 6. Test DEBIT Hotel Charge avec Revelation valide\n");
	printf("-----------------------------------------------------------------");
	printf("\nChoix :");
	
lbl1 :
	cin>>choix;
	
	
	switch(choix)
	{
	case 1:
		Send.aOperation = 'd';
		memcpy(Send.aAmount,"000000001000",12);
		memcpy(Send.aCurrencyCode,"250",3);
		memcpy(Send.aCtrlNum,"6000",4);
		Send.aRegister = '7';
		memcpy(Send.aTermNum,"313",3);
		memcpy(Send.aCashNum,"0476432",7);
		memcpy(Send.aTrnsNum,"135544",6);
		Send.aCustomerPresent = '1';
		//memcpy(Send.aChargeNum,"027411616",9);
		break;
	case 2:
		Send.aOperation = 'd';
		memcpy(Send.aAmount,"000000001000",12);
		memcpy(Send.aCurrencyCode,"978",3);
		memcpy(Send.aCtrlNum,"6000",4);
		Send.aRegister = '7';
		memcpy(Send.aTermNum,"313",3);
		memcpy(Send.aCashNum,"0476432",7);
		memcpy(Send.aTrnsNum,"135544",6);
		Send.aCustomerPresent = '1';
		//memcpy(Send.aChargeNum,"027663816",9);
		break;
	case 3:
		Send.aOperation = 'c';
		memcpy(Send.aAmount,"000000001000",12);
		memcpy(Send.aCurrencyCode,"250",3);
		memcpy(Send.aCtrlNum,"1362",4);
		Send.aRegister = '7';
		memcpy(Send.aTermNum,"313",3);
		memcpy(Send.aCashNum,"0476432",7);
		memcpy(Send.aTrnsNum,"135544",6);
		Send.aCustomerPresent = '1';
		//memcpy(Send.aChargeNum,"047663816",9);
		break;
	case 4:
		Send.aOperation = 'c';
		memcpy(Send.aAmount,"000000001000",12);
		memcpy(Send.aCurrencyCode,"250",3);
		memcpy(Send.aCtrlNum,"6000",4);
		Send.aRegister = '7';
		memcpy(Send.aTermNum,"313",3);
		memcpy(Send.aCashNum,"0476432",7);
		memcpy(Send.aTrnsNum,"135544",6);
		Send.aCustomerPresent = '1';
		//memcpy(Send.aChargeNum,"027663816",9);
		break;
	case 5:
		Send.aOperation = 'd';
		memcpy(SendRevelation.rAmount,"000000001000",12);
		memcpy(SendRevelation.rCurrencyCode,"978",3);
		memcpy(SendRevelation.rCtrlNum,"0000",4);
		SendRevelation.rRegister = '1';
		memcpy(SendRevelation.rTermNum,"001",3);
		memcpy(SendRevelation.rCashNum,"0476432",7);
		memcpy(SendRevelation.rTrnsNum,"135544",6);
		SendRevelation.rCustomerPresent = '1';
		memcpy(SendRevelation.rItem1,"0000000400",10);
		memcpy(SendRevelation.rItem2,"0000000600",10);

		memcpy(SendRevelation.rChargeNum,"631234567",9);
		break;
	case 6:
		Send.aOperation = 'd';
		memcpy(SendRevelation.rAmount,"000000001000",12);
		memcpy(SendRevelation.rCurrencyCode,"978",3);
		memcpy(SendRevelation.rCtrlNum,"4007",4);
		SendRevelation.rRegister = '1';
		memcpy(SendRevelation.rTermNum,"001",3);
		memcpy(SendRevelation.rCashNum,"0476432",7);
		memcpy(SendRevelation.rTrnsNum,"135544",6);
		SendRevelation.rCustomerPresent = '1';
		memcpy(SendRevelation.rItem1,"0000000400",10);
		memcpy(SendRevelation.rItem2,"0000000600",10);

		memcpy(SendRevelation.rChargeNum,"631234567",9);
		break;
	default:
		goto lbl1;
	}

	S1 & s = Send;
	S2 & r = Recv;

	S5 & rev = SendRevelation;

	switch(choix)
	{
	 case 1:
	 case 2:
	 case 3:
	 case 4:
         Dlp_Hc(s, r, GetSecurity, GetKey, GetStringFromKeyboard, Pos_display);
		 break;
	 case 5:
	 case 6:
         Dlp_Hc_Revelation(rev, r, GetSecurity, GetKey, GetStringFromKeyboard, Pos_display);
		 break;
	}


	//analyse du code réponse pour la transaction
	if(Recv.bResponseCode == '1')
		{
		Pos_display("testHC : Transaction refusee",0,0);
		Pos_display(Recv.bExplanation,0,0);
		}
	else if(Recv.bResponseCode == '0')
			Pos_display("testHC : Transaction acceptée",0,0);
		 else Pos_display("erreur transaction",0,0);

}

