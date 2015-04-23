#!/bin/bash
InitDir=~+
cd ..
source ./_paths.sh
CurDir=~+

echo "Copying data..."
if [ -f "$SOURCEDIR/Editor/android-build/bin/QtApp-release-signed.apk" ]; then
	cp "$SOURCEDIR/Editor/android-build/bin/QtApp-release-signed.apk" "$SiteRootNFS/$LabDir/pge_editor_dev.apk"
fi

if [ -f "$SOURCEDIR/bin/_packed/*.tar.gz" ]; then
	cp "$SOURCEDIR/bin/_packed/*.tar.gz" "$SiteRootNFS/$LabDir"
fi

if [ -f "$SiteRootNFS/$LabDir/send_to_gna.sh" ]; then
	echo "Uploading everything to the GNA"
	cd "$SiteRootNFS/$LabDir"
	sh $SiteRootNFS/$LabDir/send_to_gna.sh
fi

echo ""
echo "All done!"
echo ""
cd $InitDir
read -n 1
