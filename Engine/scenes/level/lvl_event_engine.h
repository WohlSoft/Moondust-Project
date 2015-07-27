/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2015 Vitaly Novichkov <admin@wohlnet.ru>
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

#ifndef LVL_EVENTENGINE_H
#define LVL_EVENTENGINE_H

#include <PGE_File_Formats/lvl_filedata.h>
#include <common_features/event_queue.h>
#include <QHash>
#include <QList>
#include <QString>

class LVL_EventAction{
public:
    LVL_EventAction();
    LVL_EventAction(const LVL_EventAction& ea);
    virtual ~LVL_EventAction();

    QString eventName;
    EventQueue<LVL_EventAction > action;
    float timeDelayLeft;
};

class LVL_EventEngine
{
public:
    LVL_EventEngine();
    ~LVL_EventEngine();
    void addSMBX64Event(LevelSMBX64Event &evt);
    void processTimers(float tickTime);
    void triggerEvent(QString event);

    QList<QList<LVL_EventAction > > workingEvents;
    QHash<QString, QList<LVL_EventAction >> events;
};

#endif // LVL_EVENTENGINE_H
