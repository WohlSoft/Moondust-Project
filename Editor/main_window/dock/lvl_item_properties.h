/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2014-2021 Vitaly Novichkov <admin@wohlnet.ru>
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

#pragma once
#ifndef LVL_ITEM_PROPERTIES_H
#define LVL_ITEM_PROPERTIES_H

#include <QDockWidget>
#include <QMutex>
#include "mwdock_base.h"
#include <PGE_File_Formats/lvl_filedata.h>
#include <memory>

class MainWindow;
class QComboBox;
class QPushButton;
class QSpacerItem;
class JsonSettingsWidget;

namespace Ui {
class LvlItemProperties;
}

class LvlItemProperties : public QDockWidget, public MWDock_Base
{
    Q_OBJECT

    friend class MainWindow;
private:
    explicit LvlItemProperties(QWidget *parent);
    ~LvlItemProperties();

    void re_translate_widgets();

public:
    long m_currentBlockArrayId;
    long m_currentBgoArrayId;
    long m_currentNpcArrayId;

    void setSettingsLock(bool locked);

    QComboBox *cbox_layer_block();
    QComboBox *cbox_layer_bgo();
    QComboBox *cbox_layer_npc();
    QComboBox *cbox_layer_npc_att();

    QComboBox *cbox_event_block_dest();
    QComboBox *cbox_event_block_hit();
    QComboBox *cbox_event_block_le();

    QComboBox *cbox_event_npc_act();
    QComboBox *cbox_event_npc_die();
    QComboBox *cbox_event_npc_talk();
    QComboBox *cbox_event_npc_le();

public slots:
    void setSMBX64Strict(bool en);
    void re_translate();

public:
    /**
     * @brief Synchronize active layer fields after changing of layers
     * @param Name of affected layer
     */
    void syncLayerFields(const QString &layerName = QString());

    void openBlockProps(LevelBlock& block, bool newItem=false, bool dont_reset_props=false, bool dontShow=false);
    void openBgoProps(LevelBGO& bgo, bool newItem=false, bool dont_reset_props=false, bool dontShow=false);
    void openNpcProps(LevelNPC& npc, bool newItem=false, bool dont_reset_props=false, bool dontShow=false);

    void closeProps();

private:
    void resetBox(bool isPlacingNew);
    void resetBoxEnd(bool dontShowToolbox);

    void refreshFirstNpcSpecialOption(LevelNPC &npc, bool newItem=false, bool dont_reset_props=false);

private slots:
    //Blocks
    void on_PROPS_BlockResize_clicked();
    void on_BLOCK_Width_editingFinished();
    void on_BLOCK_Height_editingFinished();
    void on_PROPS_BlockInvis_clicked(bool checked);
    void on_PROPS_BlkSlippery_clicked(bool checked);
    void on_PROPS_BlockIncludes_clicked();
    void on_PROPS_BlockLayer_currentIndexChanged(const QString &arg1);
    void on_PROPS_BlkEventDestroy_currentIndexChanged(const QString &arg1);
    void on_PROPS_BlkEventHited_currentIndexChanged(const QString &arg1);
    void on_PROPS_BlkEventLayerEmpty_currentIndexChanged(const QString &arg1);

    //BGO
    void on_PROPS_BGOLayer_currentIndexChanged(const QString &arg1);
    void on_PROPS_BGO_Z_Layer_currentIndexChanged(int index);
    void on_PROPS_BGO_Z_Offset_valueChanged(double arg1);
    void on_PROPS_BGO_smbx64_sp_valueChanged(int arg1);

    //NPC
    void on_PROPS_NPCDirLeft_clicked();
    void on_PROPS_NPCDirRand_clicked();
    void on_PROPS_NPCDirRight_clicked();
    void on_PROPS_NpcFri_clicked(bool checked);
    void on_PROPS_NPCNoMove_clicked(bool checked);
    void on_PROPS_NpcBoss_clicked(bool checked);
    void on_PROPS_NpcTMsg_clicked();
    void on_PROPS_NPCContaiter_clicked();
    void on_PROPS_NPCSpecialSpin_valueChanged(int arg1);
    void on_PROPS_NPCSpecialSpin_Auto_clicked(bool checked);
    void on_PROPS_NPCSpecialSpin_Auto_toggled(bool checked);
    void on_PROPS_NPCSpecialNPC_clicked();
    void on_PROPS_NPCSpecialBox_currentIndexChanged(int index);
    void on_PROPS_NpcGenerator_clicked(bool checked);
    void on_PROPS_NPCGenType_currentIndexChanged(int index);
    void on_PROPS_NPCGenTime_valueChanged(double arg1);
    void npcGeneratorDirectionChange(int direction);
    void on_PROPS_NpcLayer_currentIndexChanged(const QString &arg1);
    void on_PROPS_NpcAttachLayer_currentIndexChanged(const QString &arg1);
    void on_PROPS_NpcEventActivate_currentIndexChanged(const QString &arg1);
    void on_PROPS_NpcEventDeath_currentIndexChanged(const QString &arg1);
    void on_PROPS_NpcEventTalk_currentIndexChanged(const QString &arg1);
    void on_PROPS_NpcEventEmptyLayer_currentIndexChanged(const QString &arg1);

protected:
    void keyPressEvent(QKeyEvent *event);

private:
    void processNpcContainerButton(QPushButton *btn);

    void initExtraSettingsWidget(const QString &defaultLocalDir,
                                 const QString &defaultGlobalDir,
                                 const QString &layoutPath,
                                 const QString &layoutPathGlobal,
                                 QString &properties,
                                 void (LvlItemProperties::*callback)());

    void onExtraSettingsBlockChanged();
    void onExtraSettingsBGOChanged();
    void onExtraSettingsNPCChanged();

    int m_npcSpecSpinOffset;
    int m_npcSpecSpinOffset_2;

    //! A lock of item properties change events that was set externally
    bool m_externalLock = true;
    //! A lock of item properties change events that was set internally
    bool m_internalLock = false;

    QMutex m_mutex;
    std::unique_ptr<JsonSettingsWidget> m_extraSettings;
    std::unique_ptr<JsonSettingsWidget> m_extraGlobalSettings;
    std::unique_ptr<QSpacerItem> m_extraSettingsSpacer;

    int m_curItemType;

    QString m_recentBlockEventDestroy;
    QString m_recentBlockEventHit;
    QString m_recentBlockEventLayerEmpty;

    QString m_recentNpcEventActivated;
    QString m_recentNpcEventDeath;
    QString m_recentNpcEventTalk;
    QString m_recentNpcEventLayerEmpty;

    Ui::LvlItemProperties *ui;
};

#endif // LVL_ITEM_PROPERTIES_H
