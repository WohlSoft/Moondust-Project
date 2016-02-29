#!/bin/bash
InitDir=~+
cd ..
source ./_paths.sh
CurDir=~+
if [ -d "$SiteRootNFS/help" ]; then
    echo "Syncing with laboratory..."
	rsync -raz --progress "$SOURCEDIR/Content/help/" "$SiteRootNFS/help" > /dev/null
    echo "Syncing with wohlsoft.ru..."

    login=$(cat ~/.wohlsoft.login)
    password=$(cat ~/.wohlsoft.password)
    host=$(cat ~/.wohlsoft.host)
    path=$(cat ~/.wohlsoft.path)
    sshpass -p "$password" rsync -raz --progress "$SOURCEDIR/Content/help/" $login@$login.$host:$path > /dev/null

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

