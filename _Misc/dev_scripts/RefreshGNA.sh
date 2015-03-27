#!/bin/bash
source ./_paths.sh
CurDir=~+

echo "Uploading everything to the GNA"
cd "$SiteRootNFS/$LabDir"
sh $SiteRootNFS/$LabDir/send_to_gna.sh

echo ""
echo ""
echo "All done!"
echo ""
cd $CurDir
read -n 1

