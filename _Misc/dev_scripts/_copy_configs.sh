#!/bin/bash
#This file copying a config packs from testing directory into Linux server and into bin directory of dev-pack

CopyConfig()
{
	echo $1...

	rm -r "$SOURCEDIR/bin/configs/$1"
	cp -avr "$TESTDIR/configs/$1" "$SOURCEDIR/bin/configs/$1"
}

source ./_paths.sh

CopyConfig 'PGE Default'
CopyConfig SMBXInt
CopyConfig SMBX
CopyConfig 'Raocow (A2MBXT)'
CopyConfig SMBX_Redrawn

echo 'Done!'

