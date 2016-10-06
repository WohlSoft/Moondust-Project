#!/bin/bash

if [ $TRAVIS_OS_NAME == linux ];
then
    if [ -d /home/runner/PGE-Project/bin/_packed ]; 
    then
        cd /home/runner/PGE-Project/Content/configs
        zip -9 -r /home/runner/PGE-Project/bin/_packed/SMBX-Config-Patch.zip SMBX
    fi

    if [ -d /home/runner/PGE-Project/bin/_packed ]; 
    then 
        lftp -e \"put -O ./ubuntu-14-04/ /home/runner/PGE-Project/bin/_packed/pge-project-dev-linux-mint.tar.gz; put -O ./_common/ /home/runner/PGE-Project/bin/_packed/SMBX-Config-Patch.zip; exit\" -u $FTP_USER,$FTP_PASSWORD $FTP_SERVER
        lftp -e \"put -O ./_versions/ /home/runner/build_date_dev_linux.txt; put -O ./_versions/ /home/runner/PGE-Project/bin/versions/editor.txt; put -O ./_versions/ /home/runner/PGE-Project/bin/versions/editor_stable.txt; put -O ./_versions/ /home/runner/PGE-Project/bin/versions/engine.txt; put -O ./_versions/ /home/runner/PGE-Project/bin/versions/engine_stable.txt; exit\" -u $FTP_USER,$FTP_PASSWORD $FTP_SERVER
    fi

elif [ $TRAVIS_OS_NAME == osx ];
then
# ==============================================================================
# Upload created DMG file to the server
# ==============================================================================
    lftp -e "put -O ./macosx/ ./bin/_packed/pge-project-dev-macosx.dmg; put -O ./_versions/ /Users/travis/build_date_dev_osx.txt; exit" -u $FTPUser,$FTPPassword $FTPServer

fi

