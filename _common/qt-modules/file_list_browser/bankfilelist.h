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

#pragma once
#ifndef BankFileList_H
#define BankFileList_H

#include "file_list_browser.h"

class BankFileList : public FileListBrowser
{
    Q_OBJECT

public:
    enum
    {
        Type_WOPL = 0,
        Type_WOPN,
        Type_SoundFont
    };

    explicit BankFileList(QString searchDirectory, QString curFile = QString(), QWidget *parent = nullptr, int type = Type_WOPL);
    ~BankFileList();

private:
    int m_type = Type_WOPL;
};

#endif // BankFileList_H
