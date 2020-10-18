/*
 * Moondust, a free game engine for platform game making
 * Copyright (c) 2014-2020 Vitaly Novichkov <admin@wohlnet.ru>
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

#include "../lvl_player.h"
#include "../../scene_level.h"

#include <audio/pge_audio.h>
#include <Utils/dir_list_ci.h>


void LVL_Player::processWarpChecking()
{
    if(!m_contactedWithWarp) return;

    if(!m_contactedWarp) return;

    PGE_physBody::Momentum &p = m_momentum;
    PGE_physBody::Momentum &w = m_contactedWarp->m_momentum;

    switch(m_contactedWarp->data.type)
    {
    case LevelDoor::WARP_PIPE:
    {
        bool isContacted = false;

        switch(m_contactedWarp->data.idirect)
        {
        case LevelDoor::ENTRANCE_RIGHT:
            if(p.right() >= w.right() - 1.0 &&
               p.rightOld() <= w.right() &&
               p.bottom() >= w.bottom() - 1.0 &&
               p.bottom() <= w.bottom() + w.h)
                isContacted = true;

            break;

        case LevelDoor::ENTRANCE_DOWN:
            if(p.bottom() >= w.bottom() - 1.0 &&
               p.bottomOld() <= w.bottom() &&
               p.centerX() >= w.left() &&
               p.centerX() <= w.right()) isContacted = true;

            break;

        case LevelDoor::ENTRANCE_LEFT:
            if(p.left()    <= w.left() + 1.0 &&
               p.leftOld() >= w.left() &&
               p.bottom() >= w.bottom() - 1.0 &&
               p.bottom() <= w.bottom() + w.h) isContacted = true;

            break;

        case LevelDoor::ENTRANCE_UP:
            if(p.top()      <= w.top() + 1.0 &&
               p.topOld()   >= w.top() &&
               p.centerX()  >= w.left() &&
               p.centerX()  <= w.right()) isContacted = true;

            break;

        default:
            break;
        }

        if(isContacted)
        {
            bool doTeleport = false;

            switch(m_contactedWarp->data.idirect)
            {
            case LevelDoor::ENTRANCE_RIGHT:
                if(keys.right && !m_wasEntered)
                {
                    setPosX(w.right() - m_momentum.w);
                    setPosY(w.bottom() - state_cur.height);
                    doTeleport = true;
                }

                break;

            case LevelDoor::ENTRANCE_DOWN:
                if(keys.down && !m_wasEntered)
                {
                    setPosY(w.bottom() - state_cur.height);
                    m_animator.switchAnimation(MatrixAnimator::PipeUpDown, m_direction, 115);
                    doTeleport = true;
                }

                break;

            case LevelDoor::ENTRANCE_LEFT:
                if(keys.left && !m_wasEntered)
                {
                    setPosX(w.left());
                    setPosY(w.bottom() - state_cur.height);
                    doTeleport = true;
                }

                break;

            case LevelDoor::ENTRANCE_UP:
                if(keys.up && !m_wasEntered)
                {
                    setPosY(w.top());
                    m_animator.switchAnimation(MatrixAnimator::PipeUpDown, m_direction, 115);
                    doTeleport = true;
                }

                break;

            default:
                break;
            }

            if(doTeleport)
            {
                WarpTo(m_contactedWarp->data);
                m_wasEntered = true;
                m_wasEnteredTimeout = 100;
                m_scene->m_events.triggerEvent(m_contactedWarp->data.event_enter);
            }
        }
    }
    break;

    case LevelDoor::WARP_DOOR:
        if(keys.up && !m_wasEntered)
        {
            bool isContacted = false;

            if(m_momentum.bottom() <= w.bottom() + fabs(speedY()) &&
               m_momentum.bottom() >= w.bottom() - fabs(speedY()) - 4.0 &&
               m_momentum.centerX() >= w.left() &&
               m_momentum.centerX() <= w.right()) isContacted = true;

            if(isContacted)
            {
                setPosX(w.centerX() - p.w / 2.0);
                setPosY(w.bottom()  - p.h);
                WarpTo(m_contactedWarp->data);
                m_wasEntered = true;
                m_wasEnteredTimeout = 100;
                m_scene->m_events.triggerEvent(m_contactedWarp->data.event_enter);
            }
        }

        break;

    case LevelDoor::WARP_INSTANT:
    case LevelDoor::WARP_PORTAL:
    default:
        if(!m_wasEntered)
        {
            WarpTo(m_contactedWarp->data.ox, m_contactedWarp->data.oy, m_contactedWarp->data.type);
            m_wasEnteredTimeout = ((m_contactedWarp->data.type == LevelDoor::WARP_INSTANT) ? 400 : 200);
            m_wasEntered = true;
            m_scene->m_events.triggerEvent(m_contactedWarp->data.event_enter);
        }

        break;
    }
}



//Enter player to level
void LVL_Player::WarpTo(double x, double y, int warpType, int warpDirection, bool cannon, double cannon_speed)
{
    m_warpFrameW = texture.w;
    m_warpFrameH = texture.h;

    switch(warpType)
    {
    case LevelDoor::WARP_DOOR:
    {
        setGravityScale(0);
        setSpeed(0, 0);
        EventQueueEntry<LVL_Player >event2;
        event2.makeCaller([this, x, y]()->void
        {
            m_isWarping = true; setPaused(true);
            m_warpPipeOffset = 0.0;
            m_warpDirectO = 0;
            teleport(x + 16 - (m_momentum.w / 2.0),
            y + 32 - m_momentum.h);
            m_animator.unlock();
            m_animator.switchAnimation(MatrixAnimator::PipeUpDown, m_direction, 115);
        }, 0);
        m_eventQueue.events.push_back(event2);
        EventQueueEntry<LVL_Player >fadeOutBlack;
        fadeOutBlack.makeCaller([this]()->void
        {
            if(!camera->fader.isNull())
                camera->fader.setFade(10, 0.0, 0.08);
        }, 0);
        m_eventQueue.events.push_back(fadeOutBlack);
        EventQueueEntry<LVL_Player >event3;
        event3.makeCaller([this]()->void
        {
            m_isWarping = false; setSpeed(0, 0); setPaused(false);
            last_environment = -1; //!< Forcing to refresh physical environment
        }, 200);
        m_eventQueue.events.push_back(event3);
    }
    break;

    case LevelDoor::WARP_PIPE:
    {
        // Exit direction: [1] down [3] up [4] left [2] right
        setGravityScale(0);
        setSpeed(0, 0);
        EventQueueEntry<LVL_Player >eventPipeEnter;
        eventPipeEnter.makeCaller([this, warpDirection]()->void
        {
            m_isWarping = true; setPaused(true);
            m_warpDirectO = warpDirection;
            m_warpPipeOffset = 1.0;
        }, 0);
        m_eventQueue.events.push_back(eventPipeEnter);

        // Exit direction: [1] down [3] up [4] left [2] right
        switch(warpDirection)
        {
        case LevelDoor::EXIT_RIGHT:
        {
            EventQueueEntry<LVL_Player >eventX;
            eventX.makeCaller([this, x, y]()->void
            {
                m_direction = 1;
                m_animator.unlock();
                m_animator.switchAnimation(MatrixAnimator::Run, m_direction, 115);
                teleport(x, y + 32 - m_momentum.h);
            }, 0);
            m_eventQueue.events.push_back(eventX);
        }
        break;

        case LevelDoor::EXIT_DOWN:
        {
            EventQueueEntry<LVL_Player >eventX;
            eventX.makeCaller([this, x, y]()->void
            {
                m_animator.unlock();
                m_animator.switchAnimation(MatrixAnimator::PipeUpDown, m_direction, 115);
                teleport(x + 16.0 - (m_momentum.w / 2.0), y);
            }, 0);
            m_eventQueue.events.push_back(eventX);
        }
        break;

        case LevelDoor::EXIT_LEFT:
        {
            EventQueueEntry<LVL_Player >eventX;
            eventX.makeCaller([this, x, y]()->void
            {
                m_direction = -1;
                m_animator.unlock();
                m_animator.switchAnimation(MatrixAnimator::Run, m_direction, 115);
                teleport(x + 32 - m_momentum.w, y + 32 - m_momentum.h);
            }, 0);
            m_eventQueue.events.push_back(eventX);
        }
        break;

        case LevelDoor::EXIT_UP:
        {
            EventQueueEntry<LVL_Player >eventX;
            eventX.makeCaller([this, x, y]()->void
            {
                m_animator.unlock();
                m_animator.switchAnimation(MatrixAnimator::PipeUpDown, m_direction, 115);
                teleport(x + 16 - (m_momentum.w/2.0),
                y + 32 - m_momentum.h);
            }, 0);
            m_eventQueue.events.push_back(eventX);
        }
        break;

        default:
            break;
        }

        EventQueueEntry<LVL_Player >fadeOutBlack;
        fadeOutBlack.makeCaller([this]()->void
        {
            if(!camera->fader.isNull())
                camera->fader.setFade(10, 0.0, 0.08);
        }, 0);
        m_eventQueue.events.push_back(fadeOutBlack);
        EventQueueEntry<LVL_Player >wait200ms;
        wait200ms.makeTimer(250);
        m_eventQueue.events.push_back(wait200ms);

        if(cannon)
        {
            EventQueueEntry<LVL_Player >playSnd;
            playSnd.makeCaller([this, warpDirection, cannon_speed]()->void
            {
                PGE_Audio::playSoundByRole(obj_sound_role::WeaponCannon);
                m_warpPipeOffset = 0.0;
                m_isWarping = false;

                switch(warpDirection)
                {
                case LevelDoor::EXIT_RIGHT:
                    setSpeed(cannon_speed, 0);
                    break;

                case LevelDoor::EXIT_LEFT:
                    setSpeed(-cannon_speed, 0);
                    break;

                case LevelDoor::EXIT_UP:
                    setSpeed(0, -cannon_speed);
                    break;

                case LevelDoor::EXIT_DOWN:
                    setSpeed(0, cannon_speed);
                    break;
                }
                setPaused(false);
                last_environment = -1; //!< Forcing to refresh physical environment
            }, 0);
            m_eventQueue.events.push_back(playSnd);
        }
        else
        {
            EventQueueEntry<LVL_Player >playSnd;
            playSnd.makeCaller([this]()->void{PGE_Audio::playSoundByRole(obj_sound_role::WarpPipe);
                                             }, 0);
            m_eventQueue.events.push_back(playSnd);
            double pStep = 1.5 / (1000.0 / m_scene->frameDelay());
            EventQueueEntry<LVL_Player >warpOut;
            warpOut.makeWaiterCond([this, pStep]()->bool
            {
                m_warpPipeOffset -= pStep;
                return m_warpPipeOffset <= 0.0f;
            }, false, 0);
            m_eventQueue.events.push_back(warpOut);
            EventQueueEntry<LVL_Player >endWarping;
            endWarping.makeCaller([this]()->void
            {
                m_isWarping = false; setSpeed(0, 0); setPaused(false);
                last_environment = -1; //!< Forcing to refresh physical environment
            }, 0);
            m_eventQueue.events.push_back(endWarping);
        }
    }
    break;

    case LevelDoor::WARP_INSTANT:
        setSpeed(0, (speedY() < 0 ? speedY() : 0)); /*fallthrough*/ //zero X speed!

    case LevelDoor::WARP_PORTAL:
        teleport(x + 16 - (m_momentum.w/2.0),
                 y + 32 - m_momentum.h);
        break;

    default:
        break;
    }
}


