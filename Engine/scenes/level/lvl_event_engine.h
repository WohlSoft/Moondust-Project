/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2017 Vitaly Novichkov <admin@wohlnet.ru>
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
#include <string>
#include <vector>
#include <unordered_map>

class LVL_EventAction
{
public:
    LVL_EventAction();
    LVL_EventAction(const LVL_EventAction &ea) = default;
    virtual ~LVL_EventAction();

    std::string m_eventName;
    EventQueue<LVL_EventAction > m_action;
    double m_timeDelayLeft;
};

class LevelScene;
class LVL_EventEngine
{
    friend class LevelScene;
    LevelScene *m_scene;
public:
    LVL_EventEngine();
    virtual ~LVL_EventEngine();
    void addSMBX64Event(LevelSMBX64Event &evt);
    void processTimers(double tickTime);
    void triggerEvent(std::string event);
    typedef std::vector<LVL_EventAction > EventActList;
    typedef std::vector<EventActList > WorkingEvents;
    WorkingEvents   workingEvents;
    typedef std::unordered_map<std::string, EventActList > EventsTable;
    EventsTable events;
};

#endif // LVL_EVENTENGINE_H
