#!/bin/bash
InitDir=~+

function osxabspath()
{
  case "${1}" in
  [./]*)
   echo "$(cd ${1%/*}; pwd)/${1##*/}"
   ;;
  *)
   echo "${PWD}/${1}"
   ;;
  esac
}

#=============Detect directory that contains script=====================
SOURCE="${BASH_SOURCE[0]}"
while [ -h "$SOURCE" ]; do # resolve $SOURCE until the file is no longer a symlink
  CurDir="$( cd -P "$( dirname "$SOURCE" )" && pwd )"
  SOURCE="$(osxabspath "$SOURCE")"
  [[ $SOURCE != /* ]] && SOURCE="$DIR/$SOURCE" # if $SOURCE was a relative symlink, we need to resolve it relative to the path where the symlink file was located
done
CurDir="$( cd -P "$( dirname "$SOURCE" )" && pwd )"
#=======================================================================
cd ..
CurDir=$CurDir/../
source ./_paths.sh

cd $SOURCEDIR/Editor
source $SOURCEDIR/_paths.sh

$LRelease pge_editor.pro
cp languages/*.qm $SOURCEDIR/bin/languages
cp languages/*.png $SOURCEDIR/bin/languages

#Delete junk translation file causes German Ok/Cancel translations in the dialogs
if [ -f "$SOURCEDIR/bin/languages/qt_en.qm" ]; then
	rm "$SOURCEDIR/bin/languages/qt_en.qm"
fi
if [ -f "$SOURCEDIR/bin/translations/qt_en.qm" ]; then
	rm "$SOURCEDIR/bin/translations/qt_en.qm"
fi

echo "Preparing Mac OS X deploy..."
cd $SOURCEDIR/bin

DeployDir="$SOURCEDIR/bin/_macosx_deploy"
PgePrjSD_dmg="pge_project"
PgePrjSD="pge_project/PGE Project"
TarGzArName="pge-project-dev-macosx.dmg"

if [ ! -d "$DeployDir" ]; then
        mkdir "$DeployDir"
fi
if [ ! -d "$DeployDir/$PgePrjSD" ]; then
        mkdir -p "$DeployDir/$PgePrjSD"
fi

#$CurDir/upx-linux-x64 pge_editor
#$CurDir/upx-linux-x64 PNG2GIFs
#$CurDir/upx-linux-x64 GIFs2PNG
#$CurDir/upx-linux-x64 LazyFixTool
#$CurDir/upx-linux-x64 pge_calibrator
#$CurDir/upx-linux-x64 pge_engine
#$CurDir/upx-linux-x64 pge_musplay
cp -a pge_editor.app "$DeployDir/$PgePrjSD/PGE Editor.app"
cp GIFs2PNG "$DeployDir/$PgePrjSD"
cp PNG2GIFs "$DeployDir/$PgePrjSD"
cp LazyFixTool "$DeployDir/$PgePrjSD"
cp -a pge_calibrator.app "$DeployDir/$PgePrjSD/Character Calibrator.app"
cp -a pge_engine.app "$DeployDir/$PgePrjSD/pge_engine.app"
cp -a pge_musplay.app "$DeployDir/$PgePrjSD/pge_musplay.app"

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
rm $DeployDir/$PgePrjSD/themes/pge_default/*.zip
fi

if [ -f "$DeployDir/$PgePrjSD/Data directory" ]; then
    rm "$DeployDir/$PgePrjSD/Data directory"
fi
#ln -s ~/Library/Application\ Support/PGE\ Project "$DeployDir/$PgePrjSD/Data directory"

cd $DeployDir
if [ -f ./$TarGzArName ]; then rm -f ./$TarGzArName; fi
"$CurDir/create-dmg.sh" --volname "PGE Project" --window-size 800 600 --app-drop-link 450 320 "$DeployDir/$TarGzArName" "$DeployDir/$PgePrjSD_dmg"

if [ ! -d "$SOURCEDIR/bin/_packed" ]; then
	mkdir "$SOURCEDIR/bin/_packed";
fi
if [ -f "$SOURCEDIR/bin/_packed/$TarGzArName" ]; then
	rm "$SOURCEDIR/bin/_packed/$TarGzArName";
fi

if [ -f "$DeployDir/$TarGzArName" ]; then
        mv "$DeployDir/$TarGzArName" "$SOURCEDIR/bin/_packed/$TarGzArName"
else 
    if [ -f "$DeployDir/rw.$TarGzArName" ]; then
            mv "$DeployDir/rw.$TarGzArName" "$SOURCEDIR/bin/_packed/$TarGzArName"
    else
            echo "Built DMG file not found. Nothing to move!"
    fi
fi

echo ""
echo ""
echo "All done!"
echo ""
cd $InitDir
if [[ $1 != "nopause" ]]; then read -n 1; fi

