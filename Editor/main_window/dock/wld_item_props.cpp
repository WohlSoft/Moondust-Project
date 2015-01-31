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

#include <editing/_scenes/world/wld_item_placing.h>
#include <editing/_dialogs/itemselectdialog.h>
#include <editing/_scenes/world/items/item_level.h>
#include <editing/_dialogs/levelfilelist.h>
#include <file_formats/file_formats.h>

#include <defines.h>

#include <ui_mainwindow.h>
#include <mainwindow.h>

bool wld_tools_lock=false;


void MainWindow::on_WLD_ItemProps_visibilityChanged(bool visible)
{
    ui->action_Placing_ShowProperties->setChecked(visible);
}



void MainWindow::WldItemProps(int Type, WorldLevels level, bool newItem)
{
    wld_tools_lock=true;

    switch(Type)
    {
    case 0:
        {
            if(newItem)
                wlvlPtr = -1;
            else
                wlvlPtr = level.array_id;

            ui->WLD_PROPS_lvlID->setText(tr("Level ID: %1, Array ID: %2").arg(level.id).arg(level.array_id));
            ui->WLD_PROPS_lvlPos->setText( tr("Position: [%1, %2]").arg(level.x).arg(level.y) );
            ui->WLD_PROPS_LVLTitle->setText(level.title);
            ui->WLD_PROPS_LVLFile->setText(level.lvlfile);
            ui->WLD_PROPS_EnterTo->setValue(level.entertowarp);

            ui->WLD_PROPS_AlwaysVis->setChecked(level.alwaysVisible);
            ui->WLD_PROPS_PathBG->setChecked(level.pathbg);
            ui->WLD_PROPS_BigPathBG->setChecked(level.bigpathbg);
            ui->WLD_PROPS_GameStart->setChecked(level.gamestart);

            ui->WLD_PROPS_GotoX->setText((level.gotox==-1) ? "" : QString::number(level.gotox));
            ui->WLD_PROPS_GotoY->setText((level.gotoy==-1) ? "" : QString::number(level.gotoy));

            ui->WLD_PROPS_GetPoint->setChecked(false);
            ui->WLD_PROPS_GetPoint->setCheckable(false);

            if(newItem)
            {//Reset value to min, if it out of range
                if(level.left_exit >= ui->WLD_PROPS_ExitLeft->count() )
                {
                   WldPlacingItems::LevelSet.left_exit = -1;
                   level.left_exit = -1;
                }
            }
            ui->WLD_PROPS_ExitLeft->setCurrentIndex( level.left_exit+1 );

            if(newItem)
            {//Reset value to min, if it out of range
                if(level.right_exit >= ui->WLD_PROPS_ExitRight->count() )
                {
                   WldPlacingItems::LevelSet.right_exit = -1;
                   level.right_exit = -1;
                }
            }
            ui->WLD_PROPS_ExitRight->setCurrentIndex( level.right_exit+1 );

            if(newItem)
            {//Reset value to min, if it out of range
                if(level.top_exit >= ui->WLD_PROPS_ExitTop->count() )
                {
                   WldPlacingItems::LevelSet.top_exit = -1;
                   level.top_exit = -1;
                }
            }
            ui->WLD_PROPS_ExitTop->setCurrentIndex( level.top_exit+1 );

            if(newItem)
            {//Reset value to min, if it out of range
                if(level.bottom_exit >= ui->WLD_PROPS_ExitBottom->count() )
                {
                   WldPlacingItems::LevelSet.bottom_exit = -1;
                   level.bottom_exit = -1;
                }
            }
            ui->WLD_PROPS_ExitBottom->setCurrentIndex( level.bottom_exit+1 );

            ui->action_Placing_ShowProperties->setChecked(true);
            ui->WLD_ItemProps->setVisible(true);
            ui->WLD_ItemProps->show();
            ui->WLD_ItemProps->raise();

            wld_tools_lock=false;

            ui->WLD_ItemProps->show();

            break;
        }
    case -1: //Nothing to edit
    default:
        ui->WLD_ItemProps->hide();
        ui->action_Placing_ShowProperties->setChecked(false);
    }
    wld_tools_lock=false;
}

