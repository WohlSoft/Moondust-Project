;Refer to MUI. Or else it'll look like a installer for Win98.
!include MUI.nsh
!include WordFunc.nsh

;Name our program.
Name "Platformer Game Engine"

;Default install dir.
InstallDir ""

;UAC Prompt, Vista and later
RequestExecutionLevel admin

;Self-check for corruption
CRCCheck on

;Output filename
OutFile "pge-online-downloader.exe"

;Window title
Caption "PGE Online Installer (wohlsoft.ru mirror)"

;Set the text at the bottom, AKA branding text
BrandingText "tb1024's PGE Online Downloader v0.5.0. Platformer Game Engine is (C) Wohlstand."

;Branding images
!define MUI_WELCOMEFINISHPAGE_BITMAP "pge_editor_installer.bmp"
!define MUI_ICON "pge_install_icon.ico"

;Set install types:
InstType "Typical"
InstType "Minimal"
InstType "Complete with all config packs"

;Root where remember components
!define REG_ROOT      HKCU
;Branch where remember components
!define REG_KEY       "Software\Wohlhabend Team"

InstallDirRegKey ${REG_ROOT} "${REG_KEY}" "InstallLocation"

;Warn if user exits
!define MUI_ABORTWARNING

;The welcome page texts
!define MUI_WELCOMEPAGE_TITLE "Welcome to PGE Online Installer!"
!define MUI_WELCOMEPAGE_TEXT "It'll automatically download and extract latest builds for you.\r\n\r\nNo need to close any programs as no unistall information will be added.\r\n\r\nClick $\"Next$\" to continue."

;Installer pages
!define MUI_PAGE_CUSTOMFUNCTION_PRE welcome_pre
!insertmacro MUI_PAGE_WELCOME
!define MUI_PAGE_CUSTOMFUNCTION_PRE license_pre
!insertmacro MUI_PAGE_LICENSE "license.txt"
;!define MUI_PAGE_CUSTOMFUNCTION_PRE dir_pre
!insertmacro MUI_PAGE_DIRECTORY
!insertmacro MUI_PAGE_COMPONENTS
!insertmacro MUI_PAGE_INSTFILES
!insertmacro MUI_PAGE_FINISH
;Uninstaller pages
!insertmacro MUI_UNPAGE_CONFIRM
;!insertmacro MUI_UNPAGE_DIRECTORY
!insertmacro MUI_UNPAGE_INSTFILES
!insertmacro MUI_UNPAGE_FINISH
;Set the language
!insertmacro MUI_LANGUAGE "English"

Var IsUpdating
Var IsPortable

Function welcome_pre
   StrCmp $IsUpdating "0" noabort
      Abort
   noabort:
FunctionEnd

Function license_pre
   StrCmp $IsUpdating "0" noabort
      Abort
   noabort:
FunctionEnd

;Function dir_pre
;   StrCmp $IsUpdating "0" noabort
;      Abort
;   noabort:
;FunctionEnd

!macro SaveParameter Param Value
    StrCmp $IsPortable "0" writeReg writeIni
    writeReg:
        WriteRegStr ${REG_ROOT} "${REG_KEY}" "${Param}" "${Value}"
        goto endSaveParameter
    writeIni:
        WriteINIStr "$EXEDIR\install-settings.ini" "Setup" "${Param}" "${Value}"
        goto endSaveParameter
    endSaveParameter:
!macroend

Function GetParameterFunc
    Exch $0
    Exch
    Exch $1
    StrCmp $IsPortable "0" readReg readIni
    readReg:
        ReadRegStr $0 ${REG_ROOT} "${REG_KEY}" "$1"
        goto endGetParameter
    readIni:
        ReadINIStr $0 "$EXEDIR\install-settings.ini" "Setup" "$1"
        goto endGetParameter
    endGetParameter:
    Pop $1
    Exch $0
FunctionEnd

!macro GetParameter Output Param
    Push ${Param}
    Push ${Output}
    Call GetParameterFunc
    Pop "${Output}"
!macroend

;Some Variables
Var ToolsInstall
Var EngineInstall
;Var PortableInstall

; StrContains
; This function does a case sensitive searches for an occurrence of a substring in a string. 
; It returns the substring if it is found. 
; Otherwise it returns null(""). 
; Written by kenglish_hi
; Adapted from StrReplace written by dandaman32
  
