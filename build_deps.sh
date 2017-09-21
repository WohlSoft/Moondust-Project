#/bin/bash
bak=~+

if [[ "$OSTYPE" == "msys"* ]]; then
    ./build_deps.bat
    exit 0
fi

OurOS="linux_defaut"
if [[ "$OSTYPE" == "darwin"* ]]; then
    OurOS="macos"
elif [[ "$OSTYPE" == "linux-gnu" || "$OSTYPE" == "linux" ]]; then
    OurOS="linux"
elif [[ "$OSTYPE" == "freebsd"* ]]; then
    OurOS="freebsd"
elif [[ "$OSTYPE" == "msys"* ]]; then
    OurOS="win32"
elif [[ "$OSTYPE" == "haiku" ]]; then
    OurOS="haiku"
fi

echo $OurOS

#flags
flag_pause_on_end=true
flag_nolibs=false
flag_libsonly=false
flag_debug_script=false
QMAKE_EXTRA_ARGS=""
MAKE_EXTRA_ARGS="-r"
MAKE_CPUS_COUNT=4

for var in "$@"
do
    case "$var" in
    --help)
            echo ""
            printf "=== \e[44mPGE Project Dependency builder for UNIX-Like operating systems\e[0m ===\n"
            echo ""
            printf "\E[4mSYNTAX:\E[0m\n"
            echo ""
            printf "    $0 \e[90m[<arg1>] [<arg2>] [<arg2>] ...\e[0m\n"
            echo ""
            printf "\E[4mAVAILABLE ARGUMENTS:\E[0m\n"
            echo ""

            echo "--- Flags ---"
            printf " \E[1;4mno-libs\E[0m          - Don't build Autotools-based libraries like SDL2, LuaJIT, FreeType, etc.\n"
            printf " \E[1;4mlibs-only\E[0m        - Don't build QMake-based libraries\n"
            printf " \E[1;4mno-pause\E[0m         - Don't pause script on completion'\n"
            printf  " \E[1;4muse-ccache\E[0m       - Use the CCache to speed-up build process\n"
            if [[ ! -f /usr/bin/ccache && ! -f /bin/ccache && ! -f /usr/local/bin/ccache ]]; then
                printf " \E[0;4;41;37m<ccache is not installed!>\E[0m"
            fi
            printf "\n"
            echo ""

            echo "--- Special ---"
            printf " \E[1;4mdebug-script\E[0m      - Show some extra information to debug this script\n"
            echo ""

            printf "==== \e[43mIMPORTANT!\e[0m ====\n"
            echo "This script is designed for Linux and macOS operating systems."
            echo "If you trying to start it under Windows, it will automatically start"
            echo "the build_deps.bat script instead of this."
            echo "===================="
            echo ""
            exit 1
            ;;
        no-pause)
                flag_pause_on_end=false
            ;;
        use-ccache)
                QMAKE_EXTRA_ARGS="$QMAKE_EXTRA_ARGS CONFIG+=useccache"
            ;;
        no-libs)
                flag_nolibs=true
                flag_libsonly=false
            ;;
        libs-only)
                flag_nolibs=false
                flag_libsonly=true
            ;;
        debug-script)
                flag_debug_script=true
            ;;
    esac
done

# Hide command message while building
if ! $flag_debug_script ; then
    MAKE_EXTRA_ARGS="-s $MAKE_EXTRA_ARGS"
fi

#=============Detect directory that contains script=====================
SOURCE="${BASH_SOURCE[0]}"
while [ -h "$SOURCE" ]; do # resolve $SOURCE until the file is no longer a symlink
    SCRDIR="$( cd -P "$( dirname "$SOURCE" )" && pwd )"
    if [[ "$OurOS" == "macos" ]]; then
        SOURCE="$(osx_realpath "$SOURCE")"
    else
        SOURCE="$(readlink "$SOURCE")"
    fi
    [[ $SOURCE != /* ]] && SOURCE="$DIR/$SOURCE" # if $SOURCE was a relative symlink, we need to resolve it relative to the path where the symlink file was located
done
SCRDIR="$( cd -P "$( dirname "$SOURCE" )" && pwd )"
#=======================================================================
cd $SCRDIR
source ./_common/functions.sh
#=======================================================================
TIME_STARTED_LIBS=0
TIME_ENDED_LIBS=0
TIME_PASSED_LIBS=0

MAKE_CPUS_COUNT=$(getCpusCount)

buildLibs()
{
    TIME_STARTED_LIBS=$(date +%s)
    # build libraries
    cd "$PrjPath/_Libs/_sources"
    InstallTo=$(echo ~+/../_builds/$OurOS)

    if [[ "$OurOS" == "macos" ]]; then
       InstallTo=$(osx_realpath $InstallTo)
    else
       InstallTo=$(readlink -f $InstallTo)
    fi

    echo "Library install path: $InstallTo"
    source ./___build_script.sh
    TIME_ENDED_LIBS=$(date +%s)
    TIME_PASSED_LIBS=$(($TIME_ENDED_LIBS-$TIME_STARTED_LIBS))
}

PrjPath=$SCRDIR
if [ -f "$SCRDIR/_paths.sh" ]
then
	source "$SCRDIR/_paths.sh"
else
	echo ""
	echo "_paths.sh is not exist! Run \"generate_paths.sh\" first!"
	exit 1
fi

PATH=$QT_PATH:$PATH
LD_LIBRARY_PATH=$QT_LIB_PATH:$LD_LIBRARY_PATH

echo "Clonning missing submodules..."
git submodule update --init --recursive
checkState

#=======================================================================

if $flag_nolibs ; then
	echo "Building of tag.gz libraries skiped"
	echo "Building SDL Mixer X, LuaBIND and FreeImage..."
else
	buildLibs
    if $flag_libsonly ; then
        exit 0
    fi
fi

cd "$PrjPath/_Libs"

echo "Running $QMake..."
$QMake pge_deps.pro CONFIG+=release CONFIG-=debug DEFINES+=USE_LUA_JIT $QMAKE_EXTRA_ARGS
checkState

#=======================================================================
echo "Building (${MAKE_CPUS_COUNT} parallel jobs)..."
TIME_STARTED=$(date +%s)
make $MAKE_EXTRA_ARGS -j ${MAKE_CPUS_COUNT}
checkState
if [[ $OurOS == "linux" ||  $OurOS == "haiku" ]]; then
    cd SDL_Mixer_X
    echo "Linking static SDL Mixer X..."
    make staticlib $MAKE_EXTRA_ARGS -j ${MAKE_CPUS_COUNT}
    checkState
    cd ..
fi
TIME_ENDED=$(date +%s)
TIME_PASSED=$(($TIME_ENDED-$TIME_STARTED))
#=======================================================================
# copy data and configs into the build directory
echo "Installing..."
make -s install
checkState

cd ..
#=======================================================================
echo ""
echo "Autotools-based built libraries"
show_time $TIME_PASSED_LIBS

echo "QMake-based built libraries"
show_time $TIME_PASSED

echo "Total time of build"
show_time $(($TIME_PASSED + $TIME_PASSED_LIBS))

printLine "DEPENDENCIES HAS BEEN BUILT!" "\E[0;42;37m" "\E[0;32m"

cd $bak
if $flag_pause_on_end ; then
    pause
fi

exit 0
