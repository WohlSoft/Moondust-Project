/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2014-2015 Vitaly Novichkov <admin@wohlnet.ru>
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
#include "../gui/pge_msgbox.h"
#include <common_features/graphics_funcs.h>

/*****World Tiles************/
QMap<long, obj_w_tile>   ConfigManager::wld_tiles;
CustomDirManager         ConfigManager::Dir_Tiles;
QList<SimpleAnimator >   ConfigManager::Animator_Tiles;
/*****World Tiles************/

/*****World Scenery************/
QMap<long, obj_w_scenery>   ConfigManager::wld_scenery;
CustomDirManager         ConfigManager::Dir_Scenery;
QList<SimpleAnimator >   ConfigManager::Animator_Scenery;
/*****World Scenery************/

/*****World Paths************/
QMap<long, obj_w_path>   ConfigManager::wld_paths;
CustomDirManager         ConfigManager::Dir_WldPaths;
QList<SimpleAnimator >   ConfigManager::Animator_WldPaths;
/*****World Paths************/

/*****World Levels************/
QMap<long, obj_w_level>  ConfigManager::wld_levels;
CustomDirManager         ConfigManager::Dir_WldLevel;
QList<SimpleAnimator >   ConfigManager::Animator_WldLevel;
wld_levels_Markers       ConfigManager::marker_wlvl;
/*****World Levels************/


bool ConfigManager::loadWorldTiles()
{
    unsigned int i;

    obj_w_tile stile;
    unsigned long tiles_total=0;
    QString tile_ini = config_dir + "wld_tiles.ini";

    if(!QFile::exists(tile_ini))
    {
        addError(QString("ERROR LOADING wld_tiles.ini: file does not exist"), QtCriticalMsg);
        PGE_MsgBox::fatal("ERROR LOADING wld_tiles.ini: file does not exist");
        return false;
    }

    QSettings tileset(tile_ini, QSettings::IniFormat);
    tileset.setIniCodec("UTF-8");

    wld_tiles.clear();   //Clear old

    tileset.beginGroup("tiles-main");
        tiles_total = tileset.value("total", "0").toInt();
        total_data +=tiles_total;
    tileset.endGroup();

//    emit progressMax(tiles_total);
//    emit progressTitle(QApplication::tr("Loading Tiles..."));

    if(tiles_total==0)
    {
        addError(QString("ERROR LOADING wld_tiles.ini: number of items not define, or empty config"), QtCriticalMsg);
        PGE_MsgBox::fatal("ERROR LOADING wld_tiles.ini: number of items not define, or empty config");
        return false;
    }

    stile.isInit = false;
    stile.image = NULL;
    stile.textureArrayId = 0;
    stile.animator_ID = 0;

    for(i=1; i<=tiles_total; i++)
    {
        //emit progressValue(i);
        //QString errStr;

        tileset.beginGroup( QString("tile-"+QString::number(i)) );

        stile.group = tileset.value("group", "_NoGroup").toString();
        stile.category = tileset.value("category", "_Other").toString();

        stile.image_n = tileset.value("image", "").toString();
        /***************Load image*******************/
        imgFile = tileset.value("image", "").toString();
        {
            QString err;
            GraphicsHelps::loadMaskedImage(tilePath, imgFile, stile.mask_n, err);
            stile.image_n = imgFile;
            if( imgFile=="" )
            {
                addError(QString("TILE-%1 Image filename isn't defined.\n%2").arg(i).arg(err));
                goto skipTile;
            }
        }
        /***************Load image*end***************/

        stile.grid =            tileset.value("grid", default_grid).toInt();

        stile.animated =       (tileset.value("animated", "0").toString()=="1");
        stile.frames =          tileset.value("frames", "1").toInt();
        stile.framespeed =      tileset.value("frame-speed", "125").toInt();

        stile.frame_h = 0;

        stile.display_frame =   tileset.value("display-frame", "0").toInt();
        stile.row =             tileset.value("row", "0").toInt();
        stile.col =             tileset.value("col", "0").toInt();


        stile.id = i;
        wld_tiles[stile.id] = stile;

        skipTile:
        tileset.endGroup();

        if( tileset.status() != QSettings::NoError )
        {
            addError(QString("ERROR LOADING wld_tiles.ini N:%1 (tile-%2)").arg(tileset.status()).arg(i), QtCriticalMsg);
            PGE_MsgBox::error(QString("ERROR LOADING wld_tiles.ini N:%1 (tile-%2)").arg(tileset.status()).arg(i));
            return false;
        }
    }

    if((unsigned int)wld_tiles.size()<tiles_total)
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
    unsigned long scenery_total=0;
    QString scene_ini = config_dir + "wld_scenery.ini";

    if(!QFile::exists(scene_ini))
    {
        addError(QString("ERROR LOADING wld_scenery.ini: file does not exist"), QtCriticalMsg);
        PGE_MsgBox::fatal("ERROR LOADING wld_scenery.ini: file does not exist");
        return false;
    }

    QSettings sceneset(scene_ini, QSettings::IniFormat);
    sceneset.setIniCodec("UTF-8");

    wld_scenery.clear();   //Clear old

    sceneset.beginGroup("scenery-main");
        scenery_total = sceneset.value("total", "0").toInt();
        total_data +=scenery_total;
    sceneset.endGroup();

    if(scenery_total==0)
    {
        addError(QString("ERROR LOADING wld_scenery.ini: number of items not define, or empty config"), QtCriticalMsg);
        PGE_MsgBox::fatal("ERROR LOADING wld_scenery.ini: number of items not define, or empty config");
        return false;
    }

    sScene.isInit = false;
    sScene.image = NULL;
    sScene.textureArrayId = 0;
    sScene.animator_ID = 0;

    for(i=1; i<=scenery_total; i++)
    {
        sceneset.beginGroup( QString("scenery-"+QString::number(i)) );

            sScene.group =         sceneset.value("group", "_NoGroup").toString();
            sScene.category =      sceneset.value("category", "_Other").toString();

            sScene.image_n =       sceneset.value("image", "").toString();
            /***************Load image*******************/
            imgFile = sceneset.value("image", "").toString();
            {
                QString err;
                GraphicsHelps::loadMaskedImage(scenePath, imgFile, sScene.mask_n, err);
                sScene.image_n = imgFile;
                if( imgFile=="" )
                {
                    addError(QString("SCENERY-%1 Image filename isn't defined.\n%2").arg(i).arg(err));
                    goto skipScene;
                }
            }
            /***************Load image*end***************/

            sScene.grid =          sceneset.value("grid", qRound(qreal(default_grid)/2)).toInt();

            sScene.animated =     (sceneset.value("animated", "0").toString()=="1");
            sScene.frames =        sceneset.value("frames", "1").toInt();
            sScene.framespeed =    sceneset.value("frame-speed", "125").toInt();

            sScene.frame_h = 0;

            sScene.display_frame = sceneset.value("display-frame", "0").toInt();



            sScene.id = i;
            wld_scenery[sScene.id] = sScene;

        skipScene:
        sceneset.endGroup();

        if( sceneset.status() != QSettings::NoError )
        {
            addError(QString("ERROR LOADING wld_scenery.ini N:%1 (scene-%2)").arg(sceneset.status()).arg(i), QtCriticalMsg);
            PGE_MsgBox::error(QString("ERROR LOADING wld_scenery.ini N:%1 (scene-%2)").arg(sceneset.status()).arg(i));
            return false;
        }
    }

    if((unsigned int)wld_scenery.size()<scenery_total)
    {
        addError(QString("Not all Sceneries loaded! Total: %1, Loaded: %2").arg(scenery_total).arg(wld_scenery.size()));
        PGE_MsgBox::warn(QString("Not all Sceneries loaded! Total: %1, Loaded: %2").arg(scenery_total).arg(wld_scenery.size()));
    }
    return true;
}



