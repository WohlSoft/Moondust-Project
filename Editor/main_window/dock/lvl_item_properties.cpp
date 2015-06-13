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

#include <PGE_File_Formats/file_formats.h>
#include <common_features/util.h>
#include <defines.h>
#include <editing/_scenes/level/lvl_item_placing.h>
#include <editing/_dialogs/itemselectdialog.h>
#include <editing/_scenes/level/items/item_block.h>
#include <editing/_scenes/level/items/item_bgo.h>
#include <editing/_scenes/level/items/item_npc.h>
#include <editing/_scenes/level/itemmsgbox.h>

#include "lvl_item_properties.h"
#include "ui_lvl_item_properties.h"

#include <ui_mainwindow.h>
#include <mainwindow.h>


LvlItemProperties::LvlItemProperties(QWidget *parent) :
    QDockWidget(parent),
    MWDock_Base(parent),
    ui(new Ui::LvlItemProperties)
{
    setVisible(false);
    setAttribute(Qt::WA_ShowWithoutActivating);
    ui->setupUi(this);
    npcSpecSpinOffset=0;
    npcSpecSpinOffset_2=0;
    LockItemProps=true;

    curItemType=-1;
    BlockEventDestroy="";
    BlockEventHit="";
    BlockEventLayerEmpty="";

    NpcEventActivated="";
    NpcEventDeath="";
    NpcEventTalk="";
    NpcEventLayerEmpty="";

    LvlItemPropsLock=true;

    blockPtr=-1;
    bgoPtr=-1;
    npcPtr=-1;

    QRect mwg = mw()->geometry();
    int GOffset=240;
    mw()->addDockWidget(Qt::RightDockWidgetArea, this);
    connect(mw(), SIGNAL(languageSwitched()), this, SLOT(re_translate()));

    #ifdef Q_OS_WIN
    setFloating(true);
    #endif

    setGeometry(
                mwg.x()+mwg.width()-width()-GOffset,
                mwg.y()+120,
                width(),
                height()
                );
}

LvlItemProperties::~LvlItemProperties()
{
    delete ui;
}

/******************Comobo boxes*********************************/
QComboBox *LvlItemProperties::cbox_layer_block()
{
    return ui->PROPS_BlockLayer;
}

QComboBox *LvlItemProperties::cbox_layer_bgo()
{
    return ui->PROPS_BGOLayer;
}

QComboBox *LvlItemProperties::cbox_layer_npc()
{
    return ui->PROPS_NpcLayer;
}

QComboBox *LvlItemProperties::cbox_layer_npc_att()
{
    return ui->PROPS_NpcAttachLayer;
}

QComboBox *LvlItemProperties::cbox_event_block_dest()
{
    return ui->PROPS_BlkEventDestroy;
}

QComboBox *LvlItemProperties::cbox_event_block_hit()
{
    return ui->PROPS_BlkEventHited;
}

QComboBox *LvlItemProperties::cbox_event_block_le()
{
    return ui->PROPS_BlkEventLayerEmpty;
}

QComboBox *LvlItemProperties::cbox_event_npc_act()
{
    return ui->PROPS_NpcEventActivate;
}
QComboBox *LvlItemProperties::cbox_event_npc_die()
{
    return ui->PROPS_NpcEventDeath;
}
QComboBox *LvlItemProperties::cbox_event_npc_talk()
{
    return ui->PROPS_NpcEventTalk;
}
QComboBox *LvlItemProperties::cbox_event_npc_le()
{
    return ui->PROPS_NpcEventEmptyLayer;
}
/******************Comobo boxes*********************************/

void LvlItemProperties::re_translate()
{
    LvlItemPropsLock=true;
    int npcGenType = ui->PROPS_NPCGenType->currentIndex(); //backup combobox's index

    ui->retranslateUi(this);

    ui->PROPS_NPCGenType->setCurrentIndex(npcGenType);
    LvlItemPropsLock=false;
}

void LvlItemProperties::on_LvlItemProperties_visibilityChanged(bool visible)
{
    mw()->ui->action_Placing_ShowProperties->setChecked(visible);
}


void LvlItemProperties::mousePressEvent(QMouseEvent *event)
{
    mouse_pos = event->pos();
    QDockWidget::mousePressEvent(event);

}

void LvlItemProperties::mouseMoveEvent(QMouseEvent *event)
{
    if (event->buttons() && Qt::LeftButton)
    {
        QPoint diff = event->pos() - mouse_pos;
        QPoint newpos = this->pos() + diff;
        this->move(newpos);
    }
    QDockWidget::mouseMoveEvent(event);
}


void LvlItemProperties::OpenBlock(LevelBlock block, bool newItem)
{
    LvlItemProps(ItemTypes::LVL_Block, block, FileFormats::dummyLvlBgo(), FileFormats::dummyLvlNpc(), newItem);
}

void LvlItemProperties::OpenBGO(LevelBGO bgo, bool newItem)
{
    LvlItemProps(ItemTypes::LVL_BGO, FileFormats::dummyLvlBlock(), bgo, FileFormats::dummyLvlNpc(), newItem);
}

void LvlItemProperties::OpenNPC(LevelNPC npc, bool newItem)
{
    LvlItemProps(ItemTypes::LVL_NPC, FileFormats::dummyLvlBlock(), FileFormats::dummyLvlBgo(), npc, newItem);
}

void LvlItemProperties::CloseBox()
{
    this->hide();
    mw()->ui->action_Placing_ShowProperties->setChecked(false);

    ui->blockProp->setVisible(false);
    ui->bgoProps->setVisible(false);
    ui->npcProps->setVisible(false);

    LvlItemPropsLock=true;
    LockItemProps = true;
    LvlPlacingItems::npcSpecialAutoIncrement=false;
}

