/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2016 Vitaly Novichkov <admin@wohlnet.ru>
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

#include "../lvl_player.h"
#include "../../scene_level.h"

#include <audio/pge_audio.h>


void LVL_Player::processWarpChecking()
{
    if(!contactedWithWarp) return;
    if(!contactedWarp) return;

    PGE_physBody::Momentum &p = m_momentum;
    PGE_physBody::Momentum &w = contactedWarp->m_momentum;

    switch( contactedWarp->data.type )
    {
    case LevelDoor::WARP_PIPE:
        {
            bool isContacted=false;
            switch(contactedWarp->data.idirect)
            {
                case LevelDoor::ENTRANCE_RIGHT:
                    if(p.right() >= w.right()-1.0 &&
                       p.rightOld() <= w.right() &&
                       p.bottom() >= w.bottom()-1.0 &&
                       p.bottom() <= w.bottom() + w.h  )
                        isContacted = true;
                    break;
                case LevelDoor::ENTRANCE_DOWN:
                    if(p.bottom() >= w.bottom()-1.0 &&
                       p.bottomOld() <= w.bottom() &&
                       p.centerX() >= w.left() &&
                       p.centerX() <= w.right() ) isContacted = true;
                    break;
                case LevelDoor::ENTRANCE_LEFT:
                    if(p.left()    <= w.left()+1.0 &&
                       p.leftOld() >= w.left() &&
                       p.bottom() >= w.bottom() - 1.0 &&
                       p.bottom() <= w.bottom() + w.h  ) isContacted = true;
                    break;
                case LevelDoor::ENTRANCE_UP:
                    if(p.top()      <= w.top()+1.0 &&
                       p.topOld()   >= w.top() &&
                       p.centerX()  >= w.left() &&
                       p.centerX()  <= w.right()  ) isContacted = true;
                    break;
                default:
                    break;
            }

            if(isContacted)
            {
                bool doTeleport=false;
                switch(contactedWarp->data.idirect)
                {
                    case LevelDoor::ENTRANCE_RIGHT:
                        if(keys.right && !wasEntered)
                        {
                            setPosX(w.right() - m_momentum.w);
                            setPosY(w.bottom() - state_cur.height);
                            doTeleport=true;
                        }
                        break;
                    case LevelDoor::ENTRANCE_DOWN:
                        if(keys.down && !wasEntered)
                        {
                            setPosY(w.bottom() - state_cur.height);
                            animator.switchAnimation(MatrixAnimator::PipeUpDown, _direction, 115);
                            doTeleport=true;
                        }
                        break;
                    case LevelDoor::ENTRANCE_LEFT:
                        if(keys.left && !wasEntered)
                        {
                            setPosX(w.left());
                            setPosY(w.bottom() - state_cur.height);
                            doTeleport=true;
                        }
                        break;
                    case LevelDoor::ENTRANCE_UP:
                        if(keys.up && !wasEntered)
                        {
                            setPosY(w.top());
                            animator.switchAnimation(MatrixAnimator::PipeUpDown, _direction, 115);
                            doTeleport=true;
                        }
                        break;
                    default:
                        break;
                }

                if(doTeleport)
                {
                    WarpTo(contactedWarp->data);
                    wasEntered = true;
                    wasEnteredTimeout=100;
                    _scene->events.triggerEvent(contactedWarp->data.event_enter);
                }
            }

        }

        break;
    case LevelDoor::WARP_DOOR:
        if(keys.up && !wasEntered)
        {
            bool isContacted=false;

            if(m_momentum.bottom() <= w.bottom() + fabs(speedY()) &&
               m_momentum.bottom() >= w.bottom() - fabs(speedY()) - 4.0 &&
               m_momentum.centerX() >= w.left() &&
               m_momentum.centerX() <= w.right()  ) isContacted = true;

            if(isContacted)
            {
                setPosX(w.centerX() - p.w/2.0);
                setPosY(w.bottom()  - p.h);
                WarpTo(contactedWarp->data);
                wasEntered = true;
                wasEnteredTimeout=100;
                _scene->events.triggerEvent(contactedWarp->data.event_enter);
            }
        }
        break;
    case LevelDoor::WARP_INSTANT:
    case LevelDoor::WARP_PORTAL:
    default:
        if(!wasEntered)
        {
            WarpTo(contactedWarp->data.ox, contactedWarp->data.oy, contactedWarp->data.type);
            wasEnteredTimeout = ((contactedWarp->data.type==LevelDoor::WARP_INSTANT) ? 400 : 200);
            wasEntered = true;
            _scene->events.triggerEvent(contactedWarp->data.event_enter);
        }
        break;
    }
}