bool ConfigManager::loadWorldPaths()
{
    unsigned int i;

    obj_w_path sPath;
    unsigned long path_total=0;
    QString scene_ini = config_dir + "wld_paths.ini";

    if(!QFile::exists(scene_ini))
    {
        addError(QString("ERROR LOADING wld_paths.ini: file does not exist"), QtCriticalMsg);
        PGE_MsgBox::fatal("ERROR LOADING wld_paths.ini: file does not exist");
        return false;
    }

    QSettings pathset(scene_ini, QSettings::IniFormat);
    pathset.setIniCodec("UTF-8");

    wld_paths.clear();   //Clear old

    pathset.beginGroup("path-main");
        path_total = pathset.value("total", "0").toInt();
        total_data +=path_total;
    pathset.endGroup();

    if(path_total==0)
    {
        addError(QString("ERROR LOADING wld_paths.ini: number of items not define, or empty config"), QtCriticalMsg);
        PGE_MsgBox::fatal("ERROR LOADING wld_paths.ini: number of items not define, or empty config");
        return false;
    }


    sPath.isInit = false;
    sPath.image = NULL;
    sPath.textureArrayId = 0;
    sPath.animator_ID = 0;

    for(i=1; i<=path_total; i++)
    {
        pathset.beginGroup( QString("path-"+QString::number(i)) );

            sPath.group =       pathset.value("group", "_NoGroup").toString();
            sPath.category =    pathset.value("category", "_Other").toString();

            sPath.image_n =     pathset.value("image", "").toString();
            /***************Load image*******************/
            imgFile = pathset.value("image", "").toString();
            {
                QString err;
                GraphicsHelps::loadMaskedImage(pathPath, imgFile, sPath.mask_n, err);
                sPath.image_n = imgFile;
                if( imgFile=="" )
                {
                    addError(QString("PATH-%1 Image filename isn't defined.\n%2").arg(i).arg(err));
                    goto skipPath;
                }
            }
            /***************Load image*end***************/

            sPath.grid =            pathset.value("grid", default_grid).toInt();

            sPath.animated =       (pathset.value("animated", "0").toString()=="1");
            sPath.frames =          pathset.value("frames", "1").toInt();
            sPath.framespeed =      pathset.value("frame-speed", "125").toInt();

            sPath.frame_h = 0;

            sPath.display_frame=    pathset.value("display-frame", "0").toInt();
            sPath.row =             pathset.value("row", "0").toInt();
            sPath.col =             pathset.value("col", "0").toInt();


            sPath.id = i;
            wld_paths[sPath.id]=sPath;

        skipPath:
        pathset.endGroup();

        if( pathset.status() != QSettings::NoError )
        {
            addError(QString("ERROR LOADING wld_paths.ini N:%1 (path-%2)").arg(pathset.status()).arg(i), QtCriticalMsg);
            PGE_MsgBox::fatal(QString("ERROR LOADING wld_paths.ini N:%1 (path-%2)").arg(pathset.status()).arg(i));
            return false;
        }
    }

    if((unsigned int)wld_paths.size()<path_total)
    {
        addError(QString("Not all Sceneries loaded! Total: %1, Loaded: %2").arg(path_total).arg(wld_scenery.size()));
        PGE_MsgBox::warn(QString("Not all Sceneries loaded! Total: %1, Loaded: %2").arg(path_total).arg(wld_scenery.size()));
    }
    return false;
}