void LVL_Player::WarpTo(const LevelDoor &warp)
{
    m_warpFrameW = texture.w;
    m_warpFrameH = texture.h;

    switch(warp.type)
    {
    case LevelDoor::WARP_PIPE:/*******Pipe!*******/
    {
        setSpeed(0, 0);
        setPaused(true);
        //Create events
        EventQueueEntry<LVL_Player >event1;
        event1.makeCaller([this]()->void
        {
            setSpeed(0, 0); setPaused(true);
            setDuck(false);
            m_isWarping = true;
            m_warpPipeOffset = 0.0;
            PGE_Audio::playSoundByRole(obj_sound_role::WarpPipe);
        }, 0);
        m_eventQueue.events.push_back(event1);

        // Entrance direction: [3] down, [1] up, [2] left, [4] right
        switch(m_contactedWarp->data.idirect)
        {
        case LevelDoor::ENTRANCE_RIGHT://Right
        {
            EventQueueEntry<LVL_Player >eventX;
            eventX.makeCaller([this, warp]()->void
            {
                m_warpDirectO = 4;
                m_direction = 1;
                m_animator.unlock();
                m_animator.switchAnimation(MatrixAnimator::Run, m_direction, 115);
                setPos(warp.ix + 32 - m_momentum.w, posY());
            }, 0);
            m_eventQueue.events.push_back(eventX);
        }
        break;

        case LevelDoor::ENTRANCE_DOWN: //Down
        {
            EventQueueEntry<LVL_Player >eventX;
            eventX.makeCaller([this, warp]()->void
            {
                m_warpDirectO = 3;
                m_animator.unlock();
                m_animator.switchAnimation(MatrixAnimator::PipeUpDown, m_direction, 115);
                setPos(posX(), warp.iy + 32 - m_momentum.h);
            }, 0);
            m_eventQueue.events.push_back(eventX);
        }
        break;

        case LevelDoor::ENTRANCE_LEFT://Left
        {
            EventQueueEntry<LVL_Player >eventX;
            eventX.makeCaller([this, warp]()->void
            {
                m_warpDirectO = 2;
                m_direction = -1;
                m_animator.unlock();
                m_animator.switchAnimation(MatrixAnimator::Run, m_direction, 115);
                setPos(warp.ix, posY());
            }, 0);
            m_eventQueue.events.push_back(eventX);
        }
        break;

        case LevelDoor::ENTRANCE_UP: //Up
        {
            EventQueueEntry<LVL_Player >eventX;
            eventX.makeCaller([this, warp]()->void
            {
                m_warpDirectO = 1;
                m_animator.unlock();
                m_animator.switchAnimation(MatrixAnimator::PipeUpDown, m_direction, 115);
                setPos(posX(), warp.iy);
            }, 0);
            m_eventQueue.events.push_back(eventX);
        }
        break;
        }

        double pStep = 1.5 / (1000.0 / m_scene->frameDelay());
        EventQueueEntry<LVL_Player >warpIn;
        warpIn.makeWaiterCond([this, pStep]()->bool
        {
            m_warpPipeOffset += pStep;
            return m_warpPipeOffset >= 1.0;
        }, false, 0);
        m_eventQueue.events.push_back(warpIn);
        EventQueueEntry<LVL_Player >wait200ms;
        wait200ms.makeTimer(250);
        m_eventQueue.events.push_back(wait200ms);

        if((warp.lvl_o) || (!warp.lname.empty()))
        {
            EventQueueEntry<LVL_Player >event2;
            event2.makeCaller([this, warp]()->void
            {
                m_scene->m_lastWarpID = static_cast<unsigned long>(warp.meta.array_id);
                exitFromLevel(warp.lname, static_cast<unsigned long>(warp.warpto),
                warp.world_x, warp.world_y);
            }, 200);
            m_eventQueue.events.push_back(event2);
        }
        else
        {
            int sID = m_scene->findNearestSection(warp.ox, warp.oy);

            if(camera->section->id != m_scene->levelData()->sections[sID].id)
            {
                EventQueueEntry<LVL_Player >event3;
                event3.makeCaller([this]()->void
                {
                    camera->fader.setFade(10, 1.0, 0.08);
                }, 0);
                m_eventQueue.events.push_back(event3);
            }
            else
            {
                EventQueueEntry<LVL_Player >initCameraMover;
                initCameraMover.makeCaller([this, warp]()->void
                {
                    double x = warp.ox;
                    double y = warp.oy;
                    double targetX = x;
                    double targetY = y;
                    double w = m_momentum.w;
                    double h = m_momentum.h;

                    switch(warp.odirect)
                    {
                    case LevelDoor::EXIT_RIGHT:
                        targetX = x;
                        targetY = y + 32 - h;
                        break;

                    case LevelDoor::EXIT_LEFT:
                        targetX = x + 32 - w;
                        targetY = y + 32 - h;
                        break;

                    case LevelDoor::EXIT_UP:
                        targetX = x + 16 - (w / 2.0);
                        targetY = y + 32 - h;
                        break;

                    case LevelDoor::EXIT_DOWN:
                        targetX = x + 16 - (w / 2.0);
                        targetY = y;
                        break;

                    default:
                        break;
                    }
                    m_cameraMover.startAuto(posX(), posY(), targetX, targetY, 1500);
                }, 0);
                m_eventQueue.events.push_back(initCameraMover);
                EventQueueEntry<LVL_Player >whileOpacityFade;
                whileOpacityFade.makeWaiterCond([this]()->bool
                {
                    bool is = m_cameraMover.iterate(15.3);
                    setPos(m_cameraMover.posX(), m_cameraMover.posY());
                    return is;
                }, true, 0);
                m_eventQueue.events.push_back(whileOpacityFade);
            }

            EventQueueEntry<LVL_Player >whileOpacityFade;
            whileOpacityFade.makeWaiterCond([this]()->bool
            {
                return camera->fader.isFading();
            }, true, 100);
            m_eventQueue.events.push_back(whileOpacityFade);
            WarpTo(warp.ox, warp.oy, warp.type, warp.odirect, warp.cannon_exit, warp.cannon_exit_speed);
        }
    }
    break;

    case LevelDoor::WARP_DOOR:/*******Door!*******/
    {
        setSpeed(0, 0);
        setPaused(true);
        //Create events
        EventQueueEntry<LVL_Player >event1;
        event1.makeCaller([this]()->void
        {
            setSpeed(0, 0); setPaused(true); m_isWarping = true;
            m_warpPipeOffset = 0.0;
            m_warpDirectO = 0;
            setDuck(false);
            m_animator.unlock();
            m_animator.switchAnimation(MatrixAnimator::PipeUpDownRear, m_direction, 115);
            PGE_Audio::playSoundByRole(obj_sound_role::WarpDoor);
        }, 0);
        m_eventQueue.events.push_back(event1);
        EventQueueEntry<LVL_Player >event2;
        event2.makeTimer(200);
        m_eventQueue.events.push_back(event2);

        if((warp.lvl_o) || (!warp.lname.empty()))
        {
            EventQueueEntry<LVL_Player >event2;
            event2.makeCaller([this, warp]()->void
            {
                m_scene->m_lastWarpID = static_cast<unsigned long>(warp.meta.array_id);
                exitFromLevel(warp.lname, static_cast<unsigned long>(warp.warpto),
                warp.world_x, warp.world_y);
            }, 200);
            m_eventQueue.events.push_back(event2);
        }
        else
        {
            int sID = m_scene->findNearestSection(warp.ox, warp.oy);

            if(camera->section->id != m_scene->levelData()->sections[sID].id)
            {
                EventQueueEntry<LVL_Player >event3;
                event3.makeCaller([this]()->void
                {
                    camera->fader.setFade(10, 1.0, 0.08);
                }, 0);
                m_eventQueue.events.push_back(event3);
            }

            EventQueueEntry<LVL_Player >event4;
            event4.makeWaiterCond([this]()->bool
            {
                return camera->fader.isFading();
            }, true, 100);
            m_eventQueue.events.push_back(event4);
            WarpTo(warp.ox, warp.oy, warp.type, warp.odirect);
        }

        break;
    }
    }

    m_eventQueue.processEvents(0);
}


void LVL_Player::teleport(double x, double y)
{
    if(!m_scene) return;

    this->setPos(x, y);
    int sID = m_scene->findNearestSection(static_cast<long>(x), static_cast<long>(y));
    LVL_Section *t_sct = m_scene->getSection(sID);

    if(t_sct)
    {
        camera->changeSection(t_sct);
        setParentSection(t_sct);
    }
    else
        camera->resetLimits();
}




void LVL_Player::exitFromLevel(std::string levelFile, unsigned long targetWarp, long wX, long wY)
{
    isAlive = false;

    if(!levelFile.empty())
    {
        DirListCI ci(m_scene->levelData()->meta.path);
        m_scene->m_warpToLevelFile = m_scene->levelData()->meta.path + "/" + ci.resolveFileCase(levelFile);
        m_scene->m_warpToArrayID = targetWarp;
    }
    else
    {
        if((wX != -1) && (wY != -1))
        {
            m_scene->m_warpToWorld = true;
            m_scene->m_warpToWorldXY.setX(static_cast<int>(wX));
            m_scene->m_warpToWorldXY.setY(static_cast<int>(wY));
        }
    }

    m_scene->setExiting(2000, LvlExit::EXIT_Warp);
}
