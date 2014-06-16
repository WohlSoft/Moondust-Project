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

#include "../../ui_mainwindow.h"
#include "../../mainwindow.h"
#include "../../level_scene/lvl_item_placing.h"
#include "../../npc_dialog/npcdialog.h"

#include "../../level_scene/item_block.h"
#include "../../level_scene/item_bgo.h"
#include "../../level_scene/item_npc.h"
#include "../../level_scene/itemmsgbox.h"


static int npcSpecSpinOffset=0;

void MainWindow::LvlItemProps(int Type, LevelBlock block, LevelBGO bgo, LevelNPC npc, bool newItem)
{
    ui->blockProp->setVisible(false);
    ui->bgoProps->setVisible(false);
    ui->npcProps->setVisible(false);

    LvlItemPropsLock=true;

    setLayerLists();

    /*
    long blockPtr; //ArrayID of editing item (-1 - use system)
    long bgoPtr; //ArrayID of editing item
    long npcPtr; //ArrayID of editing item
    */

    switch(Type)
    {
    case 0:
    {
        ui->ItemProperties->setVisible(true);
        ui->ItemProperties->show();
        ui->ItemProperties->raise();
        ui->blockProp->show();
        ui->blockProp->raise();
        ui->ItemProps->setCurrentIndex(0);

        if(newItem)
            blockPtr = -1;
        else
            blockPtr = block.array_id;

        bool found=false;
        int j;

        //Check Index exists
        if(block.id < (unsigned int)configs.index_blocks.size())
        {
            j = configs.index_blocks[block.id].i;

            if(j<configs.main_block.size())
            {
            if(configs.main_block[j].id == block.id)
                found=true;
            }
        }
        //if Index found
        if(!found)
        {
            for(j=0;j<configs.main_block.size();j++)
            {
                if(configs.main_block[j].id==block.id)
                    break;
            }
        }
        if(j >= configs.main_block.size())
        {
            j=0;
        }

        ui->PROPS_blockPos->setText( tr("Position: [%1, %2]").arg(block.x).arg(block.y) );
        ui->PROPS_BlockResize->setVisible( configs.main_block[j].sizable );
        ui->PROPS_BlockInvis->setChecked( block.invisible );
        ui->PROPS_BlkSlippery->setChecked( block.slippery );

        ui->PROPS_BlockSquareFill->setVisible( newItem );
        ui->PROPS_BlockSquareFill->setChecked(LvlPlacingItems::fillingMode);

        ui->PROPS_BlockIncludes->setText(
                    ((block.npc_id>0)?
                         ((block.npc_id>1000)?QString("NPC-%1").arg(block.npc_id-1000):tr("%1 coins").arg(block.npc_id))
                       :tr("[empty]")
                         ) );

        ui->PROPS_BlockLayer->setCurrentIndex(0);
        for(int i=0; i<ui->PROPS_BlockLayer->count();i++)
        {
            if(ui->PROPS_BlockLayer->itemText(i)==block.layer)
            {ui->PROPS_BlockLayer->setCurrentIndex(i); break;}
        }

        ui->PROPS_BlkEventDestroy->setCurrentIndex(0);
        for(int i=0; i<ui->PROPS_BlkEventDestroy->count();i++)
        {
            if(ui->PROPS_BlkEventDestroy->itemText(i)==block.event_destroy)
            {ui->PROPS_BlkEventDestroy->setCurrentIndex(i); break;}
        }

        ui->PROPS_BlkEventHited->setCurrentIndex(0);
        for(int i=0; i<ui->PROPS_BlkEventHited->count();i++)
        {
            if(ui->PROPS_BlkEventHited->itemText(i)==block.event_hit)
            {ui->PROPS_BlkEventHited->setCurrentIndex(i); break;}
        }

        ui->PROPS_BlkEventLayerEmpty->setCurrentIndex(0);
        for(int i=0; i<ui->PROPS_BlkEventLayerEmpty->count();i++)
        {
            if(ui->PROPS_BlkEventLayerEmpty->itemText(i)==block.event_no_more)
            {ui->PROPS_BlkEventLayerEmpty->setCurrentIndex(i); break;}
        }

        break;
    }
    case 1:
    {
        ui->ItemProperties->show();
        ui->ItemProperties->raise();
        ui->bgoProps->show();
        ui->bgoProps->raise();
        if(newItem)
            bgoPtr = -1;
        else
            bgoPtr = bgo.array_id;
        ui->ItemProps->setCurrentIndex(1);

        ui->PROPS_BGOSquareFill->setVisible( newItem );
        ui->PROPS_BGOSquareFill->setChecked(LvlPlacingItems::fillingMode);

        ui->PROPS_bgoPos->setText( tr("Position: [%1, %2]").arg(bgo.x).arg(bgo.y) );

        ui->PROPS_BGOLayer->setCurrentIndex(0);
        for(int i=0; i<ui->PROPS_BGOLayer->count();i++)
        {
            if(ui->PROPS_BGOLayer->itemText(i)==bgo.layer)
            {ui->PROPS_BGOLayer->setCurrentIndex(i); break;}
        }

        ui->PROPS_BGO_smbx64_sp->setValue( bgo.smbx64_sp );

        break;
    }
    case 2:
    {
        ui->ItemProperties->show();
        ui->ItemProperties->raise();
        ui->npcProps->show();
        ui->npcProps->raise();

        if(newItem)
            npcPtr = -1;
        else
            npcPtr = npc.array_id;

        ui->ItemProps->setCurrentIndex(2);

        bool found=false;
        int j;

        //Check Index exists
        if(npc.id < (unsigned int)configs.index_npc.size())
        {
            j = configs.index_npc[npc.id].i;

            if(j<configs.main_npc.size())
            {
            if(configs.main_npc[j].id == npc.id)
                found=true;
            }
        }
        //if Index found
        if(!found)
        {
            for(j=0;j<configs.main_npc.size();j++)
            {
                if(configs.main_npc[j].id==npc.id)
                    break;
            }
        }

        if(j >= configs.main_npc.size())
        {
            j=0;
        }

        ui->PROPS_NPCContaiter->hide();
        ui->PROPS_NpcContainsLabel->hide();

        ui->PROPS_NPCBoxLabel->hide();
        ui->PROPS_NPCSpecialBox->hide();

        ui->PROPS_NpcSpinLabel->hide();
        ui->PROPS_NPCSpecialSpin->hide();
        ui->line_6->hide();

        ui->PROPS_NpcPos->setText( tr("Position: [%1, %2]").arg(npc.x).arg(npc.y) );

        if(configs.main_npc[j].direct_alt_title!="")
            ui->PROPS_NpcDir->setTitle(configs.main_npc[j].direct_alt_title);
        else
            ui->PROPS_NpcDir->setTitle( tr("Direction") );

        if(configs.main_npc[j].direct_alt_left!="")
            ui->PROPS_NPCDirLeft->setText( configs.main_npc[j].direct_alt_left );
        else
            ui->PROPS_NPCDirLeft->setText( tr("Left") );

        ui->PROPS_NPCDirRand->setEnabled( !configs.main_npc[j].direct_disable_random );

        if(configs.main_npc[j].direct_alt_right!="")
            ui->PROPS_NPCDirRight->setText( configs.main_npc[j].direct_alt_right );
        else
            ui->PROPS_NPCDirRight->setText( tr("Right") );

        switch(npc.direct)
        {
        case -1:
            ui->PROPS_NPCDirLeft->setChecked(true);
            break;
        case 0:
            ui->PROPS_NPCDirRand->setChecked(true);
            break;
        case 1:
            ui->PROPS_NPCDirRight->setChecked(true);
            break;
        }

        //; 0 combobox, 1 - spin, 2 - npc-id
        if( configs.main_npc[j].special_option )
        {
            ui->line_6->show();
            switch(configs.main_npc[j].special_type)
            {
            case 0:
                ui->PROPS_NPCBoxLabel->show();
                ui->PROPS_NPCBoxLabel->setText(configs.main_npc[j].special_name);
                ui->PROPS_NPCSpecialBox->show();

                ui->PROPS_NPCSpecialBox->clear();
                for(int i=0; i < configs.main_npc[j].special_combobox_opts.size(); i++)
                {
                    ui->PROPS_NPCSpecialBox->addItem( configs.main_npc[j].special_combobox_opts[i] );
                    if(i==npc.special_data) ui->PROPS_NPCSpecialBox->setCurrentIndex(i);
                }

                break;
            case 1:
                ui->PROPS_NpcSpinLabel->show();
                ui->PROPS_NpcSpinLabel->setText( configs.main_npc[j].special_name );
                ui->PROPS_NPCSpecialSpin->show();
                npcSpecSpinOffset = configs.main_npc[j].special_spin_value_offset;
                ui->PROPS_NPCSpecialSpin->setMinimum( configs.main_npc[j].special_spin_min + npcSpecSpinOffset );
                ui->PROPS_NPCSpecialSpin->setMaximum( configs.main_npc[j].special_spin_max + npcSpecSpinOffset );

                ui->PROPS_NPCSpecialSpin->setValue( npc.special_data + npcSpecSpinOffset );
                break;
            case 2:
                if(configs.main_npc[j].container)
                {
                    ui->PROPS_NpcContainsLabel->show();
                    ui->PROPS_NPCContaiter->show();
                    ui->PROPS_NPCContaiter->setText(
                                ((npc.special_data>0)?QString("NPC-%1").arg(npc.special_data)
                                   :tr("[empty]")
                                     ) );
                }
                break;
            default:
                break;
            }
        }
        LvlItemPropsLock=true;
        QString npcmsg = (npc.msg.isEmpty() ? tr("[none]") : npc.msg);
        if(npcmsg.size()>20)
        {
            npcmsg.resize(18);
            npcmsg.push_back("...");
        }
        ui->PROPS_NpcTMsg->setText( npcmsg );

        ui->PROPS_NpcFri->setChecked( npc.friendly );
        ui->PROPS_NPCNoMove->setChecked( npc.nomove );
        ui->PROPS_NpcBoss->setChecked( npc.legacyboss );

        ui->PROPS_NpcGenerator->setChecked( npc.generator );
        ui->PROPS_NPCGenBox->setVisible( npc.generator );
        if(npc.generator)
        {
            switch(npc.generator_type)
            {
            case 1:
                ui->PROPS_NPCGenType->setCurrentIndex( 0 );
                break;
            case 2:
            default:
                ui->PROPS_NPCGenType->setCurrentIndex( 1 );
                break;
            }

            ui->PROPS_NPCGenTime->setValue( (double)npc.generator_period / 10 );

            switch(npc.generator_direct)
            {
            case 2:
                ui->PROPS_NPCGenLeft->setChecked(true);
                break;
            case 3:
                ui->PROPS_NPCGenDown->setChecked(true);
                break;
            case 4:
                ui->PROPS_NPCGenRight->setChecked(true);
                break;
            case 1:
            default:
                ui->PROPS_NPCGenUp->setChecked(true);
                break;
            }

        }
        else
        {
            ui->PROPS_NPCGenUp->setChecked(true);
            ui->PROPS_NPCGenType->setCurrentIndex( 0 );
        }

        ui->PROPS_NpcLayer->setCurrentIndex(0);
        for(int i=0; i<ui->PROPS_NpcLayer->count();i++)
        {
            if(ui->PROPS_NpcLayer->itemText(i)==npc.layer)
            {ui->PROPS_NpcLayer->setCurrentIndex(i); break;}
        }
        ui->PROPS_NpcAttachLayer->setCurrentIndex(0);
        for(int i=0; i<ui->PROPS_NpcAttachLayer->count();i++)
        {
            if(ui->PROPS_NpcAttachLayer->itemText(i)==npc.attach_layer)
            {ui->PROPS_NpcAttachLayer->setCurrentIndex(i); break;}
        }

        ui->PROPS_NpcEventActivate->setCurrentIndex(0);
        for(int i=0; i<ui->PROPS_BlkEventDestroy->count();i++)
        {
            if(ui->PROPS_NpcEventActivate->itemText(i)==npc.event_activate)
            {ui->PROPS_NpcEventActivate->setCurrentIndex(i); break;}
        }

        ui->PROPS_NpcEventDeath->setCurrentIndex(0);
        for(int i=0; i<ui->PROPS_NpcEventDeath->count();i++)
        {
            if(ui->PROPS_NpcEventDeath->itemText(i)==npc.event_die)
            {ui->PROPS_NpcEventDeath->setCurrentIndex(i); break;}
        }

        ui->PROPS_NpcEventTalk->setCurrentIndex(0);
        for(int i=0; i<ui->PROPS_NpcEventTalk->count();i++)
        {
            if(ui->PROPS_NpcEventTalk->itemText(i)==npc.event_talk)
            {ui->PROPS_NpcEventTalk->setCurrentIndex(i); break;}
        }

        ui->PROPS_NpcEventEmptyLayer->setCurrentIndex(0);
        for(int i=0; i<ui->PROPS_NpcEventEmptyLayer->count();i++)
        {
            if(ui->PROPS_NpcEventEmptyLayer->itemText(i)==npc.event_nomore)
            {ui->PROPS_NpcEventEmptyLayer->setCurrentIndex(i); break;}
        }

        break;
    }
    case -1: //Nothing to edit
    default:
        ui->ItemProperties->hide();
    }
    LvlItemPropsLock=false;

}



