# Microsoft Developer Studio Generated NMAKE File, Based on DlpHc.dsp
!IF "$(CFG)" == ""
CFG=DlpHc - Win32 Debug
!MESSAGE No configuration specified. Defaulting to DlpHc - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "DlpHc - Win32 Release" && "$(CFG)" != "DlpHc - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "DlpHc.mak" CFG="DlpHc - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "DlpHc - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "DlpHc - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

!IF  "$(CFG)" == "DlpHc - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release
# Begin Custom Macros
OutDir=.\Release
# End Custom Macros

ALL : "$(OUTDIR)\DlpHc.dll"


CLEAN :
	-@erase "$(INTDIR)\DlpHc.obj"
	-@erase "$(INTDIR)\DlpHc.pch"
	-@erase "$(INTDIR)\MsgRecv.obj"
	-@erase "$(INTDIR)\MsgSend.obj"
	-@erase "$(INTDIR)\StdAfx.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(OUTDIR)\DlpHc.dll"
	-@erase "$(OUTDIR)\DlpHc.exp"
	-@erase "$(OUTDIR)\DlpHc.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "DLPHC_EXPORTS" /Fp"$(INTDIR)\DlpHc.pch" /Yu"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

.c{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.c{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

MTL=midl.exe
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /win32 
RSC=rc.exe
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\DlpHc.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /incremental:no /pdb:"$(OUTDIR)\DlpHc.pdb" /machine:I386 /out:"$(OUTDIR)\DlpHc.dll" /implib:"$(OUTDIR)\DlpHc.lib" 
LINK32_OBJS= \
	"$(INTDIR)\DlpHc.obj" \
	"$(INTDIR)\MsgRecv.obj" \
	"$(INTDIR)\MsgSend.obj" \
	"$(INTDIR)\StdAfx.obj"

"$(OUTDIR)\DlpHc.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "DlpHc - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug
# Begin Custom Macros
OutDir=.\Debug
# End Custom Macros

ALL : "$(OUTDIR)\DlpHc.dll"


CLEAN :
	-@erase "$(INTDIR)\DlpHc.obj"
	-@erase "$(INTDIR)\DlpHc.pch"
	-@erase "$(INTDIR)\MsgRecv.obj"
	-@erase "$(INTDIR)\MsgSend.obj"
	-@erase "$(INTDIR)\StdAfx.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\DlpHc.dll"
	-@erase "$(OUTDIR)\DlpHc.exp"
	-@erase "$(OUTDIR)\DlpHc.ilk"
	-@erase "$(OUTDIR)\DlpHc.lib"
	-@erase "$(OUTDIR)\DlpHc.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "DLPHC_EXPORTS" /Fp"$(INTDIR)\DlpHc.pch" /Yu"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

.c{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.c{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

MTL=midl.exe
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /win32 
RSC=rc.exe
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\DlpHc.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /incremental:yes /pdb:"$(OUTDIR)\DlpHc.pdb" /debug /machine:I386 /out:"$(OUTDIR)\DlpHc.dll" /implib:"$(OUTDIR)\DlpHc.lib" /pdbtype:sept 
LINK32_OBJS= \
	"$(INTDIR)\DlpHc.obj" \
	"$(INTDIR)\MsgRecv.obj" \
	"$(INTDIR)\MsgSend.obj" \
	"$(INTDIR)\StdAfx.obj"

"$(OUTDIR)\DlpHc.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ENDIF 


!IF "$(NO_EXTERNAL_DEPS)" != "1"
!IF EXISTS("DlpHc.dep")
!INCLUDE "DlpHc.dep"
!ELSE 
!MESSAGE Warning: cannot find "DlpHc.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "DlpHc - Win32 Release" || "$(CFG)" == "DlpHc - Win32 Debug"
SOURCE=.\DlpHc.cpp

"$(INTDIR)\DlpHc.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\DlpHc.pch"


SOURCE=.\MsgRecv.cpp

"$(INTDIR)\MsgRecv.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\DlpHc.pch"


SOURCE=.\MsgSend.cpp

"$(INTDIR)\MsgSend.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\DlpHc.pch"


SOURCE=.\StdAfx.cpp

!IF  "$(CFG)" == "DlpHc - Win32 Release"

CPP_SWITCHES=/nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "DLPHC_EXPORTS" /Fp"$(INTDIR)\DlpHc.pch" /Yc"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\StdAfx.obj"	"$(INTDIR)\DlpHc.pch" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "DlpHc - Win32 Debug"

CPP_SWITCHES=/nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "DLPHC_EXPORTS" /Fp"$(INTDIR)\DlpHc.pch" /Yc"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\StdAfx.obj"	"$(INTDIR)\DlpHc.pch" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 


!ENDIF 

