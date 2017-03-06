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

#include "lvl_event_engine.h"
#include "../scene_level.h"
#include <gui/pge_msgbox.h>
#include <data_configs/config_manager.h>
#include <audio/pge_audio.h>

#include <unordered_set>

LVL_EventAction::LVL_EventAction():
    m_timeDelayLeft(0.0)
{}

LVL_EventAction::~LVL_EventAction()
{}



LVL_EventEngine::LVL_EventEngine() :
    m_scene(nullptr)
{}

LVL_EventEngine::~LVL_EventEngine()
{
    events.clear();
}

void LVL_EventEngine::addSMBX64Event(LevelSMBX64Event &evt)
{
    LVL_EventAction evntAct;
    evntAct.m_eventName = evt.name;
    evntAct.m_timeDelayLeft = 0.0;
    char byte_padding7[7];
    char byte_padding4[4];

    if(!evt.layers_hide.empty())
    {
        EventQueueEntry<LVL_EventAction> hideLayers;
        PGEStringList layers = evt.layers_hide;
        bool          smoke  = !evt.nosmoke;
        hideLayers.makeCaller([this, layers, smoke, byte_padding7]()->void
        {
            for(const std::string &ly : layers)
                m_scene->layers.hide(ly, smoke);
        }, 0);
        evntAct.m_action.events.push_back(hideLayers);
    }

    if(!evt.layers_show.empty())
    {
        EventQueueEntry<LVL_EventAction> showLayers;
        PGEStringList layers = evt.layers_show;
        bool        smoke  = !evt.nosmoke;
        showLayers.makeCaller([this, layers, smoke, byte_padding7]()->void
        {
            for(const std::string &ly : layers)
                m_scene->layers.show(ly, smoke);
        }, 0);
        evntAct.m_action.events.push_back(showLayers);
    }

    if(!evt.layers_toggle.empty())
    {
        EventQueueEntry<LVL_EventAction> toggleLayers;
        PGESTRINGList   layers = evt.layers_toggle;
        bool            smoke  = !evt.nosmoke;
        toggleLayers.makeCaller([this, layers, smoke, byte_padding7]()->void
        {
            for(const std::string &ly : layers)
                m_scene->layers.toggle(ly, smoke);
        }, 0);
        evntAct.m_action.events.push_back(toggleLayers);
    }

    if(evt.sound_id > 0)
    {
        EventQueueEntry<LVL_EventAction> playsnd;
        long soundID = evt.sound_id;
        playsnd.makeCaller([this, soundID]()->void
        {
            PGE_Audio::playSound(soundID);
        }, 0);
        evntAct.m_action.events.push_back(playsnd);
    }

    for(size_t i = 0; i < evt.sets.size(); i++)
    {
        if(evt.sets[i].background_id != -1)
        {
            EventQueueEntry<LVL_EventAction> bgToggle;

            if(evt.sets[i].background_id < 0)
            {
                bgToggle.makeCaller([this, i, byte_padding4]()->void
                {
                    if(i < m_scene->sections.size())
                    {
                        m_scene->sections[i].resetBG();

                        for(int j = 0; j < m_scene->cameras.size(); j++)
                        {
                            if(m_scene->cameras[j].cur_section == &m_scene->sections[i])
                                m_scene->sections[i].initBG();
                        }
                    }

                }, 0);
            }
            else
            {
                unsigned long bgID = static_cast<unsigned long>(evt.sets[i].background_id);
                bgToggle.makeCaller([this, bgID, i, byte_padding4]()->void
                {
                    if(i < m_scene->sections.size())
                    {
                        m_scene->sections[i].setBG(bgID);

                        for(size_t j = 0; j < m_scene->cameras.size(); j++)
                        {
                            if(m_scene->cameras[j].cur_section == &m_scene->sections[i])
                                m_scene->sections[i].initBG();
                        }
                    }
                }, 0);
            }

            evntAct.m_action.events.push_back(bgToggle);
        }

        if(evt.sets[i].music_id != -1)
        {
            EventQueueEntry<LVL_EventAction> musToggle;

            if(evt.sets[i].music_id < 0)
            {
                musToggle.makeCaller([this, i, byte_padding4]()->void
                {
                    if(i < m_scene->sections.size())
                    {
                        m_scene->sections[i].resetMusic();

                        for(int j = 0; j < m_scene->cameras.size(); j++)
                        {
                            if(m_scene->cameras[j].cur_section == &m_scene->sections[i])
                                m_scene->sections[i].playMusic();
                        }
                    }
                }, 0);
            }
            else
            {
                int musID = static_cast<int>(evt.sets[i].music_id);
                musToggle.makeCaller([this, musID, i]()->void
                {
                    if(i < m_scene->sections.size())
                    {
                        m_scene->sections[i].setMusic(musID);

                        for(int j = 0; j < m_scene->cameras.size(); j++)
                        {
                            if(m_scene->cameras[j].cur_section == &m_scene->sections[i])
                                m_scene->sections[i].playMusic();
                        }
                    }
                }, 0);
            }

            evntAct.m_action.events.push_back(musToggle);
        }

        if(evt.sets[i].position_left != -1)
        {
            EventQueueEntry<LVL_EventAction> bordersToggle;

            if(evt.sets[i].position_left == -2)
            {
                bordersToggle.makeCaller([this, i, byte_padding4]()->void
                {
                    if(i < m_scene->sections.size())
                    {
                        m_scene->sections[i].resetLimits();
                    }
                }, 0);
            }
            else
            {
                long box[4] =
                {
                    evt.sets[i].position_left,
                    evt.sets[i].position_top,
                    evt.sets[i].position_right,
                    evt.sets[i].position_bottom
                };
                bordersToggle.makeCaller([this, box, i, byte_padding4]()->void
                {
                    if(i < m_scene->sections.size())
                    {
                        m_scene->sections[i].changeLimitBorders(box[0], box[1], box[2], box[3]);
                    }
                }, 0);
            }

            evntAct.m_action.events.push_back(bordersToggle);
        }
    }

    if((evt.scroll_section < m_scene->sections.size()) && ((evt.move_camera_x != 0.0) || (evt.move_camera_y != 0.0)))
    {
        EventQueueEntry<LVL_EventAction> installAutoscroll;
        installAutoscroll.makeCaller([this, evt]()->void
        {
            LVL_Section &section = m_scene->sections[static_cast<int>(evt.scroll_section)];
            section.isAutoscroll = true;
            section._autoscrollVelocityX = evt.move_camera_x;
            section._autoscrollVelocityY = evt.move_camera_y;

            for(int j = 0; j < m_scene->cameras.size(); j++)
            {
                if(m_scene->cameras[j].cur_section == &section)
                {
                    m_scene->cameras[j].m_autoScrool.enabled = true;
                    m_scene->cameras[j].m_autoScrool.resetAutoscroll();
                }
            }
        }, 0);
        evntAct.m_action.events.push_back(installAutoscroll);
    }

    if(!evt.msg.empty())
    {
        EventQueueEntry<LVL_EventAction> message;
        message.makeCaller([this, evt]()->void
        {
            EventQueueEntry<LevelScene > msgBox;
            std::string message = evt.msg;
            msgBox.makeCaller(
            [this, message]()->void{
                m_scene->m_messages.showMsg(message);
                //                                   PGE_MsgBox box(_scene, evt.msg,
                //                                   PGE_MsgBox::msg_info, PGE_Point(-1,-1), -1,
                //                                   ConfigManager::setup_message_box.sprite);
                //                                   box.exec();
            }, 0.0);
            m_scene->system_events.events.push_back(msgBox);
        }, 0);
        evntAct.m_action.events.push_back(message);
    }

    if(!evt.movelayer.empty())
    {
        EventQueueEntry<LVL_EventAction> movelayer;
        movelayer.makeCaller([this, evt]()->void
        {
            m_scene->layers.installLayerMotion(evt.movelayer, evt.layer_speed_x, evt.layer_speed_y);
        }, 0);
        evntAct.m_action.events.push_back(movelayer);
        evntAct.m_timeDelayLeft = 0.0;
    }

    events[evt.name].push_back(evntAct);

    if(!evt.trigger.empty())
    {
        LVL_EventAction trigger;
        trigger.m_eventName = evt.name;
        EventQueueEntry<LVL_EventAction> triggerEvent;
        std::string trgr = evt.trigger;
        triggerEvent.makeCaller([this, trgr]()->void
        {
            m_scene->events.triggerEvent(trgr);
        },
        static_cast<double>(evt.trigger_timer) * 100.0
        //                ((double(evt.trigger_timer) / 10.0) * 65.0)
                               );
        trigger.m_action.events.push_back(triggerEvent);
        events[evt.name].push_back(trigger);
    }

    //Automatically trigger events
    if((evt.name == "Level - Start") || evt.autostart == LevelSMBX64Event::AUTO_LevelStart)
        workingEvents.push_back(events[evt.name]);
}

