cmake_minimum_required (VERSION 2.8.12...3.10)
project(Unix2Dos C)

# Disable bogus MSVC warnings
if(MSVC)
    add_definitions(-D_CRT_SECURE_NO_WARNINGS -D_CRT_NONSTDC_NO_WARNINGS)
endif()

add_definitions(
	-DD2U_UNIFILE
	-DD2U_UNICODE
	-D_O_U16TEXT=0x20000
	-DVER_REVISION="7.4.0"
	-DVER_DATE="2017-10-10"
)

add_executable(unix2dos_custom
	src/unix2dos.c
	src/common.c
	src/querycp.c
)

install(TARGETS unix2dos_custom
    RUNTIME DESTINATION "bin/"
)
