/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2014-2016 Vitaly Novichkov <admin@wohlnet.ru>
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

#ifndef HISTORYELEMETNREMOVEWARP_H
#define HISTORYELEMETNREMOVEWARP_H

#include "ihistoryelement.h"
#include <PGE_File_Formats/file_formats.h>

class HistoryElementRemoveWarp : public QObject, public IHistoryElement
{
    Q_OBJECT
    Q_INTERFACES(IHistoryElement)

public:
    explicit HistoryElementRemoveWarp(const LevelDoor &door, QObject *parent = 0);
    virtual ~HistoryElementRemoveWarp();
    virtual QString getHistoryName();
    virtual void undo();
    virtual void redo();

private:
    LevelDoor m_removedDoor;
};

#endif // HISTORYELEMETNREMOVEWARP_H
