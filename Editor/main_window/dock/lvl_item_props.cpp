/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2014 Vitaly Novichkov <admin@wohlnet.ru>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA
 */


#include "../../ui_mainwindow.h"
#include "../../mainwindow.h"
#include "../../level_scene/lvl_item_placing.h"




void MainWindow::LvlItemProps(int Type, LevelBlock block, LevelBGO bgo, LevelNPC npc, bool newItem)
{
    ui->blockProp->hide();
    ui->bgoProps->hide();
    ui->npcProps->hide();

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


        ui->PROPS_bgoPos->setText( tr("Position: [%1, %2]").arg(bgo.x).arg(bgo.y) );

        ui->PROPS_BGOLayer->setCurrentIndex(0);
        for(int i=0; i<ui->PROPS_BGOLayer->count();i++)
        {
            if(ui->PROPS_BGOLayer->itemText(i)==bgo.layer)
            {ui->PROPS_BGOLayer->setCurrentIndex(i); break;}
        }

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

        ui->PROPS_NPCContaiter->hide();

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
                ui->PROPS_NPCSpecialSpin->setMinimum( configs.main_npc[j].special_spin_min );
                ui->PROPS_NPCSpecialSpin->setMaximum( configs.main_npc[j].special_spin_max );

                ui->PROPS_NPCSpecialSpin->setValue( npc.special_data );
                break;
            case 2:
                if(configs.main_npc[j].container)
                {
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
            case 1:
                ui->PROPS_NPCGenUp->setChecked(true);
                break;
            case 2:
                ui->PROPS_NPCGenLeft->setChecked(true);
                break;
            case 3:
                ui->PROPS_NPCGenDown->setChecked(true);
                break;
            case 4:
            default:
                ui->PROPS_NPCGenRight->setChecked(true);
                break;
            }

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

}




// ///////////BLOCKS///////////////////////////

void MainWindow::on_PROPS_BlockResize_clicked()
{

}




void MainWindow::on_PROPS_BlockSquareFill_clicked(bool checked)
{
    resetEditmodeButtons();
   //placeBlock

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
void MainWindow::on_PROPS_BlockInvis_clicked(bool checked)
{

}
void MainWindow::on_PROPS_BlkSlippery_clicked(bool checked)
{

}

void MainWindow::on_PROPS_BlockIncludes_clicked()
{

}


void MainWindow::on_PROPS_BlockLayer_currentIndexChanged(const QString &arg1)
{

}


void MainWindow::on_PROPS_BlkEventDestroy_currentIndexChanged(const QString &arg1)
{

}
void MainWindow::on_PROPS_BlkEventHited_currentIndexChanged(const QString &arg1)
{

}
void MainWindow::on_PROPS_BlkEventLayerEmpty_currentIndexChanged(const QString &arg1)
{

}

