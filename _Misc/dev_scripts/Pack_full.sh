#!/bin/bash
source ./_paths.sh
CurDir=~+

echo "Preparing Linux deploy..."
cd $SOURCEDIR/bin

DeployDir="$SOURCEDIR/bin/_linux_deploy"
PgePrjSD="PGE_Project"
TarGzArName="pge-editor-dev-linux-mint.tar.gz"


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

cd $DeployDir
if [ -f ./$TarGzArName ]; then rm -f ./$TarGzArName; fi
tar -zcvf ./$TarGzArName $PgePrjSD

#NFS [ Network File System != Need For Speed, BUT, Need For Speed Up our network channel! ]
echo "Copying data..."
cp $SOURCEDIR/Editor/android-build/bin/QtApp-release-signed.apk $SiteRootNFS/$LabDir/pge_editor_dev.apk
cp $DeployDir/pge-editor-dev-linux-mint.tar.gz $SiteRootNFS/$LabDir/pge-editor-dev-linux-mint.tar.gz

#SAMBA
#echo -n "Type samba password: "
#TempPasswd=""
#read TempPasswd
#echo "Copying data..."
#cp $SOURCEDIR/Editor/android-build/bin/QtApp-release-signed.apk $SOURCEDIR/Editor/android-build/bin/pge_editor_dev.apk
#smbclient $SiteRoot $TempPasswd -u vitaly -c "cd $LabDir; \
#lcd $SOURCEDIR/Editor/android-build/bin/; prompt; recurse; mput pge_editor_dev.apk; \
#lcd $DeployDir; recurse; mput pge-editor-dev-linux-mint.tar.gz; exit;"

echo ""
echo ""
echo "All done!"
echo ""
cd $CurDir
read -n 1