Var STR_HAYSTACK
Var STR_NEEDLE
Var STR_CONTAINS_VAR_1
Var STR_CONTAINS_VAR_2
Var STR_CONTAINS_VAR_3
Var STR_CONTAINS_VAR_4
Var STR_RETURN_VAR
 
Function StrContains
  Exch $STR_NEEDLE
  Exch 1
  Exch $STR_HAYSTACK
  ; Uncomment to debug
  ;MessageBox MB_OK 'STR_NEEDLE = $STR_NEEDLE STR_HAYSTACK = $STR_HAYSTACK '
    StrCpy $STR_RETURN_VAR ""
    StrCpy $STR_CONTAINS_VAR_1 -1
    StrLen $STR_CONTAINS_VAR_2 $STR_NEEDLE
    StrLen $STR_CONTAINS_VAR_4 $STR_HAYSTACK
    loop:
      IntOp $STR_CONTAINS_VAR_1 $STR_CONTAINS_VAR_1 + 1
      StrCpy $STR_CONTAINS_VAR_3 $STR_HAYSTACK $STR_CONTAINS_VAR_2 $STR_CONTAINS_VAR_1
      StrCmp $STR_CONTAINS_VAR_3 $STR_NEEDLE found
      StrCmp $STR_CONTAINS_VAR_1 $STR_CONTAINS_VAR_4 done
      Goto loop
    found:
      StrCpy $STR_RETURN_VAR $STR_NEEDLE
      Goto done
    done:
   Pop $STR_NEEDLE ;Prevent "invalid opcode" errors and keep the
   Exch $STR_RETURN_VAR
FunctionEnd
 
!macro _StrContainsConstructor OUT NEEDLE HAYSTACK
  Push `${HAYSTACK}`
  Push `${NEEDLE}`
  Call StrContains
  Pop `${OUT}`
!macroend
 
!define StrContains '!insertmacro "_StrContainsConstructor"'

!insertmacro WordFind
!insertmacro WordFind2X

Var NEW
Var OLD

Section "Portable instalation" SecPortable
    ;SectionIn 0
    StrCpy $IsPortable "1"
    WriteINIStr "$EXEDIR\install-settings.ini" "Setup" "is-portable" "1"
    ;Register component //Don't register "required" components
    StrCpy $NEW "$NEW [SecPortable.${SecPortable}]"
SectionEnd
LangString DESC_Portable ${LANG_ENGLISH} "Install PGE in portable mode (everything will be in the same directory)"

