#-------------------------------------------------
#
# Project created by QtCreator 2014-09-05T22:51:01
#
#-------------------------------------------------

QT += core gui opengl network
#QT += widgets

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
    ../Editor/file_formats/file_formats.cpp \
    ../Editor/file_formats/file_lvl.cpp \
    ../Editor/file_formats/file_lvlx.cpp \
    ../Editor/file_formats/file_npc_txt.cpp \
    ../Editor/file_formats/file_wld.cpp \
    ../Editor/file_formats/file_wldx.cpp \
    ../Editor/file_formats/lvl_filedata.cpp \
    ../Editor/file_formats/npc_filedata.cpp \
    ../Editor/file_formats/pge_x.cpp \
    ../Editor/file_formats/smbx64.cpp \
    ../Editor/file_formats/wld_filedata.cpp \
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
    scenes/scene_title.cpp

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
    ../Editor/file_formats/file_formats.h \
    ../Editor/file_formats/lvl_filedata.h \
    ../Editor/file_formats/npc_filedata.h \
    ../Editor/file_formats/wld_filedata.h \
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
    scenes/scene_title.h

FORMS    += \
    data_configs/select_config.ui

RESOURCES += \
    _resources/engine.qrc
