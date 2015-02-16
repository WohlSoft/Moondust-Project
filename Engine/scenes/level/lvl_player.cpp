/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2014 Vitaly Novichkov <admin@wohlnet.ru>
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

#include "lvl_player.h"
#include "../../graphics/window.h"
#include "../../data_configs/config_manager.h"

#include "lvl_scene_ptr.h"

#include <QtDebug>

LVL_Player::LVL_Player()
{
    camera = NULL;
    worldPtr = NULL;
    playerID = 0;

    type = LVLPlayer;

    force=900.0f;
    hMaxSpeed=20.0f;
    hRunningMaxSpeed=38.0f;
    fallMaxSpeed=720.0f;

    JumpPressed=false;
    onGround=true;

    climbing=false;

    bumpDown=false;
    bumpUp=false;
    bumpVelocity=0.0f;

    health=3;
    doHarm=false;
    doHarm_damage=0;

    foot_contacts=0;
    jumpForce=0;

    isLive = true;
    doKill = false;
    kill_reason=DEAD_fall;

    curHMaxSpeed = hMaxSpeed;
    isRunning = false;

    contactedWithWarp = false;
    contactedWarp = NULL;
    wasTeleported = false;
    wasEntered = false;
    warpsTouched = 0;

    isWarping=false;
    warpDo=false;
    warpDirect=0;
    warpWaitTicks=0;

    //floating
    allowFloat=true;
    isFloating=false;
    timeToFloat=0;
    maxFloatTime=1500;

    gscale_Backup=0.f;
}

LVL_Player::~LVL_Player()
{
    qDebug() << "Destroy player";

    if(physBody && worldPtr)
    {
        worldPtr->DestroyBody(physBody);
        physBody->SetUserData(NULL);
        physBody = NULL;
    }
}

void LVL_Player::init()
{
    if(!worldPtr) return;

    setSize(data.w, data.h);

    playerID = data.id;

    b2BodyDef bodyDef;
    bodyDef.type = b2_dynamicBody;
    bodyDef.position.Set
            (
                PhysUtil::pix2met((float)data.x + posX_coefficient),
                PhysUtil::pix2met((float)data.y + posY_coefficient)
            );

//    bodyDef.position.Set(PhysUtil::pix2met((float)data.x + ((float)data.w/2)),
//            PhysUtil::pix2met((float)data.y + ((float)data.w/2) ) );

    bodyDef.fixedRotation = true;
    bodyDef.bullet = true;
    bodyDef.userData = (void*)dynamic_cast<PGE_Phys_Object *>(this);

    physBody = worldPtr->CreateBody(&bodyDef);

    b2PolygonShape shape;
    shape.SetAsBox(PhysUtil::pix2met(posX_coefficient),
                   PhysUtil::pix2met(posY_coefficient));

    b2FixtureDef fixtureDef;
    fixtureDef.shape = &shape;
    fixtureDef.density = 1.0f; fixtureDef.friction = 0.3f;
    physBody->CreateFixture(&fixtureDef);

    //qDebug() <<"Start position is " << posX() << posY();
}

