Attribute VB_Name = "MoSend"
'====================================================================================
'Juin.06 - SJ
'-----------
'
'                  Module de gestion d'envoi de trame IP
'
'
'====================================================================================


' Définition des fonctions extérieures
' ------------------------------------

Private Declare Function Chiffre Lib "DlpHC.dll" Alias "EncodeDecode" (Msg As S6)



'   Définition des structures
'----------------------------

Type S6
    cOperation  As String * 1
    cInfoAller  As String * 512
    cInfoRetour As String * 512
    cCodeRetour As Integer
    cfiller     As String * 128
End Type



'------------------------------------------------------------------------------
'   Déclaration des variables globales pour communication avec DLPCC
'------------------------------------------------------------------------------'
Public glHote As String
Public glTermOn2 As String
Public glChaine As String
Public tFolio As String
Public bMonitor As Boolean
Public glDat, glWrk, glCurrent As Integer
Public Msgc As S6


Const cLogFile As String = "HCLog.txt"






'------------------------------------------------------------------------------------
' Fonction LitHcCtxDlp
'
' Type : booléen.
' Paramètres : Aucun.
' But : Ouvre le fichier HcCtxDlp en lecture et renseigne des variables.
' Retour : VRAI / FAUX.
'
' Créer le 16/06/06 par SJ.
'
':-------------------------------------------------------------:--------------------:
'                         Modification                         :        DATE        :
':-------------------------------------------------------------:--------------------:
':                                                             :                    :
':-------------------------------------------------------------:--------------------:
'
'
'------------------------------------------------------------------------------------

Function LitHcCtxDlp() As Boolean

On Error GoTo errLitHcCtxDlp

Dim Ret As Variant
Dim Ligne As String
Dim spc1, spc2, spc3 As Integer

 Open "HcCtxDlp" For Input As #1
  
 Ligne = "": spc1 = 0: spc2 = 0: spc3 = 0
 glHote = "": glTermOn2 = ""
 LitHcCtxDlp = False
  
 Do While Not EOF(1)
            
   Input #1, Ligne
   
   If Left$(Ligne, 1) <> "#" Then  'Pas en commentaire
    If InStr(1, Ligne, "STRATUS_COM") > 0 Then
     spc1 = InStr(InStr(1, Ligne, "=") + 1, Ligne, " ")
     spc2 = InStr(spc1 + 1, Ligne, " ")
     spc3 = InStr(spc2 + 1, Ligne, " ")
     If spc3 = 0 Then spc3 = Len(Ligne) + 1
    
     glHote = Trim$(Mid$(Ligne, spc1, spc2 - spc1))
     glTermOn2 = Trim$(Mid$(Ligne, spc2, spc3 - spc2))
     Exit Do
    End If
   End If
             
 Loop

 Close #1

 LitHcCtxDlp = True
Exit Function

errLitHcCtxDlp:
If bMonitor Then
  Ret = MsgBox("LitHcCtxDlp Erreur : " & Err.Number & " - " & Err.Description, _
               vbOKOnly + vbCritical, "MoSend")
End If
  
  Call LogErreur("MoSend", "LitHcCtxDlp Erreur : " & Err.Number & " - " & Err.Description)
  
  LitHcCtxDlp = False
  
End Function


Function GetLibTCP(Num As Variant) As String
On Error GoTo ErrGetLibTCP

Dim Ret As Variant

Select Case Num
    Case sckClosed: GetLibTCP = "Socket Fermée"
    Case sckOpen: GetLibTCP = "Socket Ouverte"
    Case sckListening: GetLibTCP = "Socket à l'écoute"
    Case sckConnectionPending: GetLibTCP = "Connexion en attente"
    Case sckResolvingHost: GetLibTCP = "Hote en cours de résolution"
    Case sckHostResolved: GetLibTCP = "Hote résolu"
    Case sckConnecting: GetLibTCP = "En cours de connexion"
    Case sckConnected: GetLibTCP = "Socket connectée"
    Case sckClosing: GetLibTCP = "Connexion en cours de fermeture par l'homologue"
    Case sckError: GetLibTCP = "Erreur"
    Case Else: GetLibTCP = "Etat inconnu"
  End Select
  
