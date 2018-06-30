#/bin/bash
bak=~+

if [[ "$OSTYPE" == "msys"* ]]; then
    ./build.bat
    exit 0
fi

#flags
flag_pause_on_end=true
QMAKE_EXTRA_ARGS=""
MAKE_EXTRA_ARGS="-r"
MAKE_CPUS_COUNT=4
flag_debugThisScript=false
flag_debugDependencies=false
flag_cmake_it=false
flag_cmake_deploy=false
flag_cmake_static_qt=false
flag_debug_build=false

for var in "$@"
do
    case "$var" in
        --help)
            echo ""
            printf "=== \e[44mBuild script for PGE Project for UNIX-Like operating\e[0m ===\n"
            echo ""
            printf "\E[4mSYNTAX:\E[0m\n"
            echo ""
            printf "    $0 \e[90m[<arg1>] [<arg2>] [<arg2>] ...\e[0m\n"
            echo ""
            printf "\E[4mAVAILABLE ARGUMENTS:\E[0m\n"
            echo ""

            echo "--- Actions ---"
            printf " \E[1;4mlupdate\E[0m          - Update the translations\n"
            printf " \E[1;4mlrelease\E[0m         - Compile the translations\n"
            printf " \E[1;4mclean\E[0m            - Remove all object files and caches to build from scratch\n"
            printf " \E[1;4mupdate-submodules\E[0m- Pull all submodules up to their latest states\n"
            printf " \E[1;4mrepair-submodules\E[0m- Repair invalid or broken submodules\n"
            printf " \E[1;4misvalid\E[0m          - Show validation state of dependencies\n"
            printf " \E[1;4m--help\E[0m           - Print this manual\n"
            printf " \E[1;4mldoc\E[0m             - Make lua documentation\n"
            printf " \E[1;4mcmake-it\E[0m         - Run build through experimental alternative build on CMake\n"
            echo ""

            echo "--- Flags ---"
            printf " \E[1;4mno-pause\E[0m         - Don't pause script on completion'\n"
            printf " \E[1;4msilent-make\E[0m      - Don't print build commands for each building file\n"
            printf " \E[1;4muse-ccache\E[0m       - Use the CCache to speed-up build process\n"
            printf " \E[1;4mdebug\E[0m            - Run build in debug configuration'\n"
            printf " \E[1;4mdeploy\E[0m           - Automatically run a deploymed (CMake only build)'\n"
            if [[ ! -f /usr/bin/ccache && ! -f /bin/ccache && ! -f /usr/local/bin/ccache ]]; then
                printf " \E[0;4;41;37m<ccache is not installed!>\E[0m"
            fi
            printf "\n"
            echo ""

            echo "--- Disable building of components ---"
            printf " \E[1;4mnoqt\E[0m             - Skip building of components are using Qt\n"
            printf " \E[1;4mnoeditor\E[0m         - Skip building of PGE Editor compoment\n"
            printf " \E[1;4mnoengine\E[0m         - Skip building of PGE Engine compoment\n"
            printf " \E[1;4mnocalibrator\E[0m     - Skip building of Playable Character Calibrator compoment\n"
            printf " \E[1;4mnomaintainer\E[0m     - Skip building of PGE Maintainer compoment\n"
            printf " \E[1;4mnomanager\E[0m        - Skip building of PGE Manager compoment\n"
            printf " \E[1;4mnomusicplayer\E[0m    - Skip building of PGE MusPlay compoment\n"
            printf " \E[1;4mnogifs2png\E[0m       - Skip building of GIFs2PNG compoment\n"
            printf " \E[1;4mnopng2gifs\E[0m       - Skip building of PNG2GIFs compoment\n"
            printf " \E[1;4mnolazyfixtool\E[0m    - Skip building of LazyFixTool compoment\n"
            echo ""

            echo "--- Special ---"
            printf " \E[1;4mdebug-script\E[0m      - Show some extra information to debug this script\n"
            echo ""

            echo "--- For fun ---"
            printf " \E[1;4mcolors\E[0m           - Prints various blocks of different color with showing their codes\n"
            printf " \E[1;4mcool\E[0m             - Prints some strings inside the lines (test of printLine command)\n"
            echo ""

            printf "==== \e[43mIMPORTANT!\e[0m ====\n"
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
        cmake-it)
                flag_cmake_it=true
            ;;
        deploy)
                flag_cmake_deploy=true
            ;;
        static-qt)
                flag_cmake_static_qt=true
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
        ldoc)
            cd Engine/doc
            ../../build-pge-cmake-release/bin/luajit-2.1.0-beta3 ../../../LDoc/ldoc.lua .
            cd ../..
            printf "\n==== Done! ====\n\n"
            exit 0;
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
        update-submodules)
            git submodule foreach git checkout master
            git submodule foreach git pull origin master
            exit 0
            ;;
        repair-submodules)
            #!!FIXME!! Implement parsing of submodules list and fill this array automatically
            #NOTE: Don't use "git submodule foreach" because broken submodule will not shown in it's list!
            SUBMODULES="_Libs/FreeImage"
            SUBMODULES="${SUBMODULES} _Libs/QtPropertyBrowser"
            SUBMODULES="${SUBMODULES} _Libs/sqlite3"
            SUBMODULES="${SUBMODULES} _common/PGE_File_Formats"
            SUBMODULES="${SUBMODULES} _common/PgeGameSave/submodule"
            SUBMODULES="${SUBMODULES} _Libs/AudioCodecs"
            SUBMODULES="${SUBMODULES} _Libs/SDL_Mixer_X"
            SUBMODULES="${SUBMODULES} Content/help"
            # \===============================================================================
            for s in $SUBMODULES
            do
                if [ -d $s ];then
                    echo "Remove folder ${s}..."
                    rm -Rf $s
                fi
            done
            echo "Fetching new submodules..."
            git submodule init
            git submodule update
            echo ""
            git submodule foreach git checkout master
            git submodule foreach git pull origin master
            echo ""
            echo "==== Fixed! ===="
            exit 0;
            ;;

        # Enable debuggin of this script by showing states of inernal variables with pauses
        debug-script)
            flag_debugThisScript=true
            ;;
        debug)
            flag_debug_build=true
            ;;
        test)
            flag_debugDependencies=true
            ;;
        noqt)
            QMAKE_EXTRA_ARGS="${QMAKE_EXTRA_ARGS} CONFIG+=noeditor"
            QMAKE_EXTRA_ARGS="${QMAKE_EXTRA_ARGS} CONFIG+=nocalibrator"
            QMAKE_EXTRA_ARGS="${QMAKE_EXTRA_ARGS} CONFIG+=nomanager"
            QMAKE_EXTRA_ARGS="${QMAKE_EXTRA_ARGS} CONFIG+=nomaintainer"
            QMAKE_EXTRA_ARGS="${QMAKE_EXTRA_ARGS} CONFIG+=nomusicplayer"
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
        nomusicplayer)
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

