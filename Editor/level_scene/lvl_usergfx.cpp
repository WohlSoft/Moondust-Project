/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2014 Vitaly Novichkov <admin@wohlnet.ru>
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

#include "lvlscene.h"
#include "../edit_level/leveledit.h"
#include "../file_formats/file_formats.h"

#include "../common_features/graphics_funcs.h"

//Search and load custom User's files

void LvlScene::buildAnimators()
{
    int i;
    for(i=0; i<pConfigs->main_bgo.size(); i++) //Add user images
    {
        SimpleAnimator * aniBGO = new SimpleAnimator(
                         ((pConfigs->main_bgo[i].image.isNull())?
                                uBgoImg:
                               pConfigs->main_bgo[i].image),
                              pConfigs->main_bgo[i].animated,
                              pConfigs->main_bgo[i].frames,
                              pConfigs->main_bgo[i].framespeed
                              );

        animates_BGO.push_back( aniBGO );
        if(pConfigs->main_bgo[i].id < (unsigned int)index_blocks.size())
        {
            index_bgo[pConfigs->main_bgo[i].id].ai = animates_BGO.size()-1;
        }
    }

    for(i=0; i<pConfigs->main_block.size(); i++) //Add user images
    {
        //WriteToLog(QtDebugMsg, QString("Block Animator ID: %1").arg(i));

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
                         ((pConfigs->main_block[i].image.isNull())?
                                uBgoImg:
                                pConfigs->main_block[i].image),
                                pConfigs->main_block[i].animated,
                                pConfigs->main_block[i].frames,
                                pConfigs->main_block[i].framespeed, frameFirst, frameLast,
                                pConfigs->main_block[i].animation_rev,
                                pConfigs->main_block[i].animation_bid
                              );

        animates_Blocks.push_back( aniBlock );
        if(pConfigs->main_block[i].id < (unsigned int)index_blocks.size())
        {
            index_blocks[pConfigs->main_block[i].id].ai = animates_Blocks.size()-1;
        }
    }

}

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
    QString uLVLDs = LvlData->path + "/" + LvlData->filename + "/";
    QString uLVLD = LvlData->path + "/" + LvlData->filename;
    QString uLVLs = LvlData->path + "/";


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
            if((QFile::exists(uLVLD) ) &&
                  (QFile::exists(uLVLDs + pConfigs->main_bg[i].image_n)) )
            {
                uBG.image = GraphicsHelps::loadPixmap( uLVLDs + pConfigs->main_bg[i].image_n );
                uBG.id = pConfigs->main_bg[i].id;
                if(uBG.image.isNull()) WrongImagesDetected=true;
                loaded1 = true;
            }
            else
            if(QFile::exists(uLVLs + pConfigs->main_bg[i].image_n) )
            {
                uBG.image = GraphicsHelps::loadPixmap( uLVLs + pConfigs->main_bg[i].image_n );
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
                if((QFile::exists(uLVLD) ) &&
                      (QFile::exists(uLVLDs + pConfigs->main_bg[i].second_image_n )) )
                {
                    uBG.second_image = GraphicsHelps::loadPixmap( uLVLDs + pConfigs->main_bg[i].second_image_n );
                    uBG.id = pConfigs->main_bg[i].id;
                    loaded2 = true;
                    if(uBG.second_image.isNull()) WrongImagesDetected=true;
                }
                else
                if(QFile::exists(uLVLs + pConfigs->main_bg[i].second_image_n) )
                {
                    uBG.second_image = GraphicsHelps::loadPixmap( uLVLs + pConfigs->main_bg[i].second_image_n );
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

            if((QFile::exists(uLVLD) ) &&
                  (QFile::exists(uLVLDs + pConfigs->main_block[i].image_n)) )
            {
                if(QFile::exists(uLVLDs + pConfigs->main_block[i].mask_n))
                    uBlock.mask = GraphicsHelps::loadPixmap( uLVLDs + pConfigs->main_block[i].mask_n );
                else
                    uBlock.mask = pConfigs->main_block[i].mask;

                uBlock.image = GraphicsHelps::setAlphaMask(GraphicsHelps::loadPixmap(uLVLDs + pConfigs->main_block[i].image_n ), uBlock.mask);
                if(uBlock.image.isNull()) WrongImagesDetected=true;

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
            else
            if(QFile::exists(uLVLs + pConfigs->main_block[i].image_n) )
            {
                if(QFile::exists(uLVLs + pConfigs->main_block[i].mask_n))
                    uBlock.mask = GraphicsHelps::loadPixmap(uLVLs + pConfigs->main_block[i].mask_n );
                else
                    uBlock.mask = pConfigs->main_block[i].mask;

                uBlock.image = GraphicsHelps::setAlphaMask(GraphicsHelps::loadPixmap(uLVLs + pConfigs->main_block[i].image_n ), uBlock.mask);
                if(uBlock.image.isNull()) WrongImagesDetected=true;

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
            if((QFile::exists(uLVLD) ) &&
                  (QFile::exists(uLVLDs + pConfigs->main_bgo[i].image_n)) )
            {
                if(QFile::exists(uLVLDs + pConfigs->main_bgo[i].mask_n))
                    uBGO.mask = GraphicsHelps::loadPixmap(uLVLDs + pConfigs->main_bgo[i].mask_n );
                else
                    uBGO.mask = pConfigs->main_bgo[i].mask;

                uBGO.image = GraphicsHelps::setAlphaMask(GraphicsHelps::loadPixmap(uLVLDs + pConfigs->main_bgo[i].image_n ), uBGO.mask);
                if(uBGO.image.isNull()) WrongImagesDetected=true;

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
            else
            if(QFile::exists(uLVLs + pConfigs->main_bgo[i].image_n) )
            {
                if(QFile::exists(uLVLs + pConfigs->main_bgo[i].mask_n))
                    uBGO.mask = GraphicsHelps::loadPixmap(uLVLs + pConfigs->main_bgo[i].mask_n );
                else
                    uBGO.mask = pConfigs->main_bgo[i].mask;

                uBGO.image = GraphicsHelps::setAlphaMask(GraphicsHelps::loadPixmap(uLVLs + pConfigs->main_bgo[i].image_n ), uBGO.mask);
                if(uBGO.image.isNull()) WrongImagesDetected=true;

                uBGO.id = pConfigs->main_bgo[i].id;
                uBGOs.push_back(uBGO);
                custom=true;

                //Apply index;
                if(uBGO.id < (unsigned int)index_bgo.size())
                {
                    index_bgo[uBGO.id].type = 1;
                    //index_bgo[uBGO.id].i = (uBGOs.size()-1);
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

             // /////////////////////// Looking for user's NPC.txt ////////////////////////////
             // //(for use custom image filename, need to parse NPC.txt before iamges)/////////
             if((QFile::exists(uLVLD) ) &&
                   (QFile::exists(uLVLDs +
                      "npc-" + QString::number(pConfigs->main_npc[i].id)+".txt") ) )
             {
                QFile file(uLVLDs +
                           "npc-" + QString::number(pConfigs->main_npc[i].id)+".txt");
                if(file.open(QIODevice::ReadOnly))
                {
                    uNPC.sets = FileFormats::ReadNpcTXTFile(file, true);
                    uNPC.id = pConfigs->main_npc[i].id;
                    uNPC.withTxt = true;
                }
             }
             else
             if(QFile::exists(uLVLs +
                              "npc-" + QString::number(pConfigs->main_npc[i].id)+".txt"
                              ) )
             {
                 QFile file(uLVLs +
                            "npc-" + QString::number(pConfigs->main_npc[i].id)+".txt");
                 if(file.open(QIODevice::ReadOnly))
                 {
                     uNPC.sets = FileFormats::ReadNpcTXTFile(file, true);
                     uNPC.id = pConfigs->main_npc[i].id;
                     uNPC.withTxt = true;
                 }
             }

             // ///////////////////////Looking for user's GFX
             if((QFile::exists(uLVLD) ) &&
                   (QFile::exists(uLVLDs + pConfigs->main_npc[i].image_n)) )
             {
                 if(QFile::exists(uLVLDs + pConfigs->main_npc[i].mask_n))
                     uNPC.mask = GraphicsHelps::loadPixmap(uLVLDs + pConfigs->main_npc[i].mask_n );
                 else
                     uNPC.mask = pConfigs->main_npc[i].mask;

                 uNPC.image = GraphicsHelps::setAlphaMask(GraphicsHelps::loadPixmap(uLVLDs + pConfigs->main_npc[i].image_n ), uNPC.mask);
                 if(uNPC.image.isNull()) WrongImagesDetected=true;

                 uNPC.id = pConfigs->main_npc[i].id;
                 uNPC.withImg = true;
             }
             else
             if(QFile::exists(uLVLs + pConfigs->main_npc[i].image_n) )
             {
                 if(QFile::exists(uLVLs + pConfigs->main_npc[i].mask_n))
                     uNPC.mask = GraphicsHelps::loadPixmap(uLVLs + pConfigs->main_npc[i].mask_n );
                 else
                     uNPC.mask = pConfigs->main_npc[i].mask;

                 uNPC.image = GraphicsHelps::setAlphaMask(GraphicsHelps::loadPixmap(uLVLs + pConfigs->main_npc[i].image_n ), uNPC.mask);
                 if(uNPC.image.isNull()) WrongImagesDetected=true;

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
                     //autoConf.en_gfxwidth = true;
                     unsigned int defGFX_h;
                     switch(pConfigs->main_npc[i].framestyle)
                     {
                     case 0:
                         defGFX_h = (int)round(capturedS.height() / pConfigs->main_npc[i].frames);
                         break;
                     case 1:
                         defGFX_h = (int)round((capturedS.height() / pConfigs->main_npc[i].frames)/2 );
                         break;
                     case 2:
                         defGFX_h = (int)round((capturedS.height()/pConfigs->main_npc[i].frames)/4);
                         break;
                     case 3:
                         defGFX_h = (int)round((capturedS.height()/pConfigs->main_npc[i].frames)/4);
                         break;
                     case 4:
                         defGFX_h = (int)round((capturedS.height()/pConfigs->main_npc[i].frames)/8);
                         break;
                     default:
                         defGFX_h=0;
                         break;
                     }

                     capturedS.setHeight(defGFX_h);

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
