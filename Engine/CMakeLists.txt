cmake_minimum_required (VERSION 3.5...3.10)
project(MoondustEngine C CXX)

set(DEPENDENCIES_INSTALL_DIR ${CMAKE_CURRENT_BINARY_DIR})

set(BUILD_SHARED_LIBRARIES OFF)

set(CMAKE_INSTALL_RPATH ".")
set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)
if(EMSCRIPTEN)
    set(CMAKE_BUILD_WITH_INSTALL_RPATH TRUE)
endif()

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${DEPENDENCIES_INSTALL_DIR}/include
    ${DEPENDENCIES_INSTALL_DIR}/include/lua_includes
    ${CMAKE_CURRENT_SOURCE_DIR}/../_common/ # PGE-FL
)
link_directories(${DEPENDENCIES_INSTALL_DIR}/lib)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Choose the type of build, options are: Debug Release RelWithDebInfo MinSizeRel." FORCE)
    message("== Using default build configuration which is a Release!")
endif()

if(ANDROID)
    if(${ANDROID_ABI} STREQUAL "armeabi-v7a")
        # Disable NEON support for old devices
        set(ANDROID_ARM_NEON FALSE)
    elseif(NOT DEFINED ANDROID_ARM_NEON)
        set(ANDROID_ARM_NEON TRUE)
    endif()

    if(NOT DEFINED ANDROID_STL)
        # include(ndk-stl-config.cmake)
        set(ANDROID_STL "c++_static")
    endif()
endif()

# Version
include(version.cmake)
# Default GIT version
include(../_common/git_version.cmake)
# Common building properties and optimization flags
include(../_common/build_props.cmake)
# Build properties for executable projects
include(../_common/executable_build_props.cmake)

# include shared code modules
include(../_common/ConfigPackManager/PGE_ConfigPackManager.cmake)
include(../_Libs/DirManager/dirman.cmake)
include(../_Libs/FileMapper/FileMapper.cmake)
include(../_common/fmt/fmt.cmake)
include(../_Libs/IniProcessor/IniProcessor.cmake)
include(../_common/tclap/tclap.cmake)
include(../_common/Logger/logger.cmake)
include(../_common/Utf8Main/utf8main.cmake)
include(../_common/Utils/Utils.cmake)
include(../_common/Utils/UtilsSDL.cmake)
include(../_common/Utils/openUrl.cmake)
include(../_common/Graphics/bitmask2rgba.cmake)
if(WIN32)
    include(../_common/StackWalker/StackWalker.cmake)
endif()

add_definitions("-DMOONDUST_LOGGER_FILENAME_PREFIX=\"PGE_Engine\"")

pge_cxx_standard(14)

set(_LANGAUGES_TEMP_FOLDER "${CMAKE_CURRENT_SOURCE_DIR}/languages")
if(_QT_LRELEASE_PROGRAM)
    set(_LANGAUGES_TEMP_FOLDER "${CMAKE_BINARY_DIR}/Engine_langs")

    file(MAKE_DIRECTORY ${_LANGAUGES_TEMP_FOLDER})

    set(PGE_LANG_SRC_FILES)
    FILE(GLOB PGE_LANG_SRC_FILES "${CMAKE_CURRENT_SOURCE_DIR}/languages/*.ts")
    message("-- Compiling Engine's locales...")
    foreach(LangSrcFile ${PGE_LANG_SRC_FILES})
        get_filename_component(LangBaseFile "${LangSrcFile}" NAME_WE)
        # message("-- Compiling locale ${LangSrcFile} into ${LangBaseFile}.qm")
        execute_process(COMMAND ${_QT_LRELEASE_PROGRAM}
            "${LangSrcFile}"
            -qm
            "${_LANGAUGES_TEMP_FOLDER}/${LangBaseFile}.qm"
            -idbased
            -silent
        )
    endforeach()
endif()

set(PGE_LANG_BIN_FILES)
file(GLOB PGE_LANG_BIN_FILES "${_LANGAUGES_TEMP_FOLDER}/*.qm")
#foreach(LangBinFile ${PGE_LANG_BIN_FILES})
#    message("-- locale ${LangBinFile} will be installed")
#endforeach()

if(NOT APPLE AND NOT EMSCRIPTEN AND NOT ANDROID)
    file(COPY ${PGE_LANG_BIN_FILES} DESTINATION "${CMAKE_BINARY_DIR}/bin/languages")
    install(FILES ${PGE_LANG_BIN_FILES} DESTINATION "${PGE_INSTALL_DIRECTORY}/languages")
