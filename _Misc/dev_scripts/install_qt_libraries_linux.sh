#!/bin/bash

SCRDIR=$PWD/../../
if [ -f "$SCRDIR/_paths.sh" ]
then
	source "$SCRDIR/_paths.sh"
else
	echo ""
	echo "_paths.sh is not exist! Run \"generate_paths.sh\" first!"
	exit 1
fi

if [[ $(whoami) != "root" ]]
then
	echo ""
	echo "Root is needed to copy a tonn of Qt's SO's into /usr/lib/x86_64-linux-gnu/ folder"
	exit 1
fi

cp -a "$QT_PATH"'../lib/'*'.so'* /usr/lib/x86_64-linux-gnu/
echo "DONE!"
read -n 1

