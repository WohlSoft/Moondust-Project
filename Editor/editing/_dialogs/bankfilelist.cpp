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

#include "bankfilelist.h"

#include <QIcon>

BankFileList::BankFileList(QString searchDirectory, QString curFile, QWidget *parent, int type) :
    FileListBrowser(searchDirectory, curFile, parent),
    m_type(type)
{
    QStringList filters;

    setIcon(QIcon(":/images/playmusic.png"));

    switch(m_type)
    {
    default:
    case Type_WOPL:
        setWindowTitle(tr("Select WOPL bank file"));
        setDescription(tr("Please select the WOPL instruments bank file to use"));
        filters << "*.wopl";
        setFilters(filters);
        break;
    case Type_WOPN:
        setWindowTitle(tr("Select WOPN bank file"));
        setDescription(tr("Please select the WOPN instruments bank file to use"));
        filters << "*.wopn";
        setFilters(filters);
        break;
    case Type_SoundFont:
        setWindowTitle(tr("Select SoundFont bank file"));
        setDescription(tr("Please select the SoundFont bank file to add"));
        filters << "*.sf2" << "*.sf3";
        setFilters(filters);
        break;
    }
    startListBuilder();
}

BankFileList::~BankFileList()
{}
