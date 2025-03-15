cmake_minimum_required(VERSION 3.5)
project(spc2it)

set(spc2it_sources
	src/emu.c
	src/it.c
	src/sound.c
	src/spc700.c
	src/lib.c

	src/emu.h
	src/it.h
	src/sneese_spc.h
	src/sound.h
	src/spc2ittypes.h
)

add_library(libspc2it STATIC ${spc2it_sources})
target_include_directories(libspc2it PRIVATE src)
target_include_directories(libspc2it PUBLIC include)
target_link_libraries(libspc2it m)

add_executable(spc2it_cli src/main.c)
target_link_libraries(spc2it_cli libspc2it)
set_target_properties(spc2it_cli PROPERTIES OUTPUT_NAME "spc2it")