endif()

if(ANDROID)
    file(COPY ${PGE_LANG_BIN_FILES} DESTINATION "${CMAKE_CURRENT_SOURCE_DIR}/android-project/moondust/src/main/assets/languages")
endif()

# ======= embedded resources =======
add_res_generator(
    "${CMAKE_CURRENT_SOURCE_DIR}/_resources/"
    "${CMAKE_CURRENT_BINARY_DIR}/resources/"
    "engine_resource.h"
    engine_resource_data
)

set(PGE_ENGINE_RESOURCES
    "${CMAKE_CURRENT_BINARY_DIR}/resources/resource.cpp"
    "${CMAKE_CURRENT_BINARY_DIR}/resources/engine_resource.h"
    "${CMAKE_CURRENT_BINARY_DIR}/resources/resource_data.h"
)
set_source_files_properties(${PGE_ENGINE_RESOURCES} PROPERTIES GENERATED TRUE)
# ======= embedded resources =======

set(PGE_ENGINE_SRCS)

list(APPEND PGE_ENGINE_SRCS
    ${PGE_ENGINE_RESOURCES}
    audio/pge_audio.cpp
    audio/play_music.cpp
    audio/play_sfx.cpp
    common_features/app_path/app_path.cpp
    common_features/crash_handler.cpp
    common_features/episode_state.cpp
    common_features/fader.cpp
    common_features/graphics_funcs.cpp
    common_features/matrix_animator.cpp
    common_features/md5.cpp
    common_features/npc_animator.cpp
    common_features/number_limiter.cpp
    common_features/player_calibration.cpp
    common_features/point.cpp
    common_features/pointf.cpp
    common_features/point_mover.cpp
    common_features/QTranslatorX/qm_translator.cpp
    common_features/QuadTree/LooseQuadtree-impl.cpp
    common_features/rect.cpp
    common_features/rectf.cpp
    common_features/simple_animator.cpp
    common_features/size.cpp
    common_features/sizef.cpp
    common_features/translator.cpp
    common_features/util.cpp
    common_features/version_cmp.cpp
    controls/control_keys.cpp
    controls/controllable_object.cpp
    controls/controller.cpp
    controls/controller_joystick.cpp
    controls/controller_keyboard.cpp
    data_configs/config_engine.cpp
    data_configs/config_manager.cpp
    data_configs/config_paths.cpp
    data_configs/config_select_scene/scene_config_select.cpp
    data_configs/config_textures.cpp
    data_configs/custom_data.cpp
    data_configs/obj_bg.cpp
    data_configs/obj_bgo.cpp
    data_configs/obj_block.cpp
    data_configs/obj_effect.cpp
    data_configs/obj_music.cpp
    data_configs/obj_npc.cpp
    data_configs/obj_player.cpp
    data_configs/obj_sound.cpp
    data_configs/obj_wld_items.cpp
    data_configs/setup_credits_screen.cpp
    data_configs/setup_load_screen.cpp
    data_configs/setup_lvl_scene.cpp
    data_configs/setup_title_screen.cpp
    data_configs/setup_wld_scene.cpp
    data_configs/spawn_effect_def.cpp
    engine.cpp
    fontman/font_manager.cpp
    fontman/font_manager_private.cpp
    fontman/raster_font.cpp
    fontman/ttf_font.cpp
    fontman/utf8_helpers.cpp
    graphics/gl_color.cpp
    graphics/gl_renderer.cpp
    graphics/graphics.cpp
    graphics/render/render_base.cpp
    graphics/render/render_opengl21.cpp
    graphics/render/render_opengl31.cpp
    graphics/render/render_swsdl.cpp
    graphics/vsync_validator.cpp
    graphics/window.cpp
    gui/menu/pge_bool_menuitem.cpp
    gui/menu/pge_int_menuitem.cpp
    gui/menu/pge_int_named_menuitem.cpp
    gui/menu/pge_keygrab_menuitem.cpp
    gui/menu/_pge_menuitem.cpp
    gui/pge_boxbase.cpp
    gui/pge_menuboxbase.cpp
    gui/pge_menubox.cpp
    gui/pge_menu.cpp
    gui/pge_msgbox.cpp
    gui/pge_questionbox.cpp
    gui/pge_textinputbox.cpp
    main.cpp
    networking/editor_pipe.cpp
    networking/intproc.cpp
    scenes/_base/gfx_effect.cpp
    scenes/_base/msgbox_queue.cpp
    scenes/level/base/lvl_base_physics.cpp
    scenes/level/bg/bg_multilayer.cpp
    scenes/level/bg/bg_standard.cpp
    scenes/level/lvl_backgrnd.cpp
    scenes/level/lvl_base_object.cpp
    scenes/level/lvl_bgo.cpp
    scenes/level/lvl_block.cpp
    scenes/level/lvl_camera.cpp
    scenes/level/lvl_event_engine.cpp
    scenes/level/lvl_layer_engine.cpp
    scenes/level/lvl_npc.cpp
    scenes/level/lvl_physenv.cpp
    scenes/level/lvl_player.cpp
    scenes/level/lvl_player_def.cpp
    scenes/level/lvl_player_switch.cpp
    scenes/level/lvl_quad_tree.cpp
    scenes/level/lvl_scene_files_io.cpp
    scenes/level/lvl_scene_garbage_collecting.cpp
    scenes/level/lvl_scene_init.cpp
    scenes/level/lvl_scene_interprocess.cpp
    scenes/level/lvl_scene_items.cpp
    scenes/level/lvl_scene_pausemenu.cpp
    scenes/level/lvl_scene_sections.cpp
    scenes/level/lvl_scene_timers.cpp
    scenes/level/lvl_scene_trees.cpp
    scenes/level/lvl_section.cpp
    scenes/level/lvl_subtree.cpp
    scenes/level/lvl_warp.cpp
    scenes/level/lvl_z_constants.cpp
    scenes/level/npc_detectors/lvl_base_detector.cpp
    scenes/level/npc_detectors/lvl_dtc_contact.cpp
    scenes/level/npc_detectors/lvl_dtc_inarea.cpp
    scenes/level/npc_detectors/lvl_dtc_player_pos.cpp
    scenes/level/npc/lvl_npc_activate.cpp
    scenes/level/npc/lvl_npc_generator.cpp
    scenes/level/npc/lvl_npc_groups.cpp
    scenes/level/npc/lvl_npc_kill.cpp
    scenes/level/npc/lvl_npc_lua.cpp
    scenes/level/npc/lvl_npc_physics.cpp
    scenes/level/npc/lvl_npc_render.cpp
    scenes/level/npc/lvl_npc_setup.cpp
    scenes/level/npc/lvl_npc_spritewarp.cpp
    scenes/level/npc/lvl_npc_throwned_by.cpp
    scenes/level/npc/lvl_npc_update.cpp
    scenes/level/player/lvl_player_animation.cpp
    scenes/level/player/lvl_player_attaks_and_kills.cpp
    scenes/level/player/lvl_player_physics.cpp
    scenes/level/player/lvl_player_render.cpp
    scenes/level/player/lvl_player_setup.cpp
    scenes/level/player/lvl_player_update.cpp
    scenes/level/player/lvl_player_warps_and_teleports.cpp
    scenes/scene.cpp
    scenes/scene_credits.cpp
    scenes/scene_gameover.cpp
    scenes/scene_level.cpp
    scenes/scene_loading.cpp
    scenes/scene_title.cpp
    scenes/scene_world.cpp
    scenes/title/ttl_main_menu.cpp
    scenes/world/wld_pathopener.cpp
    scenes/world/wld_player_portrait.cpp
    scenes/world/wld_tilebox.cpp
    script/bindings/core/classes/luaclass_core_data.cpp
    script/bindings/core/classes/luaclass_core_graphics.cpp
    script/bindings/core/classes/luaclass_core_scene_effects.cpp
    script/bindings/core/classes/luaclass_core_simpleevent.cpp
    script/bindings/core/events/luaevents_core_engine.cpp
    script/bindings/core/globalfuncs/luafuncs_core_audio.cpp
    script/bindings/core/globalfuncs/luafuncs_core_effect.cpp
    script/bindings/core/globalfuncs/luafuncs_core_logger.cpp
    script/bindings/core/globalfuncs/luafuncs_core_paths.cpp
    script/bindings/core/globalfuncs/luafuncs_core_renderer.cpp
    script/bindings/core/globalfuncs/luafuncs_core_settings.cpp
    script/bindings/core/lua_global_constants.cpp
    script/bindings/level/classes/luaclass_level_camera.cpp
    script/bindings/level/classes/luaclass_level_contact_detector.cpp
    script/bindings/level/classes/luaclass_level_inareadetector.cpp
    script/bindings/level/classes/luaclass_level_lvl_npc.cpp
    script/bindings/level/classes/luaclass_level_lvl_player.cpp
    script/bindings/level/classes/luaclass_level_physobj.cpp
    script/bindings/level/classes/luaclass_level_playerposdetector.cpp
    script/bindings/level/classes/luaclass_level_playerstate.cpp
    script/bindings/level/classes/luaclass_lvl_bgo.cpp
    script/bindings/level/classes/luaclass_lvl_block.cpp
    script/bindings/level/globalfuncs/luafuncs_level_bgo.cpp
    script/bindings/level/globalfuncs/luafuncs_level_blocks.cpp
    script/bindings/level/globalfuncs/luafuncs_level.cpp
    script/bindings/level/globalfuncs/luafuncs_level_lvl_npc.cpp
    script/bindings/level/globalfuncs/luafuncs_level_lvl_player.cpp
    script/lua_credits_engine.cpp
    script/lua_engine.cpp
    script/lua_event.cpp
    script/lua_global.cpp
    script/lua_level_engine.cpp
    script/lua_titlescreen_engine.cpp
    script/lua_world_engine.cpp
    settings/debugger.cpp
    settings/global_settings.cpp
)

