//*****
//* DlpCc - Interface système vente Résa
//* Initialisation et terminaison de DLPCCDLL.DLL
//* SJU - 16 février 2007
//* Dernière livraison : ??
//* Version 1.0
//*****
//* apiOn2.cpp
//*****


#include <ctype.h>
#include "DlpCcDLLpriv.h"
#include "apiDLPc3.h"
#include "galaxy.h"
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <conio.h>
#include <io.h>
#include <fcntl.h>
#include <process.h>
#include <time.h>



#include "apiON2.h"


extern char ConfigHost[32],ConfigSocket[5];
extern bool bHis;



/////////////////////////////////////////////////////////////////////////////
//                      Variables globales                                 //
/////////////////////////////////////////////////////////////////////////////






//////////////////////////////////////////////////////////
//    TraiteResa                                       //
////////////////////////////////////////////////////////
bool TraiteResa( CcPosMsgSend *lpGalxSend,
			     CcPosMsgRecv *lpGalxRecv)
{ 

  int szMsg, Ret=0;
  char Msg[255];


  //ON2_LPPAYMENT lpPayment=new(ON2_PAYMENT);
  ON2_PAYMENT Payment;
  ON2_LPPAYMENT lpPayment = &Payment;

  STM_DATETIME DateTime;
  STM_LPDATETIME lpDateTime = &DateTime;

  ON2_REFUND Refund;
  ON2_LPREFUND lpRefund = &Refund;

  ON2_APPROVAL Approval;
  ON2_LPAPPROVAL lpApproval = &Approval;

  ON2_COMMIT Commit;
  ON2_LPCOMMIT lpCommit = &Commit;

  DWORD dwTimeout = 200000;
  char *errLbL;

  bool RetourFnc = false;

  static char   szC3cfg[16];

   memcpy(lpGalxRecv->bSignature, "0",1);
   memcpy(lpGalxRecv->bPanNum, lpGalxSend->aPanNum, sizeof(lpGalxRecv->bPanNum));
   if (!bHis) memset(&lpGalxRecv->bPanNum[10],'?',sizeof(lpGalxRecv->bPanNum)-11 );
   memcpy(lpGalxRecv->bBbank, "??", sizeof(lpGalxRecv->bBbank));


  memset (szC3cfg,'\0',sizeof(szC3cfg));
  memcpy (szC3cfg,"c3config",8);
  if (GetAxisCom(szC3cfg) == false)
  {
	  //szMsg=MessageBox(NULL,"Erreur sur GetAxisCom","TraiteResa", MB_OK);
        DLP_Pos_display("Erreur sur GetAxisCom - TraiteResa",0,0);
	  return false;
  }

  memset(lpPayment,0x00,sizeof(ON2_PAYMENT));

  if ((FormateMsgPourResa(lpGalxSend, lpPayment, lpDateTime, lpRefund)))
  {
    DLP_Pos_display("Appel serveur d'autorisation",0,0);	  
    if (!ON2_Open(ConfigHost, atoi(ConfigSocket))) 
	{
		memset(Msg,0x00, sizeof(Msg));
		sprintf(Msg,"Erreur sur ouverture port IP %4.4s \0", ConfigSocket);
	    DLP_Pos_display(Msg,0,0);
		memcpy(lpGalxRecv->bResponseCode,"0",1);
		memcpy(lpGalxRecv->bExplanation, lpApproval->szApprovalText, sizeof(lpGalxRecv->bExplanation));
		Ret = ON2_Close();
	    return false;
	}
    else
	{
		Ret = PostMessage(FindWindow(NULL,"monetique"), WM_USER+5, 0, 0); //Donne l'état en cours.
		Sleep(100); //500 - Attente ouverture Sleep(1000);
        DLP_Pos_display("Demande d'autorisation envoyée",0,0);	  
		if (!memcmp(lpGalxSend->aOperation,"D",1))
		{  // transaction de débit
		  memset(lpGalxRecv->bExplanation, ' ', sizeof(lpGalxRecv->bExplanation));

		  if (bHis) lpPayment->Request.dwAmount = 0;  //Si Request de TFD alors le montant est 0 pour la pré-auto

		  Ret = ON2_SendPayment( lpDateTime,lpPayment,lpApproval,dwTimeout, errLbL);

		  if (!Ret)
		  {
			 memcpy(lpGalxRecv->bPanNum, &lpApproval->szAlias[1], sizeof(lpGalxRecv->bPanNum));

		     DLP_Pos_display("PROBLEME SURVENU LORS DE L'AUTORISATION", 0 ,0);
		     if ((int)&errLbL[0] > 31 )  DLP_Pos_display(errLbL, strlen(errLbL) ,0);
		     Ret = PostMessage(FindWindow(NULL,"monetique"), WM_USER+7, 0, 0); //Traitement refusé.
		     Ret = ON2_Close();
             memcpy(lpGalxRecv->bResponseCode,"0",1);
  		     memcpy(lpGalxRecv->bExplanation, "REFUS", 5 );
             return false;
		  }

		  if (bHis)   //Si Request de TFD alors le montant est 0 pour la pré-auto
		  {

			 switch(lpApproval->byApprovalStatusCode)  //SJ020507
			 { 
		       case ON2_APPROVALSTATUS_APPROVED :		//	Carte acceptée
				   {
			         DLP_Pos_display("Carte vérifiée avec succès", 0 ,0);
				     Ret = PostMessage(FindWindow(NULL,"monetique"), WM_USER+6, 0, 0); //Traitement autorisé.
					 memcpy(lpGalxRecv->bResponseCode,"0",1);
				   }
			   break;

			   default:
				   {
                    DLP_Pos_display("Carte refusée", 0 ,0);
				    Ret = PostMessage(FindWindow(NULL,"monetique"), WM_USER+7, 0, 0); //Traitement refusé.
					memcpy(lpGalxRecv->bResponseCode,"1",1);
					memcpy(lpGalxRecv->bPanNum,"000000000000000000",18);
					memcpy(lpGalxRecv->bExplanation, lpApproval->szApprovalText, sizeof(lpGalxRecv->bExplanation));
					Sleep(200);
			        Ret = ON2_Close();
					return false;
				   }
			   break;
			 }
			  memcpy(lpGalxRecv->bPanNum, &lpApproval->szAlias[1], sizeof(lpGalxRecv->bPanNum));
			  memcpy(lpGalxRecv->bExplanation, lpApproval->szApprovalText, sizeof(lpGalxRecv->bExplanation));
			  Sleep(200);
			  Ret = ON2_Close();
			  return true;
		  }

		  //switch(lpApproval->byApprovalResponseCode) //SJ020507
		  switch(lpApproval->byApprovalStatusCode)  //SJ020507
		  { 
		  case ON2_APPROVALSTATUS_APPROVED :		//	1° passe acceptée
			{
				 DLP_Pos_display("Confirmation de paiement en cours ...", 0 ,0);

				 if (FormateCommitPourResa(lpGalxSend, lpPayment, lpDateTime, lpCommit)) 
				 {
				   Ret = ON2_SendCommit(lpDateTime,lpCommit);
				   if (!Ret)
				   {
		             DLP_Pos_display("PROBLEME SURVENU LORS DE LA CONFIRMATION", 0 ,0);
		             Ret = PostMessage(FindWindow(NULL,"monetique"), WM_USER+7, 0, 0); //Traitement refusé.
		             RetourFnc = false;

                     memcpy(lpGalxRecv->bResponseCode,"1",1);
					 memcpy(lpGalxRecv->bExplanation, "REFUS", 5 );
				   }
				   else
				   {
					 Ret = PostMessage(FindWindow(NULL,"monetique"), WM_USER+6, 0, 0); //Traitement autorisé.
					 DLP_Pos_display("PAIEMENT ACCEPTE", 0 ,0);
					 
                     memcpy(lpGalxRecv->bResponseCode,"0",1);
					 memcpy(lpGalxRecv->bExplanation, lpApproval->szApprovalText, sizeof(lpGalxRecv->bExplanation));

					 RetourFnc = true;
				   }
				 }
				 else
				 {
					 DLP_Pos_display("PROBLEME FORMATAGE DE LA CONFIRMATION", 0 ,0);
		             Ret = PostMessage(FindWindow(NULL,"monetique"), WM_USER+7, 0, 0); //Traitement refusé.
                     memcpy(lpGalxRecv->bResponseCode,"1",1);
					 memcpy(lpGalxRecv->bExplanation, "REFUS", 5 );

		             RetourFnc = false;
				 }



			}  //  Fin Case ON2_APPROVALSTATUS_APPROVED
			break;

		  case ON2_APPROVALSTATUS_REJECTED :		//1° passe refusée
			{
			   char szError[50];
			   
			   memset(szError, 0x00, sizeof(szError));
			   DLP_Pos_display("PAIEMENT REFUSE", 0 ,0);
			   memcpy(lpGalxRecv->bResponseCode,"0",1);
/*
			   if (&errLbL[0] != 0x00 )  
			   {
				   DLP_Pos_display(errLbL, strlen(errLbL) ,0);
				   memcpy(lpGalxRecv->bExplanation, errLbL, sizeof(lpGalxRecv->bExplanation) );
 				}
*/
               memcpy(lpGalxRecv->bExplanation, lpApproval->szApprovalText, sizeof(lpGalxRecv->bExplanation) );
			   memcpy(szError, lpApproval->szApprovalText, sizeof(lpApproval->szApprovalText) );
			   

			   Ret = PostMessage(FindWindow(NULL,"monetique"), WM_USER+7, 0, 0); //Traitement refusé.
			   memcpy(lpGalxRecv->bResponseCode,"1",1);
			   DLP_Pos_display(LPSTR(szError), 0 ,0);

			   RetourFnc = true;

			}  // Fin Case ON2_APPROVALSTATUS_REJECTED
			break;

		  case ON2_APPROVALSTATUS_REFERRAL :		//1° passe Appel Phonie / Forçage
			{
			  char szNumAuto[10];
			  int MaxCarSaisie = 6;

			  memset(szNumAuto, 0x00, sizeof(szNumAuto));

			  while (true)
			  {
			    Ret=DLP_GetStringFromKeyboard(szNumAuto, MaxCarSaisie, lpApproval->szApprovalText);
				
				if (strlen(szNumAuto) > 0 && Ret == 0x12)	break;
				if (Ret == 0x41)	break;

				DLP_Pos_display(" ", 0 ,0);
				DLP_Pos_display("SAISIR UN NUMERO D'AUTORISATION.", 0 ,0);
				DLP_Pos_display(" ", 0 ,0);
			  }

			  switch (Ret)
			  {
			  case 0x12: //Appel phonie réalisé et validé
				  {
					  memcpy(lpPayment->byForcePostFlag, "1", 1);
					  memcpy(lpPayment->szApprovalCode, szNumAuto,MaxCarSaisie);
					  memset(&lpPayment->szApprovalCode[6], 0x00, 1);
					  Ret = ON2_SendPayment( lpDateTime,lpPayment,NULL,dwTimeout, errLbL);

					  if (Ret)
					  {
						  Ret = PostMessage(FindWindow(NULL,"monetique"), WM_USER+6, 0, 0); //Traitement autorisé.
					      DLP_Pos_display("PAIEMENT ACCEPTE", 0 ,0);
	  				      memcpy(lpGalxRecv->bResponseCode,"0",1);
 				          memcpy(lpGalxRecv->bExplanation, lpApproval->szApprovalText, sizeof(lpGalxRecv->bExplanation));

						  RetourFnc = true;
					  }
					  else
					  {
						  Ret = PostMessage(FindWindow(NULL,"monetique"), WM_USER+7, 0, 0); //Traitement refusé.
					      DLP_Pos_display("PROBLEME SURVENU LORS DE L'AUTORISATION", 0 ,0);
						  memcpy(lpGalxRecv->bResponseCode,"1",1);
						  memcpy(lpGalxRecv->bExplanation, "REFUS", 5 );
						  if (&errLbL[0] != 0x00 )  
						  {
							  DLP_Pos_display(errLbL, strlen(errLbL) ,0);
							  
   						  }
						  RetourFnc = false;
					  }

				  }
				  break;

			  case 0x41: //Appel phonie décliné, enchaine sur forcage
				  {
					 DLP_Pos_display("APPEL PHONIE DECLINE PAR L'UTILISATEUR", 0 ,0);
					 DLP_Pos_display("VOULEZ-VOUS FORCER LA TRANSACTION ?", 0 ,0);

					 Ret = 0x00;
					 while (Ret == 0x00)
						{
						    Ret = DLP_GetKey();
	 						Sleep(500);
						}

					 switch (Ret) //traite la réponse de GetKey
					 {
					 case 13: //Forçage demandé
						{
						  DLP_Pos_display("FORCAGE EN COURS ...", 0 ,0);
						  memcpy(lpPayment->byForcePostFlag, "1", 1);
					  	  memset(lpPayment->szApprovalCode, 0x00, ON2_APPROVALCODE_SIZE);
					  	  Ret = ON2_SendPayment( lpDateTime,lpPayment,NULL,dwTimeout, errLbL);

					  	  if (Ret)
					  	  {
						    DLP_Pos_display("PAIEMENT ACCEPTE", 0 ,0);
						    Ret = PostMessage(FindWindow(NULL,"monetique"), WM_USER+6, 0, 0); //Traitement autorisé.
	  				        memcpy(lpGalxRecv->bResponseCode,"0",1);
 				            memcpy(lpGalxRecv->bExplanation, lpApproval->szApprovalText, sizeof(lpGalxRecv->bExplanation));

						    RetourFnc = true;
					  	  }
					  	  else
					  	  {
						    DLP_Pos_display("PROBLEME SURVENU LORS DE L'AUTORISATION", 0 ,0);
							memcpy(lpGalxRecv->bResponseCode,"1",1);
					        
					        if (&errLbL[0] != 0x00 )  DLP_Pos_display(errLbL, strlen(errLbL) ,0);
						    Ret = PostMessage(FindWindow(NULL,"monetique"), WM_USER+7, 0, 0); //Traitement refusé.
						    RetourFnc = false;
					  	  }

				  		}
						break;

					 case 0x41: //Forçage décliné et abandon de trn
						{
					   DLP_Pos_display("FORCAGE DECLINE PAR L'UTILISATEUR", 0 ,0);
					   DLP_Pos_display("ABANDON DE LA TRANSACTION", 0 ,0);
					   memcpy(lpGalxRecv->bResponseCode,"1",1);
					   memcpy(lpGalxRecv->bExplanation, lpApproval->szApprovalText, sizeof(lpGalxRecv->bExplanation));

					   Ret = PostMessage(FindWindow(NULL,"monetique"), WM_USER+7, 0, 0); //Traitement refusé.

						}
					    break;

					 }  //Fin switch Réponse GetKey
				  }  // Fin Case 0x41 Appel phonie décliné, enchaine sur forcage
				  break;

			  } //Fin switch Réponse GetStringFromKeyboard
			  RetourFnc = true;
			} // Fin Case ON2_APPROVALSTATUS_REFERRAL
			break;

		   default :
			   {
				 //szMsg=MessageBox(NULL,"Code retour ON2_SendPayment inconnu","TraiteResa", MB_OK);
				 DLP_Pos_display("Code retour ON2_SendPayment inconnu - TraiteResa", 0 ,0);
				 memcpy(lpGalxRecv->bResponseCode,"1",1);
                 memcpy(lpGalxRecv->bExplanation, "REFUS", 5);
			     RetourFnc = false;
			   }
		       break;

		  //if (!ON2_Close()) szMsg=MessageBox(NULL,"Erreur sur fermeture","Protocole Résa", MB_OK);

		  //return RetourFnc;

		  } //Fin du switch Réponse 1° passe
	  	}  //Fin Débit
		else // Transaction de crédit ?
		{ 
			if (!memcmp(lpGalxSend->aOperation,"C",1))
			{
		
		       Ret = ON2_SendRefund( lpDateTime,lpRefund,lpApproval,dwTimeout);
			   if (!Ret)
					  {
						  Ret = PostMessage(FindWindow(NULL,"monetique"), WM_USER+7, 0, 0); //Traitement refusé.
					      DLP_Pos_display("PROBLEME SURVENU LORS DU REMBOURSEMENT", 0 ,0);
						  if (&errLbL[0] != 0x00 )  DLP_Pos_display(errLbL, strlen(errLbL) ,0);
						  memcpy(lpGalxRecv->bResponseCode,"1",1);
					      memcpy(lpGalxRecv->bExplanation, "REFUS", 5);
						  RetourFnc = false;
					  }
					  else
					  {
						  switch(lpApproval->byApprovalResponseCode)
						  {
							case ON2_APPROVALSTATUS_APPROVED :		//	remboursement accepté
							{
								if (!IsPosteSTM(szC3cfg))
								{
								  DLP_Pos_display("Confirmation de remboursement en cours ...", 0 ,0);

								  if (FormateCommitCreditPourResa(lpGalxSend, lpRefund, lpDateTime, lpCommit))
								  {
								  	ON2_SendCommit(lpDateTime,lpCommit);
									if (!Ret)
									{
									  DLP_Pos_display("PROBLEME SURVENU LORS DE LA CONFIRMATION", 0 ,0);
									  Ret = PostMessage(FindWindow(NULL,"monetique"), WM_USER+7, 0, 0); //Traitement refusé.
									  memcpy(lpGalxRecv->bResponseCode,"1",1);
					                  memcpy(lpGalxRecv->bExplanation, "REFUS", 5);

									  RetourFnc = false;
									}
									else
									{
									  Ret = PostMessage(FindWindow(NULL,"monetique"), WM_USER+6, 0, 0); //Traitement autorisé.
									  DLP_Pos_display("REMBOURSEMENT ACCEPTE", 0 ,0);
									  memcpy(lpGalxRecv->bResponseCode,"0",1);
					                  memcpy(lpGalxRecv->bExplanation, lpApproval->szApprovalText, sizeof(lpGalxRecv->bExplanation));

									  RetourFnc = true;
									}
								  }
								  else  //FormateCommitCreditPourResa KO
								  {
									DLP_Pos_display("PROBLEME FORMATAGE DE LA CONFIRMATION", 0 ,0);
									Ret = PostMessage(FindWindow(NULL,"monetique"), WM_USER+7, 0, 0); //Traitement refusé.
									memcpy(lpGalxRecv->bResponseCode,"1",1);
					                memcpy(lpGalxRecv->bExplanation, "REFUS", 5);

									RetourFnc = false;
								  }
								}
								else  // Le Poste est STM
								{
									Ret = PostMessage(FindWindow(NULL,"monetique"), WM_USER+6, 0, 0); //Traitement autorisé.
								    DLP_Pos_display("REMBOURSEMENT ENREGISTRE", 0 ,0);
								    memcpy(lpGalxRecv->bResponseCode,"0",1);
					                memcpy(lpGalxRecv->bExplanation, lpApproval->szApprovalText, sizeof(lpGalxRecv->bExplanation));

								    RetourFnc = true;
								}


							   // DLP_Pos_display("REMBOURSEMENT ACCEPTE", 0 ,0);
						       // Ret = PostMessage(FindWindow(NULL,"monetique"), WM_USER+6, 0, 0); //Traitement autorisé.
							   // RetourFnc = true;
							}  //  Fin Case ON2_APPROVALSTATUS_APPROVED
							break;

							default:
							{
								DLP_Pos_display("REMBOURSEMENT REFUSE", 0 ,0);
								Ret = PostMessage(FindWindow(NULL,"monetique"), WM_USER+7, 0, 0); //Traitement refusé.
								memcpy(lpGalxRecv->bResponseCode,"1",1);
					            memcpy(lpGalxRecv->bExplanation, "REFUS", 5);

								RetourFnc = false;
							}
						  } // Fin switch réponse 
						  
						  //RetourFnc = true;
					  }

		  	  
			} // Fin IF Opération = "C"
		
			
		} //Fin Crédit ?


        if (!ON2_Close()) 
		{
		  //szMsg=MessageBox(NULL,"Erreur sur fermeture","Protocole Résa", MB_OK);
			DLP_Pos_display("Erreur sur fermeture - Protocole Résa", 0 ,0);
		  return false;
		}

	} //Fin ON2_Open OK


  } //Fin FormateMessage OK
  else
  {
	  memcpy(lpGalxRecv->bResponseCode,"1",1);
	  memcpy(lpGalxRecv->bExplanation, "REFUS", 5);
	  RetourFnc = false;
  }


	return RetourFnc;

}






      ////////////////////////////////////////////////////////////////////////////////////////////////
     //                                                                                            //
    // Février 2007 - SJU                                                                         //
   //                                                                                            //
  //     Convertir le message d'autorisation entrant dans la DLL vers le protocole Résa CRO     //
 //                                                                                            //
