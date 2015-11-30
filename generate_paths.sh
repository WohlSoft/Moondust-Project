#!/bin/bash
cd $PWD

if [[ "$OSTYPE" == "msys"* ]]; then
   ./generate_paths.bat
   exit 0
fi

echo "#===============================================================================================" > _paths.sh
echo "#=================PLEASE SET UP THE ABLSOLUTE PATHS TO QMAKE AND TO LRELEASE====================" >> _paths.sh
echo "#===============================================================================================" >> _paths.sh
echo "QMake=\"qmake\"; #" >> _paths.sh
echo "#QMake=\"qmake-qt5\"; # for CentOS" >> _paths.sh
echo "" >> _paths.sh
echo "LRelease=\"lrelease\"; #" >> _paths.sh
echo "#LRelease=\"lrelease-qt5\"; # for CentOS" >> _paths.sh
echo "#===============================================================================================" >> _paths.sh
echo "#===============================================================================================" >> _paths.sh
echo "#===============================================================================================" >> _paths.sh
echo "" >> _paths.sh

if [[ "$1"!="silent" ]]; then
   if [[ "$OSTYPE" == "linux-gnu" ]]; then
      gedit _paths.sh
   else
      nano _paths.sh
   fi
fi
chmod u+x _paths.sh
