/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2014-2016 Vitaly Novichkov <admin@wohlnet.ru>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "obj_wld_items.h"
#include "config_manager.h"
#include "config_manager_private.h"
#include "../gui/pge_msgbox.h"
#include <common_features/graphics_funcs.h>
#include <Utils/files.h>
#include <fmt/fmt_format.h>

/*****World Tiles************/
PGE_DataArray<obj_w_tile>   ConfigManager::wld_tiles;
CustomDirManager         ConfigManager::Dir_Tiles;
std::vector<SimpleAnimator >   ConfigManager::Animator_Tiles;
/*****World Tiles************/

/*****World Scenery************/
PGE_DataArray<obj_w_scenery>   ConfigManager::wld_scenery;
CustomDirManager         ConfigManager::Dir_Scenery;
std::vector<SimpleAnimator >   ConfigManager::Animator_Scenery;
/*****World Scenery************/

/*****World Paths************/
PGE_DataArray<obj_w_path>   ConfigManager::wld_paths;
CustomDirManager         ConfigManager::Dir_WldPaths;
std::vector<SimpleAnimator >   ConfigManager::Animator_WldPaths;
/*****World Paths************/

/*****World Levels************/
PGE_DataArray<obj_w_level>  ConfigManager::wld_levels;
CustomDirManager         ConfigManager::Dir_WldLevel;
std::vector<SimpleAnimator >   ConfigManager::Animator_WldLevel;
wld_levels_Markers       ConfigManager::marker_wlvl;
/*****World Levels************/

bool ConfigManager::loadWorldTile(obj_w_tile &tile, std::string section, obj_w_tile *merge_with, std::string iniFile, IniProcessing *setup)
{
    bool valid = true;
    bool internal = !setup;
    std::string errStr;

    if(internal)
        setup = new IniProcessing(iniFile);

    tile.isInit = false;
    tile.image = NULL;
    tile.textureArrayId = 0;
    tile.animator_ID = 0;
    setup->beginGroup(section);

    if(tile.setup.parse(setup, tilePath, default_grid, merge_with ? &merge_with->setup : nullptr, &errStr))
        valid = true;
    else
    {
        addError(errStr);
        valid = false;
    }

    setup->endGroup();

    if(internal) delete setup;

    return valid;
}

bool ConfigManager::loadWorldPath(obj_w_path &path, std::string section, obj_w_path *merge_with, std::string iniFile, IniProcessing *setup)
{
    bool valid = true;
    bool internal = !setup;
    std::string errStr;

    if(internal)
        setup = new IniProcessing(iniFile);

    path.isInit = false;
    path.image = NULL;
    path.textureArrayId = 0;
    path.animator_ID = 0;
    setup->beginGroup(section);

    if(path.setup.parse(setup, pathPath, default_grid, merge_with ? &merge_with->setup : nullptr, &errStr))
        valid = true;
    else
    {
        addError(errStr);
        valid = false;
    }

    setup->endGroup();

    if(internal) delete setup;

    return valid;
}

bool ConfigManager::loadWorldScenery(obj_w_scenery &scene, std::string section, obj_w_scenery *merge_with, std::string iniFile, IniProcessing *setup)
{
    bool valid = true;
    bool internal = !setup;
    std::string errStr;

    if(internal)
        setup = new IniProcessing(iniFile);

    scene.isInit = false;
    scene.image = NULL;
    scene.textureArrayId = 0;
    scene.animator_ID = 0;
    setup->beginGroup(section);

    if(scene.setup.parse(setup, scenePath, default_grid, merge_with ? &merge_with->setup : nullptr, &errStr))
        valid = true;
    else
    {
        addError(errStr);
        valid = false;
    }

    setup->endGroup();

    if(internal) delete setup;

    return valid;
}

bool ConfigManager::loadWorldLevel(obj_w_level &level, std::string section, obj_w_level *merge_with, std::string iniFile, IniProcessing *setup)
{
    bool valid = true;
    bool internal = !setup;
    std::string errStr;

    if(internal)
        setup = new IniProcessing(iniFile);

    level.isInit = false;
    level.image = NULL;
    level.textureArrayId = 0;
    level.animator_ID = 0;
    setup->beginGroup(section);

    if(level.setup.parse(setup, wlvlPath, default_grid, merge_with ? &merge_with->setup : nullptr, &errStr))
        valid = true;
    else
    {
        addError(errStr);
        valid = false;
    }

    setup->endGroup();

    if(internal) delete setup;

    return valid;
}




