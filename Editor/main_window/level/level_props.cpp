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

#include <QDesktopWidget>

#include <editing/edit_level/levelprops.h>
#include <editing/_scenes/level/lvl_history_manager.h>

#include <ui_mainwindow.h>
#include "mainwindow.h"



//Open Level Properties
void MainWindow::on_actionLevelProp_triggered()
{
    if(activeChildWindow() != WND_Level)
        return;

    LevelEdit * e=activeLvlEditWin();
    if(!e) return;
    LevelProps levelProps(e->LvlData, this);
    levelProps.setWindowFlags (Qt::Window | Qt::WindowTitleHint | Qt::WindowCloseButtonHint);
    levelProps.setGeometry(util::alignToScreenCenter(levelProps.size()));
    if(levelProps.exec() == QDialog::Accepted)
    {
        QList<QVariant> lvlsetData;
        lvlsetData.push_back(e->LvlData.LevelName);
        lvlsetData.push_back(e->LvlData.custom_params);
        lvlsetData.push_back(levelProps.m_levelTitle);
        lvlsetData.push_back(levelProps.m_customParams);
        e->scene->m_history->addChangeLevelSettings(HistorySettings::SETTING_LEVELNAME, QVariant(lvlsetData));
        e->LvlData.LevelName = levelProps.m_levelTitle;
        e->LvlData.custom_params = levelProps.m_customParams;
        e->LvlData.meta.modified = true;
        e->setWindowTitle( QString(levelProps.m_levelTitle.isEmpty() ? e->userFriendlyCurrentFile() : levelProps.m_levelTitle).replace("&", "&&&") );
        updateWindowMenu();
    }
}
