#-------------------------------------------------
#
# Project created by QtCreator 2015-03-06T14:10:07
#
#-------------------------------------------------

TARGET = oolua
TEMPLATE = lib
CONFIG += staticlib
CONFIG -= qt
CONFIG += C++11

DEFINES += LIBOOLUA

QMAKE_CXXFLAGS += -Wno-maybe-uninitialized -Wstrict-aliasing=0 -Wno-unused-local-typedefs
macx:QMAKE_CXXFLAGS += -Wno-header-guard -stdlib=libstdc++

macx||linux-g++:{
DEFINES += LUA_USE_MKSTEMP
}
macx: DEFINES += OOLUA_USE_EXCEPTIONS=1 OOLUA_STORE_LAST_ERROR=0 MAC_BUILD

DEFINES += OOLUA_STD_STRING_IS_INTEGRAL=1

include (../../../_common/lib_destdir.pri)
DESTDIR = ../_builds/$$TARGETOS/lib

include(../../../_common/build_props.pri)

SOURCES += \
    ../class_from_stack.cpp \
    ../oolua.cpp \
    ../oolua_check_result.cpp \
    ../oolua_chunk.cpp \
    ../oolua_error.cpp \
    ../oolua_exception.cpp \
    ../oolua_function.cpp \
    ../oolua_helpers.cpp \
    ../oolua_open.cpp \
    ../oolua_pull.cpp \
    ../oolua_push.cpp \
    ../oolua_ref.cpp \
    ../oolua_registration.cpp \
    ../oolua_script.cpp \
    ../oolua_stack_dump.cpp \
    ../oolua_string.cpp \
    ../oolua_table.cpp \
    ../proxy_storage.cpp \
    ../push_pointer_internal.cpp \
    ../stack_get.cpp \
    ../lua/lapi.c \
    ../lua/lauxlib.c \
    ../lua/lbaselib.c \
    ../lua/lbitlib.c \
    ../lua/lcode.c \
    ../lua/lcorolib.c \
    ../lua/lctype.c \
    ../lua/ldblib.c \
    ../lua/ldebug.c \
    ../lua/ldo.c \
    ../lua/ldump.c \
    ../lua/lfunc.c \
    ../lua/lgc.c \
    ../lua/linit.c \
    ../lua/liolib.c \
    ../lua/llex.c \
    ../lua/lmathlib.c \
    ../lua/lmem.c \
    ../lua/loadlib.c \
    ../lua/lobject.c \
    ../lua/lopcodes.c \
    ../lua/loslib.c \
    ../lua/lparser.c \
    ../lua/lstate.c \
    ../lua/lstring.c \
    ../lua/lstrlib.c \
    ../lua/ltable.c \
    ../lua/ltablib.c \
    ../lua/ltm.c \
    ../lua/lua.c \
    ../lua/luac.c \
    ../lua/lundump.c \
    ../lua/lvm.c \
    ../lua/lzio.c

HEADERS += \
    ../char_arrays.h \
    ../class_from_stack.h \
    ../default_trait_caller.h \
    ../dsl_va_args.h \
    ../lua_includes.h \
    ../lvd_types.h \
    ../lvd_type_traits.h \
    ../oolua.h \
    ../oolua_boilerplate.h \
    ../oolua_check_result.h \
    ../oolua_chunk.h \
    ../oolua_config.h \
    ../oolua_dsl.h \
    ../oolua_dsl_export.h \
    ../oolua_error.h \
    ../oolua_exception.h \
    ../oolua_function.h \
    ../oolua_helpers.h \
    ../oolua_open.h \
    ../oolua_pull.h \
    ../oolua_push.h \
    ../oolua_ref.h \
    ../oolua_registration.h \
    ../oolua_registration_fwd.h \
    ../oolua_script.h \
    ../oolua_stack.h \
    ../oolua_stack_dump.h \
    ../oolua_stack_fwd.h \
    ../oolua_string.h \
    ../oolua_table.h \
    ../oolua_traits.h \
    ../oolua_traits_fwd.h \
    ../oolua_version.h \
    ../platform_check.h \
    ../proxy_base_checker.h \
    ../proxy_caller.h \
    ../proxy_class.h \
    ../proxy_constructor.h \
    ../proxy_constructor_param_tester.h \
    ../proxy_function_dispatch.h \
    ../proxy_function_exports.h \
    ../proxy_member_function.h \
    ../proxy_none_member_function.h \
    ../proxy_operators.h \
    ../proxy_public_member.h \
    ../proxy_stack_helper.h \
    ../proxy_storage.h \
    ../proxy_tag_info.h \
    ../proxy_tags.h \
    ../proxy_test.h \
    ../proxy_userdata.h \
    ../push_pointer_internal.h \
    ../stack_get.h \
    ../type_converters.h \
    ../type_list.h \
    ../typelist_structs.h \
    ../lua/lapi.h \
    ../lua/lauxlib.h \
    ../lua/lcode.h \
    ../lua/lctype.h \
    ../lua/ldebug.h \
    ../lua/ldo.h \
    ../lua/lfunc.h \
    ../lua/lgc.h \
    ../lua/llex.h \
    ../lua/llimits.h \
    ../lua/lmem.h \
    ../lua/lobject.h \
    ../lua/lopcodes.h \
    ../lua/lparser.h \
    ../lua/lstate.h \
    ../lua/lstring.h \
    ../lua/ltable.h \
    ../lua/ltm.h \
    ../lua/lua.h \
    ../lua/lua.hpp \
    ../lua/luaconf.h \
    ../lua/lualib.h \
    ../lua/lundump.h \
    ../lua/lvm.h \
    ../lua/lzio.h