SubSection "Main components" SubSecComp
	Section "Runtime libraries" SecLibs
		SectionIn RO 1 2 3
		;Set output folder
		SetOutPath $INSTDIR
		;Download the file
		NSISdl::download "http://wohlsoft.ru/PGE/_downloads/win32/online-install/install-pge-common-dev-win32.zip" "common_libs.zip"
        Pop $R0 ;Get the return value
          StrCmp $R0 "success" +3
            MessageBox MB_OK|MB_ICONSTOP "Download failed: $R0"
            Quit

		;Extract it.
		DetailPrint "Extracting..."
		ZipDLL::extractall "$OUTDIR\common_libs.zip" "$OUTDIR"
		DetailPrint "Extracting finished."
		;Delete the zip.
		Delete "$OUTDIR\common_libs.zip"
		;Store installation path into registry
		!insertmacro SaveParameter "InstallLocation" "$INSTDIR"
		
        StrCmp $IsPortable "0" regDirs portableDirs
        portableDirs:
            CreateDirectory "$INSTDIR\worlds"
            CreateDirectory "$INSTDIR\configs"
            CreateDirectory "$INSTDIR\logs"
            goto dirsDone
        regDirs:
            CreateDirectory "$PROFILE\.PGE_Project"
            CreateDirectory "$PROFILE\.PGE_Project\worlds"
            CreateDirectory "$PROFILE\.PGE_Project\configs"
            CreateDirectory "$PROFILE\.PGE_Project\logs"
        dirsDone:

        ;Register component //Don't register "required" components
        StrCpy $NEW "[SecLibs.${SecLibs}]"
	SectionEnd
	LangString DESC_Libs ${LANG_ENGLISH} "Common runtime libraries, required to run PGE applications."
	
	Section "Editor" SecEditor
		SectionIn RO 1 2 3
		;Set output folder
		SetOutPath $INSTDIR
		;Download the file
		NSISdl::download "http://wohlsoft.ru/PGE/_downloads/win32/online-install/install-pge-editor-dev-win32.zip" "editor.zip"
        Pop $R0 ;Get the return value
          StrCmp $R0 "success" +3
            MessageBox MB_OK|MB_ICONSTOP "Download failed: $R0"
            Quit

		;Extract it.
		DetailPrint "Extracting..."
		ZipDLL::extractall "$OUTDIR\editor.zip" "$OUTDIR"
		DetailPrint "Extracting finished."
        StrCmp $IsPortable "0" regMode portableMode
        portableMode:
            WriteINIStr "$INSTDIR\pge_editor.ini" "Main" "force-portable" "true"
            goto dirsDone
        regMode:
            ;Run the program
            DetailPrint "Running the configurator..."
            ExecWait '"$OUTDIR\pge_editor.exe" --install' $0
            DetailPrint "Configurator returned $0"
            WriteINIStr "$INSTDIR\pge_editor.ini" "Main" "force-portable" "false"
        dirsDone:
		;Delete the zip.
		Delete "$OUTDIR\editor.zip"
		;Store installation path into registry
		!insertmacro SaveParameter "InstallLocation" "$INSTDIR"
        ;Register component //Don't register "required" components
        StrCpy $NEW "$NEW [SecEditor.${SecEditor}]"
	SectionEnd
	LangString DESC_Editor ${LANG_ENGLISH} "Download the editor. Required."
	
	
	
	Section "Engine [Experimental]" SecEngine
		;Include in the typical installation.
		SectionIn 1 3
		;Set output folder
		SetOutPath $INSTDIR
		;Download the file
		NSISdl::download "http://wohlsoft.ru/PGE/_downloads/win32/online-install/install-pge-engine-dev-win32.zip" "engine.zip"
        Pop $R0 ;Get the return value
          StrCmp $R0 "success" +3
            MessageBox MB_OK|MB_ICONSTOP "Download failed: $R0"
            Quit

		;Extract it
		DetailPrint "Extracting..."
		ZipDLL::extractall "$OUTDIR\engine.zip" "$OUTDIR"
		DetailPrint "Extracting finished."
        StrCmp $IsPortable "0" regMode portableMode
        portableMode:
            WriteINIStr "$INSTDIR\pge_engine.ini" "Main" "force-portable" "true"
            goto dirsDone
        regMode:
            ;Run the program
            DetailPrint "Running the configurator..."
            ExecWait '"$OUTDIR\pge_engine.exe" --install' $0
            DetailPrint "Configurator returned $0"
            WriteINIStr "$INSTDIR\pge_engine.ini" "Main" "force-portable" "false"
        dirsDone:
		;Delete the zip.
		Delete "$OUTDIR\engine.zip"
		;Engine is installed
		StrCpy $EngineInstall 1
        ;Register component
        StrCpy $NEW "$NEW [SecEngine.${SecEngine}]"
	SectionEnd 
	LangString DESC_Engine ${LANG_ENGLISH} "Download the engine pre-alpha builds. Warning: Highly unstable and unfinished."

	
	Section "Help files" SecHelp
		;Include in the typical installation.
		SectionIn 1 3
		;Set Output folder
		SetOutPath "$INSTDIR"
		;Delete the previous.
		Delete "$INSTDIR\manual_editor.html"
		;Download the file
		NSISdl::download "http://wohlsoft.ru/PGE/_downloads/pge_help_standalone.zip" "help.zip"
        Pop $R0 ;Get the return value
          StrCmp $R0 "success" +3
            MessageBox MB_OK|MB_ICONSTOP "Download failed: $R0"
            Quit

		;Extract it
		DetailPrint "Extracting..."
		ZipDLL::extractall "$OUTDIR\help.zip" "$OUTDIR"
		DetailPrint "Extracting finished."
		;Delete the zip
		Delete "$OUTDIR\help.zip"
        ;Register component
        StrCpy $NEW "$NEW [SecHelp.${SecHelp}]"
	SectionEnd
	LangString DESC_Help ${LANG_ENGLISH} "Download offline documentation to read when you're not online."
	
	
	Section "Additonal console tools" SecTools
		;Include in the Typical installation
		SectionIn 1 3
		;Set output folder
		SetOutPath $INSTDIR
		;Download the file
		NSISdl::download "http://wohlsoft.ru/PGE/_downloads/win32/online-install/install-pge-tools-dev-win32.zip" "tools.zip"
        Pop $R0 ;Get the return value
          StrCmp $R0 "success" +3
            MessageBox MB_OK|MB_ICONSTOP "Download failed: $R0"
            Quit

		;Extract it
		DetailPrint "Extracting..."
		ZipDLL::extractall "$OUTDIR\tools.zip" "$OUTDIR"
		DetailPrint "Extracting finished."
        StrCmp $IsPortable "0" regMode portableMode
        portableMode:
            WriteINIStr "$INSTDIR\pge_calibrator.ini" "Main" "force-portable" "true"
            goto dirsDone
        regMode:
            ;Run the program
            DetailPrint "Running the configurator..."
            ExecWait '"$OUTDIR\pge_calibrator.exe" --install' $0
            DetailPrint "Configurator returned $0"
            WriteINIStr "$INSTDIR\pge_calibrator.ini" "Main" "force-portable" "false"
        dirsDone:
		;Delete the zip
		Delete "$OUTDIR\tools.zip"
		;Tools are installed
		StrCpy $ToolsInstall 1
        ;Register component
        StrCpy $NEW "$NEW [SecTools.${SecTools}]"
    SectionEnd	
	LangString DESC_Tools ${LANG_ENGLISH} "Download additional tools to enhance PGE's functionality."

