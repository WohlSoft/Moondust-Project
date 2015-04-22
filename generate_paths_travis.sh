#!/bin/bash
cd $PWD
echo "#===============================================================================================" > _paths.sh
echo "#=================PLEASE SET UP THE ABLSOLUTE PATHS TO QMAKE AND TO LRELEASE====================" >> _paths.sh
echo "#===============================================================================================" >> _paths.sh
#    QMAKE_SPEC - the shortname of the host mkspec that is resolved and stored in the QMAKESPEC variable during a host build
echo "QMAKESPEC=/home/travis/Qt/Qt-5.4_static/mkspecs/linux-g++" >> _paths.sh
#    QT_INSTALL_PREFIX - default prefix for all paths
echo "QT_INSTALL_PREFIX=/home/travis/Qt/Qt-5.4_static/" >> _paths.sh
#    QT_INSTALL_DATA - location of general architecture-independent Qt data
echo "QT_INSTALL_DATA=/home/travis/Qt/Qt-5.4_static/" >> _paths.sh
#    QT_INSTALL_BINS - location of Qt binaries (tools and applications)
echo "QT_INSTALL_BINS=/home/travis/Qt/Qt-5.4_static/bin" >> _paths.sh
echo "QT_HOST_BINS=/home/travis/Qt/Qt-5.4_static/bin" >> _paths.sh
echo "QT_HOST_DATA=/home/travis/Qt/Qt-5.4_static" >> _paths.sh

#    QT_INSTALL_CONFIGURATION - location for Qt settings. Not applicable on Windows
echo "QT_INSTALL_CONFIGURATION=/home/travis/Qt/Qt-5.4_static/bin" >> _paths.sh
#    QMAKE_VERSION - the current version of qmake
#    QMAKE_XSPEC - the shortname of the target mkspec that is resolved and stored in the QMAKESPEC variable during a target build
#    QT_INSTALL_ARCHDATA - location of general architecture-dependent Qt data
#    QT_INSTALL_DOCS - location of documentation
echo "QT_INSTALL_DOCS=/home/travis/Qt/Qt-5.4_static/doc" >> _paths.sh
#    QT_INSTALL_EXAMPLES - location of examples
#    QT_INSTALL_HEADERS - location for all header files
echo "QT_INSTALL_HEADERS=/home/travis/Qt/Qt-5.4_static/include" >> _paths.sh
#    QT_INSTALL_IMPORTS - location of QML 1.x extensions
echo "QT_INSTALL_IMPORTS=/home/travis/Qt/Qt-5.4_static/imports" >> _paths.sh
#    QT_INSTALL_LIBEXECS - location of executables required by libraries at runtime
echo "QT_INSTALL_LIBEXECS=/home/travis/Qt/Qt-5.4_static/lib/pkgconfig" >> _paths.sh
#    QT_INSTALL_LIBS - location of libraries
echo "QT_INSTALL_LIBS=/home/travis/Qt/Qt-5.4_static/lib" >> _paths.sh
#    QT_INSTALL_PLUGINS - location of Qt plugins
echo "QT_INSTALL_PLUGINS=/home/travis/Qt/Qt-5.4_static/plugins" >> _paths.sh
#    QT_INSTALL_QML - location of QML 2.x extensions
echo "QT_INSTALL_QML=/home/travis/Qt/Qt-5.4_static/qml" >> _paths.sh
#    QT_INSTALL_TESTS - location of Qt test cases
#    QT_INSTALL_TRANSLATIONS - location of translation information for Qt strings
echo "QT_INSTALL_TRANSLATIONS=/home/travis/Qt/Qt-5.4_static/translations" >> _paths.sh
#    QT_SYSROOT - the sysroot used by the target build environment
echo "QT_SYSROOT=/home/travis/" >> _paths.sh
#    QT_VERSION - the Qt version. We recommend that you query Qt module specific version numbers by using $$QT.<module>.version variables instead.
echo "QT_VERSION=5.4.1" >> _paths.sh

echo "export QMAKESPEC QT_INSTALL_PREFIX QT_INSTALL_DATA" >> _paths.sh
echo "QMake=\"/home/travis/Qt/Qt-5.4_static/bin/qmake\"; #" >> _paths.sh
echo "" >> _paths.sh
echo "LRelease=\"/home/travis/Qt/Qt-5.4_static/bin/lrelease\"; #" >> _paths.sh
echo "#===============================================================================================" >> _paths.sh
echo "#===============================================================================================" >> _paths.sh
echo "#===============================================================================================" >> _paths.sh
echo "" >> _paths.sh
chmod u+x _paths.sh
