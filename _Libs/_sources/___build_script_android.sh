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

if [ ! -d SDL_android ] 
	then
	mkdir SDL_android
	fi

cd SDL_android

UnArch()
{
# $1 - archive name
if [ ! -d $1 ]
	then
	tar -xvf ../$1.tar.*z
	fi
}

BuildSrc()
{
	# $1 - dir name   #2 additional props

	cd $1
	./configure $2
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
}

BuildCnfOnly()
{
	# $1 - dir name   #2 additional props
	cd $1
	./configure $2
	if [ $? -eq 0 ]
	then
	  echo "[good]"
	else
	  errorofbuid
	fi
}

BuildMakeOnly()
{
	make
	if [ $? -eq 0 ]
	then
	  echo "[good]"
	else
	  errorofbuid
	fi
	cd ..
}


BuildSrcOnly()
{
	# $1 - dir name   #2 additional props
	cd $1
	./configure $2
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
	cd ..
}
LatestSDL='SDL-2.0.4-9723'
UnArch $LatestSDL
#UnArch 'SDL2_mixer-2.0.0'
UnArch 'libogg-1.3.2'
UnArch 'libvorbis-1.3.4'
UnArch 'tremor-android' # Tremor is a fixed-point Vorbis implementation for embeded devices
UnArch 'flac-1.3.1'
#UnArch 'flac-android-4.2.1_r1.2-libFLAC'
UnArch 'libmikmod-3.3.7'
UnArch 'libmodplug-0.8.8.5'
UnArch 'libmad-0.15.1b'
UnArch 'luajit-2.0-fromgit'
cp ../libmad-0.15.1b.patched_configure.txt .
#UnArch "SDL2_ttf-2.0.12"

#############################Build libraries#####################