void MainWindow::WldItemProps_hide()
{
    ui->WLD_ItemProps->hide();
    ui->action_Placing_ShowProperties->setChecked(false);
}


void MainWindow::WldLvlExitTypeListReset()
{
    QStringList LevelExitType;
    int backup_index;

    LevelExitType.push_back(tr("* - Any"));
    LevelExitType.push_back(tr("0 - None"));
    LevelExitType.push_back(tr("1 - SMB3 Standard Exit"));
    LevelExitType.push_back(tr("2 - SMB3 Boss Exit"));
    LevelExitType.push_back(tr("3 - Walked Offscreen"));
    LevelExitType.push_back(tr("4 - Secret Exit"));
    LevelExitType.push_back(tr("5 - SMB2 Boss Exit"));
    LevelExitType.push_back(tr("6 - Warp Exit"));
    LevelExitType.push_back(tr("7 - Star Exit"));
    LevelExitType.push_back(tr("8 - SMW Exit"));

    wld_tools_lock=true;

    backup_index = ui->WLD_PROPS_ExitTop->currentIndex();
    if(backup_index<0) backup_index=0;
    ui->WLD_PROPS_ExitTop->clear();
    ui->WLD_PROPS_ExitTop->addItems(LevelExitType);
    ui->WLD_PROPS_ExitTop->setCurrentIndex(backup_index);

    backup_index = ui->WLD_PROPS_ExitLeft->currentIndex();
    if(backup_index<0) backup_index=0;
    ui->WLD_PROPS_ExitLeft->clear();
    ui->WLD_PROPS_ExitLeft->addItems(LevelExitType);
    ui->WLD_PROPS_ExitLeft->setCurrentIndex(backup_index);

    backup_index = ui->WLD_PROPS_ExitRight->currentIndex();
    if(backup_index<0) backup_index=0;
    ui->WLD_PROPS_ExitRight->clear();
    ui->WLD_PROPS_ExitRight->addItems(LevelExitType);
    ui->WLD_PROPS_ExitRight->setCurrentIndex(backup_index);

    backup_index = ui->WLD_PROPS_ExitBottom->currentIndex();
    if(backup_index<0) backup_index=0;
    ui->WLD_PROPS_ExitBottom->clear();
    ui->WLD_PROPS_ExitBottom->addItems(LevelExitType);
    ui->WLD_PROPS_ExitBottom->setCurrentIndex(backup_index);

    wld_tools_lock=false;

}


//ITemProps
void MainWindow::on_WLD_PROPS_PathBG_clicked(bool checked)
{
    if(wld_tools_lock) return;

    if(wlvlPtr<0)
    {
        WldPlacingItems::LevelSet.pathbg = checked;
    }
    else
    if (activeChildWindow()==3)
    {
        WorldData selData;
        QList<QGraphicsItem *> items = activeWldEditWin()->scene->selectedItems();
        foreach(QGraphicsItem * item, items)
        {
            if(item->data(0).toString()=="LEVEL")/*&&((item->data(2).toInt()==blockPtr))*/
            {
                selData.levels.push_back(((ItemLevel *) item)->levelData);
                ((ItemLevel*)item)->setPath(checked);
            }
        }
        activeWldEditWin()->scene->addChangeSettingsHistory(selData, HistorySettings::SETTING_PATHBACKGROUND, QVariant(checked));
    }
}

void MainWindow::on_WLD_PROPS_BigPathBG_clicked(bool checked)
{
    if(wld_tools_lock) return;

    if(wlvlPtr<0)
    {
        WldPlacingItems::LevelSet.bigpathbg = checked;
    }
    else
    if (activeChildWindow()==3)
    {
        WorldData selData;
        QList<QGraphicsItem *> items = activeWldEditWin()->scene->selectedItems();
        foreach(QGraphicsItem * item, items)
        {
            if(item->data(0).toString()=="LEVEL")/*&&((item->data(2).toInt()==blockPtr))*/
            {
                selData.levels.push_back(((ItemLevel *) item)->levelData);
                ((ItemLevel*)item)->setbPath(checked);
            }
        }
        activeWldEditWin()->scene->addChangeSettingsHistory(selData, HistorySettings::SETTING_BIGPATHBACKGROUND, QVariant(checked));
    }

}

