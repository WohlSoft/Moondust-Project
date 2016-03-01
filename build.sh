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

for var in "$@"
do
    case "$var" in
        no-pause)
                flag_pause_on_end=false
            ;;
        use-ccache)
                QMAKE_EXTRA_ARGS="$QMAKE_EXTRA_ARGS CONFIG+=useccache"
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

#=======================================================================
# build translations of the editor
cd Editor
$LRelease *.pro
checkState
cd ../Engine
$LRelease *.pro
checkState
cd ..
#=======================================================================
# build all components
if [[ "$OSTYPE" == "linux-gnu" ]]; then
    $QMake CONFIG+=release CONFIG-=debug QTPLUGIN.platforms=qxcb QMAKE_TARGET.arch=$(uname -m) $QMAKE_EXTRA_ARGS
else
    $QMake CONFIG+=release CONFIG-=debug $QMAKE_EXTRA_ARGS
fi
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

#=======================================================================
echo ""
show_time $TIME_PASSED
printf "\n\n=========BUILT!!===========\n\n"
cd $bak
if $flag_pause_on_end ; then 
    read -n 1;
fi
exit 0

