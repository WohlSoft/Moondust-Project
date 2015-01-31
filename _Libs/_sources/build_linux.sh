#!/bin/bash
InstallTo=$(echo ~+/../_builds/linux)
InstallTo=$(readlink -f $InstallTo)
echo $InstallTo
source ./___build_script.sh