SubSectionEnd
LangString DESC_Compo ${LANG_ENGLISH} "Main PGE components."


SubSection "Config Packs (Must select one)" SubSecCfg
	Section "SMBX Integrator" SecInt
		;Include in Minimal Install
		SectionIn 2
		;Set output folder
		SetOutPath "$INSTDIR\configs\"
		;Download file
		NSISdl::download "http://wohlsoft.ru/docs/_configs/SMBXInt/SMBX_Integration.zip" "SMBXIntegrator.zip"
        Pop $R0 ;Get the return value
          StrCmp $R0 "success" +3
            MessageBox MB_OK|MB_ICONSTOP "Download failed: $R0"
            Quit

		;Extract it
		DetailPrint "Extracting..."
		ZipDLL::extractall "$OUTDIR\SMBXIntegrator.zip" "$OUTDIR"
		DetailPrint "Extracting finished."
		;Run the program
		DetailPrint "Running the configurator. Please input requested data."
		ExecWait "$OUTDIR\SMBXInt\configure.exe" $0
		DetailPrint "Configurator returned $0"
		;Delete the archive
		Delete "$OUTDIR\SMBXIntegrator.zip"
        ;Register component
        StrCpy $NEW "$NEW [SecInt.${SecInt}]"
    SectionEnd
	LangString DESC_Int ${LANG_ENGLISH} "Install the SMBX Integration package. Allows you to use the SMBX folder as configuration."
	
	
	Section "SMBX 1.3 Compatible" SecMBX
		;Include in typical install
		SectionIn 1 3
		;Set Output Path
		SetOutPath "$INSTDIR\configs\"
		;Download file
		NSISdl::download "http://wohlsoft.ru/docs/_configs/SMBX13/SMBX_13_compatible.zip" "smbxcfg.zip"
        Pop $R0 ;Get the return value
          StrCmp $R0 "success" +3
            MessageBox MB_OK|MB_ICONSTOP "Download failed: $R0"
            Quit

		;ExtractIt()
		DetailPrint "Extracting..."
		ZipDLL::extractall "$OUTDIR\smbxcfg.zip" "$OUTDIR"
		DetailPrint "Extracting finished."
		;Delete
		Delete "$OUTDIR\smbxcfg.zip"
        ;Register component
        StrCpy $NEW "$NEW [SecMBX.${SecMBX}]"
	SectionEnd
	LangString Desc_SMBX ${LANG_ENGLISH} "Install the SMBX 1.3 Compatible config for SMBX-like feel."

	
	Section "Raocow TalkHaus/ASMBXT Config Pack" SecRaocow
		SectionIn 3
		;Set output folder
		SetOutPath "$INSTDIR\configs"
		;Download the file
		NSISdl::download "http://wohlsoft.ru/docs/_configs/A2MBXT/Raocow_talkhaus_full.zip" "raocow.zip"
        Pop $R0 ;Get the return value
          StrCmp $R0 "success" +3
            MessageBox MB_OK|MB_ICONSTOP "Download failed: $R0"
            Quit

		;Extract it.
		DetailPrint "Extracting..."
		ZipDLL::extractall "$OUTDIR\raocow.zip" "$OUTDIR"
		DetailPrint "Extracting finished."
		;Delete it
		Delete "$OUTDIR\raocow.zip"
        ;Register component
        StrCpy $NEW "$NEW [SecRaocow.${SecRaocow}]"
	SectionEnd
	LangString DESC_Raocow ${LANG_ENGLISH} "Install the Raocow TalkHaus config pack for ASMBXT-like experience."
	
	
	
	Section "SMBX Redrawn Config Pack" SecRedrawn
		SectionIn 3
		;Set output folder
		SetOutPath "$INSTDIR\configs"
		;Download the file
		NSISdl::download "http://wohlsoft.ru/docs/_configs/SMBX_Redrawn/SMBX_Redrawn_full.zip" "redrawn.zip"
        Pop $R0 ;Get the return value
          StrCmp $R0 "success" +3
            MessageBox MB_OK|MB_ICONSTOP "Download failed: $R0"
            Quit

		;Extract it.
		DetailPrint "Extracting..."
		ZipDLL::extractall "$OUTDIR\redrawn.zip" "$OUTDIR"
		DetailPrint "Extracting finished."
		;Delete it
		Delete "$OUTDIR\redrawn.zip"
        ;Register component
        StrCpy $NEW "$NEW [SecRedrawn.${SecRedrawn}]"
	SectionEnd
	LangString DESC_Redrawn ${LANG_ENGLISH} "Install the SMBX Redrawn config pack by SilverDeoxys563."

