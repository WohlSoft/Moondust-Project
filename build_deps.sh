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
elif [[ "$OSTYPE" == "linux-gnu" ]]; then
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

#=======================================================================

buildLibs()
{
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
	
#=======================================================================

if [[ "$1" == "no-libs" ]]; then
	echo "Building of tag.gz libraries skiped"
	echo "Building SDL2_mixer, OOLua and Box2D..."
else
	buildLibs
fi

cd "$PrjPath/_Libs"

$QMake CONFIG+=release CONFIG-=debug DEFINES+=USE_LUA_JIT
checkState

#=======================================================================
make
checkState

#=======================================================================
# copy data and configs into the build directory
make install
checkState

cd ..
#=======================================================================
printf "\n\n=========BUILT!!===========\n\n"
cd $bak
# if [[ $1 != "no-pause" ]]; then read -n 1; fi
exit 0