void LvlItemProperties::LvlItemProps(int Type, LevelBlock block, LevelBGO bgo, LevelNPC npc, bool newItem)
{
    mw()->LayerListsSync();
    mw()->EventListsSync();

    ui->blockProp->setVisible(false);
    ui->bgoProps->setVisible(false);
    ui->npcProps->setVisible(false);

    LvlItemPropsLock=true;
    LockItemProps = true;

    LvlPlacingItems::npcSpecialAutoIncrement=false;

    ui->PROPS_BlkEventDestroyedLock->setVisible(newItem);
    ui->PROPS_BlkEventHitLock->setVisible(newItem);
    ui->PROPS_BlkEventLEmptyLock->setVisible(newItem);

    ui->PROPS_NpcEventActovateLock->setVisible(newItem);
    ui->PROPS_NpcEventDeathLock->setVisible(newItem);
    ui->PROPS_NpcEventTalkLock->setVisible(newItem);
    ui->PROPS_NpcEventLEmptyLock->setVisible(newItem);

    /*
    long blockPtr; //ArrayID of editing item (-1 - use system)
    long bgoPtr; //ArrayID of editing item
    long npcPtr; //ArrayID of editing item
    */
    LvlItemPropsLock=true;

    curItemType=Type;

    switch(Type)
    {
    case ItemTypes::LVL_Block:
    {
        if(newItem)
            blockPtr = -1;
        else
            blockPtr = block.array_id;

        ui->PROPS_BlockID->setText(tr("Block ID: %1, Array ID: %2").arg(block.id).arg(block.array_id));

        bool found=false;
        int j;

        //Check Index exists
        if(block.id < (unsigned int)mw()->configs.index_blocks.size())
        {
            j = mw()->configs.index_blocks[block.id].i;

            if(j<mw()->configs.main_block.size())
            {
            if(mw()->configs.main_block[j].id == block.id)
                found=true;
            }
        }
        //if Index found
        if(!found)
        {
            for(j=0;j<mw()->configs.main_block.size();j++)
            {
                if(mw()->configs.main_block[j].id==block.id)
                    break;
            }
        }
        if(j >= mw()->configs.main_block.size())
        {
            j=0;
        }

        if(blockPtr<0)
        {
            LvlPlacingItems::blockSet.invisible = mw()->configs.main_block[j].default_invisible_value;
            block.invisible = mw()->configs.main_block[j].default_invisible_value;

            LvlPlacingItems::blockSet.slippery = mw()->configs.main_block[j].default_slippery_value;
            block.slippery = mw()->configs.main_block[j].default_slippery_value;

            LvlPlacingItems::blockSet.npc_id = mw()->configs.main_block[j].default_content_value;
            block.npc_id = mw()->configs.main_block[j].default_content_value;

            LvlPlacingItems::blockSet.layer = LvlPlacingItems::layer.isEmpty()? "Default":LvlPlacingItems::layer;
            block.layer = LvlPlacingItems::layer.isEmpty()? "Default":LvlPlacingItems::layer;
            LvlPlacingItems::layer = block.layer;

            if(ui->PROPS_BlkEventDestroyedLock->isChecked())
                LvlPlacingItems::blockSet.event_destroy=BlockEventDestroy;
            else
                LvlPlacingItems::blockSet.event_destroy="";
            block.event_destroy = LvlPlacingItems::blockSet.event_destroy;

            if(ui->PROPS_BlkEventHitLock->isChecked())
                LvlPlacingItems::blockSet.event_hit=BlockEventHit;
            else
                LvlPlacingItems::blockSet.event_hit="";
            block.event_hit=LvlPlacingItems::blockSet.event_hit;

            if(ui->PROPS_BlkEventLEmptyLock->isChecked())
                LvlPlacingItems::blockSet.event_no_more=BlockEventLayerEmpty;
            else
                LvlPlacingItems::blockSet.event_no_more="";
            block.event_no_more=LvlPlacingItems::blockSet.event_no_more;
        }


        ui->PROPS_blockPos->setText( tr("Position: [%1, %2]").arg(block.x).arg(block.y) );
        ui->PROPS_BlockResize->setVisible( mw()->configs.main_block[j].sizable );
        ui->PROPS_BlockInvis->setChecked( block.invisible );
        ui->PROPS_BlkSlippery->setChecked( block.slippery );

        //ui->PROPS_BlockSquareFill->setVisible( newItem );
        //ui->PROPS_BlockSquareFill->setChecked( LvlPlacingItems::placingMode==LvlPlacingItems::PMODE_Square );

        ui->PROPS_BlockIncludes->setText(
                    ((block.npc_id!=0)?
                         ((block.npc_id>0)?QString("NPC-%1").arg(block.npc_id):tr("%1 coins").arg(block.npc_id*-1))
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

        LvlItemPropsLock=false;
        LockItemProps=false;

        this->setVisible(true);
        this->show();
        this->raise();
        ui->blockProp->show();
        ui->blockProp->raise();
        break;
    }
    case ItemTypes::LVL_BGO:
    {
        if(newItem)
            bgoPtr = -1;
        else
            bgoPtr = bgo.array_id;


        if(bgoPtr<0)
        {
            LvlPlacingItems::bgoSet.layer = LvlPlacingItems::layer.isEmpty()? "Default":LvlPlacingItems::layer;
            bgo.layer = LvlPlacingItems::layer.isEmpty()? "Default":LvlPlacingItems::layer;
            LvlPlacingItems::layer = bgo.layer;
        }

        ui->PROPS_BgoID->setText(tr("BGO ID: %1, Array ID: %2").arg(bgo.id).arg(bgo.array_id));

        //ui->PROPS_BGOSquareFill->setVisible( newItem );
        //ui->PROPS_BGOSquareFill->setChecked( LvlPlacingItems::placingMode==LvlPlacingItems::PMODE_Square );

        ui->PROPS_bgoPos->setText( tr("Position: [%1, %2]").arg(bgo.x).arg(bgo.y) );

        ui->PROPS_BGOLayer->setCurrentIndex(0);
        for(int i=0; i<ui->PROPS_BGOLayer->count();i++)
        {
            if(ui->PROPS_BGOLayer->itemText(i)==bgo.layer)
            {ui->PROPS_BGOLayer->setCurrentIndex(i); break;}
        }

        //PGE-X values
            bool isPGE=true;
            if(mw()->activeChildWindow()==1)
                isPGE = !mw()->activeLvlEditWin()->LvlData.smbx64strict;

            ui->PROPS_BGO_Z_Pos->setEnabled(isPGE);

            int zMode=2;
            switch(bgo.z_mode)
            {
                case LevelBGO::Background2:
                    zMode=0; break;
                case LevelBGO::Background1:
                    zMode=1; break;
                case LevelBGO::Foreground1:
                    zMode=3; break;
                case LevelBGO::Foreground2:
                    zMode=4; break;
                case LevelBGO::ZDefault:
                default:
                    zMode=2; break;
            }

            ui->PROPS_BGO_Z_Layer->setCurrentIndex(zMode);

            ui->PROPS_BGO_Z_Offset->setValue( bgo.z_offset );
        //PGE-X values

        ui->PROPS_BGO_smbx64_sp->setValue( bgo.smbx64_sp );

        LvlItemPropsLock=false;
        LockItemProps=false;

        mw()->ui->action_Placing_ShowProperties->setChecked(true);
        this->show();
        this->raise();
        ui->bgoProps->show();
        ui->bgoProps->raise();
        break;
    }
    case ItemTypes::LVL_NPC:
    {
        if(newItem)
            npcPtr = -1;
        else
            npcPtr = npc.array_id;

        ui->PROPS_NpcID->setText(tr("NPC ID: %1, Array ID: %2").arg(npc.id).arg(npc.array_id));

        bool found=false;
        int j;

        //Check Index exists
        if(npc.id < (unsigned int)mw()->configs.index_npc.size())
        {
            j = mw()->configs.index_npc[npc.id].i;

            if(j<mw()->configs.main_npc.size())
            {
            if(mw()->configs.main_npc[j].id == npc.id)
                found=true;
            }
        }
        //if Index found
        if(!found)
        {
            for(j=0;j<mw()->configs.main_npc.size();j++)
            {
                if(mw()->configs.main_npc[j].id==npc.id)
                    break;
            }
        }

        if(j >= mw()->configs.main_npc.size())
        {
            j=0;
        }

        ui->PROPS_NPCContaiter->hide();
        ui->PROPS_NpcContainsLabel->hide();

        ui->PROPS_NPCBoxLabel->hide();
        ui->PROPS_NPCSpecialBox->hide();

        ui->PROPS_NpcSpinLabel->hide();
        ui->PROPS_NPCSpecialSpin->hide();
        ui->PROPS_NPCSpecialSpin_Auto->hide();

        ui->line_6->hide();

        ui->PROPS_NpcSpecial2title->hide();
        ui->PROPS_NPCSpecial2Spin->hide();
        ui->PROPS_NPCSpecial2Box->hide();
        ui->Line_Special2_sep->hide();

        if(npcPtr<0)
        {
            LvlPlacingItems::npcSet.msg="";
            npc.msg="";

            LvlPlacingItems::npcSet.friendly = mw()->configs.main_npc[j].default_friendly_value;
            npc.friendly = mw()->configs.main_npc[j].default_friendly_value;

            LvlPlacingItems::npcSet.nomove = mw()->configs.main_npc[j].default_nomovable_value;
            npc.nomove = mw()->configs.main_npc[j].default_nomovable_value;

            LvlPlacingItems::npcSet.legacyboss = mw()->configs.main_npc[j].default_boss_value;
            npc.legacyboss = mw()->configs.main_npc[j].default_boss_value;

            if(mw()->configs.main_npc[j].default_special)
            {
                LvlPlacingItems::npcSet.special_data = mw()->configs.main_npc[j].default_special_value;
                npc.special_data = mw()->configs.main_npc[j].default_special_value;
            }

            LvlPlacingItems::npcSet.layer = LvlPlacingItems::layer.isEmpty()? "Default":LvlPlacingItems::layer;
            npc.layer = LvlPlacingItems::layer.isEmpty()? "Default":LvlPlacingItems::layer;
            LvlPlacingItems::layer = npc.layer;

            if(ui->PROPS_NpcEventActovateLock->isChecked())
                LvlPlacingItems::npcSet.event_activate=NpcEventActivated;
            else
                LvlPlacingItems::npcSet.event_activate="";
            npc.event_activate=LvlPlacingItems::npcSet.event_activate;

            if(ui->PROPS_NpcEventDeathLock->isChecked())
                LvlPlacingItems::npcSet.event_die=NpcEventDeath;
            else
                LvlPlacingItems::npcSet.event_die="";
            npc.event_die=LvlPlacingItems::npcSet.event_die;

            if(ui->PROPS_NpcEventTalkLock->isChecked())
                LvlPlacingItems::npcSet.event_talk=NpcEventTalk;
            else
                LvlPlacingItems::npcSet.event_talk="";
            npc.event_talk=LvlPlacingItems::npcSet.event_talk;

            if(ui->PROPS_NpcEventLEmptyLock->isChecked())
                LvlPlacingItems::npcSet.event_nomore=NpcEventLayerEmpty;
            else
                LvlPlacingItems::npcSet.event_nomore="";
            npc.event_nomore=LvlPlacingItems::npcSet.event_nomore;
        }

        ui->PROPS_NpcPos->setText( tr("Position: [%1, %2]").arg(npc.x).arg(npc.y) );

        if(mw()->configs.main_npc[j].direct_alt_title!="")
            ui->PROPS_NpcDir->setTitle(mw()->configs.main_npc[j].direct_alt_title);
        else
            ui->PROPS_NpcDir->setTitle( tr("Direction") );

        if(mw()->configs.main_npc[j].direct_alt_left!="")
            ui->PROPS_NPCDirLeft->setText( mw()->configs.main_npc[j].direct_alt_left );
        else
            ui->PROPS_NPCDirLeft->setText( tr("Left") );

        ui->PROPS_NPCDirRand->setEnabled( !mw()->configs.main_npc[j].direct_disable_random );

        if(mw()->configs.main_npc[j].direct_alt_right!="")
            ui->PROPS_NPCDirRight->setText( mw()->configs.main_npc[j].direct_alt_right );
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
        if( mw()->configs.main_npc[j].special_option )
        {
            ui->line_6->show();
            switch(mw()->configs.main_npc[j].special_type)
            {
            case 0:
                ui->PROPS_NPCBoxLabel->show();
                ui->PROPS_NPCBoxLabel->setText(mw()->configs.main_npc[j].special_name);
                ui->PROPS_NPCSpecialBox->show();

                if(newItem)
                {//Reset value to min, if it out of range
                    if((npc.special_data>=mw()->configs.main_npc[j].special_combobox_opts.size())||
                        (npc.special_data<0))
                    {
                       LvlPlacingItems::npcSet.special_data=0;
                       LvlPlacingItems::npcSet.special_data2=0;
                       npc.special_data=0;
                       npc.special_data2=0;
                    }
                }

                ui->PROPS_NPCSpecialBox->clear();
                for(int i=0; i < mw()->configs.main_npc[j].special_combobox_opts.size(); i++)
                {
                    ui->PROPS_NPCSpecialBox->addItem( mw()->configs.main_npc[j].special_combobox_opts[i] );
                    if(i==npc.special_data) ui->PROPS_NPCSpecialBox->setCurrentIndex(i);
                }

                break;
            case 1:
                ui->PROPS_NpcSpinLabel->show();
                ui->PROPS_NpcSpinLabel->setText( mw()->configs.main_npc[j].special_name );
                ui->PROPS_NPCSpecialSpin->show();

                if(npcPtr<0)
                {
                    ui->PROPS_NPCSpecialSpin_Auto->show();
                    ui->PROPS_NPCSpecialSpin_Auto->setChecked(false);
                }

                if(newItem)
                { //Reset value to min, if it out of range
                    if((npc.special_data>mw()->configs.main_npc[j].special_spin_max)||
                       (npc.special_data<mw()->configs.main_npc[j].special_spin_max))
                    {
                       LvlPlacingItems::npcSet.special_data = mw()->configs.main_npc[j].special_spin_min;
                       npc.special_data = mw()->configs.main_npc[j].special_spin_min;
                    }
                }

                npcSpecSpinOffset = mw()->configs.main_npc[j].special_spin_value_offset;
                ui->PROPS_NPCSpecialSpin->setMinimum( mw()->configs.main_npc[j].special_spin_min + npcSpecSpinOffset );
                ui->PROPS_NPCSpecialSpin->setMaximum( mw()->configs.main_npc[j].special_spin_max + npcSpecSpinOffset );

                ui->PROPS_NPCSpecialSpin->setValue( npc.special_data + npcSpecSpinOffset );
                LvlPlacingItems::npcSpecialAutoIncrement_begin = npc.special_data;

                break;
            case 2:
                if(mw()->configs.main_npc[j].container)
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
        //refresh special option 2
        refreshSecondSpecialOption(npc.id, npc.special_data, npc.special_data2, newItem);



        QString npcmsg = (npc.msg.isEmpty() ? tr("[none]") : npc.msg);
        if(npcmsg.size()>20)
        {
            npcmsg.resize(18);
            npcmsg.push_back("...");
        }
        ui->PROPS_NpcTMsg->setText( npcmsg.replace("&", "&&&") );

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
        LvlItemPropsLock=false;
        LockItemProps=false;

        mw()->ui->action_Placing_ShowProperties->setChecked(true);
        this->show();
        this->raise();
        ui->npcProps->show();
        ui->npcProps->raise();
        npc_refreshMinHeight();
        break;
    }
    case -1: //Nothing to edit
    default:
        this->hide();
        mw()->ui->action_Placing_ShowProperties->setChecked(false);
    }
}


void LvlItemProperties::LvlItemProps_updateLayer(QString lname)
{
    if(LvlItemPropsLock) return;
    LvlItemPropsLock=true;

    if(lname.isEmpty())
        lname=LvlPlacingItems::layer;

    switch(curItemType)
    {
        case 0:
        {
            ui->PROPS_BlockLayer->setCurrentIndex(0);
            for(int i=0; i<ui->PROPS_BlockLayer->count();i++)
            {
                if(ui->PROPS_BlockLayer->itemText(i)==lname)
                {ui->PROPS_BlockLayer->setCurrentIndex(i); break;}
            }
        }
        break;
        case 1:
        {
            ui->PROPS_BGOLayer->setCurrentIndex(0);
            for(int i=0; i<ui->PROPS_BGOLayer->count();i++)
            {
                if(ui->PROPS_BGOLayer->itemText(i)==lname)
                {ui->PROPS_BGOLayer->setCurrentIndex(i); break;}
            }
        }
        break;
        case 2:
        {
            ui->PROPS_NpcLayer->setCurrentIndex(0);
            for(int i=0; i<ui->PROPS_NpcLayer->count();i++)
            {
                if(ui->PROPS_NpcLayer->itemText(i)==lname)
                {ui->PROPS_NpcLayer->setCurrentIndex(i); break;}
            }
        }
    }
    LvlItemPropsLock=false;
}

void LvlItemProperties::npc_refreshMinHeight()
{
    //Calculate max height value
    int minHeightOfBox=0;
    minHeightOfBox+=ui->PROPS_NpcPos->height()+2;
    minHeightOfBox+=ui->PROPS_NpcID->height()+2;
    minHeightOfBox+=ui->PROPS_NpcDir->height()+2;
    minHeightOfBox+=ui->PROPS_NpcFri->height()+2;
    minHeightOfBox+=ui->PROPS_NPCNoMove->height()+2;
    minHeightOfBox+=ui->PROPS_NpcTMsgLabel->height()+2;
    minHeightOfBox+=ui->PROPS_NpcTMsg->height()+2;
    if(ui->line_6->isVisible())
        minHeightOfBox+=ui->line_6->height()+2;
    if(ui->PROPS_NpcSpinLabel->isVisible())
        minHeightOfBox+=ui->PROPS_NpcSpinLabel->height()+2;
    if(ui->PROPS_NPCSpecialSpin->isVisible())
        minHeightOfBox+=ui->PROPS_NPCSpecialSpin->height()+2;
    if(ui->PROPS_NPCSpecialSpin_Auto->isVisible())
        minHeightOfBox+=ui->PROPS_NPCSpecialSpin_Auto->height()+2;
    if(ui->PROPS_NpcContainsLabel->isVisible())
        minHeightOfBox+=ui->PROPS_NpcContainsLabel->height()+2;
    if(ui->PROPS_NPCContaiter->isVisible())
        minHeightOfBox+=ui->PROPS_NPCContaiter->height()+2;
    if(ui->PROPS_NPCBoxLabel->isVisible())
        minHeightOfBox+=ui->PROPS_NPCBoxLabel->height()+2;
    if(ui->PROPS_NPCSpecialBox->isVisible())
        minHeightOfBox+=ui->PROPS_NPCSpecialBox->height()+2;
    if(ui->Line_Special2_sep->isVisible())
        minHeightOfBox+=ui->Line_Special2_sep->height()+2;
    if(ui->PROPS_NpcSpecial2title->isVisible())
        minHeightOfBox+=ui->PROPS_NpcSpecial2title->height()+2;
    if(ui->PROPS_NPCSpecial2Spin->isVisible())
        minHeightOfBox+=ui->PROPS_NPCSpecial2Spin->height()+2;
    if(ui->PROPS_NPCSpecial2Box->isVisible())
        minHeightOfBox+=ui->PROPS_NPCSpecial2Box->height()+2;

    minHeightOfBox+=ui->line_5->height()+2;
    minHeightOfBox+=ui->PROPS_NpcGenerator->height()+2;

    if(ui->PROPS_NPCGenBox->isVisible())
        minHeightOfBox+=ui->PROPS_NPCGenBox->height()+2;
    if(ui->line_2->isVisible())
        minHeightOfBox+=ui->line_2->height()+2;
    minHeightOfBox+=ui->PROPS_NpcLayer->height()+2;
    minHeightOfBox+=ui->line_3->height()+2;
    minHeightOfBox+=ui->PROPS_NpcEventActivate->height()+2;
    minHeightOfBox+=ui->PROPS_NpcEventDeath->height()+2;
    minHeightOfBox+=ui->PROPS_NpcEventTalk->height()+2;
    minHeightOfBox+=ui->PROPS_NpcEventEmptyLayer->height()+2;
    ui->npcProps->setMinimumHeight(minHeightOfBox);
}

void LvlItemProperties::refreshSecondSpecialOption(long npcID, long spcOpts, long spcOpts2, bool newItem)
{
    bool found=false;
    int j;

    //Check Index exists
    if(npcID < mw()->configs.index_npc.size())
    {
        j = mw()->configs.index_npc[npcID].i;

        if(j<mw()->configs.main_npc.size())
        {
        if(mw()->configs.main_npc[j].id == (unsigned int)npcID)
            found=true;
        }
    }
    //if Index found
    if(!found)
    {
        for(j=0;j<mw()->configs.main_npc.size();j++)
        {
            if(mw()->configs.main_npc[j].id==(unsigned int)npcID)
                break;
        }
    }

    if(j >= mw()->configs.main_npc.size())
    {
        j=0;
    }

    ui->PROPS_NpcSpecial2title->hide();
    ui->PROPS_NPCSpecial2Spin->hide();
    ui->PROPS_NPCSpecial2Box->hide();
    ui->Line_Special2_sep->hide();

    if((mw()->configs.main_npc[j].special_option_2)&&
            ((mw()->configs.main_npc[j].special_2_npc_spin_required.isEmpty())||
             (mw()->configs.main_npc[j].special_2_npc_box_required.isEmpty())||
             (mw()->configs.main_npc[j].special_2_npc_spin_required.contains(spcOpts))||
             (mw()->configs.main_npc[j].special_2_npc_box_required.contains(spcOpts)))
            )
    {
        if(
                ((mw()->configs.main_npc[j].special_2_npc_box_required.isEmpty())&&
                 (mw()->configs.main_npc[j].special_2_type==0))
                ||
                (mw()->configs.main_npc[j].special_2_npc_box_required.contains(spcOpts))
          )
        {
            ui->Line_Special2_sep->show();
            ui->PROPS_NpcSpecial2title->show();
            ui->PROPS_NpcSpecial2title->setText(mw()->configs.main_npc[j].special_2_name);

            ui->PROPS_NPCSpecial2Box->show();
            if(newItem)
            {//Reset value to min, if it out of range
                if((spcOpts2>=mw()->configs.main_npc[j].special_2_combobox_opts.size())||
                    (spcOpts2<0))
                {
                   LvlPlacingItems::npcSet.special_data2=0;
                   spcOpts2=0;
                }
            }

            ui->PROPS_NPCSpecial2Box->clear();
            for(int i=0; i < mw()->configs.main_npc[j].special_2_combobox_opts.size(); i++)
            {
                ui->PROPS_NPCSpecial2Box->addItem( mw()->configs.main_npc[j].special_2_combobox_opts[i] );
                if(i==spcOpts2) ui->PROPS_NPCSpecial2Box->setCurrentIndex(i);
            }
        }
        else
        if(
                ((mw()->configs.main_npc[j].special_2_npc_spin_required.isEmpty())
                 &&(mw()->configs.main_npc[j].special_2_type==1))||
                (mw()->configs.main_npc[j].special_2_npc_spin_required.contains(spcOpts))
          )
        {
            ui->Line_Special2_sep->show();
            ui->PROPS_NpcSpecial2title->show();
            ui->PROPS_NpcSpecial2title->setText(mw()->configs.main_npc[j].special_2_name);

            ui->PROPS_NPCSpecial2Spin->show();
            if(newItem)
            { //Reset value to min, if it out of range
                if((spcOpts2>mw()->configs.main_npc[j].special_2_spin_max)||
                   (spcOpts2<mw()->configs.main_npc[j].special_2_spin_max))
                {
                   LvlPlacingItems::npcSet.special_data2 = mw()->configs.main_npc[j].special_2_spin_min;
                   spcOpts2 = mw()->configs.main_npc[j].special_2_spin_min;
                }
            }

            npcSpecSpinOffset_2 = mw()->configs.main_npc[j].special_2_spin_value_offset;

            ui->PROPS_NPCSpecial2Spin->setMinimum( mw()->configs.main_npc[j].special_2_spin_min + npcSpecSpinOffset_2 );
            ui->PROPS_NPCSpecial2Spin->setMaximum( mw()->configs.main_npc[j].special_2_spin_max + npcSpecSpinOffset_2 );

            ui->PROPS_NPCSpecial2Spin->setValue( spcOpts2 + npcSpecSpinOffset_2 );
        }
    }
    npc_refreshMinHeight();
}


////////////////////////////////////////////////////////////////////////////////////////////

// ///////////BLOCKS///////////////////////////

void LvlItemProperties::on_PROPS_BlockResize_clicked()
{
    if(LvlItemPropsLock) return;
    if(LockItemProps) return;

    if(blockPtr<0) return;

    if (mw()->activeChildWindow()==1)
    {
        QList<QGraphicsItem *> items = mw()->activeLvlEditWin()->scene->selectedItems();
        foreach(QGraphicsItem * item, items)
        {
            if((item->data(ITEM_TYPE).toString()=="Block")&&((item->data(ITEM_ARRAY_ID).toInt()==blockPtr)))
            {
                mw()->activeLvlEditWin()->scene->setBlockResizer(item, true);
                break;
            }
        }

    }

}






//void MainWindow::on_PROPS_BlockSquareFill_clicked(bool checked)
//{
//    if(LvlItemPropsLock) return;
//    if(LockItemProps) return;

//    on_actionSquareFill_triggered(checked);
//    ui->actionSquareFill->setChecked(checked);
//}


//void MainWindow::on_PROPS_BGOSquareFill_clicked(bool checked)
//{
//    if(LvlItemPropsLock) return;
//    if(LockItemProps) return;

//    on_actionSquareFill_triggered(checked);
//    ui->actionSquareFill->setChecked(checked);
//}


void LvlItemProperties::on_PROPS_BlockInvis_clicked(bool checked)
{
    if(LvlItemPropsLock) return;
    if(LockItemProps) return;

    if(blockPtr<0)
    {
        LvlPlacingItems::blockSet.invisible = checked;
    }
    else
    if (mw()->activeChildWindow()==1)
    {
        LevelData selData;
        QList<QGraphicsItem *> items = mw()->activeLvlEditWin()->scene->selectedItems();
        foreach(QGraphicsItem * item, items)
        {
            if(item->data(ITEM_TYPE).toString()=="Block")
            {
                selData.blocks.push_back(((ItemBlock *) item)->blockData);
                ((ItemBlock*)item)->setInvisible(checked);
            }
        }
        mw()->activeLvlEditWin()->scene->addChangeSettingsHistory(selData, HistorySettings::SETTING_INVISIBLE, QVariant(checked));
    }

}


void LvlItemProperties::on_PROPS_BlkSlippery_clicked(bool checked)
{
    if(LvlItemPropsLock) return;
    if(LockItemProps) return;

    if(blockPtr<0)
    {
        LvlPlacingItems::blockSet.slippery = checked;
    }
    else
    if (mw()->activeChildWindow()==1)
    {
        LevelData selData;
        QList<QGraphicsItem *> items = mw()->activeLvlEditWin()->scene->selectedItems();
        foreach(QGraphicsItem * item, items)
        {
            if((item->data(ITEM_TYPE).toString()=="Block")/*&&((item->data(2).toInt()==blockPtr))*/)
            {
                selData.blocks.push_back(((ItemBlock *) item)->blockData);
                ((ItemBlock*)item)->setSlippery(checked);
            }
        }
        mw()->activeLvlEditWin()->scene->addChangeSettingsHistory(selData, HistorySettings::SETTING_SLIPPERY, QVariant(checked));
    }

}

void LvlItemProperties::on_PROPS_BlockIncludes_clicked()
{
    if(LvlItemPropsLock) return;
    if(LockItemProps) return;

    int npcID=0;

    if(blockPtr<0)
    {
        npcID = LvlPlacingItems::blockSet.npc_id;
    }
    else
    if (mw()->activeChildWindow()==1)
    {
        QList<QGraphicsItem *> items1 = mw()->activeLvlEditWin()->scene->selectedItems();
        foreach(QGraphicsItem * targetItem, items1)
        {
            if((targetItem->data(ITEM_TYPE).toString()=="Block")&&((targetItem->data(ITEM_ARRAY_ID).toInt()==blockPtr)))
            {
                npcID = ((ItemBlock*)targetItem)->blockData.npc_id;
                break;
            }
        }
    }

    LevelData selData;

    //NpcDialog * npcList = new NpcDialog(&configs);
    ItemSelectDialog * npcList = new ItemSelectDialog(&mw()->configs, ItemSelectDialog::TAB_NPC,
                                                   ItemSelectDialog::NPCEXTRA_WITHCOINS | (npcID < 0 && npcID != 0 ? ItemSelectDialog::NPCEXTRA_ISCOINSELECTED : 0),0,0,
                                                   (npcID < 0 && npcID != 0 ? npcID*-1 : npcID));
    npcList->setWindowFlags (Qt::Window | Qt::WindowTitleHint | Qt::WindowCloseButtonHint);
    npcList->setGeometry(QStyle::alignedRect(Qt::LeftToRight, Qt::AlignCenter, npcList->size(), qApp->desktop()->availableGeometry()));
    //npcList->setState(npcID);

    if(npcList->exec()==QDialog::Accepted)
    {
        //apply to all selected items.
        int selected_npc=0;
//        if(npcList->isEmpty)
//            selected_npc = 0;
//        else
//        if(npcList->isCoin)
//            selected_npc = npcList->coins;
//        else
//            selected_npc = npcList->selectedNPC+1000;
        if(npcList->npcID!=0){
                    if(npcList->isCoin)
                        selected_npc = npcList->npcID*-1;
                    else
                        selected_npc = npcList->npcID;
                }


        ui->PROPS_BlockIncludes->setText(
                    ((selected_npc!=0)?
                         ((selected_npc>0)?QString("NPC-%1").arg(selected_npc):tr("%1 coins").arg(selected_npc*-1))
                       :tr("[empty]")
                         ) );

        if(blockPtr<1)
        {
            LvlPlacingItems::blockSet.npc_id = selected_npc;
        }
        else
        if (mw()->activeChildWindow()==1)
        {
            QList<QGraphicsItem *> items = mw()->activeLvlEditWin()->scene->selectedItems();
            foreach(QGraphicsItem * item, items)
            {
                if((item->data(ITEM_TYPE).toString()=="Block")/*&&((item->data(2).toInt()==blockPtr))*/)
                {
                    //((ItemBlock *)item)->blockData.npc_id = selected_npc;
                    //((ItemBlock *)item)->arrayApply();
                    selData.blocks.push_back(((ItemBlock *) item)->blockData);
                    ((ItemBlock *)item)->setIncludedNPC(selected_npc);
                    //break;
                }
            }

            mw()->activeLvlEditWin()->scene->addChangeSettingsHistory(selData, HistorySettings::SETTING_CHANGENPC, QVariant(selected_npc));
        }

    }
    delete npcList;

}


void LvlItemProperties::on_PROPS_BlockLayer_currentIndexChanged(const QString &arg1)
{
    if(LvlItemPropsLock) return;
    if(LockItemProps) return;

    if(blockPtr<0)
    {
        LvlPlacingItems::blockSet.layer = arg1;
        LvlPlacingItems::layer = arg1;
    }
    else
    if (mw()->activeChildWindow()==1)
    {
        LevelData modData;
        QList<QGraphicsItem *> items = mw()->activeLvlEditWin()->scene->selectedItems();
        foreach(QGraphicsItem * item, items)
        {
            if(item->data(ITEM_TYPE).toString()=="Block")
            {
                modData.blocks.push_back(((ItemBlock*)item)->blockData);
                ((ItemBlock*)item)->setLayer(arg1);
                //break;
            }
        }
        mw()->activeLvlEditWin()->scene->addChangedLayerHistory(modData, arg1);
    }

}


void LvlItemProperties::on_PROPS_BlkEventDestroy_currentIndexChanged(const QString &arg1)
{
    if(LvlItemPropsLock) return;
    if(LockItemProps) return;


    if(blockPtr<0)
    {
        if(ui->PROPS_BlkEventDestroy->currentIndex()>0)
            LvlPlacingItems::blockSet.event_destroy = arg1;
        else
            LvlPlacingItems::blockSet.event_destroy = "";

        BlockEventDestroy=LvlPlacingItems::blockSet.event_destroy;
    }
    else
    if (mw()->activeChildWindow()==1)
    {
        LevelData modData;
        QList<QGraphicsItem *> items = mw()->activeLvlEditWin()->scene->selectedItems();
        foreach(QGraphicsItem * item, items)
        {
            if((item->data(ITEM_TYPE).toString()=="Block")/*&&((item->data(2).toInt()==blockPtr))*/)
            {
                modData.blocks.push_back(((ItemBlock*)item)->blockData);
                if(ui->PROPS_BlkEventDestroy->currentIndex()>0)
                    ((ItemBlock*)item)->blockData.event_destroy = arg1;
                else
                    ((ItemBlock*)item)->blockData.event_destroy = "";
                ((ItemBlock*)item)->arrayApply();
                //break;
            }
        }
        if(ui->PROPS_BlkEventDestroy->currentIndex()>0){
            mw()->activeLvlEditWin()->scene->addChangeSettingsHistory(modData, HistorySettings::SETTING_EV_DESTROYED, QVariant(arg1));
        }else{
            mw()->activeLvlEditWin()->scene->addChangeSettingsHistory(modData, HistorySettings::SETTING_EV_DESTROYED, QVariant(""));
        }
    }

}
void LvlItemProperties::on_PROPS_BlkEventHited_currentIndexChanged(const QString &arg1)
{
    if(LvlItemPropsLock) return;
    if(LockItemProps) return;


    if(blockPtr<0)
    {
        if(ui->PROPS_BlkEventHited->currentIndex()>0)
            LvlPlacingItems::blockSet.event_hit = arg1;
        else
            LvlPlacingItems::blockSet.event_hit = "";

        BlockEventHit = LvlPlacingItems::blockSet.event_hit;
    }
    else
    if (mw()->activeChildWindow()==1)
    {
        LevelData modData;
        QList<QGraphicsItem *> items = mw()->activeLvlEditWin()->scene->selectedItems();
        foreach(QGraphicsItem * item, items)
        {
            if((item->data(ITEM_TYPE).toString()=="Block")/*&&((item->data(2).toInt()==blockPtr))*/)
            {
                modData.blocks.push_back(((ItemBlock*)item)->blockData);
                if(ui->PROPS_BlkEventHited->currentIndex()>0)
                    ((ItemBlock*)item)->blockData.event_hit = arg1;
                else
                    ((ItemBlock*)item)->blockData.event_hit = "";
                ((ItemBlock*)item)->arrayApply();
                //break;
            }
        }
        if(ui->PROPS_BlkEventHited->currentIndex()>0){
            mw()->activeLvlEditWin()->scene->addChangeSettingsHistory(modData, HistorySettings::SETTING_EV_HITED, QVariant(arg1));
        }else{
            mw()->activeLvlEditWin()->scene->addChangeSettingsHistory(modData, HistorySettings::SETTING_EV_HITED, QVariant(""));
        }
    }

}
void LvlItemProperties::on_PROPS_BlkEventLayerEmpty_currentIndexChanged(const QString &arg1)
{
    if(LvlItemPropsLock) return;
    if(LockItemProps) return;


    if(blockPtr<0)
    {
        if(ui->PROPS_BlkEventLayerEmpty->currentIndex()>0)
            LvlPlacingItems::blockSet.event_no_more = arg1;
        else
            LvlPlacingItems::blockSet.event_no_more = "";
        BlockEventLayerEmpty = LvlPlacingItems::blockSet.event_no_more;
    }

    else
    if (mw()->activeChildWindow()==1)
    {
        LevelData modData;
        QList<QGraphicsItem *> items = mw()->activeLvlEditWin()->scene->selectedItems();
        foreach(QGraphicsItem * item, items)
        {
            if((item->data(ITEM_TYPE).toString()=="Block")/*&&((item->data(2).toInt()==blockPtr))*/)
            {
                modData.blocks.push_back(((ItemBlock*)item)->blockData);
                if(ui->PROPS_BlkEventLayerEmpty->currentIndex()>0)
                    ((ItemBlock*)item)->blockData.event_no_more = arg1;
                else
                    ((ItemBlock*)item)->blockData.event_no_more = "";
                ((ItemBlock*)item)->arrayApply();
                //break;
            }
        }
        if(ui->PROPS_BlkEventLayerEmpty->currentIndex()>0){
            mw()->activeLvlEditWin()->scene->addChangeSettingsHistory(modData, HistorySettings::SETTING_EV_LAYER_EMP, QVariant(arg1));
        }else{
            mw()->activeLvlEditWin()->scene->addChangeSettingsHistory(modData, HistorySettings::SETTING_EV_LAYER_EMP, QVariant(""));
        }
    }

}







////////////////////////////////////////////////////////////////////////////////////////////

// ///////////BGO///////////////////////////

void LvlItemProperties::on_PROPS_BGOLayer_currentIndexChanged(const QString &arg1)
{
    if(LvlItemPropsLock) return;
    if(LockItemProps) return;

    if(bgoPtr<0)
    {
        LvlPlacingItems::bgoSet.layer = arg1;
        LvlPlacingItems::layer = arg1;
    }
    else
    if (mw()->activeChildWindow()==1)
    {
        LevelData modData;
        QList<QGraphicsItem *> items = mw()->activeLvlEditWin()->scene->selectedItems();
        foreach(QGraphicsItem * item, items)
        {
            if((item->data(ITEM_TYPE).toString()=="BGO")/*&&((item->data(2).toInt()==bgoPtr))*/)
            {
                modData.bgo.push_back(((ItemBGO*)item)->bgoData);
                ((ItemBGO*)item)->setLayer(arg1);
                //break;
            }
        }
        mw()->activeLvlEditWin()->scene->addChangedLayerHistory(modData, arg1);
    }

}



void LvlItemProperties::on_PROPS_BGO_Z_Layer_currentIndexChanged(int index)
{
    if(LvlItemPropsLock) return;
    if(LockItemProps) return;

    int zMode=0;
    switch(index)
    {
    case 0:
        zMode = LevelBGO::Background2; break;
    case 1:
        zMode = LevelBGO::Background1; break;
    case 3:
        zMode = LevelBGO::Foreground1; break;
    case 4:
        zMode = LevelBGO::Foreground2; break;
    case 2:
    default:
        zMode = LevelBGO::ZDefault; break;
    }

    if(bgoPtr<0)
    {
        LvlPlacingItems::bgoSet.z_mode = zMode;
    }
    else
    if (mw()->activeChildWindow()==1)
    {
        LevelData selData;
        QList<QGraphicsItem *> items = mw()->activeLvlEditWin()->scene->selectedItems();
        foreach(QGraphicsItem * item, items)
        {
            if((item->data(ITEM_TYPE).toString()=="BGO")/*&&((item->data(2).toInt()==bgoPtr))*/)
            {
                selData.bgo.push_back(((ItemBGO*)item)->bgoData);
                ((ItemBGO*)item)->setZMode(zMode, ((ItemBGO*)item)->bgoData.z_offset);
                //((ItemBGO*)item)->arrayApply();
                //break;
            }
        }
        mw()->activeLvlEditWin()->scene->addChangeSettingsHistory(selData, HistorySettings::SETTING_Z_LAYER, QVariant(zMode));
    }


}

void LvlItemProperties::on_PROPS_BGO_Z_Offset_valueChanged(double arg1)
{
    if(LvlItemPropsLock) return;
    if(LockItemProps) return;

    if(bgoPtr<0)
    {
        LvlPlacingItems::bgoSet.z_offset = arg1;
    }
    else
    if (mw()->activeChildWindow()==1)
    {
        LevelData selData;
        QList<QGraphicsItem *> items = mw()->activeLvlEditWin()->scene->selectedItems();
        foreach(QGraphicsItem * item, items)
        {
            if((item->data(ITEM_TYPE).toString()=="BGO")/*&&((item->data(2).toInt()==bgoPtr))*/)
            {
                selData.bgo.push_back(((ItemBGO*)item)->bgoData);
                ((ItemBGO*)item)->setZMode( ((ItemBGO*)item)->bgoData.z_mode, arg1);
                //((ItemBGO*)item)->arrayApply();
                //break;
            }
        }
        mw()->activeLvlEditWin()->scene->addChangeSettingsHistory(selData, HistorySettings::SETTING_Z_OFFSET, QVariant(arg1));
    }
}


void LvlItemProperties::on_PROPS_BGO_smbx64_sp_valueChanged(int arg1)
{
    if(LvlItemPropsLock) return;
    if(LockItemProps) return;

    if(bgoPtr<0)
    {
        LvlPlacingItems::bgoSet.smbx64_sp = arg1;
    }
    else
    if (mw()->activeChildWindow()==1)
    {
        LevelData selData;
        QList<QGraphicsItem *> items = mw()->activeLvlEditWin()->scene->selectedItems();
        foreach(QGraphicsItem * item, items)
        {
            if((item->data(ITEM_TYPE).toString()=="BGO")/*&&((item->data(2).toInt()==bgoPtr))*/)
            {
                selData.bgo.push_back(((ItemBGO*)item)->bgoData);
                ((ItemBGO*)item)->bgoData.smbx64_sp = arg1;
                ((ItemBGO*)item)->arrayApply();
                //break;
            }
        }
        mw()->activeLvlEditWin()->scene->addChangeSettingsHistory(selData, HistorySettings::SETTING_BGOSORTING, QVariant(arg1));
    }
}


////////////////////////////////////////////////////////////////////////////////////////////

// ///////////NPC///////////////////////////


void LvlItemProperties::on_PROPS_NPCDirLeft_clicked()
{
    if(LvlItemPropsLock) return;
    if(LockItemProps) return;

    if(npcPtr<0)
    {
        LvlPlacingItems::npcSet.direct = -1;

        if (mw()->activeChildWindow()==1)
        {
            mw()->activeLvlEditWin()->scene->updateCursoredNpcDirection();
        }
    }
    else
    if (mw()->activeChildWindow()==1)
    {
        LevelData selData;
        QList<QGraphicsItem *> items = mw()->activeLvlEditWin()->scene->selectedItems();
        foreach(QGraphicsItem * item, items)
        {
            if((item->data(ITEM_TYPE).toString()=="NPC")/*&&((item->data(2).toInt()==npcPtr))*/)
            {
                selData.npc.push_back(((ItemNPC*)item)->npcData);
                ((ItemNPC*)item)->changeDirection(-1);
                //break;
            }
        }
        mw()->activeLvlEditWin()->scene->addChangeSettingsHistory(selData, HistorySettings::SETTING_DIRECTION, QVariant(-1));
    }

}


void LvlItemProperties::on_PROPS_NPCDirRand_clicked()
{
    if(LvlItemPropsLock) return;
    if(LockItemProps) return;

    if(npcPtr<0)
    {
        LvlPlacingItems::npcSet.direct = 0;

        if (mw()->activeChildWindow()==1)
        {
            mw()->activeLvlEditWin()->scene->updateCursoredNpcDirection();
        }
    }
    else
    if (mw()->activeChildWindow()==1)
    {
        LevelData selData;
        QList<QGraphicsItem *> items = mw()->activeLvlEditWin()->scene->selectedItems();
        foreach(QGraphicsItem * item, items)
        {
            if((item->data(ITEM_TYPE).toString()=="NPC")/*&&((item->data(2).toInt()==npcPtr))*/)
            {
                selData.npc.push_back(((ItemNPC*)item)->npcData);
                ((ItemNPC*)item)->changeDirection(0);

                //break;
            }
        }
        mw()->activeLvlEditWin()->scene->addChangeSettingsHistory(selData, HistorySettings::SETTING_DIRECTION, QVariant(0));
    }
}

void LvlItemProperties::on_PROPS_NPCDirRight_clicked()
{
    if(LvlItemPropsLock) return;
    if(LockItemProps) return;

    if(npcPtr<0)
    {
        LvlPlacingItems::npcSet.direct = 1;

        if (mw()->activeChildWindow()==1)
        {
            mw()->activeLvlEditWin()->scene->updateCursoredNpcDirection();
        }

    }
    else
    if (mw()->activeChildWindow()==1)
    {
        LevelData selData;
        QList<QGraphicsItem *> items = mw()->activeLvlEditWin()->scene->selectedItems();
        foreach(QGraphicsItem * item, items)
        {
            if((item->data(ITEM_TYPE).toString()=="NPC")/*&&((item->data(2).toInt()==npcPtr))*/)
            {
                selData.npc.push_back(((ItemNPC*)item)->npcData);
                ((ItemNPC*)item)->changeDirection(1);
                //break;
            }
        }
        mw()->activeLvlEditWin()->scene->addChangeSettingsHistory(selData, HistorySettings::SETTING_DIRECTION, QVariant(1));
    }
}




void LvlItemProperties::on_PROPS_NpcFri_clicked(bool checked)
{
    if(LvlItemPropsLock) return;
    if(LockItemProps) return;

    if(npcPtr<0)
    {
        LvlPlacingItems::npcSet.friendly = checked;
    }
    else
    if (mw()->activeChildWindow()==1)
    {
        LevelData selData;
        QList<QGraphicsItem *> items = mw()->activeLvlEditWin()->scene->selectedItems();
        foreach(QGraphicsItem * item, items)
        {
            if((item->data(ITEM_TYPE).toString()=="NPC")/*&&((item->data(2).toInt()==npcPtr))*/)
            {
                selData.npc.push_back(((ItemNPC*)item)->npcData);
                ((ItemNPC*)item)->setFriendly(checked);
                //break;
            }
        }
        mw()->activeLvlEditWin()->scene->addChangeSettingsHistory(selData, HistorySettings::SETTING_FRIENDLY, QVariant(checked));
    }

}

void LvlItemProperties::on_PROPS_NPCNoMove_clicked(bool checked)
{
    if(LvlItemPropsLock) return;
    if(LockItemProps) return;

    if(npcPtr<0)
    {
        LvlPlacingItems::npcSet.nomove = checked;
    }
    else
    if (mw()->activeChildWindow()==1)
    {
        LevelData selData;
        QList<QGraphicsItem *> items = mw()->activeLvlEditWin()->scene->selectedItems();
        foreach(QGraphicsItem * item, items)
        {
            if((item->data(ITEM_TYPE).toString()=="NPC")/*&&((item->data(2).toInt()==npcPtr))*/)
            {
                selData.npc.push_back(((ItemNPC*)item)->npcData);
                ((ItemNPC*)item)->setNoMovable(checked);
                //break;
            }
        }
        mw()->activeLvlEditWin()->scene->addChangeSettingsHistory(selData, HistorySettings::SETTING_NOMOVEABLE, QVariant(checked));
    }

}
void LvlItemProperties::on_PROPS_NpcBoss_clicked(bool checked)
{
    if(LvlItemPropsLock) return;
    if(LockItemProps) return;

    if(npcPtr<0)
    {
        LvlPlacingItems::npcSet.legacyboss = checked;
    }
    else
    if (mw()->activeChildWindow()==1)
    {
        LevelData selData;
        QList<QGraphicsItem *> items = mw()->activeLvlEditWin()->scene->selectedItems();
        foreach(QGraphicsItem * item, items)
        {
            if((item->data(ITEM_TYPE).toString()=="NPC")/*&&((item->data(2).toInt()==npcPtr))*/)
            {
                selData.npc.push_back(((ItemNPC*)item)->npcData);
                ((ItemNPC*)item)->setLegacyBoss(checked);
                //break;
            }
        }
        mw()->activeLvlEditWin()->scene->addChangeSettingsHistory(selData, HistorySettings::SETTING_BOSS, QVariant(checked));
    }
}

void LvlItemProperties::on_PROPS_NpcTMsg_clicked()
{
    if(LvlItemPropsLock) return;
    if(LockItemProps) return;

    //LevelData selData;
    //QList<QVariant> modText;
    //modText.push_back(QVariant(npcData.msg));

    QString message;
    if(npcPtr<0)
    {
        message = LvlPlacingItems::npcSet.msg;
    }
    else
    if (mw()->activeChildWindow()==1)
    {
        QList<QGraphicsItem *> items = mw()->activeLvlEditWin()->scene->selectedItems();
        foreach(QGraphicsItem * SelItem, items )
        {
            if(SelItem->data(ITEM_TYPE).toString()=="NPC")
            {
                message = ((ItemNPC *) SelItem)->npcData.msg; break;
            }
        }
    }

    ItemMsgBox * msgBox = new ItemMsgBox(message);
    util::DialogToCenter(msgBox, true);

    if(msgBox->exec()==QDialog::Accepted)
    {

        if(npcPtr<1)
        {
            LvlPlacingItems::npcSet.msg = msgBox->currentText;
        }
        else
        if (mw()->activeChildWindow()==1)
        {
            LevelData selData;
            QList<QGraphicsItem *> items = mw()->activeLvlEditWin()->scene->selectedItems();
            foreach(QGraphicsItem * SelItem, items )
            {
                if(SelItem->data(ITEM_TYPE).toString()=="NPC"){
                    selData.npc.push_back(((ItemNPC *) SelItem)->npcData);
                    ((ItemNPC *) SelItem)->setMsg( msgBox->currentText );
                }
            }
            mw()->activeLvlEditWin()->scene->addChangeSettingsHistory(selData, HistorySettings::SETTING_MESSAGE, QVariant(msgBox->currentText));
        }

        QString npcmsg = (msgBox->currentText.isEmpty() ? tr("[none]") : msgBox->currentText);
        if(npcmsg.size()>20)
        {
            npcmsg.resize(18);
            npcmsg.push_back("...");
        }
        ui->PROPS_NpcTMsg->setText( npcmsg.replace("&", "&&&") );
    }
    delete msgBox;

}

void LvlItemProperties::on_PROPS_NPCSpecialSpin_valueChanged(int arg1)
{
    if(LvlItemPropsLock) return;
    if(LockItemProps) return;

    LvlPlacingItems::npcSpecialAutoIncrement_begin = arg1 - npcSpecSpinOffset;

    if(npcPtr<0)
    {
        LvlPlacingItems::npcSet.special_data = arg1 - npcSpecSpinOffset;
    }
    else
    if (mw()->activeChildWindow()==1)
    {
        LevelData selData;
        QList<QGraphicsItem *> items = mw()->activeLvlEditWin()->scene->selectedItems();
        foreach(QGraphicsItem * item, items)
        {
            if(item->data(ITEM_TYPE).toString()=="NPC")
            {
                LevelNPC npc = ((ItemNPC*)item)->npcData;

                bool found=false;
                int j;

                //Check Index exists
                if(npc.id < (unsigned int)mw()->configs.index_npc.size())
                {
                    j = mw()->configs.index_npc[npc.id].i;

                    if(j<mw()->configs.main_npc.size())
                    {
                    if(mw()->configs.main_npc[j].id == npc.id)
                        found=true;
                    }
                }
                //if Index found
                if(!found)
                {
                    for(j=0;j<mw()->configs.main_npc.size();j++)
                    {
                        if(mw()->configs.main_npc[j].id==npc.id)
                            break;
                    }
                }

                if(j >= mw()->configs.main_npc.size())
                {
                    j=0;
                }

                if(mw()->configs.main_npc[j].special_type != 1) //wrong type, go to next one
                    continue;

                if(mw()->configs.main_npc[j].special_spin_value_offset != npcSpecSpinOffset) //wrong offset, go to next one
                    continue;

                selData.npc.push_back(((ItemNPC*)item)->npcData);
                ((ItemNPC*)item)->npcData.special_data = arg1 - npcSpecSpinOffset;
                ((ItemNPC*)item)->arrayApply();
            }
        }
        mw()->activeLvlEditWin()->scene->addChangeSettingsHistory(selData, HistorySettings::SETTING_SPECIAL_DATA, QVariant(arg1 - npcSpecSpinOffset));
    }

}

void LvlItemProperties::on_PROPS_NPCSpecialSpin_Auto_clicked(bool checked)
{
    LvlPlacingItems::npcSpecialAutoIncrement=checked;
    LvlPlacingItems::npcSet.special_data = ui->PROPS_NPCSpecialSpin->value() - npcSpecSpinOffset;

    switch(LvlPlacingItems::npcSet.direct)
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
}

void LvlItemProperties::on_PROPS_NPCSpecialSpin_Auto_toggled(bool checked)
{
    LvlPlacingItems::npcSpecialAutoIncrement=checked;
    LvlPlacingItems::npcSet.special_data = ui->PROPS_NPCSpecialSpin->value() - npcSpecSpinOffset;

    switch(LvlPlacingItems::npcSet.direct)
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
}

void LvlItemProperties::on_PROPS_NPCContaiter_clicked()
{
    if(LvlItemPropsLock) return;
    if(LockItemProps) return;

    int npcID=0;
    int contID=0;
    int spcData2=0;

    if(npcPtr<0)
    {
        npcID = LvlPlacingItems::npcSet.special_data;
        contID= LvlPlacingItems::npcSet.id;
        spcData2= LvlPlacingItems::npcSet.special_data2;
    }
    else
    if (mw()->activeChildWindow()==1)
    {
        QList<QGraphicsItem *> items1 = mw()->activeLvlEditWin()->scene->selectedItems();
        foreach(QGraphicsItem * targetItem, items1)
        {
            if((targetItem->data(ITEM_TYPE).toString()=="NPC")&&((targetItem->data(2).toInt()==npcPtr)))
            {
                contID = ((ItemNPC*)targetItem)->npcData.id;
                npcID = ((ItemNPC*)targetItem)->npcData.special_data;
                spcData2 = ((ItemNPC*)targetItem)->npcData.special_data2;
                break;
            }
        }
    }

    //LevelData selData;
    //QList<QVariant> modNPC;

    ItemSelectDialog* npcList = new ItemSelectDialog(&mw()->configs, ItemSelectDialog::TAB_NPC, 0, 0, 0, npcID);
    util::DialogToCenter(npcList, true);

    if(npcList->exec()==QDialog::Accepted)
    {
        //apply to all selected items.
        //  int selected_npc=0;
        // if(npcList->isEmpty)
        //     selected_npc = 0;
        //   else
        //   selected_npc = npcList->selectedNPC;

        int selected_npc=npcList->npcID;

        ui->PROPS_NPCContaiter->setText(
                    ((selected_npc>0)?QString("NPC-%1").arg(selected_npc)
                       :tr("[empty]")
                         ) );

        if(npcPtr<1)
        {
            LvlPlacingItems::npcSet.special_data = selected_npc;
            LockItemProps=true;
            refreshSecondSpecialOption(contID, selected_npc, spcData2);
            LockItemProps=false;
        }
        else
        if (mw()->activeChildWindow()==1)
        {
            LevelData selData;
            QList<QGraphicsItem *> items = mw()->activeLvlEditWin()->scene->selectedItems();
            foreach(QGraphicsItem * item, items)
            {
                if((item->data(ITEM_TYPE).toString()=="NPC")/*&&((item->data(2).toInt()==blockPtr))*/)
                {
                    LevelNPC npc = ((ItemNPC*)item)->npcData;

                    bool found=false;
                    int j;

                    //Check Index exists
                    if(npc.id < (unsigned int)mw()->configs.index_npc.size())
                    {
                        j = mw()->configs.index_npc[npc.id].i;

                        if(j<mw()->configs.main_npc.size())
                        {
                        if(mw()->configs.main_npc[j].id == npc.id)
                            found=true;
                        }
                    }
                    //if Index found
                    if(!found)
                    {
                        for(j=0;j<mw()->configs.main_npc.size();j++)
                        {
                            if(mw()->configs.main_npc[j].id==npc.id)
                                break;
                        }
                    }

                    if(j >= mw()->configs.main_npc.size())
                    {
                        j=0;
                    }

                    if(mw()->configs.main_npc[j].special_type != 2) //wrong type, go to next one
                        continue;

                    selData.npc.push_back(((ItemNPC *)item)->npcData);
                    ((ItemNPC *)item)->setIncludedNPC(selected_npc);
                }
            }
            mw()->activeLvlEditWin()->scene->addChangeSettingsHistory(selData, HistorySettings::SETTING_CHANGENPC, QVariant(selected_npc));
            LockItemProps=true;
            refreshSecondSpecialOption(contID, selected_npc, spcData2);
            LockItemProps=false;
        }
    }
    delete npcList;

}

void LvlItemProperties::on_PROPS_NPCSpecialBox_currentIndexChanged(int index)
{
    if(LvlItemPropsLock) return;
    if(LockItemProps) return;

    if(npcPtr<0)
    {
        LvlPlacingItems::npcSet.special_data = index;
    }
    else
    if (mw()->activeChildWindow()==1)
    {
        LevelData selData;
        QList<QGraphicsItem *> items = mw()->activeLvlEditWin()->scene->selectedItems();
        foreach(QGraphicsItem * item, items)
        {
            if(item->data(ITEM_TYPE).toString()=="NPC")
            {
                LevelNPC npc = ((ItemNPC*)item)->npcData;

                bool found=false;
                int j;

                //Check Index exists
                if(npc.id < (unsigned int)mw()->configs.index_npc.size())
                {
                    j = mw()->configs.index_npc[npc.id].i;

                    if(j<mw()->configs.main_npc.size())
                    {
                    if(mw()->configs.main_npc[j].id == npc.id)
                        found=true;
                    }
                }
                //if Index found
                if(!found)
                {
                    for(j=0;j<mw()->configs.main_npc.size();j++)
                    {
                        if(mw()->configs.main_npc[j].id==npc.id)
                            break;
                    }
                }

                if(j >= mw()->configs.main_npc.size())
                {
                    j=0;
                }

                if(mw()->configs.main_npc[j].special_type != 0) //wrong type, go to next one
                    continue;

                selData.npc.push_back(((ItemNPC*)item)->npcData);
                ((ItemNPC*)item)->npcData.special_data = index;
                ((ItemNPC*)item)->arrayApply();
            }
        }
        mw()->activeLvlEditWin()->scene->addChangeSettingsHistory(selData, HistorySettings::SETTING_SPECIAL_DATA, QVariant(index));
    }
}



void LvlItemProperties::on_PROPS_NPCSpecial2Spin_valueChanged(int arg1)
{
    if(LvlItemPropsLock) return;
    if(LockItemProps) return;

    if(npcPtr<0)
    {
        LvlPlacingItems::npcSet.special_data2 = arg1 - npcSpecSpinOffset_2;
    }
    else
    if (mw()->activeChildWindow()==1)
    {
        LevelData selData;
        QList<QGraphicsItem *> items = mw()->activeLvlEditWin()->scene->selectedItems();
        foreach(QGraphicsItem * item, items)
        {
            if(item->data(ITEM_TYPE).toString()=="NPC")
            {
                LevelNPC npc = ((ItemNPC*)item)->npcData;

                bool found=false;
                int j;

                //Check Index exists
                if(npc.id < (unsigned int)mw()->configs.index_npc.size())
                {
                    j = mw()->configs.index_npc[npc.id].i;

                    if(j<mw()->configs.main_npc.size())
                    {
                    if(mw()->configs.main_npc[j].id == npc.id)
                        found=true;
                    }
                }
                //if Index found
                if(!found)
                {
                    for(j=0;j<mw()->configs.main_npc.size();j++)
                    {
                        if(mw()->configs.main_npc[j].id==npc.id)
                            break;
                    }
                }

                if(j >= mw()->configs.main_npc.size())
                {
                    j=0;
                }

                if(mw()->configs.main_npc[j].special_2_type != 1) //wrong type, go to next one
                    continue;

                if(mw()->configs.main_npc[j].special_2_spin_value_offset != npcSpecSpinOffset_2) //wrong offset, go to next one
                    continue;

                selData.npc.push_back(((ItemNPC*)item)->npcData);
                ((ItemNPC*)item)->npcData.special_data2 = arg1 - npcSpecSpinOffset_2;
                ((ItemNPC*)item)->arrayApply();
            }
        }
        //mw()->activeLvlEditWin()->scene->addChangeSettingsHistory(selData, LvlScene::SETTING_SPECIAL_DATA, QVariant(arg1 - npcSpecSpinOffset_2));
    }


}

void LvlItemProperties::on_PROPS_NPCSpecial2Box_currentIndexChanged(int index)
{
    if(LvlItemPropsLock) return;
    if(LockItemProps) return;

    if(npcPtr<0)
    {
        LvlPlacingItems::npcSet.special_data2 = index;
    }
    else
    if (mw()->activeChildWindow()==1)
    {
        LevelData selData;
        QList<QGraphicsItem *> items = mw()->activeLvlEditWin()->scene->selectedItems();
        foreach(QGraphicsItem * item, items)
        {
            if(item->data(ITEM_TYPE).toString()=="NPC")
            {
                LevelNPC npc = ((ItemNPC*)item)->npcData;

                bool found=false;
                int j;

                //Check Index exists
                if(npc.id < (unsigned int)mw()->configs.index_npc.size())
                {
                    j = mw()->configs.index_npc[npc.id].i;

                    if(j<mw()->configs.main_npc.size())
                    {
                    if(mw()->configs.main_npc[j].id == npc.id)
                        found=true;
                    }
                }
                //if Index found
                if(!found)
                {
                    for(j=0;j<mw()->configs.main_npc.size();j++)
                    {
                        if(mw()->configs.main_npc[j].id==npc.id)
                            break;
                    }
                }

                if(j >= mw()->configs.main_npc.size())
                {
                    j=0;
                }

                if(mw()->configs.main_npc[j].special_2_type != 0) //wrong type, go to next one
                    continue;

                selData.npc.push_back(((ItemNPC*)item)->npcData);
                ((ItemNPC*)item)->npcData.special_data2 = index;
                ((ItemNPC*)item)->arrayApply();
            }
        }
        //mw()->activeLvlEditWin()->scene->addChangeSettingsHistory(selData, LvlScene::SETTING_SPECIAL_DATA, QVariant(index));
    }


}


void LvlItemProperties::on_PROPS_NpcGenerator_clicked(bool checked)
{
    if(LvlItemPropsLock) return;
    if(LockItemProps) return;

    if(npcPtr<0)
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
    if (mw()->activeChildWindow()==1)
    {
        LevelData modData;
        QList<QGraphicsItem *> items = mw()->activeLvlEditWin()->scene->selectedItems();
        foreach(QGraphicsItem * item, items)
        {
            if(item->data(ITEM_TYPE).toString()=="NPC")
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
        mw()->activeLvlEditWin()->scene->addChangeSettingsHistory(modData, HistorySettings::SETTING_GENACTIVATE, QVariant(checked));
    }
    ui->PROPS_NPCGenBox->setVisible( checked );

    npc_refreshMinHeight();
}


void LvlItemProperties::on_PROPS_NPCGenType_currentIndexChanged(int index)
{
    if(LvlItemPropsLock) return;
    if(LockItemProps) return;

    if(npcPtr<0)
    {
        LvlPlacingItems::npcSet.generator_type = index+1;
    }
    else
    if (mw()->activeChildWindow()==1)
    {
        LevelData modData;
        QList<QGraphicsItem *> items = mw()->activeLvlEditWin()->scene->selectedItems();
        foreach(QGraphicsItem * item, items)
        {
            if(item->data(ITEM_TYPE).toString()=="NPC")
            {
                modData.npc.push_back(((ItemNPC*)item)->npcData);
                ((ItemNPC*)item)->setGenerator(((ItemNPC*)item)->npcData.generator,
                 ((ItemNPC*)item)->npcData.generator_direct,
                 index+1
                 );
            }
        }
        mw()->activeLvlEditWin()->scene->addChangeSettingsHistory(modData, HistorySettings::SETTING_GENTYPE, QVariant(index+1));
    }
}

void LvlItemProperties::on_PROPS_NPCGenTime_valueChanged(double arg1)
{
    if(LvlItemPropsLock) return;
    if(LockItemProps) return;

    if(npcPtr<0)
    {
        LvlPlacingItems::npcSet.generator_period = qRound(arg1*10);
    }
    else
    if (mw()->activeChildWindow()==1)
    {
        LevelData modData;
        QList<QGraphicsItem *> items = mw()->activeLvlEditWin()->scene->selectedItems();
        foreach(QGraphicsItem * item, items)
        {
            if((item->data(ITEM_TYPE).toString()=="NPC")/*&&((item->data(2).toInt()==npcPtr))*/)
            {
                modData.npc.push_back(((ItemNPC*)item)->npcData);
                ((ItemNPC*)item)->npcData.generator_period = qRound(arg1*10);
                ((ItemNPC*)item)->arrayApply();
            }
        }
        mw()->activeLvlEditWin()->scene->addChangeSettingsHistory(modData, HistorySettings::SETTING_GENTIME, QVariant(qRound(arg1*10)));
    }

}

void LvlItemProperties::on_PROPS_NPCGenUp_clicked()
{
    if(LvlItemPropsLock) return;
    if(LockItemProps) return;

    if(npcPtr<0)
    {
        LvlPlacingItems::npcSet.generator_direct = 1;
    }
    else
    if (mw()->activeChildWindow()==1)
    {
        LevelData modData;
        QList<QGraphicsItem *> items = mw()->activeLvlEditWin()->scene->selectedItems();
        foreach(QGraphicsItem * item, items)
        {
            if((item->data(ITEM_TYPE).toString()=="NPC")/*&&((item->data(2).toInt()==npcPtr))*/)
            {
                modData.npc.push_back(((ItemNPC*)item)->npcData);
                ((ItemNPC*)item)->setGenerator(((ItemNPC*)item)->npcData.generator,
                 1,
                 ((ItemNPC*)item)->npcData.generator_type
                 );
            }
        }
        mw()->activeLvlEditWin()->scene->addChangeSettingsHistory(modData, HistorySettings::SETTING_GENDIR, QVariant(1));
    }

}

void LvlItemProperties::on_PROPS_NPCGenLeft_clicked()
{
    if(LvlItemPropsLock) return;
    if(LockItemProps) return;

    if(npcPtr<0)
    {
        LvlPlacingItems::npcSet.generator_direct = 2;
    }
    else
    if (mw()->activeChildWindow()==1)
    {
        LevelData modData;
        QList<QGraphicsItem *> items = mw()->activeLvlEditWin()->scene->selectedItems();
        foreach(QGraphicsItem * item, items)
        {
            if((item->data(ITEM_TYPE).toString()=="NPC")/*&&((item->data(2).toInt()==npcPtr))*/)
            {
                modData.npc.push_back(((ItemNPC*)item)->npcData);
                ((ItemNPC*)item)->setGenerator(((ItemNPC*)item)->npcData.generator,
                 2,
                 ((ItemNPC*)item)->npcData.generator_type
                 );
            }
        }
        mw()->activeLvlEditWin()->scene->addChangeSettingsHistory(modData, HistorySettings::SETTING_GENDIR, QVariant(2));
    }
}

void LvlItemProperties::on_PROPS_NPCGenDown_clicked()
{
    if(LvlItemPropsLock) return;
    if(LockItemProps) return;

    if(npcPtr<0)
    {
        LvlPlacingItems::npcSet.generator_direct = 3;
    }
    else
    if (mw()->activeChildWindow()==1)
    {
        LevelData modData;
        QList<QGraphicsItem *> items = mw()->activeLvlEditWin()->scene->selectedItems();
        foreach(QGraphicsItem * item, items)
        {
            if((item->data(ITEM_TYPE).toString()=="NPC")/*&&((item->data(2).toInt()==npcPtr))*/)
            {
                modData.npc.push_back(((ItemNPC*)item)->npcData);
                ((ItemNPC*)item)->setGenerator(((ItemNPC*)item)->npcData.generator,
                 3,
                 ((ItemNPC*)item)->npcData.generator_type
                 );
            }
        }
        mw()->activeLvlEditWin()->scene->addChangeSettingsHistory(modData, HistorySettings::SETTING_GENDIR, QVariant(3));
    }
}
void LvlItemProperties::on_PROPS_NPCGenRight_clicked()
{
    if(LvlItemPropsLock) return;
    if(LockItemProps) return;

    if(npcPtr<0)
    {
        LvlPlacingItems::npcSet.generator_direct = 4;
    }
    else
    if (mw()->activeChildWindow()==1)
    {
        LevelData modData;
        QList<QGraphicsItem *> items = mw()->activeLvlEditWin()->scene->selectedItems();
        foreach(QGraphicsItem * item, items)
        {
            if((item->data(ITEM_TYPE).toString()=="NPC")/*&&((item->data(2).toInt()==npcPtr))*/)
            {
                modData.npc.push_back(((ItemNPC*)item)->npcData);
                ((ItemNPC*)item)->setGenerator(((ItemNPC*)item)->npcData.generator,
                 4,
                 ((ItemNPC*)item)->npcData.generator_type
                 );
            }
        }
        mw()->activeLvlEditWin()->scene->addChangeSettingsHistory(modData, HistorySettings::SETTING_GENDIR, QVariant(4));
    }
}


void LvlItemProperties::on_PROPS_NpcLayer_currentIndexChanged(const QString &arg1)
{
    if(LvlItemPropsLock) return;
    if(LockItemProps) return;

    if(npcPtr<0)
    {
        LvlPlacingItems::npcSet.layer = arg1;
        LvlPlacingItems::layer = arg1;
    }
    else
    if (mw()->activeChildWindow()==1)
    {
        LevelData modData;
        QList<QGraphicsItem *> items = mw()->activeLvlEditWin()->scene->selectedItems();
        foreach(QGraphicsItem * item, items)
        {
            if(item->data(ITEM_TYPE).toString()=="NPC")
            {
                modData.npc.push_back(((ItemNPC*)item)->npcData);
                ((ItemNPC*)item)->setLayer(arg1);
            }
        }
        mw()->activeLvlEditWin()->scene->addChangedLayerHistory(modData, arg1);
    }

}

void LvlItemProperties::on_PROPS_NpcAttachLayer_currentIndexChanged(const QString &arg1)
{
    if(LvlItemPropsLock) return;
    if(LockItemProps) return;

    if(npcPtr<0)
    {
        if(ui->PROPS_NpcAttachLayer->currentIndex()>0)
            LvlPlacingItems::npcSet.attach_layer = arg1;
        else
            LvlPlacingItems::npcSet.attach_layer = "";
    }
    else
    if (mw()->activeChildWindow()==1)
    {
        LevelData modData;
        QList<QGraphicsItem *> items = mw()->activeLvlEditWin()->scene->selectedItems();
        foreach(QGraphicsItem * item, items)
        {
            if(item->data(ITEM_TYPE).toString()=="NPC")
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
            mw()->activeLvlEditWin()->scene->addChangeSettingsHistory(modData, HistorySettings::SETTING_ATTACHLAYER, QVariant(arg1));
        }
        else
        {
            mw()->activeLvlEditWin()->scene->addChangeSettingsHistory(modData, HistorySettings::SETTING_ATTACHLAYER, QVariant(""));
        }
    }

}
void LvlItemProperties::on_PROPS_NpcEventActivate_currentIndexChanged(const QString &arg1)
{
    if(LvlItemPropsLock) return;
    if(LockItemProps) return;


    if(npcPtr<0)
    {
        if(ui->PROPS_NpcEventActivate->currentIndex()>0)
            LvlPlacingItems::npcSet.event_activate = arg1;
        else
            LvlPlacingItems::npcSet.event_activate = "";
        NpcEventActivated = LvlPlacingItems::npcSet.event_activate;
    }
    else
    if (mw()->activeChildWindow()==1)
    {
        LevelData modData;
        QList<QGraphicsItem *> items = mw()->activeLvlEditWin()->scene->selectedItems();
        foreach(QGraphicsItem * item, items)
        {
            if(item->data(ITEM_TYPE).toString()=="NPC")
            {
                modData.npc.push_back(((ItemNPC*)item)->npcData);
                if(ui->PROPS_NpcEventActivate->currentIndex()>0)
                    ((ItemNPC*)item)->npcData.event_activate = arg1;
                else
                    ((ItemNPC*)item)->npcData.event_activate = "";
                ((ItemNPC*)item)->arrayApply();
            }
        }
        if(ui->PROPS_NpcEventActivate->currentIndex()>0){
            mw()->activeLvlEditWin()->scene->addChangeSettingsHistory(modData, HistorySettings::SETTING_EV_ACTIVATE, QVariant(arg1));
        }else{
            mw()->activeLvlEditWin()->scene->addChangeSettingsHistory(modData, HistorySettings::SETTING_EV_ACTIVATE, QVariant(""));
        }
    }

}
void LvlItemProperties::on_PROPS_NpcEventDeath_currentIndexChanged(const QString &arg1)
{
    if(LvlItemPropsLock) return;
    if(LockItemProps) return;


    if(npcPtr<0)
    {
        if(ui->PROPS_NpcEventDeath->currentIndex()>0)
            LvlPlacingItems::npcSet.event_die = arg1;
        else
            LvlPlacingItems::npcSet.event_die = "";
        NpcEventDeath = LvlPlacingItems::npcSet.event_die;
    }
    else
    if (mw()->activeChildWindow()==1)
    {
        LevelData modData;
        QList<QGraphicsItem *> items = mw()->activeLvlEditWin()->scene->selectedItems();
        foreach(QGraphicsItem * item, items)
        {
            if(item->data(ITEM_TYPE).toString()=="NPC")
            {
                modData.npc.push_back(((ItemNPC*)item)->npcData);
                if(ui->PROPS_NpcEventDeath->currentIndex()>0)
                    ((ItemNPC*)item)->npcData.event_die = arg1;
                else
                    ((ItemNPC*)item)->npcData.event_die = "";
                ((ItemNPC*)item)->arrayApply();
            }
        }
        if(ui->PROPS_NpcEventDeath->currentIndex()>0){
            mw()->activeLvlEditWin()->scene->addChangeSettingsHistory(modData, HistorySettings::SETTING_EV_DEATH, QVariant(arg1));
        }else{
            mw()->activeLvlEditWin()->scene->addChangeSettingsHistory(modData, HistorySettings::SETTING_EV_DEATH, QVariant(""));
        }
    }

}
void LvlItemProperties::on_PROPS_NpcEventTalk_currentIndexChanged(const QString &arg1)
{
    if(LvlItemPropsLock) return;
    if(LockItemProps) return;


    if(npcPtr<0)
    {
        if(ui->PROPS_NpcEventTalk->currentIndex()>0)
            LvlPlacingItems::npcSet.event_talk = arg1;
        else
            LvlPlacingItems::npcSet.event_talk = "";
        NpcEventTalk = LvlPlacingItems::npcSet.event_talk;
    }
    else
    if (mw()->activeChildWindow()==1)
    {
        LevelData modData;
        QList<QGraphicsItem *> items = mw()->activeLvlEditWin()->scene->selectedItems();
        foreach(QGraphicsItem * item, items)
        {
            if(item->data(ITEM_TYPE).toString()=="NPC")
            {
                modData.npc.push_back(((ItemNPC*)item)->npcData);
                if(ui->PROPS_NpcEventTalk->currentIndex()>0)
                    ((ItemNPC*)item)->npcData.event_talk = arg1;
                else
                    ((ItemNPC*)item)->npcData.event_talk = "";
                ((ItemNPC*)item)->arrayApply();
            }
        }
        if(ui->PROPS_NpcEventTalk->currentIndex()>0){
            mw()->activeLvlEditWin()->scene->addChangeSettingsHistory(modData, HistorySettings::SETTING_EV_TALK, QVariant(arg1));
        }else{
            mw()->activeLvlEditWin()->scene->addChangeSettingsHistory(modData, HistorySettings::SETTING_EV_TALK, QVariant(""));
        }
    }

}

void LvlItemProperties::on_PROPS_NpcEventEmptyLayer_currentIndexChanged(const QString &arg1)
{
    if(LvlItemPropsLock) return;
    if(LockItemProps) return;


    if(npcPtr<0)
    {
        if(ui->PROPS_NpcEventEmptyLayer->currentIndex()>0)
            LvlPlacingItems::npcSet.event_nomore = arg1;
        else
            LvlPlacingItems::npcSet.event_nomore = "";
        NpcEventLayerEmpty = LvlPlacingItems::npcSet.event_nomore;
    }
    else
    if (mw()->activeChildWindow()==1)
    {
        LevelData modData;
        QList<QGraphicsItem *> items = mw()->activeLvlEditWin()->scene->selectedItems();
        foreach(QGraphicsItem * item, items)
        {
            if(item->data(ITEM_TYPE).toString()=="NPC")
            {
                modData.npc.push_back(((ItemNPC*)item)->npcData);
                if(ui->PROPS_NpcEventEmptyLayer->currentIndex()>0)
                    ((ItemNPC*)item)->npcData.event_nomore = arg1;
                else
                    ((ItemNPC*)item)->npcData.event_nomore = "";
                ((ItemNPC*)item)->arrayApply();
            }
        }
        if(ui->PROPS_NpcEventEmptyLayer->currentIndex()>0){
            mw()->activeLvlEditWin()->scene->addChangeSettingsHistory(modData, HistorySettings::SETTING_EV_LAYER_EMP, QVariant(arg1));
        }else{
            mw()->activeLvlEditWin()->scene->addChangeSettingsHistory(modData, HistorySettings::SETTING_EV_LAYER_EMP, QVariant(""));
        }
    }
}
