#!/bin/bash
cd $PWD

if [[ "$OSTYPE" == "msys"* ]]; then
   ./generate_paths.bat
   exit 0
fi

QMAKE_PATH=qmake
LRELEASE_PATH=lrelease
QT_VERSION=5.5.1_static

OPEN_GEDIT=true
for var in "$@"
do
    case "$var" in
        silent)
                OPEN_GEDIT=false
            ;;
        semaphore) #Change paths ti Semaphore-CI compatible
                QMAKE_PATH=/home/runner/Qt/$QT_VERSION/bin/qmake
                LRELEASE_PATH=/home/runner/Qt/$QT_VERSION/bin/lrelease
            ;;
    esac
done

echo "#===============================================================================================" > _paths.sh
echo "#=================PLEASE SET UP THE ABLSOLUTE PATHS TO QMAKE AND TO LRELEASE====================" >> _paths.sh
echo "#===============================================================================================" >> _paths.sh
echo "QMake=\"$QMAKE_PATH\"; #" >> _paths.sh
echo "#QMake=\"qmake-qt5\"; # for CentOS" >> _paths.sh
echo "" >> _paths.sh
echo "LRelease=\"$LRELEASE_PATH\"; #" >> _paths.sh
echo "#LRelease=\"lrelease-qt5\"; # for CentOS" >> _paths.sh
echo "#===============================================================================================" >> _paths.sh
echo "#===============================================================================================" >> _paths.sh
echo "#===============================================================================================" >> _paths.sh
echo "" >> _paths.sh

if $OPEN_GEDIT ; then
   if [[ "$OSTYPE" == "linux-gnu" ]]; then
      gedit _paths.sh
   else
      nano _paths.sh
   fi
fi
chmod u+x _paths.sh