Exit Function
ErrGetLibTCP:
 If bMonitor Then
  Ret = MsgBox("GetLibTCP Erreur : " & Err.Number & " - " & Err.Description, _
               vbOKOnly + vbCritical, "MoSend")
  End If
  Call LogErreur("MoSend", "GetLibTCP Erreur : " & Err.Number & " - " & Err.Description)
  GetLibTCP = "Pas de libellé"
End Function


'===============================================================================
' Fonction HtoD
' '-------------
'
'  Conversions Hexa vers Décimal
'
'  * Paramètres
'    ----------
'       La chaine Hexa à convertir en décimal
'  * Retour
'    ------
'       La chaine convertie en décimal
'
'===============================================================================
Function HtoD(Ligne As String) As String
On Error GoTo errHtoD

Dim Ret, tmpDec As Variant
Dim szTmp As String
Dim i, coef As Integer


  HtoD = ""
  tmpDec = 0
  
  For i = Len(Ligne) To 1 Step -1
  
    Select Case Len(Ligne) - i
     Case 0: coef = 1
     Case 1: coef = 16
     Case 2: coef = 256
     Case 3: coef = 4096
     Case 4: coef = 655536
    End Select
     
  
    Select Case Mid$(Ligne, i, 1)
     Case "0":
       tmpDec = tmpDec + 0 * coef
     Case "1":
       tmpDec = tmpDec + 1 * coef
     Case "2":
       tmpDec = tmpDec + 2 * coef
     Case "3":
       tmpDec = tmpDec + 3 * coef
     Case "4":
       tmpDec = tmpDec + 4 * coef
     Case "5":
       tmpDec = tmpDec + 5 * coef
     Case "6":
       tmpDec = tmpDec + 6 * coef
     Case "7":
       tmpDec = tmpDec + 7 * coef
     Case "8":
       tmpDec = tmpDec + 8 * coef
     Case "9":
       tmpDec = tmpDec + 9 * coef
     Case "A", "a":
       tmpDec = tmpDec + 10 * coef
     Case "B", "b":
       tmpDec = tmpDec + 11 * coef
     Case "C", "c":
       tmpDec = tmpDec + 12 * coef
     Case "D", "d":
       tmpDec = tmpDec + 13 * coef
     Case "E", "e":
       tmpDec = tmpDec + 14 * coef
     Case "F", "f":
       tmpDec = tmpDec + 15 * coef
    End Select
  Next i
  
  HtoD = tmpDec
Exit Function

errHtoD:
If bMonitor Then
  Ret = MsgBox("HtoD : " & Err.Number & " - " & Err.Description, _
               vbOKOnly + vbCritical, "MoSend")
End If
  Call LogErreur("MoSend", "HtoD: " & Err.Number & " - " & Err.Description)

               
HtoD = " "
End Function



'------------------------------------------------------------------------------------
' Fonction LectureMsgHcOff
'
' Type : booléen.
' Paramètres : Aucun.
' But : Ouvre le fichier MsgHcOff.wrk, le lit et le charge en mémoire.
' Retour : VRAI / FAUX.
'
' Créer le 07/07/07 par SJ.
'
':-------------------------------------------------------------:--------------------:
'                         Modification                         :        DATE        :
':-------------------------------------------------------------:--------------------:
':                                                             :                    :
':-------------------------------------------------------------:--------------------:
'
'
'------------------------------------------------------------------------------------

Function LectureMsgHcOff(Msg As String) As Boolean

On Error GoTo errLectureMsgHcOff

Dim Ret As Variant
Dim Ligne, FichierLu, szTmp As String
Dim Lu As Variant
Dim spc1, spc2, spc3 As Integer
Dim fs, f, ts

