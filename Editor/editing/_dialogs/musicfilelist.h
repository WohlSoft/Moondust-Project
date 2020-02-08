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

#pragma once
#ifndef MUSICFILELIST_H
#define MUSICFILELIST_H

#include "file_list_browser.h"

class MusicFileList : public FileListBrowser
{
    Q_OBJECT

public:
    explicit MusicFileList(QString searchDirectory, QString currentFile = QString(), QWidget *parent = nullptr, bool sfxMode = false);
    ~MusicFileList();

private:
    bool m_sfxMode = false;
};

#endif // MUSICFILELIST_H
