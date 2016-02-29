VERSION 5.00
Object = "{248DD890-BB45-11CF-9ABC-0080C7E7B78D}#1.0#0"; "MSWINSCK.OCX"
Begin VB.Form Ftcp 
   Caption         =   "Sender HC"
   ClientHeight    =   5265
   ClientLeft      =   6675
   ClientTop       =   7200
   ClientWidth     =   5430
   LinkTopic       =   "Form1"
   ScaleHeight     =   5265
   ScaleWidth      =   5430
   Visible         =   0   'False
   Begin VB.CommandButton PbCacheAppli 
      Caption         =   "Cache l'application"
      Height          =   495
      Left            =   2760
      TabIndex        =   15
      Top             =   1320
      Width           =   2535
   End
   Begin VB.TextBox Text1 
      BackColor       =   &H00C0FFFF&
      Height          =   285
      Left            =   240
      Locked          =   -1  'True
      TabIndex        =   14
      TabStop         =   0   'False
      Text            =   "Test réseau"
      ToolTipText     =   "Etat de la connexion avec le serveur"
      Top             =   3480
      Width           =   975
   End
   Begin VB.CommandButton pbEtatApplication 
      Caption         =   "Application DESACTIVEE"
      Height          =   495
      Left            =   120
      TabIndex        =   12
      Top             =   1320
      Width           =   2535
   End
   Begin VB.TextBox Txt_Current 
      Height          =   285
      Left            =   3960
      TabIndex        =   11
      Text            =   "0"
      Top             =   3000
      Width           =   1095
   End
   Begin VB.TextBox Txt_Bloc1Fin 
      Height          =   285
      Left            =   2640
      TabIndex        =   9
      Text            =   "0"
      Top             =   3000
      Width           =   1095
   End
   Begin VB.TextBox Txt_Bloc1Deb 
      Height          =   285
      Left            =   1080
      TabIndex        =   5
      Text            =   "0"
      Top             =   3000
      Width           =   1095
   End
   Begin VB.CommandButton pbDeconnexion 
      Caption         =   "Déconnextion"
      Enabled         =   0   'False
      Height          =   495
      Left            =   2880
      TabIndex        =   4
      Top             =   3840
      Width           =   1695
   End
   Begin VB.CommandButton pbConnect 
      Caption         =   "Connexion"
      Height          =   495
      Left            =   960
      TabIndex        =   3
      Top             =   3840
      Width           =   1695
   End
   Begin VB.Timer TimerEtat 
      Enabled         =   0   'False
      Interval        =   30000
      Left            =   600
      Top             =   0
   End
   Begin VB.CommandButton pb_Sortie 
      Caption         =   "Quitter"
      Height          =   495
      Left            =   3960
      TabIndex        =   2
      Top             =   4680
      Width           =   1335
   End
   Begin VB.TextBox lbEtat 
      BackColor       =   &H00C0FFFF&
      Height          =   375
      Left            =   2520
      Locked          =   -1  'True
      TabIndex        =   0
      TabStop         =   0   'False
      ToolTipText     =   "Etat de la connexion avec le serveur"
      Top             =   240
      Width           =   2415
   End
   Begin MSWinsockLib.Winsock WinSockOn2 
      Left            =   0
      Top             =   0
      _ExtentX        =   741
      _ExtentY        =   741
      _Version        =   393216
   End
   Begin VB.Frame Frame1 
      Caption         =   "Test réseau"
      Height          =   975
      Left            =   120
      TabIndex        =   13
      Top             =   3480
      Width           =   4935
   End
   Begin VB.Label lblVersion 
      Caption         =   "Version 1.0 - Juillet 2006"
      Height          =   255
      Left            =   120
      TabIndex        =   16
      Top             =   4800
      Width           =   2055
   End
   Begin VB.Label Label9 
      Alignment       =   2  'Center
      BorderStyle     =   1  'Fixed Single
      Caption         =   "En cours de traitement"
      Height          =   735
      Left            =   3960
      TabIndex        =   10
      Top             =   2160
      Width           =   1095
   End
   Begin VB.Label Label6 
      Alignment       =   2  'Center
      BorderStyle     =   1  'Fixed Single
      Caption         =   "Queue"
      Height          =   255
      Left            =   120
      TabIndex        =   8
      Top             =   3000
      Width           =   855
   End
   Begin VB.Label Label5 
      Alignment       =   2  'Center
      BorderStyle     =   1  'Fixed Single
      Caption         =   "HC Caisse en attente de prise en charge"
      Height          =   735
      Left            =   1080
      TabIndex        =   7
      Top             =   2160
      Width           =   1215
   End
   Begin VB.Label Label4 
      Alignment       =   2  'Center
      BorderStyle     =   1  'Fixed Single
      Caption         =   "HC Caisse  pris en charge"
      Height          =   735
      Left            =   2640
      TabIndex        =   6
      Top             =   2160
      Width           =   1095
   End
   Begin VB.Label Label3 
      Caption         =   "Etat connexion TCP"
      BeginProperty Font 
         Name            =   "MS Sans Serif"
         Size            =   12
         Charset         =   0
         Weight          =   400
         Underline       =   0   'False
         Italic          =   0   'False
         Strikethrough   =   0   'False
      EndProperty
      Height          =   375
      Left            =   0
      TabIndex        =   1
      Top             =   360
      Width           =   2535
   End