Const ForReading = 1, ForWriting = 2, ForAppending = 3
Const TristateUseDefault = -2, TristateTrue = -1, TristateFalse = 0



 FichierLu = " ": szTmp = " ": Ligne = " "
 Set fs = CreateObject("Scripting.FileSystemObject")

 If fs.FileExists("CurrentHcOff.wrk") Then
  If TailleFichier("CurrentHcOff.wrk") > 0 Then
    FichierLu = "CurrentHcOff.wrk"
  Else
    Ret = SupprimeFichier("CurrentHcOff.wrk")
    glCurrent = 0: Ftcp.Txt_Current.Text = glCurrent: DoEvents
    FichierLu = "MsgHcOff.wrk"
  End If
 Else
  FichierLu = "MsgHcOff.wrk"
  If Not fs.FileExists(FichierLu) Then
    Msg = "VIDE"
    LectureMsgHcOff = True
    Exit Function
  End If
 End If

 Open FichierLu For Input As #10
 
 LectureMsgHcOff = False
  
 Line Input #10, Ligne
 
 If FichierLu = "MsgHcOff.wrk" Then
   fs.CreateTextFile "CurrentHcOff.wrk"
   Set f = fs.GetFile("CurrentHcOff.wrk")
   Set ts = f.OpenAsTextStream(ForWriting, TristateFalse)
   ts.Write Ligne
   ts.Close
   glWrk = 0
   glCurrent = 1: Ftcp.Txt_Current = glCurrent: DoEvents
   
   fs.CreateTextFile "MsgHcOff.new"
   Set f = fs.GetFile("MsgHcOff.new")
   Set ts = f.OpenAsTextStream(ForWriting, TristateFalse)
   Do While Not EOF(10)
     glWrk = glWrk + 1
     Line Input #10, szTmp
     ts.Write szTmp & vbCrLf
   Loop
   ts.Close
 End If
 
 Ftcp.Txt_Bloc1Fin.Text = glWrk: DoEvents
 Ftcp.Txt_Bloc1Deb.Text = glDat: DoEvents
 Ftcp.Txt_Current = glCurrent: DoEvents
 
 Close #10

 LectureMsgHcOff = True
 
 Msg = Ligne
 
Exit Function

errLectureMsgHcOff:
  If bMonitor Then
    Ret = MsgBox("LectureMsgHcOff Erreur : " & Err.Number & " - " & Err.Description, _
                  vbOKOnly + vbCritical, "MoSend")
  End If
  Call LogErreur("MoSend", "LectureMsgHcOff : " & Err.Number & " - " & Err.Description)
  LitMsgHcOff = False
End Function


'------------------------------------------------------------------------------------
' Fonction RenommeDatEnWrk
'
' Type : booléen.
' Paramètres : Aucun.
' But : Renomme le fichier MsgHcOff.dat en MsgHcOff.wrk
' Retour : VRAI / FAUX.
'
' Créer le 07/07/07 par SJ.
'
':-------------------------------------------------------------:--------------------:
'                         Modification                         :        DATE        :
':-------------------------------------------------------------:--------------------:
':                                                             :                    :
':-------------------------------------------------------------:--------------------:
'
'
'------------------------------------------------------------------------------------

Function RenommeDatEnWrk() As Boolean

On Error GoTo errRenommeDatEnWrk

Dim Ret As Variant
Dim fs

 RenommeDatEnWrk = False
 Set fs = CreateObject("Scripting.FileSystemObject")
 
 If Not fs.FileExists("MsgHcOff.dat") Then
   Call LogErreur("MoSend", "RenommeDatEnWrk : Pas de rattrapage dans le fichier DAT")
   RenommeDatEnWrk = True
   Exit Function
 End If
  
 If Not fs.FileExists("MsgHcOff.wrk") Then
   Call LogErreur("MoSend", "Renomme Dat en wrk")
   fs.MoveFile "MsgHcOff.dat", "MsgHcOff.wrk"
   RenommeDatEnWrk = True
 Else
   Call LogErreur("MoSend", "RenommeDatEnWrk : Rattrapage existants dans le fichier WRK")
 End If
 

Exit Function

errRenommeDatEnWrk:
If bMonitor Then
  Ret = MsgBox("RenommeDatEnWrk Erreur : " & Err.Number & " - " & Err.Description, _
               vbOKOnly + vbCritical, "MoSend")
  End If
  Call LogErreur("MoSend", "RenommeDatEnWrk : " & Err.Number & " - " & Err.Description)
  RenommeDatEnWrk = False
End Function


Function Connect() As Boolean
On Error GoTo errConnect