bool ConfigManager::loadWorldTiles()
{
    unsigned int i;
    obj_w_tile stile;
    unsigned long tiles_total = 0;
    bool useDirectory = false;
    std::string tile_ini = config_dirSTD + "wld_tiles.ini";
    std::string nestDir = "";

    if(!Files::fileExists(tile_ini))
    {
        std::string msg = "ERROR LOADING wld_tiles.ini: file does not exist";
        addError(msg);
        PGE_MsgBox::fatal(msg);
        return false;
    }

    IniProcessing setup(tile_ini);
    wld_tiles.clear();   //Clear old
    setup.beginGroup("tiles-main");
    setup.read("total", tiles_total, 0);
    setup.read("config-dir", nestDir, "");

    if(!nestDir.empty())
    {
        nestDir = config_dirSTD + nestDir;
        useDirectory = true;
    }

    setup.endGroup();
    wld_tiles.allocateSlots(tiles_total);

    //    emit progressMax(tiles_total);
    //                    //% "Loading Tiles..."
    //    emit progressTitle(qtTrId("WLD_LOADING_TILES"));

    if(tiles_total == 0)
    {
        std::string msg = "ERROR LOADING wld_tiles.ini: number of items not define, or empty config";
        addError(msg);
        PGE_MsgBox::fatal(msg);
        return false;
    }

    stile.isInit = false;
    stile.image = NULL;
    stile.textureArrayId = 0;
    stile.animator_ID = 0;

    for(i = 1; i <= tiles_total; i++)
    {
        if(useDirectory)
        {
            if(!loadWorldTile(stile, "tile", nullptr, fmt::format("{0}/tile-{1}.ini", nestDir, i)))
                return false;
        }
        else
        {
            if(!loadWorldTile(stile, fmt::format("tile-{0}", i), nullptr, "", &setup))
                return false;
        }

        stile.setup.id = i;
        //Store loaded config
        wld_tiles.storeElement(stile.setup.id, stile);
        //Load custom config if possible
        loadCustomConfig<obj_w_tile>(wld_tiles, i, Dir_Tiles, "tile", "tile", &loadWorldTile);

        if(setup.lastError() != IniProcessing::ERR_OK)
        {
            std::string msg = fmt::format("ERROR LOADING wld_tiles.ini N:{0} (tile-{2})", setup.lastError(), i);
            addError(msg);
            PGE_MsgBox::error(msg);
            return false;
        }
    }

    if(wld_tiles.stored() < tiles_total)
    {
        std::string msg = fmt::format("Not all Tiles loaded! Total: {0}, Loaded: {1}", tiles_total, wld_tiles.size());
        addError(msg);
        PGE_MsgBox::warn(msg);
    }

    return true;
}


bool ConfigManager::loadWorldScenery()
{
    unsigned int i;
    obj_w_scenery sScene;
    unsigned long scenery_total = 0;
    bool useDirectory = false;
    std::string scene_ini = config_dirSTD + "wld_scenery.ini";
    std::string  nestDir = "";

    if(!Files::fileExists(scene_ini))
    {
        std::string msg = "ERROR LOADING wld_scenery.ini: file does not exist";
        addError(msg);
        PGE_MsgBox::fatal(msg);
        return false;
    }

    IniProcessing setup(scene_ini);
    wld_scenery.clear();   //Clear old
    setup.beginGroup("scenery-main");
    setup.read("total", scenery_total, 0);
    setup.read("config-dir", nestDir, "");

    if(!nestDir.empty())
    {
        nestDir = config_dirSTD + nestDir;
        useDirectory = true;
    }

    setup.endGroup();
    wld_scenery.allocateSlots(scenery_total);

    if(scenery_total == 0)
    {
        std::string msg = "ERROR LOADING wld_scenery.ini: number of items not define, or empty config";
        addError(msg);
        PGE_MsgBox::fatal(msg);
        return false;
    }

    sScene.isInit = false;
    sScene.image = NULL;
    sScene.textureArrayId = 0;
    sScene.animator_ID = 0;

    for(i = 1; i <= scenery_total; i++)
    {
        if(useDirectory)
        {
            if(!loadWorldScenery(sScene, "scenery", nullptr, fmt::format("{0}/scenery-{1}.ini", nestDir, i)))
                return false;
        }
        else
        {
            if(!loadWorldScenery(sScene, fmt::format("scenery-{0}", i), nullptr, "", &setup))
                return false;
        }

        sScene.setup.id = i;
        //Store loaded config
        wld_scenery.storeElement(sScene.setup.id, sScene);
        //Load custom config if possible
        loadCustomConfig<obj_w_scenery>(wld_scenery, i, Dir_Scenery, "scene", "scenery", &loadWorldScenery);

        if(setup.lastError() != IniProcessing::ERR_OK)
        {
            std::string msg = fmt::format("ERROR LOADING wld_scenery.ini N:{0} (scene-{1})", setup.lastError(), i);
            addError(msg);
            PGE_MsgBox::error(msg);
            return false;
        }
    }

    if(wld_scenery.stored() < scenery_total)
    {
        std::string msg = fmt::format("Not all Sceneries loaded! Total: {0}, Loaded: {1}", scenery_total, wld_scenery.stored());
        addError(msg);
        PGE_MsgBox::warn(msg);
    }

    return true;
}



