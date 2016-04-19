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

    uTiles.clear();
    uScenes.clear();
    uPaths.clear();
    uLevels.clear();

    //QString uWLDDs = WldData->path + "/" + WldData->filename + "/";
    //QString uWLDD = WldData->path + "/" + WldData->filename;
    //QString uWLDs = WldData->path + "/";
    CustomDirManager uWLD(WldData->path, WldData->filename);

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
                    .arg(QString::number(pConfigs->main_wtiles.stored()) ) );

        progress.setValue(progress.value()+1);
    }
    qApp->processEvents();

    uTiles.allocateSlots(pConfigs->main_wtiles.total());
    uWLD.setDefaultDir(pConfigs->getTilePath());
    //Load Tiles
    for(i=1; i<pConfigs->main_wtiles.size(); i++) //Add user images
    {
        obj_w_tile &tileD = pConfigs->main_wtiles[i];
        obj_w_tile  t_tile;
        tileD.copyTo(t_tile);
        bool custom=false;

        QString CustomTxt = uWLD.getCustomFile("tile-" + QString::number(t_tile.id)+".ini", true);
        if(CustomTxt.isEmpty())
            CustomTxt=uWLD.getCustomFile("tile-" + QString::number(t_tile.id)+".txt", true);
        if(!CustomTxt.isEmpty())
        {
            //FIXME! Implement that at config pack side and uncommend those lines
            //pConfigs->loadWorldTile(t_tile, "tile", tileD, CustomTxt);
            //custom=true;
        }

        QString CustomImage = uWLD.getCustomFile(t_tile.image_n, true);
        if(! CustomImage.isEmpty() )
        {
            if(!CustomImage.endsWith(".png", Qt::CaseInsensitive))
            {
                QString CustomMask = uWLD.getCustomFile(t_tile.mask_n, false);
                GraphicsHelps::loadQImage(tempImg, CustomImage, CustomMask);
            } else {
                GraphicsHelps::loadQImage(tempImg, CustomImage);
            }
            if(tempImg.isNull())
                WrongImagesDetected=true;
            else
            {
                custom_images.push_back(QPixmap::fromImage(tempImg));
                t_tile.cur_image = &custom_images.last();
            }
            custom=true;
        }

        SimpleAnimator * aniTile = new SimpleAnimator(
                            ((t_tile.cur_image->isNull())?
                            uTileImg : *t_tile.cur_image
                               ),
                              t_tile.animated,
                              t_tile.frames,
                              t_tile.framespeed
                              );
        animates_Tiles.push_back( aniTile );
        animator.registerAnimation( aniTile );
        t_tile.animator_id = animates_Tiles.size()-1;
        uTiles.storeElement(i, t_tile);
        if(custom)
        {
            custom_Tiles.push_back(&uTiles[i]);//Register Terrain tile as customized
        }

        if(progress.wasCanceled())
            return;
    }

///////////////////////////////////////////////////////////////////////////


    if(!progress.wasCanceled())
    {
        progress.setLabelText(
                    tr("Search User Sceneries %1")
                    .arg(QString::number(pConfigs->main_wscene.size()) ) );

        progress.setValue(progress.value()+1);
    }
    qApp->processEvents();
    uScenes.allocateSlots(pConfigs->main_wscene.total());
    uWLD.setDefaultDir(pConfigs->getScenePath());
    //Load Sceneries
    for(i=1; i<pConfigs->main_wscene.size(); i++) //Add user images
    {
        obj_w_scenery &sceneryD = pConfigs->main_wscene[i];
        obj_w_scenery  t_scenery;
        sceneryD.copyTo(t_scenery);
        bool custom=false;

        QString CustomTxt = uWLD.getCustomFile("scene-" + QString::number(t_scenery.id)+".ini", true);
        if(CustomTxt.isEmpty())
            CustomTxt=uWLD.getCustomFile("scene-" + QString::number(t_scenery.id)+".txt", true);
        if(!CustomTxt.isEmpty())
        {
            //FIXME! Implement that at config pack side and uncommend those lines
            //pConfigs->loadWorldScenery(t_scenery, "scene", sceneryD, CustomTxt);
            //custom=true;
        }

        QString CustomImage = uWLD.getCustomFile(t_scenery.image_n, true);
        if(! CustomImage.isEmpty() )
        {
            if(!CustomImage.endsWith(".png", Qt::CaseInsensitive))
            {
                QString CustomMask = uWLD.getCustomFile(t_scenery.mask_n, false);
                GraphicsHelps::loadQImage(tempImg, CustomImage, CustomMask);
            } else {
                GraphicsHelps::loadQImage(tempImg, CustomImage);
            }
            if(tempImg.isNull())
                WrongImagesDetected=true;
            else
            {
                custom_images.push_back(QPixmap::fromImage(tempImg));
                t_scenery.cur_image = &custom_images.last();
            }
            custom=true;
        }

        SimpleAnimator * aniTile = new SimpleAnimator(
                            ((t_scenery.cur_image->isNull())?
                            uSceneImg : *t_scenery.cur_image
                               ),
                              t_scenery.animated,
                              t_scenery.frames,
                              t_scenery.framespeed
                              );
        animates_Scenery.push_back( aniTile );
        animator.registerAnimation( aniTile );
        t_scenery.animator_id = animates_Scenery.size()-1;
        uScenes.storeElement(i, t_scenery);
        if(custom)
        {
            custom_Scenes.push_back(&uScenes[i]);//Register Terrain tile as customized
        }

        if(progress.wasCanceled())
            return;
    }

