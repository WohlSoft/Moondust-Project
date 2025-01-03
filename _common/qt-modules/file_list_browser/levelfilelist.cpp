/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2014-2025 Vitaly Novichkov <admin@wohlnet.ru>
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

#include "levelfilelist.h"
#include <QIcon>

LevelFileList::LevelFileList(QString searchDirectory, QString currentFile, QWidget *parent) :
    FileListBrowser(searchDirectory, currentFile, parent)
{
    setIcon(QIcon(":/moondust/level16.png"));
    setWindowTitle(tr("Level files list"));
    setDescription(tr("Please, select level file from list for use them:"));

    QStringList filters;
    filters << "*.lvl" << "*.lvlx" << "*.lvlb" << "*.lvlz";
    setFilters(filters);

    startListBuilder();
}

LevelFileList::~LevelFileList()
{}
