#include "luafuncs_core_paths.h"
#include <data_configs/config_manager.h>

std::string Binding_Core_GlobalFuncs_Paths::path_commongfx()
{
    return ConfigManager::PathCommonGFX();
}

std::string Binding_Core_GlobalFuncs_Paths::path_level_block()
{
    return ConfigManager::PathLevelBlock();
}

std::string Binding_Core_GlobalFuncs_Paths::path_level_bgo()
{
    return ConfigManager::PathLevelBGO();
}

std::string Binding_Core_GlobalFuncs_Paths::path_level_npc()
{
    return ConfigManager::PathLevelNPC();
}

std::string Binding_Core_GlobalFuncs_Paths::path_effect()
{
    return ConfigManager::PathLevelEffect();
}

std::string Binding_Core_GlobalFuncs_Paths::path_world_terrain()
{
    return ConfigManager::PathWorldTiles();
}

std::string Binding_Core_GlobalFuncs_Paths::path_world_scene()
{
    return ConfigManager::PathWorldScenery();
}

std::string Binding_Core_GlobalFuncs_Paths::path_world_path()
{
    return ConfigManager::PathWorldPaths();
}

std::string Binding_Core_GlobalFuncs_Paths::path_world_level()
{
    return ConfigManager::PathWorldLevels();
}

std::string Binding_Core_GlobalFuncs_Paths::path_music()
{
    return ConfigManager::PathMusic();
}

std::string Binding_Core_GlobalFuncs_Paths::path_sound()
{
    return ConfigManager::PathSound();
}

/***
Config pack paths info functions
@module ConfigPackPaths
*/

luabind::scope Binding_Core_GlobalFuncs_Paths::bindToLua()
{
    using namespace luabind;
    return
        namespace_("Paths")[

            /***
            Common graphics
            @section Common
            */

            /***
            Get common graphics folder of config pack
            @function Paths.CommonGFX
            @treturn string Full path to common graphics folder of config pack
            */
            def("CommonGFX",   &path_commongfx),

            /***
            Get default graphical effects graphics folder of config pack
            @function Paths.EffectGFX
            @treturn string Full path to default graphical effects graphics folder of config pack
            */
            def("EffectGFX",   &path_commongfx),


            /***
            Level related graphics
            @section Level
            */

            /***
            Get default Blocks graphics folder of config pack
            @function Paths.LevelBlocks
            @treturn string Full path to default Blocks graphics folder of config pack
            */
            def("LevelBlocks", &path_level_block),

            /***
            Get default BGO graphics folder of config pack
            @function Paths.LevelBGOs
            @treturn string Full path to default BGO graphics folder of config pack
            */
            def("LevelBGOs",   &path_level_bgo),

            /***
            Get default NPC graphics folder of config pack
            @function Paths.LevelNPCs
            @treturn string Full path to default BGO graphics folder of config pack
            */
            def("LevelNPCs",   &path_level_npc),

            /***
            World map related graphics
            @section WorldMap
            */

            /***
            Get default World Map Terrain tiles graphics folder of config pack
            @function Paths.WorldTerrain
            @treturn string Full path to default World Map Terrain tiles graphics folder of config pack
            */
            def("WorldTerrain",  &path_world_terrain),

            /***
            Get default World Map Terrain tiles graphics folder of config pack, alias to @{Paths.WorldTerrain}
            @function Paths.WorldTiles
            @treturn string Full path to default World Map Terrain tiles graphics folder of config pack
            */
            def("WorldTiles",  &path_world_terrain),

            /***
            Get default World Map Scenery graphics folder of config pack
            @function Paths.WorldScenery
            @treturn string Full path to default World Map Scenery graphics folder of config pack
            */
            def("WorldScenery",&path_world_scene),

            /***
            Get default World Map Path tiles graphics folder of config pack
            @function Paths.WorldPath
            @treturn string Full path to default World Map Path tiles graphics folder of config pack
            */
            def("WorldPath",   &path_world_path),

            /***
            Get default World Map Level Entrancies graphics folder of config pack
            @function Paths.WorldPath
            @treturn string Full path to default World Map Level Entrancies graphics folder of config pack
            */
            def("WorldLevel",  &path_world_level),


            /***
            Music and Sound
            @section MusicAndSound
            */

            /***
            Get default music folder of config pack
            @function Paths.Music
            @treturn string Full path to default music folder of config pack
            */
            def("Music",  &path_music),

            /***
            Get default sound effects folder of config pack
            @function Paths.Sound
            @treturn string Full path to default sound effects folder of config pack
            */
            def("Sound",  &path_sound)
            ];
}
