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

/*****World Tiles************/
PGE_DataArray<obj_w_tile>   ConfigManager::wld_tiles;
CustomDirManager         ConfigManager::Dir_Tiles;
QList<SimpleAnimator >   ConfigManager::Animator_Tiles;
/*****World Tiles************/

/*****World Scenery************/
PGE_DataArray<obj_w_scenery>   ConfigManager::wld_scenery;
CustomDirManager         ConfigManager::Dir_Scenery;
QList<SimpleAnimator >   ConfigManager::Animator_Scenery;
/*****World Scenery************/

/*****World Paths************/
PGE_DataArray<obj_w_path>   ConfigManager::wld_paths;
CustomDirManager         ConfigManager::Dir_WldPaths;
QList<SimpleAnimator >   ConfigManager::Animator_WldPaths;
/*****World Paths************/

/*****World Levels************/
PGE_DataArray<obj_w_level>  ConfigManager::wld_levels;
CustomDirManager         ConfigManager::Dir_WldLevel;
QList<SimpleAnimator >   ConfigManager::Animator_WldLevel;
wld_levels_Markers       ConfigManager::marker_wlvl;
/*****World Levels************/

bool ConfigManager::loadWorldTile(obj_w_tile &tile, QString section, obj_w_tile *merge_with, QString iniFile, QSettings *setup)
{
    bool valid = true;
    bool internal = !setup;
    QString errStr;

    if(internal)
    {
        setup = new QSettings(iniFile, QSettings::IniFormat);
        setup->setIniCodec("UTF-8");
    }

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

bool ConfigManager::loadWorldPath(obj_w_path &path, QString section, obj_w_path *merge_with, QString iniFile, QSettings *setup)
{
    bool valid = true;
    bool internal = !setup;
    QString errStr;

    if(internal)
    {
        setup = new QSettings(iniFile, QSettings::IniFormat);
        setup->setIniCodec("UTF-8");
    }

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

bool ConfigManager::loadWorldScenery(obj_w_scenery &scene, QString section, obj_w_scenery *merge_with, QString iniFile, QSettings *setup)
{
    bool valid = true;
    bool internal = !setup;
    QString errStr;

    if(internal)
    {
        setup = new QSettings(iniFile, QSettings::IniFormat);
        setup->setIniCodec("UTF-8");
    }

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

bool ConfigManager::loadWorldLevel(obj_w_level &level, QString section, obj_w_level *merge_with, QString iniFile, QSettings *setup)
{
    bool valid = true;
    bool internal = !setup;
    QString errStr;

    if(internal)
    {
        setup = new QSettings(iniFile, QSettings::IniFormat);
        setup->setIniCodec("UTF-8");
    }

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
    QString tile_ini = config_dir + "wld_tiles.ini";
    QString nestDir = "";

    if(!QFile::exists(tile_ini))
    {
        addError(QString("ERROR LOADING wld_tiles.ini: file does not exist"), QtCriticalMsg);
        PGE_MsgBox::fatal("ERROR LOADING wld_tiles.ini: file does not exist");
        return false;
    }

    QSettings setup(tile_ini, QSettings::IniFormat);
    setup.setIniCodec("UTF-8");
    wld_tiles.clear();   //Clear old
    setup.beginGroup("tiles-main");
    tiles_total = setup.value("total", 0).toULongLong();
    nestDir =     setup.value("config-dir", "").toString();

    if(!nestDir.isEmpty())
    {
        nestDir = config_dir + nestDir;
        useDirectory = true;
    }

    setup.endGroup();
    wld_tiles.allocateSlots(tiles_total);

    //    emit progressMax(tiles_total);
    //                    //% "Loading Tiles..."
    //    emit progressTitle(qtTrId("WLD_LOADING_TILES"));

    if(tiles_total == 0)
    {
        addError(QString("ERROR LOADING wld_tiles.ini: number of items not define, or empty config"), QtCriticalMsg);
        PGE_MsgBox::fatal("ERROR LOADING wld_tiles.ini: number of items not define, or empty config");
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
            if(!loadWorldTile(stile, "tile", nullptr, QString("%1/tile-%2.ini").arg(nestDir).arg(i)))
                return false;
        }
        else
        {
            if(!loadWorldTile(stile, QString("tile-%1").arg(i), nullptr, "", &setup))
                return false;
        }

        stile.setup.id = i;
        //Store loaded config
        wld_tiles.storeElement(stile.setup.id, stile);
        //Load custom config if possible
        loadCustomConfig<obj_w_tile>(wld_tiles, i, Dir_Tiles, "tile", "tile", &loadWorldTile);

        if(setup.status() != QSettings::NoError)
        {
            addError(QString("ERROR LOADING wld_tiles.ini N:%1 (tile-%2)").arg(setup.status()).arg(i), QtCriticalMsg);
            PGE_MsgBox::error(QString("ERROR LOADING wld_tiles.ini N:%1 (tile-%2)").arg(setup.status()).arg(i));
            return false;
        }
    }

    if(wld_tiles.stored() < tiles_total)
    {
        addError(QString("Not all Tiles loaded! Total: %1, Loaded: %2").arg(tiles_total).arg(wld_tiles.size()));
        PGE_MsgBox::warn(QString("Not all Tiles loaded! Total: %1, Loaded: %2").arg(tiles_total).arg(wld_tiles.size()));
    }

    return true;
}


bool ConfigManager::loadWorldScenery()
{
    unsigned int i;
    obj_w_scenery sScene;
    unsigned long scenery_total = 0;
    bool useDirectory = false;
    QString scene_ini = config_dir + "wld_scenery.ini";
    QString nestDir = "";

    if(!QFile::exists(scene_ini))
    {
        addError(QString("ERROR LOADING wld_scenery.ini: file does not exist"), QtCriticalMsg);
        PGE_MsgBox::fatal("ERROR LOADING wld_scenery.ini: file does not exist");
        return false;
    }

    QSettings setup(scene_ini, QSettings::IniFormat);
    setup.setIniCodec("UTF-8");
    wld_scenery.clear();   //Clear old
    setup.beginGroup("scenery-main");
    scenery_total = setup.value("total", 0).toULongLong();
    nestDir =       setup.value("config-dir", "").toString();

    if(!nestDir.isEmpty())
    {
        nestDir = config_dir + nestDir;
        useDirectory = true;
    }

    setup.endGroup();
    wld_scenery.allocateSlots(scenery_total);

    if(scenery_total == 0)
    {
        addError(QString("ERROR LOADING wld_scenery.ini: number of items not define, or empty config"), QtCriticalMsg);
        PGE_MsgBox::fatal("ERROR LOADING wld_scenery.ini: number of items not define, or empty config");
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
            if(!loadWorldScenery(sScene, "scenery", nullptr, QString("%1/scenery-%2.ini").arg(nestDir).arg(i)))
                return false;
        }
        else
        {
            if(!loadWorldScenery(sScene, QString("scenery-%1").arg(i), nullptr, "", &setup))
                return false;
        }

        sScene.setup.id = i;
        //Store loaded config
        wld_scenery.storeElement(sScene.setup.id, sScene);
        //Load custom config if possible
        loadCustomConfig<obj_w_scenery>(wld_scenery, i, Dir_Scenery, "scene", "scenery", &loadWorldScenery);

        if(setup.status() != QSettings::NoError)
        {
            addError(QString("ERROR LOADING wld_scenery.ini N:%1 (scene-%2)").arg(setup.status()).arg(i), QtCriticalMsg);
            PGE_MsgBox::error(QString("ERROR LOADING wld_scenery.ini N:%1 (scene-%2)").arg(setup.status()).arg(i));
            return false;
        }
    }

    if(wld_scenery.stored() < scenery_total)
    {
        addError(QString("Not all Sceneries loaded! Total: %1, Loaded: %2").arg(scenery_total).arg(wld_scenery.stored()));
        PGE_MsgBox::warn(QString("Not all Sceneries loaded! Total: %1, Loaded: %2").arg(scenery_total).arg(wld_scenery.stored()));
    }

    return true;
}



bool ConfigManager::loadWorldPaths()
{
    unsigned int i;
    obj_w_path sPath;
    unsigned long path_total = 0;
    bool useDirectory = false;
    QString scene_ini = config_dir + "wld_paths.ini";
    QString nestDir = "";

    if(!QFile::exists(scene_ini))
    {
        addError(QString("ERROR LOADING wld_paths.ini: file does not exist"), QtCriticalMsg);
        PGE_MsgBox::fatal("ERROR LOADING wld_paths.ini: file does not exist");
        return false;
    }

    QSettings setup(scene_ini, QSettings::IniFormat);
    setup.setIniCodec("UTF-8");
    wld_paths.clear();   //Clear old
    setup.beginGroup("path-main");
    path_total = setup.value("total", 0).toULongLong();
    nestDir =    setup.value("config-dir", "").toString();

    if(!nestDir.isEmpty())
    {
        nestDir = config_dir + nestDir;
        useDirectory = true;
    }

    setup.endGroup();

    if(path_total == 0)
    {
        addError(QString("ERROR LOADING wld_paths.ini: number of items not define, or empty config"), QtCriticalMsg);
        PGE_MsgBox::fatal("ERROR LOADING wld_paths.ini: number of items not define, or empty config");
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
            if(!loadWorldPath(sPath, "path", nullptr, QString("%1/path-%2.ini").arg(nestDir).arg(i)))
                return false;
        }
        else
        {
            if(!loadWorldPath(sPath, QString("path-%1").arg(i), nullptr, "", &setup))
                return false;
        }

        sPath.setup.id = i;
        //Store loaded config
        wld_paths.storeElement(sPath.setup.id, sPath);
        //Load custom config if possible
        loadCustomConfig<obj_w_path>(wld_paths, i, Dir_WldPaths, "path", "path", &loadWorldPath);

        if(setup.status() != QSettings::NoError)
        {
            addError(QString("ERROR LOADING wld_paths.ini N:%1 (path-%2)").arg(setup.status()).arg(i), QtCriticalMsg);
            PGE_MsgBox::fatal(QString("ERROR LOADING wld_paths.ini N:%1 (path-%2)").arg(setup.status()).arg(i));
            return false;
        }
    }

    if(wld_paths.stored() < path_total)
    {
        addError(QString("Not all Sceneries loaded! Total: %1, Loaded: %2").arg(path_total).arg(wld_scenery.stored()));
        PGE_MsgBox::warn(QString("Not all Sceneries loaded! Total: %1, Loaded: %2").arg(path_total).arg(wld_scenery.stored()));
    }

    return true;
}


