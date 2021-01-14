/*
 * Moondust, a free game engine for platform game making
 * Copyright (c) 2014-2021 Vitaly Novichkov <admin@wohlnet.ru>
 *
 * This software is licensed under a dual license system (MIT or GPL version 3 or later).
 * This means you are free to choose with which of both licenses (MIT or GPL version 3 or later)
 * you want to use this software.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *
 * You can see text of MIT license in the LICENSE.mit file you can see in Engine folder,
 * or see https://mit-license.org/.
 *
 * You can see text of GPLv3 license in the LICENSE.gpl3 file you can see in Engine folder,
 * or see <http://www.gnu.org/licenses/>.
 */

#ifndef LVL_EVENTENGINE_H
#define LVL_EVENTENGINE_H

#include <PGE_File_Formats/lvl_filedata.h>
#include <common_features/event_queue.h>
#include <Utils/vptrlist.h>
#include <string>
#include <vector>
#include <unordered_map>

class LVL_EventAction
{
public:
    LVL_EventAction();
    LVL_EventAction(const LVL_EventAction &ea) = default;
    virtual ~LVL_EventAction() = default;

    std::string m_eventName;
    EventQueue<LVL_EventAction > m_action;
    double m_timeDelayLeft = 0.0;
};

class LevelScene;
class LVL_EventEngine
{
    friend class LevelScene;
    LevelScene *m_scene = nullptr;
public:
    LVL_EventEngine();
    virtual ~LVL_EventEngine();
    void addSMBX64Event(LevelSMBX64Event &evt);
    void processTimers(double tickTime);
    void triggerEvent(std::string event);
    typedef VPtrList<LVL_EventAction >  EventActList;
    typedef VPtrList<EventActList >     WorkingEvents;
    WorkingEvents   workingEvents;
    typedef std::unordered_map<std::string, EventActList > EventsTable;
    EventsTable events;
};

#endif // LVL_EVENTENGINE_H
