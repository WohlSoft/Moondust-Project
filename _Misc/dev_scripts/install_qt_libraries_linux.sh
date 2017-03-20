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

iamuser=$(whoami)

if [[ "$iamuser" != "root" ]]
then
	echo ""
	echo "Root is needed to copy a tonn of Qt's SO's into /usr/lib/x86_64-linux-gnu/ folder"
	exit 1
fi

# cp -n -a "$QT_PATH"'../lib/'*'.so'* /usr/lib/x86_64-linux-gnu/

ldconf_file=/etc/ld.so.conf.d/libqt5.pge-project.custom.conf

echo "# Custom installed Qt 5, used by PGE Project" >  $ldconf_file
echo ${QT_LIB_PATH} >> $ldconf_file

ldconfig

echo "DONE!"
read -n 1

