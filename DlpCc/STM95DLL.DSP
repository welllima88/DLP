# Microsoft Developer Studio Project File - Name="STM95DLL" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=STM95DLL - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "STM95DLL.MAK".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "STM95DLL.MAK" CFG="STM95DLL - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "STM95DLL - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "STM95DLL - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "STM95DLL - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /MT /W4 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /FR /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x40c /d "NDEBUG"
# ADD RSC /l 0x40c /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib wsock32.lib /nologo /subsystem:windows /dll /machine:I386
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Cmds=copy C:\USERS\dlp\cc\DlpCc\Debug\dlpcc.dll C:\Sites\Test\Parking\dlpcc.dll
# End Special Build Tool

!ELSEIF  "$(CFG)" == "STM95DLL - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /MTd /W4 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /FD /c
# SUBTRACT CPP /Fr
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x40c /d "_DEBUG"
# ADD RSC /l 0x40c /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 wsock32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib stm95dll.lib /nologo /subsystem:windows /dll /incremental:no /debug /machine:I386 /out:"Debug/DLPCC.dll" /pdbtype:sept
# SUBTRACT LINK32 /pdb:none
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Cmds=copy C:\USERS\dlp\cc\DlpCc\Debug\dlpcc.dll C:\Sites\Test\IPP350
# End Special Build Tool

!ENDIF 

# Begin Target

# Name "STM95DLL - Win32 Release"
# Name "STM95DLL - Win32 Debug"
# Begin Source File

SOURCE=.\apiDLPc3.h
# End Source File
# Begin Source File

SOURCE=.\apiDLPCC.cpp

!IF  "$(CFG)" == "STM95DLL - Win32 Release"

!ELSEIF  "$(CFG)" == "STM95DLL - Win32 Debug"

# ADD CPP /vmb

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\apiDLPCC.h
# End Source File
# Begin Source File

SOURCE=.\apiOn2.cpp
# End Source File
# Begin Source File

SOURCE=.\apiON2.h
# End Source File
# Begin Source File

SOURCE=.\DlpCc.RC
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\DLPCCDLL.cpp
# End Source File
# Begin Source File

SOURCE=.\DLPCCDLL.def
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\DlpCcDLLpriv.h
# End Source File
# Begin Source File

SOURCE=.\dlpc3dll.lib
# End Source File
# Begin Source File

SOURCE=.\STM95DLL.lib
# End Source File
# End Target
# End Project
# Section STM95DLL : {DFAA6C74-1CEA-11D4-903B-0080C824A047}
# 	2:21:DefaultSinkHeaderFile:_mlcomtools.h
# 	2:16:DefaultSinkClass:C_MLComTools
# End Section
# Section STM95DLL : {DFAA6C73-1CEA-11D4-903B-0080C824A047}
# 	2:5:Class:C_MLComTools
# 	2:10:HeaderFile:_mlcomtools.h
# 	2:8:ImplFile:_mlcomtools.cpp
# End Section
