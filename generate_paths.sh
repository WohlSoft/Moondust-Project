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
    esac
done


if ! $PREFER_STATIC; then
    #At first detect globally installed Qt
    if [[ $QT_VERSION == "undefined/gcc_64" ]]; then
        if [ -f /usr/bin/qmake ]; then
            QT_VERSION=/usr
            QT_PATH=/usr/bin/
            QT_LIB_PATH=/usr/lib
            AUTODETECTED=true
        elif [ -f /usr/local/bin/qmake ]; then
            QT_VERSION=/usr/local
            QT_PATH=/usr/local/bin/
            QT_LIB_PATH=/usr/lib
            AUTODETECTED=true
        elif [ -f /usr/bin/qmake-qt5 ]; then
            QT_VERSION=/usr
            QT_PATH=/usr/bin/
            QT_LIB_PATH=/usr/local/lib
            QMAKE_PATH=qmake-qt5
            LRELEASE_PATH=lrelease-qt5
            AUTODETECTED=true
        elif [ -f /usr/local/bin/qmake-qt5 ]; then
            QT_VERSION=/usr/local
            QT_PATH=/usr/local/bin/
            QT_LIB_PATH=/usr/local/lib
            QMAKE_PATH=qmake-qt5
            LRELEASE_PATH=lrelease-qt5
            AUTODETECTED=true
        fi
    fi

    # Automatically detect manually installed Qt
    for var in 5.4 5.4.0 5.4.1 5.4.2 \
               5.5 5.5.0 5.5.1 \
               5.6 5.6.0 5.6.1 5.6.1-1 \
               5.7 5.7.1 5.7.2 \
               5.8 5.8.1 5.8.2 5.8.3 \
               5.9 5.9.1 5.9.2 5.9.3 \
               5.10 5.10.1 5.10.2 5.10.3 \
               5.11 5.11.0 5.11.1 5.11.2 5.11.3 \
               5.12 5.12.0 5.12.1 5.12.2 5.12.3 5.12.4 5.12.5 \
               5.13 5.13.0 5.13.1 5.13.2 5.13.3 5.13.4 \
               5.14 5.14.0 5.14.1 5.14.2 5.14.3 5.14.4 5.14.5 \
               5.15 5.15.0 5.15.1 5.15.2 5.15.3 5.15.4 5.15.5 5.15.6
    do
        if [[ -f /opt/Qt/$var/$GCC_ARCH/bin/qmake ]]; then
            QT_VERSION=$var/$GCC_ARCH
            QT_PATH=~/Qt/$QT_VERSION/bin/
            QT_LIB_PATH=~/Qt/$QT_VERSION/lib/
            AUTODETECTED=true
        elif [[ -f ~/Qt/$var/$GCC_ARCH/bin/qmake ]]; then
            QT_VERSION=$var/$GCC_ARCH
            QT_PATH=~/Qt/$QT_VERSION/bin/
            QT_LIB_PATH=~/Qt/$QT_VERSION/lib/
            AUTODETECTED=true
        elif [[ -f /opt/Qt/$var/$CLANG_ARCH/bin/qmake ]]; then
            QT_VERSION=$var/$CLANG_ARCH
            QT_PATH=~/Qt/$QT_VERSION/bin/
            QT_LIB_PATH=~/Qt/$QT_VERSION/lib/
            AUTODETECTED=true
        elif [[ -f ~/Qt/$var/$CLANG_ARCH/bin/qmake ]]; then
            QT_VERSION=$var/$CLANG_ARCH
            QT_PATH=~/Qt/$QT_VERSION/bin/
            QT_LIB_PATH=~/Qt/$QT_VERSION/lib/
            AUTODETECTED=true
        fi
    done
fi

if [[ $PREFER_SHARED == false && $AUTODETECTED == false ]]; then
    for var in 5.4 5.4.0 5.4.1 5.4.2 \
               5.5 5.5.0 5.5.1 \
               5.6 5.6.0 5.6.1 5.6.1-1 \
               5.7 5.7.1 5.7.2 \
               5.8 5.8.1 5.8.2 5.8.3 \
               5.9 5.9.1 5.9.2 5.9.3 \
               5.10 5.10.1 5.10.2 5.10.3 \
               5.11 5.11.0 5.11.1 5.11.2 5.11.3 \
               5.12 5.12.0 5.12.1 5.12.2 5.12.3 5.12.4 5.12.5 \
               5.13 5.13.0 5.13.1 5.13.2 5.13.3 5.13.4 \
               5.14 5.14.0 5.14.1 5.14.2 5.14.3 5.14.4 5.14.5 \
               5.15 5.15.0 5.15.1 5.15.2 5.15.3 5.15.4 5.15.5 5.15.6
    do
        if [[ -f ~/Qt/${var}_static/bin/qmake ]]; then
            QT_VERSION=${var}_static
            QT_PATH=~/Qt/$QT_VERSION/bin/
            QT_LIB_PATH=~/Qt/$QT_VERSION/lib/
            AUTODETECTED=true
            STATIC_QT=true
        fi
    done
fi

if [[ "$OSTYPE" == "msys"* ]]; then
    if [[ $PREFER_STATIC == false && -f "${MSYSTEM_PREFIX}/bin/qmake" ]]; then
        QT_VERSION="${MSYSTEM_PREFIX:1}/"
        QT_PATH="${MSYSTEM_PREFIX}/bin/"
        QT_LIB_PATH="${MSYSTEM_PREFIX}/lib/"
        AUTODETECTED=true
    elif [[ $PREFER_SHARED == false && -f "${MSYSTEM_PREFIX}/qt5-static/bin/qmake" ]]; then
        QT_VERSION="${MSYSTEM_PREFIX:1}/qt5-static"
        QT_PATH="${MSYSTEM_PREFIX}/qt5-static/bin/"
        QT_LIB_PATH="${MSYSTEM_PREFIX}/qt5-static/lib/"
        AUTODETECTED=true
        STATIC_QT=true
    elif [[ $PREFER_STATIC == false && -f /mingw64/bin/qmake ]]; then
        QT_VERSION=mingw64/qt5
        QT_PATH=/mingw64/qt5/bin/
        QT_LIB_PATH=/mingw64/qt5/lib/
        AUTODETECTED=true
    elif [[ $PREFER_SHARED == false && -f /mingw64/qt5-static/bin/qmake ]]; then
        QT_VERSION=mingw64/qt5-static
        QT_PATH=/mingw64/qt5-static/bin/
        QT_LIB_PATH=/mingw64/qt5-static/lib/
        AUTODETECTED=true
        STATIC_QT=true
    elif [[ $PREFER_STATIC == false && -f /mingw32/bin/qmake ]]; then
        QT_VERSION=mingw32/qt5
        QT_PATH=/mingw32/qt5/bin/
        QT_LIB_PATH=/mingw32/qt5/lib/
        AUTODETECTED=true
    elif [[ $PREFER_SHARED == false && -f /mingw32/qt5-static/bin/qmake ]]; then
        QT_VERSION=mingw32/qt5-static
        QT_PATH=/mingw32/qt5-static/bin/
        QT_LIB_PATH=/mingw32/qt5-static/lib/
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
