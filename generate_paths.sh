#!/bin/bash
cd $PWD

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
STATIC_QT=false
PREFER_SHARED=false
PREFER_STATIC=false
USE_QT6=false
QT_SUBDIR=qt5

for var in "$@"
do
    case "$var" in
        --shared)
            PREFER_SHARED=true
        ;;
        static)
            PREFER_STATIC=true
        ;;
        --static)
            PREFER_STATIC=true
        ;;
        --qt6)
            USE_QT6=true
            QT_SUBDIR=qt6
        ;;
    esac
done

if $USE_QT6 ; then
    QT_VERSIONS_SEEK=""
    for((i=0;i<15;++i));do
        QT_VERSIONS_SEEK="${QT_VERSIONS_SEEK} 6.$i 6.$i.1 6.$i.2 6.$i.3 6.$i.4 6.$i.5 "
    done
else
    QT_VERSIONS_SEEK=""
    for((i=4;i<=15;++i));do
        QT_VERSIONS_SEEK="${QT_VERSIONS_SEEK} 5.$i"
        for((j=0;j<=25;++j));do
            QT_VERSIONS_SEEK="${QT_VERSIONS_SEEK} 5.$i.$j"
            if [[ "$i" == "6" && "$j" == "1" ]]; then
                QT_VERSIONS_SEEK="${QT_VERSIONS_SEEK} 5.$i.$j-1"
            fi
        done
    done
fi

echo "Seek for Qt: $QT_VERSIONS_SEEK"

if ! $PREFER_STATIC; then
    #At first detect globally installed Qt
    if [[ $QT_VERSION == "undefined/gcc_64" ]]; then
        if [[ -f /usr/bin/qmake ]]; then
            QT_VERSION=/usr
            QT_PATH=/usr/bin/
            QT_LIB_PATH=/usr/lib
            AUTODETECTED=true
        elif [[ -f /usr/local/bin/qmake ]]; then
            QT_VERSION=/usr/local
            QT_PATH=/usr/local/bin/
            QT_LIB_PATH=/usr/lib
            AUTODETECTED=true
        elif [[ -f /usr/bin/qmake-$QT_SUBDIR ]]; then
            QT_VERSION=/usr
            QT_PATH=/usr/bin/
            QT_LIB_PATH=/usr/local/lib
            QMAKE_PATH=qmake-$QT_SUBDIR
            LRELEASE_PATH=lrelease-$QT_SUBDIR
            AUTODETECTED=true
        elif [[ -f /usr/local/bin/qmake-$QT_SUBDIR ]]; then
            QT_VERSION=/usr/local
            QT_PATH=/usr/local/bin/
            QT_LIB_PATH=/usr/local/lib
            QMAKE_PATH=qmake-$QT_SUBDIR
            LRELEASE_PATH=lrelease-$QT_SUBDIR
            AUTODETECTED=true
        fi
    fi

    # Automatically detect manually installed Qt
    for var in $QT_VERSIONS_SEEK
    do
        if [[ "$OSTYPE" == "darwin"* ]]; then
            if [[ -f /usr/local/Cellar/qt@5/$var/bin/qmake ]]; then
                    QT_VERSION=$var
                    QT_PATH=/usr/local/Cellar/qt@5/$QT_VERSION/bin
                    QT_LIB_PATH=/usr/local/Cellar/qt@5/$QT_VERSION/lib
                    AUTODETECTED=true
            fi
        else
            if [[ -f /opt/Qt/$var/$GCC_ARCH/bin/qmake ]]; then
                QT_VERSION=$var/$GCC_ARCH
                QT_PATH=~/opt/Qt/$QT_VERSION/bin
                QT_LIB_PATH=/opt/Qt/$QT_VERSION/lib
                AUTODETECTED=true
            elif [[ -f ~/Qt/$var/$GCC_ARCH/bin/qmake ]]; then
                QT_VERSION=$var/$GCC_ARCH
                QT_PATH=~/Qt/$QT_VERSION/bin
                QT_LIB_PATH=~/Qt/$QT_VERSION/lib
                AUTODETECTED=true
            elif [[ -f /opt/Qt/$var/$CLANG_ARCH/bin/qmake ]]; then
                QT_VERSION=$var/$CLANG_ARCH
                QT_PATH=/opt/Qt/$QT_VERSION/bin
                QT_LIB_PATH=/opt/Qt/$QT_VERSION/lib
                AUTODETECTED=true
            elif [[ -f ~/Qt/$var/$CLANG_ARCH/bin/qmake ]]; then
                QT_VERSION=$var/$CLANG_ARCH
                QT_PATH=~/Qt/$QT_VERSION/bin
                QT_LIB_PATH=~/Qt/$QT_VERSION/lib
                AUTODETECTED=true
            fi
        fi
    done
