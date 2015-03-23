#!/bin/bash
InstallTo=$(echo ~+/../_builds/android)
InstallTo=$(readlink -f $InstallTo)

#PATH="/usr/src/android-ndk-r9d:$PATH"          # for 'ndk-build'
PATH="/home/vitaly/_git_repos/android-sdk-linux/tools:$PATH"  # for 'android'
PATH="/home/vitaly/_git_repos/android-ndk-r10d:$PATH"

echo $InstallTo
source ./___build_script_android.sh

printf "\n\n=========BUILT!!===========\n\n"
read -n 1
exit 0