MAKE_CPUS_COUNT=$(getCpusCount)

if $flag_debugThisScript; then
    echo "QMAKE_EXTRA_ARGS = ${QMAKE_EXTRA_ARGS}"
    echo "MAKE_EXTRA_ARGS = ${MAKE_EXTRA_ARGS}"
    echo "MAKE_CPUS_COUNT = ${MAKE_CPUS_COUNT}"
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
    elif [[ "$OSTYPE" == "haiku" ]]; then
        osDir="haiku"
        libPref="lib"
    elif [[ "$OSTYPE" == "msys"* ]]; then
        dlibExt="a"
        osDir="win32"
        libPref="lib"
    fi
    libsDir=$SCRDIR/_Libs/_builds/$osDir/
    #echo "$libsDir"

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
        if $flag_debugDependencies; then
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
            ${QT_PATH}/lupdate -locations none -no-ui-lines Editor/pge_editor.pro

            printLine "Engine" "\E[0;42;37m" "\E[0;34m"
            ${QT_PATH}/lupdate -locations none Engine/pge_engine.pro

            printLine "Done!" "\E[0;42;37m" "\E[0;32m"
            exit 0;
            ;;
        lrelease)
            echo ""
            echo "Running translation compilation...";

            printLine "Editor" "\E[0;42;37m" "\E[0;34m"
            ${QT_PATH}/$LRelease Editor/pge_editor.pro

            printLine "Engine" "\E[0;42;37m" "\E[0;34m"
            ${QT_PATH}/$LRelease Engine/pge_engine.pro

            printLine "Done!" "\E[0;42;37m" "\E[0;32m"
            exit 0;
            ;;
    esac