bool ConfigManager::loadWorldPaths()
{
    unsigned int i;
    obj_w_path sPath;
    unsigned long path_total = 0;
    bool useDirectory = false;
    std::string scene_ini = config_dirSTD + "wld_paths.ini";
    std::string nestDir = "";

    if(!Files::fileExists(scene_ini))
    {
        std::string msg = "ERROR LOADING wld_paths.ini: file does not exist";
        addError(msg);
        PGE_MsgBox::fatal(msg);
        return false;
    }

    IniProcessing setup(scene_ini);
    wld_paths.clear();   //Clear old
    setup.beginGroup("path-main");
    path_total = setup.value("total", 0).toULongLong();
    nestDir =    setup.value("config-dir", "").toString();

    if(!nestDir.empty())
    {
        nestDir = config_dirSTD + nestDir;
        useDirectory = true;
    }

    setup.endGroup();

    if(path_total == 0)
    {
        std::string msg = "ERROR LOADING wld_paths.ini: number of items not define, or empty config";
        addError(msg);
        PGE_MsgBox::fatal(msg);
        return false;
    }

    wld_paths.allocateSlots(path_total);
    sPath.isInit = false;
    sPath.image = NULL;
    sPath.textureArrayId = 0;
    sPath.animator_ID = 0;

    for(i = 1; i <= path_total; i++)
    {
        if(useDirectory)
        {
            if(!loadWorldPath(sPath, "path", nullptr, fmt::format("{0}/path-{1}.ini", nestDir, i)))
                return false;
        }
        else
        {
            if(!loadWorldPath(sPath, fmt::format("path-{0}", i), nullptr, "", &setup))
                return false;
        }

        sPath.setup.id = i;
        //Store loaded config
        wld_paths.storeElement(sPath.setup.id, sPath);
        //Load custom config if possible
        loadCustomConfig<obj_w_path>(wld_paths, i, Dir_WldPaths, "path", "path", &loadWorldPath);

        if(setup.lastError() != IniProcessing::ERR_OK)
        {
            std::string msg = fmt::format("ERROR LOADING wld_paths.ini N:{0} (path-{1})", setup.lastError(), i);
            addError(msg);
            PGE_MsgBox::fatal(msg);
            return false;
        }
    }

    if(wld_paths.stored() < path_total)
    {
        std::string msg = fmt::format("Not all Sceneries loaded! Total: {0}, Loaded: {1}", path_total, wld_scenery.stored());
        addError(msg);
        PGE_MsgBox::warn(msg);
    }

    return true;
}


bool ConfigManager::loadWorldLevels()
{
    unsigned int i;
    obj_w_level slevel;
    unsigned long levels_total = 0;
    bool useDirectory = false;
    std::string level_ini = config_dirSTD + "wld_levels.ini";
    std::string nestDir = "";

    if(!Files::fileExists(level_ini))
    {
        std::string msg = "ERROR LOADING wld_levels.ini: file does not exist";
        addError(msg);
        PGE_MsgBox::fatal(msg);
        return false;
    }

    IniProcessing setup(level_ini);
    wld_levels.clear();   //Clear old
    setup.beginGroup("levels-main");
    levels_total =  setup.value("total", 0).toULongLong();
    nestDir =       setup.value("config-dir", "").toString();

    if(!nestDir.empty())
    {
        nestDir = config_dirSTD + nestDir;
        useDirectory = true;
    }

    marker_wlvl.path = setup.value("path", 0).toULongLong();
    marker_wlvl.bigpath = setup.value("bigpath", 0).toULongLong();
    setup.endGroup();

    if(levels_total == 0)
    {
        std::string msg = "ERROR LOADING wld_levels.ini: number of items not define, or empty config";
        addError(msg);
        PGE_MsgBox::fatal(msg);
        return false;
    }

    wld_levels.allocateSlots(levels_total);
    slevel.isInit = false;
    slevel.image = NULL;
    slevel.textureArrayId = 0;
    slevel.animator_ID = 0;

    for(i = 0; i <= levels_total; i++)
    {
        if(useDirectory)
        {
            if(!loadWorldLevel(slevel, "level", nullptr, fmt::format("{0}/level-{1}.ini", nestDir, i)))
                return false;
        }
        else
        {
            if(!loadWorldLevel(slevel, fmt::format("level-{0}", i), nullptr, "", &setup))
                return false;
        }

        slevel.setup.id = i;
        //Store loaded config
        wld_levels.storeElement(slevel.setup.id, slevel);
        //Load custom config if possible
        loadCustomConfig<obj_w_level>(wld_levels, i, Dir_WldLevel, "level", "level", &loadWorldLevel);

        if(setup.lastError() != IniProcessing::ERR_OK)
        {
            std::string msg = fmt::format("ERROR LOADING wld_levels.ini N:{0} (level-{1})", setup.lastError(), i);
            addError(msg);
            PGE_MsgBox::error(msg);
            return false;
        }
    }

    if(wld_levels.stored() < levels_total)
    {
        std::string msg = fmt::format("Not all Level images loaded! Total: {0}, Loaded: {1}", levels_total, wld_levels.stored());
        addError(msg);
        PGE_MsgBox::warn(msg);
    }

    return true;
}
