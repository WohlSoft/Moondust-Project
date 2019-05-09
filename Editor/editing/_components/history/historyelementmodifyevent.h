/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2014-2019 Vitaly Novichkov <admin@wohlnet.ru>
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
#ifndef HISTORYELEMENTMODIFYEVENT_H
#define HISTORYELEMENTMODIFYEVENT_H

#include "ihistoryelement.h"
#include <PGE_File_Formats/file_formats.h>

class HistoryElementModifyEvent : public QObject, public IHistoryElement
{
    Q_OBJECT
    Q_INTERFACES(IHistoryElement)

public:
    explicit HistoryElementModifyEvent(const LevelSMBX64Event &event, bool didRemove, QObject *parent = 0);
    virtual ~HistoryElementModifyEvent();
    virtual QString getHistoryName();
    virtual void undo();
    virtual void redo();

    void doEventRemove();
    void doEventPlace();

private:
    LevelSMBX64Event m_event;
    bool m_didRemove;
};

#endif // HISTORYELEMENTMODIFYEVENT_H