///////////////////////////////////////////////////////////////////////////////////////////////


bool FormateMsgPourResa(CcPosMsgSend *lpGalxSend,
						ON2_LPPAYMENT lpPayment,
						STM_LPDATETIME lpDateTime,
						ON2_LPREFUND lpRefund)
{
bool Ret = true;
char szSystem[5], szTerminal[5], szTrn[5],szDevise[4], szPan[38], szAmount[13], szCvx2[4], szC3Cfg[32]; 

    memset(szSystem,0x00, sizeof(szSystem));
	memset(szTerminal,0x00, sizeof(szTerminal));
	memset(szTrn,0x00, sizeof(szTrn));
	memset(szDevise,0x00, sizeof(szDevise));
	memset(szCvx2,0x00, sizeof(szCvx2));
	memset(szPan,0x00, sizeof(szPan));
	memset(szAmount,0x00, sizeof(szAmount));
	memset(szC3Cfg,0x00, sizeof(szC3Cfg));

	sprintf(szC3Cfg, "%s.","c3Config");
	
	if (!memcmp(lpGalxSend->aOperation,"D",1) || bHis)
		{  // transaction de débit
		  memcpy(lpPayment->byForcePostFlag, "0", 1);
		  memcpy(lpPayment->szApprovalCode, "000000", 6);

		  sprintf(szTerminal,"%-1.1s%-3.3s\0", lpGalxSend->aRegister, lpGalxSend->aTermNum);
	      lpPayment->Request.wTerminal = (WORD) atoi(szTerminal);

		  memcpy(szSystem, lpGalxSend->aCtrlNum, 4);
		  lpPayment->Request.wSystem = (WORD)atoi(szSystem);
	
		  memcpy(szTrn, &lpGalxSend->aTrnsNum[2], 4);
		  lpPayment->Request.wTransaction = (WORD) atoi(szTrn);

		  memcpy(lpPayment->Request.szCashier, &lpGalxSend->aCashNum[1],6);

		  lpPayment->Request.byCardStatusCode = 2;
		  if(bHis) lpPayment->Request.byCardStatusCode = 0;

		  lpPayment->Request.byCardCallCode = 0;

		  
		  int i,j;

		  i=0, j=0;

		  while (true)
		  {
			
			if (memcmp(&lpGalxSend->aPanNum[j], "0", 1) != 0) break;
			j++;
		  }
		  
		  memcpy(&szPan[0],";",1);
		  memcpy(&szPan[1],&lpGalxSend->aPanNum[j],19-j);

		  i=19-j+1;

          memcpy(&szPan[i],"=",1);

		  if( IsPostePKG(szC3Cfg) || bHis){
			memcpy(&szPan[i+1], lpGalxSend->aEndValid,2);
			memcpy(&szPan[i+3], &lpGalxSend->aEndValid[2],2);
		  }
		  else{
			memcpy(&szPan[i+1], &lpGalxSend->aEndValid[2],2);
			memcpy(&szPan[i+3], lpGalxSend->aEndValid,2);
		  }

		  memcpy(&szPan[i+5], "101",3);
		  
		  for (j=i+8; j < 40 ; j++)
		  {
			memcpy(&szPan[j],"0",1);
		  }
	
		  memcpy(&szPan[39],"?",1);

		  memcpy(lpPayment->Request.szCardTrack2Data, szPan, strlen(szPan));

		  memset(lpPayment->Request.abyAlign,0x00,1);

		  memcpy(szDevise, lpGalxSend->aCurrencyCode, 3);
		  lpPayment->Request.wCurrency = (WORD) atoi(szDevise);

		  memcpy(szAmount,lpGalxSend->aAmount,12);
		  lpPayment->Request.dwAmount = (long) atoi(szAmount);

		  memcpy(szCvx2, lpGalxSend->aCvv2, 3);
		  lpPayment->Request.wCvx2 = (WORD) atoi(szCvx2);

		  memcpy(lpPayment->Request.szNumAuto, lpGalxSend->aTrnsRef, 10); 

	
		  // Formatage de l'horodatage.
		  //----------------------------

		  time_t time_c;
		  struct tm *dat_j;

		  /* Appel fonction date systeme */
		  time (&time_c);
		  dat_j = localtime (&time_c);
	
		  lpDateTime->wYear = dat_j->tm_year - 100;
		  lpDateTime->byMonth = dat_j->tm_mon + 1;
		  lpDateTime->byDay = dat_j->tm_mday;
		  lpDateTime->byHour = dat_j->tm_hour;
		  lpDateTime->byMin = dat_j->tm_min;
	  
		}


	if (!memcmp(lpGalxSend->aOperation,"C",1))
		{  // transaction de crédit
		  
		  sprintf(szTerminal,"%-1.1s%-3.3s\0", lpGalxSend->aRegister, lpGalxSend->aTermNum);
	      lpRefund->Request.wTerminal = (WORD) atoi(szTerminal);

		  memcpy(szSystem, lpGalxSend->aCtrlNum, 4);
		  lpRefund->Request.wSystem = (WORD)atoi(szSystem);
	
		  memcpy(szTrn, &lpGalxSend->aTrnsNum[2], 4);
		  lpRefund->Request.wTransaction = (WORD) atoi(szTrn);

		  memcpy(lpRefund->Request.szCashier, &lpGalxSend->aCashNum[1],6);

		  lpRefund->Request.byCardStatusCode = 2;
		  lpRefund->Request.byCardCallCode = 0;


		  int i,j;

		  i=0, j=0;

		  while (true)
		  {
			
			if (memcmp(&lpGalxSend->aPanNum[j], "0", 1) != 0) break;
			j++;
		  }

		  memcpy(&szPan[0],";",1);
		  memcpy(&szPan[1],&lpGalxSend->aPanNum[j],19-j);

		  i=19-j+1;

          memcpy(&szPan[i],"=",1);
		  memcpy(&szPan[i+1], &lpGalxSend->aEndValid[2],2);
		  memcpy(&szPan[i+3], lpGalxSend->aEndValid,2);
		  memcpy(&szPan[i+5], "101",3);

		  for (j=i+8; j < 40 ; j++)
		  {
			memcpy(&szPan[j],"0",1);
		  }
	
		  memcpy(&szPan[39],"?",1);

		  memcpy(lpRefund->Request.szCardTrack2Data, szPan, strlen(szPan));

		  memset(lpRefund->Request.abyAlign,0x00,1);

		  memcpy(szDevise, lpGalxSend->aCurrencyCode, 3);
		  lpRefund->Request.wCurrency = (WORD) atoi(szDevise);

		  memcpy(szAmount,lpGalxSend->aAmount,12);
		  lpRefund->Request.dwAmount = (long) atoi(szAmount);


		  memcpy(lpRefund->Request.szNumAuto, "          ", 10);

	
		  // Formatage de l'horodatage.
		  //----------------------------

		  time_t time_c;
		  struct tm *dat_j;

		  /* Appel fonction date systeme */
		  time (&time_c);
		  dat_j = localtime (&time_c);
	
		  lpDateTime->wYear = dat_j->tm_year - 100;
		  lpDateTime->byMonth = dat_j->tm_mon + 1;
		  lpDateTime->byDay = dat_j->tm_mday;
		  lpDateTime->byHour = dat_j->tm_hour;
		  lpDateTime->byMin = dat_j->tm_min;
		}


return Ret;
}