fi

if [[ $PREFER_SHARED == false && $AUTODETECTED == false && $USE_QT6 == false ]]; then
    for var in $QT_VERSIONS_SEEK
    do
        if [[ -f ~/Qt/${var}_static/bin/qmake ]]; then
            QT_VERSION=${var}_static
            QT_PATH=~/Qt/$QT_VERSION/bin
            QT_LIB_PATH=~/Qt/$QT_VERSION/lib
            AUTODETECTED=true
            STATIC_QT=true
        fi
    done
fi

if [[ "$OSTYPE" == "msys"* ]]; then
    if [[ $PREFER_STATIC == false && -f "${MSYSTEM_PREFIX}/bin/qmake" ]]; then
        QT_VERSION="${MSYSTEM_PREFIX:1}"
        QT_PATH="${MSYSTEM_PREFIX}/bin"
        QT_LIB_PATH="${MSYSTEM_PREFIX}/lib"
        AUTODETECTED=true
    elif [[ $PREFER_SHARED == false && -f "${MSYSTEM_PREFIX}/$QT_SUBDIR-static/bin/qmake" ]]; then
        QT_VERSION="${MSYSTEM_PREFIX:1}/$QT_SUBDIR-static"
        QT_PATH="${MSYSTEM_PREFIX}/$QT_SUBDIR-static/bin"
        QT_LIB_PATH="${MSYSTEM_PREFIX}/$QT_SUBDIR-static/lib"
        AUTODETECTED=true
        STATIC_QT=true
    elif [[ $PREFER_STATIC == false && -f /mingw64/bin/qmake ]]; then
        QT_VERSION=mingw64/$QT_SUBDIR
        QT_PATH=/mingw64/$QT_SUBDIR/bin
        QT_LIB_PATH=/mingw64/$QT_SUBDIR/lib
        AUTODETECTED=true
    elif [[ $PREFER_SHARED == false && -f /mingw64/$QT_SUBDIR-static/bin/qmake ]]; then
        QT_VERSION=mingw64/$QT_SUBDIR-static
        QT_PATH=/mingw64/$QT_SUBDIR-static/bin
        QT_LIB_PATH=/mingw64/$QT_SUBDIR-static/lib
        AUTODETECTED=true
        STATIC_QT=true
    elif [[ $PREFER_STATIC == false && -f /mingw32/bin/qmake ]]; then
        QT_VERSION=mingw32/$QT_SUBDIR
        QT_PATH=/mingw32/$QT_SUBDIR/bin
        QT_LIB_PATH=/mingw32/$QT_SUBDIR/lib
        AUTODETECTED=true
    elif [[ $PREFER_SHARED == false && -f /mingw32/$QT_SUBDIR-static/bin/qmake ]]; then
        QT_VERSION=mingw32/$QT_SUBDIR-static
        QT_PATH=/mingw32/$QT_SUBDIR-static/bin
        QT_LIB_PATH=/mingw32/$QT_SUBDIR-static/lib
        AUTODETECTED=true
        STATIC_QT=true
    fi
fi

OPEN_GEDIT=true
for var in "$@"
do
    case "$var" in
        silent)
            OPEN_GEDIT=false
            AUTODETECTED=false
        ;;
    esac
done

echo "#===============================================================================================" > _paths.sh
echo "#===========================PLEASE SET UP THE ABLSOLUTE PATHS TO Qt=============================" >> _paths.sh
echo "#===============================================================================================" >> _paths.sh
echo "#-----------------Uncomment this if you want use Qt 6!-----------------" >> _paths.sh
if $USE_QT6; then
    echo "USE_QT6=true" >> _paths.sh
else
    echo "#USE_QT6=true" >> _paths.sh
fi
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
if $STATIC_QT; then
    echo "#---------Force using of Static Qt build mode------------" >> _paths.sh
    echo "flag_cmake_static_qt=true;" >> _paths.sh
    echo "" >> _paths.sh
fi
if ! $AUTODETECTED ; then
    echo "#---------Define this flag if you use a Static Qt build------------" >> _paths.sh
    echo "#flag_cmake_static_qt=true;" >> _paths.sh
    echo "" >> _paths.sh
fi
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
    if $STATIC_QT; then
        echo " -- DETECTED STATIC BUILD --"
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
      elif [ -f /bin/nano ]; then
          nano _paths.sh
      else
          echo "================CAN'T FIND A TEXT EDITOR=================="
          echo "Please check the just generated _paths.sh file in any text editor for corrected paths before execute build scripts!"
          read -n 1
      fi
   fi
fi
chmod u+x _paths.sh