Dim Ret As Variant
Dim Timeout As Boolean
Dim TimeStart As Date

 Connect = False: Timeout = False
 

 Timeout = False
 Ftcp.WinSockOn2.RemoteHost = glHote
 Ftcp.WinSockOn2.RemotePort = glTermOn2
  
 TimeStart = Now()
 Ftcp.WinSockOn2.Connect
 DoEvents
 Ftcp.pbConnect.Enabled = Not Ftcp.pbConnect.Enabled
 Ftcp.pbDeconnexion.Enabled = Not Ftcp.pbDeconnexion.Enabled
    
  While Ftcp.WinSockOn2.State <> sckConnected And Not Timeout
   
    If DateDiff("s", TimeStart, Now()) > 10 Then
      Timeout = Not Timeout
    End If
    DoEvents
  Wend
   
  lbEtat = GetLibTCP(Ftcp.WinSockOn2.State)
  DoEvents
  
  If Not Timeout Then
    Connect = True
   Else
    If bMonitor Then
      Ret = MsgBox("Connect : " & Ftcp.WinSockOn2.State & " - " & _
                    GetLibTCP(Ftcp.WinSockOn2.State), _
                    vbOKOnly + vbCritical, "MoSend")
      End If

     Call LogErreur("MoSend", "Connect : " & Ftcp.WinSockOn2.State & _
                    " - " & GetLibTCP(Ftcp.WinSockOn2.State))
                    
     Connect = False
   End If
Exit Function

errConnect:
  If bMonitor Then
     Ret = MsgBox("Connect : " & Err.Number & " - " & Err.Description, _
                   vbOKOnly + vbCritical, "MoSend")
  End If
  Call LogErreur("MoSend", "Connect : " & Err.Number & " - " & Err.Description)
  
End Function


'------------------------------------------------------------------------------------
' Fonction LogErreur
'
' Type : Aucun.
' Paramètres : Titre de la fenêtre, Erreur rencontrée.
' But : Ecrit l'erreur dans le fichier
' Retour : Aucun.
'
' Créer le 11/07/07 par SJ.
'
':-------------------------------------------------------------:--------------------:
'                         Modification                         :        DATE        :
':-------------------------------------------------------------:--------------------:
':                                                             :                    :
':-------------------------------------------------------------:--------------------:
'
'
'------------------------------------------------------------------------------------
Sub LogErreur(Titre As String, Erreur As String)

Dim fs
Dim TailleMax As Double

TailleMax = 131072
 
 Set fs = CreateObject("Scripting.FileSystemObject")
 
 If fs.FileExists(cLogFile) Then
   If TailleFichier(cLogFile) > TailleMax Then '128 Ko
     If fs.FileExists(Trim$(Left$(cLogFile, Len(Trim$(cLogFile)) - 4)) & ".old") Then
       fs.deletefile Trim$(Left$(cLogFile, Len(Trim$(cLogFile)) - 4)) & ".old", True
     End If
     fs.MoveFile cLogFile, Trim$(Left$(cLogFile, Len(Trim$(cLogFile)) - 4)) & ".old"
   End If
 End If
 
 Open cLogFile For Append As #1
  Write #1, Format(Now(), "dd-mm-yy hh:mm:ss") & "|" & Titre & "|" & Erreur
 Close #1
End Sub

'------------------------------------------------------------------------------------
' Fonction TraiteHC
'
' Type : booléen.
' Paramètres : Aucun.
' But : Prend une ligne du fichier et l'envoi.
' Retour : VRAI / FAUX.
'
' Créer le 09/07/07 par SJ.
'
':-------------------------------------------------------------:--------------------:
'                         Modification                         :        DATE        :
':-------------------------------------------------------------:--------------------:
':                                                             :                    :
':-------------------------------------------------------------:--------------------:
'
'
'------------------------------------------------------------------------------------

Function TraiteHC() As Boolean

On Error GoTo errTraiteHC

