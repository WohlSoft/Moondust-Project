#/bin/bash
bak=~+

errorofbuid()
{
	printf "\n\n=========ERROR!!===========\n\n"
	cd $bak
	exit 1
}

checkState()
{
	if [ $? -eq 0 ]
	then
	  echo "[good]"
	else
	  errorofbuid
	fi
}

osx_realpath() {
  case "${1}" in
    [./]*)
    echo "$(cd ${1%/*}; pwd)/${1##*/}"
    ;;
    *)
    echo "${PWD}/${1}"
    ;;
  esac
}

OurOS="linux_defaut"
if [[ "$OSTYPE" == "darwin"* ]]; then
  OurOS="macos"
elif [[ "$OSTYPE" == "linux-gnu" || "$OSTYPE" == "linux" ]]; then
  OurOS="linux"
elif [[ "$OSTYPE" == "freebsd"* ]]; then
  OurOS="freebsd"
elif [[ "$OSTYPE" == "msys"* ]]; then
  OurOS="win32"
fi

if [[ "$OurOS" == "win32" ]]; then
   ./build_deps.bat
   exit 0
   checkState
fi

echo $OurOS

#flags
flag_pause_on_end=true
flag_nolibs=false
flag_libsonly=false
QMAKE_EXTRA_ARGS=""
MAKE_EXTRA_ARGS="-r -j 4"

for var in "$@"
do
    case "$var" in
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
    esac
done

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
	errorofbuid
fi

PATH=$QT_PATH:$PATH

#=======================================================================

if $flag_nolibs ; then
	echo "Building of tag.gz libraries skiped"
	echo "Building SDL2_mixer_ext, LuaBIND and FreeImage..."
else
	buildLibs
    if $flag_libsonly ; then
        exit 0
    fi
fi

cd "$PrjPath/_Libs"

$QMake CONFIG+=release CONFIG-=debug DEFINES+=USE_LUA_JIT $QMAKE_EXTRA_ARGS
checkState

#=======================================================================
TIME_STARTED=$(date +%s)
make $MAKE_EXTRA_ARGS
checkState
TIME_ENDED=$(date +%s)
TIME_PASSED=$(($TIME_ENDED-$TIME_STARTED))
#=======================================================================
# copy data and configs into the build directory
make install
checkState

cd ..
#=======================================================================
echo ""
echo "Autotools-based built libraries"
show_time $TIME_PASSED_LIBS
echo "QMake-based built libraries"
show_time $TIME_PASSED
echo "Total time of build"
show_time $(($TIME_PASSED+$TIME_PASSED_LIBS))
printf "\n\n=========DEPENDENCIES HAS BEEN BUILT!!===========\n\n"
cd $bak
if $flag_pause_on_end ; then
    read -n 1;
fi
exit 0
