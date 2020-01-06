#!/bin/bash
bak=~+

#flags
flag_pause_on_end=true
CMAKE_EXTRA_ARGS=""
MAKE_EXTRA_ARGS="-r"
MAKE_CPUS_COUNT=4
CMAKE_GENERATOR="Unix Makefiles"
flag_debugThisScript=false
flag_debugDependencies=false
flag_pack_src=false
flag_pack_src_gz=false
flag_cmake_it_ninja=false
flag_cmake_deploy=false
flag_cmake_static_qt=false
flag_debug_build=false
flag_portable=false

for var in "$@"
do
    case "$var" in
        --help)
            echo ""
            printf "=== \e[44mBuild script for PGE Project for UNIX-Like operating\e[0m ===\n"
            echo ""
            if [[ "$(which cmake)" == "" ]]; then
                printf " \E[0;4;41;37mIMPORTANT!: CMake is not installed! CMake is required to build this project.\E[0m\n\n"
            fi
            printf "\E[4mSYNTAX:\E[0m\n"
            echo ""
            printf "    $0 \e[90m[<arg1>] [<arg2>] [<arg2>] ...\e[0m\n"
            echo ""
            printf "\E[4mAVAILABLE ARGUMENTS:\E[0m\n"
            echo ""

            echo "--- Actions ---"
            printf " \E[1;4mlupdate\E[0m          - Update the translations\n"
            printf " \E[1;4mclean\E[0m            - Remove all object files and caches to build from scratch\n"
            printf " \E[1;4mupdate-submodules\E[0m- Pull all submodules up to their latest states\n"
            printf " \E[1;4mrepair-submodules\E[0m- Repair invalid or broken submodules\n"
            printf " \E[1;4misvalid\E[0m          - Show validation state of dependencies\n"
            printf " \E[1;4m--help\E[0m           - Print this manual\n"
            printf " \E[1;4mldoc\E[0m             - Make lua documentation\n"
            if [[ ! -f ../LDoc/ldoc.lua ]]; then
                printf " \E[0;4;41;37m<LDoc repository is not clonned! Clone it into parent folder!>\E[0m\n"
            elif [[ ! -f ./build-pge-cmake-release/bin/luajit-2.1.0-beta3 ]]; then
                printf " \E[0;4;44;37m<To use LDoc you need to build the project first!>\E[0m\n"
            fi

            printf " \E[1;4mpack-src\E[0m         - Create the source code archive\n"
            printf "                    (git-archive-all is required!)\n"
            if [[ "$(which git-archive-all)" == "" ]]; then
                printf " \E[0;4;41;37m<git-archive-all is not installed!>\E[0m\n"
            fi

            echo ""

            echo "--- Flags ---"
            printf " \E[1;4mno-pause\E[0m         - Don't pause script on completion'\n"
            printf " \E[1;4msilent-make\E[0m      - Don't print build commands for each building file\n"
            printf " \E[1;4mdebug\E[0m            - Run build in debug configuration'\n"
            printf " \E[1;4mninja\E[0m            - Use Ninja build system (CMake only build)'\n"
            if [[ "$(which ninja)" == "" ]]; then
                printf " \E[0;4;41;37m<ninja is not installed!>\E[0m\n"
            fi

            printf " \E[1;4mdeploy\E[0m           - Automatically run a deploymed'\n"
            printf " \E[1;4muse-ccache\E[0m       - Use the CCache to speed-up build process\n"
            if [[ "$(which ccache)" == "" ]]; then
                printf " \E[0;4;41;37m<ccache is not installed!>\E[0m"
            fi
            printf "\n"
            echo ""

            echo "--- Compile options ---"
            printf " \E[1;4msystem-png\E[0m       - Prefer to use libPNG and ZLib from system\n"
            printf " \E[1;4msystem-libs\E[0m      - Prefer to use most of libraries from system\n"
            printf "\n"
            echo ""

            echo "--- Deployment options ---"
            printf " \E[1;4mportable\E[0m         - Deploy a portable installation that will don't store\n"
            printf "                    settings, logs, and game saves in system or home directory.\n"
            if [[ "$OSTYPE" == "darwin"* ]]; then
                printf " \E[0;4;44;37m<Not supported on macOS, this option will take no effect>\E[0m\n"
            fi
            printf "\n"
            echo ""

            echo "--- Disable building of components ---"
            printf " \E[1;4mnoqt\E[0m             - Skip building of components are using Qt\n"
            printf " \E[1;4mnoeditor\E[0m         - Skip building of PGE Editor compoment (Qt5)\n"
            printf " \E[1;4mnoengine\E[0m         - Skip building of PGE Engine compoment (SDL2)\n"
            printf " \E[1;4mnocalibrator\E[0m     - Skip building of Playable Character Calibrator (Qt5)\n"
            printf " \E[1;4mnomaintainer\E[0m     - Skip building of PGE Maintainer compoment (Qt5)\n"
            printf " \E[1;4mnomanager\E[0m        - Skip building of PGE Manager compoment (Qt5)\n"
            printf " \E[1;4mnomusicplayer\E[0m    - Skip building of PGE MusPlay compoment (Qt5)\n"
            printf " \E[1;4mnogifs2png\E[0m       - Skip building of GIFs2PNG compoment (STL)\n"
            printf " \E[1;4mnopng2gifs\E[0m       - Skip building of PNG2GIFs compoment (STL)\n"
            printf " \E[1;4mnolazyfixtool\E[0m    - Skip building of LazyFixTool compoment (STL)\n"
            echo ""

            echo "--- Special ---"
            printf " \E[1;4mdebug-script\E[0m     - Show some extra information to debug this script\n"
            echo ""

            echo "--- For fun ---"
            printf " \E[1;4mcolors\E[0m           - Prints various blocks of different color\n"
            printf "                    with showing their codes\n"
            printf " \E[1;4mcool\E[0m             - Prints some strings inside the lines\n"
            printf "                    (test of printLine command)\n"
            echo ""

            #printf "==== \e[43mIMPORTANT!\e[0m ====\n"
            #echo "This script is designed for Linux and macOS operating systems."
            #echo "If you trying to start it under Windows, it will automatically start"
            #echo "the build.bat script instead of this."
            #echo "===================="
            #echo ""
            exit 1
            ;;
        no-pause)
                flag_pause_on_end=false
            ;;
        silent-make)
                MAKE_EXTRA_ARGS="${MAKE_EXTRA_ARGS} -s"
            ;;
        pack-src)
                flag_pack_src=true
            ;;
        gz)
                flag_pack_src_gz=true
            ;;
        cmake-it)
                # dummy
            ;;
        qmake-it)
                echo "QMake support has been removed! Please use CMake build instead!"
                exit 1;
            ;;
        ninja)
                CMAKE_GENERATOR="Ninja"
                flag_cmake_it_ninja=true
                MAKE_EXTRA_ARGS=""
            ;;
        deploy)
                flag_cmake_deploy=true
            ;;
        portable)
                flag_portable=true
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
            # FIXME: 1) Autodetect luajit executible. 2) Don't rely on canonical paths, add LDoc as the submodule
            cd Engine/doc
            LUAJIT_BIN="../../build-pge-cmake-release/bin/luajit"
            if [[ -f "../../build-pge-cmake-release/bin/luajit" ]]; then
                LUAJIT_BIN="../../build-pge-cmake-release/bin/luajit"
            elif [[ -f "../../build-pge-cmake-release/bin/luajit.exe" ]]; then
                LUAJIT_BIN="../../build-pge-cmake-release/bin/luajit.exe"
            elif [[ -f "../../build-pge-cmake-release/bin/luajit-2.1.0-beta3" ]]; then
                LUAJIT_BIN="../../build-pge-cmake-release/bin/luajit-2.1.0-beta3"
            fi
            ${LUAJIT_BIN} ../../../LDoc/ldoc.lua .
            cd ../..
            printf "\n==== Done! ====\n\n"
            exit 0;
            ;;
        use-ccache)
                # FIXME: Implement this option for CMake case
                # if [[ "$OSTYPE" == "linux-gnu" ]]; then
                #     QMAKE_EXTRA_ARGS="$QMAKE_EXTRA_ARGS -spec linux-g++ CONFIG+=useccache"
                # else
                #     QMAKE_EXTRA_ARGS="$QMAKE_EXTRA_ARGS CONFIG+=useccache"
                # fi
            ;;
        clean)
                echo "======== Remove all cached object files and automatically generated Makefiles ========"

                if [[ -d ./${BinDir}/_build_x32 ]]; then
                    echo "removing ${BinDir}/_build_x32 ..."
                    rm -Rf ./${BinDir}/_build_x32
                fi

                if [[ -d ./${BinDir}/_build_x64 ]]; then
                    echo "removing ${BinDir}/_build_x64 ..."
                    rm -Rf ./${BinDir}/_build_x64
                fi

                if [[ -d ./build-pge-cmake-release/ ]]; then
                    echo "removing build-pge-cmake-release/ ..."
                    cd ./build-pge-cmake-release/
                    find . -not -path "./bin*" -delete
                    cd ..
                fi

                if [[ -d ./build-pge-cmake-debug/ ]]; then
                    echo "removing build-pge-cmake-debug/ ..."
                    cd ./build-pge-cmake-debug/
                    find . -not -path "./bin*" -delete
                    cd ..
                fi

                echo 'removing Dependencies build cache ...'

                if [[ -d $PWD/_Libs/_sources/_build_cache ]];
                then
                    echo "Deleting $PWD/_Libs/_sources/_build_cache..."
                    rm -Rf $PWD/_Libs/_sources/_build_cache
                fi

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
            for s in ${SUBMODULES}
            do
                if [[ -d ${s} ]];then
                    echo "Remove folder ${s}..."
                    rm -Rf ${s}
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

        # Enable debugging of this script by showing states of internal variables with pauses
        debug-script)
            flag_debugThisScript=true
            ;;
        debug)
            flag_debug_build=true
            ;;
        test)
            flag_debugDependencies=true
            ;;
        system-png)
            CMAKE_EXTRA_ARGS="${CMAKE_EXTRA_ARGS} -DUSE_SYSTEM_LIBPNG=ON"
            ;;
        system-libs)
            CMAKE_EXTRA_ARGS="${CMAKE_EXTRA_ARGS} -DUSE_SYSTEM_LIBC=ON -DUSE_SYSTEM_LIBFREETYPE=ON -DUSE_SYSTEM_LIBPNG=ON -DUSE_SYSTEM_SDL2=ON -DUSE_SYSTEM_SQLITE=ON -DPGE_STATIC_SDLMIXER=OFF -DUSE_SHARED_FREEIMAGE=ON"
            ;;
        noqt)
            CMAKE_EXTRA_ARGS="${CMAKE_EXTRA_ARGS} -DPGE_ENABLE_QT=OFF"
            ;;
        # Disable building of some components
        noeditor)
            CMAKE_EXTRA_ARGS="${CMAKE_EXTRA_ARGS} -DPGE_BUILD_EDITOR=OFF"
            ;;
        noengine)
            CMAKE_EXTRA_ARGS="${CMAKE_EXTRA_ARGS} -DPGE_BUILD_ENGINE=OFF"
            ;;
        nocalibrator)
            CMAKE_EXTRA_ARGS="${CMAKE_EXTRA_ARGS} -DPGE_BUILD_PLAYERCALIBRATOR=OFF"
            ;;
        nogifs2png)
            CMAKE_EXTRA_ARGS="${CMAKE_EXTRA_ARGS} -DPGE_BUILD_GIFS2PNG=OFF"
            ;;
        nopng2gifs)
            CMAKE_EXTRA_ARGS="${CMAKE_EXTRA_ARGS} -DPGE_BUILD_PNG2GIFS=OFF"
            ;;
        nolazyfixtool)
            CMAKE_EXTRA_ARGS="${CMAKE_EXTRA_ARGS} -DPGE_BUILD_LAZYFIXTOOL=OFF"
            ;;
        nomanager)
            # WIP
            # CMAKE_EXTRA_ARGS="${CMAKE_EXTRA_ARGS} -DPGE_BUILD_MANAGER=OFF"
            ;;
        nomaintainer)
            CMAKE_EXTRA_ARGS="${CMAKE_EXTRA_ARGS} -DPGE_BUILD_MAINTAINER=OFF"
            ;;
        nomusicplayer)
            CMAKE_EXTRA_ARGS="${CMAKE_EXTRA_ARGS} -DPGE_BUILD_MUSICPLAYER=OFF"
            ;;
    esac
