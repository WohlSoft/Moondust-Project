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

#include <editing/edit_level/levelprops.h>

#include <ui_mainwindow.h>
#include "mainwindow.h"



//Open Level Properties
void MainWindow::on_actionLevelProp_triggered()
{
    if(activeChildWindow()==1)
    {
        LevelEdit * e=activeLvlEditWin();
        if(!e) return;
        LevelProps LevProps(e->LvlData, this);
        LevProps.setWindowFlags (Qt::Window | Qt::WindowTitleHint | Qt::WindowCloseButtonHint);
        LevProps.setGeometry(QStyle::alignedRect(Qt::LeftToRight, Qt::AlignCenter, LevProps.size(), qApp->desktop()->availableGeometry()));
        if(LevProps.exec()==QDialog::Accepted)
        {
            QList<QVariant> lvlsetData;
            lvlsetData.push_back(e->LvlData.LevelName);
            lvlsetData.push_back(LevProps.LevelTitle);
            e->scene->addChangeLevelSettingsHistory(HistorySettings::SETTING_LEVELNAME, QVariant(lvlsetData));
            e->LvlData.LevelName = LevProps.LevelTitle;
            e->LvlData.modified = true;
            e->setWindowTitle( QString(LevProps.LevelTitle.isEmpty() ? e->userFriendlyCurrentFile() : LevProps.LevelTitle).replace("&", "&&&") );
            updateWindowMenu();
        }
    }
}