Dim TimeStart, Ret As Variant
Dim szTemp As String
Dim Timeout As Boolean

   Timeout = False: TraiteHC = False
  
  If Not RenommeDatEnWrk() Then
    Call LogErreur("TraiteHC", "Fichier DAT non renommé")
  End If
  
  If TailleFichier("MsgHcOff.wrk") = 0 Then
   Ret = SupprimeFichier("MsgHcOff.wrk")
   glWrk = 0: Ftcp.Txt_Bloc1Fin.Text = glWrk: DoEvents
  End If
  If LectureMsgHcOff(szTemp) Then
    If szTemp <> "VIDE" Then
      If Connect() Then
        Msgc.cOperation = "E"
        Msgc.cInfoAller = (szTemp)
        Call Chiffre(Msgc)
        'Ftcp.WinSockOn2.SendData (Decode(szTemp, Len(szTemp)))
        Ftcp.WinSockOn2.SendData (szTemp)
        Call LogErreur("Envoi", szTemp)
        DoEvents
        Ftcp.WinSockOn2.Close
        Ftcp.pbConnect.Enabled = Not Ftcp.pbConnect.Enabled
        Ftcp.pbDeconnexion.Enabled = Not Ftcp.pbDeconnexion.Enabled
        DoEvents
        SupprimeFichier ("CurrentHcOff.wrk")
        glCurrent = 0: Ftcp.Txt_Current = glCurrent: DoEvents
        If Not RenommeNewEnWrk() Then
            Call LogErreur("TraiteHC", "Pas de renommage possible sur fichier VIDE")
        End If
        
        TraiteHC = True
       Else 'Connexion IP KO
          If bMonitor Then
          Ret = MsgBox("Impossible de démarrer la connexion IP", _
                       vbExclamation + vbOKOnly, "TraiteHC")
          End If
          Call LogErreur("TraiteHC", _
              "Impossible de démarrer la connexion IP : " & Err.Number & " - " & Err.Description)
       End If 'Connexion IP
    Else 'Fichier Vide
          If bMonitor Then
            Ret = MsgBox("Rattrapage terminé.", vbExclamation + vbOKOnly, "TraiteHC")
          End If
          glWrk = 0: Ftcp.Txt_Bloc1Fin.Text = glWrk: DoEvents
          glDat = 0: Ftcp.Txt_Bloc1Deb.Text = glDat: DoEvents
          glCurrent = 0: Ftcp.Txt_Current = glCurrent: DoEvents
          'Call LogErreur("TraiteHC", "Rattrapage terminé.")
          SupprimeFichier ("MsgHcOff.wrk")
          If Not RenommeDatEnWrk() Then
            Call LogErreur("TraiteHC", "Renommage impossible")
            TraiteHC = False
            Exit Function
          End If
          TraiteHC = True
    End If
  Else 'Lecture wrk KO
    If bMonitor Then
      Ret = MsgBox("Lecture fichier impossible.", vbExclamation + vbOKOnly, "TraiteHC")
     End If
     Call LogErreur("TraiteHC", "Lecture fichier impossible.")

  End If 'Lecture wrk
 

Exit Function

errTraiteHC:
If bMonitor Then
  Ret = MsgBox("TraiteHC Erreur : " & Err.Number & " - " & Err.Description, _
               vbOKOnly + vbCritical, "MoSend")
End If
  Call LogErreur("MoSend", "TraiteHC : " & Err.Number & " - " & Err.Description)
  TraiteHC = False
End Function


'------------------------------------------------------------------------------------
' Fonction GetMonitorState
'
' Type : booléen.
' Paramètres : Aucun.
' But : Cherche le fichier Monitor
' Retour : VRAI si Monitor est trouvé
'          FAUX si Monitor non trouvé
'
' Créer le 07/07/07 par SJ.
'
':-------------------------------------------------------------:--------------------:
'                         Modification                         :        DATE        :
':-------------------------------------------------------------:--------------------:
':                                                             :                    :
':-------------------------------------------------------------:--------------------:
'
'
'------------------------------------------------------------------------------------

Function GetMonitorState() As Boolean

On Error GoTo errGetMonitorState
Dim Ret As Variant
Dim fs

 GetMonitorState = False
 Set fs = CreateObject("Scripting.FileSystemObject")
 
 If fs.FileExists("Monitor") Then
  GetMonitorState = True
  Else
  GetMonitorState = False
 End If
 

Exit Function

errGetMonitorState:

  Call LogErreur("MoSend", "GetMonitorState : " & Err.Number & " - " & Err.Description)
  GetMonitorState = False
End Function


'------------------------------------------------------------------------------------
' Fonction SupprimeFichier
'
' Type : booléen.
' Paramètres : Nom du fichier à effacer.
' But : destruction d'un fichier
' Retour : VRAI / FAUX.
'
' Créer le 11/07/07 par SJ.
'
':-------------------------------------------------------------:--------------------:
'                         Modification                         :        DATE        :
':-------------------------------------------------------------:--------------------:
':                                                             :                    :
':-------------------------------------------------------------:--------------------:
'
'
'------------------------------------------------------------------------------------

Function SupprimeFichier(NomFichier) As Boolean

On Error GoTo errSupprimeFichier