//Enter player to level
void LVL_Player::WarpTo(float x, float y, int warpType, int warpDirection, bool cannon, float cannon_speed)
{
    warpFrameW = texture.w;
    warpFrameH = texture.h;

    switch(warpType)
    {
        case LevelDoor::WARP_DOOR:
        {
                setGravityScale(0);
                setSpeed(0, 0);
                EventQueueEntry<LVL_Player >event2;
                event2.makeCaller([this,x,y]()->void{
                                      isWarping=true; setPaused(true);
                                      warpPipeOffset=0.0;
                                      warpDirectO=0;
                                      teleport(x+16-m_width_half,
                                                     y+32-m_height_registered);
                                      animator.unlock();
                                      animator.switchAnimation(MatrixAnimator::PipeUpDown, _direction, 115);
                                  }, 0);
                event_queue.events.push_back(event2);

                EventQueueEntry<LVL_Player >fadeOutBlack;
                fadeOutBlack.makeCaller([this]()->void{
                                      if(!camera->fader.isNull())
                                          camera->fader.setFade(10, 0.0, 0.08);
                                  }, 0);
                event_queue.events.push_back(fadeOutBlack);

                EventQueueEntry<LVL_Player >event3;
                event3.makeCaller([this]()->void{
                                      isWarping=false; setSpeed(0, 0); setPaused(false);
                                      last_environment=-1;//!< Forcing to refresh physical environment
                                  }, 200);
                event_queue.events.push_back(event3);
        }
        break;
    case LevelDoor::WARP_PIPE:
        {
            // Exit direction: [1] down [3] up [4] left [2] right
            setGravityScale(0);
            setSpeed(0, 0);

            EventQueueEntry<LVL_Player >eventPipeEnter;
            eventPipeEnter.makeCaller([this,warpDirection]()->void{
                                    isWarping=true; setPaused(true);
                                    warpDirectO=warpDirection;
                                    warpPipeOffset=1.0;
                              }, 0);
            event_queue.events.push_back(eventPipeEnter);

            // Exit direction: [1] down [3] up [4] left [2] right
            switch(warpDirection)
            {
                case LevelDoor::EXIT_RIGHT:
                    {
                        EventQueueEntry<LVL_Player >eventX;
                        eventX.makeCaller([this,x,y]()->void{
                                _direction=1;
                                animator.unlock();
                                animator.switchAnimation(MatrixAnimator::Run, _direction, 115);
                                teleport(x, y+32-m_height_registered);
                                          }, 0);
                        event_queue.events.push_back(eventX);
                    }
                    break;
                case LevelDoor::EXIT_DOWN:
                    {
                        EventQueueEntry<LVL_Player >eventX;
                        eventX.makeCaller([this,x,y]()->void{
                                animator.unlock();
                                animator.switchAnimation(MatrixAnimator::PipeUpDown, _direction, 115);
                                teleport(x+16-m_width_half, y);
                                          }, 0);
                        event_queue.events.push_back(eventX);
                    }
                    break;
                case LevelDoor::EXIT_LEFT:
                    {
                        EventQueueEntry<LVL_Player >eventX;
                        eventX.makeCaller([this,x, y]()->void{
                                _direction=-1;
                                animator.unlock();
                                animator.switchAnimation(MatrixAnimator::Run, _direction, 115);
                                teleport(x+32-m_width_registered, y+32-m_height_registered);
                                          }, 0);
                        event_queue.events.push_back(eventX);
                    }
                    break;
                case LevelDoor::EXIT_UP:
                    {
                        EventQueueEntry<LVL_Player >eventX;
                        eventX.makeCaller([this,x,y]()->void{
                                animator.unlock();
                                animator.switchAnimation(MatrixAnimator::PipeUpDown, _direction, 115);
                                teleport(x+16-m_width_half,
                                         y+32-m_height_registered);
                                          }, 0);
                        event_queue.events.push_back(eventX);
                    }
                    break;
                default:
                    break;
            }

            EventQueueEntry<LVL_Player >fadeOutBlack;
            fadeOutBlack.makeCaller([this]()->void{
                                  if(!camera->fader.isNull())
                                      camera->fader.setFade(10, 0.0, 0.08);
                              }, 0);
            event_queue.events.push_back(fadeOutBlack);

            EventQueueEntry<LVL_Player >wait200ms;
            wait200ms.makeTimer(250);
            event_queue.events.push_back(wait200ms);

            if(cannon)
            {
                EventQueueEntry<LVL_Player >playSnd;
                playSnd.makeCaller([this, warpDirection, cannon_speed]()->void
                           {
                               PGE_Audio::playSoundByRole(obj_sound_role::WeaponCannon);
                               warpPipeOffset=0.0f;
                               isWarping=false;
                                       switch(warpDirection)
                                       {
                                        case LevelDoor::EXIT_RIGHT: setSpeed(cannon_speed, 0);break;
                                        case LevelDoor::EXIT_LEFT: setSpeed(-cannon_speed, 0);break;
                                        case LevelDoor::EXIT_UP: setSpeed(0, -cannon_speed);break;
                                        case LevelDoor::EXIT_DOWN: setSpeed(0, cannon_speed);break;
                                       }
                               setPaused(false);
                               last_environment=-1;//!< Forcing to refresh physical environment
                           },0);
                event_queue.events.push_back(playSnd);
            }
            else
            {
                EventQueueEntry<LVL_Player >playSnd;
                playSnd.makeCaller([this]()->void{PGE_Audio::playSoundByRole(obj_sound_role::WarpPipe);
                                            },0);
                event_queue.events.push_back(playSnd);

                float pStep = 1.5f/PGE_Window::TicksPerSecond;

                EventQueueEntry<LVL_Player >warpOut;
                warpOut.makeWaiterCond([this, pStep]()->bool{
                                          warpPipeOffset -= pStep;
                                          return warpPipeOffset<=0.0f;
                                      }, false, 0);
                event_queue.events.push_back(warpOut);

                EventQueueEntry<LVL_Player >endWarping;
                endWarping.makeCaller([this]()->void{
                                      isWarping=false; setSpeed(0, 0); setPaused(false);
                                      last_environment=-1;//!< Forcing to refresh physical environment
                                  }, 0);
                event_queue.events.push_back(endWarping);
            }
        }
        break;
    case LevelDoor::WARP_INSTANT:
        setSpeed(0, (speedY()<0?speedY():0) );//zero X speed!
    case LevelDoor::WARP_PORTAL:
        teleport(x+16-m_width_half,
                     y+32-m_height_registered);
        break;
    default:
        break;
    }
}