////////////////////////////////////////////////////////////////////////////////////////////

// ///////////BLOCKS///////////////////////////

void MainWindow::on_PROPS_BlockResize_clicked()
{
    if(blockPtr<1) return;

    if (activeChildWindow()==1)
    {
        QList<QGraphicsItem *> items = activeLvlEditWin()->scene->selectedItems();
        foreach(QGraphicsItem * item, items)
        {
            if((item->data(0).toString()=="Block")&&((item->data(2).toInt()==blockPtr)))
            {
                activeLvlEditWin()->scene->setBlockResizer(item, true);
                break;
            }
        }

    }

}




void MainWindow::on_PROPS_BlockSquareFill_clicked(bool checked)
{
    resetEditmodeButtons();
   //placeBlock

    ui->PROPS_BGOSquareFill->setChecked(checked);
    if (activeChildWindow()==1)
    {
       activeLvlEditWin()->scene->clearSelection();
       activeLvlEditWin()->changeCursor(2);
       activeLvlEditWin()->scene->EditingMode = 2;
       activeLvlEditWin()->scene->disableMoveItems=false;
       activeLvlEditWin()->scene->DrawMode=true;
       activeLvlEditWin()->scene->EraserEnabled = false;
       LvlPlacingItems::fillingMode = checked;
       activeLvlEditWin()->scene->setItemPlacer(0, LvlPlacingItems::blockSet.id );
       WriteToLog(QtDebugMsg, QString("Block Square draw -> %1").arg(checked));
       activeLvlEditWin()->setFocus();
    }
}


