#!/bin/bash
InitDir=~+
#=============Detect directory that contains script=====================
SOURCE="${BASH_SOURCE[0]}"
while [ -h "$SOURCE" ]; do # resolve $SOURCE until the file is no longer a symlink
  CurDir="$( cd -P "$( dirname "$SOURCE" )" && pwd )"
  SOURCE="$(readlink "$SOURCE")"
  [[ $SOURCE != /* ]] && SOURCE="$DIR/$SOURCE" # if $SOURCE was a relative symlink, we need to resolve it relative to the path where the symlink file was located
done
CurDir="$( cd -P "$( dirname "$SOURCE" )" && pwd )"
#=======================================================================
cd ..
CurDir=$CurDir/../
source ./_paths.sh

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
