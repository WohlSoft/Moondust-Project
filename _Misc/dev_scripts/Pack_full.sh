#!/bin/bash
source ./_paths.sh
CurDir=~+

echo "Preparing Linux deploy..."
cd $SOURCEDIR/bin

DeployDir="$SOURCEDIR/bin/_linux_deploy"
PgePrjSD="PGE_Project"
if [ ! -d "$DeployDir" ]; then
	mkdir "$DeployDir"
	mkdir "$DeployDir/$PgePrjSD"
fi

cp pge_editor "$DeployDir/$PgePrjSD"
cp -a *.so* "$DeployDir/$PgePrjSD"
cp -r ./languages "$DeployDir/$PgePrjSD"
cp -r ./themes "$DeployDir/$PgePrjSD"
rm "$DeployDir/$PgePrjSD/themes/*.zip"
if [ ! -d "$DeployDir/$PgePrjSD/help" ]; then
	mkdir "$DeployDir/$PgePrjSD/help"
	echo "<html><head><meta http-equiv=\"refresh\" content=\"0; url=http://help.engine.wohlnet.ru/manual_editor.html\"/></head><body></body></html>" > "$DeployDir/$PgePrjSD/help/manual_editor.htm"
fi
cp "$SOURCEDIR/Editor/changelog.editor.txt" "$DeployDir/$PgePrjSD"
cp "$SOURCEDIR/LICENSE" "$DeployDir/$PgePrjSD/license.txt"

cd $DeployDir
tar -zcvf "./pge-editor-dev-linux-mint.tar.gz" $PgePrjSD

echo -n "Type samba password: "
TempPasswd=""
read TempPasswd

echo "Copying android build..."

cp $SOURCEDIR/Editor/android-build/bin/QtApp-release-signed.apk $SOURCEDIR/Editor/android-build/bin/pge_editor_dev.apk
smbclient $SiteRoot $TempPasswd -u vitaly -c "cd $LabDir; \
lcd $SOURCEDIR/Editor/android-build/bin/; prompt; recurse; mput pge_editor_dev.apk; \
lcd $DeployDir; prompt; recurse; mput pge-editor-dev-linux-mint.tar.gz; exit;"

echo ""
echo ""
echo "All done!"
echo ""
cd $CurDir
read -n 1