done

if $flag_debug_build ; then
    echo "==DEBUG BUILD!=="
    BUILD_DIR_SUFFUX="-debug"
    CONFIG_QMAKE="CONFIG-=release CONFIG+=debug"
    CONFIG_CMAKE="Debug"
else
    echo "==RELEASE BUILD!=="
    BUILD_DIR_SUFFUX="-release"
    CONFIG_QMAKE="CONFIG+=release CONFIG-=debug"
    CONFIG_CMAKE="Release"
fi

# Alternative building through CMake
if $flag_cmake_it ; then
    echo "==== Alternative build via CMake will be ran ===="
    echo "It's experimental build yet which will replace"
    echo "this clunky build system soon."
    echo "I hope you will like it ;3"
    echo ""
    echo "Wohlstand."
    echo "================================================="
    echo ""

    BUILD_DIR="${SCRDIR}/build-pge-cmake${BUILD_DIR_SUFFUX}"
    INSTALL_DIR="${SCRDIR}/bin-cmake${BUILD_DIR_SUFFUX}"

    if [ ! -d "${BUILD_DIR}" ]; then
        mkdir -p "${BUILD_DIR}"
    fi

    cd "${BUILD_DIR}"

    CMAKE_STATIC_QT=""
    if $flag_cmake_static_qt; then
        CMAKE_STATIC_QT="-DPGE_ENABLE_STATIC_QT=ON"
    fi

    #=======================================================================
    cmake \
        -G "Unix Makefiles" \
        -DCMAKE_PREFIX_PATH=$(realpath "${QT_PATH}/../") \
        -DCMAKE_INSTALL_PREFIX=${INSTALL_DIR} \
        -DCMAKE_BUILD_TYPE=$CONFIG_CMAKE \
        -DPGE_INSTALL_DIRECTORY="PGE_Project" \
        "${SCRDIR}" \
        ${CMAKE_STATIC_QT}
        # -DQT_QMAKE_EXECUTABLE="$QMake"
    checkState

    #=======================================================================
    echo "Building (${MAKE_CPUS_COUNT} parallel jobs)..."
    TIME_STARTED=$(date +%s)
    make ${MAKE_EXTRA_ARGS} -j ${MAKE_CPUS_COUNT}
    checkState
    TIME_ENDED=$(date +%s)
    TIME_PASSED=$(($TIME_ENDED-$TIME_STARTED))
    #=======================================================================
    # copy data and configs into the build directory

    echo "Installing..."
    make -s install
    checkState

    if $flag_cmake_deploy ; then
        echo "Deploying..."
        make -s put_online_help
        checkState
        make -s enable_portable
        checkState
        make -s create_tar
        checkState
    fi

    cd "${SCRDIR}"

    #=======================================================================
    echo ""

    show_time $TIME_PASSED
    printLine "BUILT!" "\E[0;42;37m" "\E[0;32m"

    cd $bak
    if $flag_pause_on_end ; then
        pause
    fi

    exit 0;
fi

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
    $QMake $CONFIG_QMAKE QTPLUGIN.platforms=qxcb QMAKE_TARGET.arch=$(uname -m) $QMAKE_EXTRA_ARGS
else
    $QMake $CONFIG_QMAKE $QMAKE_EXTRA_ARGS
fi
checkState

#=======================================================================
echo "Building (${MAKE_CPUS_COUNT} parallel jobs)..."
TIME_STARTED=$(date +%s)
make ${MAKE_EXTRA_ARGS} -j ${MAKE_CPUS_COUNT}
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

