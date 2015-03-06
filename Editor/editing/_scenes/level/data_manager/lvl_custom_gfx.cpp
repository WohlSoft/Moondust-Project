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

#include "../../../../defines.h"

#include "../lvl_scene.h"


//Search and load custom User's files
void LvlScene::loadUserData(QProgressDialog &progress)
{
    int i, total=0;

    UserBGs uBG;
    UserBlocks uBlock;
    UserBGOs uBGO;
    UserNPCs uNPC;

    bool WrongImagesDetected=false;

    uBGOs.clear();
    uBlocks.clear();
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
                    .arg(QString::number(pConfigs->main_bg.size()) ) );

    qApp->processEvents();
    //Load Backgrounds
    for(i=0; i<pConfigs->main_bg.size(); i++) //Add user images
        {
            loaded1 = false;
            loaded2 = false;

            //check for first image
            QString CustomFile=uLVL.getCustomFile(pConfigs->main_bg[i].image_n);
            if(!CustomFile.isEmpty())
            {
                uBG.image = GraphicsHelps::loadPixmap( CustomFile );
                uBG.id = pConfigs->main_bg[i].id;
                if(uBG.image.isNull()) WrongImagesDetected=true;
                loaded1 = true;
            }

            if((loaded1)&&(pConfigs->main_bg[i].animated) )
            {
                uBG.image=uBG.image.copy(0, 0, uBG.image.width(), (int)round(uBG.image.height()/pConfigs->main_bg[i].frames));
            }

            //check for second image
            if(pConfigs->main_bg[i].type == 1)
            {
                QString CustomFile=uLVL.getCustomFile(pConfigs->main_bg[i].second_image_n);
                if(!CustomFile.isEmpty())
                {
                    uBG.second_image = GraphicsHelps::loadPixmap( CustomFile );
                    uBG.id = pConfigs->main_bg[i].id;
                    loaded2 = true;
                    if(uBG.second_image.isNull()) WrongImagesDetected=true;
                }
            }

            if((loaded1)&&(!loaded2)) uBG.q = 0;
            if((!loaded1)&&(loaded2)) uBG.q = 1;
            if((loaded1)&&(loaded2)) uBG.q = 2;


            //If user images found and loaded
            if( (loaded1) || (loaded2) )
                uBGs.push_back(uBG);

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
                    .arg(QString::number(pConfigs->main_block.size()) ) );
        progress.setValue(progress.value()+1);
    }
    qApp->processEvents();
    //Load Blocks
    for(i=0; i<pConfigs->main_block.size(); i++) //Add user images
    {

        bool custom=false;

            QString CustomFile=uLVL.getCustomFile(pConfigs->main_block[i].image_n);
            if(!CustomFile.isEmpty())
            {
                QString CustomMask=uLVL.getCustomFile(pConfigs->main_block[i].mask_n);
                if(!CustomMask.isEmpty())
                    uBlock.mask = GraphicsHelps::loadPixmap( CustomMask );
                else
                    uBlock.mask = pConfigs->main_block[i].mask;

                uBlock.image = GraphicsHelps::setAlphaMask(GraphicsHelps::loadPixmap(CustomFile), uBlock.mask);
                if(uBlock.image.isNull()) WrongImagesDetected=true;

                uBlock.mask = QPixmap(); //!< Clear mask for save RAM space (for Huge images)

                uBlock.id = pConfigs->main_block[i].id;
                uBlocks.push_back(uBlock);
                custom=true;

                //Apply index;
                if(uBlock.id < (unsigned int)index_blocks.size())
                {
                    index_blocks[uBlock.id].type = 1;
                    //index_blocks[uBlock.id].i = (uBlocks.size()-1);
                }
            }

            int frameFirst;
            int frameLast;

            switch(pConfigs->main_block[i].algorithm)
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
                        ((custom)?
                             ((uBlocks.last().image.isNull())?
                                uBgoImg:
                                    uBlocks.last().image)
                                 :
                             ((pConfigs->main_block[i].image.isNull())?
                                uBgoImg:
                                   pConfigs->main_block[i].image)
                             ),
                                  pConfigs->main_block[i].animated,
                                  pConfigs->main_block[i].frames,
                                  pConfigs->main_block[i].framespeed, frameFirst,frameLast,
                                  pConfigs->main_block[i].animation_rev,
                                  pConfigs->main_block[i].animation_bid
                                  );

            animates_Blocks.push_back( aniBlock );
            index_blocks[pConfigs->main_block[i].id].i = i;
            index_blocks[pConfigs->main_block[i].id].ai = animates_Blocks.size()-1;

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
                    .arg(QString::number(pConfigs->main_bgo.size()) ) );

        progress.setValue(progress.value()+1);
    }
    qApp->processEvents();
    //Load BGO
    for(i=0; i<pConfigs->main_bgo.size(); i++) //Add user images
    {

        bool custom=false;
            QString CustomFile=uLVL.getCustomFile(pConfigs->main_bgo[i].image_n);
            if(!CustomFile.isEmpty())
            {
                QString CustomMask=uLVL.getCustomFile(pConfigs->main_bgo[i].mask_n);
                if(!CustomMask.isEmpty())
                    uBGO.mask = GraphicsHelps::loadPixmap( CustomMask );
                else
                    uBGO.mask = pConfigs->main_bgo[i].mask;

                uBGO.image = GraphicsHelps::setAlphaMask(GraphicsHelps::loadPixmap(CustomFile), uBGO.mask);
                if(uBGO.image.isNull()) WrongImagesDetected=true;

                uBGO.mask = QPixmap(); //!< Clear mask for save RAM space (for Huge images)

                uBGO.id = pConfigs->main_bgo[i].id;
                uBGOs.push_back(uBGO);
                custom=true;

                //Apply index;
                if(uBGO.id < (unsigned int)index_bgo.size())
                {
                    index_bgo[uBGO.id].type = 1;
                    index_bgo[uBGO.id].i = (uBGOs.size()-1);
                }
            }

            SimpleAnimator * aniBGO = new SimpleAnimator(
                        ((custom)?
                             ((uBGOs.last().image.isNull())?
                                uBgoImg:
                                    uBGOs.last().image)
                                 :
                             ((pConfigs->main_bgo[i].image.isNull())?
                                uBgoImg:
                                   pConfigs->main_bgo[i].image)
                             ),
                                  pConfigs->main_bgo[i].animated,
                                  pConfigs->main_bgo[i].frames,
                                  pConfigs->main_bgo[i].framespeed
                                  );
            animates_BGO.push_back( aniBGO );
            index_bgo[pConfigs->main_bgo[i].id].i = i;
            index_bgo[pConfigs->main_bgo[i].id].ai = animates_BGO.size()-1;
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
                    tr("Search User NPCs %1")
                    .arg(QString::number(pConfigs->main_npc.size()) ) );

        progress.setValue(progress.value()+1);
    }
    qApp->processEvents();

    //Load NPC
    for(i=0; i<pConfigs->main_npc.size(); i++) //Add user images
    {
        if(!progress.wasCanceled())
            progress.setLabelText(
                        tr("Search User NPCs %1")
                        .arg(QString::number(i+1)+"/"+QString::number(pConfigs->main_npc.size()) ) );

             uNPC.withImg = false;
             uNPC.withTxt = false;

             QSize capturedS = QSize(0,0);

             if(!pConfigs->main_npc[i].image.isNull())
                 capturedS = pConfigs->main_npc[i].image.size();

             // /////////////////////// Looking for user's NPC.txt ////////////////////////////
             // //(for use custom image filename, need to parse NPC.txt before iamges)/////////
             QString CustomTxt=uLVL.getCustomFile("npc-" + QString::number(pConfigs->main_npc[i].id)+".txt");
             if(!CustomTxt.isEmpty())
             {
                QFile file(CustomTxt);
                if(file.open(QIODevice::ReadOnly))
                {
                    uNPC.sets = FileFormats::ReadNpcTXTFile(file, true);
                    uNPC.id = pConfigs->main_npc[i].id;
                    uNPC.withTxt = true;
                }
             }

             // ///////////////////////Looking for user's GFX
             QString CustomImg=uLVL.getCustomFile(pConfigs->main_npc[i].image_n);
             if(!CustomImg.isEmpty())
             {
                 QString CustomMask=uLVL.getCustomFile(pConfigs->main_npc[i].mask_n);
                 if(!CustomMask.isEmpty())
                     uNPC.mask = GraphicsHelps::loadPixmap( CustomMask );
                 else
                     uNPC.mask = pConfigs->main_npc[i].mask;

                 uNPC.image = GraphicsHelps::setAlphaMask(GraphicsHelps::loadPixmap( CustomImg ), uNPC.mask);
                 if(uNPC.image.isNull()) WrongImagesDetected=true;

                 uNPC.mask = QPixmap(); //!< Clear mask for save RAM space (for Huge images)

                 uNPC.id = pConfigs->main_npc[i].id;
                 uNPC.withImg = true;
             }

             if(uNPC.withImg)
             {
                 capturedS = QSize(uNPC.image.width(), uNPC.image.height());
             }

             if(uNPC.withTxt)
             {  //Merge global and user's settings from NPC.txt file
                 uNPC.merged = FileFormats::mergeNPCConfigs(pConfigs->main_npc[i], uNPC.sets, capturedS);
             }
             else
             {
                 if(uNPC.withImg)
                 {
                     NPCConfigFile autoConf = FileFormats::CreateEmpytNpcTXTArray();

                     autoConf.gfxwidth = capturedS.width();

                     uNPC.merged = FileFormats::mergeNPCConfigs(
                                 pConfigs->main_npc[i],
                                 autoConf, capturedS);
                 }

             }

             //Apply only if custom config or image was found
             if((uNPC.withImg)||(uNPC.withTxt))
             {
                 uNPCs.push_back(uNPC);
                 //Apply index;
                 if(uNPC.id < (unsigned int)index_npc.size())
                 {
                     index_npc[uNPC.id].type = 1;
                     index_npc[uNPC.id].i = (uNPCs.size()-1);
                 }
             }


             AdvNpcAnimator * aniNPC = new AdvNpcAnimator(
                         ((uNPC.withImg)?
                              ((uNPCs.last().image.isNull())?
                                 uNpcImg:
                                     uNPCs.last().image)
                                  :
                              ((pConfigs->main_npc[i].image.isNull())?
                                 uNpcImg:
                                    pConfigs->main_npc[i].image)
                              ),
                                 ((uNPC.withTxt)? uNPCs.last().merged : pConfigs->main_npc[i])
                                   );
             animates_NPC.push_back( aniNPC );
             index_npc[pConfigs->main_npc[i].id].ai = animates_NPC.size()-1;

         qApp->processEvents(QEventLoop::ExcludeUserInputEvents);
         if(progress.wasCanceled())
             /*progress.setValue(progress.value()+1);
         else*/ return;
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
    bool noimage=true, found=false;
    bool isUser=false, isUserTxt=false;
    int j, q;
    QPixmap tempI;
    int gfxH = 0;
    obj_npc merged;

    //Check Index exists
    if(npcID < (unsigned int)index_npc.size())
    {
        j = index_npc[npcID].gi;
        if(j<pConfigs->main_npc.size())
        {
            if(pConfigs->main_npc[j].id == npcID)
                found=true;
        }
    }

    //if Index found
    if(found)
    {   //get neccesary element directly
        if(index_npc[npcID].type==1)
        {
            isUser=true;
            if(uNPCs[index_npc[npcID].i].withImg)
            {
                noimage=false;
                tempI = uNPCs[index_npc[npcID].i].image;
            }
            if(uNPCs[index_npc[npcID].i].withTxt)
            {
                gfxH = uNPCs[index_npc[npcID].i].merged.gfx_h;
                merged = uNPCs[index_npc[npcID].i].merged;
            }
            else
            {
                gfxH = pConfigs->main_npc[index_npc[npcID].gi].height;
                merged = pConfigs->main_npc[index_npc[npcID].gi];
            }
        }

        if(!noimage)
        {
            tempI = pConfigs->main_npc[(isUser) ? index_npc[npcID].gi : index_npc[npcID].i].image;
            noimage=false;
        }

    }
    else
    {
        //found neccesary element in arrays and select
        for(q=0;q<uNPCs.size();q++)
        {
            if(uNPCs[q].id == npcID)
            {
                if(uNPCs[q].withImg)
                {
                    isUser=true;
                    noimage=false;
                    tempI = uNPCs[q].image;
                }
                if(uNPCs[q].withTxt)
                {
                    isUserTxt = true;
                    gfxH = uNPCs[q].merged.gfx_h;
                    merged = uNPCs[q].merged;
                }
                break;
            }
        }

        for(j=0;j<pConfigs->main_npc.size();j++)
        {
            if(pConfigs->main_npc[j].id==npcID)
            {
                noimage=false;
                if(!isUser)
                    tempI = pConfigs->main_npc[j].image;
                if(!isUserTxt)
                {
                    gfxH =  pConfigs->main_npc[j].gfx_h;
                    merged = pConfigs->main_npc[j];
                }
                break;
            }
        }
    }

    if((noimage)||(tempI.isNull()))
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
        return tempI.copy(0,frame*gfxH, tempI.width(), gfxH );
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
        return tempI.copy(0,frame*gfxH, tempI.width(), gfxH );
    }

}