if(ANDROID)
    list(APPEND PGE_ENGINE_SRCS
        controls/controller_touchscreen.cpp
    )
endif()

if(APPLE)
    FILE(GLOB PGE_FILE_ICONS "${CMAKE_CURRENT_SOURCE_DIR}/_resources/file_icons/*.icns")
    list(APPEND PGE_ENGINE_SRCS
        common_features/app_path/apple/app_path_macosx.m
        _resources/cat.icns
        ${PGE_LANG_BIN_FILES}
        ${PGE_FILE_ICONS}
    )
endif()

if(EMSCRIPTEN)
    message("Is EMSCRIPTEN!")
    set(PGE_PRELOAD_CONFIG_PACK "/home/vitaly/.PGE_Project/configs/A2XT" CACHE STRING "Path to config pack to pack")
    set(PGE_PRELOAD_CONFIG_PACK_NAME "A2XT" CACHE STRING "Name of folder of embedded config pack")
    set(PGE_PRELOAD_EPISODE "/home/vitaly/.PGE_Project/worlds/Lowsers Conquest Demo v 2.1" CACHE STRING "Path to episode to pack")
    set(PGE_PRELOAD_EPISODE_NAME "lcd" CACHE STRING "Name of folder of episode")
    set(CMAKE_EXECUTABLE_SUFFIX ".html")
