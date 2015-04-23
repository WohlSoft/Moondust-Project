#!/bin/bash
source ./_paths.sh
CurDir=~+

echo "Copying data..."
if [ "$1" != "no-android" ]; then
	cp $SOURCEDIR/Editor/android-build/bin/QtApp-release-signed.apk $SiteRootNFS/$LabDir/pge_editor_dev.apk
fi
cp $SOURCEDIR/bin/_packed/pge-project-dev-linux-mint.tar.gz $SiteRootNFS/$LabDir/pge-project-dev-linux-mint.tar.gz

echo "Uploading everything to the GNA"
cd "$SiteRootNFS/$LabDir"
sh $SiteRootNFS/$LabDir/send_to_gna.sh

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
echo "All done!"
echo ""
cd $CurDir
read -n 1