buildSDL()
{
	###########SDL2###########
	echo "=======SDL2========="
	#BuildSrc $LatestSDL $SDL_ARGS'--prefix='$InstallTo
	cd $LatestSDL
	echo $PWD
	ndk-build NDK_PROJECT_PATH=. APP_BUILD_SCRIPT=./Android.mk APP_PLATFORM=$ANDR_PLTF

	if [ $? -eq 0 ]
	then
	  echo "[good]"
	else
	  errorofbuid
	fi

	mkdir -p $InstallTo/include/SDL2/
	cp include/* $InstallTo/include/SDL2/
	cp obj/local/armeabi/libSDL2.so $InstallTo/lib/
	cd ..
	#apply fix of SDL2 bug
	cp ../SDL_platform.h $InstallTo/include/SDL_platform.h
}

buildOgg()
{
	###########OGG###########
	echo "=========OGG==========="
	export LIBS="-lc "
	ln -s $AN_ROOT/platforms/$ANDR_PLTF/arch-arm/usr/lib/crtbegin_so.o 'libogg-1.3.2/src'
	ln -s $AN_ROOT/platforms/$ANDR_PLTF/arch-arm/usr/lib/crtend_so.o 'libogg-1.3.2/src'
	sed  -i 's/-version-info [^ ]\+/-avoid-version /g' 'libogg-1.3.2/src/Makefile.am'
	sed  -i 's/-version-info [^ ]\+/-avoid-version /g' 'libogg-1.3.2/src/Makefile.in'
	BuildCnfOnly 'libogg-1.3.2' $CONFIGURE_ARGS'--prefix='$InstallTo' CFLAGS=-march=armv7-a -mfloat-abi=hard -mfpu=vfpv3-d16'
	sed  -i 's/-version-info [^ ]\+/-avoid-version /g' src/Makefile
	BuildMakeOnly
	cp -a 'libogg-1.3.2/src/.libs/'*.so* $InstallTo"/lib"
	cp -a 'libogg-1.3.2/src/.libs/'*.a $InstallTo"/lib"
	mkdir -p $InstallTo"/include/ogg"
	cp -a 'libogg-1.3.2/include/ogg/'*.h $InstallTo"/include/ogg"
}
buildVorbis()
{

	###########VORBIS###########
	echo "============VORBIS================"
	export LIBS="-lc -lgcc -logg "
	ln -s $AN_ROOT/platforms/$ANDR_PLTF/arch-arm/usr/lib/crtbegin_so.o 'libvorbis-1.3.4/lib'
	ln -s $AN_ROOT/platforms/$ANDR_PLTF/arch-arm/usr/lib/crtend_so.o 'libvorbis-1.3.4/lib'
	BuildCnfOnly 'libvorbis-1.3.4' $CONFIGURE_ARGS'--prefix='$InstallTo' --enable-docs=no CFLAGS="-march=armv7-a -mfloat-abi=hard -mfpu=vfpv3-d16"'
	sed  -i 's/-version-info [^ ]\+/-avoid-version /g' lib/Makefile
	BuildMakeOnly
	cp -a 'libvorbis-1.3.4/lib/.libs/'*.so* $InstallTo"/lib"
	cp -a 'libvorbis-1.3.4/lib/.libs/'*.a $InstallTo"/lib"
	mkdir -p $InstallTo"/include/vorbis"
	cp -a 'libvorbis-1.3.4/include/vorbis/'*.h $InstallTo"/include/vorbis"
}
buildTremor()
{
	###########VORBIS#TREMOR##########
	echo "============VORBIS=TREMOR========="
	export LIBS="-lc -logg "
	cd 'Tremor-android'
	echo $PWD
	ndk-build NDK_PROJECT_PATH=. APP_BUILD_SCRIPT=./Android.mk APP_PLATFORM=$ANDR_PLTF
	if [ $? -eq 0 ]
	then
	  echo "[good]"
	else
	  errorofbuid
	fi
	mkdir -p $InstallTo/include/tremor/
	cp Tremor/*.h $InstallTo/include/tremor/
	cp obj/local/armeabi/libvorbisidec.so $InstallTo/lib/
	cd ..
}

buildFlac()
{
	###########FLAC###########
	#//////////// Don't wanna be built, because:
    #  stream_encoder.c:2930:3: error: aggregate value used where an integer was expected
    #   b[0] = (FLAC__byte)page.body[total_samples_byte_offset] & 0xF0;
	echo "==========FLAC========="
	export LIBS="-lc -lgcc -logg "
	export LDFLAGS=$CORE_LDFLAGS" "$CMN_LDFLAGS
	cd "flac-1.3.1"
	./autogen.sh
	./configure $CONFIGURE_ARGS'--enable-shared=off --disable-xmms-plugin --prefix='$InstallTo' CFLAGS="-fPIC -mthumb -Wno-psabi -march=armv7-a -mfloat-abi=softfp"'
	cd src/libFLAC
	make
	if [ $? -eq 0 ]
	then
	  echo "[good]"
	else
	  errorofbuid
	fi

	cd ../../..
	#cp -a 'libogg-1.3.2/src/.libs/'*.so $InstallTo"/lib"
	#cp -a 'libogg-1.3.2/src/.libs/'*.a $InstallTo"/lib"
	#mkdir -p $InstallTo"/include/ogg"
	#cp -a 'libogg-1.3.2/include/ogg/'*.h $InstallTo"/include/ogg"
	#BuildSrc 'flac-1.3.0' '--disable-xmms-plugin --prefix='$InstallTo
}

buildFlac2()
{
	###########FLAC-2##########
	echo "============FLAC-2========="
	export LIBS="-lc -logg "
	cd 'flac-android-4.2.1_r1.2-libFLAC'
	echo $PWD
	#./autogen.sh
	#./configure $CONFIGURE_ARGS'--enable-shared=off --disable-xmms-plugin --prefix='$InstallTo' CFLAGS="-fPIC -mthumb -Wno-psabi -march=armv7-a -mfloat-abi=softfp"'
	ndk-build NDK_PROJECT_PATH=. APP_BUILD_SCRIPT=./Android.mk APP_PLATFORM=$ANDR_PLTF LOCAL_C_INCLUDES+=$InstallTo/include LOCAL_C_INCLUDES+=$PWD/include
	if [ $? -eq 0 ]
	then
	  echo "[good]"
	else
	  errorofbuid
	fi
	#mkdir -p $InstallTo/include/ogg/
	#cp Tremor/*.h $InstallTo/include/ogg/
	#cp obj/local/armeabi/libvorbisidec.so $InstallTo/lib/
	#cd ..
}

buildModPlug()
{
	###########MODPLUG###########
	echo "==========MODPLUG=========="
	export LIBS="-lc -lgcc "
	ln -s $AN_ROOT/platforms/$ANDR_PLTF/arch-arm/usr/lib/crtbegin_so.o 'libmodplug-0.8.8.5/src'
	ln -s $AN_ROOT/platforms/$ANDR_PLTF/arch-arm/usr/lib/crtend_so.o 'libmodplug-0.8.8.5/src'
	cp ../autogen.sh_for_libmad.txt 'libmodplug-0.8.8.5/autogen.sh'
	cd 'libmodplug-0.8.8.5'
	chmod u+x ./autogen.sh
	./autogen.sh
	cd ..
	#BuildSrc 'libmodplug-0.8.8.5' '--prefix='$InstallTo
	sed -i 's/-version-info \$(MODPLUG_LIBRARY_VERSION)/-avoid-version/g' 'libmodplug-0.8.8.5/src/Makefile.am'
	sed -i 's/-version-info \$(MODPLUG_LIBRARY_VERSION)/-avoid-version/g' 'libmodplug-0.8.8.5/src/Makefile.in'
	BuildCnfOnly 'libmodplug-0.8.8.5' $CONFIGURE_ARGS' CC='$CC' CXX='$CXX' CXXFLAGS="'$CPPFLAGS' -DHAVE_SETENV=1" CFLAGS='$CFLAGS' LDFLAGS='$LDFLAGS' LIBS='$LIBS' -DHAVE_SETENV=0 --prefix='$InstallTo
    sed -i 's/\/\* #undef HAVE_SETENV \*\//#define HAVE_SETENV  1/g' src/config.h
    sed -i 's/\/\* #undef HAVE_SINF \*\//#define HAVE_SINF  1/g' src/config.h
	BuildMakeOnly
	mkdir -p $InstallTo"/include/libmodplug"
	cp -a 'libmodplug-0.8.8.5/src/.libs/'*.so* $InstallTo"/lib"
	cp -a 'libmodplug-0.8.8.5/src/'*.h $InstallTo"/include/libmodplug"
}

buildMAD()
{
	###########LibMAD###########
	echo "==========LibMAD============"
	export LIBS="-lc "
	cd libmad-0.15.1b
	if [ ! -f ./configure_before_patch ]
	then
		mv ./configure ./configure_before_patch
		cp ../libmad-0.15.1b.patched_configure.txt ./configure
		cp ../../autogen.sh_for_libmad.txt ./autogen.sh
		chmod u+x ./autogen.sh
		chmod u+x ./configure
	fi
	./autogen.sh
	cd ..
	ln -s $AN_ROOT/platforms/$ANDR_PLTF/arch-arm/usr/lib/crtbegin_so.o 'libmad-0.15.1b'
	ln -s $AN_ROOT/platforms/$ANDR_PLTF/arch-arm/usr/lib/crtend_so.o 'libmad-0.15.1b'
	sed -i 's/-version-info \$(version_info)/-avoid-version/g' 'libmad-0.15.1b/Makefile.am'
	sed -i 's/-version-info \$(version_info)/-avoid-version/g' 'libmad-0.15.1b/Makefile.in'
	BuildCnfOnly 'libmad-0.15.1b' $CONFIGURE_ARGS'--prefix='$InstallTo' CFLAGS=-march=armv7-a -mfloat-abi=hard -mfpu=vfpv3-d16'
	BuildMakeOnly
	cp -a 'libmad-0.15.1b/.libs/'*.a* $InstallTo"/lib"
	cp -a 'libmad-0.15.1b/.libs/'*.so* $InstallTo"/lib"
	cp -a 'libmad-0.15.1b/'mad.h $InstallTo"/include/"
}

buildLuaJit()
{
	NDKABI=18
	NDKP=$PREBUILT/prebuilt/linux-x86_64/bin/arm-linux-androideabi-
	NDKF="--sysroot "$SYSROOT
	NDKARCH="-march=armv7-a -mfloat-abi=softfp -Wl,--fix-cortex-a8"
	###########LuaJIT###########
	echo "==========LuaJIT============"
	cd luajit-2.0
	make HOST_CC="gcc -m32" CROSS=$NDKP TARGET_FLAGS="$NDKF $NDKARCH" PREFIX=$InstallTo BUILDMODE=static
	if [ $? -eq 0 ]
	then
	  echo "[good]"
	else
	  errorofbuid
	fi
	make install PREFIX=$InstallTo BUILDMODE=static
	if [ $? -eq 0 ]
	then
	  echo "[good]"
	else
	  errorofbuid
	fi
	cd ..
}


#BuildSrc 'libmad-0.15.1b' '--prefix='$InstallTo

buildSDL
buildOgg
buildVorbis
buildTremor
#buildFlac # don't wanna :P
buildModPlug
buildMAD
#buildLuaJit

#Copy header of SDL Mixer into target path
cp ../../SDL2_mixer_modified/SDL_mixer.h $InstallTo/include/SDL2/

###########MIKMOD###########
#echo "=============MIKMOD=========="
#BuildSrc 'libmikmod-3.3.7' '--prefix='$InstallTo



#=======================================================================
echo Libraries installed into $InstallTo


