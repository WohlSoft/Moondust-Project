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

#include "../lvl_player.h"
#include "../lvl_scene_ptr.h"

#include <audio/pge_audio.h>


void LVL_Player::processWarpChecking()
{
    if(!contactedWithWarp) return;
    if(!contactedWarp) return;

    switch( contactedWarp->data.type )
    {
    case 1://pipe
        {
            bool isContacted=false;
       // Entrance direction: [3] down, [1] up, [2] left, [4] right

            switch(contactedWarp->data.idirect)
            {
                case 4://right
                    if(posRect.right() >= contactedWarp->right()-1.0 &&
                       posRect.right() <= contactedWarp->right()+speedX() &&
                       posRect.bottom() >= contactedWarp->bottom()-1.0 &&
                       posRect.bottom() <= contactedWarp->bottom()+32.0  ) isContacted = true;
                    break;
                case 3://down
                    if(posRect.bottom() >= contactedWarp->bottom()-1.0 &&
                       posRect.bottom() <= contactedWarp->bottom()+speedY() &&
                       posRect.center().x() >= contactedWarp->left() &&
                       posRect.center().x() <= contactedWarp->right()
                            ) isContacted = true;
                    break;
                case 2://left
                    if(posRect.left() <= contactedWarp->left()+1.0 &&
                       posRect.left() >= contactedWarp->left()+speedX() &&
                       posRect.bottom() >= contactedWarp->bottom()-1.0 &&
                       posRect.bottom() <= contactedWarp->bottom()+32.0  ) isContacted = true;
                    break;
                case 1://up
                    if(posRect.top() <= contactedWarp->top()+1.0 &&
                       posRect.top() >= contactedWarp->top()+speedY() &&
                       posRect.center().x() >= contactedWarp->left() &&
                       posRect.center().x() <= contactedWarp->right()  ) isContacted = true;
                    break;
                default:
                    break;
            }

            if(isContacted)
            {
                bool doTeleport=false;
                switch(contactedWarp->data.idirect)
                {
                    case 4://right
                        if(keys.right && !wasEntered) { setPosX(contactedWarp->right()-posRect.width()); doTeleport=true; }
                        break;
                    case 3://down
                        if(keys.down && !wasEntered) { setPosY(contactedWarp->bottom()-state_cur.height);
                            animator.switchAnimation(MatrixAnimator::PipeUpDown, _direction, 115);
                            doTeleport=true;}
                        break;
                    case 2://left
                        if(keys.left && !wasEntered) { setPosX(contactedWarp->left()); doTeleport=true; }
                        break;
                    case 1://up
                        if(keys.up && !wasEntered) {  setPosY(contactedWarp->top());
                            animator.switchAnimation(MatrixAnimator::PipeUpDown, _direction, 115);
                            doTeleport=true;}
                        break;
                    default:
                        break;
                }

                if(doTeleport)
                {
                    WarpTo(contactedWarp->data);
                    wasEntered = true;
                    wasEnteredTimeout=800;
                }
            }

        }

        break;
    case 2://door
        if(keys.up && !wasEntered)
        {
            bool isContacted=false;

            if(posRect.bottom() <= contactedWarp->bottom()+fabs(speedY()) &&
               posRect.bottom() >= contactedWarp->bottom()-fabs(speedY())-4.0 &&
               posRect.center().x() >= contactedWarp->left() &&
               posRect.center().x() <= contactedWarp->right()  ) isContacted = true;

            if(isContacted)
            {
                setPosY(contactedWarp->bottom()-posRect.height());
                WarpTo(contactedWarp->data);
                wasEntered = true;
                wasEnteredTimeout=800;
            }
        }
        break;
    case 0://Instant
    default:
        if(!wasEntered)
        {
            WarpTo(contactedWarp->data.ox, contactedWarp->data.oy, contactedWarp->data.type);
            wasEnteredTimeout=800;
            wasEntered = true;
        }
        break;
    }
}