SubSectionEnd
LangString DESC_Cfg ${LANG_ENGLISH} "Configuration packages for PGE."



;Associate files
Section "Associate file extensions" SecFileAssc
	SectionIn 1 3
	;Write Registry Keys at HKCU. Thanks to Wohlstand.
	;lvlx
	WriteRegStr HKCU "Software\Classes\.lvlx" '' "PGEWohlstand.Level"
	WriteRegStr HKCU "Software\Classes\PGEWohlstand.Level" '' "PGE Level file"
	WriteRegStr HKCU "Software\Classes\PGEWohlstand.Level\DefaultIcon" '' "$\"$INSTDIR\pge_editor.exe$\",1"
	WriteRegStr HKCU "Software\Classes\PGEWohlstand.Level\Shell\Open\Command" '' "$\"$INSTDIR\pge_editor.exe$\" $\"%1$\""
	;wldx
	WriteRegStr HKCU "Software\Classes\.wldx" '' "PGEWohlstand.World"
	WriteRegStr HKCU "Software\Classes\PGEWohlstand.World" '' "PGE World Map"
	WriteRegStr HKCU "Software\Classes\PGEWohlstand.World\DefaultIcon" '' "$\"$INSTDIR\pge_editor.exe$\",2"
	WriteRegStr HKCU "Software\Classes\PGEWohlstand.World\Shell\Open\Command" '' "$\"$INSTDIR\pge_editor.exe$\" $\"%1$\""
	;lvl
	WriteRegStr HKCU "Software\Classes\.lvl" '' "SMBX64.Level"
	WriteRegStr HKCU "Software\Classes\SMBX64.Level" '' "SMBX Level File"
	WriteRegStr HKCU "Software\Classes\SMBX64.Level\DefaultIcon" '' "$\"$INSTDIR\pge_editor.exe$\",3"
	WriteRegStr HKCU "Software\Classes\SMBX64.Level\Shell\Open\Command" '' "$\"$INSTDIR\pge_editor.exe$\" $\"%1$\""
	;wld
	WriteRegStr HKCU "Software\Classes\.wld" '' "SMBX64.World"
	WriteRegStr HKCU "Software\Classes\SMBX64.World" '' "SMBX World File"
	WriteRegStr HKCU "Software\Classes\SMBX64.World\DefaultIcon" '' "$\"$INSTDIR\pge_editor.exe$\",4"
	WriteRegStr HKCU "Software\Classes\SMBX64.World\Shell\Open\Command" '' "$\"$INSTDIR\pge_editor.exe$\" $\"%1$\""
    ;Register component
    StrCpy $NEW "$NEW [SecFileAssc.${SecFileAssc}]"
