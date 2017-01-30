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
make $MAKE_EXTRA_ARGS release
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
printf "\n\n=========\E[37;42mBUILT!!\E[0m===========\n\n"
cd $bak
if $flag_pause_on_end ; then
    read -n 1;
fi
exit 0
