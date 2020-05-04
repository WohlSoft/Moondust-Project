/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2014-2020 Vitaly Novichkov <admin@wohlnet.ru>
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
#include <Utils/maths.h>

#include "../../../../defines.h"

#include "../lvl_scene.h"


//Search and load custom User's files
void LvlScene::loadUserData(QProgressDialog &progress)
{
    int i, total = 0;

    //UserNPCs uNPC;
    QImage tempImg;

    bool WrongImagesDetected = false;

    m_localConfigBlocks.clear();
    m_localConfigBGOs.clear();
    m_localConfigNPCs.clear();

    m_localConfigBackgrounds.clear();

    bool loaded1, loaded2;

    CustomDirManager uLVL(m_data->meta.path, m_data->meta.filename);

    //Load custom rotation rules
    QString rTableFile = uLVL.getCustomFile("rotation_table.ini");
    if(!rTableFile.isEmpty())
    {
        QSettings rTableINI(rTableFile, QSettings::IniFormat);
        rTableINI.setIniCodec("UTF-8");

        QStringList rules = rTableINI.childGroups();

        int count = 0;
        for(const QString &x : rules)
        {
            rTableINI.beginGroup(x);
            obj_rotation_table t;
            t.id = rTableINI.value("id", 0).toInt();
            t.type = Items::getItemType(rTableINI.value("type", "-1").toString());
            t.rotate_left = rTableINI.value("rotate-left", 0).toInt();
            t.rotate_right = rTableINI.value("rotate-right", 0).toInt();
            t.flip_h = rTableINI.value("flip-h", 0).toInt();
            t.flip_v = rTableINI.value("flip-v", 0).toInt();
            rTableINI.endGroup();
            if(t.id <= 0) continue;

            if(t.type == ItemTypes::LVL_Block)
            {
                local_rotation_table_blocks[t.id] = t;
                count++;
            }
            else if(t.type == ItemTypes::LVL_BGO)
            {
                local_rotation_table_bgo[t.id] = t;
                count++;
            }
            else if(t.type == ItemTypes::LVL_NPC)
            {
                local_rotation_table_npc[t.id] = t;
                count++;
            }
        }
        qDebug() << "Loaded custom rotation rules: " << count;
    }

    //Get extra folders for search
    QString sFoldersFile = uLVL.getCustomFile("folders.ini");
    if(!sFoldersFile.isEmpty())
    {
        LogDebug(QString("Found folders.ini: %1").arg(sFoldersFile));
        QSettings rTableINI(sFoldersFile, QSettings::IniFormat);
        rTableINI.setIniCodec("UTF-8");

        rTableINI.beginGroup("folders");
        auto keys = rTableINI.allKeys();
        for(const QString &k : keys)
        {
            QString val = rTableINI.value(k, "").toString();
            if(!val.isEmpty())
            {
                uLVL.addExtraDir(m_data->meta.path + "/" + val);
                LogDebug(QString("Adding extra folder: %1").arg(m_data->meta.path + "/" + val));
            }
        }
        rTableINI.endGroup();
    }

    if(!progress.wasCanceled())
        progress.setLabelText(
            tr("Search User Backgrounds %1")
            .arg(QString::number(m_configs->main_bg.stored())));

    qApp->processEvents();
    uLVL.setDefaultDir(m_configs->getBGPath());
    //Load Backgrounds
    for(int i = 1; i < m_configs->main_bg.size(); i++)
    {
        loaded1 = false;
        loaded2 = false;
        obj_BG *bgD = &m_configs->main_bg[i];
        UserBGs uBG;

        QString customINI = uLVL.getCustomFile("background2-" + QString::number(bgD->setup.id) + ".ini", true);
        if(!customINI.isEmpty())
        {
            m_localConfigBackgrounds[bgD->setup.id] = *bgD;
            obj_BG &bgN = m_localConfigBackgrounds[bgD->setup.id];
            m_configs->loadLevelBackground(bgN, "background2", bgD, customINI);
            bgD = &bgN;
        }

        //check for first image
        QString CustomFile = uLVL.getCustomFile(bgD->setup.image_n, true);
        if(!CustomFile.isEmpty())
        {
            GraphicsHelps::loadQImage(tempImg, CustomFile);
            if(tempImg.isNull())
                WrongImagesDetected = true;
            else
                uBG.image = QPixmap::fromImage(tempImg);
            uBG.id = bgD->setup.id;
            loaded1 = true;
        }

        if((loaded1) && (bgD->setup.animated))
        {
            double h      = static_cast<double>(uBG.image.height());
            double frames = static_cast<double>(bgD->setup.frames);
            uBG.image = uBG.image.copy(0, 0,
                                       uBG.image.width(),
                                       Maths::iRound(h / frames));
        }

        //check for second image
        if(bgD->setup.type == 1)
        {
            QString CustomFile = uLVL.getCustomFile(bgD->setup.second_image_n, true);
            if(!CustomFile.isEmpty())
            {
                GraphicsHelps::loadQImage(tempImg, CustomFile);
                if(tempImg.isNull())
                    WrongImagesDetected = true;
                else
                    uBG.second_image = QPixmap::fromImage(tempImg);
                uBG.id = bgD->setup.id;
                loaded2 = true;
            }
        }

        if((loaded1) && (!loaded2)) uBG.q = 0;
        if((!loaded1) && (loaded2)) uBG.q = 1;
        if((loaded1) && (loaded2)) uBG.q = 2;

        //If user images found and loaded
        if((loaded1) || (loaded2))
        {
            if(!m_localConfigBackgrounds.contains(uBG.id)) m_localConfigBackgrounds[uBG.id] = *bgD;
            obj_BG &bgU = m_localConfigBackgrounds[uBG.id];
            if(loaded1) bgU.image = uBG.image;
            if(loaded2) bgU.second_image = uBG.second_image;
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
            .arg(QString::number(m_configs->main_block.stored())));
        progress.setValue(progress.value() + 1);
    }
    m_localConfigBlocks.allocateSlots(m_configs->main_block.total());
    qApp->processEvents();
    uLVL.setDefaultDir(m_configs->getBlockPath());
    //Load Blocks
    for(i = 1; i < m_configs->main_block.size(); i++) //Add user images
    {
        obj_block *blockD = &m_configs->main_block[i];
        obj_block t_block;
        blockD->copyTo(t_block);

        bool custom = false;

        QStringList customINIs;
        customINIs = uLVL.getCustomFiles("block-" + QString::number(blockD->setup.id), {".ini", ".txt"}, true);
        for(QString &iniFile : customINIs)
        {
            m_configs->loadLevelBlock(t_block, "block", &t_block, iniFile);
            custom = true;
        }

        QString customImgFile = uLVL.getCustomFile(t_block.setup.image_n, true);
        if(!customImgFile.isEmpty())
        {
            if(!customImgFile.endsWith(".png", Qt::CaseInsensitive))
            {
                QString CustomMask = uLVL.getCustomFile(t_block.setup.mask_n, false);
                GraphicsHelps::loadQImage(tempImg, customImgFile, CustomMask, &blockD->image);
            }
            else
                GraphicsHelps::loadQImage(tempImg, customImgFile);
            if(tempImg.isNull())
                WrongImagesDetected = true;
            else
            {
                m_localImages.push_back(QPixmap::fromImage(tempImg));
                t_block.cur_image = &m_localImages.last();
            }
            custom = true;
        }

        SimpleAnimator *aniBlock = new SimpleAnimator(
            ((t_block.cur_image->isNull()) ?
             m_dummyBlockImg : *t_block.cur_image),
            t_block.setup.animated,
            t_block.setup.frames,
            t_block.setup.framespeed, 0, -1,
            t_block.setup.animation_rev,
            t_block.setup.animation_bid
        );

        if(!t_block.setup.frame_sequence.isEmpty())
            aniBlock->setFrameSequance(t_block.setup.frame_sequence);

        t_block.animator_id = m_animatorsBlocks.size();
        m_animatorsBlocks.push_back(aniBlock);
        m_animationTimer.registerAnimation(aniBlock);

        m_localConfigBlocks.storeElement(i, t_block);
        if(custom)
        {
            m_customBlocks.push_back(&m_localConfigBlocks[i]);//Register BGO as customized
        }

        //qApp->processEvents(QEventLoop::ExcludeUserInputEvents);
        if(progress.wasCanceled())
            return;
    }

    ///////////////////////////////////////////////////////////////////////////

    if(!progress.wasCanceled())
    {
        progress.setLabelText(
            tr("Search User BGOs %1")
            .arg(QString::number(m_configs->main_bgo.stored())));

        progress.setValue(progress.value() + 1);
    }

    qApp->processEvents();
    uLVL.setDefaultDir(m_configs->getBgoPath());
    //Load BGO
    m_localConfigBGOs.allocateSlots(m_configs->main_bgo.total());
    for(int i = 1; i < m_configs->main_bgo.size(); i++)
    {
        obj_bgo *bgoD = &m_configs->main_bgo[i];
        obj_bgo t_bgo; //Allocate new BGO Config entry
        bool custom = false;

        bgoD->copyTo(t_bgo);//init configs

        QStringList customINIs;
        customINIs = uLVL.getCustomFiles("background-" + QString::number(bgoD->setup.id), {".ini", ".txt"}, true);
        for(QString &iniFile : customINIs)
        {
            m_configs->loadLevelBGO(t_bgo, "background", &t_bgo, iniFile);
            custom = true;
        }

        QString customImgFile = uLVL.getCustomFile(t_bgo.setup.image_n, true);
        if(!customImgFile.isEmpty())
        {
            if(!customImgFile.endsWith(".png", Qt::CaseInsensitive))
            {
                QString CustomMask = uLVL.getCustomFile(t_bgo.setup.mask_n, false);
                GraphicsHelps::loadQImage(tempImg, customImgFile, CustomMask, &bgoD->image);
            }
            else
                GraphicsHelps::loadQImage(tempImg, customImgFile);
            if(tempImg.isNull())
                WrongImagesDetected = true;
            else
            {
                m_localImages.push_back(QPixmap::fromImage(tempImg));
                t_bgo.cur_image = &m_localImages.last();
            }
            custom = true;
        }

        SimpleAnimator *aniBGO = new SimpleAnimator(
            ((t_bgo.cur_image->isNull()) ?
             m_dummyBgoImg : *t_bgo.cur_image
            ),
            t_bgo.setup.animated,
            t_bgo.setup.frames,
            t_bgo.setup.framespeed
        );

        if(!t_bgo.setup.frame_sequence.isEmpty())
            aniBGO->setFrameSequance(t_bgo.setup.frame_sequence);

        t_bgo.animator_id = m_animatorsBGO.size();
        m_animatorsBGO.push_back(aniBGO);
        m_animationTimer.registerAnimation(aniBGO);

        m_localConfigBGOs.storeElement(i, t_bgo);
        if(custom)
        {
            m_customBGOs.push_back(&m_localConfigBGOs[i]);//Register BGO as customized
        }

        //qApp->processEvents(QEventLoop::ExcludeUserInputEvents);
        if(progress.wasCanceled())
            return;
    }

    ///////////////////////////////////////////////////////////////////////////


    if(!progress.wasCanceled())
    {
        progress.setLabelText(
            tr("Search User NPCs %1")
            .arg(QString::number(m_configs->main_npc.stored())));

        progress.setValue(progress.value() + 1);
    }
    qApp->processEvents();
    uLVL.setDefaultDir(m_configs->getNpcPath());
    //Load NPC
    m_localConfigNPCs.allocateSlots(m_configs->main_npc.total());
    NPCConfigFile sets;

    for(i = 1; i < m_configs->main_npc.size(); i++) //Add user images
    {
        obj_npc *npcD = &m_configs->main_npc[i];
        obj_npc t_npc; //Allocate new NPC Config entry
        bool custom = false;
        bool cimage = false;
        bool npctxt = false;

        npcD->copyTo(t_npc);//init configs

        QSize capturedS = QSize(0, 0);

        if(!t_npc.cur_image->isNull())
            capturedS = t_npc.cur_image->size();

        // NPC.INI and NPC.TXT are has different format. Therefore parse both of them
        QString customINI = uLVL.getCustomFile("npc-" + QString::number(t_npc.setup.id) + ".ini", true);
        if(!customINI.isEmpty())
        {
            m_configs->loadLevelNPC(t_npc, "npc", npcD, customINI);
            custom = true;
        }

        // /////////////////////// Looking for user's NPC.txt ////////////////////////////
        // //(for use custom image filename, need to parse NPC.txt before iamges)/////////
        customINI = uLVL.getCustomFile("npc-" + QString::number(t_npc.setup.id) + ".txt", true);
        if(!customINI.isEmpty())
        {
            QFile file(customINI);
            if(file.open(QIODevice::ReadOnly))
            {
                file.close();
                FileFormats::ReadNpcTXTFileF(customINI, sets, true);
                npctxt = true;
                custom = true;
            }
        }
        QString imgFileName = (npctxt && sets.en_image) ? sets.image : t_npc.setup.image_n;

        // ///////////////////////Looking for user's GFX
        QString customImgFile = uLVL.getCustomFile(imgFileName, true);
        if(customImgFile.isEmpty())
            customImgFile = uLVL.getCustomFile(t_npc.setup.image_n, true);

        if(!customImgFile.isEmpty())
        {
            if(!customImgFile.endsWith(".png", Qt::CaseInsensitive))
            {
                QString CustomMask = uLVL.getCustomFile(t_npc.setup.mask_n, false);
                GraphicsHelps::loadQImage(tempImg, customImgFile, CustomMask, &npcD->image);
            }
            else
                GraphicsHelps::loadQImage(tempImg, customImgFile);
            if(tempImg.isNull())
                WrongImagesDetected = true;
            else
            {
                m_localImages.push_back(QPixmap::fromImage(tempImg));
                t_npc.cur_image = &m_localImages.last();
                capturedS = QSize(t_npc.cur_image->width(), t_npc.cur_image->height());
            }
            cimage = true;
            custom = true;
        }

        if(npctxt)
        {
            //Merge global and user's settings from NPC.txt file
            uint32_t npcImgW = static_cast<uint32_t>(capturedS.width());
            uint32_t npcImgH = static_cast<uint32_t>(capturedS.height());
            t_npc.setup.applyNPCtxt(&sets, t_npc.setup, npcImgW, npcImgH);
        }
        else
        {
            if(cimage)
            {
                NPCConfigFile autoConf = FileFormats::CreateEmpytNpcTXT();
                uint32_t npcImgW = static_cast<uint32_t>(capturedS.width());
                uint32_t npcImgH = static_cast<uint32_t>(capturedS.height());
                autoConf.gfxwidth = npcImgW;
                autoConf.width = t_npc.setup.width;
                autoConf.en_width = true;
                t_npc.setup.applyNPCtxt(&autoConf, t_npc.setup, npcImgW, npcImgH);
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
        AdvNpcAnimator *aniNPC = new AdvNpcAnimator(
            ((t_npc.cur_image->isNull()) ?
             m_dummyNpcImg :
             *t_npc.cur_image),
            t_npc);

        t_npc.animator_id = uint64_t(m_animatorsNPC.size());
        m_animatorsNPC.push_back(aniNPC);

        m_localConfigNPCs.storeElement(i, t_npc);
        if(custom)
            m_customNPCs.push_back(&m_localConfigNPCs[i]);

        //qApp->processEvents(QEventLoop::ExcludeUserInputEvents);
        if(progress.wasCanceled())
            return;
    }

    progress.setValue(progress.value() + 1);
    qApp->processEvents();

    //Notification about wrong custom image sprites
    if(WrongImagesDetected)
    {
        QMessageBox *msg = new QMessageBox();
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
    bool found = false;
    found = (npcID > 0) && m_localConfigNPCs.contains(npcID);

    if(!found)
        return m_dummyNpcImg;

    int gfxH = 0;
    obj_npc &merged = m_localConfigNPCs[npcID];
    found = merged.isValid;
    gfxH  = merged.setup.gfx_h;

    if(merged.cur_image->isNull())
        return m_dummyNpcImg;

    if(Direction <= 0)
    {
        int frame = 0;
        if(merged.setup.custom_animate)
            frame = merged.setup.custom_ani_fl;
        return merged.cur_image->copy(0, frame * gfxH, merged.cur_image->width(), gfxH);
    }
    else
    {
        int frame = 0;
        int framesQ;
        if(merged.setup.custom_animate)
            frame = merged.setup.custom_ani_fr;
        else
        {
            switch(merged.setup.framestyle)
            {
            case 2:
                framesQ = merged.setup.frames * 4;
                frame = (int)(framesQ - (framesQ / 4) * 3);
                break;
            case 1:
                framesQ = merged.setup.frames * 2;
                frame = (int)(framesQ / 2);
                break;
            case 0:
            default:
                break;
            }
        }
        return merged.cur_image->copy(0, frame * gfxH, merged.cur_image->width(), gfxH);
    }
}
