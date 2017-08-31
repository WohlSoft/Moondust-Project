#define MyAppName "PGE Project"
#define MyAppVersion "0.4.1"
#define MyAppPublisher "PGE Project Team"
#define MyAppURL "http://wohlsoft.ru/PGE/"

#define INSTALLER_VERSION_CHECK "http://wohlsoft.ru/docs/_laboratory/installers/installer_version.txt"
#define INSTALLER_VERSION "0.1"

#define NEW_INSTALLER_URL "http://wohlsoft.ru/docs/_laboratory/installers/pge-online-installer-innosetup.exe"

; Root where remember components
#define REG_ROOT      HKCU
; Branch where remember components
#define REG_KEY       "Software\Wohlhabend Team"

; Stable channel
#define FILE_URL_COMMON64 "http://wohlsoft.ru/PGE/_downloads/win64/online-install/install-pge-common-dev-win64.zip"
#define FILE_URL_COMMON32 "http://wohlsoft.ru/PGE/_downloads/win32/online-install/install-pge-common-dev-win32.zip"

#define FILE_URL_EDITOR64 "http://wohlsoft.ru/PGE/_downloads/win64/online-install/install-pge-editor-dev-win64.zip"
#define FILE_URL_EDITOR32 "http://wohlsoft.ru/PGE/_downloads/win32/online-install/install-pge-editor-dev-win32.zip"

#define FILE_URL_ENGINE64 "http://wohlsoft.ru/PGE/_downloads/win64/online-install/install-pge-engine-dev-win64.zip"
#define FILE_URL_ENGINE32 "http://wohlsoft.ru/PGE/_downloads/win32/online-install/install-pge-engine-dev-win32.zip"

#define FILE_URL_HELP     "http://wohlsoft.ru/PGE/_downloads/pge_help_standalone.zip"

#define FILE_URL_TOOLS64  "http://wohlsoft.ru/PGE/_downloads/win64/online-install/install-pge-tools-dev-win64.zip"
#define FILE_URL_TOOLS32  "http://wohlsoft.ru/PGE/_downloads/win32/online-install/install-pge-tools-dev-win32.zip"

#define FILE_URL_CONFIG_SMBXINT       "http://wohlsoft.ru/docs/_configs/SMBXInt/SMBX_Integration.zip"
#define FILE_URL_CONFIG_SMBX13        "http://wohlsoft.ru/docs/_configs/SMBX13/SMBX_13_compatible.zip"
#define FILE_URL_CONFIG_A2XT          "http://wohlsoft.ru/docs/_configs/A2XT/A2XT.zip"
#define FILE_URL_CONFIG_ASMBXT        "http://wohlsoft.ru/docs/_configs/A2MBXT/Raocow_talkhaus_full.zip"
#define FILE_URL_CONFIG_SMBXREDRAW    "http://wohlsoft.ru/docs/_configs/SMBX_Redrawn/SMBX_Redrawn_full.zip"
#define FILE_URL_CONFIG_SMBXPANDED    "http://wohlsoft.ru/docs/_configs/SMBXpandedBySednaiur/SMBXpanded_full.zip"
#define FILE_URL_CONFIG_SMBX38A       "http://wohlsoft.ru/docs/_configs/SMBXby38A/SMBXby38a_full.zip"

; Laboratory channel
#define LAB_FILE_URL_COMMON64 "http://wohlsoft.ru/docs/_laboratory/_Builds/win32/bin-w64/_packed/install-pge-common-dev-win64.zip"
#define LAB_FILE_URL_COMMON32 "http://wohlsoft.ru/docs/_laboratory/_Builds/win32/bin-w32/_packed/install-pge-common-dev-win32.zip"

#define LAB_FILE_URL_EDITOR64 "http://wohlsoft.ru/docs/_laboratory/_Builds/win32/bin-w64/_packed/install-pge-editor-dev-win64.zip"
#define LAB_FILE_URL_EDITOR32 "http://wohlsoft.ru/docs/_laboratory/_Builds/win32/bin-w32/_packed/install-pge-editor-dev-win32.zip"