void MainWindow::on_WLD_PROPS_AlwaysVis_clicked(bool checked)
{
    if(wld_tools_lock) return;

    if(wlvlPtr<0)
    {
        WldPlacingItems::LevelSet.alwaysVisible = checked;
    }
    else
    if (activeChildWindow()==3)
    {
        WorldData selData;
        QList<QGraphicsItem *> items = activeWldEditWin()->scene->selectedItems();
        foreach(QGraphicsItem * item, items)
        {
            if(item->data(0).toString()=="LEVEL")
            {
                selData.levels.push_back(((ItemLevel *) item)->levelData);
                ((ItemLevel*)item)->alwaysVisible(checked);
            }
        }
        activeWldEditWin()->scene->addChangeSettingsHistory(selData, HistorySettings::SETTING_ALWAYSVISIBLE, QVariant(checked));
    }

}

void MainWindow::on_WLD_PROPS_GameStart_clicked(bool checked)
{
    if(wld_tools_lock) return;

    if(wlvlPtr<0)
    {
        WldPlacingItems::LevelSet.gamestart = checked;
    }
    else
    if (activeChildWindow()==3)
    {
        WorldData selData;
        QList<QGraphicsItem *> items = activeWldEditWin()->scene->selectedItems();
        foreach(QGraphicsItem * item, items)
        {
            if(item->data(0).toString()=="LEVEL")
            {
                selData.levels.push_back(((ItemLevel *) item)->levelData);
                ((ItemLevel*)item)->levelData.gamestart = checked;
                ((ItemLevel*)item)->arrayApply();
            }
        }
        activeWldEditWin()->scene->addChangeSettingsHistory(selData, HistorySettings::SETTING_GAMESTARTPOINT, QVariant(checked));
    }

}

void MainWindow::on_WLD_PROPS_LVLFile_editingFinished()
{
    if(wld_tools_lock) return;

    if(!ui->WLD_PROPS_LVLFile->isModified()) return;
    ui->WLD_PROPS_LVLFile->setModified(false);

    if(wlvlPtr<0)
    {
        WldPlacingItems::LevelSet.lvlfile = ui->WLD_PROPS_LVLFile->text();
    }
    else
    if (activeChildWindow()==3)
    {
        WorldData selData;
        QList<QGraphicsItem *> items = activeWldEditWin()->scene->selectedItems();
        foreach(QGraphicsItem * item, items)
        {
            if(item->data(0).toString()=="LEVEL")
            {
                selData.levels.push_back(((ItemLevel *) item)->levelData);
                ((ItemLevel*)item)->levelData.lvlfile = ui->WLD_PROPS_LVLFile->text();
                ((ItemLevel*)item)->arrayApply();
            }
        }
        activeWldEditWin()->scene->addChangeSettingsHistory(selData, HistorySettings::SETTING_LEVELFILE, QVariant(ui->WLD_PROPS_LVLFile->text()));
    }

}

void MainWindow::on_WLD_PROPS_LVLTitle_editingFinished()
{
    if(wld_tools_lock) return;

    if(!ui->WLD_PROPS_LVLTitle->isModified()) return;
    ui->WLD_PROPS_LVLTitle->setModified(false);

    if(wlvlPtr<0)
    {
        WldPlacingItems::LevelSet.title = ui->WLD_PROPS_LVLTitle->text();
    }
    else
    if (activeChildWindow()==3)
    {
        WorldData selData;
        QList<QGraphicsItem *> items = activeWldEditWin()->scene->selectedItems();
        foreach(QGraphicsItem * item, items)
        {
            if(item->data(0).toString()=="LEVEL")
            {
                selData.levels.push_back(((ItemLevel *) item)->levelData);
                ((ItemLevel*)item)->levelData.title = ui->WLD_PROPS_LVLTitle->text();
                ((ItemLevel*)item)->arrayApply();
            }
        }
        activeWldEditWin()->scene->addChangeSettingsHistory(selData, HistorySettings::SETTING_LEVELTITLE, QVariant(ui->WLD_PROPS_LVLTitle->text()));
    }
}

