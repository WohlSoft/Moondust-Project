call _paths.bat

echo copy updated archives to Google Drive
copy %LabDir%\pge_engine_dev.zip "%G_DRIVE%\_laboratory\"

copy %LabDir%\pge_editor_dev.zip "%G_DRIVE%\_laboratory\"
copy %LabDir%\pge_tools_dev.zip "%G_DRIVE%\_laboratory\"
copy %LabDir%\pge_help_standalone.zip "%G_DRIVE%\_laboratory\"

echo Updating of SMBX config pack data
copy %LabDir%\SMBX_config_pack_full.zip "%G_DRIVE%\_config_packs\SMBX13\"
copy %LabDir%\SMBX_config_pack_lite.zip "%G_DRIVE%\_config_packs\SMBX13\"

xcopy /Y /E /I "%LabDir%\..\_configs\SMBX13\*.*" "%G_DRIVE%\_config_packs\SMBX13\"

echo Updating of A2MBXT config pack data
xcopy /Y /E /I "%LabDir%\..\_configs\A2MBXT\*.*" "%G_DRIVE%\_config_packs\A2MBXT\"

echo Updating of SMBXInt config pack data
xcopy /Y /E /I "%LabDir%\..\_configs\SMBXInt\*.*" "%G_DRIVE%\_config_packs\SMBXInt\"

echo Updating of SMBX_Redrawn config pack data
xcopy /Y /E /I "%LabDir%\..\_configs\SMBX_Redrawn\*.*" "%G_DRIVE%\_config_packs\SMBX_Redrawn\"