void LVL_Player::update(float ticks)
{
    if(!physBody) return;
    if(!camera) return;

    if(_player_moveup)
    {
        physBody->SetTransform(b2Vec2(
             physBody->GetPosition().x,
             physBody->GetPosition().y-0.2), 0.0f);
        _player_moveup = false;
    }

    if(doKill)
    {
        doKill=false;
        isLive = false;
        physBody->SetActive(false);
        LvlSceneP::s->checkPlayers();
    }

    if(climbing)
    {
        if(gscale_Backup != 1)
        {
            physBody->SetGravityScale(0);
            gscale_Backup = 1;
        }
    }
    else
    {
        if(gscale_Backup != 0.f)
        {
            physBody->SetGravityScale(1);
            gscale_Backup = 0.f;
        }
    }

    if(climbing)
    {
        physBody->SetLinearVelocity(b2Vec2(0, 0));
    }
    else
    {
        if(physBody->GetLinearVelocity().y > 72)
            physBody->SetLinearVelocity(b2Vec2(physBody->GetLinearVelocity().x, 72));
    }


    //Running key
    if(keys.run)
    {
        if(!isRunning)
        {
            curHMaxSpeed = hRunningMaxSpeed;
            isRunning=true;
        }
    }
    else
    {
        if(isRunning)
        {
            curHMaxSpeed = hMaxSpeed;
            isRunning=false;
        }
    }


    //if
    if(!keys.up && !keys.down && !keys.left && !keys.right)
    {
        if(wasEntered) wasEntered = false;
    }


    if(keys.up)
    {
        if(climbing)
        {
            physBody->SetLinearVelocity(b2Vec2(physBody->GetLinearVelocity().x, -15));
        }
        else
        if(climbable_map.size()>0)
        {
            climbing=true;
        }
    }

    if(keys.down)
    {
        if(climbing)
        {
            physBody->SetLinearVelocity(b2Vec2(physBody->GetLinearVelocity().x, 15));
        }
        else
        if(climbable_map.size()>0)
        {
            climbing=true;
        }
    }

    //If left key is pressed
    if(keys.right)
    {
        if(climbing)
        {
            physBody->SetLinearVelocity(b2Vec2(15, physBody->GetLinearVelocity().y));
        }
        else
        {
        if(physBody->GetLinearVelocity().x <= curHMaxSpeed)
            physBody->ApplyForceToCenter(b2Vec2(force, 0.0f), true);
        }
    }

    //If right key is pressed
    if(keys.left)
    {
        if(climbing)
        {
            physBody->SetLinearVelocity(b2Vec2(-15, physBody->GetLinearVelocity().y));
        }
        else
        {
            if(physBody->GetLinearVelocity().x >= -curHMaxSpeed)
                physBody->ApplyForceToCenter(b2Vec2(-force, 0.0f), true);
        }
    }

    if( keys.jump )
    {
        if(!JumpPressed)
        {
            JumpPressed=true;
            if(onGround || climbing)
            {
                climbing=false;
                jumpForce=20;
                timeToFloat = maxFloatTime;
                physBody->SetLinearVelocity(b2Vec2(physBody->GetLinearVelocity().x, -37.0f-fabs(physBody->GetLinearVelocity().x/6)));
            }
            else
            if((allowFloat)&&(timeToFloat>0))
            {
                isFloating=true;
                physBody->SetLinearVelocity(b2Vec2(physBody->GetLinearVelocity().x, 0));
                physBody->SetGravityScale(0);
            }
        }
        else
        {
            if(jumpForce>0)
            {
                jumpForce--;
                physBody->SetLinearVelocity(b2Vec2(physBody->GetLinearVelocity().x, -37.0f-fabs(physBody->GetLinearVelocity().x/6)));
            }

            if(isFloating)
            {
                timeToFloat -= ticks;
                physBody->SetLinearVelocity(b2Vec2(physBody->GetLinearVelocity().x, 3.5*sin(timeToFloat/60.0)) );
                if(timeToFloat<=0)
                {
                    timeToFloat=0;
                    isFloating=false;
                    physBody->SetGravityScale((int)(!climbing));
                }
            }

        }
    }
    else
    {
        jumpForce=0;
        if(JumpPressed)
        {
            JumpPressed=false;
            if(allowFloat)
            {
                if(isFloating)
                {
                    timeToFloat=0;
                    isFloating=false;
                    physBody->SetGravityScale((int)(!climbing));
                }
            }
        }
    }


    //Return player to start position on fall down
    if( posY() > camera->limitBottom+height )
    {
        kill(DEAD_fall);
    }

    if(bumpDown)
    {
        bumpDown=false;
        physBody->SetLinearVelocity(b2Vec2(physBody->GetLinearVelocity().x, bumpVelocity));
    }
    else
    if(bumpUp)
    {
        bumpUp=false;
        physBody->SetLinearVelocity(
                                b2Vec2(physBody->GetLinearVelocity().x,
                                (keys.jump?(-75.f-fabs(physBody->GetLinearVelocity().x/6))
                                                                                    :-32.5f)
                                           )
                                    );
    }


    //Connection of section opposite sides
    if(camera->isWarp)
    {
        if(posX() < camera->limitLeft-width-1 )
            physBody->SetTransform(b2Vec2(
                 PhysUtil::pix2met(camera->limitRight+posX_coefficient-1),
                 physBody->GetPosition().y), 0.0f);
        else
        if(posX() > camera->limitRight + 1 )
            physBody->SetTransform(b2Vec2(
                 PhysUtil::pix2met(camera->limitLeft-posX_coefficient+1 ),
                 physBody->GetPosition().y), 0.0f
                                   );
    }
    else
    {

        if(camera->ExitOffscreen)
        {
            if(camera->RightOnly)
            {
                if( posX() < camera->limitLeft)
                {
                    physBody->SetTransform(b2Vec2(
                         PhysUtil::pix2met(camera->limitLeft + posX_coefficient),
                            physBody->GetPosition().y), 0.0f);

                    physBody->SetLinearVelocity(b2Vec2(0, physBody->GetLinearVelocity().y));
                }
            }

            if((posX() < camera->limitLeft-width-1 ) || (posX() > camera->limitRight + 1 ))
            {
                LvlSceneP::s->setExiting(1000, LevelScene::EXIT_OffScreen);
            }
        }
        else
        {
            //Prevent moving of player away from screen
            if( posX() < camera->limitLeft)
            {
                physBody->SetTransform(b2Vec2(
                     PhysUtil::pix2met(camera->limitLeft + posX_coefficient),
                        physBody->GetPosition().y), 0.0f);

                physBody->SetLinearVelocity(b2Vec2(0, physBody->GetLinearVelocity().y));
            }
            else
            if( posX()+width > camera->limitRight)
            {
                physBody->SetTransform(b2Vec2(
                     PhysUtil::pix2met(camera->limitRight-posX_coefficient ),
                        physBody->GetPosition().y), 0.0f
                                       );
                physBody->SetLinearVelocity(b2Vec2(0, physBody->GetLinearVelocity().y));
            }
        }
    }

    if(contactedWithWarp)
    {
        if(contactedWarp)
        {

            switch( contactedWarp->data.type )
            {
            case 1://pipe
                {
                    bool isContacted=false;
               // Entrance direction: [3] down, [1] up, [2] left, [4] right

                    switch(contactedWarp->data.idirect)
                    {
                        case 4://right
                            if(this->right() >= contactedWarp->right()-1 &&
                               this->right() <= contactedWarp->right() &&
                               this->bottom() >= contactedWarp->bottom()-1 &&
                               this->bottom() <= contactedWarp->bottom()  ) isContacted = true;
                            break;
                        case 3://down
                            if(this->bottom() >= contactedWarp->bottom()-1 &&
                               this->bottom() <= contactedWarp->bottom()) isContacted = true;
                            break;
                        case 2://left
                            if(this->left() <= contactedWarp->left()+1 &&
                               this->left() >= contactedWarp->left() &&
                               this->bottom() >= contactedWarp->bottom()-1 &&
                               this->bottom() <= contactedWarp->bottom()  ) isContacted = true;
                            break;
                        case 1://up
                            if(this->top() <= contactedWarp->top()+1 &&
                               this->top() >= contactedWarp->top()) isContacted = true;
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
                                if(keys.right && !wasEntered) doTeleport=true;
                                break;
                            case 3://down
                                if(keys.down && !wasEntered) doTeleport=true;
                                break;
                            case 2://left
                                if(keys.left && !wasEntered) doTeleport=true;
                                break;
                            case 1://up
                                if(keys.up && !wasEntered) doTeleport=true;
                                break;
                            default:
                                break;
                        }

                        if(doTeleport)
                        {

                            if( (contactedWarp->data.lvl_o) || (!contactedWarp->data.lname.isEmpty()) )
                            {
                                exitFromLevel(contactedWarp->data.lname, contactedWarp->data.warpto);
                            }
                            else
                            {
                                // Exit direction: [1] down [3] up [4] left [2] right
                                physBody->SetLinearVelocity(b2Vec2(0, 0));
                                switch(contactedWarp->data.odirect)
                                {
                                    case 2://right
                                        teleport(contactedWarp->data.ox,
                                                     contactedWarp->data.oy+32-height);
                                        break;
                                    case 1://down
                                        teleport(contactedWarp->data.ox+16-posX_coefficient,
                                                     contactedWarp->data.oy);
                                        break;
                                    case 4://left
                                        teleport(contactedWarp->data.ox+32-width,
                                                     contactedWarp->data.oy+32-height);
                                        if(keys.left && !wasEntered) doTeleport=true;
                                        break;
                                    case 3://up
                                        teleport(contactedWarp->data.ox+16-posX_coefficient,
                                                     contactedWarp->data.oy+32-height);
                                        break;
                                    default:
                                        break;
                                }
                            }
                            wasEntered = true;
                        }
                    }

                }

                break;
            case 2://door
                if(keys.up && !wasEntered)
                {
                    if(!wasTeleported)
                    {
                        if( (contactedWarp->data.lvl_o) || (!contactedWarp->data.lname.isEmpty()) )
                        {
                            exitFromLevel(contactedWarp->data.lname, contactedWarp->data.warpto);
                        }
                        else
                        {
                            physBody->SetLinearVelocity(b2Vec2(0, 0));
                            teleport(contactedWarp->data.ox+16-posX_coefficient,
                                         contactedWarp->data.oy+32-height);
                        }
                        wasEntered = true;
                    }
                }
                break;
            case 0://Instant
            default:
                if(!wasTeleported && !wasEntered)
                {
                    teleport(contactedWarp->data.ox+16-posX_coefficient,
                                 contactedWarp->data.oy+32-height);
                    wasTeleported = true;
                    wasEntered = true;
                }
                break;
            }
            //qDebug()<< "Warp!!!" << contactedWarp->data.ox << contactedWarp->data.oy;
        }
        //contactedWithWarp = false;
        //contactedWarp = NULL;
    }
    else
    {
        if(wasTeleported)
        {
            if(warpsTouched==0)
                wasTeleported = false;
        }
    }

    camera->setPos( posX() - PGE_Window::Width/2 + posX_coefficient,
                    posY() - PGE_Window::Height/2 + posY_coefficient );
}