SectionEnd
LangString DESC_FileAssc ${LANG_ENGLISH} "Associate LVLX, WLDX, LVL and WLD extension to PGE. Code contributed by Wohlstand."

Section "Add a desktop shortcut" SecDesk
    ;Tell the user
	DetailPrint "Adding Desktop Shortcut..."
	;Create the shortcut
	CreateShortCut "$DESKTOP\PGE Editor.lnk" "$INSTDIR\pge_editor.exe"
	StrCmp $EngineInstall 1 +1 tools
	CreateShortCut "$DESKTOP\Play PGE Games.lnk" "$INSTDIR\pge_engine.exe"
	tools:
    ;Register component
    StrCpy $NEW "$NEW [${SecDesk}.SecDesk]"
SectionEnd
LangString DESC_Desk ${LANG_ENGLISH} "Add a desktop shortcut."

Section "Add a start menu shortcut" SecSM
	SectionIn 1 2 3
	;Tell the user
	DetailPrint "Creating Start Menu folder and shortcuts..."
	;Create Folder
	CreateDirectory "$SMPROGRAMS\PGE Project"
	;Add editor shortcuts
	CreateShortCut "$SMPROGRAMS\PGE Project\PGE Editor.lnk" "$INSTDIR\pge_editor.exe"
	CreateShortCut "$SMPROGRAMS\PGE Project\Data directory.lnk" "$PROFILE\.PGE_Project"
	CreateShortCut "$SMPROGRAMS\PGE Project\Worlds directory.lnk" "$PROFILE\.PGE_Project\worlds"
	;If engine installed, add it
	StrCmp $EngineInstall 1 +1 tools
	CreateShortCut "$SMPROGRAMS\PGE Project\PGE Engine.lnk" "$INSTDIR\pge_engine.exe"
	tools:
	;Tools? You have it?
	StrCmp $ToolsInstall 1 +1 unin
	CreateDirectory "$SMPROGRAMS\PGE Project\Tools"
	CreateShortCut "$SMPROGRAMS\PGE Project\Tools\PNG2GIFs.lnk" "$INSTDIR\PNG2GIFs.exe"
	CreateShortCut "$SMPROGRAMS\PGE Project\Tools\GIFs2PNG.lnk" "$INSTDIR\GIFs2PNG.exe"
	CreateShortCut "$SMPROGRAMS\PGE Project\Tools\Playable Calibrator.lnk" "$INSTDIR\pge_calibrator.exe"
	CreateShortCut "$SMPROGRAMS\PGE Project\Tools\Lazy Fix Tool.lnk" "$INSTDIR\pge_editor.exe"
	unin:
	CreateShortCut "$SMPROGRAMS\PGE Project\Uninstall PGE Project.lnk" "$INSTDIR\pge-uninstall.exe"
    ;Register component
    StrCpy $NEW "$NEW [SecSM.${SecSM}]"
SectionEnd
LangString DESC_SM ${LANG_ENGLISH} "Add a shortcut to the start menu."

Section "Add PGE path into PATH environment" SecPathEnv
    ;Tell the user
    DetailPrint "Add PGE path into PATH environment..."
    ;Add PATH variables
    ReadRegStr $0 HKLM "SYSTEM\CurrentControlSet\Control\Session Manager\Environment" 'Path'
    ${StrContains} $2 $INSTDIR $0
    StrCmp $2 "" needtoaddpath
      goto pathalreadyadded
    needtoaddpath:
      StrCpy $1 "$0;$INSTDIR"
    WriteRegStr HKLM "SYSTEM\CurrentControlSet\Control\Session Manager\Environment" 'Path' $1
    pathalreadyadded:
    ;Register component
    StrCpy $NEW "$NEW [SecPathEnv.${SecPathEnv}]"
SectionEnd
LangString DESC_PathEnv ${LANG_ENGLISH} "Add PGE path into PATH environment for easier access to GIFs2PNG, PNG2GIFs, LazyFixTool from terminal. WARNING: This option is not recomended on Windows 10!"

