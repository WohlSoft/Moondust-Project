#!/bin/bash
InitDir=~+
cd ..
source ./_paths.sh
CurDir=~+

cd $SOURCEDIR/Editor
source $SOURCEDIR/_paths.sh

$LRelease pge_editor.pro
cp languages/*.qm $SOURCEDIR/bin/languages
cp languages/*.png $SOURCEDIR/bin/languages

echo "Preparing Linux deploy..."
cd $SOURCEDIR/bin

DeployDir="$SOURCEDIR/bin/_linux_deploy"
PgePrjSD="PGE_Project"
TarGzArName="pge-project-dev-linux-mint.tar.gz"


if [ ! -d "$DeployDir" ]; then
	mkdir "$DeployDir"
	mkdir "$DeployDir/$PgePrjSD"
fi

$CurDir/upx-linux-x64 pge_editor
$CurDir/upx-linux-x64 PNG2GIFs
$CurDir/upx-linux-x64 GIFs2PNG
$CurDir/upx-linux-x64 LazyFixTool
$CurDir/upx-linux-x64 pge_calibrator
$CurDir/upx-linux-x64 pge_engine
cp pge_editor "$DeployDir/$PgePrjSD"
cp GIFs2PNG "$DeployDir/$PgePrjSD"
cp PNG2GIFs "$DeployDir/$PgePrjSD"
cp LazyFixTool "$DeployDir/$PgePrjSD"
cp pge_calibrator "$DeployDir/$PgePrjSD"
cp pge_engine "$DeployDir/$PgePrjSD"

cp -a *.so* "$DeployDir/$PgePrjSD"
cp -r ./languages "$DeployDir/$PgePrjSD"
cp -r ./themes "$DeployDir/$PgePrjSD"
if [ ! -d "$DeployDir/$PgePrjSD/help" ]; then
	mkdir "$DeployDir/$PgePrjSD/help"
	echo "<html><head><meta http-equiv=\"refresh\" content=\"0; url=http://help.engine.wohlnet.ru/manual_editor.html\"/></head><body></body></html>" > "$DeployDir/$PgePrjSD/help/manual_editor.htm"
fi
cp -r ./calibrator "$DeployDir/$PgePrjSD"
cp $SOURCEDIR/Content/readmes/*.txt "$DeployDir/$PgePrjSD"
cp "$SOURCEDIR/Editor/changelog.editor.txt" "$DeployDir/$PgePrjSD"
cp "$SOURCEDIR/LICENSE" "$DeployDir/$PgePrjSD/license.txt"

rm $DeployDir/$PgePrjSD/themes/*.zip
rm $DeployDir/$PgePrjSD/themes/pge_default/*.zip

cd $DeployDir
if [ -f ./$TarGzArName ]; then rm -f ./$TarGzArName; fi
tar -zcvf ./$TarGzArName $PgePrjSD

if [ ! -d "$SOURCEDIR/bin/_packed" ]; then
	mkdir "$SOURCEDIR/bin/_packed";
fi
if [ -f "$SOURCEDIR/bin/_packed/$TarGzArName" ]; then
	rm "$SOURCEDIR/bin/_packed/$TarGzArName";
fi
mv ./$TarGzArName "$SOURCEDIR/bin/_packed/$TarGzArName"

echo ""
echo "All done!"
echo ""
cd $InitDir
read -n 1