#    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -msse -msse2")
    # -DIS_CXX -s USE_PTHREADS=1
#    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -s WASM=1 -s 'BINARYEN_METHOD=\"native-wasm\"'")
    #set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -s EMTERPRETIFY=1 -s EMTERPRETIFY_ASYNC=1  -s 'EMTERPRETIFY_FILE=\"pge_engine.binary\"'")
    #set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -s ASSERTIONS=1")
    # -s \"EMTERPRETIFY_WHITELIST=['_main']\"
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -s ASYNCIFY=1") #-s USE_SDL=2
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -s ERROR_ON_UNDEFINED_SYMBOLS=0 -s ASYNCIFY=1 -lidbfs.js")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -s \"EXPORTED_FUNCTIONS=['_main', '_unlockLoadingCustomState']\"")
#    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -s 'BINARYEN_TRAP_MODE=\"clamp\"'")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -s TOTAL_MEMORY=83886080 --no-heap-copy -s ALLOW_MEMORY_GROWTH=1")
#    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -s \"EXTRA_EXPORTED_RUNTIME_METHODS=['Pointer_stringify']\"")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} --embed-file \"${_LANGAUGES_TEMP_FOLDER}\"@\"languages/\"")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} --preload-file \"${PGE_PRELOAD_CONFIG_PACK}\"@\"configs/${PGE_PRELOAD_CONFIG_PACK_NAME}/\"")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} --preload-file \"${PGE_PRELOAD_EPISODE}\"@\"worlds/${PGE_PRELOAD_EPISODE_NAME}/\"")
    # set(LINK_FLAGS "${LINK_FLAGS} -DIS_LINK -s USE_PTHREADS=1 -s FORCE_FILESYSTEM=1 --embed-file '${CMAKE_CURRENT_SOURCE_DIR}/@languages'")
endif()