void MainWindow::on_WLD_PROPS_EnterTo_valueChanged(int arg1)
{
    if(wld_tools_lock) return;

    if(wlvlPtr<0)
    {
        WldPlacingItems::LevelSet.entertowarp = arg1;
    }
    else
    if (activeChildWindow()==3)
    {
        WorldData selData;
        QList<QGraphicsItem *> items = activeWldEditWin()->scene->selectedItems();
        foreach(QGraphicsItem * item, items)
        {
            if(item->data(0).toString()=="LEVEL")
            {
                selData.levels.push_back(((ItemLevel *) item)->levelData);
                ((ItemLevel*)item)->levelData.entertowarp = arg1;
                ((ItemLevel*)item)->arrayApply();
            }
        }
        activeWldEditWin()->scene->addChangeSettingsHistory(selData, HistorySettings::SETTING_DOORID, QVariant(arg1));
    }

}

void MainWindow::on_WLD_PROPS_LVLBrowse_clicked()
{
    if(wld_tools_lock) return;

    QString dirPath;
    if(activeChildWindow()==3)
    {
        dirPath = activeWldEditWin()->WldData.path;
    }
    else
        return;

    LevelFileList levelList(dirPath, ui->WLD_PROPS_LVLFile->text());
    if( levelList.exec() == QDialog::Accepted )
    {
        ui->WLD_PROPS_LVLFile->setText(levelList.SelectedFile);
        ui->WLD_PROPS_LVLFile->setModified(true);
        on_WLD_PROPS_LVLFile_editingFinished();

        QRegExp lvlext = QRegExp("*.lvl");
        lvlext.setPatternSyntax(QRegExp::Wildcard);

        //Attempt to read level title:
        QString FilePath = dirPath+"/"+levelList.SelectedFile;
        QFile file(FilePath);

        if (!file.open(QIODevice::ReadOnly)) return;

        LevelData getLevelHead;
        getLevelHead.LevelName = "";
        if( lvlext.exactMatch(FilePath) )
        {
            getLevelHead = FileFormats::ReadLevelFile(file); //function in file_formats.cpp
            if( !getLevelHead.ReadFileValid ) return;
        }

        file.close();
        if(!getLevelHead.LevelName.isEmpty())
        {
            ui->WLD_PROPS_LVLTitle->setText( getLevelHead.LevelName );
            ui->WLD_PROPS_LVLTitle->setModified(true);
            on_WLD_PROPS_LVLTitle_editingFinished();
        }
    }

}

void MainWindow::on_WLD_PROPS_ExitTop_currentIndexChanged(int index)
{
    if(wld_tools_lock) return;

    if(wlvlPtr<0)
    {
        WldPlacingItems::LevelSet.top_exit = index-1;
    }
    else
    if (activeChildWindow()==3)
    {
        WorldData selData;
        QList<QGraphicsItem *> items = activeWldEditWin()->scene->selectedItems();
        foreach(QGraphicsItem * item, items)
        {
            if(item->data(0).toString()=="LEVEL")/*&&((item->data(2).toInt()==blockPtr))*/
            {
                selData.levels.push_back(((ItemLevel *) item)->levelData);
                ((ItemLevel*)item)->levelData.top_exit = index-1;
                ((ItemLevel*)item)->arrayApply();
            }
        }
        activeWldEditWin()->scene->addChangeSettingsHistory(selData, HistorySettings::SETTING_PATHBYTOP, QVariant(index-1));
    }

}