bool TraiteResa_EcranMonetique(char *Msg, lpCcPosMsgSend Galaxy)
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
    Sleep(200);
	if (memcmp("VADCANCEL",(char *)MPtr,9) == 0) // Bouton Cancel VAD
	{
		SendMessage (FindWindow(NULL,"monetique"), WM_USER+9, 0L, 0L);
		sprintf ((char *) MPtr, "                             \0");
		DestroyMappingFile(MPtr);
		return FALSE;
	}
	if (memcmp("VADACCEPT",(char *)MPtr,9) == 0) //Bouton OK VAD
	{
		Sleep(300);
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

		memset (Galaxy->aPanNum,'0', sizeof(Galaxy->aPanNum));
		lg = sizeof(Galaxy->aPanNum) - strlen(szBin);
		memcpy (&Galaxy->aPanNum[lg],szBin, strlen(szBin));
		
		return TRUE;
	}
  }

  return FALSE;
}


bool LectureCarte(char *Msg, lpCcPosMsgSend Galaxy)
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
    Sleep(200);
	if (memcmp("VADCANCEL",(char *)MPtr,9) == 0) // Bouton Cancel VAD
	{
		SendMessage (FindWindow(NULL,"monetique"), WM_USER+9, 0L, 0L);
		sprintf ((char *) MPtr, "                             \0");
		DestroyMappingFile(MPtr);
		return FALSE;
	}
	if (memcmp("VADACCEPT",(char *)MPtr,9) == 0) //Bouton OK VAD
	{
		Sleep(300);
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

		memset (Galaxy->aPanNum,'0', sizeof(Galaxy->aPanNum));
		lg = sizeof(Galaxy->aPanNum) - strlen(szBin);
		memcpy (&Galaxy->aPanNum[lg],szBin, strlen(szBin));
		
		return TRUE;
	}
  }

  return FALSE;
}



      ////////////////////////////////////////////////////////////////////////////////////////////////
     //                                                                                            //
    // Mars 2007 - SJU                                                                            //
   //                                                                                            //
  //     Formate le message de confirmation vers le protocole Résa CRO                          //
 //                                                                                            //
