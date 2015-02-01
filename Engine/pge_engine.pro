#-------------------------------------------------
#
# Project created by QtCreator 2014-09-05T22:51:01
#
#-------------------------------------------------

QT += core gui opengl network
#QT += widgets

QMAKE_CXXFLAGS += -Wno-maybe-uninitialized -Wstrict-aliasing=0 -Wno-unused-local-typedefs

DESTDIR = ../bin

static: {
release:OBJECTS_DIR = ../bin/_build/engine/_release/.obj
release:MOC_DIR     = ../bin/_build/engine/_release/.moc
release:RCC_DIR     = ../bin/_build/engine/_release/.rcc
release:UI_DIR      = ../bin/_build/engine/_release/.ui

debug:OBJECTS_DIR   = ../bin/_build/engine/_debug/.obj
debug:MOC_DIR       = ../bin/_build/engine/_debug/.moc
debug:RCC_DIR       = ../bin/_build/engine/_debug/.rcc
debug:UI_DIR        = ../bin/_build/engine/_debug/.ui
} else {
release:OBJECTS_DIR = ../bin/_build/_dynamic/engine/_release/.obj
release:MOC_DIR     = ../bin/_build/_dynamic/engine/_release/.moc
release:RCC_DIR     = ../bin/_build/_dynamic/engine/_release/.rcc
release:UI_DIR      = ../bin/_build/_dynamic/engine/_release/.ui

debug:OBJECTS_DIR   = ../bin/_build/_dynamic/engine/_debug/.obj
debug:MOC_DIR       = ../bin/_build/_dynamic/engine/_debug/.moc
debug:RCC_DIR       = ../bin/_build/_dynamic/engine/_debug/.rcc
debug:UI_DIR        = ../bin/_build/_dynamic/engine/_debug/.ui
}

TARGET = pge_engine
TEMPLATE = app
CONFIG += c++11

CONFIG += static
CONFIG += thread

DEFINES += PGE_ENGINE

unix: DEFINES += LUA_USE_MKSTEMP

INCLUDEPATH += "../_Libs/" "../Editor/file_formats"

win32: {
    LIBS += -L../_Libs/_builds/win32/lib
    INCLUDEPATH += ../_Libs/_builds/win32/include
}

LIBS += -lSDL2
win32: LIBS += -lSDL2main
win32: LIBS += libversion
unix:  LIBS += -lglut -lGLU

RC_FILE = _resources/engine.rc

