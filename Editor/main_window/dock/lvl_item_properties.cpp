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

#include <QJsonObject>
#include <QMutexLocker>
#include <PGE_File_Formats/file_formats.h>
#include <common_features/util.h>
#include <common_features/json_settings_widget.h>
#include <defines.h>
#include <editing/_scenes/level/lvl_history_manager.h>
#include <editing/_scenes/level/lvl_item_placing.h>
#include <editing/_dialogs/itemselectdialog.h>
#include <editing/_scenes/level/itemmsgbox.h>
#include <common_features/direction_switch_widget.h>

#include "lvl_item_properties.h"
#include "ui_lvl_item_properties.h"

#include <ui_mainwindow.h>
#include <mainwindow.h>

#include <editing/_components/history/settings/lvl_block_userdata.hpp>
#include <editing/_components/history/settings/lvl_bgo_userdata.hpp>
#include <editing/_components/history/settings/lvl_npc_userdata.hpp>

/*!
 * \brief Return reference to global NPC settings container with dependency to container setup. If this NPC a container - find config from contained NPC
 * \param npc Level NPC settings
 * \param mw Pointer to Main Window
 * \return Reference to global NPC config entry
 */
static obj_npc &getNpcProps(LevelNPC &npc, MainWindow *mw)
{
    int tarNPC = static_cast<int>(npc.id);
    bool isLvlWin = ((mw->activeChildWindow() == MainWindow::WND_Level) && (mw->activeLvlEditWin()));
    PGE_DataArray<obj_npc> &dataSource = isLvlWin ?
                                         mw->activeLvlEditWin()->scene->m_localConfigNPCs :
                                         mw->configs.main_npc;
    obj_npc *findNPC;
    obj_npc &thisNPC = dataSource[tarNPC];
    if(thisNPC.setup.container && !thisNPC.setup.special_option)
        findNPC = &dataSource[static_cast<int>(npc.contents)];
    else
        findNPC = &dataSource[tarNPC];
    return *findNPC;
}

LvlItemProperties::LvlItemProperties(QWidget *parent) :
    QDockWidget(parent),
    MWDock_Base(parent),
    ui(new Ui::LvlItemProperties)
{
    setVisible(false);
    ui->setupUi(this);

    ui->npcGeneratorDirection->setPixmap(DirectionSwitchWidget::S_CENTER, QPixmap(":/arrows/arrows/box.png"));
    ui->npcGeneratorDirection->setPixmap(DirectionSwitchWidget::S_LEFT, QPixmap(":/arrows/arrows/black_left.png"));
    ui->npcGeneratorDirection->setPixmap(DirectionSwitchWidget::S_RIGHT, QPixmap(":/arrows/arrows/black_right.png"));
    ui->npcGeneratorDirection->setPixmap(DirectionSwitchWidget::S_TOP, QPixmap(":/arrows/arrows/black_up.png"));
    ui->npcGeneratorDirection->setPixmap(DirectionSwitchWidget::S_BOTTOM, QPixmap(":/arrows/arrows/black_down.png"));
    ui->npcGeneratorDirection->setPixmapOn(DirectionSwitchWidget::S_LEFT, QPixmap(":/arrows/arrows/green_left.png"));
    ui->npcGeneratorDirection->setPixmapOn(DirectionSwitchWidget::S_RIGHT, QPixmap(":/arrows/arrows/green_right.png"));
    ui->npcGeneratorDirection->setPixmapOn(DirectionSwitchWidget::S_TOP, QPixmap(":/arrows/arrows/green_up.png"));
    ui->npcGeneratorDirection->setPixmapOn(DirectionSwitchWidget::S_BOTTOM, QPixmap(":/arrows/arrows/green_down.png"));
    ui->npcGeneratorDirection->mapValue(DirectionSwitchWidget::S_LEFT, LevelNPC::NPC_GEN_LEFT);
    ui->npcGeneratorDirection->mapValue(DirectionSwitchWidget::S_RIGHT, LevelNPC::NPC_GEN_RIGHT);
    ui->npcGeneratorDirection->mapValue(DirectionSwitchWidget::S_TOP, LevelNPC::NPC_GEN_UP);
    ui->npcGeneratorDirection->mapValue(DirectionSwitchWidget::S_BOTTOM, LevelNPC::NPC_GEN_DOWN);

    re_translate_widgets();

    m_extraSettingsSpacer.reset(new QSpacerItem(100, 999999, QSizePolicy::Minimum, QSizePolicy::Expanding));
    setAttribute(Qt::WA_LayoutUsesWidgetRect);
    setAttribute(Qt::WA_ShowWithoutActivating);
    setFocusPolicy(Qt::StrongFocus);
    setFocusProxy(mw());
    ui->scrollArea->setWidgetResizable(true);
    m_npcSpecSpinOffset = 0;
    m_npcSpecSpinOffset_2 = 0;
    m_internalLock = true;

    m_curItemType = -1;
    m_recentBlockEventDestroy = "";
    m_recentBlockEventHit = "";
    m_recentBlockEventLayerEmpty = "";

    m_recentNpcEventActivated = "";
    m_recentNpcEventDeath = "";
    m_recentNpcEventTalk = "";
    m_recentNpcEventLayerEmpty = "";

    m_externalLock = true;

    m_currentBlockArrayId = -1;
    m_currentBgoArrayId = -1;
    m_currentNpcArrayId = -1;

    QRect mwg = mw()->geometry();
    int GOffset = 10;
    mw()->addDockWidget(Qt::RightDockWidgetArea, this);
    connect(mw(), &MainWindow::languageSwitched, this, &LvlItemProperties::re_translate);
    connect(mw(), &MainWindow::setSMBX64Strict, this, &LvlItemProperties::setSMBX64Strict);
    connect(this, &QDockWidget::visibilityChanged, mw()->ui->action_Placing_ShowProperties, &QAction::setChecked);
    mw()->ui->action_Placing_ShowProperties->setChecked(isVisible());

    connect(ui->npcGeneratorDirection, &DirectionSwitchWidget::clicked, this, &LvlItemProperties::npcGeneratorDirectionChange);

    setFloating(true);
    setGeometry(
        mwg.right() - width() - GOffset,
        mwg.y() + 120,
        width(),
        height()
    );
}

LvlItemProperties::~LvlItemProperties()
{
    if(m_extraSettings.get())
        ui->extraSettings->layout()->removeWidget(m_extraSettings.get()->getWidget());
    if(m_extraGlobalSettings.get())
        ui->extraSettings->layout()->removeWidget(m_extraGlobalSettings.get()->getWidget());
    ui->extraSettings->layout()->removeItem(m_extraSettingsSpacer.get());
    m_extraSettings.reset();
    m_extraGlobalSettings.reset();
    m_extraSettingsSpacer.reset();
    delete ui;
}

void LvlItemProperties::re_translate_widgets()
{
    ui->npcGeneratorDirection->mapToolTip(DirectionSwitchWidget::S_LEFT, tr("Left", "Throwing direction of NPC, spawned via Generator object."));
    ui->npcGeneratorDirection->mapToolTip(DirectionSwitchWidget::S_RIGHT, tr("Right", "Throwing direction of NPC, spawned via Generator object."));
    ui->npcGeneratorDirection->mapToolTip(DirectionSwitchWidget::S_TOP, tr("Up", "Throwing direction of NPC, spawned via Generator object."));
    ui->npcGeneratorDirection->mapToolTip(DirectionSwitchWidget::S_BOTTOM, tr("Down", "Throwing direction of NPC, spawned via Generator object."));
    ui->npcGeneratorDirection->mapToolTip(DirectionSwitchWidget::S_TOP_LEFT, tr("Up-Left", "Throwing direction of NPC, spawned via Generator object."));
    ui->npcGeneratorDirection->mapToolTip(DirectionSwitchWidget::S_TOP_RIGHT, tr("Up-Right", "Throwing direction of NPC, spawned via Generator object."));
    ui->npcGeneratorDirection->mapToolTip(DirectionSwitchWidget::S_BOTTOM_LEFT, tr("Down-Left", "Throwing direction of NPC, spawned via Generator object."));
    ui->npcGeneratorDirection->mapToolTip(DirectionSwitchWidget::S_BOTTOM_RIGHT, tr("Down-Right", "Throwing direction of NPC, spawned via Generator object."));
}

