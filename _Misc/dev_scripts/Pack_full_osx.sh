#!/bin/bash
source ./_paths.sh
CurDir=~+

cd $SOURCEDIR/Editor
source $SOURCEDIR/_paths.sh

$LRelease pge_editor.pro
cp languages/*.qm $SOURCEDIR/bin/languages
cp languages/*.png $SOURCEDIR/bin/languages

echo "Preparing Mac OS X deploy..."
cd $SOURCEDIR/bin

DeployDir="$SOURCEDIR/bin/_macosx_deploy"
PgePrjSD_dmg="pge_prject"
PgePrjSD="pge_prject/PGE Project"
TarGzArName="pge-project-dev-macosx.dmg"

if [ ! -d "$DeployDir" ]; then
        mkdir "$DeployDir"
fi
if [ ! -d "$DeployDir/$PgePrjSD" ]; then
        mkdir -p "$DeployDir/$PgePrjSD"
fi
if [ ! -d "$DeployDir/$PgePrjSD/configs" ]; then
        mkdir "$DeployDir/$PgePrjSD/configs"
fi

#$CurDir/upx-linux-x64 pge_editor
#$CurDir/upx-linux-x64 PNG2GIFs
#$CurDir/upx-linux-x64 GIFs2PNG
#$CurDir/upx-linux-x64 LazyFixTool
#$CurDir/upx-linux-x64 pge_calibrator
#$CurDir/upx-linux-x64 pge_engine
cp -a pge_editor.app "$DeployDir/$PgePrjSD/PGE Editor.app"
cp GIFs2PNG "$DeployDir/$PgePrjSD"
cp PNG2GIFs "$DeployDir/$PgePrjSD"
cp LazyFixTool "$DeployDir/$PgePrjSD"
cp -a pge_calibrator.app "$DeployDir/$PgePrjSD/Character Calibrator.app"
cp -a pge_engine.app "$DeployDir/$PgePrjSD/pge_engine.app"

if [ -d "$DeployDir/$PgePrjSD/_Libs" ]; then
    rm -Rf "$DeployDir/$PgePrjSD/_Libs"
fi
cp -a _Libs "$DeployDir/$PgePrjSD"

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

if [ -f "$DeployDir/$PgePrjSD/themes/*.zip" ]; then
rm "$DeployDir/$PgePrjSD/themes/*.zip"
fi

if [ -f "$DeployDir/$PgePrjSD/Data directory" ]; then
    rm "$DeployDir/$PgePrjSD/Data directory"
fi
ln -s ~/Library/Application\ Support/PGE\ Project "$DeployDir/$PgePrjSD/Data directory"

cd $DeployDir
if [ -f ./$TarGzArName ]; then rm -f ./$TarGzArName; fi
"$CurDir/create-dmg.sh" --volname "PGE Project" --window-size 800 600 --app-drop-link 450 320 "$DeployDir/$TarGzArName" "$DeployDir/$PgePrjSD_dmg"

echo ""
echo ""
echo "All done!"
echo ""
cd $CurDir
read -n 1