bool ConfigManager::loadWorldLevels()
{
    unsigned int i;
    obj_w_level slevel;
    unsigned long levels_total = 0;
    bool useDirectory = false;
    QString level_ini = config_dir + "wld_levels.ini";
    QString nestDir = "";

    if(!QFile::exists(level_ini))
    {
        addError(QString("ERROR LOADING wld_levels.ini: file does not exist"), QtCriticalMsg);
        PGE_MsgBox::fatal("ERROR LOADING wld_levels.ini: file does not exist");
        return false;
    }

    QSettings setup(level_ini, QSettings::IniFormat);
    setup.setIniCodec("UTF-8");
    wld_levels.clear();   //Clear old
    setup.beginGroup("levels-main");
    levels_total =  setup.value("total", 0).toULongLong();
    nestDir =       setup.value("config-dir", "").toString();

    if(!nestDir.isEmpty())
    {
        nestDir = config_dir + nestDir;
        useDirectory = true;
    }

    marker_wlvl.path = setup.value("path", 0).toULongLong();
    marker_wlvl.bigpath = setup.value("bigpath", 0).toULongLong();
    setup.endGroup();

    if(levels_total == 0)
    {
        addError(QString("ERROR LOADING wld_levels.ini: number of items not define, or empty config"), QtCriticalMsg);
        PGE_MsgBox::fatal("ERROR LOADING wld_levels.ini: number of items not define, or empty config");
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
            if(!loadWorldLevel(slevel, "level", nullptr, QString("%1/level-%2.ini").arg(nestDir).arg(i)))
                return false;
        }
        else
        {
            if(!loadWorldLevel(slevel, QString("level-%1").arg(i), nullptr, "", &setup))
                return false;
        }

        slevel.setup.id = i;
        //Store loaded config
        wld_levels.storeElement(slevel.setup.id, slevel);
        //Load custom config if possible
        loadCustomConfig<obj_w_level>(wld_levels, i, Dir_WldLevel, "level", "level", &loadWorldLevel);

        if(setup.status() != QSettings::NoError)
        {
            addError(QString("ERROR LOADING wld_levels.ini N:%1 (level-%2)").arg(setup.status()).arg(i), QtCriticalMsg);
            PGE_MsgBox::error(QString("ERROR LOADING wld_levels.ini N:%1 (level-%2)").arg(setup.status()).arg(i));
            return false;
        }
    }

    if(wld_levels.stored() < levels_total)
    {
        addError(QString("Not all Level images loaded! Total: %1, Loaded: %2").arg(levels_total).arg(wld_levels.stored()));
        PGE_MsgBox::warn(QString("Not all Level images loaded! Total: %1, Loaded: %2").arg(levels_total).arg(wld_levels.stored()));
    }

    return true;
}