void MainWindow::on_PROPS_BGOSquareFill_clicked(bool checked)
{
    resetEditmodeButtons();
   //placeBlock

    ui->PROPS_BlockSquareFill->setChecked(checked);
    if (activeChildWindow()==1)
    {
       activeLvlEditWin()->scene->clearSelection();
       activeLvlEditWin()->changeCursor(2);
       activeLvlEditWin()->scene->EditingMode = 2;
       activeLvlEditWin()->scene->disableMoveItems=false;
       activeLvlEditWin()->scene->DrawMode=true;
       activeLvlEditWin()->scene->EraserEnabled = false;
       LvlPlacingItems::fillingMode = checked;
       activeLvlEditWin()->scene->setItemPlacer(1, LvlPlacingItems::bgoSet.id );
       WriteToLog(QtDebugMsg, QString("BGO Square draw -> %1").arg(checked));
       activeLvlEditWin()->setFocus();
    }
}


void MainWindow::on_PROPS_BlockInvis_clicked(bool checked)
{
    if(blockPtr<1)
    {
        LvlPlacingItems::blockSet.invisible = checked;
    }
    else
    if (activeChildWindow()==1)
    {
        LevelData selData;
        QList<QGraphicsItem *> items = activeLvlEditWin()->scene->selectedItems();
        foreach(QGraphicsItem * item, items)
        {
            if(item->data(0).toString()=="Block")
            {
                selData.blocks.push_back(((ItemBlock *) item)->blockData);
                ((ItemBlock*)item)->setInvisible(checked);
            }
        }
        activeLvlEditWin()->scene->addChangeSettingsHistory(selData, LvlScene::SETTING_INVISIBLE, QVariant(checked));
    }

}
void MainWindow::on_PROPS_BlkSlippery_clicked(bool checked)
{
    if(blockPtr<1)
    {
        LvlPlacingItems::blockSet.slippery = checked;
    }
    else
    if (activeChildWindow()==1)
    {
        LevelData selData;
        QList<QGraphicsItem *> items = activeLvlEditWin()->scene->selectedItems();
        foreach(QGraphicsItem * item, items)
        {
            if((item->data(0).toString()=="Block")/*&&((item->data(2).toInt()==blockPtr))*/)
            {
                selData.blocks.push_back(((ItemBlock *) item)->blockData);
                ((ItemBlock*)item)->setSlippery(checked);
            }
        }
        activeLvlEditWin()->scene->addChangeSettingsHistory(selData, LvlScene::SETTING_SLIPPERY, QVariant(checked));
    }

}

void MainWindow::on_PROPS_BlockIncludes_clicked()
{
    int npcID=0;

    if(blockPtr<1)
    {
        npcID = LvlPlacingItems::blockSet.npc_id;
    }
    else
    if (activeChildWindow()==1)
    {
        QList<QGraphicsItem *> items1 = activeLvlEditWin()->scene->selectedItems();
        foreach(QGraphicsItem * targetItem, items1)
        {
            if((targetItem->data(0).toString()=="Block")&&((targetItem->data(2).toInt()==blockPtr)))
            {
                npcID = ((ItemBlock*)targetItem)->blockData.npc_id;
                break;
            }
        }
    }

    LevelData selData;

    NpcDialog * npcList = new NpcDialog(&configs);
    npcList->setWindowFlags (Qt::Window | Qt::WindowTitleHint | Qt::WindowCloseButtonHint);
    npcList->setGeometry(QStyle::alignedRect(Qt::LeftToRight, Qt::AlignCenter, npcList->size(), qApp->desktop()->availableGeometry()));
    npcList->setState(npcID);

    if(npcList->exec()==QDialog::Accepted)
    {
        //apply to all selected items.
        int selected_npc=0;
        if(npcList->isEmpty)
            selected_npc = 0;
        else
        if(npcList->isCoin)
            selected_npc = npcList->coins;
        else
            selected_npc = npcList->selectedNPC+1000;


        ui->PROPS_BlockIncludes->setText(
                    ((selected_npc>0)?
                         ((selected_npc>1000)?QString("NPC-%1").arg(selected_npc-1000):tr("%1 coins").arg(selected_npc))
                       :tr("[empty]")
                         ) );

        if(blockPtr<1)
        {
            LvlPlacingItems::blockSet.npc_id = selected_npc;
        }
        else
        if (activeChildWindow()==1)
        {
            QList<QGraphicsItem *> items = activeLvlEditWin()->scene->selectedItems();
            foreach(QGraphicsItem * item, items)
            {
                if((item->data(0).toString()=="Block")/*&&((item->data(2).toInt()==blockPtr))*/)
                {
                    //((ItemBlock *)item)->blockData.npc_id = selected_npc;
                    //((ItemBlock *)item)->arrayApply();
                    selData.blocks.push_back(((ItemBlock *) item)->blockData);
                    ((ItemBlock *)item)->setIncludedNPC(selected_npc);
                    //break;
                }
            }

            activeLvlEditWin()->scene->addChangeSettingsHistory(selData, LvlScene::SETTING_CHANGENPC, QVariant(selected_npc));
        }

    }

}