#define LAB_FILE_URL_ENGINE64 "http://wohlsoft.ru/docs/_laboratory/_Builds/win32/bin-w64/_packed/install-pge-engine-dev-win64.zip"
#define LAB_FILE_URL_ENGINE32 "http://wohlsoft.ru/docs/_laboratory/_Builds/win32/bin-w32/_packed/install-pge-engine-dev-win32.zip"

#define LAB_FILE_URL_TOOLS64  "http://wohlsoft.ru/docs/_laboratory/_Builds/win32/bin-w64/_packed/install-pge-tools-dev-win64.zip"
#define LAB_FILE_URL_TOOLS32  "http://wohlsoft.ru/docs/_laboratory/_Builds/win32/bin-w32/_packed/install-pge-tools-dev-win32.zip"

#define LAB_FILE_URL_HELP     "http://wohlsoft.ru/docs/_laboratory/pge_help_standalone.zip"

#define LAB_FILE_URL_CONFIG_SMBXINT     "http://wohlsoft.ru/docs/_laboratory/config_packs/SMBX_Integration.zip"
#define LAB_FILE_URL_CONFIG_SMBX13      "http://wohlsoft.ru/docs/_laboratory/config_packs/SMBX_13_compatible.zip"
#define LAB_FILE_URL_CONFIG_A2XT        "http://wohlsoft.ru/docs/_laboratory/config_packs/A2XT.zip"
#define LAB_FILE_URL_CONFIG_ASMBXT      "http://wohlsoft.ru/docs/_laboratory/config_packs/Raocow_talkhaus_full.zip"
#define LAB_FILE_URL_CONFIG_SMBXREDRAW  "http://wohlsoft.ru/docs/_laboratory/config_packs/SMBX_Redrawn_full.zip"
#define LAB_FILE_URL_CONFIG_SMBXPANDED  "http://wohlsoft.ru/docs/_laboratory/config_packs/SMBXpanded_full.zip"
#define LAB_FILE_URL_CONFIG_SMBX38A     "http://wohlsoft.ru/docs/_laboratory/config_packs/SMBXpanded_full.zip"