void LvlItemProperties::setSettingsLock(bool locked)
{
    m_externalLock = locked;
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

void LvlItemProperties::setSMBX64Strict(bool en)
{
    dataconfigs &c = mw()->configs;
    bool zLayer_hide = (c.editor.supported_features.level_bgo_z_layer == EditorSetup::FeaturesSupport::F_HIDDEN);
    bool zLayer_active = (c.editor.supported_features.level_bgo_z_layer == EditorSetup::FeaturesSupport::F_ENABLED);

    bool zPos_hide = (c.editor.supported_features.level_bgo_z_position == EditorSetup::FeaturesSupport::F_HIDDEN);
    bool zPos_active = (c.editor.supported_features.level_bgo_z_position == EditorSetup::FeaturesSupport::F_ENABLED);

    bool sp_hide = (c.editor.supported_features.level_bgo_smbx64_sp == EditorSetup::FeaturesSupport::F_HIDDEN);
    bool sp_active = (c.editor.supported_features.level_bgo_smbx64_sp == EditorSetup::FeaturesSupport::F_ENABLED);

    ui->PROPS_BGO_Z_Pos->setEnabled(!en);
    ui->PROPS_BGO_Z_Pos->setVisible(!zLayer_hide || !zPos_hide);

    ui->PROPS_BGO_Z_Layer->setEnabled(zLayer_active);
    ui->PROPS_BGO_Z_Layer->setHidden(zLayer_hide);

    ui->PROPS_BGO_Z_Layer_label->setEnabled(zLayer_active);
    ui->PROPS_BGO_Z_Layer_label->setHidden(zLayer_hide);

    ui->PROPS_BGO_Z_Offset->setEnabled(zPos_active);
    ui->PROPS_BGO_Z_Offset_label->setEnabled(zPos_active);

    ui->PROPS_BGO_Z_Offset->setHidden(zPos_hide);
    ui->PROPS_BGO_Z_Offset_label->setHidden(zPos_hide);

    ui->smbx64SpGroup->setHidden(sp_hide);
    ui->smbx64SpGroup->setEnabled(sp_active);

    ui->extraSettings->setEnabled(!en);
}
/******************Combo boxes*********************************/

void LvlItemProperties::re_translate()
{
    m_externalLock = true;
    int npcGenType = ui->PROPS_NPCGenType->currentIndex(); //backup combobox's index

    ui->retranslateUi(this);
    re_translate_widgets();

    ui->PROPS_NPCGenType->setCurrentIndex(npcGenType);
    m_externalLock = false;
}

// Dummy objects
static LevelBlock   dummyBlock = FileFormats::CreateLvlBlock();
static LevelBGO     dummyBgo   = FileFormats::CreateLvlBgo();
static LevelNPC     dummyNpc   = FileFormats::CreateLvlNpc();

void LvlItemProperties::OpenBlock(LevelBlock &block, bool newItem, bool dont_reset_props, bool dontShow)
{
    openPropertiesFor(ItemTypes::LVL_Block,
                 block,
                 dummyBgo,
                 dummyNpc,
                 newItem,
                 dont_reset_props,
                 dontShow);
}

void LvlItemProperties::OpenBGO(LevelBGO &bgo, bool newItem, bool dont_reset_props, bool dontShow)
{
    openPropertiesFor(ItemTypes::LVL_BGO,
                 dummyBlock,
                 bgo,
                 dummyNpc,
                 newItem,
                 dont_reset_props,
                 dontShow);
}

void LvlItemProperties::OpenNPC(LevelNPC &npc, bool newItem, bool dont_reset_props, bool dontShow)
{
    openPropertiesFor(ItemTypes::LVL_NPC,
                 dummyBlock,
                 dummyBgo,
                 npc,
                 newItem,
                 dont_reset_props,
                 dontShow);
}

void LvlItemProperties::CloseBox()
{
    hide();
    ui->blockProp->setVisible(false);
    ui->bgoProps->setVisible(false);
    ui->npcProps->setVisible(false);

    m_externalLock = true;
    m_internalLock = true;
    LvlPlacingItems::npcSpecialAutoIncrement = false;
}

void LvlItemProperties::openPropertiesFor(int Type,
                                     LevelBlock &block,
                                     LevelBGO &bgo,
                                     LevelNPC &npc,
                                     bool isPlacingNew,
                                     bool dontResetProps,
                                     bool dontShowToolbox)
{
    QMutexLocker mlock(&m_mutex); Q_UNUSED(mlock)

    mw()->LayerListsSync();
    mw()->EventListsSync();

    ui->blockProp->setVisible(false);
    ui->bgoProps->setVisible(false);
    ui->npcProps->setVisible(false);

    m_externalLock = true;
    m_internalLock = true;

    LvlPlacingItems::npcSpecialAutoIncrement = false;

    ui->extraSettings->setToolTip("");
    ui->extraSettings->setMinimumHeight(0);
    ui->extraSettings->setStyleSheet("");
    if(m_extraSettings.get())
        ui->extraSettings->layout()->removeWidget(m_extraSettings.get()->getWidget());
    if(m_extraGlobalSettings.get())
        ui->extraSettings->layout()->removeWidget(m_extraGlobalSettings.get()->getWidget());
    ui->extraSettings->layout()->removeItem(m_extraSettingsSpacer.get());
    m_extraSettings.reset();
    m_extraGlobalSettings.reset();

    ui->PROPS_BlkEventDestroyedLock->setVisible(isPlacingNew);
    ui->PROPS_BlkEventHitLock->setVisible(isPlacingNew);
    ui->PROPS_BlkEventLEmptyLock->setVisible(isPlacingNew);

    ui->PROPS_NpcEventActovateLock->setVisible(isPlacingNew);
    ui->PROPS_NpcEventDeathLock->setVisible(isPlacingNew);
    ui->PROPS_NpcEventTalkLock->setVisible(isPlacingNew);
    ui->PROPS_NpcEventLEmptyLock->setVisible(isPlacingNew);

    QString configDir = mw()->configs.config_dir;

    /*
    long blockPtr; //ArrayID of editing item (-1 - use system)
    long bgoPtr; //ArrayID of editing item
    long npcPtr; //ArrayID of editing item
    */
    m_externalLock = true;

    m_curItemType = Type;

    switch(Type)
    {
    case ItemTypes::LVL_Block:
    {
        if(isPlacingNew)
            m_currentBlockArrayId = -1;
        else
            m_currentBlockArrayId = block.meta.array_id;

        ui->PROPS_BlockID->setText(tr("Block ID: %1, Array ID: %2").arg(block.id).arg(block.meta.array_id));

        bool isLvlWin = ((mw()->activeChildWindow() == MainWindow::WND_Level) && (mw()->activeLvlEditWin()));

        obj_block &t_block = isLvlWin ?
                             mw()->activeLvlEditWin()->scene->m_localConfigBlocks[block.id] :
                             mw()->configs.main_block[block.id];
        if(!t_block.isValid)
            t_block = mw()->configs.main_block[1];


        if(isPlacingNew && (!dontResetProps))
        {
            LvlPlacingItems::blockSet.invisible = t_block.setup.default_invisible_value;
            block.invisible = t_block.setup.default_invisible_value;

            LvlPlacingItems::blockSet.slippery = t_block.setup.default_slippery_value;
            block.slippery = t_block.setup.default_slippery_value;

            LvlPlacingItems::blockSet.npc_id = t_block.setup.default_content_value;
            block.npc_id = t_block.setup.default_content_value;

            LvlPlacingItems::blockSet.layer = LvlPlacingItems::layer.isEmpty() ? "Default" : LvlPlacingItems::layer;
            block.layer = LvlPlacingItems::layer.isEmpty() ? "Default" : LvlPlacingItems::layer;
            LvlPlacingItems::layer = block.layer;

            if(ui->PROPS_BlkEventDestroyedLock->isChecked())
                LvlPlacingItems::blockSet.event_destroy = m_recentBlockEventDestroy;
            else
                LvlPlacingItems::blockSet.event_destroy = "";
            block.event_destroy = LvlPlacingItems::blockSet.event_destroy;

            if(ui->PROPS_BlkEventHitLock->isChecked())
                LvlPlacingItems::blockSet.event_hit = m_recentBlockEventHit;
            else
                LvlPlacingItems::blockSet.event_hit = "";
            block.event_hit = LvlPlacingItems::blockSet.event_hit;

            if(ui->PROPS_BlkEventLEmptyLock->isChecked())
                LvlPlacingItems::blockSet.event_emptylayer = m_recentBlockEventLayerEmpty;
            else
                LvlPlacingItems::blockSet.event_emptylayer = "";
            block.event_emptylayer = LvlPlacingItems::blockSet.event_emptylayer;
        }


        ui->PROPS_blockPos->setText(tr("Position: [%1, %2]").arg(block.x).arg(block.y));
        ui->PROPS_BlockResize->setVisible(t_block.setup.sizable);
        ui->sizeOfBlock->setVisible(t_block.setup.sizable && (!isPlacingNew));
        ui->BLOCK_Width->setValue(block.w);
        ui->BLOCK_Height->setValue(block.h);

        ui->PROPS_BlockInvis->setChecked(block.invisible);
        ui->PROPS_BlkSlippery->setChecked(block.slippery);

        //ui->PROPS_BlockSquareFill->setVisible( newItem );
        //ui->PROPS_BlockSquareFill->setChecked( LvlPlacingItems::placingMode==LvlPlacingItems::PMODE_Square );

        ui->PROPS_BlockIncludes->setText(
            ((block.npc_id != 0) ?
             ((block.npc_id > 0) ? QString("NPC-%1").arg(block.npc_id) : tr("%1 coins").arg(block.npc_id * -1))
             : tr("[empty]")
            ));

        ui->PROPS_BlockLayer->setCurrentIndex(0);
        for(int i = 0; i < ui->PROPS_BlockLayer->count(); i++)
        {
            if(ui->PROPS_BlockLayer->itemText(i) == block.layer)
            {
                ui->PROPS_BlockLayer->setCurrentIndex(i);
                break;
            }
        }

        ui->PROPS_BlkEventDestroy->setCurrentIndex(0);
        for(int i = 0; i < ui->PROPS_BlkEventDestroy->count(); i++)
        {
            if(ui->PROPS_BlkEventDestroy->itemText(i) == block.event_destroy)
            {
                ui->PROPS_BlkEventDestroy->setCurrentIndex(i);
                break;
            }
        }

        ui->PROPS_BlkEventHited->setCurrentIndex(0);
        for(int i = 0; i < ui->PROPS_BlkEventHited->count(); i++)
        {
            if(ui->PROPS_BlkEventHited->itemText(i) == block.event_hit)
            {
                ui->PROPS_BlkEventHited->setCurrentIndex(i);
                break;
            }
        }

        ui->PROPS_BlkEventLayerEmpty->setCurrentIndex(0);
        for(int i = 0; i < ui->PROPS_BlkEventLayerEmpty->count(); i++)
        {
            if(ui->PROPS_BlkEventLayerEmpty->itemText(i) == block.event_emptylayer)
            {
                ui->PROPS_BlkEventLayerEmpty->setCurrentIndex(i);
                break;
            }
        }

        initExtraSettingsWidget(mw()->configs.getBlockExtraSettingsPath(),
                                configDir,
                                t_block.setup.extra_settings,
                                "global_block.json",
                                block.meta.custom_params,
                                &LvlItemProperties::onExtraSettingsBlockChanged);
        m_externalLock = false;
        m_internalLock = false;

        ui->blockProp->show();
        ui->blockProp->adjustSize();
        if(!dontShowToolbox)
        {
            show();
            raise();
        }
        break;
    }
    case ItemTypes::LVL_BGO:
    {
        if(isPlacingNew)
            m_currentBgoArrayId = -1;
        else
            m_currentBgoArrayId = bgo.meta.array_id;


        if((m_currentBgoArrayId < 0) && (!dontResetProps))
        {
            LvlPlacingItems::bgoSet.layer = LvlPlacingItems::layer.isEmpty() ? "Default" : LvlPlacingItems::layer;
            bgo.layer = LvlPlacingItems::layer.isEmpty() ? "Default" : LvlPlacingItems::layer;
            LvlPlacingItems::layer = bgo.layer;
        }

        ui->PROPS_BgoID->setText(tr("BGO ID: %1, Array ID: %2").arg(bgo.id).arg(bgo.meta.array_id));

        bool isLvlWin = ((mw()->activeChildWindow() == MainWindow::WND_Level) && (mw()->activeLvlEditWin()));

        obj_bgo &t_bgo   = isLvlWin ?
                           mw()->activeLvlEditWin()->scene->m_localConfigBGOs[bgo.id] :
                           mw()->configs.main_bgo[bgo.id];

        if(!t_bgo.isValid)
            t_bgo = mw()->configs.main_bgo[1];

        //ui->PROPS_BGOSquareFill->setVisible( newItem );
        //ui->PROPS_BGOSquareFill->setChecked( LvlPlacingItems::placingMode==LvlPlacingItems::PMODE_Square );

        ui->PROPS_bgoPos->setText(tr("Position: [%1, %2]").arg(bgo.x).arg(bgo.y));

        ui->PROPS_BGOLayer->setCurrentIndex(0);
        for(int i = 0; i < ui->PROPS_BGOLayer->count(); i++)
        {
            if(ui->PROPS_BGOLayer->itemText(i) == bgo.layer)
            {
                ui->PROPS_BGOLayer->setCurrentIndex(i);
                break;
            }
        }

        //PGE-X values
        int zMode = 2;
        switch(bgo.z_mode)
        {
        case LevelBGO::Background2:
            zMode = 0;
            break;
        case LevelBGO::Background1:
            zMode = 1;
            break;
        case LevelBGO::Foreground1:
            zMode = 3;
            break;
        case LevelBGO::Foreground2:
            zMode = 4;
            break;
        case LevelBGO::ZDefault:
        default:
            zMode = 2;
            break;
        }

        ui->PROPS_BGO_Z_Layer->setCurrentIndex(zMode);

        ui->PROPS_BGO_Z_Offset->setValue(bgo.z_offset);
        //PGE-X values

        ui->PROPS_BGO_smbx64_sp->setValue(bgo.smbx64_sp);

        initExtraSettingsWidget(mw()->configs.getBgoExtraSettingsPath(),
                                configDir,
                                t_bgo.setup.extra_settings,
                                "global_bgo.json",
                                bgo.meta.custom_params,
                                &LvlItemProperties::onExtraSettingsBGOChanged);

        m_externalLock = false;
        m_internalLock = false;

        ui->bgoProps->show();
        ui->bgoProps->adjustSize();
        if(!dontShowToolbox)
        {
            show();
            raise();
        }
        break;
    }
    case ItemTypes::LVL_NPC:
    {
        if(isPlacingNew)
            m_currentNpcArrayId = -1;
        else
            m_currentNpcArrayId = npc.meta.array_id;

        ui->PROPS_NpcID->setText(tr("NPC ID: %1, Array ID: %2").arg(npc.id).arg(npc.meta.array_id));

        bool isLvlWin = ((mw()->activeChildWindow() == MainWindow::WND_Level) && (mw()->activeLvlEditWin()));
        obj_npc &t_npc   = isLvlWin ?
                           mw()->activeLvlEditWin()->scene->m_localConfigNPCs[npc.id] :
                           mw()->configs.main_npc[npc.id];

        if(!t_npc.isValid)
            t_npc = mw()->configs.main_npc[1];

        ui->PROPS_NPCContaiter->hide();
        ui->PROPS_NpcContainsLabel->hide();

        ui->PROPS_NPCSpecialNPC->hide();
        ui->PROPS_NPCNpcLabel->hide();

        ui->PROPS_NPCBoxLabel->hide();
        ui->PROPS_NPCSpecialBox->hide();

        ui->PROPS_NpcSpinLabel->hide();
        ui->PROPS_NPCSpecialSpin->hide();
        ui->PROPS_NPCSpecialSpin_Auto->hide();

        ui->line_6->hide();

        if((m_currentNpcArrayId < 0) && (!dontResetProps))
        {
            LvlPlacingItems::npcSet.msg = "";
            npc.msg = "";

            LvlPlacingItems::npcSet.friendly = t_npc.setup.default_friendly_value;
            npc.friendly = t_npc.setup.default_friendly_value;

            LvlPlacingItems::npcSet.nomove = t_npc.setup.default_nomovable_value;
            npc.nomove = t_npc.setup.default_nomovable_value;

            LvlPlacingItems::npcSet.is_boss = t_npc.setup.default_boss_value;
            npc.is_boss = t_npc.setup.default_boss_value;

            if(t_npc.setup.default_special)
            {
                LvlPlacingItems::npcSet.special_data = t_npc.setup.default_special_value;
                npc.special_data = t_npc.setup.default_special_value;
            }
            LvlPlacingItems::npcSet.special_data2 = 0;
            npc.special_data2 = 0;

            LvlPlacingItems::npcSet.generator = false;
            npc.generator = false;

            LvlPlacingItems::npcSet.layer = LvlPlacingItems::layer.isEmpty() ? "Default" : LvlPlacingItems::layer;
            npc.layer = LvlPlacingItems::layer.isEmpty() ? "Default" : LvlPlacingItems::layer;
            LvlPlacingItems::layer = npc.layer;

            npc.attach_layer = "";
            LvlPlacingItems::npcSet.attach_layer = "";

            LvlPlacingItems::npcSet.event_activate = ui->PROPS_NpcEventActovateLock->isChecked() ? m_recentNpcEventActivated : "";
            npc.event_activate = LvlPlacingItems::npcSet.event_activate;

            LvlPlacingItems::npcSet.event_die = ui->PROPS_NpcEventDeathLock->isChecked() ? m_recentNpcEventDeath : "";
            npc.event_die = LvlPlacingItems::npcSet.event_die;

            LvlPlacingItems::npcSet.event_talk = ui->PROPS_NpcEventTalkLock->isChecked() ? m_recentNpcEventTalk : "";
            npc.event_talk = LvlPlacingItems::npcSet.event_talk;

            LvlPlacingItems::npcSet.event_emptylayer = ui->PROPS_NpcEventLEmptyLock->isChecked() ? m_recentNpcEventLayerEmpty : "";
            npc.event_emptylayer = LvlPlacingItems::npcSet.event_emptylayer;
        }

        ui->PROPS_NpcPos->setText(tr("Position: [%1, %2]").arg(npc.x).arg(npc.y));
        ui->PROPS_NpcDir->setTitle(t_npc.setup.direct_alt_title.isEmpty() ? tr("Direction") : t_npc.setup.direct_alt_title);
        ui->PROPS_NPCDirLeft->setText(t_npc.setup.direct_alt_left.isEmpty() ? tr("Left") : t_npc.setup.direct_alt_left);
        ui->PROPS_NPCDirRand->setEnabled(!t_npc.setup.direct_disable_random);
        ui->PROPS_NPCDirRand->setText(t_npc.setup.direct_alt_rand.isEmpty() ? tr("Random") : t_npc.setup.direct_alt_rand);
        ui->PROPS_NPCDirRight->setText(t_npc.setup.direct_alt_right.isEmpty() ? tr("Right") : t_npc.setup.direct_alt_right);

        switch(npc.direct)
        {
        case -1:
            ui->PROPS_NPCDirLeft->setChecked(true);
            break;
        default:
        case 0:
            ui->PROPS_NPCDirRand->setChecked(true);
            break;
        case 1:
            ui->PROPS_NPCDirRight->setChecked(true);
            break;
        }

        if(t_npc.setup.container)
        {
            ui->PROPS_NpcContainsLabel->show();
            ui->PROPS_NPCContaiter->show();
            ui->PROPS_NPCContaiter->setText(
                ((npc.contents > 0) ? QString("NPC-%1").arg(npc.contents)
                 : tr("[empty]")
                ));
        }

        //refresh special option 1
        refreshFirstNpcSpecialOption(npc, isPlacingNew, dontResetProps);

        QString npcmsg = (npc.msg.isEmpty() ? tr("[none]") : npc.msg);
        if(npcmsg.size() > 20)
        {
            npcmsg.resize(18);
            npcmsg.push_back("...");
        }
        ui->PROPS_NpcTMsg->setText(npcmsg.replace("&", "&&&"));

        ui->PROPS_NpcFri->setChecked(npc.friendly);
        ui->PROPS_NPCNoMove->setChecked(npc.nomove);
        ui->PROPS_NpcBoss->setChecked(npc.is_boss);

        ui->PROPS_NpcGenerator->setChecked(npc.generator);
        ui->PROPS_NPCGenBox->setVisible(npc.generator);
        if(npc.generator)
        {
            switch(npc.generator_type)
            {
            case 1:
                ui->PROPS_NPCGenType->setCurrentIndex(0);
                break;
            case 2:
            default:
                ui->PROPS_NPCGenType->setCurrentIndex(1);
                break;
            }

            ui->PROPS_NPCGenTime->setValue((double)npc.generator_period / 10);

            ui->npcGeneratorDirection->setDirection(npc.generator_direct);
        }
        else
        {
            ui->npcGeneratorDirection->setDirection(LevelNPC::NPC_GEN_UP);
            ui->PROPS_NPCGenType->setCurrentIndex(0);
        }

        ui->PROPS_NpcLayer->setCurrentIndex(0);
        for(int i = 0; i < ui->PROPS_NpcLayer->count(); i++)
        {
            if(ui->PROPS_NpcLayer->itemText(i) == npc.layer)
            {
                ui->PROPS_NpcLayer->setCurrentIndex(i);
                break;
            }
        }
        ui->PROPS_NpcAttachLayer->setCurrentIndex(0);
        for(int i = 0; i < ui->PROPS_NpcAttachLayer->count(); i++)
        {
            if(ui->PROPS_NpcAttachLayer->itemText(i) == npc.attach_layer)
            {
                ui->PROPS_NpcAttachLayer->setCurrentIndex(i);
                break;
            }
        }

        ui->PROPS_NpcEventActivate->setCurrentIndex(0);
        for(int i = 0; i < ui->PROPS_NpcEventActivate->count(); i++)
        {
            if(ui->PROPS_NpcEventActivate->itemText(i) == npc.event_activate)
            {
                ui->PROPS_NpcEventActivate->setCurrentIndex(i);
                break;
            }
        }

        ui->PROPS_NpcEventDeath->setCurrentIndex(0);
        for(int i = 0; i < ui->PROPS_NpcEventDeath->count(); i++)
        {
            if(ui->PROPS_NpcEventDeath->itemText(i) == npc.event_die)
            {
                ui->PROPS_NpcEventDeath->setCurrentIndex(i);
                break;
            }
        }

        ui->PROPS_NpcEventTalk->setCurrentIndex(0);
        for(int i = 0; i < ui->PROPS_NpcEventTalk->count(); i++)
        {
            if(ui->PROPS_NpcEventTalk->itemText(i) == npc.event_talk)
            {
                ui->PROPS_NpcEventTalk->setCurrentIndex(i);
                break;
            }
        }

        ui->PROPS_NpcEventEmptyLayer->setCurrentIndex(0);
        for(int i = 0; i < ui->PROPS_NpcEventEmptyLayer->count(); i++)
        {
            if(ui->PROPS_NpcEventEmptyLayer->itemText(i) == npc.event_emptylayer)
            {
                ui->PROPS_NpcEventEmptyLayer->setCurrentIndex(i);
                break;
            }
        }

        initExtraSettingsWidget(mw()->configs.getNpcExtraSettingsPath(),
                                configDir,
                                t_npc.setup.extra_settings,
                                "global_npc.json",
                                npc.meta.custom_params,
                                &LvlItemProperties::onExtraSettingsNPCChanged);

        m_externalLock = false;
        m_internalLock = false;

        ui->npcProps->show();
        ui->npcProps->adjustSize();
        if(!dontShowToolbox)
        {
            show();
            raise();
        }
        break;
    }
    case -1: //Nothing to edit
    default:
        hide();
    }
}

void LvlItemProperties::LvlItemProps_updateLayer(QString lname)
{
    if(m_externalLock) return;
    m_externalLock = true;

    if(lname.isEmpty())
        lname = LvlPlacingItems::layer;

    switch(m_curItemType)
    {
    case 0:
    {
        ui->PROPS_BlockLayer->setCurrentIndex(0);
        for(int i = 0; i < ui->PROPS_BlockLayer->count(); i++)
        {
            if(ui->PROPS_BlockLayer->itemText(i) == lname)
            {
                ui->PROPS_BlockLayer->setCurrentIndex(i);
                break;
            }
        }
    }
    break;
    case 1:
    {
        ui->PROPS_BGOLayer->setCurrentIndex(0);
        for(int i = 0; i < ui->PROPS_BGOLayer->count(); i++)
        {
            if(ui->PROPS_BGOLayer->itemText(i) == lname)
            {
                ui->PROPS_BGOLayer->setCurrentIndex(i);
                break;
            }
        }
    }
    break;
    case 2:
    {
        ui->PROPS_NpcLayer->setCurrentIndex(0);
        for(int i = 0; i < ui->PROPS_NpcLayer->count(); i++)
        {
            if(ui->PROPS_NpcLayer->itemText(i) == lname)
            {
                ui->PROPS_NpcLayer->setCurrentIndex(i);
                break;
            }
        }
    }
    }
    m_externalLock = false;
}

void LvlItemProperties::initExtraSettingsWidget(const QString &defaultLocalDir,
                                                const QString &defaultGlobalDir,
                                                const QString &layoutPath,
                                                const QString &layoutPathGlobal,
                                                QString &properties,
                                                void (LvlItemProperties::*callback)())
{
    LevelEdit *edit = nullptr;
    if((!layoutPath.isEmpty() || !layoutPathGlobal.isEmpty()) &&
        (mw()->activeChildWindow() == MainWindow::WND_Level) &&
        (edit = mw()->activeLvlEditWin()))
    {
        CustomDirManager uLVL(edit->LvlData.meta.path, edit->LvlData.meta.filename);

        uLVL.setDefaultDir(defaultLocalDir);
        QString esLayoutFile = uLVL.getCustomFile(layoutPath);
        uLVL.setDefaultDir(defaultGlobalDir);
        QString gsLayoutFile = uLVL.getCustomFile(layoutPathGlobal);
        bool hasError = false;
        bool spacerNeeded = false;

        QJsonParseError errCode = QJsonParseError();
        QJsonDocument tree;
        if(!properties.isEmpty())
            tree = QJsonDocument::fromJson(properties.toUtf8(), &errCode);

        // Global object's extra settings
        if(!gsLayoutFile.isEmpty())
        {
            QFile layoutFile(gsLayoutFile);
            if(layoutFile.open(QIODevice::ReadOnly))
            {
                QByteArray rawLayout = layoutFile.readAll();
                m_extraGlobalSettings.reset(new JsonSettingsWidget(ui->extraSettings));
                Q_ASSERT(m_extraGlobalSettings.get());

                m_extraGlobalSettings->setSearchDirectories(edit->LvlData.meta.path, edit->LvlData.meta.filename);
                m_extraGlobalSettings->setConfigPack(&mw()->configs);

                QJsonDocument branch = tree;
                if(errCode.error == QJsonParseError::NoError)
                {
                    QJsonObject root = branch.object();
                    auto global_key = root.find("global");
                    if(global_key != root.end())
                    {
                        auto global_val = global_key.value();
                        if(global_val.isObject())
                            branch.setObject(global_val.toObject());
                        else
                            branch = QJsonDocument();
                    } else
                        branch = QJsonDocument();
                } else
                    branch = QJsonDocument();

                if(!m_extraGlobalSettings->loadLayout(branch, rawLayout))
                {
                    LogWarning(m_extraGlobalSettings->errorString());
                    ui->extraSettings->setToolTip(tr("Error in the file %1:\n%2")
                                                  .arg(esLayoutFile)
                                                  .arg(m_extraGlobalSettings->errorString()));
                    ui->extraSettings->setStyleSheet("*{background-color: #FF0000;}");
                    hasError = true;
                }
                auto *widget = m_extraGlobalSettings->getWidget();
                if(widget)
                {
                    ui->extraSettings->layout()->addWidget(widget);
                    JsonSettingsWidget::connect(m_extraGlobalSettings.get(),
                                                &JsonSettingsWidget::settingsChanged,
                                                this,
                                                callback);
                    spacerNeeded = spacerNeeded || m_extraGlobalSettings->spacerNeeded();
                }

                layoutFile.close();
            }
        }

        // Local object's extra settings
        if(!hasError && !esLayoutFile.isEmpty())
        {
            QFile layoutFile(esLayoutFile);
            if(layoutFile.open(QIODevice::ReadOnly))
            {
                QByteArray rawLayout = layoutFile.readAll();
                m_extraSettings.reset(new JsonSettingsWidget(ui->extraSettings));
                Q_ASSERT(m_extraSettings.get());

                m_extraSettings->setSearchDirectories(edit->LvlData.meta.path, edit->LvlData.meta.filename);
                m_extraSettings->setConfigPack(&mw()->configs);

                QJsonDocument branch = tree;
                if(errCode.error == QJsonParseError::NoError)
                {
                    QJsonObject root = branch.object();
                    auto local_key = root.find("local");
                    if(local_key != root.end())
                    {
                        auto local_val = local_key.value();
                        if(local_val.isObject())
                            branch.setObject(local_val.toObject());
                    } else {
                        root.remove("global");
                        branch.setObject(root);
                    }
                }

                if(!m_extraSettings->loadLayout(branch, rawLayout))
                {
                    LogWarning(m_extraSettings->errorString());
                    ui->extraSettings->setToolTip(tr("Error in the file %1:\n%2")
                                                  .arg(esLayoutFile)
                                                  .arg(m_extraSettings->errorString()));
                    ui->extraSettings->setStyleSheet("*{background-color: #FF0000;}");
                }
                auto *widget = m_extraSettings->getWidget();
                if(widget)
                {
                    ui->extraSettings->layout()->addWidget(widget);
                    JsonSettingsWidget::connect(m_extraSettings.get(),
                                                &JsonSettingsWidget::settingsChanged,
                                                this,
                                                callback);
                    spacerNeeded = spacerNeeded || m_extraSettings->spacerNeeded();
                }

                layoutFile.close();
            }
        }

        ui->extraSettings->setMinimumHeight(spacerNeeded ? 0 : 150);
        if(spacerNeeded)
            ui->extraSettings->layout()->addItem(m_extraSettingsSpacer.get());
    }
}

static QString extraGetSettings(JsonSettingsWidget *localWidget,
                                JsonSettingsWidget *globalWidget,
                                bool atRoot)
{
    QJsonObject l;
    QJsonObject g;
    QJsonObject mix;

    if(localWidget)
    {
        QJsonDocument local = localWidget->getSettings();
        l = local.object();
    }

    if(globalWidget)
    {
        QJsonDocument global = globalWidget->getSettings();
        g = global.object();
    }

    if(atRoot) // Store all local settings in the root
        mix = l;
    else // Otherwise, store them into "local" branch
        if(!l.isEmpty())
            mix["local"] = l;

    if(!g.isEmpty())
        mix["global"] = g;

    return QString::fromUtf8(QJsonDocument(mix).toJson());
}

void LvlItemProperties::onExtraSettingsBlockChanged()
{
    QString custom_params = extraGetSettings(m_extraSettings.get(),
                                             m_extraGlobalSettings.get(),
                                             mw()->configs.isExtraSettingsLocalAtRoot());
    if(m_currentBlockArrayId < 0)
        LvlPlacingItems::blockSet.meta.custom_params = custom_params;
    else
    if(mw()->activeChildWindow() == MainWindow::WND_Level)
    {
        LevelData selData;
        LevelEdit *edit = mw()->activeLvlEditWin();
        QList<QGraphicsItem *> items = edit->scene->selectedItems();
        edit->LvlData.meta.modified = true;
        foreach(QGraphicsItem *item, items)
        {
            if(item->data(ITEM_TYPE).toString() == "Block")
            {
                ItemBlock *n = qgraphicsitem_cast<ItemBlock*>(item);
                Q_ASSERT(n);
                selData.blocks.push_back(n->m_data);
                n->m_data.meta.custom_params = custom_params;
                n->arrayApply();
            }
        }
        edit->scene->m_history->addChangeSettings(selData, new BlockHistory_UserData(), custom_params);
    }
}

void LvlItemProperties::onExtraSettingsBGOChanged()
{
    QString custom_params = extraGetSettings(m_extraSettings.get(),
                                             m_extraGlobalSettings.get(),
                                             mw()->configs.isExtraSettingsLocalAtRoot());
    if(m_currentBgoArrayId < 0)
        LvlPlacingItems::bgoSet.meta.custom_params = custom_params;
    else
    if(mw()->activeChildWindow() == MainWindow::WND_Level)
    {
        LevelData selData;
        LevelEdit *edit = mw()->activeLvlEditWin();
        QList<QGraphicsItem *> items = edit->scene->selectedItems();
        edit->LvlData.meta.modified = true;
        foreach(QGraphicsItem *item, items)
        {
            if(item->data(ITEM_TYPE).toString() == "BGO")
            {
                ItemBGO *n = qgraphicsitem_cast<ItemBGO*>(item);
                Q_ASSERT(n);
                selData.bgo.push_back(n->m_data);
                n->m_data.meta.custom_params = custom_params;
                n->arrayApply();
            }
        }
        edit->scene->m_history->addChangeSettings(selData, new BgoHistory_UserData(), custom_params);
    }
}

void LvlItemProperties::onExtraSettingsNPCChanged()
{
    QString custom_params = extraGetSettings(m_extraSettings.get(),
                                             m_extraGlobalSettings.get(),
                                             mw()->configs.isExtraSettingsLocalAtRoot());
    if(m_currentNpcArrayId < 0)
        LvlPlacingItems::npcSet.meta.custom_params = custom_params;
    else
    if(mw()->activeChildWindow() == MainWindow::WND_Level)
    {
        LevelData selData;
        LevelEdit *edit = mw()->activeLvlEditWin();
        QList<QGraphicsItem *> items = edit->scene->selectedItems();
        edit->LvlData.meta.modified = true;
        foreach(QGraphicsItem *item, items)
        {
            if(item->data(ITEM_TYPE).toString() == "NPC")
            {
                ItemNPC *n = qgraphicsitem_cast<ItemNPC*>(item);
                Q_ASSERT(n);
                selData.npc.push_back(n->m_data);
                n->m_data.meta.custom_params = custom_params;
                n->arrayApply();
            }
        }
        edit->scene->m_history->addChangeSettings(selData, new NPCHistory_UserData(), custom_params);
    }
}


////////////////////////////////////////////////////////////////////////////////////////////

// ///////////BLOCKS///////////////////////////

void LvlItemProperties::on_PROPS_BlockResize_clicked()
{
    if(m_externalLock || m_internalLock)
        return;

    if(m_currentBlockArrayId < 0) return;

    if(mw()->activeChildWindow() == MainWindow::WND_Level)
    {
        QList<QGraphicsItem *> items = mw()->activeLvlEditWin()->scene->selectedItems();
        foreach(QGraphicsItem *item, items)
        {
            if((item->data(ITEM_TYPE).toString() == "Block") && ((item->data(ITEM_ARRAY_ID).toInt() == m_currentBlockArrayId)))
            {
                mw()->activeLvlEditWin()->scene->setBlockResizer(item, true);
                break;
            }
        }

    }

}


void LvlItemProperties::on_BLOCK_Width_editingFinished()
{
    if(m_externalLock || m_internalLock)
        return;

    if(m_currentBlockArrayId < 0)
        return;
    else if(mw()->activeChildWindow() == MainWindow::WND_Level)
    {
        LevelEdit *edit = mw()->activeLvlEditWin();
        QList<QGraphicsItem *> items = edit->scene->selectedItems();
        edit->LvlData.meta.modified = true;
        foreach(QGraphicsItem *item, items)
        {
            if(item->data(ITEM_TYPE).toString() == "Block")
            {
                ItemBlock *blk = ((ItemBlock *)item);
                if(blk->isSizable())
                {
                    QRect oldSize = QRect(blk->m_data.x, blk->m_data.y, blk->m_data.w, blk->m_data.h);
                    QRect newSize = QRect(blk->m_data.x, blk->m_data.y, ui->BLOCK_Width->value(), blk->m_data.h);
                    if(oldSize != newSize)
                    {
                        blk->setBlockSize(newSize);
                        edit->scene->m_history->addResizeBlock(blk->m_data,
                                                               oldSize.left(),
                                                               oldSize.top(),
                                                               oldSize.right(),
                                                               oldSize.bottom(),
                                                               newSize.left(),
                                                               newSize.top(),
                                                               newSize.right(),
                                                               newSize.bottom());
                    }
                }
            }
        }
    }
}

void LvlItemProperties::on_BLOCK_Height_editingFinished()
{
    if(m_externalLock || m_internalLock)
        return;

    if(m_currentBlockArrayId < 0)
        return;
    else if(mw()->activeChildWindow() == MainWindow::WND_Level)
    {
        LevelEdit *edit = mw()->activeLvlEditWin();
        QList<QGraphicsItem *> items = edit->scene->selectedItems();
        edit->LvlData.meta.modified = true;
        foreach(QGraphicsItem *item, items)
        {
            if(item->data(ITEM_TYPE).toString() == "Block")
            {
                ItemBlock *blk = qgraphicsitem_cast<ItemBlock *>(item);
                if(blk->isSizable())
                {
                    QRect oldSize = QRect(blk->m_data.x, blk->m_data.y, blk->m_data.w, blk->m_data.h);
                    QRect newSize = QRect(blk->m_data.x, blk->m_data.y, blk->m_data.w, ui->BLOCK_Height->value());
                    if(oldSize != newSize)
                    {
                        blk->setBlockSize(newSize);
                        edit->scene->m_history->addResizeBlock(blk->m_data,
                                                               oldSize.left(),
                                                               oldSize.top(),
                                                               oldSize.right(),
                                                               oldSize.bottom(),
                                                               newSize.left(),
                                                               newSize.top(),
                                                               newSize.right(),
                                                               newSize.bottom());
                    }
                }
            }
        }
    }

}


void LvlItemProperties::on_PROPS_BlockInvis_clicked(bool checked)
{
    if(m_externalLock || m_internalLock)
        return;

    if(m_currentBlockArrayId < 0)
        LvlPlacingItems::blockSet.invisible = checked;
    else if(mw()->activeChildWindow() == MainWindow::WND_Level)
    {
        LevelData selData;
        LevelEdit *edit = mw()->activeLvlEditWin();
        QList<QGraphicsItem *> items = edit->scene->selectedItems();
        edit->LvlData.meta.modified = true;
        foreach(QGraphicsItem *item, items)
        {
            if(item->data(ITEM_TYPE).toString() == "Block")
            {
                selData.blocks.push_back(((ItemBlock *) item)->m_data);
                ((ItemBlock *)item)->setInvisible(checked);
            }
        }
        edit->scene->m_history->addChangeSettings(selData, HistorySettings::SETTING_INVISIBLE, QVariant(checked));
    }

}

void LvlItemProperties::on_PROPS_BlkSlippery_clicked(bool checked)
{
    if(m_externalLock || m_internalLock)
        return;

    if(m_currentBlockArrayId < 0)
        LvlPlacingItems::blockSet.slippery = checked;
    else if(mw()->activeChildWindow() == MainWindow::WND_Level)
    {
        LevelData selData;
        LevelEdit *edit = mw()->activeLvlEditWin();
        QList<QGraphicsItem *> items = edit->scene->selectedItems();
        edit->LvlData.meta.modified = true;
        foreach(QGraphicsItem *item, items)
        {
            if((item->data(ITEM_TYPE).toString() == "Block")/*&&((item->data(2).toInt()==blockPtr))*/)
            {
                selData.blocks.push_back(((ItemBlock *) item)->m_data);
                ((ItemBlock *)item)->setSlippery(checked);
            }
        }
        edit->scene->m_history->addChangeSettings(selData, HistorySettings::SETTING_SLIPPERY, QVariant(checked));
    }

}

void LvlItemProperties::on_PROPS_BlockIncludes_clicked()
{
    if(m_externalLock || m_internalLock)
        return;

    int npcID = 0;

    if(m_currentBlockArrayId < 0)
        npcID = LvlPlacingItems::blockSet.npc_id;
    else if(mw()->activeChildWindow() == MainWindow::WND_Level)
    {
        QList<QGraphicsItem *> items1 = mw()->activeLvlEditWin()->scene->selectedItems();
        foreach(QGraphicsItem *targetItem, items1)
        {
            if((targetItem->data(ITEM_TYPE).toString() == "Block") && ((targetItem->data(ITEM_ARRAY_ID).toInt() == m_currentBlockArrayId)))
            {
                npcID = ((ItemBlock *)targetItem)->m_data.npc_id;
                break;
            }
        }
    }

    LevelData selData;

    ItemSelectDialog *npcList = new ItemSelectDialog(&mw()->configs, ItemSelectDialog::TAB_NPC,
            ItemSelectDialog::NPCEXTRA_WITHCOINS | (npcID < 0 && npcID != 0 ? ItemSelectDialog::NPCEXTRA_ISCOINSELECTED : 0), 0, 0,
            (npcID < 0 && npcID != 0 ? npcID * -1 : npcID), 0, 0, 0, 0, 0, this);
    util::DialogToCenter(npcList, true);

    if(npcList->exec() == QDialog::Accepted)
    {
        //apply to all selected items.
        int selected_npc = 0;
        if(npcList->npcID != 0)
        {
            if(npcList->isCoin)
                selected_npc = npcList->npcID * -1;
            else
                selected_npc = npcList->npcID;
        }


        ui->PROPS_BlockIncludes->setText(
            ((selected_npc != 0) ?
             ((selected_npc > 0) ? QString("NPC-%1").arg(selected_npc) : tr("%1 coins").arg(selected_npc * -1))
             : tr("[empty]")
            ));

        if(m_currentBlockArrayId < 1)
            LvlPlacingItems::blockSet.npc_id = selected_npc;
        else if(mw()->activeChildWindow() == MainWindow::WND_Level)
        {
            LevelEdit *edit = mw()->activeLvlEditWin();
            QList<QGraphicsItem *> items = edit->scene->selectedItems();
            edit->LvlData.meta.modified = true;
            foreach(QGraphicsItem *item, items)
            {
                if(item->data(ITEM_TYPE).toString() == "Block")
                {
                    selData.blocks.push_back(((ItemBlock *) item)->m_data);
                    ((ItemBlock *)item)->setIncludedNPC(selected_npc);
                }
            }
            edit->scene->m_history->addChangeSettings(selData, HistorySettings::SETTING_CHANGENPC, QVariant(selected_npc));
        }

    }
    delete npcList;

}

void LvlItemProperties::on_PROPS_BlockLayer_currentIndexChanged(const QString &arg1)
{
    if(m_externalLock || m_internalLock)
        return;

    if(m_currentBlockArrayId < 0)
    {
        LvlPlacingItems::blockSet.layer = arg1;
        LvlPlacingItems::layer = arg1;
    }
    else if(mw()->activeChildWindow() == MainWindow::WND_Level)
    {
        LevelData modData;
        LevelEdit *edit = mw()->activeLvlEditWin();
        QList<QGraphicsItem *> items = edit->scene->selectedItems();
        edit->LvlData.meta.modified = true;
        foreach(QGraphicsItem *item, items)
        {
            if(item->data(ITEM_TYPE).toString() == "Block")
            {
                modData.blocks.push_back(((ItemBlock *)item)->m_data);
                ((ItemBlock *)item)->setLayer(arg1);
            }
        }
        edit->scene->m_history->addChangedLayer(modData, arg1);
    }

}

void LvlItemProperties::on_PROPS_BlkEventDestroy_currentIndexChanged(const QString &arg1)
{
    if(m_externalLock || m_internalLock)
        return;

    if(m_currentBlockArrayId < 0)
    {
        if(ui->PROPS_BlkEventDestroy->currentIndex() > 0)
            LvlPlacingItems::blockSet.event_destroy = arg1;
        else
            LvlPlacingItems::blockSet.event_destroy = "";

        m_recentBlockEventDestroy = LvlPlacingItems::blockSet.event_destroy;
    }
    else if(mw()->activeChildWindow() == MainWindow::WND_Level)
    {
        LevelData modData;
        LevelEdit *edit = mw()->activeLvlEditWin();
        QList<QGraphicsItem *> items = edit->scene->selectedItems();
        edit->LvlData.meta.modified = true;
        foreach(QGraphicsItem *item, items)
        {
            if((item->data(ITEM_TYPE).toString() == "Block")/*&&((item->data(2).toInt()==blockPtr))*/)
            {
                modData.blocks.push_back(((ItemBlock *)item)->m_data);
                if(ui->PROPS_BlkEventDestroy->currentIndex() > 0)
                    ((ItemBlock *)item)->m_data.event_destroy = arg1;
                else
                    ((ItemBlock *)item)->m_data.event_destroy = "";
                ((ItemBlock *)item)->arrayApply();
                //break;
            }
        }
        if(ui->PROPS_BlkEventDestroy->currentIndex() > 0)
            edit->scene->m_history->addChangeSettings(modData, HistorySettings::SETTING_EV_DESTROYED, QVariant(arg1));
        else
            edit->scene->m_history->addChangeSettings(modData, HistorySettings::SETTING_EV_DESTROYED, QVariant(""));
    }

}

void LvlItemProperties::on_PROPS_BlkEventHited_currentIndexChanged(const QString &arg1)
{
    if(m_externalLock || m_internalLock)
        return;

    if(m_currentBlockArrayId < 0)
    {
        if(ui->PROPS_BlkEventHited->currentIndex() > 0)
            LvlPlacingItems::blockSet.event_hit = arg1;
        else
            LvlPlacingItems::blockSet.event_hit = "";

        m_recentBlockEventHit = LvlPlacingItems::blockSet.event_hit;
    }
    else if(mw()->activeChildWindow() == MainWindow::WND_Level)
    {
        LevelData modData;
        LevelEdit *edit = mw()->activeLvlEditWin();
        QList<QGraphicsItem *> items = edit->scene->selectedItems();
        edit->LvlData.meta.modified = true;
        foreach(QGraphicsItem *item, items)
        {
            if((item->data(ITEM_TYPE).toString() == "Block")/*&&((item->data(2).toInt()==blockPtr))*/)
            {
                modData.blocks.push_back(((ItemBlock *)item)->m_data);
                if(ui->PROPS_BlkEventHited->currentIndex() > 0)
                    ((ItemBlock *)item)->m_data.event_hit = arg1;
                else
                    ((ItemBlock *)item)->m_data.event_hit = "";
                ((ItemBlock *)item)->arrayApply();
                //break;
            }
        }
        if(ui->PROPS_BlkEventHited->currentIndex() > 0)
            edit->scene->m_history->addChangeSettings(modData, HistorySettings::SETTING_EV_HITED, QVariant(arg1));
        else
            edit->scene->m_history->addChangeSettings(modData, HistorySettings::SETTING_EV_HITED, QVariant(""));
    }

}

void LvlItemProperties::on_PROPS_BlkEventLayerEmpty_currentIndexChanged(const QString &arg1)
{
    if(m_externalLock || m_internalLock)
        return;

    if(m_currentBlockArrayId < 0)
    {
        if(ui->PROPS_BlkEventLayerEmpty->currentIndex() > 0)
            LvlPlacingItems::blockSet.event_emptylayer = arg1;
        else
            LvlPlacingItems::blockSet.event_emptylayer = "";
        m_recentBlockEventLayerEmpty = LvlPlacingItems::blockSet.event_emptylayer;
    }

    else if(mw()->activeChildWindow() == MainWindow::WND_Level)
    {
        LevelData modData;
        LevelEdit *edit = mw()->activeLvlEditWin();
        QList<QGraphicsItem *> items = edit->scene->selectedItems();
        edit->LvlData.meta.modified = true;
        foreach(QGraphicsItem *item, items)
        {
            if((item->data(ITEM_TYPE).toString() == "Block")/*&&((item->data(2).toInt()==blockPtr))*/)
            {
                modData.blocks.push_back(((ItemBlock *)item)->m_data);
                if(ui->PROPS_BlkEventLayerEmpty->currentIndex() > 0)
                    ((ItemBlock *)item)->m_data.event_emptylayer = arg1;
                else
                    ((ItemBlock *)item)->m_data.event_emptylayer = "";
                ((ItemBlock *)item)->arrayApply();
                //break;
            }
        }
        if(ui->PROPS_BlkEventLayerEmpty->currentIndex() > 0)
            edit->scene->m_history->addChangeSettings(modData, HistorySettings::SETTING_EV_LAYER_EMP, QVariant(arg1));
        else
            edit->scene->m_history->addChangeSettings(modData, HistorySettings::SETTING_EV_LAYER_EMP, QVariant(""));
    }

}



////////////////////////////////////////////////////////////////////////////////////////////

// ///////////BGO///////////////////////////

void LvlItemProperties::on_PROPS_BGOLayer_currentIndexChanged(const QString &arg1)
{
    if(m_externalLock || m_internalLock)
        return;

    if(m_currentBgoArrayId < 0)
    {
        LvlPlacingItems::bgoSet.layer = arg1;
        LvlPlacingItems::layer = arg1;
    }
    else if(mw()->activeChildWindow() == MainWindow::WND_Level)
    {
        LevelData modData;
        LevelEdit *edit = mw()->activeLvlEditWin();
        QList<QGraphicsItem *> items = edit->scene->selectedItems();
        edit->LvlData.meta.modified = true;
        foreach(QGraphicsItem *item, items)
        {
            if((item->data(ITEM_TYPE).toString() == "BGO")/*&&((item->data(2).toInt()==bgoPtr))*/)
            {
                modData.bgo.push_back(((ItemBGO *)item)->m_data);
                ((ItemBGO *)item)->setLayer(arg1);
                //break;
            }
        }
        edit->scene->m_history->addChangedLayer(modData, arg1);
    }

}

void LvlItemProperties::on_PROPS_BGO_Z_Layer_currentIndexChanged(int index)
{
    if(m_externalLock || m_internalLock)
        return;

    int zMode = 0;
    switch(index)
    {
    case 0:
        zMode = LevelBGO::Background2;
        break;
    case 1:
        zMode = LevelBGO::Background1;
        break;
    case 3:
        zMode = LevelBGO::Foreground1;
        break;
    case 4:
        zMode = LevelBGO::Foreground2;
        break;
    case 2:
    default:
        zMode = LevelBGO::ZDefault;
        break;
    }

    if(m_currentBgoArrayId < 0)
        LvlPlacingItems::bgoSet.z_mode = zMode;
    else if(mw()->activeChildWindow() == MainWindow::WND_Level)
    {
        LevelData selData;
        LevelEdit *edit = mw()->activeLvlEditWin();
        QList<QGraphicsItem *> items = edit->scene->selectedItems();
        edit->LvlData.meta.modified = true;
        foreach(QGraphicsItem *item, items)
        {
            if((item->data(ITEM_TYPE).toString() == "BGO")/*&&((item->data(2).toInt()==bgoPtr))*/)
            {
                selData.bgo.push_back(((ItemBGO *)item)->m_data);
                ((ItemBGO *)item)->setZMode(zMode, ((ItemBGO *)item)->m_data.z_offset);
                //((ItemBGO*)item)->arrayApply();
                //break;
            }
        }
        edit->scene->m_history->addChangeSettings(selData, HistorySettings::SETTING_Z_LAYER, QVariant(zMode));
    }


}

void LvlItemProperties::on_PROPS_BGO_Z_Offset_valueChanged(double arg1)
{
    if(m_externalLock || m_internalLock)
        return;

    if(m_currentBgoArrayId < 0)
        LvlPlacingItems::bgoSet.z_offset = arg1;
    else if(mw()->activeChildWindow() == MainWindow::WND_Level)
    {
        LevelData selData;
        LevelEdit *edit = mw()->activeLvlEditWin();
        QList<QGraphicsItem *> items = edit->scene->selectedItems();
        edit->LvlData.meta.modified = true;
        foreach(QGraphicsItem *item, items)
        {
            if((item->data(ITEM_TYPE).toString() == "BGO")/*&&((item->data(2).toInt()==bgoPtr))*/)
            {
                selData.bgo.push_back(((ItemBGO *)item)->m_data);
                ((ItemBGO *)item)->setZMode(((ItemBGO *)item)->m_data.z_mode, arg1);
                //((ItemBGO*)item)->arrayApply();
                //break;
            }
        }
        edit->scene->m_history->addChangeSettings(selData, HistorySettings::SETTING_Z_OFFSET, QVariant(arg1));
    }
}

void LvlItemProperties::on_PROPS_BGO_smbx64_sp_valueChanged(int arg1)
{
    if(m_externalLock || m_internalLock)
        return;

    if(m_currentBgoArrayId < 0)
        LvlPlacingItems::bgoSet.smbx64_sp = arg1;
    else if(mw()->activeChildWindow() == MainWindow::WND_Level)
    {
        LevelData selData;
        LevelEdit *edit = mw()->activeLvlEditWin();
        QList<QGraphicsItem *> items = edit->scene->selectedItems();
        edit->LvlData.meta.modified = true;
        foreach(QGraphicsItem *item, items)
        {
            if((item->data(ITEM_TYPE).toString() == "BGO")/*&&((item->data(2).toInt()==bgoPtr))*/)
            {
                selData.bgo.push_back(((ItemBGO *)item)->m_data);
                ((ItemBGO *)item)->m_data.smbx64_sp = arg1;
                ((ItemBGO *)item)->arrayApply();
                //break;
            }
        }
        edit->scene->m_history->addChangeSettings(selData, HistorySettings::SETTING_BGOSORTING, QVariant(arg1));
    }
}


////////////////////////////////////////////////////////////////////////////////////////////

void LvlItemProperties::refreshFirstNpcSpecialOption(LevelNPC &npc, bool newItem, bool dont_reset_props)
{
    obj_npc &t_npc = getNpcProps(npc, mw());

    ui->PROPS_NPCSpecialNPC->hide();
    ui->PROPS_NPCNpcLabel->hide();

    ui->PROPS_NPCBoxLabel->hide();
    ui->PROPS_NPCSpecialBox->hide();

    ui->PROPS_NpcSpinLabel->hide();
    ui->PROPS_NPCSpecialSpin->hide();
    ui->PROPS_NPCSpecialSpin_Auto->hide();

    ui->line_6->hide();

    //; 0 combobox, 1 - spin, 2 - npc-id
    if(t_npc.setup.special_option)
    {
        ui->line_6->show();
        switch(t_npc.setup.special_type)
        {
        case 0: // Combobox
            ui->PROPS_NPCBoxLabel->show();
            ui->PROPS_NPCBoxLabel->setText(t_npc.setup.special_name);
            ui->PROPS_NPCSpecialBox->show();

            if((newItem) && (!dont_reset_props))
            {
                //Reset value to min, if it out of range
                if((npc.special_data >= t_npc.setup.special_combobox_opts.size()) ||
                   (npc.special_data < 0))
                {
                    npc.special_data  = 0;
                    npc.special_data2 = 0;
                    LvlPlacingItems::npcSet.special_data  = npc.special_data;
                    LvlPlacingItems::npcSet.special_data2 = npc.special_data2;
                }
            }

            ui->PROPS_NPCSpecialBox->clear();
            for(int i = 0; i < t_npc.setup.special_combobox_opts.size(); i++)
            {
                ui->PROPS_NPCSpecialBox->addItem(t_npc.setup.special_combobox_opts[i]);
                if(i == npc.special_data)
                    ui->PROPS_NPCSpecialBox->setCurrentIndex(i);
            }

            break;
        case 1: // Spinbox
            ui->PROPS_NpcSpinLabel->show();
            ui->PROPS_NpcSpinLabel->setText(t_npc.setup.special_name);
            ui->PROPS_NPCSpecialSpin->show();

            if(m_currentNpcArrayId < 0)
            {
                ui->PROPS_NPCSpecialSpin_Auto->setVisible(t_npc.setup.special_spin_allow_autoincrement);
                if(!dont_reset_props)
                    ui->PROPS_NPCSpecialSpin_Auto->setChecked(false);
            }

            if((newItem) && (!dont_reset_props))
            {
                //Reset value to min, if it out of range
                if((npc.special_data > t_npc.setup.special_spin_max) ||
                   (npc.special_data < t_npc.setup.special_spin_min))
                {
                    LvlPlacingItems::npcSet.special_data = t_npc.setup.special_spin_min;
                    npc.special_data = t_npc.setup.special_spin_min;
                }
            }

            m_npcSpecSpinOffset = t_npc.setup.special_spin_value_offset;
            ui->PROPS_NPCSpecialSpin->setMinimum(t_npc.setup.special_spin_min + m_npcSpecSpinOffset);
            ui->PROPS_NPCSpecialSpin->setMaximum(t_npc.setup.special_spin_max + m_npcSpecSpinOffset);

            ui->PROPS_NPCSpecialSpin->setValue(npc.special_data + m_npcSpecSpinOffset);
            LvlPlacingItems::npcSpecialAutoIncrement_begin = npc.special_data;

            break;
        case 2: // NPC-ID selector
            ui->PROPS_NPCNpcLabel->show();
            ui->PROPS_NPCNpcLabel->setText(t_npc.setup.special_name);
            ui->PROPS_NPCSpecialNPC->show();
            ui->PROPS_NPCSpecialNPC->setText(
                ((npc.special_data > 0) ? QString("NPC-%1").arg(npc.special_data)
                 : tr("[empty]")
                ));
            break;
        default:
            break;
        }
    }
}

void LvlItemProperties::on_PROPS_NPCDirLeft_clicked()
{
    if(m_externalLock || m_internalLock)
        return;

    if(m_currentNpcArrayId < 0)
    {
        LvlPlacingItems::npcSet.direct = -1;

        if(mw()->activeChildWindow() == MainWindow::WND_Level)
            mw()->activeLvlEditWin()->scene->updateCursoredNpcDirection();
    }
    else if(mw()->activeChildWindow() == MainWindow::WND_Level)
    {
        LevelData selData;
        LevelEdit *edit = mw()->activeLvlEditWin();
        QList<QGraphicsItem *> items = edit->scene->selectedItems();
        edit->LvlData.meta.modified = true;
        foreach(QGraphicsItem *item, items)
        {
            if((item->data(ITEM_TYPE).toString() == "NPC")/*&&((item->data(2).toInt()==npcPtr))*/)
            {
                selData.npc.push_back(((ItemNPC *)item)->m_data);
                ((ItemNPC *)item)->changeDirection(-1);
                //break;
            }
        }
        edit->scene->m_history->addChangeSettings(selData, HistorySettings::SETTING_DIRECTION, QVariant(-1));
    }

}

void LvlItemProperties::on_PROPS_NPCDirRand_clicked()
{
    if(m_externalLock || m_internalLock)
        return;

    if(m_currentNpcArrayId < 0)
    {
        LvlPlacingItems::npcSet.direct = 0;

        if(mw()->activeChildWindow() == MainWindow::WND_Level)
            mw()->activeLvlEditWin()->scene->updateCursoredNpcDirection();
    }
    else if(mw()->activeChildWindow() == MainWindow::WND_Level)
    {
        LevelData selData;
        LevelEdit *edit = mw()->activeLvlEditWin();
        QList<QGraphicsItem *> items = edit->scene->selectedItems();
        edit->LvlData.meta.modified = true;
        for(QGraphicsItem *item : items)
        {
            if((item->data(ITEM_TYPE).toString() == "NPC")/*&&((item->data(2).toInt()==npcPtr))*/)
            {
                selData.npc.push_back(((ItemNPC *)item)->m_data);
                ((ItemNPC *)item)->changeDirection(0);

                //break;
            }
        }
        edit->scene->m_history->addChangeSettings(selData, HistorySettings::SETTING_DIRECTION, QVariant(0));
    }
}

void LvlItemProperties::on_PROPS_NPCDirRight_clicked()
{
    if(m_externalLock || m_internalLock)
        return;

    if(m_currentNpcArrayId < 0)
    {
        LvlPlacingItems::npcSet.direct = 1;

        if(mw()->activeChildWindow() == MainWindow::WND_Level)
            mw()->activeLvlEditWin()->scene->updateCursoredNpcDirection();

    }
    else if(mw()->activeChildWindow() == MainWindow::WND_Level)
    {
        LevelData selData;
        LevelEdit *edit = mw()->activeLvlEditWin();
        QList<QGraphicsItem *> items = edit->scene->selectedItems();
        edit->LvlData.meta.modified = true;
        for(QGraphicsItem *item : items)
        {
            if((item->data(ITEM_TYPE).toString() == "NPC")/*&&((item->data(2).toInt()==npcPtr))*/)
            {
                selData.npc.push_back(((ItemNPC *)item)->m_data);
                ((ItemNPC *)item)->changeDirection(1);
                //break;
            }
        }
        edit->scene->m_history->addChangeSettings(selData, HistorySettings::SETTING_DIRECTION, QVariant(1));
    }
}

void LvlItemProperties::on_PROPS_NpcFri_clicked(bool checked)
{
    if(m_externalLock || m_internalLock)
        return;

    if(m_currentNpcArrayId < 0)
        LvlPlacingItems::npcSet.friendly = checked;
    else if(mw()->activeChildWindow() == MainWindow::WND_Level)
    {
        LevelData selData;
        LevelEdit *edit = mw()->activeLvlEditWin();
        QList<QGraphicsItem *> items = edit->scene->selectedItems();
        edit->LvlData.meta.modified = true;
        foreach(QGraphicsItem *item, items)
        {
            if((item->data(ITEM_TYPE).toString() == "NPC")/*&&((item->data(2).toInt()==npcPtr))*/)
            {
                selData.npc.push_back(((ItemNPC *)item)->m_data);
                ((ItemNPC *)item)->setFriendly(checked);
                //break;
            }
        }
        edit->scene->m_history->addChangeSettings(selData, HistorySettings::SETTING_FRIENDLY, QVariant(checked));
    }

}

void LvlItemProperties::on_PROPS_NPCNoMove_clicked(bool checked)
{
    if(m_externalLock || m_internalLock)
        return;

    if(m_currentNpcArrayId < 0)
        LvlPlacingItems::npcSet.nomove = checked;
    else if(mw()->activeChildWindow() == MainWindow::WND_Level)
    {
        LevelData selData;
        LevelEdit *edit = mw()->activeLvlEditWin();
        QList<QGraphicsItem *> items = edit->scene->selectedItems();
        edit->LvlData.meta.modified = true;
        foreach(QGraphicsItem *item, items)
        {
            if((item->data(ITEM_TYPE).toString() == "NPC")/*&&((item->data(2).toInt()==npcPtr))*/)
            {
                selData.npc.push_back(((ItemNPC *)item)->m_data);
                ((ItemNPC *)item)->setNoMovable(checked);
                //break;
            }
        }
        edit->scene->m_history->addChangeSettings(selData, HistorySettings::SETTING_NOMOVEABLE, QVariant(checked));
    }

}

void LvlItemProperties::on_PROPS_NpcBoss_clicked(bool checked)
{
    if(m_externalLock || m_internalLock)
        return;

    if(m_currentNpcArrayId < 0)
        LvlPlacingItems::npcSet.is_boss = checked;
    else if(mw()->activeChildWindow() == MainWindow::WND_Level)
    {
        LevelData selData;
        LevelEdit *edit = mw()->activeLvlEditWin();
        QList<QGraphicsItem *> items = edit->scene->selectedItems();
        edit->LvlData.meta.modified = true;
        foreach(QGraphicsItem *item, items)
        {
            if((item->data(ITEM_TYPE).toString() == "NPC")/*&&((item->data(2).toInt()==npcPtr))*/)
            {
                selData.npc.push_back(((ItemNPC *)item)->m_data);
                ((ItemNPC *)item)->setLegacyBoss(checked);
                //break;
            }
        }
        edit->scene->m_history->addChangeSettings(selData, HistorySettings::SETTING_BOSS, QVariant(checked));
    }
}

void LvlItemProperties::on_PROPS_NpcTMsg_clicked()
{
    if(m_externalLock || m_internalLock)
        return;

    //LevelData selData;
    //QList<QVariant> modText;
    //modText.push_back(QVariant(npcData.msg));

    QString message;
    bool friendly = false;
    if(m_currentNpcArrayId < 0)
    {
        message = LvlPlacingItems::npcSet.msg;
        friendly = LvlPlacingItems::npcSet.friendly;
    }
    else if(mw()->activeChildWindow() == MainWindow::WND_Level)
    {
        LevelEdit *edit = mw()->activeLvlEditWin();
        QList<QGraphicsItem *> items = edit->scene->selectedItems();
        edit->LvlData.meta.modified = true;
        foreach(QGraphicsItem *SelItem, items)
        {
            if(SelItem->data(ITEM_TYPE).toString() == "NPC")
            {
                message = ((ItemNPC *) SelItem)->m_data.msg;
                friendly = ((ItemNPC *) SelItem)->m_data.friendly;
                break;
            }
        }
    }

    ItemMsgBox *msgBox = new ItemMsgBox(Opened_By::NPC, message, friendly, "", "", this);
    util::DialogToCenter(msgBox, true);

    if(msgBox->exec() == QDialog::Accepted)
    {

        if(m_currentNpcArrayId < 1)
        {
            LvlPlacingItems::npcSet.msg = msgBox->currentText;
            LvlPlacingItems::npcSet.friendly = msgBox->isFriendlyChecked();
        }
        else if(mw()->activeChildWindow() == MainWindow::WND_Level)
        {
            LevelData selData;
            LevelEdit *edit = mw()->activeLvlEditWin();
            QList<QGraphicsItem *> items = edit->scene->selectedItems();
            edit->LvlData.meta.modified = true;
            foreach(QGraphicsItem *SelItem, items)
            {
                if(SelItem->data(ITEM_TYPE).toString() == "NPC")
                {
                    selData.npc.push_back(((ItemNPC *) SelItem)->m_data);
                    ((ItemNPC *) SelItem)->setMsg(msgBox->currentText);
                    ((ItemNPC *) SelItem)->setFriendly(msgBox->isFriendlyChecked());
                }
            }
            edit->scene->m_history->addChangeSettings(selData, HistorySettings::SETTING_MESSAGE, QVariant(msgBox->currentText));
            edit->scene->m_history->addChangeSettings(selData, HistorySettings::SETTING_FRIENDLY, QVariant(msgBox->isFriendlyChecked()));
        }

        QString npcmsg = (msgBox->currentText.isEmpty() ? tr("[none]") : msgBox->currentText);
        if(npcmsg.size() > 20)
        {
            npcmsg.resize(18);
            npcmsg.push_back("...");
        }
        ui->PROPS_NpcTMsg->setText(npcmsg.replace("&", "&&&"));
        ui->PROPS_NpcFri->setChecked(msgBox->isFriendlyChecked());
    }
    delete msgBox;

}

void LvlItemProperties::on_PROPS_NPCSpecialSpin_valueChanged(int arg1)
{
    if(m_externalLock || m_internalLock)
        return;

    LvlPlacingItems::npcSpecialAutoIncrement_begin = arg1 - m_npcSpecSpinOffset;

    if(m_currentNpcArrayId < 0)
    {
        LvlPlacingItems::npcSet.special_data = arg1 - m_npcSpecSpinOffset;
        return;
    }

    if(mw()->activeChildWindow() != MainWindow::WND_Level)
        return;

    LevelData selData;
    LevelEdit *edit = mw()->activeLvlEditWin();
    QList<QGraphicsItem *> items = edit->scene->selectedItems();
    edit->LvlData.meta.modified = true;
    for(QGraphicsItem *item : items)
    {
        if(item->data(ITEM_TYPE).toString() == "NPC")
        {
            LevelNPC npc = ((ItemNPC *)item)->m_data;

            //Inherit preferences from contained NPC if this NPC a container
            obj_npc &t_npc = getNpcProps(npc, mw());

            if(t_npc.setup.special_type != 1) //wrong type, go to next one
                continue;

            if(t_npc.setup.special_spin_value_offset != m_npcSpecSpinOffset) //wrong offset, go to next one
                continue;

            selData.npc.push_back(((ItemNPC *)item)->m_data);
            ((ItemNPC *)item)->m_data.special_data = arg1 - m_npcSpecSpinOffset;
            ((ItemNPC *)item)->arrayApply();
        }
    }
    edit->scene->m_history->addChangeSettings(selData, HistorySettings::SETTING_SPECIAL_DATA, QVariant(arg1 - m_npcSpecSpinOffset));
}

void LvlItemProperties::on_PROPS_NPCSpecialSpin_Auto_clicked(bool checked)
{
    LvlPlacingItems::npcSpecialAutoIncrement = checked;
    LvlPlacingItems::npcSet.special_data = ui->PROPS_NPCSpecialSpin->value() - m_npcSpecSpinOffset;

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
    on_PROPS_NPCSpecialSpin_Auto_clicked(checked);
}

void LvlItemProperties::processNpcContainerButton(QPushButton *btn)
{
    int npcID = 0;

    if(m_currentNpcArrayId < 0)
    {
        npcID = LvlPlacingItems::npcSet.contents;
    }
    else if(mw()->activeChildWindow() == MainWindow::WND_Level)
    {
        QList<QGraphicsItem *> items1 = mw()->activeLvlEditWin()->scene->selectedItems();
        for(QGraphicsItem *targetItem : items1)
        {
            if((targetItem->data(ITEM_TYPE).toString() == "NPC") && ((targetItem->data(ITEM_ARRAY_ID).toInt() == m_currentNpcArrayId)))
            {
                npcID = ((ItemNPC *)targetItem)->m_data.contents;
                break;
            }
        }
    }

    ItemSelectDialog *npcList = new ItemSelectDialog(&mw()->configs, ItemSelectDialog::TAB_NPC, 0, 0, 0, npcID, 0, 0, 0, 0, 0, this);
    util::DialogToCenter(npcList, true);
    if(npcList->exec() == QDialog::Accepted)
    {
        int selected_npc = npcList->npcID;
        btn->setText(
            ((selected_npc > 0) ? QString("NPC-%1").arg(selected_npc)
             : tr("[empty]")
            ));

        if(m_currentNpcArrayId < 1)
        {
            if(ui->PROPS_NPCContaiter == btn)
            {
                LvlPlacingItems::npcSet.contents = selected_npc;
                m_internalLock = true;
                refreshFirstNpcSpecialOption(LvlPlacingItems::npcSet);
                m_internalLock = false;
            }
            else if(ui->PROPS_NPCSpecialNPC == btn)
                LvlPlacingItems::npcSet.special_data = selected_npc;
        }
        else if(mw()->activeChildWindow() == MainWindow::WND_Level)
        {
            LevelData selData;
            LevelNPC npc;
            LevelEdit *edit = mw()->activeLvlEditWin();
            QList<QGraphicsItem *> items = edit->scene->selectedItems();
            edit->LvlData.meta.modified = true;
            foreach(QGraphicsItem *item, items)
            {
                if((item->data(ITEM_TYPE).toString() == "NPC")/*&&((item->data(ITEM_ARRAY_ID).toInt()==blockPtr))*/)
                {
                    ItemNPC *npcI = ((ItemNPC *)item);
                    selData.npc.push_back(npcI->m_data);
                    if(ui->PROPS_NPCContaiter == btn)
                        npcI->setIncludedNPC(selected_npc);
                    else if(ui->PROPS_NPCSpecialNPC == btn)
                        npcI->m_data.special_data = selected_npc;
                    npc = npcI->m_data;
                }
            }
            edit->scene->m_history->addChangeSettings(selData,
                    ((ui->PROPS_NPCContaiter == btn) ?
                     HistorySettings::SETTING_CHANGENPC :
                     HistorySettings::SETTING_SPECIAL_DATA),
                    QVariant(selected_npc));
            m_internalLock = true;
            if(items.size() > 0)
                refreshFirstNpcSpecialOption(npc);
            m_internalLock = false;
        }
    }
    delete npcList;
}

void LvlItemProperties::on_PROPS_NPCContaiter_clicked()
{
    if(m_externalLock || m_internalLock)
        return;

    processNpcContainerButton(ui->PROPS_NPCContaiter);
}

void LvlItemProperties::on_PROPS_NPCSpecialNPC_clicked()
{
    if(m_externalLock || m_internalLock)
        return;

    processNpcContainerButton(ui->PROPS_NPCSpecialNPC);
}

void LvlItemProperties::on_PROPS_NPCSpecialBox_currentIndexChanged(int index)
{
    if(m_externalLock || m_internalLock)
        return;

    if(m_currentNpcArrayId < 0)
        LvlPlacingItems::npcSet.special_data = index;
    else if(mw()->activeChildWindow() == MainWindow::WND_Level)
    {
        LevelData selData;
        LevelEdit *edit = mw()->activeLvlEditWin();
        QList<QGraphicsItem *> items = edit->scene->selectedItems();
        edit->LvlData.meta.modified = true;
        foreach(QGraphicsItem *item, items)
        {
            if(item->data(ITEM_TYPE).toString() == "NPC")
            {
                LevelNPC npc = ((ItemNPC *)item)->m_data;

                //Inherit preferences from contained NPC if this NPC a container
                obj_npc &t_npc = getNpcProps(npc, mw());

                if(t_npc.setup.special_type != 0) //wrong type, go to next one
                    continue;

                selData.npc.push_back(((ItemNPC *)item)->m_data);
                ((ItemNPC *)item)->m_data.special_data = index;
                ((ItemNPC *)item)->arrayApply();
            }
        }
        edit->scene->m_history->addChangeSettings(selData, HistorySettings::SETTING_SPECIAL_DATA, QVariant(index));
    }
}

void LvlItemProperties::on_PROPS_NpcGenerator_clicked(bool checked)
{
    if(m_externalLock || m_internalLock)
        return;

    if(m_currentNpcArrayId < 0)
    {
        LvlPlacingItems::npcSet.generator = checked;
        if(checked)
            LvlPlacingItems::gridSz = 16;
        else
            LvlPlacingItems::gridSz = static_cast<int>(LvlPlacingItems::npcGrid);

        m_externalLock = true;

        ui->PROPS_NPCGenType->setCurrentIndex((LvlPlacingItems::npcSet.generator_type) - 1);
        ui->PROPS_NPCGenTime->setValue((double)(LvlPlacingItems::npcSet.generator_period) / 10);
        ui->npcGeneratorDirection->setDirection(LvlPlacingItems::npcSet.generator_direct);

        m_externalLock = false;

    }
    else if(mw()->activeChildWindow() == MainWindow::WND_Level)
    {
        LevelData modData;
        LevelEdit *edit = mw()->activeLvlEditWin();
        QList<QGraphicsItem *> items = edit->scene->selectedItems();
        edit->LvlData.meta.modified = true;
        foreach(QGraphicsItem *item, items)
        {
            if(item->data(ITEM_TYPE).toString() == "NPC")
            {
                auto *it = qgraphicsitem_cast<ItemNPC *>(item);
                Q_ASSERT(it);
                modData.npc.push_back(it->m_data);
                it->setGenerator(checked, it->m_data.generator_direct, it->m_data.generator_type);
                m_externalLock = true;
                ui->PROPS_NPCGenType->setCurrentIndex(it->m_data.generator_type - 1);
                ui->PROPS_NPCGenTime->setValue(double(it->m_data.generator_period / 10));
                ui->npcGeneratorDirection->setDirection(it->m_data.generator_direct);
                m_externalLock = false;
            }
        }
        edit->scene->m_history->addChangeSettings(modData, HistorySettings::SETTING_GENACTIVATE, QVariant(checked));
    }
    ui->PROPS_NPCGenBox->setVisible(checked);
}

void LvlItemProperties::on_PROPS_NPCGenType_currentIndexChanged(int index)
{
    if(m_externalLock || m_internalLock)
        return;

    if(m_currentNpcArrayId < 0)
        LvlPlacingItems::npcSet.generator_type = index + 1;
    else if(mw()->activeChildWindow() == MainWindow::WND_Level)
    {
        LevelData modData;
        LevelEdit *edit = mw()->activeLvlEditWin();
        QList<QGraphicsItem *> items = edit->scene->selectedItems();
        edit->LvlData.meta.modified = true;
        foreach(QGraphicsItem *item, items)
        {
            if(item->data(ITEM_TYPE).toString() == "NPC")
            {
                modData.npc.push_back(((ItemNPC *)item)->m_data);
                ((ItemNPC *)item)->setGenerator(((ItemNPC *)item)->m_data.generator,
                                                ((ItemNPC *)item)->m_data.generator_direct,
                                                index + 1
                                               );
            }
        }
        edit->scene->m_history->addChangeSettings(modData, HistorySettings::SETTING_GENTYPE, QVariant(index + 1));
    }
}

void LvlItemProperties::on_PROPS_NPCGenTime_valueChanged(double arg1)
{
    if(m_externalLock || m_internalLock)
        return;

    if(m_currentNpcArrayId < 0)
        LvlPlacingItems::npcSet.generator_period = qRound(arg1 * 10);
    else if(mw()->activeChildWindow() == MainWindow::WND_Level)
    {
        LevelData modData;
        LevelEdit *edit = mw()->activeLvlEditWin();
        QList<QGraphicsItem *> items = edit->scene->selectedItems();
        edit->LvlData.meta.modified = true;
        for(QGraphicsItem *item : items)
        {
            if(item->data(ITEM_TYPE).toString() != "NPC")
                continue;
            auto *it = qgraphicsitem_cast<ItemNPC*>(item);
            Q_ASSERT(it);
            modData.npc.push_back(it->m_data);
            it->m_data.generator_period = qRound(arg1 * 10);
            it->arrayApply();
        }
        edit->scene->m_history->addChangeSettings(modData, HistorySettings::SETTING_GENTIME, QVariant(qRound(arg1 * 10)));
    }
}

void LvlItemProperties::npcGeneratorDirectionChange(int direction)
{
    if(m_externalLock || m_internalLock)
        return;

    if(m_currentNpcArrayId < 0)
    {
        LvlPlacingItems::npcSet.generator_direct = direction;
        return;
    }

    if(mw()->activeChildWindow() != MainWindow::WND_Level)
        return;

    LevelData modData;
    LevelEdit *edit = mw()->activeLvlEditWin();
    QList<QGraphicsItem *> items = edit->scene->selectedItems();
    edit->LvlData.meta.modified = true;

    for(QGraphicsItem *item : items)
    {
        if(item->data(ITEM_TYPE).toString() != "NPC")
            continue;
        ItemNPC *it = qgraphicsitem_cast<ItemNPC*>(item);
        Q_ASSERT(it);
        modData.npc.push_back(it->m_data);
        it->setGenerator(it->m_data.generator, direction, it->m_data.generator_type);
    }
    edit->scene->m_history->addChangeSettings(modData, HistorySettings::SETTING_GENDIR, QVariant(direction));
}

void LvlItemProperties::on_PROPS_NpcLayer_currentIndexChanged(const QString &arg1)
{
    if(m_externalLock || m_internalLock)
        return;

    if(m_currentNpcArrayId < 0)
    {
        LvlPlacingItems::npcSet.layer = arg1;
        LvlPlacingItems::layer = arg1;
    }
    else if(mw()->activeChildWindow() == MainWindow::WND_Level)
    {
        LevelData modData;
        LevelEdit *edit = mw()->activeLvlEditWin();
        QList<QGraphicsItem *> items = edit->scene->selectedItems();
        edit->LvlData.meta.modified = true;
        for(QGraphicsItem *item : items)
        {
            if(item->data(ITEM_TYPE).toString() == "NPC")
            {
                modData.npc.push_back(((ItemNPC *)item)->m_data);
                ((ItemNPC *)item)->setLayer(arg1);
            }
        }
        edit->scene->m_history->addChangedLayer(modData, arg1);
    }

}

void LvlItemProperties::on_PROPS_NpcAttachLayer_currentIndexChanged(const QString &arg1)
{
    if(m_externalLock || m_internalLock)
        return;

    if(m_currentNpcArrayId < 0)
    {
        if(ui->PROPS_NpcAttachLayer->currentIndex() > 0)
            LvlPlacingItems::npcSet.attach_layer = arg1;
        else
            LvlPlacingItems::npcSet.attach_layer.clear();
        return;
    }

    if(mw()->activeChildWindow() != MainWindow::WND_Level)
        return;

    LevelData modData;
    LevelEdit *edit = mw()->activeLvlEditWin();
    QList<QGraphicsItem *> items = edit->scene->selectedItems();
    edit->LvlData.meta.modified = true;
    for(QGraphicsItem *item : items)
    {
        if(item->data(ITEM_TYPE).toString() == "NPC")
        {
            modData.npc.push_back(((ItemNPC *)item)->m_data);
            if(ui->PROPS_NpcAttachLayer->currentIndex() > 0)
                ((ItemNPC *)item)->m_data.attach_layer = arg1;
            else
                ((ItemNPC *)item)->m_data.attach_layer.clear();
            ((ItemNPC *)item)->arrayApply();
        }
    }
    if(ui->PROPS_NpcAttachLayer->currentIndex() > 0)
        edit->scene->m_history->addChangeSettings(modData, HistorySettings::SETTING_ATTACHLAYER, QVariant(arg1));
    else
        edit->scene->m_history->addChangeSettings(modData, HistorySettings::SETTING_ATTACHLAYER, QVariant(""));
}

void LvlItemProperties::on_PROPS_NpcEventActivate_currentIndexChanged(const QString &arg1)
{
    if(m_externalLock || m_internalLock)
        return;

    if(m_currentNpcArrayId < 0)
    {
        if(ui->PROPS_NpcEventActivate->currentIndex() > 0)
            LvlPlacingItems::npcSet.event_activate = arg1;
        else
            LvlPlacingItems::npcSet.event_activate = "";
        m_recentNpcEventActivated = LvlPlacingItems::npcSet.event_activate;
        return;
    }

    if(mw()->activeChildWindow() != MainWindow::WND_Level)
        return;

    LevelData modData;
    LevelEdit *edit = mw()->activeLvlEditWin();
    QList<QGraphicsItem *> items = edit->scene->selectedItems();
    edit->LvlData.meta.modified = true;
    for(QGraphicsItem *item : items)
    {
        if(item->data(ITEM_TYPE).toString() == "NPC")
        {
            modData.npc.push_back(((ItemNPC *)item)->m_data);
            if(ui->PROPS_NpcEventActivate->currentIndex() > 0)
                ((ItemNPC *)item)->m_data.event_activate = arg1;
            else
                ((ItemNPC *)item)->m_data.event_activate = "";
            ((ItemNPC *)item)->arrayApply();
        }
    }
    if(ui->PROPS_NpcEventActivate->currentIndex() > 0)
        edit->scene->m_history->addChangeSettings(modData, HistorySettings::SETTING_EV_ACTIVATE, QVariant(arg1));
    else
        edit->scene->m_history->addChangeSettings(modData, HistorySettings::SETTING_EV_ACTIVATE, QVariant(""));
}

void LvlItemProperties::on_PROPS_NpcEventDeath_currentIndexChanged(const QString &arg1)
{
    if(m_externalLock || m_internalLock)
        return;

    if(m_currentNpcArrayId < 0)
    {
        if(ui->PROPS_NpcEventDeath->currentIndex() > 0)
            LvlPlacingItems::npcSet.event_die = arg1;
        else
            LvlPlacingItems::npcSet.event_die = "";
        m_recentNpcEventDeath = LvlPlacingItems::npcSet.event_die;
        return;
    }

    if(mw()->activeChildWindow() != MainWindow::WND_Level)
        return;

    LevelData modData;
    LevelEdit *edit = mw()->activeLvlEditWin();
    QList<QGraphicsItem *> items = edit->scene->selectedItems();
    edit->LvlData.meta.modified = true;
    for(QGraphicsItem *item : items)
    {
        if(item->data(ITEM_TYPE).toString() == "NPC")
        {
            modData.npc.push_back(((ItemNPC *)item)->m_data);
            if(ui->PROPS_NpcEventDeath->currentIndex() > 0)
                ((ItemNPC *)item)->m_data.event_die = arg1;
            else
                ((ItemNPC *)item)->m_data.event_die = "";
            ((ItemNPC *)item)->arrayApply();
        }
    }
    if(ui->PROPS_NpcEventDeath->currentIndex() > 0)
        edit->scene->m_history->addChangeSettings(modData, HistorySettings::SETTING_EV_DEATH, QVariant(arg1));
    else
        edit->scene->m_history->addChangeSettings(modData, HistorySettings::SETTING_EV_DEATH, QVariant(""));
}

void LvlItemProperties::on_PROPS_NpcEventTalk_currentIndexChanged(const QString &arg1)
{
    if(m_externalLock || m_internalLock)
        return;

    if(m_currentNpcArrayId < 0)
    {
        if(ui->PROPS_NpcEventTalk->currentIndex() > 0)
            LvlPlacingItems::npcSet.event_talk = arg1;
        else
            LvlPlacingItems::npcSet.event_talk = "";
        m_recentNpcEventTalk = LvlPlacingItems::npcSet.event_talk;
        return;
    }

    if(mw()->activeChildWindow() != MainWindow::WND_Level)
        return;

    LevelData modData;
    LevelEdit *edit = mw()->activeLvlEditWin();
    QList<QGraphicsItem *> items = edit->scene->selectedItems();
    edit->LvlData.meta.modified = true;
    for(QGraphicsItem *item : items)
    {
        if(item->data(ITEM_TYPE).toString() == "NPC")
        {
            modData.npc.push_back(((ItemNPC *)item)->m_data);
            if(ui->PROPS_NpcEventTalk->currentIndex() > 0)
                ((ItemNPC *)item)->m_data.event_talk = arg1;
            else
                ((ItemNPC *)item)->m_data.event_talk = "";
            ((ItemNPC *)item)->arrayApply();
        }
    }
    if(ui->PROPS_NpcEventTalk->currentIndex() > 0)
        edit->scene->m_history->addChangeSettings(modData, HistorySettings::SETTING_EV_TALK, QVariant(arg1));
    else
        edit->scene->m_history->addChangeSettings(modData, HistorySettings::SETTING_EV_TALK, QVariant(""));
}

void LvlItemProperties::on_PROPS_NpcEventEmptyLayer_currentIndexChanged(const QString &arg1)
{
    if(m_externalLock || m_internalLock)
        return;

    if(m_currentNpcArrayId < 0)
    {
        if(ui->PROPS_NpcEventEmptyLayer->currentIndex() > 0)
            LvlPlacingItems::npcSet.event_emptylayer = arg1;
        else
            LvlPlacingItems::npcSet.event_emptylayer.clear();
        m_recentNpcEventLayerEmpty = LvlPlacingItems::npcSet.event_emptylayer;
        return;
    }

    if(mw()->activeChildWindow() != MainWindow::WND_Level)
        return;

    LevelData modData;
    LevelEdit *edit = mw()->activeLvlEditWin();
    QList<QGraphicsItem *> items = edit->scene->selectedItems();
    edit->LvlData.meta.modified = true;
    for(QGraphicsItem *item : items)
    {
        if(item->data(ITEM_TYPE).toString() == "NPC")
        {
            modData.npc.push_back(((ItemNPC *)item)->m_data);
            if(ui->PROPS_NpcEventEmptyLayer->currentIndex() > 0)
                ((ItemNPC *)item)->m_data.event_emptylayer = arg1;
            else
                ((ItemNPC *)item)->m_data.event_emptylayer = "";
            ((ItemNPC *)item)->arrayApply();
        }
    }
    if(ui->PROPS_NpcEventEmptyLayer->currentIndex() > 0)
        edit->scene->m_history->addChangeSettings(modData, HistorySettings::SETTING_EV_LAYER_EMP, QVariant(arg1));
    else
        edit->scene->m_history->addChangeSettings(modData, HistorySettings::SETTING_EV_LAYER_EMP, QVariant(""));
}

void LvlItemProperties::keyPressEvent(QKeyEvent *event)
{
    switch(event->key())
    {
    case Qt::Key_Left:
    case Qt::Key_Right:
    case Qt::Key_Up:
    case Qt::Key_Down:
        if(mw())
        {
            qApp->setActiveWindow(mw());
            mw()->setFocus(Qt::MouseFocusReason);
            mw()->ui->centralWidget->setFocus(Qt::MouseFocusReason);
            //mw()->keyPressEvent(event);
            return;
        }
    default:
        break;
    }
    QDockWidget::keyPressEvent(event);
}