///////////////////////////////////////////////////////////////////////////////////////////////


bool FormateCommitPourResa(CcPosMsgSend *lpGalxSend,
						ON2_LPPAYMENT lpPayment,
						STM_LPDATETIME lpDateTime,
						ON2_LPCOMMIT lpCommit)
{
bool Ret = true;
	
	memset(lpCommit->szCardCertificate, 0x00,sizeof(lpCommit->szCardCertificate));	
	memcpy(lpCommit->szCardCertificate, "NO",2);	

	memset(lpCommit->abyAlign, 0x00, sizeof(lpCommit->abyAlign));

	memcpy(lpCommit->Payment.byForcePostFlag, lpPayment->byForcePostFlag, sizeof(lpPayment->byForcePostFlag));
	memcpy(lpCommit->Payment.szApprovalCode, lpPayment->szApprovalCode, sizeof(lpPayment->szApprovalCode));
	memcpy(lpCommit->Payment.Request.abyAlign, lpPayment->Request.abyAlign, sizeof(lpPayment->Request.abyAlign));
	lpCommit->Payment.Request.byCardCallCode = lpPayment->Request.byCardCallCode;
	lpCommit->Payment.Request.byCardStatusCode = lpPayment->Request.byCardStatusCode;
	lpCommit->Payment.Request.dwAmount =  lpPayment->Request.dwAmount;
	memcpy(lpCommit->Payment.Request.szCardTrack2Data, lpPayment->Request.szCardTrack2Data, sizeof(lpPayment->Request.szCardTrack2Data));
	memcpy(lpCommit->Payment.Request.szCashier, lpPayment->Request.szCashier, sizeof(lpPayment->Request.szCashier));
	memcpy(lpCommit->Payment.Request.szNumAuto, lpPayment->Request.szNumAuto, sizeof(lpPayment->Request.szNumAuto));
	lpCommit->Payment.Request.wCurrency = lpPayment->Request.wCurrency;
	lpCommit->Payment.Request.wSystem = lpPayment->Request.wSystem;
	lpCommit->Payment.Request.wTerminal = lpPayment->Request.wTerminal;
	lpCommit->Payment.Request.wTransaction = lpPayment->Request.wTransaction;
    lpCommit->Payment.Request.wCvx2 = lpPayment->Request.wCvx2;

	
	// Formatage de l'horodatage.
	//----------------------------

	time_t time_c;
	struct tm *dat_j;

	/* Appel fonction date systeme */
	time (&time_c);
	dat_j = localtime (&time_c);
	
	lpDateTime->wYear = dat_j->tm_year - 100;
	lpDateTime->byMonth = dat_j->tm_mon + 1;
	lpDateTime->byDay = dat_j->tm_mday;
	lpDateTime->byHour = dat_j->tm_hour;
	lpDateTime->byMin = dat_j->tm_min;
	


	

return Ret;
}

      ////////////////////////////////////////////////////////////////////////////////////////////////
     //                                                                                            //
    // Mars 2007 - SJU                                                                            //
   //                                                                                            //
  //     Formate le message de confirmation vers le protocole Résa CRO                          //
 //                                                                                            //
