#!/bin/bash
InitDir=~+
cd ..
source ./_paths.sh
CurDir=~+

echo "Copying data..."
if [ -f "$SOURCEDIR/bin/_packed/pge-project-dev-linux-mint.tar.gz" ]; then
	echo "copy $SOURCEDIR/bin/_packed/pge-project-dev-linux-mint.tar.gz..."
	cp "$SOURCEDIR/bin/_packed/pge-project-dev-linux-mint.tar.gz" "$SiteRootNFS/$LabDir"
fi
echo "Done!"

if [ -f "$SiteRootNFS/$LabDir/send_to_gna.sh" ]; then
        echo "Updating config index..."
        cd "$SiteRootNFS/$LabDir/../_configs"
        php .update_index_time.php
	echo "Uploading everything to the GNA"
	cd "$SiteRootNFS/$LabDir"
	sh $SiteRootNFS/$LabDir/send_to_gna.sh
fi

echo ""
echo "All done!"
echo ""
cd $InitDir
read -n 1