SOURCES += main.cpp \
    ../_Libs/Box2D/Collision/Shapes/b2ChainShape.cpp \
    ../_Libs/Box2D/Collision/Shapes/b2CircleShape.cpp \
    ../_Libs/Box2D/Collision/Shapes/b2EdgeShape.cpp \
    ../_Libs/Box2D/Collision/Shapes/b2PolygonShape.cpp \
    ../_Libs/Box2D/Collision/b2BroadPhase.cpp \
    ../_Libs/Box2D/Collision/b2CollideCircle.cpp \
    ../_Libs/Box2D/Collision/b2CollideEdge.cpp \
    ../_Libs/Box2D/Collision/b2CollidePolygon.cpp \
    ../_Libs/Box2D/Collision/b2Collision.cpp \
    ../_Libs/Box2D/Collision/b2Distance.cpp \
    ../_Libs/Box2D/Collision/b2DynamicTree.cpp \
    ../_Libs/Box2D/Collision/b2TimeOfImpact.cpp \
    ../_Libs/Box2D/Common/b2BlockAllocator.cpp \
    ../_Libs/Box2D/Common/b2Draw.cpp \
    ../_Libs/Box2D/Common/b2Math.cpp \
    ../_Libs/Box2D/Common/b2Settings.cpp \
    ../_Libs/Box2D/Common/b2StackAllocator.cpp \
    ../_Libs/Box2D/Common/b2Timer.cpp \
    ../_Libs/Box2D/Dynamics/Contacts/b2ChainAndCircleContact.cpp \
    ../_Libs/Box2D/Dynamics/Contacts/b2ChainAndPolygonContact.cpp \
    ../_Libs/Box2D/Dynamics/Contacts/b2CircleContact.cpp \
    ../_Libs/Box2D/Dynamics/Contacts/b2Contact.cpp \
    ../_Libs/Box2D/Dynamics/Contacts/b2ContactSolver.cpp \
    ../_Libs/Box2D/Dynamics/Contacts/b2EdgeAndCircleContact.cpp \
    ../_Libs/Box2D/Dynamics/Contacts/b2EdgeAndPolygonContact.cpp \
    ../_Libs/Box2D/Dynamics/Contacts/b2PolygonAndCircleContact.cpp \
    ../_Libs/Box2D/Dynamics/Contacts/b2PolygonContact.cpp \
    ../_Libs/Box2D/Dynamics/Joints/b2DistanceJoint.cpp \
    ../_Libs/Box2D/Dynamics/Joints/b2FrictionJoint.cpp \
    ../_Libs/Box2D/Dynamics/Joints/b2GearJoint.cpp \
    ../_Libs/Box2D/Dynamics/Joints/b2Joint.cpp \
    ../_Libs/Box2D/Dynamics/Joints/b2MotorJoint.cpp \
    ../_Libs/Box2D/Dynamics/Joints/b2MouseJoint.cpp \
    ../_Libs/Box2D/Dynamics/Joints/b2PrismaticJoint.cpp \
    ../_Libs/Box2D/Dynamics/Joints/b2PulleyJoint.cpp \
    ../_Libs/Box2D/Dynamics/Joints/b2RevoluteJoint.cpp \
    ../_Libs/Box2D/Dynamics/Joints/b2RopeJoint.cpp \
    ../_Libs/Box2D/Dynamics/Joints/b2WeldJoint.cpp \
    ../_Libs/Box2D/Dynamics/Joints/b2WheelJoint.cpp \
    ../_Libs/Box2D/Dynamics/b2Body.cpp \
    ../_Libs/Box2D/Dynamics/b2ContactManager.cpp \
    ../_Libs/Box2D/Dynamics/b2Fixture.cpp \
    ../_Libs/Box2D/Dynamics/b2Island.cpp \
    ../_Libs/Box2D/Dynamics/b2World.cpp \
    ../_Libs/Box2D/Dynamics/b2WorldCallbacks.cpp \
    ../_Libs/Box2D/Rope/b2Rope.cpp \
    physics/base_object.cpp \
    physics/phys_util.cpp \
    graphics/lvl_camera.cpp \
    graphics/graphics.cpp \
    scenes/scene_level.cpp \
    scenes/scene.cpp \
    scenes/scene_world.cpp \
    scenes/scene_credits.cpp \
    scenes/scene_gameover.cpp \
    graphics/gl_renderer.cpp \
    graphics/window.cpp \
    graphics/graphics_lvl_backgrnd.cpp \
    controls/controllable_object.cpp \
    controls/controller.cpp \
    scenes/level/lvl_player.cpp \
    controls/control_keys.cpp \
    data_configs/config_manager.cpp \
    common_features/app_path.cpp \
    common_features/graphics_funcs.cpp \
    ../_Libs/EasyBMP/EasyBMP.cpp \
    data_configs/obj_block.cpp \
    controls/controller_keyboard.cpp \
    data_configs/select_config.cpp \
    common_features/util.cpp \
    scenes/level/lvl_block.cpp \
    data_configs/custom_data.cpp \
    data_configs/config_paths.cpp \
    common_features/simple_animator.cpp \
    data_configs/config_textures.cpp \
    data_configs/obj_bgo.cpp \
    scenes/level/lvl_bgo.cpp \
    data_configs/obj_bg.cpp \
    physics/contact_listener.cpp \
    scenes/level/lvl_warp.cpp \
    scenes/level/lvl_scene_ptr.cpp \
    scenes/level/lvl_scene_items.cpp \
    scenes/level/lvl_scene_sections.cpp \
    scenes/level/lvl_scene_timers.cpp \
    scenes/level/lvl_scene_files_io.cpp \
    scenes/level/lvl_scene_init.cpp \
    physics/engine/pge_phys_world.cpp \
    physics/engine/Quadtree.cpp \
    physics/engine/pge_phys_body.cpp \
    gui/pge_msgbox.cpp \
    gui/pge_boxbase.cpp \
    fontman/font_manager.cpp \
    networking/editor_pipe.cpp \
    networking/intproc.cpp \
    common_features/logger.cpp \
    gui/pge_menu.cpp \
    scenes/scene_loading.cpp \
    scenes/scene_title.cpp \
    ../Editor/file_formats/file_formats.cpp \
    ../Editor/file_formats/file_rw_lvl.cpp \
    ../Editor/file_formats/file_rw_lvlx.cpp \
    ../Editor/file_formats/file_rw_meta.cpp \
    ../Editor/file_formats/file_rw_npc_txt.cpp \
    ../Editor/file_formats/file_rw_sav.cpp \
    ../Editor/file_formats/file_rw_wld.cpp \
    ../Editor/file_formats/file_rw_wldx.cpp \
    ../Editor/file_formats/file_rwopen.cpp \
    ../Editor/file_formats/file_strlist.cpp \
    ../Editor/file_formats/lvl_filedata.cpp \
    ../Editor/file_formats/npc_filedata.cpp \
    ../Editor/file_formats/pge_x.cpp \
    ../Editor/file_formats/save_filedata.cpp \
    ../Editor/file_formats/smbx64.cpp \
    ../Editor/file_formats/wld_filedata.cpp \
    ../_Libs/oolua/class_from_stack.cpp \
    ../_Libs/oolua/oolua.cpp \
    ../_Libs/oolua/oolua_check_result.cpp \
    ../_Libs/oolua/oolua_chunk.cpp \
    ../_Libs/oolua/oolua_error.cpp \
    ../_Libs/oolua/oolua_exception.cpp \
    ../_Libs/oolua/oolua_function.cpp \
    ../_Libs/oolua/oolua_helpers.cpp \
    ../_Libs/oolua/oolua_open.cpp \
    ../_Libs/oolua/oolua_pull.cpp \
    ../_Libs/oolua/oolua_push.cpp \
    ../_Libs/oolua/oolua_ref.cpp \
    ../_Libs/oolua/oolua_registration.cpp \
    ../_Libs/oolua/oolua_script.cpp \
    ../_Libs/oolua/oolua_stack_dump.cpp \
    ../_Libs/oolua/oolua_string.cpp \
    ../_Libs/oolua/oolua_table.cpp \
    ../_Libs/oolua/proxy_storage.cpp \
    ../_Libs/oolua/push_pointer_internal.cpp \
    ../_Libs/oolua/stack_get.cpp \
    ../_Libs/oolua/lua/lapi.c \
    ../_Libs/oolua/lua/lauxlib.c \
    ../_Libs/oolua/lua/lbaselib.c \
    ../_Libs/oolua/lua/lbitlib.c \
    ../_Libs/oolua/lua/lcode.c \
    ../_Libs/oolua/lua/lcorolib.c \
    ../_Libs/oolua/lua/lctype.c \
    ../_Libs/oolua/lua/ldblib.c \
    ../_Libs/oolua/lua/ldebug.c \
    ../_Libs/oolua/lua/ldo.c \
    ../_Libs/oolua/lua/ldump.c \
    ../_Libs/oolua/lua/lfunc.c \
    ../_Libs/oolua/lua/lgc.c \
    ../_Libs/oolua/lua/linit.c \
    ../_Libs/oolua/lua/liolib.c \
    ../_Libs/oolua/lua/llex.c \
    ../_Libs/oolua/lua/lmathlib.c \
    ../_Libs/oolua/lua/lmem.c \
    ../_Libs/oolua/lua/loadlib.c \
    ../_Libs/oolua/lua/lobject.c \
    ../_Libs/oolua/lua/lopcodes.c \
    ../_Libs/oolua/lua/loslib.c \
    ../_Libs/oolua/lua/lparser.c \
    ../_Libs/oolua/lua/lstate.c \
    ../_Libs/oolua/lua/lstring.c \
    ../_Libs/oolua/lua/lstrlib.c \
    ../_Libs/oolua/lua/ltable.c \
    ../_Libs/oolua/lua/ltablib.c \
    ../_Libs/oolua/lua/ltm.c \
    ../_Libs/oolua/lua/lua.c \
    ../_Libs/oolua/lua/luac.c \
    ../_Libs/oolua/lua/lundump.c \
    ../_Libs/oolua/lua/lvm.c \
    ../_Libs/oolua/lua/lzio.c

