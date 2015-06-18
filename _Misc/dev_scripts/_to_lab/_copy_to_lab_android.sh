#!/bin/bash
InitDir=~+
cd ..
source ./_paths.sh
CurDir=~+

echo "Copying data..."
if [ -f "$SOURCEDIR/Editor/android-build/bin/QtApp-release-signed.apk" ]; then
	echo "copy $SOURCEDIR/Editor/android-build/bin/QtApp-release-signed.apk..."
	cp "$SOURCEDIR/Editor/android-build/bin/QtApp-release-signed.apk" "$SiteRootNFS/$LabDir/pge_editor_dev.apk"
fi
if [ -f "$SOURCEDIR/android-build/bin/QtApp-release-signed.apk" ]; then
	echo "copy $SOURCEDIR/android-build/bin/QtApp-release-signed.apk..."
	cp "$SOURCEDIR/android-build/bin/QtApp-release-signed.apk" "$SiteRootNFS/$LabDir/pge_editor_dev.apk"
fi
echo "Done!"
cd $InitDir
read -n 1
