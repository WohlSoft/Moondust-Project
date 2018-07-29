/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2014-2018 Vitaly Novichkov <admin@wohlnet.ru>
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

#include <common_features/graphics_funcs.h>
#include <common_features/items.h>
#include <editing/edit_world/world_edit.h>
#include <PGE_File_Formats/file_formats.h>

#include "../wld_scene.h"


//Search and load custom User's files
void WldScene::loadUserData(QProgressDialog &progress)
{
    int i;

    QImage tempImg;
    bool WrongImagesDetected=false;

    m_localConfigTerrain.clear();
    m_localConfigScenery.clear();
    m_localConfigPaths.clear();
    m_localConfigLevels.clear();

    //QString uWLDDs = WldData->path + "/" + WldData->filename + "/";
    //QString uWLDD = WldData->path + "/" + WldData->filename;
    //QString uWLDs = WldData->path + "/";
    CustomDirManager uWLD(m_data->meta.path, m_data->meta.filename);

    //Load custom rotation rules
    QString rTableFile = uWLD.getCustomFile("rotation_table.ini", true);
    if(!rTableFile.isEmpty())
    {
        QSettings rTableINI(rTableFile, QSettings::IniFormat);
        rTableINI.setIniCodec("UTF-8");

        QStringList rules = rTableINI.childGroups();

        int count=0;
        foreach(QString x, rules)
        {
            obj_rotation_table t;
            rTableINI.beginGroup(x);
                t.id = rTableINI.value("id", 0).toInt();
                t.type = Items::getItemType(rTableINI.value("type", "-1").toString());
                t.rotate_left = rTableINI.value("rotate-left", 0).toInt();
                t.rotate_right = rTableINI.value("rotate-right", 0).toInt();
                t.flip_h = rTableINI.value("flip-h", 0).toInt();
                t.flip_v = rTableINI.value("flip-v", 0).toInt();
            rTableINI.endGroup();
            if(t.id<=0) continue;

            if(t.type==ItemTypes::WLD_Tile)
            {
                local_rotation_table_tiles[t.id]=t;
                count++;
            }
            else
            if(t.type==ItemTypes::WLD_Scenery)
            {
                local_rotation_table_sceneries[t.id]=t;
                count++;
            }
            else
            if(t.type==ItemTypes::WLD_Path)
            {
                local_rotation_table_paths[t.id]=t;
                count++;
            }
            else
            if(t.type==ItemTypes::WLD_Level)
            {
                local_rotation_table_levels[t.id]=t;
                count++;
            }
        }
        qDebug() << "Loaded custom rotation rules: " << count;
    }

    if(!progress.wasCanceled())
    {
        progress.setLabelText(
                    tr("Search User Tiles %1")
                    .arg(QString::number(m_configs->main_wtiles.stored()) ) );

        progress.setValue(progress.value()+1);
    }
    qApp->processEvents();

    m_localConfigTerrain.allocateSlots(m_configs->main_wtiles.total());
    uWLD.setDefaultDir(m_configs->getTilePath());
    //Load Tiles
    for(i=1; i<m_configs->main_wtiles.size(); i++) //Add user images
    {
        obj_w_tile &tileD = m_configs->main_wtiles[i];
        obj_w_tile  t_tile;
        tileD.copyTo(t_tile);
        bool custom=false;

        QStringList customINIs;
        customINIs = uWLD.getCustomFiles("tile-" + QString::number(t_tile.setup.id), {".ini", ".txt"}, true);
        for(QString &iniFile : customINIs)
        {
            m_configs->loadWorldTerrain(t_tile, "tile", &t_tile, iniFile);
            custom = true;
        }

        QString CustomImage = uWLD.getCustomFile(t_tile.setup.image_n, true);
        if(! CustomImage.isEmpty() )
        {
            if(!CustomImage.endsWith(".png", Qt::CaseInsensitive))
            {
                QString CustomMask = uWLD.getCustomFile(t_tile.setup.mask_n, false);
                GraphicsHelps::loadQImage(tempImg, CustomImage, CustomMask);
            } else {
                GraphicsHelps::loadQImage(tempImg, CustomImage);
            }
            if(tempImg.isNull())
                WrongImagesDetected=true;
            else
            {
                m_localImages.push_back(QPixmap::fromImage(tempImg));
                t_tile.cur_image = &m_localImages.last();
            }
            custom=true;
        }

        SimpleAnimator * aniTile = new SimpleAnimator(
                            ((t_tile.cur_image->isNull())?
                            m_dummyTerrainImg : *t_tile.cur_image
                               ),
                              t_tile.setup.animated,
                              t_tile.setup.frames,
                              t_tile.setup.framespeed
                              );

        if(!t_tile.setup.frame_sequence.isEmpty())
            aniTile->setFrameSequance(t_tile.setup.frame_sequence);

        m_animatorsTerrain.push_back( aniTile );
        m_animationTimer.registerAnimation( aniTile );
        t_tile.animator_id = m_animatorsTerrain.size()-1;
        m_localConfigTerrain.storeElement(i, t_tile);
        if(custom)
        {
            m_customTerrain.push_back(&m_localConfigTerrain[i]);//Register Terrain tile as customized
        }

        if(progress.wasCanceled())
            return;
    }

///////////////////////////////////////////////////////////////////////////


    if(!progress.wasCanceled())
    {
        progress.setLabelText(
                    tr("Search User Sceneries %1")
                    .arg(QString::number(m_configs->main_wscene.size()) ) );

        progress.setValue(progress.value()+1);
    }
    qApp->processEvents();
    m_localConfigScenery.allocateSlots(m_configs->main_wscene.total());
    uWLD.setDefaultDir(m_configs->getScenePath());
    //Load Sceneries
    for(i=1; i<m_configs->main_wscene.size(); i++) //Add user images
    {
        obj_w_scenery &sceneryD = m_configs->main_wscene[i];
        obj_w_scenery  t_scenery;
        sceneryD.copyTo(t_scenery);
        bool custom=false;

        QStringList customINIs;
        customINIs = uWLD.getCustomFiles("scene-" + QString::number(t_scenery.setup.id), {".ini", ".txt"}, true);
        for(QString &iniFile : customINIs)
        {
            m_configs->loadWorldScene(t_scenery, "scenery", &t_scenery, iniFile);
            custom = true;
        }

        QString CustomImage = uWLD.getCustomFile(t_scenery.setup.image_n, true);
        if(! CustomImage.isEmpty() )
        {
            if(!CustomImage.endsWith(".png", Qt::CaseInsensitive))
            {
                QString CustomMask = uWLD.getCustomFile(t_scenery.setup.mask_n, false);
                GraphicsHelps::loadQImage(tempImg, CustomImage, CustomMask);
            } else {
                GraphicsHelps::loadQImage(tempImg, CustomImage);
            }
            if(tempImg.isNull())
                WrongImagesDetected=true;
            else
            {
                m_localImages.push_back(QPixmap::fromImage(tempImg));
                t_scenery.cur_image = &m_localImages.last();
            }
            custom=true;
        }

        SimpleAnimator * aniScene = new SimpleAnimator(
                            ((t_scenery.cur_image->isNull())?
                            m_dummySceneryImg : *t_scenery.cur_image
                               ),
                              t_scenery.setup.animated,
                              t_scenery.setup.frames,
                              t_scenery.setup.framespeed
                              );

        if(!t_scenery.setup.frame_sequence.isEmpty())
            aniScene->setFrameSequance(t_scenery.setup.frame_sequence);

        m_animatorsScenery.push_back( aniScene );
        m_animationTimer.registerAnimation( aniScene );
        t_scenery.animator_id = m_animatorsScenery.size()-1;
        m_localConfigScenery.storeElement(i, t_scenery);
        if(custom)
        {
            m_customSceneries.push_back(&m_localConfigScenery[i]);//Register Terrain tile as customized
        }

        if(progress.wasCanceled())
            return;
    }

///////////////////////////////////////////////////////////////////////////

    if(!progress.wasCanceled())
    {
        progress.setLabelText(
                    tr("Search User Paths %1")
                    .arg(QString::number(m_configs->main_wpaths.size()) ) );

        progress.setValue(progress.value()+1);
    }
    qApp->processEvents();
    m_localConfigPaths.allocateSlots(m_configs->main_wpaths.total());
    uWLD.setDefaultDir(m_configs->getPathPath());
    //Load Path tiles
    for(i=1; i<m_configs->main_wpaths.size(); i++) //Add user images
    {
        obj_w_path &pathD = m_configs->main_wpaths[i];
        obj_w_path t_path;
        pathD.copyTo(t_path);
        bool custom=false;

        QStringList customINIs;
        customINIs = uWLD.getCustomFiles("path-" + QString::number(t_path.setup.id), {".ini", ".txt"}, true);
        for(QString &iniFile : customINIs)
        {
            m_configs->loadWorldPath(t_path, "path", &t_path, iniFile);
            custom = true;
        }

        QString CustomImage = uWLD.getCustomFile(t_path.setup.image_n, true);
        if(! CustomImage.isEmpty() )
        {
            if(!CustomImage.endsWith(".png", Qt::CaseInsensitive))
            {
                QString CustomMask = uWLD.getCustomFile(t_path.setup.mask_n, false);
                GraphicsHelps::loadQImage(tempImg, CustomImage, CustomMask);
            } else {
                GraphicsHelps::loadQImage(tempImg, CustomImage);
            }
            if(tempImg.isNull())
                WrongImagesDetected=true;
            else
            {
                m_localImages.push_back(QPixmap::fromImage(tempImg));
                t_path.cur_image = &m_localImages.last();
            }
            custom=true;
        }

        SimpleAnimator * aniPath = new SimpleAnimator(
                            ((t_path.cur_image->isNull())?
                            m_dummyPathImg : *t_path.cur_image
                               ),
                              t_path.setup.animated,
                              t_path.setup.frames,
                              t_path.setup.framespeed
                              );

        if(!t_path.setup.frame_sequence.isEmpty())
            aniPath->setFrameSequance(t_path.setup.frame_sequence);

        m_animatorsPaths.push_back( aniPath );
        m_animationTimer.registerAnimation( aniPath );
        t_path.animator_id = m_animatorsPaths.size()-1;
        m_localConfigPaths.storeElement(i, t_path);
        if(custom)
        {
            m_customPaths.push_back(&m_localConfigPaths[i]);//Register Terrain tile as customized
        }

        if(progress.wasCanceled())
            return;
    }

///////////////////////////////////////////////////////////////////////////

    if(!progress.wasCanceled())
    {
        progress.setLabelText(
                    tr("Search User Levels %1")
                    .arg(QString::number(m_configs->main_wlevels.size()) ) );

        progress.setValue(progress.value()+1);
    }
    qApp->processEvents();
    m_localConfigLevels.allocateSlots(m_configs->main_wlevels.total());
    uWLD.setDefaultDir(m_configs->getWlvlPath());

    //Load Level tiles
    for(i=0; i<m_configs->main_wlevels.size(); i++)//Add user images
    {
        obj_w_level &levelD = m_configs->main_wlevels[i];
        obj_w_level t_level;
        levelD.copyTo(t_level);
        bool custom=false;

        QStringList customINIs;
        customINIs = uWLD.getCustomFiles("level-" + QString::number(t_level.setup.id), {".ini", ".txt"}, true);
        for(QString &iniFile : customINIs)
        {
            m_configs->loadWorldLevel(t_level, "level", &t_level, iniFile);
            custom = true;
        }

        QString CustomImage = uWLD.getCustomFile(t_level.setup.image_n, true);
        if(! CustomImage.isEmpty() )
        {
            if(!CustomImage.endsWith(".png", Qt::CaseInsensitive))
            {
                QString CustomMask = uWLD.getCustomFile(t_level.setup.mask_n, false);
                GraphicsHelps::loadQImage(tempImg, CustomImage, CustomMask);
            } else {
                GraphicsHelps::loadQImage(tempImg, CustomImage);
            }
            if(tempImg.isNull())
                WrongImagesDetected=true;
            else
            {
                m_localImages.push_back(QPixmap::fromImage(tempImg));
                t_level.cur_image = &m_localImages.last();
            }
            custom=true;
        }

        SimpleAnimator * aniLevel = new SimpleAnimator(
                            ((t_level.cur_image->isNull())?
                            m_dummyLevelImg : *t_level.cur_image
                               ),
                              t_level.setup.animated,
                              t_level.setup.frames,
                              t_level.setup.framespeed
                              );

        if(!t_level.setup.frame_sequence.isEmpty())
            aniLevel->setFrameSequance(t_level.setup.frame_sequence);

        m_animatorsLevels.push_back( aniLevel );
        m_animationTimer.registerAnimation( aniLevel );
        t_level.animator_id = m_animatorsLevels.size()-1;
        m_localConfigLevels.storeElement(i, t_level);
        if(custom)
        {
            m_customLevels.push_back(&m_localConfigLevels[i]);//Register Terrain tile as customized
        }

        if(progress.wasCanceled())
            return;
    }

///////////////////////////////////////////////////////////////////////////

    progress.setValue(progress.value()+1);
    qApp->processEvents();

    //Notification about wrong custom image sprites
    if(WrongImagesDetected)
    {
        QMessageBox * msg = new QMessageBox();
        msg->setWindowFlags(msg->windowFlags() | Qt::WindowStaysOnTopHint);
        msg->setWindowTitle(tr("Wrong custom images"));
        msg->setText(tr("This level have a wrong custom graphics files.\nYou will see 'ghosties' or other dummy images instead custom GFX of items, what used broken images. It occurred because, for example, the BMP format with GIF extension was used.\nPlease, reconvert your images to valid format and try to reload this level."));
        msg->addButton(QMessageBox::Ok);
        msg->setIcon(QMessageBox::Warning);

        msg->exec();
    }
}