Dim fs

 SupprimeFichier = False
 Set fs = CreateObject("Scripting.FileSystemObject")
 
 If fs.FileExists(NomFichier) Then
   fs.deletefile NomFichier
   SupprimeFichier = True
 End If
 
Exit Function
errSupprimeFichier:
  Call LogErreur("MoSend", "SupprimeFichier : " & Err.Number & " - " & Err.Description)
  SupprimeFichier = False

End Function


'------------------------------------------------------------------------------------
' Fonction SupprimeFichier
'
' Type : booléen.
' Paramètres : Nom du fichier à effacer.
' But : destruction d'un fichier
' Retour : VRAI / FAUX.
'
' Créer le 11/07/07 par SJ.
'
':-------------------------------------------------------------:--------------------:
'                         Modification                         :        DATE        :
':-------------------------------------------------------------:--------------------:
':                                                             :                    :
':-------------------------------------------------------------:--------------------:
'
'
'------------------------------------------------------------------------------------

Function TailleFichier(NomFichier) As Double

On Error GoTo errTailleFichier

Dim fs, f

 TailleFichier = False
 Set fs = CreateObject("Scripting.FileSystemObject")
 
 If fs.FileExists(NomFichier) Then
   Set f = fs.GetFile(NomFichier)
   TailleFichier = f.Size
 End If
 
Exit Function
errTailleFichier:
  Call LogErreur("MoSend", "TailleFichier : " & Err.Number & " - " & Err.Description)
  TailleFichier = False

End Function

'------------------------------------------------------------------------------------
' Fonction RenommeDatEnWrk
'
' Type : booléen.
' Paramètres : Aucun.
' But : Renomme le fichier MsgHcOff.dat en MsgHcOff.wrk
' Retour : VRAI / FAUX.
'
' Créer le 07/07/07 par SJ.
'
':-------------------------------------------------------------:--------------------:
'                         Modification                         :        DATE        :
':-------------------------------------------------------------:--------------------:
':                                                             :                    :
':-------------------------------------------------------------:--------------------:
'
'
'------------------------------------------------------------------------------------

Function RenommeNewEnWrk() As Boolean

On Error GoTo errRenommeNewEnWrk

Dim Ret As Variant
Dim fs

 RenommeNewEnWrk = False
 Set fs = CreateObject("Scripting.FileSystemObject")
 
 If Not fs.FileExists("MsgHcOff.New") Then
   Call LogErreur("MoSend", "RenommeNewEnWrk : Pas de fichier NEW")
   RenommeNewEnWrk = True
   Exit Function
 End If
  
 If fs.FileExists("MsgHcOff.new") Then
   If SupprimeFichier("MsgHcOff.wrk") Then
     fs.MoveFile "MsgHcOff.new", "MsgHcOff.wrk"
     RenommeNewEnWrk = True
   End If
 Else
   Call LogErreur("MoSend", "RenommeNewEnWrk : Fichier NEW inexistant")
 End If
 

Exit Function

errRenommeNewEnWrk:
If bMonitor Then
  Ret = MsgBox("RenommeNewEnWrk Erreur : " & Err.Number & " - " & Err.Description, _
               vbOKOnly + vbCritical, "MoSend")
  End If
  Call LogErreur("MoSend", "RenommeNewEnWrk : " & Err.Number & " - " & Err.Description)
  RenommeNewEnWrk = False
End Function


'------------------------------------------------------------------------------------
' Fonction IsApplicationVisible
'
' Type : booléen.
' Paramètres : Nom du fichier à chercher.
' But : Affiche ou cache la fenêtre de l'application
' Retour : VRAI / FAUX.
'
' Créer le 12/07/07 par SJ.
'
':-------------------------------------------------------------:--------------------:
'                         Modification                         :        DATE        :
':-------------------------------------------------------------:--------------------:
':                                                             :                    :
':-------------------------------------------------------------:--------------------:
'
'
'------------------------------------------------------------------------------------

Function IsApplicationVisible() As Boolean

On Error GoTo errIsApplicationVisible

Dim fs, f

 IsApplicationVisible = False
 Set fs = CreateObject("Scripting.FileSystemObject")
 
 If fs.FileExists("MonitorHC") Then
  IsApplicationVisible = True
 End If
 
Exit Function
errIsApplicationVisible:
  Call LogErreur("MoSend", "IsApplicationVisible : " & Err.Number & " - " & Err.Description)
  IsApplicationVisible = False

End Function