Section "Uninstall"
    DetailPrint "Removing shortcuts..."
    RMDir /r "$SMPROGRAMS\PGE Project"
    Delete "$DESKTOP\PGE Editor.lnk"
    Delete "$DESKTOP\Play PGE Games.lnk"
    DetailPrint "Removing PGE directory and files inside..."
    RMDir /r "$INSTDIR"
    #Remove installer's key
    DeleteRegKey ${REG_ROOT} "${REG_KEY}"
    DeleteRegKey ${REG_ROOT} "Software\Wohlhabend Networks"
    #Remove file associations
    DeleteRegKey HKCU "Software\Classes\.lvlx"
    DeleteRegKey HKCU "Software\Classes\PGEWohlstand.Level"
    DeleteRegKey HKCU "Software\Classes\.wldx"
    DeleteRegKey HKCU "Software\Classes\PGEWohlstand.World"
    DeleteRegKey HKCU "Software\Classes\.lvl"
    DeleteRegKey HKCU "Software\Classes\SMBX64.Level"
    DeleteRegKey HKCU "Software\Classes\.wld"
    DeleteRegKey HKCU "Software\Classes\SMBX64.World"
SectionEnd

;Installed components
Var FLAG_SecPortable
Var FLAG_SecEngine
Var FLAG_SecHelp
Var FLAG_SecTools
Var FLAG_SecInt
Var FLAG_SecMBX
Var FLAG_SecRaocow
Var FLAG_SecRedrawn
Var FLAG_SecFileAssc
Var FLAG_SecDesk
Var FLAG_SecSM
Var FLAG_SecPathEnv

Function .onInit
    ;Detect portable config
    StrCpy $IsPortable "0"
        ReadINIStr $0 "$EXEDIR\install-settings.ini" "Setup" "is-portable"
        StrCmp $0 "" skipPortableInit doPortableInit
    doPortableInit:
        StrCpy $IsPortable "1"
    skipPortableInit:
    
    StrCpy $INSTDIR "$PROGRAMFILES\PGE"
    
    ;Detect already-installed preferences
    !insertmacro GetParameter $OLD "InstalledComponents"
	;ReadRegStr $OLD ${REG_ROOT} "${REG_KEY}" "InstalledComponents"
	ClearErrors
	StrCmp $OLD '' NewInstall Update

	NewInstall:
        StrCpy $IsUpdating "0"
        goto end

	Update:
        StrCpy $IsUpdating "1"
        StrCpy $2 1

        !insertmacro GetParameter $INSTDIR "InstallLocation"

        ;set all flags to 0
        StrCpy $FLAG_SecPortable 0
        StrCpy $FLAG_SecEngine 0
        StrCpy $FLAG_SecHelp 0
        StrCpy $FLAG_SecTools 0
        StrCpy $FLAG_SecInt 0
        StrCpy $FLAG_SecMBX 0
        StrCpy $FLAG_SecRaocow 0
        StrCpy $FLAG_SecRedrawn 0
        StrCpy $FLAG_SecFileAssc 0
        StrCpy $FLAG_SecDesk 0
        StrCpy $FLAG_SecSM 0
        StrCpy $FLAG_SecPathEnv 0

    ;Set pre-installed sections
	loop:
        ${WordFind2X} "$OLD" "[" "." "E+$2" $R0
        IfErrors doSetFlags
        ;nsislog::log "Install.log" "$R0 is installed"
        StrCmp $R0 'SecPortable'     checkSecPortable
        StrCmp $R0 'SecEngine'       checkSecEngine
        StrCmp $R0 'SecHelp'         checkSecHelp
        StrCmp $R0 'SecTools'        checkSecTools
        StrCmp $R0 'SecInt'          checkSecInt
        StrCmp $R0 'SecMBX'          checkSecMBX
        StrCmp $R0 'SecRaocow'       checkSecRaocow
        StrCmp $R0 'SecRedrawn'      checkSecRedrawn
        StrCmp $R0 'SecFileAssc'     checkSecFileAssc
        StrCmp $R0 'SecDesk'         checkSecDesk
        StrCmp $R0 'SecSM'           checkSecSM
        StrCmp $R0 'SecPathEnv'      checkSecPathEnv
        goto doIterate
    checkSecPortable:
        StrCpy $FLAG_SecPortable 1
        goto doIterate
    checkSecEngine:
        StrCpy $FLAG_SecEngine 1
        goto doIterate
    checkSecHelp:
        StrCpy $FLAG_SecHelp 1
        goto doIterate
    checkSecTools:
        StrCpy $FLAG_SecTools 1
        goto doIterate
    checkSecInt:
        StrCpy $FLAG_SecInt 1
        goto doIterate
    checkSecMBX:
        StrCpy $FLAG_SecMBX 1
        goto doIterate
    checkSecRaocow:
        StrCpy $FLAG_SecRaocow 1
        goto doIterate
    checkSecRedrawn:
        StrCpy $FLAG_SecRedrawn 1
        goto doIterate
    checkSecFileAssc:
        StrCpy $FLAG_SecFileAssc 1
        goto doIterate
    checkSecDesk:
        StrCpy $FLAG_SecDesk 1
        goto doIterate
    checkSecSM:
        StrCpy $FLAG_SecSM 1
        goto doIterate
    checkSecPathEnv:
        StrCpy $FLAG_SecPathEnv 1
        goto doIterate
    doIterate:
        IntOp $2 $2 + 1
        goto loop    
	
    doSetFlags:
    SectionSetFlags ${SecPortable}      $FLAG_SecPortable
    SectionSetFlags ${SecEngine}        $FLAG_SecEngine
    SectionSetFlags ${SecHelp}          $FLAG_SecHelp
    SectionSetFlags ${SecTools}         $FLAG_SecTools
    SectionSetFlags ${SecInt}           $FLAG_SecInt
    SectionSetFlags ${SecMBX}           $FLAG_SecMBX
    SectionSetFlags ${SecRaocow}        $FLAG_SecRaocow
    SectionSetFlags ${SecRedrawn}       $FLAG_SecRedrawn
    SectionSetFlags ${SecFileAssc}      $FLAG_SecFileAssc
    SectionSetFlags ${SecDesk}          $FLAG_SecDesk
    SectionSetFlags ${SecSM}            $FLAG_SecSM
    SectionSetFlags ${SecPathEnv}       $FLAG_SecPathEnv
    end:
FunctionEnd

Function .onInstSuccess
    ;Write uninstaller
    WriteUninstaller "pge-uninstall.exe"
    ;Remember installed components
    !insertmacro SaveParameter "InstalledComponents" "$NEW"
FunctionEnd

!insertmacro MUI_FUNCTION_DESCRIPTION_BEGIN
	!insertmacro MUI_DESCRIPTION_TEXT ${SecPortable} $(DESC_Portable)
	!insertmacro MUI_DESCRIPTION_TEXT ${SubSecComp} $(DESC_Compo)
	!insertmacro MUI_DESCRIPTION_TEXT ${SecLibs} $(DESC_Libs)
	!insertmacro MUI_DESCRIPTION_TEXT ${SecEditor} $(DESC_Editor)
	!insertmacro MUI_DESCRIPTION_TEXT ${SecEngine} $(DESC_Engine)
	!insertmacro MUI_DESCRIPTION_TEXT ${SecHelp} $(DESC_Help)
	!insertmacro MUI_DESCRIPTION_TEXT ${SecTools} $(DESC_Tools)
	!insertmacro MUI_DESCRIPTION_TEXT ${SubSecCfg} $(DESC_Cfg)
	!insertmacro MUI_DESCRIPTION_TEXT ${SecInt} $(DESC_Int)
	!insertmacro MUI_DESCRIPTION_TEXT ${SecMBX} $(DESC_SMBX)
	!insertmacro MUI_DESCRIPTION_TEXT ${SecRaocow} $(DESC_Raocow)
	!insertmacro MUI_DESCRIPTION_TEXT ${SecRedrawn} $(DESC_Redrawn)
	!insertmacro MUI_DESCRIPTION_TEXT ${SecFileAssc} $(DESC_FileAssc)
	!insertmacro MUI_DESCRIPTION_TEXT ${SecDesk} $(DESC_Desk)
	!insertmacro MUI_DESCRIPTION_TEXT ${SecSM} $(DESC_SM)
	!insertmacro MUI_DESCRIPTION_TEXT ${SecPathEnv} $(DESC_PathEnv)
!insertmacro MUI_FUNCTION_DESCRIPTION_END