HEADERS  += \
    ../_Libs/Box2D/Box2D.h \
    ../_Libs/Box2D/Collision/Shapes/b2ChainShape.h \
    ../_Libs/Box2D/Collision/Shapes/b2CircleShape.h \
    ../_Libs/Box2D/Collision/Shapes/b2EdgeShape.h \
    ../_Libs/Box2D/Collision/Shapes/b2PolygonShape.h \
    ../_Libs/Box2D/Collision/Shapes/b2Shape.h \
    ../_Libs/Box2D/Collision/b2BroadPhase.h \
    ../_Libs/Box2D/Collision/b2Collision.h \
    ../_Libs/Box2D/Collision/b2Distance.h \
    ../_Libs/Box2D/Collision/b2DynamicTree.h \
    ../_Libs/Box2D/Collision/b2TimeOfImpact.h \
    ../_Libs/Box2D/Common/b2BlockAllocator.h \
    ../_Libs/Box2D/Common/b2Draw.h \
    ../_Libs/Box2D/Common/b2GrowableStack.h \
    ../_Libs/Box2D/Common/b2Math.h \
    ../_Libs/Box2D/Common/b2Settings.h \
    ../_Libs/Box2D/Common/b2StackAllocator.h \
    ../_Libs/Box2D/Common/b2Timer.h \
    ../_Libs/Box2D/Dynamics/Contacts/b2ChainAndCircleContact.h \
    ../_Libs/Box2D/Dynamics/Contacts/b2ChainAndPolygonContact.h \
    ../_Libs/Box2D/Dynamics/Contacts/b2CircleContact.h \
    ../_Libs/Box2D/Dynamics/Contacts/b2Contact.h \
    ../_Libs/Box2D/Dynamics/Contacts/b2ContactSolver.h \
    ../_Libs/Box2D/Dynamics/Contacts/b2EdgeAndCircleContact.h \
    ../_Libs/Box2D/Dynamics/Contacts/b2EdgeAndPolygonContact.h \
    ../_Libs/Box2D/Dynamics/Contacts/b2PolygonAndCircleContact.h \
    ../_Libs/Box2D/Dynamics/Contacts/b2PolygonContact.h \
    ../_Libs/Box2D/Dynamics/Joints/b2DistanceJoint.h \
    ../_Libs/Box2D/Dynamics/Joints/b2FrictionJoint.h \
    ../_Libs/Box2D/Dynamics/Joints/b2GearJoint.h \
    ../_Libs/Box2D/Dynamics/Joints/b2Joint.h \
    ../_Libs/Box2D/Dynamics/Joints/b2MotorJoint.h \
    ../_Libs/Box2D/Dynamics/Joints/b2MouseJoint.h \
    ../_Libs/Box2D/Dynamics/Joints/b2PrismaticJoint.h \
    ../_Libs/Box2D/Dynamics/Joints/b2PulleyJoint.h \
    ../_Libs/Box2D/Dynamics/Joints/b2RevoluteJoint.h \
    ../_Libs/Box2D/Dynamics/Joints/b2RopeJoint.h \
    ../_Libs/Box2D/Dynamics/Joints/b2WeldJoint.h \
    ../_Libs/Box2D/Dynamics/Joints/b2WheelJoint.h \
    ../_Libs/Box2D/Dynamics/b2Body.h \
    ../_Libs/Box2D/Dynamics/b2ContactManager.h \
    ../_Libs/Box2D/Dynamics/b2Fixture.h \
    ../_Libs/Box2D/Dynamics/b2Island.h \
    ../_Libs/Box2D/Dynamics/b2TimeStep.h \
    ../_Libs/Box2D/Dynamics/b2World.h \
    ../_Libs/Box2D/Dynamics/b2WorldCallbacks.h \
    ../_Libs/Box2D/Rope/b2Rope.h \
    physics/base_object.h \
    physics/phys_util.h \
    graphics/lvl_camera.h \
    graphics/graphics.h \
    scenes/scene_level.h \
    scenes/scene.h \
    scenes/scene_world.h \
    scenes/scene_credits.h \
    scenes/scene_gameover.h \
    graphics/gl_renderer.h \
    graphics/window.h \
    controls/controllable_object.h \
    controls/controller.h \
    scenes/level/lvl_player.h \
    controls/control_keys.h \
    data_configs/config_manager.h \
    data_configs/obj_block.h \
    common_features/app_path.h \
    common_features/graphics_funcs.h \
    common_features/pge_texture.h \
    ../_Libs/EasyBMP/EasyBMP.h \
    ../_Libs/EasyBMP/EasyBMP_BMP.h \
    ../_Libs/EasyBMP/EasyBMP_DataStructures.h \
    ../_Libs/EasyBMP/EasyBMP_VariousBMPutilities.h \
    controls/controller_keyboard.h \
    data_configs/select_config.h \
    common_features/util.h \
    scenes/level/lvl_block.h \
    data_configs/custom_data.h \
    common_features/simple_animator.h \
    data_configs/obj_bgo.h \
    scenes/level/lvl_bgo.h \
    data_configs/obj_bg.h \
    graphics/graphics_lvl_backgrnd.h \
    version.h \
    physics/contact_listener.h \
    scenes/level/lvl_warp.h \
    scenes/level/lvl_scene_ptr.h \
    physics/engine/pge_phys_world.h \
    physics/engine/Quadtree.h \
    physics/engine/pge_phys_body.h \
    gui/pge_msgbox.h \
    gui/pge_boxbase.h \
    fontman/font_manager.h \
    networking/editor_pipe.h \
    networking/intproc.h \
    common_features/logger.h \
    common_features/logger_sets.h \
    gui/pge_menu.h \
    scenes/scene_loading.h \
    scenes/scene_title.h \
    ../Editor/file_formats/file_formats.h \
    ../Editor/file_formats/file_strlist.h \
    ../Editor/file_formats/lvl_filedata.h \
    ../Editor/file_formats/meta_filedata.h \
    ../Editor/file_formats/npc_filedata.h \
    ../Editor/file_formats/pge_x.h \
    ../Editor/file_formats/save_filedata.h \
    ../Editor/file_formats/smbx64.h \
    ../Editor/file_formats/wld_filedata.h \
    ../_Libs/oolua/lua/lapi.h \
    ../_Libs/oolua/lua/lauxlib.h \
    ../_Libs/oolua/lua/lcode.h \
    ../_Libs/oolua/lua/lctype.h \
    ../_Libs/oolua/lua/ldebug.h \
    ../_Libs/oolua/lua/ldo.h \
    ../_Libs/oolua/lua/lfunc.h \
    ../_Libs/oolua/lua/lgc.h \
    ../_Libs/oolua/lua/llex.h \
    ../_Libs/oolua/lua/llimits.h \
    ../_Libs/oolua/lua/lmem.h \
    ../_Libs/oolua/lua/lobject.h \
    ../_Libs/oolua/lua/lopcodes.h \
    ../_Libs/oolua/lua/lparser.h \
    ../_Libs/oolua/lua/lstate.h \
    ../_Libs/oolua/lua/lstring.h \
    ../_Libs/oolua/lua/ltable.h \
    ../_Libs/oolua/lua/ltm.h \
    ../_Libs/oolua/lua/lua.h \
    ../_Libs/oolua/lua/lua.hpp \
    ../_Libs/oolua/lua/luaconf.h \
    ../_Libs/oolua/lua/lualib.h \
    ../_Libs/oolua/lua/lundump.h \
    ../_Libs/oolua/lua/lvm.h \
    ../_Libs/oolua/lua/lzio.h \
    ../_Libs/oolua/char_arrays.h \
    ../_Libs/oolua/class_from_stack.h \
    ../_Libs/oolua/default_trait_caller.h \
    ../_Libs/oolua/dsl_va_args.h \
    ../_Libs/oolua/lua_includes.h \
    ../_Libs/oolua/lvd_type_traits.h \
    ../_Libs/oolua/lvd_types.h \
    ../_Libs/oolua/oolua.h \
    ../_Libs/oolua/oolua_boilerplate.h \
    ../_Libs/oolua/oolua_check_result.h \
    ../_Libs/oolua/oolua_chunk.h \
    ../_Libs/oolua/oolua_config.h \
    ../_Libs/oolua/oolua_dsl.h \
    ../_Libs/oolua/oolua_dsl_export.h \
    ../_Libs/oolua/oolua_error.h \
    ../_Libs/oolua/oolua_exception.h \
    ../_Libs/oolua/oolua_function.h \
    ../_Libs/oolua/oolua_helpers.h \
    ../_Libs/oolua/oolua_open.h \
    ../_Libs/oolua/oolua_pull.h \
    ../_Libs/oolua/oolua_push.h \
    ../_Libs/oolua/oolua_ref.h \
    ../_Libs/oolua/oolua_registration.h \
    ../_Libs/oolua/oolua_registration_fwd.h \
    ../_Libs/oolua/oolua_script.h \
    ../_Libs/oolua/oolua_stack.h \
    ../_Libs/oolua/oolua_stack_dump.h \
    ../_Libs/oolua/oolua_stack_fwd.h \
    ../_Libs/oolua/oolua_string.h \
    ../_Libs/oolua/oolua_table.h \
    ../_Libs/oolua/oolua_traits.h \
    ../_Libs/oolua/oolua_traits_fwd.h \
    ../_Libs/oolua/oolua_version.h \
    ../_Libs/oolua/platform_check.h \
    ../_Libs/oolua/proxy_base_checker.h \
    ../_Libs/oolua/proxy_caller.h \
    ../_Libs/oolua/proxy_class.h \
    ../_Libs/oolua/proxy_constructor.h \
    ../_Libs/oolua/proxy_constructor_param_tester.h \
    ../_Libs/oolua/proxy_function_dispatch.h \
    ../_Libs/oolua/proxy_function_exports.h \
    ../_Libs/oolua/proxy_member_function.h \
    ../_Libs/oolua/proxy_none_member_function.h \
    ../_Libs/oolua/proxy_operators.h \
    ../_Libs/oolua/proxy_public_member.h \
    ../_Libs/oolua/proxy_stack_helper.h \
    ../_Libs/oolua/proxy_storage.h \
    ../_Libs/oolua/proxy_tag_info.h \
    ../_Libs/oolua/proxy_tags.h \
    ../_Libs/oolua/proxy_test.h \
    ../_Libs/oolua/proxy_userdata.h \
    ../_Libs/oolua/push_pointer_internal.h \
    ../_Libs/oolua/stack_get.h \
    ../_Libs/oolua/type_converters.h \
    ../_Libs/oolua/type_list.h \
    ../_Libs/oolua/typelist_structs.h

FORMS    += \
    data_configs/select_config.ui

RESOURCES += \
    _resources/engine.qrc
