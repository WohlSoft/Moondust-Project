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

#ifndef NPCEDIT_H
#define NPCEDIT_H

#include <QWidget>
#include <QFile>
#include <QMessageBox>

#include "../../data_configs/data_configs.h"
#include "../../file_formats/npc_filedata.h"
#include "../_scenes/level/item_npc.h"

#define NPC_EDIT_CLASS "NpcEdit"

namespace Ui {
class NpcEdit;
}

class NpcEdit : public QWidget
{
    Q_OBJECT
    
public:
    explicit NpcEdit(dataconfigs *configs, QWidget *parent = 0);
    ~NpcEdit();

    void newFile(unsigned long npcID);
    bool loadFile(const QString &fileName, NPCConfigFile FileData);
    NPCConfigFile NpcData;
    unsigned long npc_id;

    bool save();
    bool saveAs();
    bool saveFile(const QString &fileName);
    QString userFriendlyCurrentFile();
    QString currentFile() { return curFile; }

    QString curFile;

    bool isModyfied;
    bool isUntitled;

protected:
    void closeEvent(QCloseEvent *event);

private slots:
    void on_ResetNPCData_clicked();

    void documentWasModified();
    void documentNotModified();

    void on_en_GFXOffsetX_clicked(bool checked);
    void on_en_GFXOffsetY_clicked(bool checked);
    void on_En_GFXw_clicked(bool checked);
    void on_En_GFXh_clicked(bool checked);
    void on_En_Frames_clicked(bool checked);
    void on_En_Framespeed_clicked(bool checked);
    void on_En_Framestyle_clicked(bool checked);
    void on_En_IsForeground_clicked(bool checked);
    void on_En_GrabSide_clicked(bool checked);
    void on_En_GrabTop_clicked(bool checked);
    void on_En_JumpHurt_clicked(bool checked);
    void on_En_DontHurt_clicked(bool checked);
    void on_En_Score_clicked(bool checked);
    void on_En_NoEat_clicked(bool checked);
    void on_En_NoFireball_clicked(bool checked);
    void on_En_NoIceball_clicked(bool checked);
    void on_En_Width_clicked(bool checked);
    void on_En_Height_clicked(bool checked);
    void on_En_Speed_clicked(bool checked);
    void on_En_PlayerBlock_clicked(bool checked);
    void on_En_PlayerBlockTop_clicked(bool checked);
    void on_En_NPCBlock_clicked(bool checked);
    void on_En_NPCBlockTop_clicked(bool checked);
    void on_En_NoBlockCollision_clicked(bool checked);
    void on_En_NoGravity_clicked(bool checked);
    void on_En_TurnCliff_clicked(bool checked);

    void on_GFXOffSetX_valueChanged(int arg1);
    void on_GFXOffSetY_valueChanged(int arg1);
    void on_GFXw_valueChanged(int arg1);
    void on_GFXh_valueChanged(int arg1);
    void on_Frames_valueChanged(int arg1);
    void on_Framespeed_valueChanged(int arg1);
    void on_FrameStyle_currentIndexChanged(int index);
    void on_IsForeground_stateChanged(int arg1);
    void on_GrabSide_stateChanged(int arg1);
    void on_GrabTop_stateChanged(int arg1);
    void on_JumpHurt_stateChanged(int arg1);
    void on_DontHurt_stateChanged(int arg1);
    void on_Score_currentIndexChanged(int index);
    void on_NoEat_stateChanged(int arg1);
    void on_NoFireball_stateChanged(int arg1);
    void on_NoIceball_stateChanged(int arg1);
    void on_Width_valueChanged(int arg1);
    void on_Height_valueChanged(int arg1);
    void on_Speed_valueChanged(double arg1);
    void on_PlayerBlock_stateChanged(int arg1);
    void on_PlayerBlockTop_stateChanged(int arg1);
    void on_NPCBlock_stateChanged(int arg1);
    void on_NPCBlockTop_stateChanged(int arg1);
    void on_NoBlockCollision_stateChanged(int arg1);
    void on_NoGravity_stateChanged(int arg1);
    void on_TurnCliff_stateChanged(int arg1);

    //Extended
    void on_En_NoHammer_clicked(bool checked);
    void on_NoHammer_stateChanged(int arg1);

    void on_DirectLeft_clicked();

    void on_DirectRight_clicked();

    void on_En_NoShell_clicked(bool checked);
    void on_NoShell_stateChanged(int checked);

    void on_En_Name_clicked(bool checked);

    void on_Name_textEdited(const QString &arg1);

private:
    Ui::NpcEdit *ui;

    dataconfigs *pConfigs;
    NPCConfigFile StartNPCData;
    NPCConfigFile DefaultNPCData;
    void setDefaultData(unsigned long npc_id);

    QGraphicsScene * PreviewScene;

    void loadPreview();
    void updatePreview();

    void loadImageFile();
    void refreshImageFile();

    ItemNPC* npcPreview;
    QGraphicsRectItem * physics;

    QPixmap npcImage;
    QPixmap npcMask;
    obj_npc defaultNPC;
    int direction;

    bool maybeSave();
    void setCurrentFile(const QString &fileName);
    void setDataBoxes();
    QString strippedName(const QString &fullFileName);
    unsigned int FileType;
};

#endif // NPCEDIT_H
