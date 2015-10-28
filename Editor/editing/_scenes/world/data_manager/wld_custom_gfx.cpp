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

#include <common_features/graphics_funcs.h>
#include <common_features/items.h>
#include <editing/edit_world/world_edit.h>
#include <PGE_File_Formats/file_formats.h>

#include "../wld_scene.h"


//Search and load custom User's files
void WldScene::loadUserData(QProgressDialog &progress)
{
    int i;

    UserIMGs uTile;
    UserIMGs uScenery;
    UserIMGs uPath;
    UserIMGs uLevel;

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
    QString rTableFile = uWLD.getCustomFile("rotation_table.ini");
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
                    .arg(QString::number(pConfigs->main_wtiles.size()) ) );

        progress.setValue(progress.value()+1);
    }
    qApp->processEvents();


    //Load Tiles
    for(i=0; i<pConfigs->main_wtiles.size(); i++) //Add user images
    {
        bool custom=false;

            QString CustomImg = uWLD.getCustomFile(pConfigs->main_wtiles[i].image_n);
            if(!CustomImg.isEmpty())
            {
                if(!CustomImg.endsWith(".png", Qt::CaseInsensitive))
                {
                    QString CustomMask = uWLD.getCustomFile(pConfigs->main_wtiles[i].mask_n);
                    if(!CustomMask.isEmpty())
                        uTile.mask = GraphicsHelps::loadPixmap( CustomMask );
                    else
                        uTile.mask = pConfigs->main_wtiles[i].mask;
                    uTile.image = GraphicsHelps::mergeToRGBA(GraphicsHelps::loadPixmap( CustomImg ), uTile.mask);
                } else {
                    uTile.image = GraphicsHelps::loadPixmap( CustomImg );
                    uTile.mask=QPixmap();
                }

                if(uTile.image.isNull()) WrongImagesDetected=true;

                uTile.mask = QPixmap(); //!< Clear mask for save RAM space (for Huge images)

                uTile.id = pConfigs->main_wtiles[i].id;
                uTiles.push_back(uTile);
                custom=true;

                //Apply index;
                if(uTile.id < (unsigned int)index_tiles.size())
                {
                    index_tiles[uTile.id].type = 1;
                    //index_tiles[uTile.id].i = (uTiles.size()-1);
                }
            }

//            WriteToLog(QtDebugMsg, QString("SimpleAnimator").arg(i));

            SimpleAnimator * aniTile = new SimpleAnimator(
                        ((custom)?
                             ((uTiles.last().image.isNull())?
                                uTileImg:
                                    uTiles.last().image)
                                 :
                             ((pConfigs->main_wtiles[i].image.isNull())?
                                uTileImg:
                                   pConfigs->main_wtiles[i].image)
                             ),
                                  pConfigs->main_wtiles[i].animated,
                                  pConfigs->main_wtiles[i].frames,
                                  pConfigs->main_wtiles[i].framespeed
                                  );
            animates_Tiles.push_back( aniTile );
            index_tiles[pConfigs->main_wtiles[i].id].i = i;
            index_tiles[pConfigs->main_wtiles[i].id].ai = animates_Tiles.size()-1;

//            WriteToLog(QtDebugMsg, QString("SimpleProgress").arg(i));
        if(progress.wasCanceled())
            return;

//        WriteToLog(QtDebugMsg, QString("done").arg(i));
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


    //Load Sceneries
    for(i=0; i<pConfigs->main_wscene.size(); i++) //Add user images
    {

        bool custom=false;
            QString CustomImg = uWLD.getCustomFile(pConfigs->main_wscene[i].image_n);
            if(!CustomImg.isEmpty())
            {
                if(!CustomImg.endsWith(".png", Qt::CaseInsensitive))
                {
                    QString CustomMask = uWLD.getCustomFile(pConfigs->main_wscene[i].mask_n);
                    if(!CustomMask.isEmpty())
                        uScenery.mask = GraphicsHelps::loadPixmap( CustomMask );
                    else
                        uScenery.mask = pConfigs->main_wscene[i].mask;
                    uScenery.image = GraphicsHelps::mergeToRGBA(GraphicsHelps::loadPixmap( CustomImg ), uScenery.mask);
                } else {
                    uScenery.image = GraphicsHelps::loadPixmap( CustomImg );
                    uScenery.mask = QPixmap();
                }
                if(uScenery.image.isNull()) WrongImagesDetected=true;

                uScenery.mask = QPixmap(); //!< Clear mask for save RAM space (for Huge images)

                uScenery.id = pConfigs->main_wscene[i].id;
                uScenes.push_back(uScenery);
                custom=true;

                //Apply index;
                if(uScenery.id < (unsigned int)index_scenes.size())
                {
                    index_scenes[uScenery.id].type = 1;
                    //index_scenes[uScenery.id].i = (uScenes.size()-1);
                }
            }

            SimpleAnimator * aniScene = new SimpleAnimator(
                        ((custom)?
                             ((uScenes.last().image.isNull())?
                                uSceneImg:
                                    uScenes.last().image)
                                 :
                             ((pConfigs->main_wscene[i].image.isNull())?
                                uSceneImg:
                                   pConfigs->main_wscene[i].image)
                             ),
                                  pConfigs->main_wscene[i].animated,
                                  pConfigs->main_wscene[i].frames,
                                  pConfigs->main_wscene[i].framespeed
                                  );
            animates_Scenery.push_back( aniScene );
            index_scenes[pConfigs->main_wscene[i].id].i = i;
            index_scenes[pConfigs->main_wscene[i].id].ai = animates_Scenery.size()-1;

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


    //Load Tiles
    for(i=0; i<pConfigs->main_wpaths.size(); i++) //Add user images
    {

        bool custom=false;
            QString CustomImg = uWLD.getCustomFile(pConfigs->main_wpaths[i].image_n);
            if( !CustomImg.isEmpty() )
            {
                if(!CustomImg.endsWith(".png", Qt::CaseInsensitive))
                {
                    QString CustomMask = uWLD.getCustomFile(pConfigs->main_wpaths[i].mask_n);
                    if(!CustomMask.isEmpty())
                        uPath.mask = GraphicsHelps::loadPixmap( CustomMask );
                    else
                        uPath.mask = pConfigs->main_wpaths[i].mask;
                    uPath.image = GraphicsHelps::mergeToRGBA(GraphicsHelps::loadPixmap( CustomImg ), uPath.mask);
                } else {
                    uPath.image = GraphicsHelps::loadPixmap( CustomImg );
                    uPath.mask = QPixmap();
                }
                if(uPath.image.isNull()) WrongImagesDetected=true;

                uPath.mask = QPixmap(); //!< Clear mask for save RAM space (for Huge images)

                uPath.id = pConfigs->main_wpaths[i].id;
                uPaths.push_back(uPath);
                custom=true;

                //Apply index;
                if(uPath.id < (unsigned int)index_paths.size())
                {
                    index_paths[uPath.id].type = 1;
                    //index_paths[uPath.id].i = (uPaths.size()-1);
                }
            }

            SimpleAnimator * aniPath = new SimpleAnimator(
                        ((custom)?
                             ((uPaths.last().image.isNull())?
                                uPathImg:
                                    uPaths.last().image)
                                 :
                             ((pConfigs->main_wpaths[i].image.isNull())?
                                uPathImg:
                                   pConfigs->main_wpaths[i].image)
                             ),
                                  pConfigs->main_wpaths[i].animated,
                                  pConfigs->main_wpaths[i].frames,
                                  pConfigs->main_wpaths[i].framespeed
                                  );
            animates_Paths.push_back( aniPath );
            index_paths[pConfigs->main_wpaths[i].id].i = i;
            index_paths[pConfigs->main_wpaths[i].id].ai = animates_Paths.size()-1;

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


    //Load Tiles
    for(i=0; i<pConfigs->main_wlevels.size(); i++) //Add user images
    {

        bool custom=false;
            QString CustomImg = uWLD.getCustomFile(pConfigs->main_wlevels[i].image_n);
            if(!CustomImg.isEmpty())
            {
                if(!CustomImg.endsWith(".png", Qt::CaseInsensitive))
                {
                    QString CustomMask = uWLD.getCustomFile(pConfigs->main_wlevels[i].mask_n);
                    if(!CustomMask.isEmpty())
                        uLevel.mask = GraphicsHelps::loadPixmap( CustomMask );
                    else
                        uLevel.mask = pConfigs->main_wlevels[i].mask;
                    uLevel.image = GraphicsHelps::mergeToRGBA(GraphicsHelps::loadPixmap( CustomImg ), uLevel.mask);
                } else {
                    uLevel.image = GraphicsHelps::loadPixmap( CustomImg );
                    uLevel.mask = QPixmap();
                }
                if(uLevel.image.isNull()) WrongImagesDetected=true;

                uLevel.mask = QPixmap(); //!< Clear mask for save RAM space (for Huge images)

                uLevel.id = pConfigs->main_wlevels[i].id;
                uLevels.push_back(uLevel);
                custom=true;

                //Apply index;
                if(uLevel.id < (unsigned int)index_levels.size())
                {
                    index_levels[uLevel.id].type = 1;
                    //index_levels[uLevel.id].i = (uLevels.size()-1);
                }
            }

            SimpleAnimator * aniLevel = new SimpleAnimator(
                        ((custom)?
                             ((uLevels.last().image.isNull())?
                                uLevelImg:
                                    uLevels.last().image)
                                 :
                             ((pConfigs->main_wlevels[i].image.isNull())?
                                uLevelImg:
                                   pConfigs->main_wlevels[i].image)
                             ),
                                  pConfigs->main_wlevels[i].animated,
                                  pConfigs->main_wlevels[i].frames,
                                  pConfigs->main_wlevels[i].framespeed
                                  );
            animates_Levels.push_back( aniLevel );
            index_levels[pConfigs->main_wlevels[i].id].i = i;
            index_levels[pConfigs->main_wlevels[i].id].ai = animates_Levels.size()-1;

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