void LVL_EventEngine::processTimers(double tickTime)
{
    std::unordered_set<std::string> triggered;

    //static double smbxTimeUnit = 65.0 / 1000.0;
    for(size_t i = 0; i < workingEvents.size(); i++)
    {
        EventActList *ea = &workingEvents[i];
        if(ea->empty())
        {
            workingEvents.erase(workingEvents.begin() + i);
            i--;
            continue;
        }

        for(size_t j = 0; j < ea->size(); j++)
        {
            LVL_EventAction *ee = &(*ea)[j];
            if(ee->m_timeDelayLeft <= 0.0)
            {
                ee->m_action.processEvents(tickTime /* *smbxTimeUnit*/);

                if(ee->m_action.events.empty())
                {
                    if(triggered.find(ee->m_eventName) != triggered.end())
                    {
                        ea->erase(ea->begin() + j);
                        j--;
                        break;
                    }
                    triggered.insert(ee->m_eventName);
                    ea->erase(ea->begin() + j);
                    j--;
                    continue;
                }
            }
            else
                ee->m_timeDelayLeft -= tickTime;
        }
    }
}

void LVL_EventEngine::triggerEvent(std::string event)
{
    if(event.empty())
        return;

    EventsTable::iterator e = events.find(event);
    if(e != events.end())
        workingEvents.push_back(e->second);
}