bool ConfigManager::loadWorldLevels()
{
    unsigned int i;

    obj_w_level slevel;
    unsigned long levels_total=0;
    QString level_ini = config_dir + "wld_levels.ini";

    if(!QFile::exists(level_ini))
    {
        addError(QString("ERROR LOADING wld_levels.ini: file does not exist"), QtCriticalMsg);
        PGE_MsgBox::fatal("ERROR LOADING wld_levels.ini: file does not exist");
        return false;
    }


    QSettings levelset(level_ini, QSettings::IniFormat);
    levelset.setIniCodec("UTF-8");

    wld_levels.clear();   //Clear old

    levelset.beginGroup("levels-main");
        levels_total = levelset.value("total", "0").toInt();
        marker_wlvl.path = levelset.value("path", "0").toInt();
        marker_wlvl.bigpath = levelset.value("bigpath", "0").toInt();
        total_data +=levels_total;
    levelset.endGroup();

    if(levels_total==0)
    {
        addError(QString("ERROR LOADING wld_levels.ini: number of items not define, or empty config"), QtCriticalMsg);
        PGE_MsgBox::fatal("ERROR LOADING wld_levels.ini: number of items not define, or empty config");
        return false;
    }

    slevel.isInit = false;
    slevel.image = NULL;
    slevel.textureArrayId = 0;
    slevel.animator_ID = 0;

    for(i=0; i<=levels_total; i++)
    {
        levelset.beginGroup( QString("level-"+QString::number(i)) );

        slevel.group =      levelset.value("group", "_NoGroup").toString();
        slevel.category =   levelset.value("category", "_Other").toString();

        slevel.image_n =    levelset.value("image", "").toString();
        /***************Load image*******************/
        imgFile = levelset.value("image", "").toString();
        {
            QString err;
            GraphicsHelps::loadMaskedImage(wlvlPath, imgFile, slevel.mask_n, err);
            slevel.image_n = imgFile;
            if( imgFile=="" )
            {
                addError(QString("LEVEL-%1 Image filename isn't defined.\n%2").arg(i).arg(err));
                goto skipLevel;
            }
        }
        /***************Load image*end***************/

        slevel.grid =       levelset.value("grid", default_grid).toInt();

        slevel.animated =  (levelset.value("animated", "0").toString()=="1");
        slevel.frames =     levelset.value("frames", "1").toInt();
        slevel.framespeed = levelset.value("frame-speed", "125").toInt();

        slevel.frame_h =   0;

        slevel.display_frame = levelset.value("display-frame", "0").toInt();


        slevel.id = i;
        wld_levels[slevel.id]=slevel;

        skipLevel:
        levelset.endGroup();

        if( levelset.status() != QSettings::NoError )
        {
            addError(QString("ERROR LOADING wld_levels.ini N:%1 (level-%2)").arg(levelset.status()).arg(i), QtCriticalMsg);
            PGE_MsgBox::error(QString("ERROR LOADING wld_levels.ini N:%1 (level-%2)").arg(levelset.status()).arg(i));
            return false;
        }
    }

    if((unsigned int)wld_levels.size()<levels_total)
    {
        addError(QString("Not all Level images loaded! Total: %1, Loaded: %2").arg(levels_total).arg(wld_levels.size()));
        PGE_MsgBox::warn(QString("Not all Level images loaded! Total: %1, Loaded: %2").arg(levels_total).arg(wld_levels.size()));
    }
    return true;
}

