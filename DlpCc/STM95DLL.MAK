# Microsoft Developer Studio Generated NMAKE File, Based on STM95DLL.DSP
!IF "$(CFG)" == ""
CFG=STM95DLL - Win32 Debug
!MESSAGE No configuration specified. Defaulting to STM95DLL - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "STM95DLL - Win32 Release" && "$(CFG)" != "STM95DLL - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
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
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "STM95DLL - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release
# Begin Custom Macros
OutDir=.\Release
# End Custom Macros

ALL : "$(OUTDIR)\STM95DLL.dll" "$(OUTDIR)\STM95DLL.bsc"


CLEAN :
	-@erase "$(INTDIR)\apiDLPCC.obj"
	-@erase "$(INTDIR)\apiDLPCC.sbr"
	-@erase "$(INTDIR)\DlpCc.res"
	-@erase "$(INTDIR)\DLPCCDLL.obj"
	-@erase "$(INTDIR)\DLPCCDLL.sbr"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(OUTDIR)\STM95DLL.bsc"
	-@erase "$(OUTDIR)\STM95DLL.dll"
	-@erase "$(OUTDIR)\STM95DLL.exp"
	-@erase "$(OUTDIR)\STM95DLL.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MT /W4 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\STM95DLL.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32 
RSC_PROJ=/l 0x40c /fo"$(INTDIR)\DlpCc.res" /d "NDEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\STM95DLL.bsc" 
BSC32_SBRS= \
	"$(INTDIR)\apiDLPCC.sbr" \
	"$(INTDIR)\DLPCCDLL.sbr"

"$(OUTDIR)\STM95DLL.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib wsock32.lib /nologo /subsystem:windows /dll /incremental:no /pdb:"$(OUTDIR)\STM95DLL.pdb" /machine:I386 /out:"$(OUTDIR)\STM95DLL.dll" /implib:"$(OUTDIR)\STM95DLL.lib" 
LINK32_OBJS= \
	"$(INTDIR)\apiDLPCC.obj" \
	"$(INTDIR)\DLPCCDLL.obj" \
	"$(INTDIR)\DlpCc.res" \
	".\dlpc3dll.lib" \
	".\stm95dll.lib"

"$(OUTDIR)\STM95DLL.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

SOURCE="$(InputPath)"
DS_POSTBUILD_DEP=$(INTDIR)\postbld.dep

ALL : $(DS_POSTBUILD_DEP)

# Begin Custom Macros
OutDir=.\Release
# End Custom Macros

$(DS_POSTBUILD_DEP) : "$(OUTDIR)\STM95DLL.dll" "$(OUTDIR)\STM95DLL.bsc"
   copy Release\STM95DLL.DLL TestSTM95DLL\Release\*.* /v
	echo Helper for Post-build step > "$(DS_POSTBUILD_DEP)"

!ELSEIF  "$(CFG)" == "STM95DLL - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug
# Begin Custom Macros
OutDir=.\Debug
# End Custom Macros

ALL : "$(OUTDIR)\DLPCC.dll" "$(OUTDIR)\STM95DLL.bsc"


CLEAN :
	-@erase "$(INTDIR)\apiDLPCC.obj"
	-@erase "$(INTDIR)\apiDLPCC.sbr"
	-@erase "$(INTDIR)\DlpCc.res"
	-@erase "$(INTDIR)\DLPCCDLL.obj"
	-@erase "$(INTDIR)\DLPCCDLL.sbr"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\DLPCC.dll"
	-@erase "$(OUTDIR)\DLPCC.exp"
	-@erase "$(OUTDIR)\DLPCC.ilk"
	-@erase "$(OUTDIR)\DLPCC.lib"
	-@erase "$(OUTDIR)\DLPCC.pdb"
	-@erase "$(OUTDIR)\STM95DLL.bsc"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MTd /W4 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\STM95DLL.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32 
RSC_PROJ=/l 0x40c /fo"$(INTDIR)\DlpCc.res" /i "MonetiX.dll" /d "_DEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\STM95DLL.bsc" 
BSC32_SBRS= \
	"$(INTDIR)\apiDLPCC.sbr" \
	"$(INTDIR)\DLPCCDLL.sbr"

"$(OUTDIR)\STM95DLL.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
LINK32_FLAGS=wsock32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /incremental:yes /pdb:"$(OUTDIR)\DLPCC.pdb" /debug /machine:I386 /out:"$(OUTDIR)\DLPCC.dll" /implib:"$(OUTDIR)\DLPCC.lib" /pdbtype:sept 
LINK32_OBJS= \
	"$(INTDIR)\apiDLPCC.obj" \
	"$(INTDIR)\DLPCCDLL.obj" \
	"$(INTDIR)\DlpCc.res" \
	".\dlpc3dll.lib" \
	".\stm95dll.lib"

"$(OUTDIR)\DLPCC.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ENDIF 

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


!IF "$(NO_EXTERNAL_DEPS)" != "1"
!IF EXISTS("STM95DLL.DEP")
!INCLUDE "STM95DLL.DEP"
!ELSE 
!MESSAGE Warning: cannot find "STM95DLL.DEP"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "STM95DLL - Win32 Release" || "$(CFG)" == "STM95DLL - Win32 Debug"
SOURCE=.\apiDLPCC.cpp

"$(INTDIR)\apiDLPCC.obj"	"$(INTDIR)\apiDLPCC.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=.\DlpCc.RC

"$(INTDIR)\DlpCc.res" : $(SOURCE) "$(INTDIR)"
	$(RSC) $(RSC_PROJ) $(SOURCE)


SOURCE=.\DLPCCDLL.cpp

"$(INTDIR)\DLPCCDLL.obj"	"$(INTDIR)\DLPCCDLL.sbr" : $(SOURCE) "$(INTDIR)"



!ENDIF 