[Setup]
; NOTE: The value of AppId uniquely identifies this application.
; Do not use the same AppId value in installers for other applications.
; (To generate a new GUID, click Tools | Generate GUID inside the IDE.)
AppId={{5300CE4F-6C4E-4443-8629-F937CA83697C}
AppName={#MyAppName}
AppVersion={#MyAppVersion}
;AppVerName={#MyAppName} {#MyAppVersion}
AppPublisher={#MyAppPublisher}
AppPublisherURL={#MyAppURL}
AppSupportURL={#MyAppURL}
AppUpdatesURL={#MyAppURL}
DefaultDirName={pf}\{#MyAppName}
DefaultGroupName={#MyAppName}
LicenseFile=license.txt
OutputBaseFilename=pge-online-installer-innosetup
SetupIconFile=pge_install_icon.ico
WizardImageFile=pge_installer.bmp
WizardSmallImageFile=pge_image_small.bmp
Compression=lzma
SolidCompression=yes
DisableWelcomePage=no
ArchitecturesInstallIn64BitMode=x64
CloseApplications=yes
CloseApplicationsFilter="pge_editor.exe,pge_engine.exe,pge_calibrator.exe,pge_musplay.exe,pge_maintainer.exe"

[Languages]
Name: "en"; MessagesFile: "compiler:Default.isl"
Name: "de"; MessagesFile: "compiler:Languages\German.isl"
Name: "pl"; MessagesFile: "compiler:Languages\Polish.isl"
Name: "pt_br"; MessagesFile: "compiler:Languages\Portuguese.isl"
Name: "ru"; MessagesFile: "compiler:Languages\Russian.isl"
Name: "es"; MessagesFile: "compiler:Languages\Spanish.isl"

#include <idp.iss>
#include <idplang\German.iss>
#include <idplang\Polish.iss>
#include <idplang\BrazilianPortuguese.iss>
#include <idplang\Russian.iss>
#include <idplang\Spanish.iss>

[Types]
Name: typical; Description: "Typical installation"
Name: minimal; Description: "Minimal installation"
Name: full;    Description: "Complete with all config packs"
Name: custom;  Description: "Custom installation"; Flags: iscustom

[Components]
Name: devkit; Description: "Development tools";
Name: devkit\runtime; Description: "Runtime libraries"; Types: typical minimal full custom; Flags: fixed
Name: devkit\editor;  Description: "Editor";  Types: typical minimal full custom;
Name: devkit\tools;   Description: "Tools";   Types: typical full custom
Name: devkit\help;    Description: "Standalone help files"; Types: typical full custom
Name: devkit\assoc;   Description: "Associate file types"; Types: typical minimal full custom;
Name: engine;         Description: "Runtime Engine [Experimental]"; Types: typical full custom

Name: configs;            Description: "Config packs";
Name: configs\a2xt;       Description: "A2XT"; Types: typical full custom
Name: configs\smbxint;    Description: "SMBX Integration"; Types: full minimal custom
Name: configs\smbx13;     Description: "SMBX 1.3 Compatibility"; Types: full
Name: configs\smbxredraw; Description: "SMBX Redrawn"; Types: full
Name: configs\smbxpanded; Description: "SMB-Xpanded"; Types: full
Name: configs\smbx38a;    Description: "SMBX-38A"; Types: full

[Registry]
Root: HKCU; Subkey: "Software\PGE Team"; Flags: uninsdeletekey;
Root: HKCU; Subkey: "Software\PGE Team\PGE Content Manager"; ValueType: string; \
    Flags: uninsdeletekey; Components: devkit\tools;
Root: HKCU; Subkey: "Software\PGE Team\PGE Editor"; ValueType: string; \
    ValueName: "EnableUserDir"; ValueData: "true"; Flags: uninsdeletekey; Components: devkit\editor;
Root: HKCU; Subkey: "Software\PGE Team\PGE Maintainer"; ValueType: string; \
    ValueName: "EnableUserDir"; ValueData: "true"; Flags: uninsdeletekey; Components: devkit\tools;
Root: HKCU; Subkey: "Software\PGE Team\PGE Music Player"; ValueType: string; \
    Flags: uninsdeletekey; Components: devkit\tools;
Root: HKCU; Subkey: "Software\PGE Team\Playable Character Calibrator"; ValueType: string; \
    Flags: uninsdeletekey; Components: devkit\tools;
Root: HKCU; Subkey: "Software\PGE Team\PGE Engine"; ValueType: string; \
    ValueName: "EnableUserDir"; ValueData: "true"; Flags: uninsdeletekey; Components: engine;
Root: HKCU; Subkey: "Software\Wohlhabend Networks"; Flags: uninsdeletekey;
Root: HKCU; Subkey: "Software\Wohlhabend Networks\PGE Engine"; ValueType: string; \
    ValueName: "EnableUserDir"; ValueData: "true"; Flags: uninsdeletekey; Components: engine;

Root: HKCU; Subkey: "Software\Classes\.lvlx"; ValueType: string; \
    ValueData: "PGEWohlstand.Level"; Flags: uninsdeletekey; Components: devkit\editor devkit\assoc;
Root: HKCU; Subkey: "Software\Classes\PGEWohlstand.Level"; ValueType: string; \
    ValueData: "PGE Level file"; Flags: uninsdeletekey; Components: devkit\editor devkit\assoc;
Root: HKCU; Subkey: "Software\Classes\PGEWohlstand.Level"; ValueType: string; \
    ValueName: "DefaultIcon"; ValueData: """{app}\pge_editor.exe"",1"; Flags: uninsdeletekey; Components: devkit\editor devkit\assoc;
Root: HKCU; Subkey: "Software\Classes\PGEWohlstand.Level\Shell\Open\Command"; ValueType: string; \
    ValueData: """{app}\pge_editor.exe"" ""%1"""; Flags: uninsdeletekey; Components: devkit\editor devkit\assoc;
Root: HKCU; Subkey: "Software\Classes\PGEWohlstand.Level\Shell\PlayLevel"; ValueType: string; \
    ValueData: "Play level"; Flags: uninsdeletekey; Components: engine devkit\assoc;
Root: HKCU; Subkey: "Software\Classes\PGEWohlstand.Level\Shell\PlayLevel\Command"; ValueType: string; \
    ValueData: """{app}\pge_engine.exe"" ""%1"""; Flags: uninsdeletekey; Components: engine devkit\assoc;

Root: HKCU; Subkey: "Software\Classes\.wldx"; ValueType: string; \
    ValueData: "PGEWohlstand.World"; Flags: uninsdeletekey; Components: devkit\editor devkit\assoc;
Root: HKCU; Subkey: "Software\Classes\PGEWohlstand.World"; ValueType: string; \
    ValueData: "PGE World Map"; Flags: uninsdeletekey; Components: devkit\editor devkit\assoc;
Root: HKCU; Subkey: "Software\Classes\PGEWohlstand.World"; ValueType: string; \
    ValueName: "DefaultIcon"; ValueData: """{app}\pge_editor.exe"",2"; Flags: uninsdeletekey; Components: devkit\editor devkit\assoc;
Root: HKCU; Subkey: "Software\Classes\PGEWohlstand.World\Shell\Open\Command"; ValueType: string; \
    ValueData: """{app}\pge_editor.exe"" ""%1"""; Flags: uninsdeletekey; Components: devkit\editor devkit\assoc;

Root: HKCU; Subkey: "Software\Classes\.lvl"; ValueType: string; \
    ValueData: "SMBX64.Level"; Flags: uninsdeletekey; Components: devkit\editor devkit\assoc;
Root: HKCU; Subkey: "Software\Classes\SMBX64.Level"; ValueType: string; \
    ValueData: "SMBX Level File"; Flags: uninsdeletekey; Components: devkit\editor devkit\assoc;
Root: HKCU; Subkey: "Software\Classes\SMBX64.Level"; ValueType: string; \
    ValueName: "DefaultIcon"; ValueData: """{app}\pge_editor.exe"",3"; Flags: uninsdeletekey; Components: devkit\editor devkit\assoc;
Root: HKCU; Subkey: "Software\Classes\SMBX64.Level\Shell\Open\Command"; ValueType: string; \
    ValueData: """{app}\pge_editor.exe"" ""%1"""; Flags: uninsdeletekey; Components: devkit\editor devkit\assoc;
Root: HKCU; Subkey: "Software\Classes\SMBX64.Level\Shell\PlayLevel"; ValueType: string; \
    ValueData: "Play level"; Flags: uninsdeletekey; Components: engine devkit\assoc;
Root: HKCU; Subkey: "Software\Classes\SMBX64.Level\Shell\PlayLevel\Command"; ValueType: string; \
    ValueData: """{app}\pge_engine.exe"" ""%1"""; Flags: uninsdeletekey; Components: engine devkit\assoc;

Root: HKCU; Subkey: "Software\Classes\.wld"; ValueType: string; \
    ValueData: "SMBX64.World"; Flags: uninsdeletekey; Components: devkit\editor devkit\assoc;
Root: HKCU; Subkey: "Software\Classes\SMBX64.World"; ValueType: string; \
    ValueData: "SMBX World File"; Flags: uninsdeletekey; Components: devkit\editor devkit\assoc;
Root: HKCU; Subkey: "Software\Classes\SMBX64.World"; ValueType: string; \
    ValueName: "DefaultIcon"; ValueData: """{app}\pge_editor.exe"",4"; Flags: uninsdeletekey; Components: devkit\editor devkit\assoc;
Root: HKCU; Subkey: "Software\Classes\SMBX64.World\Shell\Open\Command"; ValueType: string; \
    ValueData: """{app}\pge_editor.exe"" ""%1"""; Flags: uninsdeletekey; Components: devkit\editor devkit\assoc;

[Files]
Source: "7za.exe"; DestDir: "{tmp}"; Flags: deleteafterinstall;

[UninstallDelete]
Type: filesandordirs; Name: "{app}\*.*"

[Dirs]
Name: "{app}\configs"
Name: "{%USERPROFILE}\.PGE_Project"; Flags: uninsneveruninstall
Name: "{%USERPROFILE}\.PGE_Project\logs"; Flags: uninsneveruninstall
Name: "{%USERPROFILE}\.PGE_Project\screenshots"; Flags: uninsneveruninstall
Name: "{%USERPROFILE}\.PGE_Project\editor-plugins"; Flags: uninsneveruninstall
Name: "{%USERPROFILE}\.PGE_Project\settings"; Flags: uninsneveruninstall
Name: "{%USERPROFILE}\.PGE_Project\worlds"; Flags: uninsneveruninstall

[Icons]
Name: "{group}\{cm:ProgramOnTheWeb,{#MyAppName}}"; Filename: "{#MyAppURL}"
Name: "{group}\{cm:UninstallProgram,{#MyAppName}}"; Filename: "{uninstallexe}"

Name: "{group}\PGE Editor"; Filename: "{app}\pge_editor.exe";  Components: devkit\editor
Name: "{group}\Changelog (Editor)"; Filename: "{app}\changelog.editor.txt";  Components: devkit\editor
Name: "{group}\PGE Editor Help"; Filename: "{app}\help\manual_editor.html";  Components: devkit\help

Name: "{group}\PGE Engine"; Filename: "{app}\pge_engine.exe";  Components: engine
Name: "{group}\PGE Engine Readme"; Filename: "{app}\GIFs2PNG_Readme.txt";  Components: engine
Name: "{group}\Changelog (Engine)"; Filename: "{app}\changelog.engine.txt";  Components: engine

Name: "{group}\User data directory"; Filename: "{%USERPROFILE}\.PGE_Project";
Name: "{group}\Worlds directory"; Filename: "{%USERPROFILE}\.PGE_Project\worlds";

Name: "{group}\License"; Filename: "{app}\GPLv3.txt";

Name: "{group}\Tools\Playable characters calibrator"; Filename: "{app}\pge_calibrator.exe";  Components: devkit\tools
Name: "{group}\Tools\Maintainer"; Filename: "{app}\pge_maintainer.exe";  Components: devkit\tools
Name: "{group}\Tools\Music testing player"; Filename: "{app}\pge_musplay.exe";  Components: devkit\tools
Name: "{group}\Tools\GIFs2PNG Console tool readme"; Filename: "{app}\GIFs2PNG_Readme.txt";  Components: devkit\tools
Name: "{group}\Tools\PNG2GIFs Console tool readme"; Filename: "{app}\PNG2GIFs_Readme.txt";  Components: devkit\tools
Name: "{group}\Tools\LazyFixTool Console tool readme"; Filename: "{app}\LazyFixTool_Readme.txt";  Components: devkit\tools


[Run]
; === Executable components ===
StatusMsg: "Installing Runtime libraries..."; \
  Filename: {tmp}\7za.exe; \
  Parameters: "x ""{tmp}\runtime.zip"" -o""{app}\"" * -r -aoa"; \
  Flags: runhidden runascurrentuser; Components: devkit\runtime
StatusMsg: "Installing Editor..."; \
  Filename: {tmp}\7za.exe; \
  Parameters: "x ""{tmp}\editor.zip"" -o""{app}\"" * -r -aoa"; \
  Flags: runhidden runascurrentuser; Components: devkit\editor
StatusMsg: "Installing Tools..."; \
  Filename: {tmp}\7za.exe; \
  Parameters: "x ""{tmp}\tools.zip"" -o""{app}\"" * -r -aoa"; \
  Flags: runhidden runascurrentuser; Components: devkit\tools
StatusMsg: "Installing Engine..."; \
  Filename: {tmp}\7za.exe; \
  Parameters: "x ""{tmp}\engine.zip"" -o""{app}\"" * -r -aoa"; \
  Flags: runhidden runascurrentuser; Components: engine

; === Manual ===
StatusMsg: "Installing Help..."; \
  Filename: {tmp}\7za.exe; \
  Parameters: "x ""{tmp}\help.zip"" -o""{app}\"" * -r -aoa"; \
  Flags: runhidden runascurrentuser; Components: devkit\help

; === Config packs ===
StatusMsg: "Installing A2XT Config Pack..."; \
  Filename: {tmp}\7za.exe; \
  Parameters: "x ""{tmp}\config-a2xt.zip"" -o""{app}\configs\"" * -r -aoa"; \
  Flags: runhidden runascurrentuser; Components: configs\a2xt
StatusMsg: "Installing SMBX Integration Config Pack..."; \
  Filename: {tmp}\7za.exe; \
  Parameters: "x ""{tmp}\config-smbxint.zip"" -o""{app}\configs\"" * -r -aoa"; \
  Flags: runhidden runascurrentuser; Components: configs\smbxint
StatusMsg: "Installing SMBX 1.3 Compatibility Config Pack..."; \
  Filename: {tmp}\7za.exe; \
  Parameters: "x ""{tmp}\config-smbx13.zip"" -o""{app}\configs\"" * -r -aoa"; \
  Flags: runhidden runascurrentuser; Components: configs\smbx13
StatusMsg: "Installing SMBX Redrawn Config Pack..."; \
  Filename: {tmp}\7za.exe; \
  Parameters: "x ""{tmp}\config-smbxredraw.zip"" -o""{app}\configs\"" * -r -aoa"; \
  Flags: runhidden runascurrentuser; Components: configs\smbxredraw
StatusMsg: "Installing SMB-Xpanded Config Pack..."; \
  Filename: {tmp}\7za.exe; \
  Parameters: "x ""{tmp}\config-smbxpanded.zip"" -o""{app}\configs\"" * -r -aoa"; \
  Flags: runhidden runascurrentuser; Components: configs\smbxpanded
StatusMsg: "Installing SMBX-38A Config Pack..."; \
  Filename: {tmp}\7za.exe; \
  Parameters: "x ""{tmp}\config-smbx38a.zip"" -o""{app}\configs\"" * -r -aoa"; \
  Flags: runhidden runascurrentuser; Components: configs\smbx38a

[CustomMessages]
en.PGE_NewInstaller =New version of online installer has been detected. Do you want to download and start it?
de.PGE_NewInstaller =Neue Version des Online-Installers wurde erkannt. Möchten Sie herunterladen und starten?
pl.PGE_NewInstaller =Znaleziono nową wersję instalatora online. Czy chcesz go pobrać i uruchomić?
ru.PGE_NewInstaller =Найдена новая версия онлайн-установщика. Хотите загрузить его и запустить?
es.PGE_NewInstaller =Se ha detectado una nueva versión del instalador en línea. ¿Desea descargarlo e iniciarlo?

en.PGE_CantConnect  =Can't connect to remote server! Please check your internet connection and try to start this setup again.
de.PGE_CantConnect  =Kann keine Verbindung zum Remote-Server herstellen! Bitte überprüfen Sie Ihre Internetverbindung und versuchen Sie, dieses Setup erneut zu starten.
pl.PGE_CantConnect  =Nie można połączyć się z serwerem zdalnym! Sprawdź połączenie z Internetem i spróbuj ponownie uruchomić tę instalację.
ru.PGE_CantConnect  =Невозможно подключиться к удалённому серверу! Пожалуйста, проверьте Ваше соединение с Интернетом и попробуйте запустить эту установку снова.
es.PGE_CantConnect  =No se puede conectar al servidor remoto! Compruebe su conexión a Internet y vuelva a intentar iniciar esta configuración.

en.PGE_UpdateChannel =Updates channel
ru.PGE_UpdateChannel =Канал обновлений

en.PGE_UpdateChannelDesc =Which update channel you want to use?
ru.PGE_UpdateChannelDesc =Какой канал обновлений вы хотите использовать?

en.PGE_UpdateChannelDetail =Please specify update channel from which you want to install PGE Project, then click Next.
ru.PGE_UpdateChannelDetail =Пожалуйста, укажите канал обновлений с которого вы хотите установить PGE Project, затем нажмите 'Далее'.

en.PGE_ChanRelease =Release (Stable builds)
ru.PGE_ChanRelease =Релизный (Стабильные сборки)

en.PGE_ChanLab =Laboratory (Fresh experimental builds)
ru.PGE_ChanLab =Лаборатория (Свежие экспериментальные сборки)


[Code]
var
    UpdateChannelPage: TInputOptionWizardPage;
    UpdateChannelId: Integer;

function IfThen(ACondition: Boolean; ATrue: string; AFalse: string): string;
var Res : string;
begin
    if ACondition
    then
        Res := ATrue
    else
        Res := AFalse;
    Result := Res;
end;

function InitializeSetup(): Boolean;
var
    FileList: TStringList;
    verFile: string;
    ResultCode: Integer;
begin
    UpdateChannelId := 0;
    verFile := ExpandConstant('{tmp}\installer_version.txt');
    idpDownloadFile('{#INSTALLER_VERSION_CHECK}', verFile);
    if (FileExists(verFile)) then
    Begin
        FileList := TstringList.Create;
        FileList.LoadFromFile(ExpandConstant('{tmp}\installer_version.txt'));
        If (FileList.Count > 0) AND (StrToFloat(FileList.Strings[0]) > {#INSTALLER_VERSION}) Then
        Begin
            if (MsgBox(ExpandConstant('{cm:PGE_NewInstaller}'), mbConfirmation, MB_YESNO) = IDYES) then
            begin
                idpDownloadFile('{#NEW_INSTALLER_URL}', ExpandConstant('{tmp}\new_installer.exe'));
                Exec(ExpandConstant('{tmp}\new_installer.exe'), '', '', SW_SHOW, ewNoWait, ResultCode);
                Result := False;
                exit;
            end
        End;
    End Else
    Begin
        MsgBox(ExpandConstant('{cm:PGE_CantConnect}'), mbError, MB_OK);
        Result := False;
        exit;
    End;
    Result := True;
end;

procedure InitializeWizard;
begin
    UpdateChannelPage := CreateInputOptionPage(wpSelectComponents,
      ExpandConstant('{cm:PGE_UpdateChannel}'), ExpandConstant('{cm:PGE_UpdateChannelDesc}'),
      ExpandConstant('{cm:PGE_UpdateChannelDetail}'), True, False);
    UpdateChannelPage.Add(ExpandConstant('{cm:PGE_ChanRelease}'));
    UpdateChannelPage.Add(ExpandConstant('{cm:PGE_ChanLab}'));

    case GetPreviousData('UpdateChannel', '') of
        'release':    UpdateChannelPage.SelectedValueIndex := 0;
        'laboratory': UpdateChannelPage.SelectedValueIndex := 1;
    else
        UpdateChannelPage.SelectedValueIndex := 0;
    end;
end;

function NextButtonClick(CurPageID: Integer): Boolean;
var
    UpdateChannel: String;
begin
  { Validate certain pages before allowing the user to proceed }
  if CurPageID = UpdateChannelPage.ID then begin
      UpdateChannelId := UpdateChannelPage.SelectedValueIndex
      case UpdateChannelPage.SelectedValueIndex of
          0: UpdateChannel := 'stable';
          1: UpdateChannel := 'laboratory';
      end;

      if UpdateChannelId = 0 then
      begin
          idpClearFiles();
          idpAddFileComp(IfThen(IsWin64, '{#FILE_URL_COMMON64}', '{#FILE_URL_COMMON32}'),  ExpandConstant('{tmp}\runtime.zip'), 'devkit\runtime');
          idpAddFileComp(IfThen(IsWin64, '{#FILE_URL_EDITOR64}', '{#FILE_URL_EDITOR32}'),  ExpandConstant('{tmp}\editor.zip'),  'devkit\editor');
          idpAddFileComp(IfThen(IsWin64, '{#FILE_URL_TOOLS64}',  '{#FILE_URL_TOOLS32}'),   ExpandConstant('{tmp}\tools.zip'),   'devkit\tools');
          idpAddFileComp('{#FILE_URL_HELP}',      ExpandConstant('{tmp}\help.zip'),    'devkit\help');
          idpAddFileComp(IfThen(IsWin64, '{#FILE_URL_ENGINE64}', '{#FILE_URL_ENGINE32}'),  ExpandConstant('{tmp}\engine.zip'),  'engine');

          idpAddFileComp('{#FILE_URL_CONFIG_A2XT}',    ExpandConstant('{tmp}\config-a2xt.zip'),           'configs\a2xt');
          idpAddFileComp('{#FILE_URL_CONFIG_SMBXINT}', ExpandConstant('{tmp}\config-smbxint.zip'),        'configs\smbxint');
          idpAddFileComp('{#FILE_URL_CONFIG_SMBX13}',  ExpandConstant('{tmp}\config-smbx13.zip'),         'configs\smbx13');
          idpAddFileComp('{#FILE_URL_CONFIG_SMBXREDRAW}',  ExpandConstant('{tmp}\config-smbxredraw.zip'), 'configs\smbxredraw');
          idpAddFileComp('{#FILE_URL_CONFIG_SMBXPANDED}',  ExpandConstant('{tmp}\config-smbxpanded.zip'), 'configs\smbxpanded');
          idpAddFileComp('{#FILE_URL_CONFIG_SMBX38A}', ExpandConstant('{tmp}\config-smbx38a.zip'),        'configs\smbx38a');
          idpDownloadAfter(wpReady);
      end else

      if UpdateChannelId = 1 then
      begin
          idpClearFiles();
          idpAddFileComp(IfThen(IsWin64, '{#LAB_FILE_URL_COMMON64}', '{#LAB_FILE_URL_COMMON32}'),  ExpandConstant('{tmp}\runtime.zip'), 'devkit\runtime');
          idpAddFileComp(IfThen(IsWin64, '{#LAB_FILE_URL_EDITOR64}', '{#LAB_FILE_URL_EDITOR32}'),  ExpandConstant('{tmp}\editor.zip'),  'devkit\editor');
          idpAddFileComp(IfThen(IsWin64, '{#LAB_FILE_URL_TOOLS64}',  '{#LAB_FILE_URL_TOOLS32}'),   ExpandConstant('{tmp}\tools.zip'),   'devkit\tools');
          idpAddFileComp('{#LAB_FILE_URL_HELP}',      ExpandConstant('{tmp}\help.zip'),    'devkit\help');
          idpAddFileComp(IfThen(IsWin64, '{#FILE_URL_ENGINE64}', '{#LAB_FILE_URL_ENGINE32}'),  ExpandConstant('{tmp}\engine.zip'),  'engine');

          idpAddFileComp('{#LAB_FILE_URL_CONFIG_A2XT}',    ExpandConstant('{tmp}\config-a2xt.zip'),           'configs\a2xt');
          idpAddFileComp('{#LAB_FILE_URL_CONFIG_SMBXINT}', ExpandConstant('{tmp}\config-smbxint.zip'),        'configs\smbxint');
          idpAddFileComp('{#LAB_FILE_URL_CONFIG_SMBX13}',  ExpandConstant('{tmp}\config-smbx13.zip'),         'configs\smbx13');
          idpAddFileComp('{#LAB_FILE_URL_CONFIG_SMBXREDRAW}',  ExpandConstant('{tmp}\config-smbxredraw.zip'), 'configs\smbxredraw');
          idpAddFileComp('{#LAB_FILE_URL_CONFIG_SMBXPANDED}',  ExpandConstant('{tmp}\config-smbxpanded.zip'), 'configs\smbxpanded');
          idpAddFileComp('{#LAB_FILE_URL_CONFIG_SMBX38A}', ExpandConstant('{tmp}\config-smbx38a.zip'),        'configs\smbx38a');
          idpDownloadAfter(wpReady);
      end;
  end;
  Result := True;
end;

procedure RegisterPreviousData(PreviousDataKey: Integer);
var
    UpdateChannel: String;
begin
    { Store the settings so we can restore them next time }
    case UpdateChannelPage.SelectedValueIndex of
        0: UpdateChannel := 'stable';
        1: UpdateChannel := 'laboratory';
    end;
    // MsgBox(UpdateChannel, mbError, MB_OK);
    SetPreviousData(PreviousDataKey, 'UpdateChannel', UpdateChannel);
end;
