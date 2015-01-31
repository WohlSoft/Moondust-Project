#!/bin/bash

#Install Builds into _builds/linux directory
#InstallTo=~0/../_builds/linux

#Install Builds into _builds/macos directory
#InstallTo=~0/../_builds/macos

#Install Builds into /usr/ directory globally
#InstallTo = /usr/

#=======================================================================
errorofbuid()
{
	printf "\n\n=========ERROR!!===========\n\n"
	exit 1
}
#=======================================================================

if [ ! -d SDL ] 
	then
	mkdir SDL
	fi


cd SDL

if [ ! -d SDL2-2.0.3 ]
	then
	tar -xvf ../SDL2-2.0.3.tar.gz
	fi

if [ ! -d SDL2-2.0.3 ]
	then
	tar -xvf ../SDL2_mixer-2.0.0.tar.gz
	fi

if [ ! -d libogg-1.3.2 ]
	then
	tar -xvf ../libogg-1.3.2.tar.gz
	fi

if [ ! -d libvorbis-1.3.4 ]
	then
	tar -xvf ../libvorbis-1.3.4.tar.gz
	fi

if [ ! -d flac-1.3.0 ]
	then
	tar -xvf ../flac-1.3.0.tar.xz
	fi

if [ ! -d libmikmod-3.3.7 ]
	then
	tar -xvf ../libmikmod-3.3.7.tar.gz
	fi

if [ ! -d libmad-0.15.1b ]
	then
	tar -xvf ../libmad-0.15.1b.tar.gz
	fi

cp ../libmad-0.15.1b.patched_configure.txt .


#############################Build libraries#####################

###########SDL2###########
cd SDL2-2.0.3
./configure --prefix=$InstallTo
if [ $? -eq 0 ]
then
  echo "[good]"
else
  errorofbuid
fi

make
if [ $? -eq 0 ]
then
  echo "[good]"
else
  errorofbuid
fi

make install
if [ $? -eq 0 ]
then
  echo "[good]"
else
  errorofbuid
fi
cd ..

###########OGG###########
cd libogg-1.3.2
./configure --prefix=$InstallTo
if [ $? -eq 0 ]
then
  echo "[good]"
else
  errorofbuid
fi

make

if [ $? -eq 0 ]
then
  echo "[good]"
else
  errorofbuid
fi

make install

if [ $? -eq 0 ]
then
  echo "[good]"
else
  errorofbuid
fi

cd ..

###########VORBIS###########
cd libvorbis-1.3.4
./configure --prefix=$InstallTo

	if [ $? -eq 0 ]
	then
	  echo "[good]"
	else
	  errorofbuid
	fi
make
	if [ $? -eq 0 ]
	then
	  echo "[good]"
	else
	  errorofbuid
	fi
make install
	if [ $? -eq 0 ]
	then
	  echo "[good]"
	else
	  errorofbuid
	fi
cd ..

###########FLAC###########
cd flac-1.3.0
./configure --enable-static=yes --disable-xmms-plugin --prefix=$InstallTo
	if [ $? -eq 0 ]
	then
	  echo "[good]"
	else
	  errorofbuid
	fi
make
	if [ $? -eq 0 ]
	then
	  echo "[good]"
	else
	  errorofbuid
	fi
make install
	if [ $? -eq 0 ]
	then
	  echo "[good]"
	else
	  errorofbuid
	fi
cd ..

###########MIKMOD###########
cd libmikmod-3.3.7
./configure --prefix=$InstallTo
	if [ $? -eq 0 ]
	then
	  echo "[good]"
	else
	  errorofbuid
	fi
make
	if [ $? -eq 0 ]
	then
	  echo "[good]"
	else
	  errorofbuid
	fi
make install
	if [ $? -eq 0 ]
	then
	  echo "[good]"
	else
	  errorofbuid
	fi
cd ..

###########LibMAD###########
cd libmad-0.15.1b
if [ ! -f ./configure_before_patch ]
then
	mv ./configure ./configure_before_patch
	cp ../libmad-0.15.1b.patched_configure.txt ./configure
	chmod u+x ./configure
fi

./configure --prefix=$InstallTo
	if [ $? -eq 0 ]
	then
	  echo "[good]"
	else
	  errorofbuid
	fi
make
	if [ $? -eq 0 ]
	then
	  echo "[good]"
	else
	  errorofbuid
	fi
make install
	if [ $? -eq 0 ]
	then
	  echo "[good]"
	else
	  errorofbuid
	fi
cd ..

###########SDL2_mixer###########
cd SDL2_mixer-2.0.0
./configure --enable-music-mp3 --enable-music-mp3-mad-gpl --disable-music-mp3-smpeg  --enable-music-midi --enable-music-midi-timidity --disable-music-midi-fluidsynth --disable-music-mod-modplug --enable-music-mod --enable-music-mod-mikmod --disable-music-mod-mikmod-shared --enable-music-flac=yes --disable-music-flac-shared --with-gnu-ld --disable-sdltest
	if [ $? -eq 0 ]
	then
	  echo "[good]"
	else
	  errorofbuid
	fi
make
	if [ $? -eq 0 ]
	then
	  echo "[good]"
	else
	  errorofbuid
	fi
make install
	if [ $? -eq 0 ]
	then
	  echo "[good]"
	else
	  errorofbuid
	fi
cd ..

#=======================================================================
echo Libraries installed into $InstallTo
printf "\n\n=========BUILT!!===========\n\n"
read -n 1
exit 0

