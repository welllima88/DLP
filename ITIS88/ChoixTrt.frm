VERSION 5.00
Begin {C62A69F0-16DC-11CE-9E98-00AA00574A4F} ChoixTrt 
   Caption         =   "Management Carte Media"
   ClientHeight    =   3120
   ClientLeft      =   45
   ClientTop       =   435
   ClientWidth     =   4860
   OleObjectBlob   =   "ChoixTrt.frx":0000
   StartUpPosition =   1  'CenterOwner
End
Attribute VB_Name = "ChoixTrt"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = False


Private Sub pbCancel_Click()
On Error GoTo ErrpbCancel_Click
Dim Ret As Variant
 Unload ChoixTrt
Exit Sub
ErrpbCancel_Click:
Ret = MsgBox("Erreur " & Err.Number & " " & Err.Description & vbCrLf & _
                vbOKOnly + vbCritical, "pbCancel_Click")
End Sub

Private Sub pbCreate_Click()
On Error GoTo ErrpbCreate_Click
Dim Ret As Variant
 CallSmed

Exit Sub
ErrpbCreate_Click:
Ret = MsgBox("Erreur " & Err.Number & " " & Err.Description & vbCrLf & _
                vbOKOnly + vbCritical, "pbCreate_Click")
End Sub

Private Sub pbDelete_Click()
On Error GoTo ErrpbDelete_Click
Dim Ret As Variant

DeleteSmed

Exit Sub
ErrpbDelete_Click:
Ret = MsgBox("Erreur " & Err.Number & " " & Err.Description & vbCrLf & _
                vbOKOnly + vbCritical, "pbDelete_Click")
End Sub