void MainWindow::on_WLD_PROPS_ExitLeft_currentIndexChanged(int index)
{
    if(wld_tools_lock) return;

    if(wlvlPtr<0)
    {
        WldPlacingItems::LevelSet.left_exit = index-1;
    }
    else
    if (activeChildWindow()==3)
    {
        WorldData selData;
        QList<QGraphicsItem *> items = activeWldEditWin()->scene->selectedItems();
        foreach(QGraphicsItem * item, items)
        {
            if(item->data(0).toString()=="LEVEL")/*&&((item->data(2).toInt()==blockPtr))*/
            {
                selData.levels.push_back(((ItemLevel *) item)->levelData);
                ((ItemLevel*)item)->levelData.left_exit = index-1;
                ((ItemLevel*)item)->arrayApply();
            }
        }
        activeWldEditWin()->scene->addChangeSettingsHistory(selData, HistorySettings::SETTING_PATHBYLEFT, QVariant(index-1));
    }

}

void MainWindow::on_WLD_PROPS_ExitRight_currentIndexChanged(int index)
{
    if(wld_tools_lock) return;

    if(wlvlPtr<0)
    {
        WldPlacingItems::LevelSet.right_exit = index-1;
    }
    else
    if (activeChildWindow()==3)
    {
        WorldData selData;
        QList<QGraphicsItem *> items = activeWldEditWin()->scene->selectedItems();
        foreach(QGraphicsItem * item, items)
        {
            if(item->data(0).toString()=="LEVEL")/*&&((item->data(2).toInt()==blockPtr))*/
            {
                selData.levels.push_back(((ItemLevel *) item)->levelData);
                ((ItemLevel*)item)->levelData.right_exit = index-1;
                ((ItemLevel*)item)->arrayApply();
            }
        }
        activeWldEditWin()->scene->addChangeSettingsHistory(selData, HistorySettings::SETTING_PATHBYRIGHT, QVariant(index-1));
    }

}

void MainWindow::on_WLD_PROPS_ExitBottom_currentIndexChanged(int index)
{
    if(wld_tools_lock) return;

    if(wlvlPtr<0)
    {
        WldPlacingItems::LevelSet.bottom_exit = index-1;
    }
    else
    if (activeChildWindow()==3)
    {
        WorldData selData;
        QList<QGraphicsItem *> items = activeWldEditWin()->scene->selectedItems();
        foreach(QGraphicsItem * item, items)
        {
            if(item->data(0).toString()=="LEVEL")/*&&((item->data(2).toInt()==blockPtr))*/
            {
                selData.levels.push_back(((ItemLevel *) item)->levelData);
                ((ItemLevel*)item)->levelData.bottom_exit = index-1;
                ((ItemLevel*)item)->arrayApply();
            }
        }
        activeWldEditWin()->scene->addChangeSettingsHistory(selData, HistorySettings::SETTING_PATHBYBOTTOM, QVariant(index-1));
    }


}

//void MainWindow::on_WLD_PROPS_GotoX_textEdited(const QString &arg1)
void MainWindow::on_WLD_PROPS_GotoX_editingFinished()
{
    if(wld_tools_lock) return;

    if(!ui->WLD_PROPS_GotoX->isModified()) return;
    ui->WLD_PROPS_GotoX->setModified(false);

    if(wlvlPtr<0)
    {
        WldPlacingItems::LevelSet.gotox = (ui->WLD_PROPS_GotoX->text().isEmpty())? -1 : ui->WLD_PROPS_GotoX->text().toInt();
    }
    else
    if (activeChildWindow()==3)
    {
        WorldData selData;
        QList<QGraphicsItem *> items = activeWldEditWin()->scene->selectedItems();
        foreach(QGraphicsItem * item, items)
        {
            if(item->data(0).toString()=="LEVEL")
            {
                selData.levels.push_back(((ItemLevel *) item)->levelData);
                ((ItemLevel*)item)->levelData.gotox = (ui->WLD_PROPS_GotoX->text().isEmpty())? -1 : ui->WLD_PROPS_GotoX->text().toInt();
                ((ItemLevel*)item)->arrayApply();
            }
        }
        activeWldEditWin()->scene->addChangeSettingsHistory(selData, HistorySettings::SETTING_GOTOX, QVariant((ui->WLD_PROPS_GotoX->text().isEmpty())? -1 : ui->WLD_PROPS_GotoX->text().toInt()));
    }

}