void LVL_Player::WarpTo(LevelDoor warp)
{
    warpFrameW = texture.w;
    warpFrameH = texture.h;

    switch(warp.type)
    {
        case LevelDoor::WARP_PIPE:/*******Pipe!*******/
        {
            setSpeed(0, 0);
            setPaused(true);

            //Create events
            EventQueueEntry<LVL_Player >event1;
            event1.makeCaller([this]()->void{
                                setSpeed(0, 0); setPaused(true);
                                isWarping=true;
                                warpPipeOffset=0.0f;
                                setDuck(false);
                                PGE_Audio::playSoundByRole(obj_sound_role::WarpPipe);
                              }, 0);
            event_queue.events.push_back(event1);

            // Entrance direction: [3] down, [1] up, [2] left, [4] right
            switch(contactedWarp->data.idirect)
            {
                case LevelDoor::ENTRANCE_RIGHT://Right
                    {
                        EventQueueEntry<LVL_Player >eventX;
                        eventX.makeCaller([this,warp]()->void{
                                  warpDirectO=4;
                                  _direction=1;
                                  animator.unlock();
                                  animator.switchAnimation(MatrixAnimator::Run, _direction, 115);
                                  setPos(warp.ix+32-m_width_registered, posY());
                                          }, 0);
                        event_queue.events.push_back(eventX);
                    }
                break;
                case LevelDoor::ENTRANCE_DOWN: //Down
                    {
                        EventQueueEntry<LVL_Player >eventX;
                        eventX.makeCaller([this,warp]()->void{
                                  warpDirectO=3;
                                  animator.unlock();
                                  animator.switchAnimation(MatrixAnimator::PipeUpDown, _direction, 115);
                                  setPos(posX(), warp.iy+32-m_height_registered);
                                          }, 0);
                        event_queue.events.push_back(eventX);
                    }
                break;
                case LevelDoor::ENTRANCE_LEFT://Left
                    {
                        EventQueueEntry<LVL_Player >eventX;
                        eventX.makeCaller([this,warp]()->void{
                                  warpDirectO=2;
                                  _direction=-1;
                                  animator.unlock();
                                  animator.switchAnimation(MatrixAnimator::Run, _direction, 115);
                                  setPos(warp.ix, posY());
                                          }, 0);
                        event_queue.events.push_back(eventX);
                    }
                break;
                case LevelDoor::ENTRANCE_UP: //Up
                    {
                        EventQueueEntry<LVL_Player >eventX;
                        eventX.makeCaller([this,warp]()->void{
                                  warpDirectO=1;
                                  animator.unlock();
                                  animator.switchAnimation(MatrixAnimator::PipeUpDown, _direction, 115);
                                  setPos(posX(), warp.iy);
                                          }, 0);
                        event_queue.events.push_back(eventX);
                    }
                break;
            }

            float pStep = 1.5f/PGE_Window::TicksPerSecond;
            EventQueueEntry<LVL_Player >warpIn;
            warpIn.makeWaiterCond([this, pStep]()->bool{
                                      warpPipeOffset += pStep;
                                      return warpPipeOffset>=1.0f;
                                  }, false, 0);
            event_queue.events.push_back(warpIn);

            EventQueueEntry<LVL_Player >wait200ms;
            wait200ms.makeTimer(250);
            event_queue.events.push_back(wait200ms);

            if( (warp.lvl_o) || (!warp.lname.isEmpty()) )
            {
                EventQueueEntry<LVL_Player >event2;
                event2.makeCaller([this, warp]()->void{
                          _scene->lastWarpID=warp.meta.array_id;
                          exitFromLevel(warp.lname, warp.warpto,
                                        warp.world_x, warp.world_y);
                                  }, 200);
                event_queue.events.push_back(event2);
            }
            else
            {
                int sID = _scene->findNearestSection(warp.ox, warp.oy);
                if(camera->section->id != _scene->levelData()->sections[sID].id)
                {
                    EventQueueEntry<LVL_Player >event3;
                    event3.makeCaller([this]()->void{
                                          camera->fader.setFade(10, 1.0, 0.08);
                                      }, 0);
                    event_queue.events.push_back(event3);
                }

                EventQueueEntry<LVL_Player >whileOpacityFade;
                whileOpacityFade.makeWaiterCond([this]()->bool{
                                          return camera->fader.isFading();
                                      }, true, 100);
                event_queue.events.push_back(whileOpacityFade);

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
            event1.makeCaller([this]()->void{
                                setSpeed(0, 0); setPaused(true); isWarping=true;
                                warpPipeOffset=0.0;
                                warpDirectO=0;
                                setDuck(false);
                                animator.unlock();
                                animator.switchAnimation(MatrixAnimator::PipeUpDownRear, _direction, 115);
                                PGE_Audio::playSoundByRole(obj_sound_role::WarpDoor);
                              }, 0);
            event_queue.events.push_back(event1);

            EventQueueEntry<LVL_Player >event2;
            event2.makeTimer(200);
            event_queue.events.push_back(event2);

            if( (warp.lvl_o) || (!warp.lname.isEmpty()) )
            {
                EventQueueEntry<LVL_Player >event2;
                event2.makeCaller([this, warp]()->void{
                          _scene->lastWarpID=warp.meta.array_id;
                          exitFromLevel(warp.lname, warp.warpto,
                                        warp.world_x, warp.world_y);
                                  }, 200);
                event_queue.events.push_back(event2);

            }
            else
            {
                int sID = _scene->findNearestSection(warp.ox, warp.oy);
                if(camera->section->id != _scene->levelData()->sections[sID].id)
                {
                    EventQueueEntry<LVL_Player >event3;
                    event3.makeCaller([this]()->void{
                                          camera->fader.setFade(10, 1.0, 0.08);
                                      }, 0);
                    event_queue.events.push_back(event3);
                }

                EventQueueEntry<LVL_Player >event4;
                event4.makeWaiterCond([this]()->bool{
                                          return camera->fader.isFading();
                                      }, true, 100);
                event_queue.events.push_back(event4);

                WarpTo(warp.ox, warp.oy, warp.type, warp.odirect);
            }
        break;
        }
    }
    event_queue.processEvents(0);
}


void LVL_Player::teleport(float x, float y)
{
    if(!_scene) return;

    this->setPos(x, y);

    int sID = _scene->findNearestSection(x, y);
    LVL_Section* t_sct = _scene->getSection(sID);

    if(t_sct)
    {
        camera->changeSection(t_sct);
        setParentSection(t_sct);
    }
    else
    {
        camera->resetLimits();
    }
}




void LVL_Player::exitFromLevel(QString levelFile, int targetWarp, long wX, long wY)
{
    isAlive = false;
    if(!levelFile.isEmpty())
    {
        _scene->warpToLevelFile =
                _scene->levelData()->meta.path+"/"+levelFile;
        _scene->warpToArrayID = targetWarp;
    }
    else
    {
        if((wX != -1)&&( wY != -1))
        {
            _scene->warpToWorld=true;
            _scene->warpToWorldXY.setX(wX);
            _scene->warpToWorldXY.setY(wY);
        }
    }
    _scene->setExiting(2000, LvlExit::EXIT_Warp);
}

