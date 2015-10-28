#/bin/bash
bak=~+

#=============Detect directory that contains script=====================
SOURCE="${BASH_SOURCE[0]}"
while [ -h "$SOURCE" ]; do # resolve $SOURCE until the file is no longer a symlink
  SCRDIR="$( cd -P "$( dirname "$SOURCE" )" && pwd )"
  SOURCE="$(readlink "$SOURCE")"
  [[ $SOURCE != /* ]] && SOURCE="$DIR/$SOURCE" # if $SOURCE was a relative symlink, we need to resolve it relative to the path where the symlink file was located
done
SCRDIR="$( cd -P "$( dirname "$SOURCE" )" && pwd )"
#=======================================================================

cd $SCRDIR

#=======================================================================
errorofbuid()
{
	printf "\n\n=========AN ERROR OCCURED!==========\n\n"
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

if [ -f "$SCRDIR/_paths.sh" ]
then
	source "$SCRDIR/_paths.sh"
else
	echo ""
	echo "_paths.sh is not exist! Run \"generate_paths.sh\" first!"
	errorofbuid
fi

#=======================================================================
# build translations of the editor
cd Editor
$LRelease *.pro
checkState
cd ..

#=======================================================================
# build all components
$QMake CONFIG+=release CONFIG-=debug
checkState

#=======================================================================
make
checkState

#=======================================================================
# copy data and configs into the build directory
make install
checkState


#=======================================================================
printf "\n\n=========BUILT!!===========\n\n"
cd $bak
if [[ $1 != "no-pause" ]]; then read -n 1; fi
exit 0