if(WIN32 AND NOT EMSCRIPTEN)
    list(APPEND PGE_ENGINE_SRCS _resources/engine.rc)
endif()

set(PGE_ENGINE_SRC_ARGS
    ${PGE_ENGINE_SRCS}
    ${CONFIG_PACK_MANAGER_SRCS}
    ${DIRMANAGER_SRCS}
    ${FILEMAPPER_SRCS}
    ${FMT_SRCS}
    ${INIPROCESSOR_SRCS}
    ${LOGGER_SRCS}
    ${PGE_GAMESAVE_SRCS}
    ${STACK_WALKER_SRCS}
    ${UTF8MAIN_SRCS}
    ${OPENURL_SRCS}
    ${UTILS_SRCS}
    ${UTILS_SDL_SRCS}
    ${BITMASK2RGBA_SRCS}
)

if(ANDROID)
    set(CMAKE_DEBUG_POSTFIX "")
    add_library(pge_engine SHARED
        ${PGE_ENGINE_SRC_ARGS}
    )
    target_compile_options(pge_engine PRIVATE -fPIC)

    # Include own-built libraries into the APK
#    set(APK_PACK_LIBS ${CMAKE_SOURCE_DIR}/Engine/android-project/moondust/jniLibs/${CMAKE_BUILD_TYPE_LOWER}/${ANDROID_ABI})
#    add_custom_target(SDL_ApkPackLibs_makeDir ALL COMMAND ${CMAKE_COMMAND} -E make_directory "${APK_PACK_LIBS}")
#
#    add_custom_target(SDL_ApkPackLibs_sdl2 ALL
#            COMMAND ${CMAKE_COMMAND} -E copy "${SDL2_SO_Lib}" "${APK_PACK_LIBS}/libSDL2.so"
#            DEPENDS SDL_ApkPackLibs_makeDir "${SDL2_SO_Lib}" SDL2_Local
#            )
#    add_dependencies(pge_engine SDL_ApkPackLibs_sdl2)

#    add_custom_target(SDL_ApkPackLibs_SDL2 ALL
#        COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_BINARY_DIR}/lib/libSDL2.so" "${APK_PACK_LIBS}"
#        DEPENDS SDL_ApkPackLibs_makeDir "${CMAKE_BINARY_DIR}/lib/libSDL2.so" AudioCodecs_Local
#    )
#    add_custom_target(SDL_ApkPackLibs_hidapi ALL
#        COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_BINARY_DIR}/lib/libhidapi.so" "${APK_PACK_LIBS}"
#        DEPENDS SDL_ApkPackLibs_makeDir "${CMAKE_BINARY_DIR}/lib/libhidapi.so" AudioCodecs_Local
#    )
#    add_dependencies(pge_engine SDL_ApkPackLibs_SDL2) #SDL_ApkPackLibs_hidapi
else()
    add_executable(pge_engine
        ${PGE_ENGINE_SRC_ARGS}
    )
endif()

target_compile_definitions(pge_engine
    PRIVATE
    -DPGE_ENGINE
)

pge_set_nopie(pge_engine)

# add "USE_LUA_JIT" when use LuaJIT or don't add when using PUC-Rio Lua
if(PGE_USE_LUAJIT)
    message("== Engine will be built with LuaJIT support")
    target_compile_definitions(pge_engine
        PUBLIC
        -DUSE_LUA_JIT
    )
else()
    message("== Engine will be built with PUC-Rio Lua support")
endif()

if(EMSCRIPTEN)
    target_compile_definitions(pge_engine
        PRIVATE
        -DPGE_NO_THREADING
        -DLUA_USE_LINUX
    )
endif()

if(WIN32 AND NOT EMSCRIPTEN)
    target_compile_definitions(pge_engine
        PUBLIC
        -DUNICODE
        -D_UNICODE
        -DNOMINMAX
    )
endif()

string(TOLOWER "${CMAKE_BUILD_TYPE}" CMAKE_BUILD_TYPE_LOWER)
if(${CMAKE_BUILD_TYPE_LOWER} STREQUAL "debug")
    target_compile_definitions(pge_engine
        PRIVATE
        -DDEBUG_BUILD
    )
endif()

add_dependencies(pge_engine
    SDLMixerX_Local
    SDL2_Local
    LuaBind_Local
    FreeImage_Local
#    SQLite_Local
    PGE_FileFormats
    FREETYPE_Local
    engine_resource_data
)

if(NOT SDL2_USE_SYSTEM)
    add_dependencies(pge_engine SDL2_Local)