End
Attribute VB_Name = "Ftcp"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = False

Private Sub Form_Load()
On Error GoTo Err_Form_Load_Ftcp
Dim Ret As Variant
Dim TimeStart As Date
Dim Timeout As Boolean
Dim i As Integer
Dim szTemp As String


If IsApplicationVisible() Then
  Ftcp.Visible = True
Else
  Ftcp.Visible = False
End If


 glChaine = "": szTemp = "": Timetout = False
 
 glDat = 0: glWrk = 0: glCurrent = 0
 
 bMonitor = GetMonitorState()
 
 If Not LitHcCtxDlp() Then
   If bMonitor Then
     Ret = MsgBox("Impossible de trouver le fichier HcCtxDlp", vbExclamation + vbOKOnly, "Chargement SenderHC")
   End If
   Call LogErreur("Chargement SenderHC", "Impossible de trouver le fichier HcCtxDlp")
   Exit Sub
 End If
 
 Ftcp.Caption = Ftcp.Caption & " " & glHote & " " & glTermOn2
 TimerEtat.Enabled = True
 pbEtatApplication.Caption = "Application ACTIVEE"
 lbEtat.Text = GetLibTCP(WinSockOn2.State)
 Call LogErreur("Chargement SenderHC", "Lancement OK")
 Exit Sub
 
Err_Form_Load_Ftcp:

  If bMonitor Then
    Ret = MsgBox("Form_Load  : " & Err.Number & " - " & Err.Description, _
                  vbOKOnly + vbCritical, "Ftcp")
  End If
               
  Call LogErreur("Ftcp", "Form_Load : " & Err.Number & " - " & Err.Description)

 
 
End Sub

Private Sub Form_Unload(Cancel As Integer)
  Ftcp.WinSockOn2.Close
End Sub

Private Sub pb_Sortie_Click()
 Unload Me
End Sub

Private Sub PbCacheAppli_Click()

  Ftcp.Visible = False
  SupprimeFichier ("MonitorHC")
End Sub

Private Sub pbConnect_Click()
On Error GoTo errpbConnect_Click

Dim Ret As Variant
Dim Timeout As Boolean

  Timeout = False
  Ftcp.WinSockOn2.RemoteHost = glHote
  Ftcp.WinSockOn2.RemotePort = glTermOn2
  
  TimeStart = Now()
  Ftcp.WinSockOn2.Connect
  DoEvents
  pbConnect.Enabled = Not pbConnect.Enabled
  pbDeconnexion.Enabled = Not pbDeconnexion.Enabled
    
   While Ftcp.WinSockOn2.State <> sckConnected And Not Timeout
   
     If DateDiff("s", TimeStart, Now()) > 10 Then
       Timeout = Not Timeout
     End If
     DoEvents
   Wend
   
    lbEtat.Text = GetLibTCP(sckConnected)
    DoEvents

Exit Sub

errpbConnect_Click:
Ret = MsgBox("errpbConnect_Click : " & Err.Number & " - " & Err.Description, _
               vbOKOnly + vbCritical, "FTcp")
  
End Sub

Private Sub pbDeconnexion_Click()
 Ftcp.WinSockOn2.Close
 
 DoEvents
  pbConnect.Enabled = Not pbConnect.Enabled
  pbDeconnexion.Enabled = Not pbDeconnexion.Enabled
  lbEtat.Text = GetLibTCP(WinSockOn2.State)
End Sub


Private Sub pbEtatApplication_Click()
  Dim Etat As Boolean
  
   If pbEtatApplication.Caption = "Application DESACTIVEE" Then
     TimerEtat = True
     pbEtatApplication.Caption = "Application ACTIVEE"
   Else
     TimerEtat = False
     pbEtatApplication.Caption = "Application DESACTIVEE"
   End If
   
End Sub

Private Sub TimerEtat_Timer()

Dim Ret As Variant
   
  If IsApplicationVisible() Then
    Ftcp.Visible = True
  Else
    Ftcp.Visible = False
  End If

  If Ftcp.WinSockOn2.State = sckClosed Then
    If Not TraiteHC() Then
      Ret = MsgBox("Timer d'envoi suspendu", vbOKOnly + vbCritical, "Ftcp")
      Call LogErreur("Ftcp", "Timer d'envoi suspendu")
      pbEtatApplication.Caption = "Application DESACTIVEE"
      TimerEtat.Enabled = False
      DoEvents
    End If
  Else
    If bMonitor Then
      Ret = MsgBox("TimerEtat Socket : " & Ftcp.WinSockOn2.State & " - " & _
                    GetLibTCP(Ftcp.WinSockOn2.State), _
                    vbOKOnly + vbCritical, "Ftcp")
     End If
     Call LogErreur("Ftcp", "TimerEtat Socket : " & Ftcp.WinSockOn2.State & _
                    " - " & GetLibTCP(Ftcp.WinSockOn2.State))
  End If
  DoEvents
End Sub




Private Sub WinSockOn2_Connect()
    'MsgBox ("Connexion réussie")
End Sub


