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

#include "lvl_event_engine.h"
#include "lvl_scene_ptr.h"
#include <gui/pge_msgbox.h>
#include <data_configs/config_manager.h>
#include <audio/pge_audio.h>

LVL_EventAction::LVL_EventAction()
{
    timeDelayLeft=0.0f;
}

LVL_EventAction::LVL_EventAction(const LVL_EventAction &ea)
{
    action = ea.action;
    timeDelayLeft=ea.timeDelayLeft;
    eventName=ea.eventName;
}

LVL_EventAction::~LVL_EventAction()
{}




LVL_EventEngine::LVL_EventEngine()
{}

LVL_EventEngine::~LVL_EventEngine()
{
    events.clear();
}

void LVL_EventEngine::addSMBX64Event(LevelSMBX64Event &evt)
{
    LVL_EventAction evntAct;
        evntAct.eventName=evt.name;
        evntAct.timeDelayLeft=0;

        EventQueueEntry<LVL_EventAction> hideLayers;
        hideLayers.makeCaller([this,evt]()->void{
                               foreach(QString ly, evt.layers_hide)
                                   LvlSceneP::s->layers.hide(ly, !evt.nosmoke);
                           }, 0);
        evntAct.action.events.push_back(hideLayers);

        EventQueueEntry<LVL_EventAction> showLayers;
        showLayers.makeCaller([this,evt]()->void{
                               foreach(QString ly, evt.layers_show)
                                   LvlSceneP::s->layers.show(ly, !evt.nosmoke);
                           }, 0);
        evntAct.action.events.push_back(showLayers);

        EventQueueEntry<LVL_EventAction> toggleLayers;
        toggleLayers.makeCaller([this,evt]()->void{
                               foreach(QString ly, evt.layers_toggle)
                                   LvlSceneP::s->layers.toggle(ly, !evt.nosmoke);
                           }, 0);
        evntAct.action.events.push_back(toggleLayers);

        if(evt.sound_id>0)
        {
            EventQueueEntry<LVL_EventAction> playsnd;
            playsnd.makeCaller([this,evt]()->void{
                                       PGE_Audio::playSound(evt.sound_id);
                               }, 0);
            evntAct.action.events.push_back(playsnd);
        }

        if(!evt.msg.isEmpty())
        {
            EventQueueEntry<LVL_EventAction> message;
            message.makeCaller([this,evt]()->void{
                EventQueueEntry<LevelScene > msgBox;
                msgBox.makeCaller(
                            [this,evt]()->void{

                                   PGE_MsgBox box(LvlSceneP::s, evt.msg,
                                   PGE_MsgBox::msg_info, PGE_Point(-1,-1), -1,
                                   ConfigManager::setup_message_box.sprite);
                                   box.exec();
                                }, 0);
                LvlSceneP::s->system_events.events.push_back(msgBox);
                               }, 0);
            evntAct.action.events.push_back(message);
        }

    events[evt.name].push_back(evntAct);

    if(!evt.trigger.isEmpty())
    {
        LVL_EventAction trigger;
        trigger.eventName=evt.name;
        trigger.timeDelayLeft=0;
        EventQueueEntry<LVL_EventAction> triggerEvent;
        triggerEvent.makeCaller([this,evt]()->void{
                                LvlSceneP::s->events.triggerEvent(evt.trigger);
                               }, evt.trigger_timer*100);
        trigger.action.events.push_back(triggerEvent);
        events[evt.name].push_back(trigger);
    }

    //Automatically trigger events
    if((evt.name=="Level - Start")||evt.autostart)
        workingEvents.push_back(events[evt.name]);
}

void LVL_EventEngine::processTimers(float tickTime)
{
    for(int i=0; i<workingEvents.size(); i++)
    {
        if(workingEvents[i].isEmpty())
        {
            workingEvents.removeAt(i); i--; continue;
        }

        for(int j=0; j<workingEvents[i].size(); j++)
        {
            if(workingEvents[i][j].timeDelayLeft<=0)
            {
                workingEvents[i][j].action.processEvents(tickTime);
                if(workingEvents[i][j].action.events.isEmpty())
                {
                    workingEvents[i].removeAt(j); j--; continue;
                }
            }
            else
            {
                workingEvents[i][j].timeDelayLeft-=tickTime;
            }
        }
    }
}

void LVL_EventEngine::triggerEvent(QString event)
{
    if(events.contains(event))
        workingEvents.push_back(events[event]);
}





