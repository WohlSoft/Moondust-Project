#!/bin/bash
InitDir=~+
cd ..
source ./_paths.sh
CurDir=~+
if [ -d "$SiteRootNFS/$LabDir/../_sources/devel/Content/help" ]; then
echo "Syncing with laboratory..."
	rsync -raz --progress "$SOURCEDIR/Content/help/" "$SiteRootNFS/$LabDir/../_sources/devel/Content/help"
fi
if [ -d "$SiteRootNFS/$LabDir" ]; then
echo "Packing zip into laboratory..."
	cd "$SOURCEDIR/Content"
	rm "$SiteRootNFS/$LabDir/pge_help_standalone.zip"
	zip -r "$SiteRootNFS/$LabDir/pge_help_standalone.zip" "help"
fi

echo ""
echo "All done!"
echo ""
cd $InitDir
read -n 1