done

#=============Detect directory that contains script=====================
SOURCE="${BASH_SOURCE[0]}"
while [[ -h "${SOURCE}" ]]; do # resolve $SOURCE until the file is no longer a symlink
    SCRDIR="$( cd -P "$( dirname "${SOURCE}" )" && pwd )"
    SOURCE="$(readlink "${SOURCE}")"
    [[ ${SOURCE} != /* ]] && SOURCE="${DIR}/$SOURCE" # if $SOURCE was a relative symlink, we need to resolve it relative to the path where the symlink file was located
done
SCRDIR="$( cd -P "$( dirname "$SOURCE" )" && pwd )"
#=======================================================================
echo ${SCRDIR}
cd ${SCRDIR}
source ./_common/functions.sh
#=======================================================================

if [[ -f "${SCRDIR}/_paths.sh" ]]
then
    source "${SCRDIR}/_paths.sh"
else
    echo ""
    echo "_paths.sh is not exist! Run \"generate_paths.sh\" first!"
    errorofbuild
fi

PATH=${QT_PATH}:$PATH
LD_LIBRARY_PATH=${QT_LIB_PATH}:${LD_LIBRARY_PATH}

MAKE_CPUS_COUNT=$(getCpusCount)

if ${flag_debugThisScript}; then
    echo "MAKE_EXTRA_ARGS = ${MAKE_EXTRA_ARGS}"
    echo "MAKE_CPUS_COUNT = ${MAKE_CPUS_COUNT}"
    pause
fi

# Check input arguments again
for var in "$@"
do
    case "$var" in
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
            # ${QT_PATH}/lupdate -locations none -no-ui-lines Editor/pge_editor.pro

            cd Editor

            find . \( -name "*.h" -o -name "*.cpp" -o -name "*.ui" -o -name "*.hpp" \) -printf "%P\n" > _lupdate_temp_list.tmp
            find ../_common/ \( -name "*.h" -o -name "*.cpp" -o -name "*.ui" -o -name "*.hpp" \) -printf "%p\n" >> _lupdate_temp_list.tmp
            for lang in bg bs de en es fr "he-il" "id" it ja "nl" "nb-no" pl "pt-br" "pt-pt" ru sv sr uk zh; do
                ${QT_PATH}/lupdate @_lupdate_temp_list.tmp -ts languages/editor_$lang.ts -I .
            done
            rm _lupdate_temp_list.tmp

            cd ..

            printLine "Engine" "\E[0;42;37m" "\E[0;34m"
            # ${QT_PATH}/lupdate -locations none Engine/pge_engine.pro

            cd Engine

            find . \( -name "*.h" -o -name "*.cpp" -o -name "*.hpp" \) -printf "%P\n" > _lupdate_temp_list.tmp
            for lang in de bs en es fr "he-il" it ja "nl" "nb-no" pl "pt-br" "pt-pt" ru sv sr zh; do
                ${QT_PATH}/lupdate @_lupdate_temp_list.tmp -ts languages/engine_$lang.ts -I .
            done
            rm _lupdate_temp_list.tmp

            cd ..

            printLine "Done!" "\E[0;42;37m" "\E[0;32m"
            exit 0;
            ;;
        lrelease)
            echo ""
            echo "-- This action is deprecated! --";
            echo "Translations will be compiled automatically while build process is going";
            echo "...Do nothing since this option is now useless...";
            exit 0;
            ;;
    esac
done

# ===== Source code packer =====
if ${flag_pack_src} ; then
    if [[ ! -d bin-archives ]]; then
        mkdir bin-archives
    fi
    if ${flag_pack_src_gz} ; then
        ARFORMAT=gz
    else
        ARFORMAT=bz2
    fi

    echo "Packing source code..."
    git archive-all -v --force-submodules bin-archives/pge-project-full-src.tar.${ARFORMAT}
    checkState

    printLine "Packed!" "\E[0;42;37m" "\E[0;32m"
    cd ${bak}
    if ${flag_pause_on_end} ; then
        pause
    fi

    exit 0;
fi


# ===== Build project =====

if ${flag_debug_build} ; then
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

# ===== Building through CMake =====
if [[ "$(which cmake)" == "" ]]; then
    printf " \E[0;4;41;37mFATAL ERROR!: CMake is not installed! CMake is required to build this project.\E[0m\n\n"
    exit 1
fi

deployTarget="create_tar"
if [[ "$OSTYPE" == "darwin"* ]]; then
    deployTarget="create_dmg"
elif [[ "$OSTYPE" == "msys"* ]]; then
    deployTarget="create_zip"
fi

BUILD_DIR="${SCRDIR}/build-pge-cmake${BUILD_DIR_SUFFUX}"
INSTALL_DIR="${SCRDIR}/bin-cmake${BUILD_DIR_SUFFUX}"

if [[ ! -d "${BUILD_DIR}" ]]; then
    mkdir -p "${BUILD_DIR}"
fi

cd "${BUILD_DIR}"

CMAKE_STATIC_QT=""
if ${flag_cmake_static_qt}; then
    CMAKE_STATIC_QT="-DPGE_ENABLE_STATIC_QT=ON"
fi

if [[ "$OSTYPE" == "darwin"* ]]; then
    QT_PREFIX_ROOT=$(osx_realpath "${QT_PATH}/../")
elif [[ -f /usr/bin/realpath ]]; then
    QT_PREFIX_ROOT=$(realpath "${QT_PATH}/../")
else
    QT_PREFIX_ROOT=$(readlink -f -- "${QT_PATH}/../")
fi

if [[ "$OSTYPE" == "msys"* ]]; then
    if ! $flag_cmake_it_ninja ; then
        CMAKE_GENERATOR="MSYS Makefiles"
    fi
    export CC="gcc"
    export CXX="g++"
fi

#=======================================================================
cmake \
    -G "${CMAKE_GENERATOR}" \
    -DCMAKE_PREFIX_PATH=${QT_PREFIX_ROOT} \
    -DCMAKE_INSTALL_PREFIX=${INSTALL_DIR} \
    -DCMAKE_BUILD_TYPE=${CONFIG_CMAKE} \
    -DPGE_INSTALL_DIRECTORY="PGE_Project" \
    "${SCRDIR}" \
    ${CMAKE_STATIC_QT} \
    ${CMAKE_EXTRA_ARGS}
    # -DQT_QMAKE_EXECUTABLE="$QMake"
checkState

#=======================================================================
echo "Building (${MAKE_CPUS_COUNT} parallel jobs)..."
TIME_STARTED=$(date +%s)
cmake --build . --target all -- ${MAKE_EXTRA_ARGS} -j ${MAKE_CPUS_COUNT}
checkState
TIME_ENDED=$(date +%s)
TIME_PASSED=$(($TIME_ENDED-$TIME_STARTED))
#=======================================================================
# copy data and configs into the build directory

echo "Installing..."
cmake --build . --target install
checkState

if ${flag_cmake_deploy} ; then
    echo "Deploying..."
    cmake --build . --target put_online_help
    checkState
    if [[ ${flag_portable} == true && "$OSTYPE" != "darwin"* ]]; then
        cmake --build . --target enable_portable
        checkState
    fi

    if [[ "$OSTYPE" == "msys"* ]]; then
        cmake --build . --target windeploy
        checkState
    fi

    cmake --build . --target $deployTarget
    checkState

    if [[ "$OSTYPE" == "msys"* ]]; then  # run extra tasks for Windows
        cmake --build . --target create_zip_tools
        checkState
        cmake --build . --target create_zip_install
        checkState
    fi
fi

cd "${SCRDIR}"

#=======================================================================
echo ""

show_time ${TIME_PASSED}
printLine "BUILT!" "\E[0;42;37m" "\E[0;32m"

cd ${bak}
if ${flag_pause_on_end} ; then
    pause
fi

exit 0;

