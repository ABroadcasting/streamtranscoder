Function .onInit
  SetOutPath $TEMP
  File /oname=spltmp.bmp "oddsock_logo.bmp"
  File /oname=spltmp.exe "D:\program files\nsis\splash.exe"
  ExecWait '"$TEMP\spltmp.exe" 4000 $HWNDPARENT $TEMP\spltmp'
  Delete $TEMP\spltmp.exe
  Delete $TEMP\spltmp.bmp
FunctionEnd
; sripperwa3.nsi
;
; This script will generate an installer that installs a Winamp plug-in.
; It also puts a license page on, for shits and giggles.
;
; This installer will automatically alert the user that installation was
; successful, and ask them whether or not they would like to make the 
; plug-in the default and run Winamp.
;

; The name of the installer
Name "Oddcast DSP For Winamp3"

; The file to write
OutFile "oddcast_dsp_v2.0.3.exe"

!verbose 3
!include "${NSISDIR}\Examples\WinMessages.nsh"
!include "${NSISDIR}\Examples\Modern UI\ModernUI.nsh"
!verbose 4

!insertmacro MUI_INTERFACE "modern2.exe" "adni18-installer-C-no48xp.ico" "adni18-uninstall-C-no48xp.ico" "modern.bmp" "smooth"

LicenseText "Oddcast DSP is released under the GNU Public License"
LicenseData ..\COPYING

; The default installation directory
InstallDir $PROGRAMFILES\Winamp3
; detect winamp path from uninstall string if available
InstallDirRegKey HKLM \
                 "Software\Microsoft\Windows\CurrentVersion\Uninstall\Winamp3" \
                 "UninstallString"

; The text to prompt the user to enter a directory
DirText "Please select your Winamp path below (you will be able to proceed when Winamp is detected):"
; DirShow hide

; automatically close the installer when done.
AutoCloseWindow true
; hide the "show details" box
; ShowInstDetails nevershow

; this is probably buggy
Function QueryWinampWacPath ; sets $1 with vis path
  StrCpy $1 $INSTDIR\Wacs
  StrCpy $2 $INSTDIR
FunctionEnd

; The stuff to install
Section "ThisNameIsIgnoredSoWhyBother?"

  Call QueryWinampWacPath
  SetOutPath $1

  ; File to extract
  File "Release\oddcast_dsp.wac"
  SetOutPath $2
  File "..\external\lib\bass.dll"
  File "..\external\lib\basswma.dll"
  File "..\external\lib\ogg.dll"
  File "..\external\lib\vorbis.dll"
  File "..\external\lib\vorbisenc.dll"
  File "..\bin\oddcast_editConfig.exe"

  ; prompt user, and if they select no, skip the following 3 instructions.
  MessageBox MB_YESNO|MB_ICONQUESTION \
             "The plug-in was installed. Would you like to run Winamp3 now? (you will find it in the thinger bar)" \
             IDNO NoWinamp
    Exec '"$INSTDIR\Studio.exe"'
  NoWinamp:
SectionEnd

; eof