void MainWindow::on_PROPS_BlockLayer_currentIndexChanged(const QString &arg1)
{
    if(LvlItemPropsLock) return;

    if(blockPtr<1)
    {
        LvlPlacingItems::blockSet.layer = arg1;
    }
    else
    if (activeChildWindow()==1)
    {
        LevelData modData;
        QList<QGraphicsItem *> items = activeLvlEditWin()->scene->selectedItems();
        foreach(QGraphicsItem * item, items)
        {
            if((item->data(0).toString()=="Block")/*&&((item->data(2).toInt()==blockPtr))*/)
            {
                modData.blocks.push_back(((ItemBlock*)item)->blockData);
                ((ItemBlock*)item)->setLayer(arg1);
                //break;
            }
        }
        activeLvlEditWin()->scene->addChangedLayerHistory(modData, arg1);
    }

}


void MainWindow::on_PROPS_BlkEventDestroy_currentIndexChanged(const QString &arg1)
{
    if(LvlItemPropsLock) return;

    if(blockPtr<1)
    {
        if(ui->PROPS_BlkEventDestroy->currentIndex()>0)
            LvlPlacingItems::blockSet.event_destroy = arg1;
        else
            LvlPlacingItems::blockSet.event_destroy = "";
    }
    else
    if (activeChildWindow()==1)
    {
        QList<QGraphicsItem *> items = activeLvlEditWin()->scene->selectedItems();
        foreach(QGraphicsItem * item, items)
        {
            if((item->data(0).toString()=="Block")/*&&((item->data(2).toInt()==blockPtr))*/)
            {
                if(ui->PROPS_BlkEventDestroy->currentIndex()>0)
                    ((ItemBlock*)item)->blockData.event_destroy = arg1;
                else
                    ((ItemBlock*)item)->blockData.event_destroy = "";
                ((ItemBlock*)item)->arrayApply();
                //break;
            }
        }
    }

}
void MainWindow::on_PROPS_BlkEventHited_currentIndexChanged(const QString &arg1)
{
    if(LvlItemPropsLock) return;

    if(blockPtr<1)
    {
        if(ui->PROPS_BlkEventHited->currentIndex()>0)
            LvlPlacingItems::blockSet.event_hit = arg1;
        else
            LvlPlacingItems::blockSet.event_hit = "";
    }
    else
    if (activeChildWindow()==1)
    {
        QList<QGraphicsItem *> items = activeLvlEditWin()->scene->selectedItems();
        foreach(QGraphicsItem * item, items)
        {
            if((item->data(0).toString()=="Block")/*&&((item->data(2).toInt()==blockPtr))*/)
            {
                if(ui->PROPS_BlkEventHited->currentIndex()>0)
                    ((ItemBlock*)item)->blockData.event_hit = arg1;
                else
                    ((ItemBlock*)item)->blockData.event_hit = "";
                ((ItemBlock*)item)->arrayApply();
                //break;
            }
        }
    }

}
void MainWindow::on_PROPS_BlkEventLayerEmpty_currentIndexChanged(const QString &arg1)
{
    if(LvlItemPropsLock) return;

    if(blockPtr<1)
    {
        if(ui->PROPS_BlkEventLayerEmpty->currentIndex()>0)
            LvlPlacingItems::blockSet.event_no_more = arg1;
        else
            LvlPlacingItems::blockSet.event_no_more = "";
    }

    else
    if (activeChildWindow()==1)
    {
        QList<QGraphicsItem *> items = activeLvlEditWin()->scene->selectedItems();
        foreach(QGraphicsItem * item, items)
        {
            if((item->data(0).toString()=="Block")/*&&((item->data(2).toInt()==blockPtr))*/)
            {
                if(ui->PROPS_BlkEventLayerEmpty->currentIndex()>0)
                    ((ItemBlock*)item)->blockData.event_no_more = arg1;
                else
                    ((ItemBlock*)item)->blockData.event_no_more = "";
                ((ItemBlock*)item)->arrayApply();
                //break;
            }
        }
    }

}







////////////////////////////////////////////////////////////////////////////////////////////

// ///////////BGO///////////////////////////

void MainWindow::on_PROPS_BGOLayer_currentIndexChanged(const QString &arg1)
{
    if(LvlItemPropsLock) return;

    if(bgoPtr<1)
    {
        LvlPlacingItems::bgoSet.layer = arg1;
    }
    else
    if (activeChildWindow()==1)
    {
        LevelData modData;
        QList<QGraphicsItem *> items = activeLvlEditWin()->scene->selectedItems();
        foreach(QGraphicsItem * item, items)
        {
            if((item->data(0).toString()=="BGO")/*&&((item->data(2).toInt()==bgoPtr))*/)
            {
                modData.bgo.push_back(((ItemBGO*)item)->bgoData);
                ((ItemBGO*)item)->setLayer(arg1);
                //break;
            }
        }
        activeLvlEditWin()->scene->addChangedLayerHistory(modData, arg1);
    }

}


void MainWindow::on_PROPS_BGO_smbx64_sp_valueChanged(int arg1)
{
    if(LvlItemPropsLock) return;

    if(bgoPtr<1)
    {
        LvlPlacingItems::bgoSet.smbx64_sp = arg1;
    }
    else
    if (activeChildWindow()==1)
    {
        QList<QGraphicsItem *> items = activeLvlEditWin()->scene->selectedItems();
        foreach(QGraphicsItem * item, items)
        {
            if((item->data(0).toString()=="BGO")/*&&((item->data(2).toInt()==bgoPtr))*/)
            {
                ((ItemBGO*)item)->bgoData.smbx64_sp = arg1;
                ((ItemBGO*)item)->arrayApply();
                //break;
            }
        }
    }

}