///////////////////////////////////////////////////////////////////////////

    if(!progress.wasCanceled())
    {
        progress.setLabelText(
                    tr("Search User Paths %1")
                    .arg(QString::number(pConfigs->main_wpaths.size()) ) );

        progress.setValue(progress.value()+1);
    }
    qApp->processEvents();
    uPaths.allocateSlots(pConfigs->main_wpaths.total());
    uWLD.setDefaultDir(pConfigs->getPathPath());
    //Load Path tiles
    for(i=1; i<pConfigs->main_wpaths.size(); i++) //Add user images
    {
        obj_w_path &pathD = pConfigs->main_wpaths[i];
        obj_w_path t_path;
        pathD.copyTo(t_path);
        bool custom=false;

        QString CustomTxt = uWLD.getCustomFile("path-" + QString::number(t_path.id)+".ini", true);
        if(CustomTxt.isEmpty())
            CustomTxt=uWLD.getCustomFile("path-" + QString::number(t_path.id)+".txt", true);
        if(!CustomTxt.isEmpty())
        {
            //FIXME! Implement that at config pack side and uncommend those lines
            //pConfigs->loadWorldPath(t_path, "path", pathD, CustomTxt);
            //custom=true;
        }

        QString CustomImage = uWLD.getCustomFile(t_path.image_n, true);
        if(! CustomImage.isEmpty() )
        {
            if(!CustomImage.endsWith(".png", Qt::CaseInsensitive))
            {
                QString CustomMask = uWLD.getCustomFile(t_path.mask_n, false);
                GraphicsHelps::loadQImage(tempImg, CustomImage, CustomMask);
            } else {
                GraphicsHelps::loadQImage(tempImg, CustomImage);
            }
            if(tempImg.isNull())
                WrongImagesDetected=true;
            else
            {
                custom_images.push_back(QPixmap::fromImage(tempImg));
                t_path.cur_image = &custom_images.last();
            }
            custom=true;
        }

        SimpleAnimator * aniTile = new SimpleAnimator(
                            ((t_path.cur_image->isNull())?
                            uPathImg : *t_path.cur_image
                               ),
                              t_path.animated,
                              t_path.frames,
                              t_path.framespeed
                              );
        animates_Paths.push_back( aniTile );
        animator.registerAnimation( aniTile );
        t_path.animator_id = animates_Paths.size()-1;
        uPaths.storeElement(i, t_path);
        if(custom)
        {
            custom_Paths.push_back(&uPaths[i]);//Register Terrain tile as customized
        }

        if(progress.wasCanceled())
            return;
    }

///////////////////////////////////////////////////////////////////////////

    if(!progress.wasCanceled())
    {
        progress.setLabelText(
                    tr("Search User Levels %1")
                    .arg(QString::number(pConfigs->main_wlevels.size()) ) );

        progress.setValue(progress.value()+1);
    }
    qApp->processEvents();
    uLevels.allocateSlots(pConfigs->main_wlevels.total());
    uWLD.setDefaultDir(pConfigs->getWlvlPath());

    //Load Level tiles
    for(i=0; i<pConfigs->main_wlevels.size(); i++)//Add user images
    {
        obj_w_level &levelD = pConfigs->main_wlevels[i];
        obj_w_level t_level;
        levelD.copyTo(t_level);
        bool custom=false;

        QString CustomTxt = uWLD.getCustomFile("level-" + QString::number(t_level.id)+".ini", true);
        if(CustomTxt.isEmpty())
            CustomTxt=uWLD.getCustomFile("level-" + QString::number(t_level.id)+".txt", true);
        if(!CustomTxt.isEmpty())
        {
            //FIXME! Implement that at config pack side and uncommend those lines
            //pConfigs->loadWorldLevel(t_level, "level", sceneryD, CustomTxt);
            //custom=true;
        }

        QString CustomImage = uWLD.getCustomFile(t_level.image_n, true);
        if(! CustomImage.isEmpty() )
        {
            if(!CustomImage.endsWith(".png", Qt::CaseInsensitive))
            {
                QString CustomMask = uWLD.getCustomFile(t_level.mask_n, false);
                GraphicsHelps::loadQImage(tempImg, CustomImage, CustomMask);
            } else {
                GraphicsHelps::loadQImage(tempImg, CustomImage);
            }
            if(tempImg.isNull())
                WrongImagesDetected=true;
            else
            {
                custom_images.push_back(QPixmap::fromImage(tempImg));
                t_level.cur_image = &custom_images.last();
            }
            custom=true;
        }

        SimpleAnimator * aniTile = new SimpleAnimator(
                            ((t_level.cur_image->isNull())?
                            uLevelImg : *t_level.cur_image
                               ),
                              t_level.animated,
                              t_level.frames,
                              t_level.framespeed
                              );
        animates_Levels.push_back( aniTile );
        animator.registerAnimation( aniTile );
        t_level.animator_id = animates_Levels.size()-1;
        uLevels.storeElement(i, t_level);
        if(custom)
        {
            custom_Levels.push_back(&uLevels[i]);//Register Terrain tile as customized
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

