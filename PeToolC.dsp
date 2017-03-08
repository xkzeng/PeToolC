# Microsoft Developer Studio Project File - Name="PeToolC" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=PeToolC - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "PeToolC.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "PeToolC.mak" CFG="PeToolC - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "PeToolC - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "PeToolC - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "PeToolC - Win32 Release"

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
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x804 /d "NDEBUG"
# ADD RSC /l 0x804 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib comctl32.lib Shlwapi.lib /nologo /subsystem:windows /machine:I386
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Cmds=cscript /nologo cmd\CopyByTime.js Release\PeToolC.exe R
# End Special Build Tool

!ELSEIF  "$(CFG)" == "PeToolC - Win32 Debug"

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
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /GZ /c
# ADD CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x804 /d "_DEBUG"
# ADD RSC /l 0x804 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib comctl32.lib Shlwapi.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Cmds=cscript /nologo cmd\CopyByTime.vbs Debug\PeToolC.exe D
# End Special Build Tool

!ENDIF 

# Begin Target

# Name "PeToolC - Win32 Release"
# Name "PeToolC - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\MainWnd.c
# End Source File
# Begin Source File

SOURCE=.\PeFile.c
# End Source File
# Begin Source File

SOURCE=.\PeToolC.c
# End Source File
# Begin Source File

SOURCE=.\TabBlkTbl.c
# End Source File
# Begin Source File

SOURCE=.\TabDatDir.c
# End Source File
# Begin Source File

SOURCE=.\TabDosHdr.c
# End Source File
# Begin Source File

SOURCE=.\TabExpTbl.c
# End Source File
# Begin Source File

SOURCE=.\TabFilHdr.c
# End Source File
# Begin Source File

SOURCE=.\TabImpTbl.c
# End Source File
# Begin Source File

SOURCE=.\TabOptHdr.c
# End Source File
# Begin Source File

SOURCE=.\TabPeFile.c
# End Source File
# Begin Source File

SOURCE=.\TabResTbl.c
# End Source File
# Begin Source File

SOURCE=.\TabRlcTbl.c
# End Source File
# Begin Source File

SOURCE=.\TabUsrOpr.c
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\Define.h
# End Source File
# Begin Source File

SOURCE=.\Global.h
# End Source File
# Begin Source File

SOURCE=.\MainWnd.h
# End Source File
# Begin Source File

SOURCE=.\PeFile.h
# End Source File
# Begin Source File

SOURCE=.\PeToolC.h
# End Source File
# Begin Source File

SOURCE=.\Resource.h
# End Source File
# Begin Source File

SOURCE=.\TabBlkTbl.h
# End Source File
# Begin Source File

SOURCE=.\TabDatDir.h
# End Source File
# Begin Source File

SOURCE=.\TabDosHdr.h
# End Source File
# Begin Source File

SOURCE=.\TabExpTbl.h
# End Source File
# Begin Source File

SOURCE=.\TabFilHdr.h
# End Source File
# Begin Source File

SOURCE=.\TabImpTbl.h
# End Source File
# Begin Source File

SOURCE=.\TabOptHdr.h
# End Source File
# Begin Source File

SOURCE=.\TabPeFile.h
# End Source File
# Begin Source File

SOURCE=.\TabResTbl.h
# End Source File
# Begin Source File

SOURCE=.\TabRlcTbl.h
# End Source File
# Begin Source File

SOURCE=.\TabUsrOpr.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\res\PeToolC.ico
# End Source File
# Begin Source File

SOURCE=.\res\PeToolC.rc
# End Source File
# End Group
# End Target
# End Project
