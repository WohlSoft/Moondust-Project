#!/bin/bash

if [[ "$InstallTo"=="" ]]; then
    InstallTo=$(echo ~+/../_builds/$OurOS)
    InstallTo=$(readlink $InstallTo)
fi

mkdir -p $InstallTo/lib
mkdir -p $InstallTo/include

instLib()
{
    cp -a ./SDL/$1/*.dylib $installTo/lib
    cp -a ./SDL/$1/*.a $installTo/lib
    cp -a ./SDL/$1/*.la* $installTo/lib
}

#install libOGG
mkdir -p $InstallTo/include/ogg
instLib libogg-1.3.2/src/.libs
cp -a ./SDL/libogg-1.3.2/include/ogg/*.h $InstallTo/include/ogg


#install libVorbis
mkdir -p $InstallTo/include/vorbis
instLib libvorbis-1.3.4/lib/.libs
cp -a ./SDL/libvorbis-1.3.4/include/vorbis/*.h $InstallTo/include/vorbis


#install FLAC
mkdir -p $InstallTo/include/FLAC
mkdir -p $InstallTo/include/FLAC++
cp -a ./SDL/flac-1.3.1/include/FLAC/*.h $InstallTo/include/FLAC
cp -a ./SDL/flac-1.3.1/include/FLAC++/*.h $InstallTo/include/FLAC++
instLib flac-1.3.1/src/libFLAC/.libs
instLib flac-1.3.1/src/libFLAC++/.libs

#install libModPlug
mkdir -p $InstallTo/include/libmodplug
instLib libmodplug-0.8.8.5/src/.libs
cp -a ./SDL/libmodplug-0.8.8.5/src/libmodplug/*.h $InstallTo/include/libmodplug
cp -a ./SDL/libmodplug-0.8.8.5/src/modplug.h $InstallTo/include/libmodplug

#install libMAD
instLib libmad-0.15.1b/.libs
cp -a ./SDL/libmad-0.15.1b/mad.h $InstallTo/include/


#Install LuaJIT
mkdir -p $InstallTo/include/luajit-2.0
cp -a ./SDL/uajit-2.0/src/lauxlib.h $InstallTo/include/luajit-2.0
cp -a ./SDL/uajit-2.0/src/lua.h $InstallTo/include/luajit-2.0
cp -a ./SDL/uajit-2.0/src/lua.hpp $InstallTo/include/luajit-2.0
cp -a ./SDL/uajit-2.0/src/luaconf.h $InstallTo/include/luajit-2.0
cp -a ./SDL/uajit-2.0/src/luajit.h $InstallTo/include/luajit-2.0
cp -a ./SDL/uajit-2.0/src/lualib.h $InstallTo/include/luajit-2.0
cp -a ./SDL/uajit-2.0/src/libluajit.a $InstallTo/lib/libluajit.a
cp -a ./SDL/uajit-2.0/src/libluajit.a $InstallTo/lib/libluajit-5.1.a

#install SDL
cd SDL/SDL-dbcbdc2940ef

bash build-scripts/updaterev.sh
bash build-scripts/mkinstalldirs $InstallTo/bin
ginstall -c -m 755 sdl2-config $InstallTo/bin/sdl2-config
bash build-scripts/mkinstalldirs $InstallTo/include/SDL2
mkdir -p -- $InstallTo/include/SDL2
for file in SDL.h SDL_assert.h SDL_atomic.h SDL_audio.h SDL_bits.h SDL_blendmode.h SDL_clipboard.h SDL_cpuinfo.h SDL_egl.h SDL_endian.h SDL_error.h SDL_events.h SDL_filesystem.h SDL_gamecontroller.h SDL_gesture.h SDL_haptic.h SDL_hints.h SDL_joystick.h SDL_keyboard.h SDL_keycode.h SDL_loadso.h SDL_log.h SDL_main.h SDL_messagebox.h SDL_mouse.h SDL_mutex.h SDL_name.h SDL_opengl.h SDL_opengl_glext.h SDL_opengles.h SDL_opengles2_gl2ext.h SDL_opengles2_gl2.h SDL_opengles2_gl2platform.h SDL_opengles2.h SDL_opengles2_khrplatform.h SDL_pixels.h SDL_platform.h SDL_power.h SDL_quit.h SDL_rect.h SDL_render.h SDL_rwops.h SDL_scancode.h SDL_shape.h SDL_stdinc.h SDL_surface.h SDL_system.h SDL_syswm.h SDL_thread.h SDL_timer.h SDL_touch.h SDL_types.h SDL_version.h SDL_video.h begin_code.h close_code.h SDL_test.h SDL_test_assert.h SDL_test_common.h SDL_test_compare.h SDL_test_crc32.h SDL_test_font.h SDL_test_fuzzer.h SDL_test_harness.h SDL_test_images.h SDL_test_log.h SDL_test_md5.h SDL_test_random.h; do \
    ginstall -c -m 644 ./include/$file $InstallTo/include/SDL2/$file; \
done
ginstall -c -m 644 include/SDL_config.h $InstallTo/include/SDL2/SDL_config.h
if test -f include/SDL_revision.h; then \
	    ginstall -c -m 644 ./include/SDL_revision.h $InstallTo/include/SDL2/SDL_revision.h; \
	else \
	    ginstall -c -m 644 ./include/SDL_revision.h $InstallTo/include/SDL2/SDL_revision.h; \
	fi
bash build-scripts/mkinstalldirs $InstallTo/lib
mkdir -p -- $InstallTo/lib
bash ./libtool --quiet --mode=install ginstall -c build/libSDL2.la $InstallTo/lib/libSDL2.la
ginstall -c -m 644 build/libSDL2main.a $InstallTo/lib/libSDL2main.a
ranlib $InstallTo/lib/libSDL2main.a
ginstall -c -m 644 build/libSDL2_test.a $InstallTo/lib/libSDL2_test.a
ranlib $InstallTo/lib/libSDL2_test.a
bash build-scripts/mkinstalldirs $InstallTo/share/aclocal
mkdir -p -- $InstallTo/share/aclocal

ginstall -c -m 644 sdl2.m4 $InstallTo/share/aclocal/sdl2.m4
bash build-scripts/mkinstalldirs $InstallTo/lib/pkgconfig
mkdir -p -- $InstallTo/lib/pkgconfig
ginstall -c -m 644 sdl2.pc $InstallTo/lib/pkgconfig
bash build-scripts/mkinstalldirs $InstallTo/lib/cmake/SDL2
mkdir -p -- $InstallTo/lib/cmake/SDL2
ginstall -c -m 644 sdl2-config.cmake $InstallTo/lib/cmake/SDL2

cd ../..