////////////////////////////////////////////////////////////////////////////////////////////

// ///////////NPC///////////////////////////


void MainWindow::on_PROPS_NPCDirLeft_clicked()
{
    if(LvlItemPropsLock) return;
    if(npcPtr<1)
    {
        LvlPlacingItems::npcSet.direct = -1;
    }
    else
    if (activeChildWindow()==1)
    {
        LevelData selData;
        QList<QGraphicsItem *> items = activeLvlEditWin()->scene->selectedItems();
        foreach(QGraphicsItem * item, items)
        {
            if((item->data(0).toString()=="NPC")/*&&((item->data(2).toInt()==npcPtr))*/)
            {
                selData.npc.push_back(((ItemNPC*)item)->npcData);
                ((ItemNPC*)item)->changeDirection(-1);
                //break;
            }
        }
        activeLvlEditWin()->scene->addChangeSettingsHistory(selData, LvlScene::SETTING_DIRECTION, QVariant(-1));
    }

}


void MainWindow::on_PROPS_NPCDirRand_clicked()
{
    if(LvlItemPropsLock) return;
    if(npcPtr<1)
    {
        LvlPlacingItems::npcSet.direct = 0;
    }
    else
    if (activeChildWindow()==1)
    {
        LevelData selData;
        QList<QGraphicsItem *> items = activeLvlEditWin()->scene->selectedItems();
        foreach(QGraphicsItem * item, items)
        {
            if((item->data(0).toString()=="NPC")/*&&((item->data(2).toInt()==npcPtr))*/)
            {
                selData.npc.push_back(((ItemNPC*)item)->npcData);
                ((ItemNPC*)item)->changeDirection(0);

                //break;
            }
        }
        activeLvlEditWin()->scene->addChangeSettingsHistory(selData, LvlScene::SETTING_DIRECTION, QVariant(0));
    }
}

void MainWindow::on_PROPS_NPCDirRight_clicked()
{
    if(LvlItemPropsLock) return;
    if(npcPtr<1)
    {
        LvlPlacingItems::npcSet.direct = 1;
    }
    else
    if (activeChildWindow()==1)
    {
        LevelData selData;
        QList<QGraphicsItem *> items = activeLvlEditWin()->scene->selectedItems();
        foreach(QGraphicsItem * item, items)
        {
            if((item->data(0).toString()=="NPC")/*&&((item->data(2).toInt()==npcPtr))*/)
            {
                selData.npc.push_back(((ItemNPC*)item)->npcData);
                ((ItemNPC*)item)->changeDirection(1);
                //break;
            }
        }
        activeLvlEditWin()->scene->addChangeSettingsHistory(selData, LvlScene::SETTING_DIRECTION, QVariant(1));
    }
}




void MainWindow::on_PROPS_NpcFri_clicked(bool checked)
{
    if(LvlItemPropsLock) return;
    if(npcPtr<1)
    {
        LvlPlacingItems::npcSet.friendly = checked;
    }
    else
    if (activeChildWindow()==1)
    {
        LevelData selData;
        QList<QGraphicsItem *> items = activeLvlEditWin()->scene->selectedItems();
        foreach(QGraphicsItem * item, items)
        {
            if((item->data(0).toString()=="NPC")/*&&((item->data(2).toInt()==npcPtr))*/)
            {
                ((ItemNPC*)item)->setFriendly(checked);
                selData.npc.push_back(((ItemNPC*)item)->npcData);
                //break;
            }
        }
        activeLvlEditWin()->scene->addChangeSettingsHistory(selData, LvlScene::SETTING_FRIENDLY, QVariant(checked));
    }

}

void MainWindow::on_PROPS_NPCNoMove_clicked(bool checked)
{
    if(LvlItemPropsLock) return;
    if(npcPtr<1)
    {
        LvlPlacingItems::npcSet.nomove = checked;
    }
    else
    if (activeChildWindow()==1)
    {
        LevelData selData;
        QList<QGraphicsItem *> items = activeLvlEditWin()->scene->selectedItems();
        foreach(QGraphicsItem * item, items)
        {
            if((item->data(0).toString()=="NPC")/*&&((item->data(2).toInt()==npcPtr))*/)
            {
                ((ItemNPC*)item)->setNoMovable(checked);
                selData.npc.push_back(((ItemNPC*)item)->npcData);
                //break;
            }
        }
        activeLvlEditWin()->scene->addChangeSettingsHistory(selData, LvlScene::SETTING_NOMOVEABLE, QVariant(checked));
    }

}
void MainWindow::on_PROPS_NpcBoss_clicked(bool checked)
{
    if(LvlItemPropsLock) return;
    if(npcPtr<1)
    {
        LvlPlacingItems::npcSet.legacyboss = checked;
    }
    else
    if (activeChildWindow()==1)
    {
        LevelData selData;
        QList<QGraphicsItem *> items = activeLvlEditWin()->scene->selectedItems();
        foreach(QGraphicsItem * item, items)
        {
            if((item->data(0).toString()=="NPC")/*&&((item->data(2).toInt()==npcPtr))*/)
            {
                ((ItemNPC*)item)->setLegacyBoss(checked);
                selData.npc.push_back(((ItemNPC*)item)->npcData);
                //break;
            }
        }
        activeLvlEditWin()->scene->addChangeSettingsHistory(selData, LvlScene::SETTING_BOSS, QVariant(checked));
    }
}

