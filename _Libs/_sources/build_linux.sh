#!/bin/bash
InstallTo=$(echo ~+/../_builds/linux)
InstallTo=$(readlink -f $InstallTo)
echo $InstallTo
source ./___build_script.sh

printf "\n\n=========BUILT!!===========\n\n"
read -n 1
exit 0