void LVL_Player::kill(deathReason reason)
{
    doKill=true;
    kill_reason=reason;
}

void LVL_Player::harm(int _damage)
{
    doHarm=true;
    doHarm_damage=_damage;
}

void LVL_Player::bump(bool _up)
{
    if(_up)
        bumpUp=true;
    else
        bumpDown=true;
    if(physBody)
        bumpVelocity = fabs(physBody->GetLinearVelocity().y)/4;
}

void LVL_Player::teleport(float x, float y)
{
    if(!LvlSceneP::s) return;

    this->setPos(x, y);

    int sID = LvlSceneP::s->findNearSection(x, y);

    if(camera->section->id != LvlSceneP::s->levelData()->sections[sID].id)
    {
        camera->changeSection(LvlSceneP::s->levelData()->sections[sID]);

        if(ConfigManager::lvl_bg_indexes.contains(camera->BackgroundID))
        {
            LvlSceneP::s->bgList()->last()->setBg(ConfigManager::lvl_bg_indexes[camera->BackgroundID]);

            if(ConfigManager::lvl_bg_indexes[camera->BackgroundID].animated)
                ConfigManager::Animator_BG[ConfigManager::lvl_bg_indexes[camera->BackgroundID].animator_ID].start();
        }
        else
            LvlSceneP::s->bgList()->last()->setNone();
    }
    else
    {
        camera->resetLimits();
    }
}

void LVL_Player::exitFromLevel(QString levelFile, int targetWarp)
{
    isLive = false;
    //physBody->SetActive(false);
    if(!levelFile.isEmpty())
    {
        LvlSceneP::s->warpToLevelFile =
                LvlSceneP::s->levelData()->path+"/"+levelFile;
        LvlSceneP::s->warpToArrayID = targetWarp;
    }
    LvlSceneP::s->setExiting(2000, LevelScene::EXIT_Warp);
}



void LVL_Player::render(float camX, float camY)
{

    if(!isLive) return;

    QRectF player = QRectF( posX()
                            -camX,

                            posY()
                            -camY,

                            width,
                            height
                         );
//        qDebug() << "PlPos" << pl.left() << pl.top() << player.right() << player.bottom();

    glDisable(GL_TEXTURE_2D);
    glColor4f( 0.f, 0.f, 1.f, 1.f);
    glBegin( GL_QUADS );
        glVertex2f( player.left(), player.top());
        glVertex2f( player.right(), player.top());
        glVertex2f( player.right(),  player.bottom());
        glVertex2f( player.left(),  player.bottom());
    glEnd();
}

