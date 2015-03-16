#!/bin/bash
source ./_paths.sh

echo -n "Type samba password: "
TempPasswd=""
read TempPasswd

echo "Copying android build..."

cp $SOURCEDIR/Editor/android-build/bin/QtApp-release-signed.apk $SOURCEDIR/Editor/android-build/bin/pge_editor_dev.apk
smbclient $SiteRoot $TempPasswd -u vitaly -c "cd $LabDir; lcd $SOURCEDIR/Editor/android-build/bin/; prompt; recurse; mput pge_editor_dev.apk; exit;"

echo ""
echo ""
echo "All done!"
echo ""

read -n 1
