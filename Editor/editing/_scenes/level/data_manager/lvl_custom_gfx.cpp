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
#include <editing/edit_level/level_edit.h>
#include <PGE_File_Formats/file_formats.h>
#include <data_functions/npctxt_manager.h>

#include "../../../../defines.h"

#include "../lvl_scene.h"


//Search and load custom User's files
void LvlScene::loadUserData(QProgressDialog &progress)
{
    int i, total=0;

    UserNPCs uNPC;
    QImage tempImg;

    bool WrongImagesDetected=false;

    uBlocks.clear();
    uBGOs.clear();
    uNPCs.clear();

    uBGs.clear();

    bool loaded1, loaded2;

    CustomDirManager uLVL(LvlData->path, LvlData->filename);

    //Load custom rotation rules
    QString rTableFile = uLVL.getCustomFile("rotation_table.ini");
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

            if(t.type==ItemTypes::LVL_Block)
            {
                local_rotation_table_blocks[t.id]=t;
                count++;
            }
            else
            if(t.type==ItemTypes::LVL_BGO)
            {
                local_rotation_table_bgo[t.id]=t;
                count++;
            }
            else
            if(t.type==ItemTypes::LVL_NPC)
            {
                local_rotation_table_npc[t.id]=t;
                count++;
            }
        }
        qDebug() << "Loaded custom rotation rules: " << count;
    }

    if(!progress.wasCanceled())
        progress.setLabelText(
                    tr("Search User Backgrounds %1")
                    .arg(QString::number(pConfigs->main_bg.stored()) ) );

    qApp->processEvents();
    uLVL.setDefaultDir(pConfigs->getBGPath());
    //Load Backgrounds
    for(int i=1; i<pConfigs->main_bg.size(); i++)
        {
            loaded1 = false;
            loaded2 = false;
            obj_BG *bgD = &pConfigs->main_bg[i];
            UserBGs uBG;

            QString CustomTxt=uLVL.getCustomFile("background2-" + QString::number(bgD->id)+".txt", true);
            if(!CustomTxt.isEmpty())
            {
                uBGs[bgD->id]=*bgD;
                obj_BG &bgN=uBGs[bgD->id];
                pConfigs->loadLevelBackground(bgN, "background2", bgD, CustomTxt);
                bgD=&bgN;
            }

            //check for first image
            QString CustomFile=uLVL.getCustomFile(bgD->image_n, true);
            if(!CustomFile.isEmpty())
            {
                GraphicsHelps::loadQImage(tempImg, CustomFile);
                if(tempImg.isNull())
                    WrongImagesDetected=true;
                else
                    uBG.image = std::move(QPixmap::fromImage(tempImg));
                uBG.id = bgD->id;
                loaded1 = true;
            }

            if((loaded1)&&(bgD->animated) )
            {
                uBG.image=uBG.image.copy(0, 0, uBG.image.width(), (int)round(uBG.image.height()/bgD->frames));
            }

            //check for second image
            if(bgD->type == 1)
            {
                QString CustomFile=uLVL.getCustomFile(bgD->second_image_n, true);
                if(!CustomFile.isEmpty())
                {
                    GraphicsHelps::loadQImage(tempImg, CustomFile);
                    if(tempImg.isNull())
                        WrongImagesDetected=true;
                    else
                        uBG.second_image = std::move(QPixmap::fromImage(tempImg));
                    uBG.id = bgD->id;
                    loaded2 = true;
                }
            }

            if((loaded1)&&(!loaded2)) uBG.q = 0;
            if((!loaded1)&&(loaded2)) uBG.q = 1;
            if((loaded1)&&(loaded2)) uBG.q = 2;

            //If user images found and loaded
            if( (loaded1) || (loaded2) )
            {
                if(!uBGs.contains(uBG.id)) uBGs[uBG.id]=*bgD;
                obj_BG &bgU=uBGs[uBG.id];
                if(loaded1) bgU.image=uBG.image;
                if(loaded2) bgU.second_image=uBG.second_image;
            }

        total++;
        qApp->processEvents(QEventLoop::ExcludeUserInputEvents);
        if(progress.wasCanceled())
            /*progress.setValue(progress.value()+1);
        else*/ return;
        }

