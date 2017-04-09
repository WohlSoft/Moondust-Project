#/bin/bash
bak=~+

if [[ "$OSTYPE" == "msys"* ]]; then
    ./build.bat
    exit 0
fi

#flags
flag_pause_on_end=true
QMAKE_EXTRA_ARGS=""
MAKE_EXTRA_ARGS="-r -j 4"
flag_debugThisScript=false

for var in "$@"
do
    case "$var" in
        --help)
            echo ""
            echo "=== Build script for PGE Project ==="
            echo ""
            echo "SYNTAX:"
            echo ""
            echo "    $0 [<arg1>] [<arg2>] [<arg2>] ..."
            echo ""
            echo "AVAILABLE ARGUMENTS:"
            echo ""

            echo "--- Actions ---"
            echo " lupdate          - Update the translations"
            echo " clean            - Remove all object files and caches to build from scratch"
            echo " isvalid          - Show validation state of dependencies"
            echo ""

            echo "--- Flags ---"
            echo " no-pause         - Don't pause script on completion'"
            echo " silent-make      - Don't print build commands for each building file"
            printf " use-ccache       - Use the CCache to speed-up build process"
            if [[ ! -f /usr/bin/ccache && ! -f /bin/ccache && ! -f /usr/local/bin/ccache ]]; then
                printf " \E[0;41;37m<CCache is NOT INSTALLED!>\E[0m"
            fi
            printf "\n"
            echo ""

            echo "--- Disable building of components ---"
            echo " noeditor         - Skip building of PGE Editor compoment"
            echo " noengine         - Skip building of PGE Engine compoment"
            echo " nocalibrator     - Skip building of Playable Character Calibrator compoment"
            echo " nomaintainer     - Skip building of PGE Maintainer compoment"
            echo " nomanager        - Skip building of PGE Manager compoment"
            echo " nogifs2png       - Skip building of GIFs2PNG compoment"
            echo " nopng2gifs       - Skip building of PNG2GIFs compoment"
            echo " nolazyfixtool    - Skip building of LazyFixTool compoment"
            echo ""

            echo "--- Special ---"
            echo " debugscript      - Show some extra information to debug this script"
            echo ""

            echo "--- For fun ---"
            echo " colors           - Prints various blocks of different color with showing their codes"
            echo " cool             - Prints some strings inside the lines (test of printLine command)"
            echo ""

            echo "==== IMPORTANT! ===="
            echo "This script is designed for Linux and macOS operating systems."
            echo "If you trying to start it under Windows, it will automatically start"
            echo "the build.bat script instead of this."
            echo "===================="
            echo ""
            exit 1
            ;;
        no-pause)
                flag_pause_on_end=false
            ;;
        silent-make)
                MAKE_EXTRA_ARGS="${MAKE_EXTRA_ARGS} -s"
            ;;
        colors)
            for((i=0;i<=1;i++))
            do
                printf "="
                for((j=0;j<=7;j++))
                do
                    printf "\E[${i};3${j};4${j}m"
                    printf "[${i};3${j};4${j}]"
                    printf "\E[0;00m "
                done
                printf "=\n"
            done
            exit 0
            ;;
        use-ccache)
                if [[ "$OSTYPE" == "linux-gnu" ]]; then
                    QMAKE_EXTRA_ARGS="$QMAKE_EXTRA_ARGS -spec linux-g++ CONFIG+=useccache"
                else
                    QMAKE_EXTRA_ARGS="$QMAKE_EXTRA_ARGS CONFIG+=useccache"
                fi
            ;;
        clean)
                echo "======== Remove all cached object files and automatically generated Makefiles ========"
                if [[ "$OSTYPE" == "msys"* ]]; then
                    ./clean_make.bat nopause
                    BinDir=bin-w32
                else
                    ./clean_make.sh nopause
                    BinDir=bin
                fi

                if [ -d ./$BinDir/_build_x32 ]; then
                    echo "removing $BinDir/_build_x32 ..."
                    rm -Rf ./$BinDir/_build_x32
                fi

                if [ -d ./$BinDir/_build_x64 ]; then
                    echo "removing $BinDir/_build_x64 ..."
                    rm -Rf ./$BinDir/_build_x64
                fi

                echo 'removing Dependencies build cache ...'

                ./clear_deps.sh
                echo "==== Clear! ===="
                exit 0;
            ;;

        # Enable debuggin of this script by showing states of inernal variables with pauses
        debugscript)
            flag_debugThisScript=true
            ;;

        # Disable building of some compnents
        noeditor)
            QMAKE_EXTRA_ARGS="${QMAKE_EXTRA_ARGS} CONFIG+=${var}"
            ;;
        noengine)
            QMAKE_EXTRA_ARGS="${QMAKE_EXTRA_ARGS} CONFIG+=${var}"
            ;;
        nocalibrator)
            QMAKE_EXTRA_ARGS="${QMAKE_EXTRA_ARGS} CONFIG+=${var}"
            ;;
        nogifs2png)
            QMAKE_EXTRA_ARGS="${QMAKE_EXTRA_ARGS} CONFIG+=${var}"
            ;;
        nopng2gifs)
            QMAKE_EXTRA_ARGS="${QMAKE_EXTRA_ARGS} CONFIG+=${var}"
            ;;
        nolazyfixtool)
            QMAKE_EXTRA_ARGS="${QMAKE_EXTRA_ARGS} CONFIG+=${var}"
            ;;
        nomanager)
            QMAKE_EXTRA_ARGS="${QMAKE_EXTRA_ARGS} CONFIG+=${var}"
            ;;
        nomaintainer)
            QMAKE_EXTRA_ARGS="${QMAKE_EXTRA_ARGS} CONFIG+=${var}"
            ;;
    esac
