cmake_minimum_required(VERSION 3.2...3.5)

project(mmutil VERSION 1.10.2 LANGUAGES C)

set(LIBMMUTIL_SRC
	source/adpcm.c
	source/adpcm.h
	source/defs.h
	source/deftypes.h
	source/errors.h
	source/files.c
	source/files.h
	source/gba.c
	source/gba.h
	source/it.c
	source/it.h
	source/kiwi.c
	source/mas.c
	source/mas.h
	source/mod.c
	source/mod.h
	source/msl.c
	source/nds.c
	source/nds.h
	source/poopen.h
	source/s3m.c
	source/s3m.h
	source/samplefix.c
	source/samplefix.h
	source/simple.c
	source/simple.h
	source/solution.h
	source/systems.h
	# source/upload.c
	source/version.h
	source/wav.c
	source/wav.h
	source/xm.c
	source/xm.h
	source/arm7_st.inc
	source/arm9_st.inc

	public/mmutil.h
)

add_library(libmmutil "${LIBMMUTIL_SRC}")
target_include_directories(libmmutil PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/public")
target_link_libraries(libmmutil PUBLIC m)
set_target_properties(libmmutil PROPERTIES OUTPUT_NAME "mmutil")

add_executable(mmutil source/main.c)
target_compile_definitions(mmutil PRIVATE "-DPACKAGE_VERSION=\"${CMAKE_PROJECT_VERSION}\"")
target_link_libraries(mmutil PRIVATE libmmutil)

if(NOT MSVC)
	target_compile_options(libmmutil PRIVATE -Wall -Wextra -Wno-multichar)
	target_compile_options(mmutil PRIVATE -Wall -Wextra -Wno-multichar)
endif()