///////////////////////////////////////////////////////////////////////////

    if(!progress.wasCanceled())
    {
        progress.setLabelText(
                    tr("Search User Blocks %1")
                    .arg(QString::number(pConfigs->main_block.stored()) ) );
        progress.setValue(progress.value()+1);
    }
    uBlocks.allocateSlots(pConfigs->main_block.total());
    qApp->processEvents();
    uLVL.setDefaultDir(pConfigs->getBlockPath());
    //Load Blocks
    for(i=1; i<pConfigs->main_block.size(); i++) //Add user images
    {
        obj_block *blockD = &pConfigs->main_block[i];
        obj_block t_block;
        blockD->copyTo(t_block);

        bool custom=false;

            QString CustomTxt = uLVL.getCustomFile("block-" + QString::number(blockD->id)+".ini", true);
            if(CustomTxt.isEmpty())
                CustomTxt=uLVL.getCustomFile("block-" + QString::number(blockD->id)+".txt", true);
            if(!CustomTxt.isEmpty())
            {
                //Not implemented yet!
                //pConfigs->loadLevelBlock(t_block, "block", blockD, CustomTxt);
                //custom=true;
            }

            QString CustomFile=uLVL.getCustomFile(t_block.image_n, true);
            if(!CustomFile.isEmpty())
            {
                if(!CustomFile.endsWith(".png", Qt::CaseInsensitive))
                {
                    QString CustomMask = uLVL.getCustomFile(t_block.mask_n, false);
                    GraphicsHelps::loadQImage(tempImg, CustomFile, CustomMask);
                } else {
                    GraphicsHelps::loadQImage(tempImg, CustomFile);
                }
                if(tempImg.isNull())
                    WrongImagesDetected=true;
                else
                {
                    custom_images.push_back(QPixmap::fromImage(tempImg));
                    t_block.cur_image = &custom_images.last();
                }
                custom=true;
            }

            int frameFirst;
            int frameLast;

            switch(t_block.algorithm)
            {
                case 1: // Invisible block
                {
                    frameFirst = 5;
                    frameLast = 6;
                    break;
                }
                case 3: //Player's character block
                {
                    frameFirst = 0;
                    frameLast = 1;
                    break;
                }
                case 4: //Player's character switch
                {
                    frameFirst = 0;
                    frameLast = 3;
                    break;
                }
                default: //Default block
                {
                    frameFirst = 0;
                    frameLast = -1;
                    break;
                }
            }

            SimpleAnimator * aniBlock = new SimpleAnimator(
                        ((t_block.cur_image->isNull())?
                                uBlockImg : *t_block.cur_image),
                                  t_block.animated,
                                  t_block.frames,
                                  t_block.framespeed, frameFirst,frameLast,
                                  t_block.animation_rev,
                                  t_block.animation_bid
                                  );

            t_block.animator_id = animates_Blocks.size();
            animates_Blocks.push_back( aniBlock );

            uBlocks.storeElement(i, t_block);
            if(custom)
            {
                custom_Blocks.push_back(&uBlocks[i]);//Register BGO as customized
            }

            #ifdef _DEBUG_
                WriteToLog(QtDebugMsg, QString("BGO Animator ID: %1").arg(index_bgo[pConfigs->main_bgo[i].id].ai));
            #endif
    qApp->processEvents(QEventLoop::ExcludeUserInputEvents);
    if(progress.wasCanceled())
        /*progress.setValue(progress.value()+1);
    else*/ return;
    }