void MainWindow::on_PROPS_NpcTMsg_clicked()
{

    //LevelData selData;
    //QList<QVariant> modText;
    //modText.push_back(QVariant(npcData.msg));

    QString message;
    if(npcPtr<1)
    {
        message = LvlPlacingItems::npcSet.msg;
    }
    else
    {
        QList<QGraphicsItem *> items = activeLvlEditWin()->scene->selectedItems();
        foreach(QGraphicsItem * SelItem, items )
        {
            if(SelItem->data(0).toString()=="NPC")
            {
                message = ((ItemNPC *) SelItem)->npcData.msg; break;
            }
        }
    }

    ItemMsgBox * msgBox = new ItemMsgBox(message);
    msgBox->setWindowFlags (Qt::Window | Qt::WindowTitleHint | Qt::WindowCloseButtonHint);
    msgBox->setGeometry(QStyle::alignedRect(Qt::LeftToRight, Qt::AlignCenter, msgBox->size(), qApp->desktop()->availableGeometry()));
    if(msgBox->exec()==QDialog::Accepted)
    {

        if(npcPtr<1)
        {
            LvlPlacingItems::npcSet.msg = msgBox->currentText;
        }
        else
        {
            LevelData selData;
            QList<QGraphicsItem *> items = activeLvlEditWin()->scene->selectedItems();
            foreach(QGraphicsItem * SelItem, items )
            {
                if(SelItem->data(0).toString()=="NPC"){
                    selData.npc.push_back(((ItemNPC *) SelItem)->npcData);
                    ((ItemNPC *) SelItem)->setMsg( msgBox->currentText );
                }
            }
            activeLvlEditWin()->scene->addChangeSettingsHistory(selData, LvlScene::SETTING_MESSAGE, QVariant(msgBox->currentText));
        }

        QString npcmsg = (msgBox->currentText.isEmpty() ? tr("[none]") : msgBox->currentText);
        if(npcmsg.size()>20)
        {
            npcmsg.resize(18);
            npcmsg.push_back("...");
        }
        ui->PROPS_NpcTMsg->setText( npcmsg );
    }

}

void MainWindow::on_PROPS_NPCSpecialSpin_valueChanged(int arg1)
{
    if(LvlItemPropsLock) return;
    if(npcPtr<1)
    {
        LvlPlacingItems::npcSet.special_data = arg1;
    }
    else
    if (activeChildWindow()==1)
    {
        QList<QGraphicsItem *> items = activeLvlEditWin()->scene->selectedItems();
        foreach(QGraphicsItem * item, items)
        {
            if(item->data(0).toString()=="NPC")
            {
                ((ItemNPC*)item)->npcData.special_data = arg1 - npcSpecSpinOffset;
                ((ItemNPC*)item)->arrayApply();
            }
        }
    }

}

void MainWindow::on_PROPS_NPCContaiter_clicked()
{
    int npcID=0;

    if(npcPtr<1)
    {
        npcID = LvlPlacingItems::npcSet.special_data;
    }
    else
    if (activeChildWindow()==1)
    {
        QList<QGraphicsItem *> items1 = activeLvlEditWin()->scene->selectedItems();
        foreach(QGraphicsItem * targetItem, items1)
        {
            if((targetItem->data(0).toString()=="NPC")&&((targetItem->data(2).toInt()==npcPtr)))
            {
                npcID = ((ItemNPC*)targetItem)->npcData.special_data;
                break;
            }
        }
    }

    //LevelData selData;
    //QList<QVariant> modNPC;

    NpcDialog * npcList = new NpcDialog(&configs);
    npcList->setWindowFlags (Qt::Window | Qt::WindowTitleHint | Qt::WindowCloseButtonHint);
    npcList->setGeometry(QStyle::alignedRect(Qt::LeftToRight, Qt::AlignCenter, npcList->size(), qApp->desktop()->availableGeometry()));
    npcList->setState(npcID, 2);

    if(npcList->exec()==QDialog::Accepted)
    {
        //apply to all selected items.
        int selected_npc=0;
        if(npcList->isEmpty)
            selected_npc = 0;
        else
            selected_npc = npcList->selectedNPC;

        ui->PROPS_NPCContaiter->setText(
                    ((selected_npc>0)?QString("NPC-%1").arg(selected_npc)
                       :tr("[empty]")
                         ) );

        if(npcPtr<1)
        {
            LvlPlacingItems::npcSet.special_data = selected_npc;
        }
        else
        if (activeChildWindow()==1)
        {
            QList<QGraphicsItem *> items = activeLvlEditWin()->scene->selectedItems();
            foreach(QGraphicsItem * item, items)
            {
                if((item->data(0).toString()=="NPC")/*&&((item->data(2).toInt()==blockPtr))*/)
                {
                    ((ItemNPC *)item)->setIncludedNPC(selected_npc);
                }
            }
        }
    }

}

void MainWindow::on_PROPS_NPCSpecialBox_currentIndexChanged(int index)
{
    if(LvlItemPropsLock) return;
    if(npcPtr<1)
    {
        LvlPlacingItems::npcSet.special_data = index;
    }
    else
    if (activeChildWindow()==1)
    {
        QList<QGraphicsItem *> items = activeLvlEditWin()->scene->selectedItems();
        foreach(QGraphicsItem * item, items)
        {
            if(item->data(0).toString()=="NPC")
            {
                ((ItemNPC*)item)->npcData.special_data = index;
                ((ItemNPC*)item)->arrayApply();
            }
        }
    }
}

