#!/bin/bash
cd $PWD
echo "#===============================================================================================" > _paths.sh
echo "#=================PLEASE SET UP THE ABLSOLUTE PATHS TO QMAKE AND TO LRELEASE====================" >> _paths.sh
echo "#===============================================================================================" >> _paths.sh
echo "QMAKESPEC=/home/travis/Qt/Qt-5.4_static/mkspecs/linux-g++" >> _paths.sh
echo "QT_INSTALL_PREFIX=/home/travis/Qt/Qt-5.4_static/" >> _paths.sh
echo "QT_INSTALL_DATA=/home/travis/Qt/Qt-5.4_static/" >> _paths.sh
echo "export QMAKESPEC QT_INSTALL_PREFIX QT_INSTALL_DATA" >> _paths.sh
echo "QMake=\"/home/travis/Qt/Qt-5.4_static/bin/qmake\"; #" >> _paths.sh
echo "" >> _paths.sh
echo "LRelease=\"/home/travis/Qt/Qt-5.4_static/bin/lrelease\"; #" >> _paths.sh
echo "#===============================================================================================" >> _paths.sh
echo "#===============================================================================================" >> _paths.sh
echo "#===============================================================================================" >> _paths.sh
echo "" >> _paths.sh
chmod u+x _paths.sh