done

#=============Detect directory that contains script=====================
SOURCE="${BASH_SOURCE[0]}"
while [ -h "$SOURCE" ]; do # resolve $SOURCE until the file is no longer a symlink
    SCRDIR="$( cd -P "$( dirname "$SOURCE" )" && pwd )"
    SOURCE="$(readlink "$SOURCE")"
    [[ $SOURCE != /* ]] && SOURCE="$DIR/$SOURCE" # if $SOURCE was a relative symlink, we need to resolve it relative to the path where the symlink file was located
done
SCRDIR="$( cd -P "$( dirname "$SOURCE" )" && pwd )"
#=======================================================================
echo $SCRDIR
cd $SCRDIR
source ./_common/functions.sh
#=======================================================================

if [ -f "$SCRDIR/_paths.sh" ]
then
	source "$SCRDIR/_paths.sh"
else
	echo ""
	echo "_paths.sh is not exist! Run \"generate_paths.sh\" first!"
	errorofbuild
fi

PATH=$QT_PATH:$PATH
LD_LIBRARY_PATH=$QT_LIB_PATH:$LD_LIBRARY_PATH

if $flag_debugThisScript; then
    echo "QMAKE_EXTRA_ARGS = ${QMAKE_EXTRA_ARGS}"
    echo "MAKE_EXTRA_ARGS = ${MAKE_EXTRA_ARGS}"
    pause
fi

checkForDependencies()
{
    libPref="lib"
    dlibExt="so"
    slibExt="a"
    osDir="linux_default"
    if [[ "$OSTYPE" == "darwin"* ]]; then
        dlibExt="dylib"
        slibExt="a"
        libPref="lib"
        osDir="macos"
    elif [[ "$OSTYPE" == "linux-gnu" || "$OSTYPE" == "linux" ]]; then
        osDir="linux"
        libPref="lib"
    elif [[ "$OSTYPE" == "freebsd"* ]]; then
        osDir="freebsd"
        libPref="lib"
    elif [[ "$OSTYPE" == "msys"* ]]; then
        dlibExt="a"
        osDir="win32"
        libPref="lib"
    fi
    libsDir=$SCRDIR/_Libs/_builds/$osDir/

    HEADS="SDL2/SDL.h"
    HEADS="${HEADS} SDL2/SDL_mixer_ext.h"
    HEADS="${HEADS} FreeImageLite.h"
    HEADS="${HEADS} mad.h"
    HEADS="${HEADS} sqlite3.h"
    HEADS="${HEADS} freetype2/ft2build.h"
    HEADS="${HEADS} FLAC/all.h"
    HEADS="${HEADS} luajit-2.1/lua.h"
    HEADS="${HEADS} luajit-2.1/luajit.h"
    HEADS="${HEADS} ogg/ogg.h"
    HEADS="${HEADS} vorbis/codec.h"
    HEADS="${HEADS} vorbis/vorbisfile.h"
    HEADS="${HEADS} vorbis/vorbisenc.h"
    for head in $HEADS
    do
        if [[ $1 == "test" ]]
        then
            echo "Checking include ${head}..."
        fi
        if [[ ! -f ${libsDir}/include/${head} ]]
        then
            lackOfDependency
        fi
    done

    DEPS="FLAC.$slibExt"
    DEPS="${DEPS} ogg.$slibExt"
    DEPS="${DEPS} vorbis.$slibExt"
    DEPS="${DEPS} vorbisfile.$slibExt"
    DEPS="${DEPS} mad.$slibExt"
    DEPS="${DEPS} SDL2.$slibExt"
    DEPS="${DEPS} freetype.$slibExt"
    DEPS="${DEPS} sqlite3.$slibExt"
    for lib in $DEPS
    do
        if [[ $1 == "test" ]]
        then
            echo "Checking library ${libPref}${lib}..."
        fi
        if [[ ! -f ${libsDir}/lib/${libPref}${lib} ]]
        then
            lackOfDependency
        fi
    done
}

# Check input arguments again
for var in "$@"
do
    case "$var" in
        isvalid)
            checkForDependencies "test"
            printf "=== \E[37;42mOK!\E[0m ===\n\n"
            exit 0
            ;;
        cool)
            printLine "Yeah!"                   "\E[0;42;37m" "\E[0;34m"
            printLine "This must be cool!"      "\E[0;42;36m" "\E[0;35m"
            printLine "Really?"                 "\E[0;42;35m" "\E[0;31m"
            printLine "You are cool!?"          "\E[0;42;34m" "\E[0;32m"
            printLine "Yeah!"                   "\E[0;42;33m" "\E[0;36m"
            exit 0
            ;;
        lupdate)
            echo ""
            echo "Running translation refreshing...";

            printLine "Editor" "\E[0;42;37m" "\E[0;34m"
            ${QT_PATH}/lupdate Editor/pge_editor.pro

            printLine "Engine" "\E[0;42;37m" "\E[0;34m"
            ${QT_PATH}/lupdate Engine/pge_engine.pro

            printLine "Done!" "\E[0;42;37m" "\E[0;32m"
            exit 0;
        ;;
    esac
done

# Validate built dependencies!
checkForDependencies

#=======================================================================
# build translations of the editor
#cd Editor
#$LRelease *.pro
#checkState
#cd ../Engine
#$LRelease -idbased *.pro
#checkState
#cd ..
#=======================================================================
# build all components
echo "Running $QMake..."
if [[ "$OSTYPE" == "linux-gnu" || "$OSTYPE" == "linux" ]]; then
    $QMake CONFIG+=release CONFIG-=debug QTPLUGIN.platforms=qxcb QMAKE_TARGET.arch=$(uname -m) $QMAKE_EXTRA_ARGS
else
    $QMake CONFIG+=release CONFIG-=debug $QMAKE_EXTRA_ARGS
fi
checkState

#=======================================================================
echo "Building..."
TIME_STARTED=$(date +%s)
make $MAKE_EXTRA_ARGS
checkState
TIME_ENDED=$(date +%s)
TIME_PASSED=$(($TIME_ENDED-$TIME_STARTED))
#=======================================================================
# copy data and configs into the build directory
echo "Installing..."
make -s install
checkState

#=======================================================================
echo ""

show_time $TIME_PASSED
printLine "BUILT!" "\E[0;42;37m" "\E[0;32m"

cd $bak
if $flag_pause_on_end ; then
    pause
fi
exit 0