void MainWindow::on_PROPS_NpcGenerator_clicked(bool checked)
{
    if(LvlItemPropsLock) return;
    if(npcPtr<1)
    {
        LvlPlacingItems::npcSet.generator = checked;
        if(checked)
            LvlPlacingItems::gridSz = 16;
        else
            LvlPlacingItems::gridSz = LvlPlacingItems::npcGrid;


        LvlItemPropsLock=true;

        ui->PROPS_NPCGenType->setCurrentIndex( (LvlPlacingItems::npcSet.generator_type)-1);
        ui->PROPS_NPCGenTime->setValue( (double)(LvlPlacingItems::npcSet.generator_period)/10);

        switch(LvlPlacingItems::npcSet.generator_direct)
        {
        case 2:
            ui->PROPS_NPCGenLeft->setChecked(true);
            break;
        case 3:
            ui->PROPS_NPCGenDown->setChecked(true);
            break;
        case 4:
            ui->PROPS_NPCGenRight->setChecked(true);
            break;
        case 1:
        default:
            ui->PROPS_NPCGenUp->setChecked(true);
            break;
        }
        LvlItemPropsLock=false;

    }
    else
    if (activeChildWindow()==1)
    {
        LevelData modData;
        QList<QGraphicsItem *> items = activeLvlEditWin()->scene->selectedItems();
        foreach(QGraphicsItem * item, items)
        {
            if(item->data(0).toString()=="NPC")
            {
                modData.npc.push_back(((ItemNPC*)item)->npcData);
                ((ItemNPC*)item)->setGenerator(checked,
                 ((ItemNPC*)item)->npcData.generator_direct,
                 ((ItemNPC*)item)->npcData.generator_type
                 );
                LvlItemPropsLock=true;
                ui->PROPS_NPCGenType->setCurrentIndex( (((ItemNPC*)item)->npcData.generator_type)-1);
                ui->PROPS_NPCGenTime->setValue( (double)(((ItemNPC*)item)->npcData.generator_period)/10);

                switch(((ItemNPC*)item)->npcData.generator_direct)
                {
                case 2:
                    ui->PROPS_NPCGenLeft->setChecked(true);
                    break;
                case 3:
                    ui->PROPS_NPCGenDown->setChecked(true);
                    break;
                case 4:
                    ui->PROPS_NPCGenRight->setChecked(true);
                    break;
                case 1:
                default:
                    ui->PROPS_NPCGenUp->setChecked(true);
                    break;
                }
                LvlItemPropsLock=false;
            }
        }
        activeLvlEditWin()->scene->addChangeSettingsHistory(modData, LvlScene::SETTING_GENACTIVATE, QVariant(checked));
    }
    ui->PROPS_NPCGenBox->setVisible( checked );


}
void MainWindow::on_PROPS_NPCGenType_currentIndexChanged(int index)
{
    if(LvlItemPropsLock) return;
    if(npcPtr<1)
    {
        LvlPlacingItems::npcSet.generator_type = index+1;
    }
    else
    if (activeChildWindow()==1)
    {
        LevelData modData;
        QList<QGraphicsItem *> items = activeLvlEditWin()->scene->selectedItems();
        foreach(QGraphicsItem * item, items)
        {
            if(item->data(0).toString()=="NPC")
            {
                modData.npc.push_back(((ItemNPC*)item)->npcData);
                ((ItemNPC*)item)->setGenerator(((ItemNPC*)item)->npcData.generator,
                 ((ItemNPC*)item)->npcData.generator_direct,
                 index+1
                 );
            }
        }
        activeLvlEditWin()->scene->addChangeSettingsHistory(modData, LvlScene::SETTING_GENTYPE, QVariant(index+1));
    }
}
void MainWindow::on_PROPS_NPCGenTime_valueChanged(double arg1)
{
    if(LvlItemPropsLock) return;
    if(npcPtr<1)
    {
        LvlPlacingItems::npcSet.generator_period = qRound(arg1*10);
    }
    else
    if (activeChildWindow()==1)
    {
        LevelData modData;
        QList<QGraphicsItem *> items = activeLvlEditWin()->scene->selectedItems();
        foreach(QGraphicsItem * item, items)
        {
            if((item->data(0).toString()=="NPC")/*&&((item->data(2).toInt()==npcPtr))*/)
            {
                modData.npc.push_back(((ItemNPC*)item)->npcData);
                ((ItemNPC*)item)->npcData.generator_period = qRound(arg1*10);
                ((ItemNPC*)item)->arrayApply();
            }
        }
        activeLvlEditWin()->scene->addChangeSettingsHistory(modData, LvlScene::SETTING_GENTIME, QVariant(qRound(arg1*10)));
    }

}

void MainWindow::on_PROPS_NPCGenUp_clicked()
{
    if(LvlItemPropsLock) return;
    if(npcPtr<1)
    {
        LvlPlacingItems::npcSet.generator_direct = 1;
    }
    else
    if (activeChildWindow()==1)
    {
        LevelData modData;
        QList<QGraphicsItem *> items = activeLvlEditWin()->scene->selectedItems();
        foreach(QGraphicsItem * item, items)
        {
            if((item->data(0).toString()=="NPC")/*&&((item->data(2).toInt()==npcPtr))*/)
            {
                modData.npc.push_back(((ItemNPC*)item)->npcData);
                ((ItemNPC*)item)->setGenerator(((ItemNPC*)item)->npcData.generator,
                 1,
                 ((ItemNPC*)item)->npcData.generator_type
                 );
            }
        }
        activeLvlEditWin()->scene->addChangeSettingsHistory(modData, LvlScene::SETTING_GENDIR, QVariant(1));
    }

}

void MainWindow::on_PROPS_NPCGenLeft_clicked()
{
    if(LvlItemPropsLock) return;
    if(npcPtr<1)
    {
        LvlPlacingItems::npcSet.generator_direct = 2;
    }
    else
    if (activeChildWindow()==1)
    {
        LevelData modData;
        QList<QGraphicsItem *> items = activeLvlEditWin()->scene->selectedItems();
        foreach(QGraphicsItem * item, items)
        {
            if((item->data(0).toString()=="NPC")/*&&((item->data(2).toInt()==npcPtr))*/)
            {
                modData.npc.push_back(((ItemNPC*)item)->npcData);
                ((ItemNPC*)item)->setGenerator(((ItemNPC*)item)->npcData.generator,
                 2,
                 ((ItemNPC*)item)->npcData.generator_type
                 );
            }
        }
        activeLvlEditWin()->scene->addChangeSettingsHistory(modData, LvlScene::SETTING_GENDIR, QVariant(2));
    }
}

void MainWindow::on_PROPS_NPCGenDown_clicked()
{
    if(LvlItemPropsLock) return;
    if(npcPtr<1)
    {
        LvlPlacingItems::npcSet.generator_direct = 3;
    }
    else
    if (activeChildWindow()==1)
    {
        LevelData modData;
        QList<QGraphicsItem *> items = activeLvlEditWin()->scene->selectedItems();
        foreach(QGraphicsItem * item, items)
        {
            if((item->data(0).toString()=="NPC")/*&&((item->data(2).toInt()==npcPtr))*/)
            {
                modData.npc.push_back(((ItemNPC*)item)->npcData);
                ((ItemNPC*)item)->setGenerator(((ItemNPC*)item)->npcData.generator,
                 3,
                 ((ItemNPC*)item)->npcData.generator_type
                 );
            }
        }
        activeLvlEditWin()->scene->addChangeSettingsHistory(modData, LvlScene::SETTING_GENDIR, QVariant(3));
    }
}
void MainWindow::on_PROPS_NPCGenRight_clicked()
{
    if(LvlItemPropsLock) return;
    if(npcPtr<1)
    {
        LvlPlacingItems::npcSet.generator_direct = 4;
    }
    else
    if (activeChildWindow()==1)
    {
        LevelData modData;
        QList<QGraphicsItem *> items = activeLvlEditWin()->scene->selectedItems();
        foreach(QGraphicsItem * item, items)
        {
            if((item->data(0).toString()=="NPC")/*&&((item->data(2).toInt()==npcPtr))*/)
            {
                modData.npc.push_back(((ItemNPC*)item)->npcData);
                ((ItemNPC*)item)->setGenerator(((ItemNPC*)item)->npcData.generator,
                 4,
                 ((ItemNPC*)item)->npcData.generator_type
                 );
            }
        }
        activeLvlEditWin()->scene->addChangeSettingsHistory(modData, LvlScene::SETTING_GENDIR, QVariant(4));
    }
}


