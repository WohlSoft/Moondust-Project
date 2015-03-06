#/bin/bash
bak=~+
cd $PWD

PrjPath=$PWD
source "$PrjPath/_paths.sh"

#=======================================================================
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
	
#=======================================================================
# build libraries
cd "$PrjPath/_Libs/_sources"
InstallTo=$(echo ~+/../_builds/linux)
InstallTo=$(readlink -f $InstallTo)
echo $InstallTo
source ./___build_script.sh

cd "$PrjPath/_Libs"

$QMake CONFIG+=release CONFIG-=debug
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
read -n 1
exit 0