//void MainWindow::on_WLD_PROPS_GotoY_textEdited(const QString &arg1)
void MainWindow::on_WLD_PROPS_GotoY_editingFinished()
{
    if(wld_tools_lock) return;

    if(!ui->WLD_PROPS_GotoY->isModified()) return;
    ui->WLD_PROPS_GotoY->setModified(false);

    if(wlvlPtr<0)
    {
        WldPlacingItems::LevelSet.gotoy = (ui->WLD_PROPS_GotoY->text().isEmpty())? -1 : ui->WLD_PROPS_GotoY->text().toInt();
    }
    else
    if (activeChildWindow()==3)
    {
        WorldData selData;
        QList<QGraphicsItem *> items = activeWldEditWin()->scene->selectedItems();
        foreach(QGraphicsItem * item, items)
        {
            if(item->data(0).toString()=="LEVEL")/*&&((item->data(2).toInt()==blockPtr))*/
            {
                selData.levels.push_back(((ItemLevel *) item)->levelData);
                ((ItemLevel*)item)->levelData.gotoy = (ui->WLD_PROPS_GotoY->text().isEmpty())? -1 : ui->WLD_PROPS_GotoY->text().toInt();
                ((ItemLevel*)item)->arrayApply();
            }
        }
        activeWldEditWin()->scene->addChangeSettingsHistory(selData, HistorySettings::SETTING_GOTOY, QVariant((ui->WLD_PROPS_GotoY->text().isEmpty())? -1 : ui->WLD_PROPS_GotoY->text().toInt()));
    }
}

void MainWindow::WLD_returnPointToLevelProperties(QPoint p)
{
    ui->WLD_PROPS_GotoX->setText(QString::number(p.x()));
    ui->WLD_PROPS_GotoY->setText(QString::number(p.y()));

    ui->WLD_PROPS_GotoX->setModified(true);
    on_WLD_PROPS_GotoX_editingFinished();
    ui->WLD_PROPS_GotoY->setModified(true);
    on_WLD_PROPS_GotoY_editingFinished();

    ui->WLD_PROPS_GetPoint->setChecked(false);
    ui->WLD_PROPS_GetPoint->setCheckable(false);
}

void MainWindow::on_WLD_PROPS_GetPoint_clicked()
{
    if(wld_tools_lock) return;

    if(wlvlPtr<0)
    {
        QMessageBox::information(this, QString(tr("Placing mode")), tr("Place item on the map first and call 'Properties' context menu item."), QMessageBox::Ok);
        return;
    }
    else
    if (activeChildWindow()==3)
    {
        if(ui->WLD_PROPS_GetPoint->isCheckable())
        {
            ui->WLD_PROPS_GetPoint->setChecked(false);
            ui->WLD_PROPS_GetPoint->setCheckable(false);

            activeWldEditWin()->scene->SwitchEditingMode(WldScene::MODE_Selecting);
        }
        else
        {
            ui->WLD_PROPS_GetPoint->setCheckable(true);
            ui->WLD_PROPS_GetPoint->setChecked(true);

            //activeWldEditWin()->changeCursor(WorldEdit::MODE_PlaceItem);
            activeWldEditWin()->scene->SwitchEditingMode(WldScene::MODE_SetPoint);

            WldPlacingItems::placingMode = WldPlacingItems::PMODE_Brush;

            //WldPlacingItems::squarefillingMode = false;
            ui->actionSquareFill->setChecked(false);
            ui->actionSquareFill->setEnabled(false);

            //WldPlacingItems::lineMode = false;
            ui->actionLine->setChecked(false);
            ui->actionLine->setEnabled(false);
            if(ui->WLD_PROPS_GotoX->text().isEmpty()||ui->WLD_PROPS_GotoY->text().isEmpty())
            {
                activeWldEditWin()->scene->selectedPointNotUsed = true;
            }
            else
            {
                activeWldEditWin()->scene->selectedPointNotUsed = false;
                activeWldEditWin()->scene->selectedPoint=QPoint(ui->WLD_PROPS_GotoX->text().toInt(), ui->WLD_PROPS_GotoY->text().toInt());
            }

            activeWldEditWin()->scene->setItemPlacer(5);
            activeWldEditWin()->setFocus();
        }
    }

}
