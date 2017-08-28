
!define PGE_CAPTION "Laboratory PGE Online Installer for x86 and x86_64"

!ifndef PGE_OUTFILE
!define PGE_OUTFILE "pge-online-installer-dev.exe"
!endif

;Branding images
!define MUI_WELCOMEFINISHPAGE_BITMAP "pge_installer_lab.bmp"
!define MUI_ICON "pge_install_icon.ico"

;The welcome page texts
!define MUI_WELCOMEPAGE_TITLE "Welcome to Laboratory PGE Online Installer!"
!define MUI_WELCOMEPAGE_TEXT "It'll automatically download and extract most fresh dev-builds for you.\r\n\r\nNo need to close any programs as no unistall information will be added.\r\n\r\nNote: downloading builds are experimental, and in some moment it may be buggy or unstable. Please report about any bugs and crashes here: http://wohlsoft.ru/forum/memberlist.php?mode=contactadmin\r\n\r\nClick $\"Next$\" to continue."

;Files download links
!define CUSTOM_FILES
!define FILE_URL_COMMON64 "http://wohlsoft.ru/docs/_laboratory/_Builds/win32/bin-w64/_packed/install-pge-common-dev-win64.zip"
!define FILE_URL_COMMON32 "http://wohlsoft.ru/docs/_laboratory/_Builds/win32/bin-w32/_packed/install-pge-common-dev-win32.zip"

!define FILE_URL_EDITOR64 "http://wohlsoft.ru/docs/_laboratory/_Builds/win32/bin-w64/_packed/install-pge-editor-dev-win64.zip"
!define FILE_URL_EDITOR32 "http://wohlsoft.ru/docs/_laboratory/_Builds/win32/bin-w32/_packed/install-pge-editor-dev-win32.zip"

!define FILE_URL_ENGINE64 "http://wohlsoft.ru/docs/_laboratory/_Builds/win32/bin-w64/_packed/install-pge-engine-dev-win64.zip"
!define FILE_URL_ENGINE32 "http://wohlsoft.ru/docs/_laboratory/_Builds/win32/bin-w32/_packed/install-pge-engine-dev-win32.zip"

!define FILE_URL_TOOLS64  "http://wohlsoft.ru/docs/_laboratory/_Builds/win32/bin-w64/_packed/install-pge-tools-dev-win64.zip"
!define FILE_URL_TOOLS32  "http://wohlsoft.ru/docs/_laboratory/_Builds/win32/bin-w32/_packed/install-pge-tools-dev-win32.zip"

!define FILE_URL_HELP     "http://wohlsoft.ru/docs/_laboratory/pge_help_standalone.zip"

!define FILE_URL_CONFIG_SMBXINT 	"http://wohlsoft.ru/docs/_laboratory/config_packs/SMBX_Integration.zip"
!define FILE_URL_CONFIG_SMBX13		"http://wohlsoft.ru/docs/_laboratory/config_packs/SMBX_13_compatible.zip"
!define FILE_URL_CONFIG_A2XT		"http://wohlsoft.ru/docs/_laboratory/config_packs/A2XT.zip"
!define FILE_URL_CONFIG_ASMBXT		"http://wohlsoft.ru/docs/_laboratory/config_packs/Raocow_talkhaus_full.zip"
!define FILE_URL_CONFIG_SMBXREDRAW  "http://wohlsoft.ru/docs/_laboratory/config_packs/SMBX_Redrawn_full.zip"
!define FILE_URL_CONFIG_SMBXPANDED  "http://wohlsoft.ru/docs/_laboratory/config_packs/SMBXpanded_full.zip"
!define FILE_URL_CONFIG_SMBX38A		"http://wohlsoft.ru/docs/_laboratory/config_packs/SMBXpanded_full.zip"

!include pge-online-installer.nsi