///////////////////////////////////////////////////////////////////////////

    if(!progress.wasCanceled())
    {
        progress.setLabelText(
                    tr("Search User BGOs %1")
                    .arg(QString::number(pConfigs->main_bgo.stored()) ) );

        progress.setValue(progress.value()+1);
    }

    qApp->processEvents();
    uLVL.setDefaultDir(pConfigs->getBgoPath());
    //Load BGO
    uBGOs.allocateSlots(pConfigs->main_bgo.total());
    for(int i=1; i<pConfigs->main_bgo.size(); i++)
    {
        obj_bgo *bgoD = &pConfigs->main_bgo[i];
        obj_bgo t_bgo; //Allocate new BGO Config entry
        bool custom=false;

        bgoD->copyTo(t_bgo);//init configs

        QString CustomTxt = uLVL.getCustomFile("background-" + QString::number(bgoD->id)+".ini", true);
        if(CustomTxt.isEmpty())
            CustomTxt=uLVL.getCustomFile("background-" + QString::number(bgoD->id)+".txt", true);
        if(!CustomTxt.isEmpty())
        {
            pConfigs->loadLevelBGO(t_bgo, "background", bgoD, CustomTxt);
            custom=true;
        }

        QString CustomImage=uLVL.getCustomFile(t_bgo.image_n, true);
        if(!CustomImage.isEmpty())
        {
            if(!CustomImage.endsWith(".png", Qt::CaseInsensitive))
            {
                QString CustomMask = uLVL.getCustomFile(t_bgo.mask_n, false);
                GraphicsHelps::loadQImage(tempImg, CustomImage, CustomMask);
            } else {
                GraphicsHelps::loadQImage(tempImg, CustomImage);
            }
            if(tempImg.isNull())
                WrongImagesDetected=true;
            else
            {
                custom_images.push_back(QPixmap::fromImage(tempImg));
                t_bgo.cur_image = &custom_images.last();
            }
            custom=true;
        }

        SimpleAnimator * aniBGO = new SimpleAnimator(
                    ((t_bgo.cur_image->isNull())?
                            uBgoImg : *t_bgo.cur_image
                               ),
                              t_bgo.animated,
                              t_bgo.frames,
                              t_bgo.framespeed
                              );
            t_bgo.animator_id = animates_BGO.size();

            animates_BGO.push_back( aniBGO );

        uBGOs.storeElement(i, t_bgo);
        if(custom)
        {
            custom_BGOs.push_back(&uBGOs[i]);//Register BGO as customized
        }

        qApp->processEvents(QEventLoop::ExcludeUserInputEvents);
        if(progress.wasCanceled())
            /*progress.setValue(progress.value()+1);
        else*/ return;
    }

