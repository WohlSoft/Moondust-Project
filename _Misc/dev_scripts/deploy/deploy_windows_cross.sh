#!/bin/bash
InitDir=$PWD
cd ..
source ./_paths.sh
CurDir=$PWD

cd $SOURCEDIR/Editor
source $SOURCEDIR/_paths.sh

$LRelease pge_editor.pro
cp languages/*.qm $SOURCEDIR/bin-w32/languages
cp languages/*.png $SOURCEDIR/bin-w32/languages

echo "Preparing Linux deploy..."
cd $SOURCEDIR/bin-w32

DeployDir="$SOURCEDIR/bin-w32/_win32_deploy"
PgePrjSD="PGE_Project"
TarGzArName="pge-project-dev-win32.zip"

if [ -d "$DeployDir" ]; then
	rm -Rf "$DeployDir"
fi
mkdir "$DeployDir"
mkdir "$DeployDir/$PgePrjSD"

$CurDir/upx-linux-x64 --compress-icons=0 pge_editor.exe
$CurDir/upx-linux-x64 --compress-icons=0 PNG2GIFs.exe
$CurDir/upx-linux-x64 --compress-icons=0 GIFs2PNG.exe
$CurDir/upx-linux-x64 --compress-icons=0 LazyFixTool.exe
$CurDir/upx-linux-x64 --compress-icons=0 pge_calibrator.exe
$CurDir/upx-linux-x64 --compress-icons=0 pge_engine.exe
$CurDir/upx-linux-x64 --compress-icons=0 pge_manager.exe
$CurDir/upx-linux-x64 --compress-icons=0 pge_maintainer.exe
$CurDir/upx-linux-x64 --compress-icons=0 SDL2.dll
$CurDir/upx-linux-x64 --compress-icons=0 libFLAC-8.dll
$CurDir/upx-linux-x64 --compress-icons=0 libFLAC++-6.dll
$CurDir/upx-linux-x64 --compress-icons=0 libmodplug-1.dll
cp pge_editor.exe "$DeployDir/$PgePrjSD"
cp GIFs2PNG.exe "$DeployDir/$PgePrjSD"
cp PNG2GIFs.exe "$DeployDir/$PgePrjSD"
cp LazyFixTool.exe "$DeployDir/$PgePrjSD"
cp pge_calibrator.exe "$DeployDir/$PgePrjSD"
cp pge_engine.exe "$DeployDir/$PgePrjSD"

cp -a *.dll "$DeployDir/$PgePrjSD"
cp -r ./languages "$DeployDir/$PgePrjSD"
cp -r ./themes "$DeployDir/$PgePrjSD"
if [ ! -d "$DeployDir/$PgePrjSD/help" ]; then
	mkdir "$DeployDir/$PgePrjSD/help"
	echo "<html><head><meta http-equiv=\"refresh\" content=\"0; url=http://help.engine.wohlnet.ru/manual_editor.html\"/></head><body></body></html>" > "$DeployDir/$PgePrjSD/help/manual_editor.htm"
fi
cp -r ./calibrator "$DeployDir/$PgePrjSD"
cp $SOURCEDIR/Content/readmes/*.txt "$DeployDir/$PgePrjSD"
cp "$SOURCEDIR/Editor/changelog.editor.txt" "$DeployDir/$PgePrjSD"
cp "$SOURCEDIR/Engine/changelog.engine.txt" "$DeployDir/$PgePrjSD"
cp "$SOURCEDIR/LICENSE" "$DeployDir/$PgePrjSD/license.txt"

rm $DeployDir/$PgePrjSD/themes/*.zip
rm $DeployDir/$PgePrjSD/themes/pge_default/*.zip
mkdir "$DeployDir/$PgePrjSD/configs"
mkdir "$DeployDir/$PgePrjSD/worlds"
mkdir "$DeployDir/$PgePrjSD/screenshots"
mkdir "$DeployDir/$PgePrjSD/logs"
mkdir "$DeployDir/$PgePrjSD/tools"
unzip "$CurDir/../tools/sox-win32.zip" -d "$DeployDir/$PgePrjSD/tools"

echo "Creating INI-files with portable config"
echo "[Main]"> "$DeployDir/$PgePrjSD/pge_editor.ini"
echo "force-portable=true">> "$DeployDir/$PgePrjSD/pge_editor.ini"
echo "[Main]"> "$DeployDir/$PgePrjSD/pge_engine.ini"
echo "force-portable=true">> "$DeployDir/$PgePrjSD/pge_engine.ini"
echo "[Main]"> "$DeployDir/$PgePrjSD/pge_calibrator.ini"
echo "force-portable=true">> "$DeployDir/$PgePrjSD/pge_calibrator.ini"

cd $DeployDir
if [ -f ./$TarGzArName ]; then rm -f ./$TarGzArName; fi
zip -9 -r ./$TarGzArName $PgePrjSD

if [ ! -d "$SOURCEDIR/bin-w32/_packed" ]; then
	mkdir "$SOURCEDIR/bin-w32/_packed";
fi
if [ -f "$SOURCEDIR/bin-w32/_packed/$TarGzArName" ]; then
	rm "$SOURCEDIR/bin-w32/_packed/$TarGzArName";
fi
mv ./$TarGzArName "$SOURCEDIR/bin-w32/_packed/$TarGzArName"

cd "$DeployDir/$PgePrjSD/"

PGECommon=
PGECommon=$PGECommon' ./*.dll'
PGECommon=$PGECommon' ./license.txt'
PGECommon=$PGECommon' ./LICENSE.'*'.txt'
PGECommon=$PGECommon' ./languages/*'
PGECommon=$PGECommon' ./help/*'

PGEEditor=
PGEEditor=$PGEEditor' ./changelog.editor.txt'
PGEEditor=$PGEEditor' ./pge_editor.exe'
PGEEditor=$PGEEditor' ./themes/*'
PGEEditor=$PGEEditor' ./tools/*'

PGEEngine=
PGEEngine=$PGEEngine' ./pge_engine.exe'
PGEEngine=$PGEEngine' ./Engine.Readme.txt'
PGEEngine=$PGEEngine' ./changelog.engine.txt'

PGETools=
PGETools=$PGETools' ./GIFs2PNG.exe'
PGETools=$PGETools' ./'*'Readme.txt'
PGETools=$PGETools' ./LazyFixTool.exe'
PGETools=$PGETools' ./PNG2GIFs.exe'
PGETools=$PGETools' ./pge_calibrator.exe'
PGETools=$PGETools' ./calibrator/*'

echo "Packing of editor data\n\n\n"
zip -9 -r "../install-pge-common-dev-win32.zip" $PGECommon
zip -9 -r "../install-pge-editor-dev-win32.zip" $PGEEditor
zip -9 -r "../install-pge-engine-dev-win32.zip" $PGEEngine
zip -9 -r "../install-pge-tools-dev-win32.zip" $PGETools
mv ../*.zip "$SOURCEDIR/bin-w32/_packed"

echo ""
echo "All done!"
echo ""
cd $InitDir
read -n 1

