#!/bin/bash
cd $PWD

if [[ "$OSTYPE" == "msys"* ]]; then
   ./generate_paths.bat
   exit 0
fi

MACHINE_TYPE=$(uname -m)
GCC_ARCH=gcc_64
CLANG_ARCH=clang_64
if [[ $MACHINE_TYPE=="x86_64" ]]; then
    GCC_ARCH=gcc_64
    CLANG_ARCH=clang_64
else
    GCC_ARCH=gcc_32
    CLANG_ARCH=clang_32
fi

QMAKE_PATH=qmake
LRELEASE_PATH=lrelease
QT_VERSION=undefined/$GCC_ARCH
QT_PATH=~/Qt/$QT_VERSION/bin/
QT_LIB_PATH=~/Qt/$QT_VERSION/lib/
AUTODETECTED=false

#At first detect globally installed Qt
if [[ $QT_VERSION == "undefined/gcc_64" ]]; then
    if [ -f /usr/bin/qmake ]; then
        QT_VERSION=/usr
        QT_PATH=/usr/bin/
        QT_LIB_PATH=/usr/lib
        AUTODETECTED=true
    fi
    if [ -f /usr/local/bin/qmake ]; then
        QT_VERSION=/usr/local
        QT_PATH=/usr/local/bin/
        QT_LIB_PATH=/usr/lib
        AUTODETECTED=true
    fi
    if [ -f /usr/bin/qmake-qt5 ]; then
        QT_VERSION=/usr
        QT_PATH=/usr/bin/
        QT_LIB_PATH=/usr/local/lib
        QMAKE_PATH=qmake-qt5
        LRELEASE_PATH=lrelease-qt5
        AUTODETECTED=true
    fi
    if [ -f /usr/local/bin/qmake-qt5 ]; then
        QT_VERSION=/usr/local
        QT_PATH=/usr/local/bin/
        QT_LIB_PATH=/usr/local/lib
        QMAKE_PATH=qmake-qt5
        LRELEASE_PATH=lrelease-qt5
        AUTODETECTED=true
    fi
fi

#Automatically detect manually installed Qt
for var in 5.4 5.4.0 5.4.1 5.4.2 5.5 5.5.0 5.5.1 5.6 5.6.0 5.6.1 5.6.1-1 5.7 5.7.1 5.7.2
do
    if [ -f /opt/Qt/$var/$GCC_ARCH/bin/qmake ]; then
        QT_VERSION=$var/$GCC_ARCH
        QT_PATH=~/Qt/$QT_VERSION/bin/
        QT_LIB_PATH=~/Qt/$QT_VERSION/lib/
        AUTODETECTED=true
    fi
    if [ -f ~/Qt/$var/$GCC_ARCH/bin/qmake ]; then
        QT_VERSION=$var/$GCC_ARCH
        QT_PATH=~/Qt/$QT_VERSION/bin/
        QT_LIB_PATH=~/Qt/$QT_VERSION/lib/
        AUTODETECTED=true
    fi
    if [ -f /opt/Qt/$var/$CLANG_ARCH/bin/qmake ]; then
        QT_VERSION=$var/$CLANG_ARCH
        QT_PATH=~/Qt/$QT_VERSION/bin/
        QT_LIB_PATH=~/Qt/$QT_VERSION/lib/
        AUTODETECTED=true
    fi
    if [ -f ~/Qt/$var/$CLANG_ARCH/bin/qmake ]; then
        QT_VERSION=$var/$CLANG_ARCH
        QT_PATH=~/Qt/$QT_VERSION/bin/
        QT_LIB_PATH=~/Qt/$QT_VERSION/lib/
        AUTODETECTED=true
    fi
done

OPEN_GEDIT=true
for var in "$@"
do
    case "$var" in
        silent)
                OPEN_GEDIT=false
                AUTODETECTED=false
            ;;
        semaphore) #Change paths ti Semaphore-CI compatible
                AUTODETECTED=true
                QT_VERSION=5.6.0_static
                QT_PATH=/home/runner/Qt/$QT_VERSION/bin/
                QT_LIB_PATH=/home/runner/Qt/$QT_VERSION/lib/
                QMAKE_PATH=/home/runner/Qt/$QT_VERSION/bin/qmake
                LRELEASE_PATH=/home/runner/Qt/$QT_VERSION/bin/lrelease
            ;;
    esac
done

echo "#===============================================================================================" > _paths.sh
echo "#===========================PLEASE SET UP THE ABLSOLUTE PATHS TO Qt=============================" >> _paths.sh
echo "#===============================================================================================" >> _paths.sh
echo "" >> _paths.sh
echo "#-----------------Path to Qt bin directory-----------------" >> _paths.sh
echo "QT_PATH=\"$QT_PATH\";" >> _paths.sh
echo "" >> _paths.sh
echo "#-----------------Path to Qt lib directory-----------------" >> _paths.sh
echo "QT_LIB_PATH=\"$QT_LIB_PATH\";" >> _paths.sh
echo "" >> _paths.sh
echo "#-----------qmake executable name or full path-------------" >> _paths.sh
echo "QMake=\"$QMAKE_PATH\";" >> _paths.sh
echo "" >> _paths.sh
echo "#---------lrelease executable name or full path------------" >> _paths.sh
echo "LRelease=\"$LRELEASE_PATH\";" >> _paths.sh
echo "" >> _paths.sh
echo "#===============================================================================================" >> _paths.sh
echo "#===============================================================================================" >> _paths.sh
echo "#===============================================================================================" >> _paths.sh
echo "" >> _paths.sh

if $AUTODETECTED ; then
    echo
    echo "==================Detected Qt $QT_VERSION======================="
    if [ -f $QMAKE_PATH ]; then
        $QMAKE_PATH -v
    elif [ -f $QT_PATH$QMAKE_PATH ]; then
        $QT_PATH$QMAKE_PATH -v
    fi
    echo "================================================================"

elif $OPEN_GEDIT ; then
   if [[ ($DISPLAY != "") && ("$OSTYPE" == "linux-gnu" || "$OSTYPE" == "linux") ]]; then
      if [ -f /usr/bin/gedit ]; then
          gedit _paths.sh
      elif [ -f /usr/bin/xed ]; then
          xed _paths.sh
      elif [ -f /usr/bin/pluma ]; then
          pluma _paths.sh
      elif [ -f /usr/bin/kwrite ]; then
          kwrite _paths.sh
      elif [ -f /usr/bin/kate ]; then
          kate _paths.sh
      elif [ -f /usr/bin/leafpad ]; then
          leafpad _paths.sh
      else
          echo "================CAN'T FIND A TEXT EDITOR=================="
          echo "Please check the just generated _paths.sh file in any text editor for corrected paths before execute build scripts!"
          read -n 1
      fi
   else
      if [ -f /usr/bin/mcedit ]; then
          mcedit _paths.sh
      elif [ -f /usr/bin/nano ]; then
          nano _paths.sh
      elif [ -f /usr/bin/pico ]; then
          pico _paths.sh
      elif [ -f /usr/bin/emacs ]; then
          emacs _paths.sh
      else
          echo "================CAN'T FIND A TEXT EDITOR=================="
          echo "Please check the just generated _paths.sh file in any text editor for corrected paths before execute build scripts!"
          read -n 1
      fi
   fi
fi
chmod u+x _paths.sh

