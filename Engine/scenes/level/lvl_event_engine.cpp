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

#include "lvl_event_engine.h"
#include "lvl_layer_engine.h"
#include "../scene_level.h"
#include <gui/pge_msgbox.h>
#include <data_configs/config_manager.h>
#include <audio/pge_audio.h>
#include <Utils/maths.h>

#include <unordered_set>

LVL_EventAction::LVL_EventAction():
    m_timeDelayLeft(0.0)
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

    if(!evt.layers_hide.empty())
    {
        EventQueueEntry<LVL_EventAction> hideLayers;
        PGEStringList layers = evt.layers_hide;
        bool          smoke  = !evt.nosmoke;
        hideLayers.makeCaller([this, layers, smoke]()->void
        {
            for(const std::string &ly : layers)
                m_scene->m_layers.hide(ly, smoke);
        }, 0);
        evntAct.m_action.events.push_back(hideLayers);
    }

    if(!evt.layers_show.empty())
    {
        EventQueueEntry<LVL_EventAction> showLayers;
        PGEStringList   layers  = evt.layers_show;
        bool            smoke   = !evt.nosmoke;
        // Detect "Destroyed blocks" layer and replace it with special command
        for(auto it = layers.begin(); it != layers.end(); )
        {
            std::string *lyr = &(*it);
            // UnDestroy destroyed blocks
            if(*lyr == DESTROYED_LAYER_NAME)
            {
                EventQueueEntry<LVL_EventAction> undestroyBlocks;
                undestroyBlocks.makeCaller([this, smoke]()->void
                {
                    m_scene->restoreDestroyedBlocks(smoke);
                }, 0);
                evntAct.m_action.events.push_back(undestroyBlocks);
                it = layers.erase(it);
            }
            else
                it++;
        }
        showLayers.makeCaller([this, layers, smoke]()->void
        {
            for(const std::string &ly : layers)
                m_scene->m_layers.show(ly, smoke);
        }, 0);
        evntAct.m_action.events.push_back(showLayers);
    }

    if(!evt.layers_toggle.empty())
    {
        EventQueueEntry<LVL_EventAction> toggleLayers;
        PGESTRINGList   layers = evt.layers_toggle;
        bool            smoke  = !evt.nosmoke;
        toggleLayers.makeCaller([this, layers, smoke]()->void
        {
            for(const std::string &ly : layers)
                m_scene->m_layers.toggle(ly, smoke);
        }, 0);
        evntAct.m_action.events.push_back(toggleLayers);
    }

    if(evt.sound_id > 0)
    {
        EventQueueEntry<LVL_EventAction> playsnd;
        long soundID = evt.sound_id;
        playsnd.makeCaller([soundID]()->void
        {
            PGE_Audio::playSound(static_cast<size_t>(soundID));
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
                bgToggle.makeCaller([this, i]()->void
                {
                    if(i < m_scene->m_sections.size())
                    {
                        m_scene->m_sections[i].resetBG();

                        for(auto &m_camera : m_scene->m_cameras)
                        {
                            if(m_camera.cur_section == &m_scene->m_sections[i])
                                m_scene->m_sections[i].initBG();
                        }
                    }

                }, 0);
            }
            else
            {
                auto bgID = static_cast<unsigned long>(evt.sets[i].background_id);
                bgToggle.makeCaller([this, bgID, i]()->void
                {
                    if(i < m_scene->m_sections.size())
                    {
                        m_scene->m_sections[i].setBG(bgID);

                        for(auto &m_camera : m_scene->m_cameras)
                        {
                            if(m_camera.cur_section == &m_scene->m_sections[i])
                                m_scene->m_sections[i].initBG();
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
                musToggle.makeCaller([this, i]()->void
                {
                    if(i < m_scene->m_sections.size())
                    {
                        m_scene->m_sections[i].resetMusic();

                        for(auto &m_camera : m_scene->m_cameras)
                        {
                            if(m_camera.cur_section == &m_scene->m_sections[i])
                                m_scene->m_sections[i].playMusic();
                        }
                    }
                }, 0);
            }
            else
            {
                int musID = static_cast<int>(evt.sets[i].music_id);
                musToggle.makeCaller([this, musID, i]()->void
                {
                    if(i < m_scene->m_sections.size())
                    {
                        m_scene->m_sections[i].setMusic(static_cast<unsigned int>(musID));

                        for(auto &m_camera : m_scene->m_cameras)
                        {
                            if(m_camera.cur_section == &m_scene->m_sections[i])
                                m_scene->m_sections[i].playMusic();
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
                bordersToggle.makeCaller([this, i]()->void
                {
                    if(i < m_scene->m_sections.size())
                    {
                        m_scene->m_sections[i].resetLimits();
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
                bordersToggle.makeCaller([this, box, i]()->void
                {
                    if(i < m_scene->m_sections.size())
                    {
                        m_scene->m_sections[i].changeLimitBorders(box[0], box[1], box[2], box[3]);
                    }
                }, 0);
            }

            evntAct.m_action.events.push_back(bordersToggle);
        }
    }

    if(
        (evt.scroll_section < static_cast<long>(m_scene->m_sections.size()))
        && ((evt.move_camera_x != 0.0) || (evt.move_camera_y != 0.0))
    )
    {
        EventQueueEntry<LVL_EventAction> installAutoscroll;
        installAutoscroll.makeCaller([this, evt]()->void
        {
            LVL_Section &section = m_scene->m_sections[static_cast<size_t>(evt.scroll_section)];
            section.m_isAutoscroll = true;
            section.m_autoscrollVelocityX = evt.move_camera_x;
            section.m_autoscrollVelocityY = evt.move_camera_y;

            for(auto &m_camera : m_scene->m_cameras)
            {
                if(m_camera.cur_section == &section)
                {
                    m_camera.m_autoScrool.enabled = true;
                    m_camera.m_autoScrool.resetAutoscroll();
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
            const std::string &msg = evt.msg;
            msgBox.makeCaller(
            [this, msg]()->void{
                m_scene->m_messages.showMsg(msg);
                //                                   PGE_MsgBox box(_scene, evt.msg,
                //                                   PGE_MsgBox::msg_info, PGE_Point(-1,-1), -1,
                //                                   ConfigManager::setup_message_box.sprite);
                //                                   box.exec();
            }, 0.0);
            m_scene->m_systemEvents.events.push_back(msgBox);
        }, 0);
        evntAct.m_action.events.push_back(message);
    }

    if(!evt.movelayer.empty())
    {
        EventQueueEntry<LVL_EventAction> movelayer;
        movelayer.makeCaller([this, evt]()->void
        {
            m_scene->m_layers.installLayerMotion(evt.movelayer, evt.layer_speed_x, evt.layer_speed_y);
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
        double triggerDelay = static_cast<double>(evt.trigger_timer) * 100.0;
        //FIXME: put the correct milliseconds to SMBX ticks convert formula
        triggerDelay = (std::round(triggerDelay * 2.5 / 39.0) * 39.6) / 2.5;
        Maths::clearPrecision(triggerDelay);
        triggerEvent.makeCaller([this, trgr]()->void
        {
            m_scene->m_events.triggerEvent(trgr);
        },
        triggerDelay
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

    // const double smbxTimeUnit = 1000.0 / 65;
    for(size_t i = 0; i < workingEvents.size(); i++)
    {
        EventActList *ea = &workingEvents[i];
        if(ea->empty())
        {
            workingEvents.erase(workingEvents.begin() + int(i--));
            continue;
        }

        for(size_t j = 0; j < ea->size(); j++)
        {
            LVL_EventAction *ee = &(*ea)[j];
            if(ee->m_timeDelayLeft < 0.0)
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
            } else
                ee->m_timeDelayLeft -= tickTime;
            Maths::clearPrecision(ee->m_timeDelayLeft);
        }
    }
}

void LVL_EventEngine::triggerEvent(std::string event)
{
    if(event.empty())
        return;

    auto e = events.find(event);
    if(e != events.end())
        workingEvents.push_back(e->second);
}