void MainWindow::on_PROPS_NpcLayer_currentIndexChanged(const QString &arg1)
{
    if(LvlItemPropsLock) return;

    if(npcPtr<1)
    {
        LvlPlacingItems::npcSet.layer = arg1;
    }
    else
    if (activeChildWindow()==1)
    {
        LevelData modData;
        QList<QGraphicsItem *> items = activeLvlEditWin()->scene->selectedItems();
        foreach(QGraphicsItem * item, items)
        {
            if(item->data(0).toString()=="NPC")
            {
                modData.npc.push_back(((ItemNPC*)item)->npcData);
                ((ItemNPC*)item)->setLayer(arg1);
            }
        }
        activeLvlEditWin()->scene->addChangedLayerHistory(modData, arg1);
    }

}

void MainWindow::on_PROPS_NpcAttachLayer_currentIndexChanged(const QString &arg1)
{
    if(LvlItemPropsLock) return;

    if(npcPtr<1)
    {
        if(ui->PROPS_NpcAttachLayer->currentIndex()>0)
            LvlPlacingItems::npcSet.attach_layer = arg1;
        else
            LvlPlacingItems::npcSet.attach_layer = "";
    }
    else
    if (activeChildWindow()==1)
    {
        LevelData modData;
        QList<QGraphicsItem *> items = activeLvlEditWin()->scene->selectedItems();
        foreach(QGraphicsItem * item, items)
        {
            if(item->data(0).toString()=="NPC")
            {
                modData.npc.push_back(((ItemNPC*)item)->npcData);
                if(ui->PROPS_NpcAttachLayer->currentIndex()>0)
                {
                    ((ItemNPC*)item)->npcData.attach_layer = arg1;
                }
                else
                {
                    ((ItemNPC*)item)->npcData.attach_layer = "";
                }
                ((ItemNPC*)item)->arrayApply();
            }
        }
        if(ui->PROPS_NpcAttachLayer->currentIndex()>0)
        {
            activeLvlEditWin()->scene->addChangeSettingsHistory(modData, LvlScene::SETTING_ATTACHLAYER, QVariant(arg1));
        }
        else
        {
            activeLvlEditWin()->scene->addChangeSettingsHistory(modData, LvlScene::SETTING_ATTACHLAYER, QVariant(""));
        }
    }

}
void MainWindow::on_PROPS_NpcEventActivate_currentIndexChanged(const QString &arg1)
{
    if(LvlItemPropsLock) return;

    if(npcPtr<1)
    {
        if(ui->PROPS_NpcEventActivate->currentIndex()>0)
            LvlPlacingItems::npcSet.event_activate = arg1;
        else
            LvlPlacingItems::npcSet.event_activate = "";
    }
    else
    if (activeChildWindow()==1)
    {
        QList<QGraphicsItem *> items = activeLvlEditWin()->scene->selectedItems();
        foreach(QGraphicsItem * item, items)
        {
            if(item->data(0).toString()=="NPC")
            {
                if(ui->PROPS_NpcEventActivate->currentIndex()>0)
                    ((ItemNPC*)item)->npcData.event_activate = arg1;
                else
                    ((ItemNPC*)item)->npcData.event_activate = "";
                ((ItemNPC*)item)->arrayApply();
            }
        }
    }

}
void MainWindow::on_PROPS_NpcEventDeath_currentIndexChanged(const QString &arg1)
{
    if(LvlItemPropsLock) return;
    if(npcPtr<1)
    {
        if(ui->PROPS_NpcEventDeath->currentIndex()>0)
            LvlPlacingItems::npcSet.event_die = arg1;
        else
            LvlPlacingItems::npcSet.event_die = "";
    }
    else
    if (activeChildWindow()==1)
    {
        QList<QGraphicsItem *> items = activeLvlEditWin()->scene->selectedItems();
        foreach(QGraphicsItem * item, items)
        {
            if(item->data(0).toString()=="NPC")
            {
                if(ui->PROPS_NpcEventDeath->currentIndex()>0)
                    ((ItemNPC*)item)->npcData.event_die = arg1;
                else
                    ((ItemNPC*)item)->npcData.event_die = "";
                ((ItemNPC*)item)->arrayApply();
            }
        }
    }

}
void MainWindow::on_PROPS_NpcEventTalk_currentIndexChanged(const QString &arg1)
{
    if(LvlItemPropsLock) return;

    if(npcPtr<1)
    {
        if(ui->PROPS_NpcEventTalk->currentIndex()>0)
            LvlPlacingItems::npcSet.event_talk = arg1;
        else
            LvlPlacingItems::npcSet.event_talk = "";
    }
    else
    if (activeChildWindow()==1)
    {
        QList<QGraphicsItem *> items = activeLvlEditWin()->scene->selectedItems();
        foreach(QGraphicsItem * item, items)
        {
            if(item->data(0).toString()=="NPC")
            {
                if(ui->PROPS_NpcEventTalk->currentIndex()>0)
                    ((ItemNPC*)item)->npcData.event_talk = arg1;
                else
                    ((ItemNPC*)item)->npcData.event_talk = "";
                ((ItemNPC*)item)->arrayApply();
            }
        }
    }

}
void MainWindow::on_PROPS_NpcEventEmptyLayer_currentIndexChanged(const QString &arg1)
{
    if(LvlItemPropsLock) return;

    if(npcPtr<1)
    {
        if(ui->PROPS_NpcEventEmptyLayer->currentIndex()>0)
            LvlPlacingItems::npcSet.event_nomore = arg1;
        else
            LvlPlacingItems::npcSet.event_nomore = "";
    }
    else
    if (activeChildWindow()==1)
    {
        QList<QGraphicsItem *> items = activeLvlEditWin()->scene->selectedItems();
        foreach(QGraphicsItem * item, items)
        {
            if(item->data(0).toString()=="NPC")
            {
                if(ui->PROPS_NpcEventEmptyLayer->currentIndex()>0)
                    ((ItemNPC*)item)->npcData.event_nomore = arg1;
                else
                    ((ItemNPC*)item)->npcData.event_nomore = "";
                ((ItemNPC*)item)->arrayApply();
            }
        }
    }

}

