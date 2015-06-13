#!/bin/bash
cd $PWD
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
gedit _paths.sh
chmod u+x _paths.sh