else()
    target_include_directories(pge_engine PRIVATE ${SDL2_INCLUDE_DIRS})
endif()

set(PGE_ENGINE_LINK_LIBS)
set(PGE_ENGINE_INSTALLS)

list(APPEND PGE_ENGINE_INSTALLS
    pge_engine
)

list(APPEND PGE_ENGINE_LINK_LIBS
    ${UTILS_LIBS}
    PGE_FileFormats
    PGE_FreeImage
    PGE_LuaBind
    PGE_LuaJIT
    PGE_FreeType
#    PGE_SQLite
)

if(PGE_STATIC_SDLMIXER)
    list(APPEND PGE_ENGINE_LINK_LIBS PGE_SDLMixerX_static PGE_SDL2_static)
else()
    list(APPEND PGE_ENGINE_LINK_LIBS PGE_SDLMixerX PGE_SDL2)
endif()

if(NOT APPLE AND NOT HAIKU AND NOT MSVC AND NOT USE_SYSTEM_LIBC)
    set_property(TARGET pge_engine APPEND_STRING PROPERTY LINK_FLAGS " -static-libgcc -static-libstdc++")
endif()

#if(ANDROID)
#    list(APPEND PGE_ENGINE_LINK_LIBS
#        "cpufeatures"
#    )
#endif()

if(NOT EMSCRIPTEN AND NOT MSVC AND NOT ANDROID)
    if(WIN32)
#        set_property(TARGET pge_engine APPEND_STRING PROPERTY LINK_FLAGS " -mconsole")
        set_target_properties(pge_engine PROPERTIES WIN32_EXECUTABLE OFF)
        list(APPEND PGE_ENGINE_LINK_LIBS "-static")
    endif()
    find_library(_LIB_PTHREAD pthread)
    if(_LIB_PTHREAD)
        list(APPEND PGE_ENGINE_LINK_LIBS ${_LIB_PTHREAD})
    endif()
endif()

if(APPLE)
    set_target_properties(pge_engine PROPERTIES
        OUTPUT_NAME "Moondust Engine"
        MACOSX_BUNDLE TRUE
        MACOSX_BUNDLE_INFO_PLIST "${CMAKE_CURRENT_SOURCE_DIR}/_resources/pge_engine.plist.in"
        MACOSX_BUNDLE_EXECUTABLE_NAME "pge_engine"
        MACOSX_BUNDLE_BUNDLE_NAME "Moondust Engine"
        MACOSX_BUNDLE_GUI_IDENTIFIER "ru.wohlsoft.pge-engine"
        MACOSX_BUNDLE_SHORT_VERSION_STRING "${ENGINE_VERSION_STRING}"
        MACOSX_BUNDLE_LONG_VERSION_STRING "${ENGINE_VERSION_STRING}"
        MACOSX_BUNDLE_ICON_FILE "cat.icns"
        CPACK_BUNDLE_NAME "Moondust Engine"
        MACOSX_BUNDLE_INFO_STRING "Moondust Engine Runtime"
    )
    set_source_files_properties("${CMAKE_CURRENT_SOURCE_DIR}/_resources/cat.icns" PROPERTIES MACOSX_PACKAGE_LOCATION "Resources")
    set_source_files_properties(${PGE_LANG_BIN_FILES} PROPERTIES MACOSX_PACKAGE_LOCATION "Resources/languages")
    set_source_files_properties(${PGE_FILE_ICONS} PROPERTIES MACOSX_PACKAGE_LOCATION "Resources")

    list(APPEND PGE_ENGINE_LINK_LIBS iconv)
    # if(PGE_USE_LUAJIT AND "${TARGET_PROCESSOR}" STREQUAL "x86_64")
    #     # Required to link on 64-bit macOS
    #     # See: http://luajit.org/install.html
    #     set_property(TARGET pge_engine APPEND_STRING PROPERTY
    #         LINK_FLAGS " -pagezero_size 10000 -image_base 100000000"
    #     )
    # endif()
endif()

target_link_libraries(pge_engine
    ${PGE_ENGINE_LINK_LIBS}
)

install(TARGETS ${PGE_ENGINE_INSTALLS}
    RUNTIME DESTINATION "${PGE_INSTALL_DIRECTORY}/"
    LIBRARY DESTINATION "${PGE_INSTALL_DIRECTORY}/"
    BUNDLE DESTINATION "${PGE_INSTALL_DIRECTORY}/"
)
