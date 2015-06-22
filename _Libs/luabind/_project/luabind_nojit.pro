
#QMAKE_CXXFLAGS += -Wno-unused-local-typedefs -Wno-unused-variable -Wno-unused-parameter -Wno-write-strings -Wno-ignored-qualifiers -Wno-deprecated-declarations

TARGET = luabind
TEMPLATE = lib
CONFIG += staticlib
CONFIG -= qt
CONFIG += c++11

DEFINES += _LIBLUABIND

unix:{
DEFINES += LUA_USE_LINUX
}

message("Luabind will be built with regular LUA 5.1!")

include (../../../_common/lib_destdir.pri)
DESTDIR = ../../_builds/$$TARGETOS/lib

include(../../../_common/build_props.pri)

INCLUDEPATH += $$PWD/../ $$PWD/../lua/

SOURCES +=  ../lua/lapi.c \
    ../lua/lauxlib.c \
    ../lua/lbaselib.c \
    ../lua/lcode.c \
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
    ../lua/lundump.c \
    ../lua/lvm.c \
    ../lua/lzio.c \
    ../lua/print.c \
    ../src/class.cpp \
    ../src/class_info.cpp \
    ../src/class_registry.cpp \
    ../src/class_rep.cpp \
    ../src/create_class.cpp \
    ../src/error.cpp \
    ../src/exception_handler.cpp \
    ../src/function.cpp \
    ../src/function_introspection.cpp \
    ../src/headertest.cpp \
    ../src/inheritance.cpp \
    ../src/link_compatibility.cpp \
    ../src/object_rep.cpp \
    ../src/open.cpp \
    ../src/operator.cpp \
    ../src/pcall.cpp \
    ../src/scope.cpp \
    ../src/set_package_preload.cpp \
    ../src/stack_content_by_name.cpp \
    ../src/weak_ref.cpp \
    ../src/wrapper_base.cpp

HEADERS += ../lua/lapi.h \
    ../lua/lauxlib.h \
    ../lua/lcode.h \
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
    ../lua/luaconf.h \
    ../lua/lualib.h \
    ../lua/lundump.h \
    ../lua/lvm.h \
    ../lua/lzio.h \
    ../luabind/detail/conversion_policies/conversion_base.hpp \
    ../luabind/detail/conversion_policies/conversion_policies.hpp \
    ../luabind/detail/conversion_policies/enum_converter.hpp \
    ../luabind/detail/conversion_policies/function_converter.hpp \
    ../luabind/detail/conversion_policies/lua_proxy_converter.hpp \
    ../luabind/detail/conversion_policies/native_converter.hpp \
    ../luabind/detail/conversion_policies/pointer_converter.hpp \
    ../luabind/detail/conversion_policies/reference_converter.hpp \
    ../luabind/detail/conversion_policies/value_converter.hpp \
    ../luabind/detail/call.hpp \
    ../luabind/detail/call_function.hpp \
    ../luabind/detail/call_member.hpp \
    ../luabind/detail/call_shared.hpp \
    ../luabind/detail/call_traits.hpp \
    ../luabind/detail/class_registry.hpp \
    ../luabind/detail/class_rep.hpp \
    ../luabind/detail/constructor.hpp \
    ../luabind/detail/conversion_policies.hpp \
    ../luabind/detail/conversion_storage.hpp \
    ../luabind/detail/convert_to_lua.hpp \
    ../luabind/detail/crtp_iterator.hpp \
    ../luabind/detail/debug.hpp \
    ../luabind/detail/decorate_type.hpp \
    ../luabind/detail/deduce_signature.hpp \
    ../luabind/detail/enum_maker.hpp \
    ../luabind/detail/format_signature.hpp \
    ../luabind/detail/garbage_collector.hpp \
    ../luabind/detail/has_get_pointer.hpp \
    ../luabind/detail/inheritance.hpp \
    ../luabind/detail/instance_holder.hpp \
    ../luabind/detail/link_compatibility.hpp \
    ../luabind/detail/make_instance.hpp \
    ../luabind/detail/meta.hpp \
    ../luabind/detail/most_derived.hpp \
    ../luabind/detail/object.hpp \
    ../luabind/detail/object_rep.hpp \
    ../luabind/detail/open.hpp \
    ../luabind/detail/operator_id.hpp \
    ../luabind/detail/other.hpp \
    ../luabind/detail/pcall.hpp \
    ../luabind/detail/pointee_sizeof.hpp \
    ../luabind/detail/pointee_typeid.hpp \
    ../luabind/detail/policy.hpp \
    ../luabind/detail/primitives.hpp \
    ../luabind/detail/property.hpp \
    ../luabind/detail/ref.hpp \
    ../luabind/detail/signature_match.hpp \
    ../luabind/detail/stack_utils.hpp \
    ../luabind/detail/typetraits.hpp \
    ../luabind/detail/yes_no.hpp \
    ../luabind/adopt_policy.hpp \
    ../luabind/back_reference.hpp \
    ../luabind/back_reference_fwd.hpp \
    ../luabind/class.hpp \
    ../luabind/class_info.hpp \
    ../luabind/config.hpp \
    ../luabind/container_policy.hpp \
    ../luabind/copy_policy.hpp \
    ../luabind/dependency_policy.hpp \
    ../luabind/discard_result_policy.hpp \
    ../luabind/error.hpp \
    ../luabind/error_callback_fun.hpp \
    ../luabind/exception_handler.hpp \
    ../luabind/from_stack.hpp \
    ../luabind/function.hpp \
    ../luabind/function_introspection.hpp \
    ../luabind/get_main_thread.hpp \
    ../luabind/handle.hpp \
    ../luabind/iterator_policy.hpp \
    ../luabind/lua_argument_proxy.hpp \
    ../luabind/lua_include.hpp \
    ../luabind/lua_index_proxy.hpp \
    ../luabind/lua_iterator_proxy.hpp \
    ../luabind/lua_proxy.hpp \
    ../luabind/lua_proxy_interface.hpp \
    ../luabind/lua_state_fwd.hpp \
    ../luabind/luabind.hpp \
    ../luabind/make_function.hpp \
    ../luabind/nil.hpp \
    ../luabind/no_dependency.hpp \
    ../luabind/object.hpp \
    ../luabind/open.hpp \
    ../luabind/operator.hpp \
    ../luabind/out_value_policy.hpp \
    ../luabind/pointer_traits.hpp \
    ../luabind/prefix.hpp \
    ../luabind/raw_policy.hpp \
    ../luabind/return_reference_to_policy.hpp \
    ../luabind/scope.hpp \
    ../luabind/set_package_preload.hpp \
    ../luabind/shared_ptr_converter.hpp \
    ../luabind/tag_function.hpp \
    ../luabind/typeid.hpp \
    ../luabind/version.hpp \
    ../luabind/weak_ref.hpp \
    ../luabind/wrapper_base.hpp \
    ../luabind/yield_policy.hpp \
    ../lua_inclues/lua.hpp