///////////////////////////////////////////////////////////////////////////////////////////////


bool FormateCommitCreditPourResa(CcPosMsgSend *lpGalxSend,
						ON2_LPREFUND lpRefund,
						STM_LPDATETIME lpDateTime,
						ON2_LPCOMMIT lpCommit)
{
bool Ret = true;
	
	memset(lpCommit->szCardCertificate, 0x00,sizeof(lpCommit->szCardCertificate));	
	memcpy(lpCommit->szCardCertificate, "NO",2);	

	memset(lpCommit->abyAlign, 0x00, sizeof(lpCommit->abyAlign));

	memset(lpCommit->Payment.byForcePostFlag, '0', sizeof(lpCommit->Payment.byForcePostFlag));
	memset(lpCommit->Payment.szApprovalCode, '0', sizeof(lpCommit->Payment.szApprovalCode));
	memcpy(lpCommit->Payment.Request.abyAlign, lpRefund->Request.abyAlign, sizeof(lpRefund->Request.abyAlign));
	lpCommit->Payment.Request.byCardCallCode = lpRefund->Request.byCardCallCode;
	lpCommit->Payment.Request.byCardStatusCode = lpRefund->Request.byCardStatusCode;
	lpCommit->Payment.Request.dwAmount =  lpRefund->Request.dwAmount;
	memcpy(lpCommit->Payment.Request.szCardTrack2Data, lpRefund->Request.szCardTrack2Data, sizeof(lpRefund->Request.szCardTrack2Data));
	memcpy(lpCommit->Payment.Request.szCashier, lpRefund->Request.szCashier, sizeof(lpRefund->Request.szCashier));
	memcpy(lpCommit->Payment.Request.szNumAuto, lpRefund->Request.szNumAuto, sizeof(lpRefund->Request.szNumAuto));
	lpCommit->Payment.Request.wCurrency = lpRefund->Request.wCurrency;
	lpCommit->Payment.Request.wSystem = lpRefund->Request.wSystem;
	lpCommit->Payment.Request.wTerminal = lpRefund->Request.wTerminal;
	lpCommit->Payment.Request.wTransaction = lpRefund->Request.wTransaction;
	lpCommit->Payment.Request.wCvx2 = lpRefund->Request.wCvx2;

	
	// Formatage de l'horodatage.
	//----------------------------

	time_t time_c;
	struct tm *dat_j;

	/* Appel fonction date systeme */
	time (&time_c);
	dat_j = localtime (&time_c);
	
	lpDateTime->wYear = dat_j->tm_year - 100;
	lpDateTime->byMonth = dat_j->tm_mon + 1;
	lpDateTime->byDay = dat_j->tm_mday;
	lpDateTime->byHour = dat_j->tm_hour;
	lpDateTime->byMin = dat_j->tm_min;
	


	

return Ret;
}