//Enter player to level
void LVL_Player::WarpTo(float x, float y, int warpType, int warpDirection)
{
    warpFrameW = texture.w;
    warpFrameH = texture.h;

    switch(warpType)
    {
        case 2://door
        {
                setGravityScale(0);
                setSpeed(0, 0);
                EventQueueEntry<LVL_Player >event2;
                event2.makeCaller([this,x,y]()->void{
                                      isWarping=true; setPaused(true);
                                      warpPipeOffset=0.0;
                                      warpDirectO=0;
                                      teleport(x+16-_width_half,
                                                     y+32-_height);
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
    case 1://Pipe
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
                case 2://right
                    {
                        EventQueueEntry<LVL_Player >eventX;
                        eventX.makeCaller([this,x,y]()->void{
                                _direction=1;
                                animator.unlock();
                                animator.switchAnimation(MatrixAnimator::Run, _direction, 115);
                                teleport(x, y+32-_height);
                                          }, 0);
                        event_queue.events.push_back(eventX);
                    }
                    break;
                case 1://down
                    {
                        EventQueueEntry<LVL_Player >eventX;
                        eventX.makeCaller([this,x,y]()->void{
                                animator.unlock();
                                animator.switchAnimation(MatrixAnimator::PipeUpDown, _direction, 115);
                                teleport(x+16-_width_half, y);
                                          }, 0);
                        event_queue.events.push_back(eventX);
                    }
                    break;
                case 4://left
                    {
                        EventQueueEntry<LVL_Player >eventX;
                        eventX.makeCaller([this,x, y]()->void{
                                _direction=-1;
                                animator.unlock();
                                animator.switchAnimation(MatrixAnimator::Run, _direction, 115);
                                teleport(x+32-_width, y+32-_height);
                                          }, 0);
                        event_queue.events.push_back(eventX);
                    }
                    break;
                case 3://up
                    {
                        EventQueueEntry<LVL_Player >eventX;
                        eventX.makeCaller([this,x,y]()->void{
                                animator.unlock();
                                animator.switchAnimation(MatrixAnimator::PipeUpDown, _direction, 115);
                                teleport(x+16-_width_half,
                                         y+32-_height);
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

            EventQueueEntry<LVL_Player >playSnd;
            playSnd.makeCaller([this]()->void{PGE_Audio::playSoundByRole(obj_sound_role::WarpPipe);
                                        },0);
            event_queue.events.push_back(playSnd);

            float pStep = 1.5f/((float)PGE_Window::TicksPerSecond);

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
        break;
    case 0://Instant
        teleport(x+16-_width_half,
                     y+32-_height);
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
        case 1:/*******Pipe!*******/
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
                case 4://Right
                    {
                        EventQueueEntry<LVL_Player >eventX;
                        eventX.makeCaller([this,warp]()->void{
                                  warpDirectO=4;
                                  _direction=1;
                                  animator.unlock();
                                  animator.switchAnimation(MatrixAnimator::Run, _direction, 115);
                                  setPos(warp.ix+32-_width, posY());
                                          }, 0);
                        event_queue.events.push_back(eventX);
                    }
                break;
                case 3: //Down
                    {
                        EventQueueEntry<LVL_Player >eventX;
                        eventX.makeCaller([this,warp]()->void{
                                  warpDirectO=3;
                                  animator.unlock();
                                  animator.switchAnimation(MatrixAnimator::PipeUpDown, _direction, 115);
                                  setPos(posX(), warp.iy+32-_height);
                                          }, 0);
                        event_queue.events.push_back(eventX);
                    }
                break;
                case 2://Left
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
                case 1: //Up
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

            float pStep = 1.5f/((float)PGE_Window::TicksPerSecond);
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
                          LvlSceneP::s->lastWarpID=warp.array_id;
                          exitFromLevel(warp.lname, warp.warpto,
                                        warp.world_x, warp.world_y);
                                  }, 200);
                event_queue.events.push_back(event2);
            }
            else
            {
                int sID = LvlSceneP::s->findNearestSection(warp.ox, warp.oy);
                if(camera->section->id != LvlSceneP::s->levelData()->sections[sID].id)
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

                WarpTo(warp.ox, warp.oy, warp.type, warp.odirect);
            }
        }
        break;
        case 2:/*******Door!*******/
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
                          LvlSceneP::s->lastWarpID=warp.array_id;
                          exitFromLevel(warp.lname, warp.warpto,
                                        warp.world_x, warp.world_y);
                                  }, 200);
                event_queue.events.push_back(event2);

            }
            else
            {
                int sID = LvlSceneP::s->findNearestSection(warp.ox, warp.oy);
                if(camera->section->id != LvlSceneP::s->levelData()->sections[sID].id)
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
    if(!LvlSceneP::s) return;

    this->setPos(x, y);

    int sID = LvlSceneP::s->findNearestSection(x, y);
    LVL_Section* t_sct = LvlSceneP::s->getSection(sID);

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
        LvlSceneP::s->warpToLevelFile =
                LvlSceneP::s->levelData()->path+"/"+levelFile;
        LvlSceneP::s->warpToArrayID = targetWarp;
    }
    else
    {
        if((wX != -1)&&( wY != -1))
        {
            LvlSceneP::s->warpToWorld=true;
            LvlSceneP::s->warpToWorldXY.setX(wX);
            LvlSceneP::s->warpToWorldXY.setY(wY);
        }
    }
    LvlSceneP::s->setExiting(2000, LvlExit::EXIT_Warp);
}