///////////////////////////////////////////////////////////////////////////


    if(!progress.wasCanceled())
    {
        progress.setLabelText(
                    tr("Search User NPCs %1")
                    .arg(QString::number(pConfigs->main_npc.stored()) ) );

        progress.setValue(progress.value()+1);
    }
    qApp->processEvents();
    uLVL.setDefaultDir(pConfigs->getNpcPath());
    //Load NPC
    uNPCs.allocateSlots(pConfigs->main_npc.total());
    NPCConfigFile sets;

    for(i=1; i<pConfigs->main_npc.size(); i++) //Add user images
    {
        obj_npc *npcD = &pConfigs->main_npc[i];
        obj_npc t_npc; //Allocate new NPC Config entry
        bool custom=false;
        bool cimage=false;
        bool npctxt=false;

        npcD->copyTo(t_npc);//init configs

             QSize capturedS = QSize(0,0);

             if(!t_npc.cur_image->isNull())
                 capturedS = t_npc.cur_image->size();

             // /////////////////////// Looking for user's NPC.txt ////////////////////////////
             // //(for use custom image filename, need to parse NPC.txt before iamges)/////////
             QString CustomTxt=uLVL.getCustomFile("npc-" + QString::number(t_npc.id)+".txt", true);
             if(!CustomTxt.isEmpty())
             {
                QFile file(CustomTxt);
                if(file.open(QIODevice::ReadOnly))
                {
                    file.close();
                    sets = FileFormats::ReadNpcTXTFile(CustomTxt, true);
                    npctxt = true;
                    custom = true;
                }
             }
             QString imgFileName = (npctxt && sets.en_image) ? sets.image : t_npc.image_n;

             // ///////////////////////Looking for user's GFX
             QString CustomImage=uLVL.getCustomFile(imgFileName, true);
             if(CustomImage.isEmpty())
                CustomImage=uLVL.getCustomFile(t_npc.image_n, true);

             if(!CustomImage.isEmpty())
             {
                 if(!CustomImage.endsWith(".png", Qt::CaseInsensitive))
                 {
                     QString CustomMask = uLVL.getCustomFile(t_npc.mask_n, false);
                     GraphicsHelps::loadQImage(tempImg, CustomImage, CustomMask);
                 } else {
                     GraphicsHelps::loadQImage(tempImg, CustomImage);
                 }
                 if(tempImg.isNull())
                 {
                     WrongImagesDetected=true;
                 } else {
                     custom_images.push_back(QPixmap::fromImage(tempImg));
                     t_npc.cur_image = &custom_images.last();
                     capturedS = QSize(t_npc.cur_image->width(), t_npc.cur_image->height());
                 }
                 cimage=true;
                 custom=true;
             }

             if(npctxt)
             {  //Merge global and user's settings from NPC.txt file
                 t_npc = mergeNPCConfigs(t_npc, sets, capturedS);
             }
             else
             {
                 if(cimage)
                 {
                     NPCConfigFile autoConf = FileFormats::CreateEmpytNpcTXT();
                     autoConf.gfxwidth = capturedS.width();
                     t_npc = mergeNPCConfigs(
                                 t_npc,
                                 autoConf, capturedS);
                 }
             }

//             //Apply only if custom config or image was found
//             if(custom)
//             {
//                 //Apply index;
//                 if(uNPC.id < (unsigned int)index_npc.size())
//                 {
//                     index_npc[uNPC.id].type = 1;
//                     index_npc[uNPC.id].i = (uNPCs.size()-1);
//                 }
//             }
             AdvNpcAnimator * aniNPC = new AdvNpcAnimator(
                         ((t_npc.cur_image->isNull())?
                              uNpcImg:
                              *t_npc.cur_image),
                         t_npc );

             t_npc.animator_id = animates_NPC.size();
             animates_NPC.push_back( aniNPC );

             uNPCs.storeElement(i, t_npc);
             if(custom)
             {
                 custom_NPCs.push_back(&uNPCs[i]);
             }

         qApp->processEvents(QEventLoop::ExcludeUserInputEvents);
         if(progress.wasCanceled())
             return;
    }

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

QPixmap LvlScene::getNPCimg(unsigned long npcID, int Direction)
{
    bool found=false;
    found = (npcID>0) && uNPCs.contains(npcID);

    if(!found)
    {
        return uNpcImg;
    }

    int gfxH = 0;
    obj_npc &merged = uNPCs[npcID];
    found = merged.isValid;
    gfxH  = merged.gfx_h;

    if(merged.cur_image->isNull())
    {
        return uNpcImg;
    }

    if(Direction<=0)
    {
        int frame=0;
        if(merged.custom_animate)
        {
            frame=merged.custom_ani_fl;
        }
        return merged.cur_image->copy(0,frame*gfxH, merged.cur_image->width(), gfxH );
    }
    else
    {
        int frame=0;
        int framesQ;
        if(merged.custom_animate)
        {
            frame=merged.custom_ani_fr;
        }
        else
        {
            switch(merged.framestyle)
            {
            case 2:
                framesQ = merged.frames * 4;
                frame = (int)(framesQ-(framesQ/4)*3);
                break;
            case 1:
                framesQ = merged.frames * 2;
                frame = (int)(framesQ / 2);
                break;
            case 0:
            default:
                break;
            }

        }
        return merged.cur_image->copy(0,frame*gfxH, merged.cur_image->width(), gfxH );
    }
}
