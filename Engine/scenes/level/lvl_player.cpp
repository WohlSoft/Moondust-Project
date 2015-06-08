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

#include "lvl_player.h"

#include <graphics/window.h>
#include <graphics/gl_renderer.h>
#include <data_configs/config_manager.h>
#include <audio/pge_audio.h>

#include <common_features/rectf.h>
#include <common_features/pointf.h>

#include "lvl_scene_ptr.h"

#include <QtDebug>

LVL_Player::LVL_Player()
{
    camera = NULL;
    worldPtr = NULL;
    playerID = 0;
    isLocked = true;
    isInited = false;

    direction = 1;

    type = LVLPlayer;

    frameW=100;
    frameH=100;

    int CharacterID = 1;
    stateID=1;

    setup = ConfigManager::playable_characters[CharacterID];
    physics = setup.phys_default;
    states =   ConfigManager::playable_characters[CharacterID].states;
    state_cur = states[stateID];
    long tID = ConfigManager::getLvlPlayerTexture(CharacterID, stateID);
    if( tID >= 0 )
    {
        texId = ConfigManager::level_textures[tID].texture;
        texture = ConfigManager::level_textures[tID];
        frameW = ConfigManager::level_textures[tID].w / setup.matrix_width;
        frameH = ConfigManager::level_textures[tID].h / setup.matrix_height;
    }

    animator.setSize(setup.matrix_width, setup.matrix_height);
    animator.installAnimationSet(state_cur.sprite_setup);
    animator.switchAnimation(MatrixAnimator::Idle, direction, 100);
    animator.tickAnimation(0.f);

    environment = LVL_PhysEnv::Env_Air;
    last_environment = LVL_PhysEnv::Env_Air;
    physics_cur = physics[environment];

    /********************floating************************/
    floating_allow=state_cur.allow_floating;
    floating_maxtime=state_cur.floating_max_time; //!< Max time to float
    floating_isworks=false;                         //!< Is character currently floating in air
    floating_timer=0;                            //!< Milliseconds to float
    floating_start_type=false;
    /********************floating************************/

    /********************Attack************************/
    attack_enabled=true;
    attack_pressed=false;
    /********************Attack************************/

    /********************duck**************************/
    duck_allow= state_cur.duck_allow;
    ducking=false;
    /********************duck**************************/

    JumpPressed=false;
    onGround=false;

    climbing=false;

    collide = PGE_Phys_Object::COLLISION_ANY;

    bumpDown=false;
    bumpUp=false;
    bumpVelocity=0.0f;

    health=3;
    doHarm=false;
    doHarm_damage=0;

    foot_contacts=0;
    jumpForce=0;

    jumpForce_default=260;

    isLive = true;
    doKill = false;
    kill_reason=DEAD_fall;

    curHMaxSpeed = physics_cur.MaxSpeed_walk;
    isRunning = false;

    contactedWithWarp = false;
    contactedWarp = NULL;
    wasTeleported = false;
    wasEntered = false;
    warpsTouched = 0;
    warpDirectO = 0;
    isWarping=false;
    warpPipeOffset=0;

    warpDo=false;
    warpDirect=0;
    warpWaitTicks=0;

    gscale_Backup=0.f;//!< BackUP of last gravity scale
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
    playerID = data.id;

    b2BodyDef bodyDef;
    bodyDef.type = b2_dynamicBody;
    bodyDef.position.Set
            (
                PhysUtil::pix2met((float)data.x + posX_coefficient),
                PhysUtil::pix2met((float)data.y + posY_coefficient)
            );
    //PhysUtil::pix2met((float)(data.x-((data.w-posX_coefficient)/2)-1) + posX_coefficient),
    //PhysUtil::pix2met((float)(data.y-(data.h-(height+2))-1) + posY_coefficient-0.1)

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

    f_player = physBody->CreateFixture(&fixtureDef);

    animator.tickAnimation(0.f);
    //qDebug() <<"Start position is " << posX() << posY();
    isLocked=false;
    isInited=true;
}

void LVL_Player::initSize()
{
    obj_player_state state = states[stateID];
    setSize(state.width-state.width%2, state.height-state.height%2);
    data.x = data.x+(data.w/2)-(state.width/2);
    data.y = data.y+data.h-state.height;
    direction = data.direction;
}

void LVL_Player::update(float ticks)
{
    if(isLocked) return;
    if(!isInited) return;
    if(!physBody) return;
    if(!camera) return;
    LVL_Section* section = sct();
    if(!section) return;
    PGE_Phys_Object::update(ticks);

//    while(!npc_queue.isEmpty())
//        npc_queue.dequeue()->kill();

    event_queue.processEvents(ticks);
    if(isWarping)
    {
        physBody->SetLinearVelocity(b2Vec2(0, 0));
        animator.tickAnimation(ticks);
        updateCamera();
        return;
    }

    if(_player_moveup)
    {
        physBody->SetTransform(b2Vec2(
             physBody->GetPosition().x,
             physBody->GetPosition().y-0.2), 0.0f);
        _player_moveup = false;
    }

    onGround = !foot_contacts_map.isEmpty();

    if(doKill)
    {
        doKill=false;
        isLive = false;
        if(physBody)
        {
            physBody->SetGravityScale(0);
            physBody->SetAwake(false);
            physBody->SetActive(false);
        }
        LvlSceneP::s->checkPlayers();
        return;
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
            physBody->SetGravityScale(physics_cur.gravity_scale);
            gscale_Backup = 0.f;
        }
    }

    if(environment==LVL_PhysEnv::Env_Quicksand)
    {
        physBody->SetLinearVelocity(b2Vec2(0, 0));
    }

    if(climbing)
    {
        physBody->SetLinearVelocity(b2Vec2(0, 0));
    }
    else
    {
        if(physBody->GetLinearVelocity().y > physics_cur.MaxSpeed_down)
            physBody->SetLinearVelocity(b2Vec2(physBody->GetLinearVelocity().x, physics_cur.MaxSpeed_down));
        else
        if(physBody->GetLinearVelocity().y < -physics_cur.MaxSpeed_up)
            physBody->SetLinearVelocity(b2Vec2(physBody->GetLinearVelocity().x, -physics_cur.MaxSpeed_up));
    }


    if(environments_map.isEmpty())
    {
        if(last_environment!=section->getPhysicalEnvironment() )
        {
            environment = section->getPhysicalEnvironment();
        }
    }
    else
    {
        int newEnv = section->getPhysicalEnvironment();

        foreach(int x, environments_map)
        {
            newEnv = x;
        }

        if(last_environment != newEnv)
        {
            qDebug()<<"Enter to environment" << newEnv;
            environment = newEnv;
        }
    }


    if((last_environment!=environment)||(last_environment==-1))
    {
        physics_cur = physics[environment];

        last_environment=environment;

        if(physics_cur.zero_speed_y_on_enter)
            physBody->SetLinearVelocity(b2Vec2(physBody->GetLinearVelocity().x, 0));

        if(physics_cur.slow_speed_x_on_enter)
            physBody->SetLinearVelocity(b2Vec2(physBody->GetLinearVelocity().x/2, physBody->GetLinearVelocity().y));

        physBody->SetLinearDamping(physics_cur.damping);
        physBody->SetGravityScale(physics_cur.gravity_scale);
        curHMaxSpeed = isRunning ?
                    physics_cur.MaxSpeed_run :
                    physics_cur.MaxSpeed_walk;

        floating_isworks=false;//< Reset floating on re-entering into another physical envirinment
    }

    if(onGround)
    {
        if(!floating_isworks)
        {
            floating_timer=floating_maxtime;
        }
    }

    //Running key
    if(keys.run)
    {
        if(!isRunning)
        {
            curHMaxSpeed = physics_cur.MaxSpeed_run;
            isRunning=true;
        }
    }
    else
    {
        if(isRunning)
        {
            curHMaxSpeed = physics_cur.MaxSpeed_walk;
            isRunning=false;
        }
    }

    if(keys.alt_run)
    {
        if(attack_enabled && !attack_pressed && !climbing)
        {
            attack_pressed=true;

            if(keys.up)
                attack(Attack_Up);
            else
            if(keys.down)
                attack(Attack_Down);
            else
            {
                attack(Attack_Forward);
                PGE_Audio::playSoundByRole(obj_sound_role::PlayerTail);
                animator.playOnce(MatrixAnimator::RacoonTail, direction, 75);
            }
        }
    }
    else
    {
        if(attack_pressed) attack_pressed=false;
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
            physBody->SetLinearVelocity(b2Vec2(physBody->GetLinearVelocity().x,
                                               -physics_cur.velocity_climb));
        }
        else
        if(climbable_map.size()>0)
        {
            climbing=true;
            floating_isworks=false;//!< Reset floating on climbing start
        }
    }

    if(keys.down)
    {
        if(climbing)
        {
            physBody->SetLinearVelocity(b2Vec2(physBody->GetLinearVelocity().x,
                                               physics_cur.velocity_climb));
        }
        else
        if(climbable_map.size()>0)
        {
            climbing=true;
            floating_isworks=false;//!< Reset floating on climbing start
        }
        else
        {
            if(duck_allow & !ducking)
            {
                setDuck(true);
            }
        }
    }
    else
    {
        if(ducking)
            setDuck(false);
    }

    slippery_surface = !foot_sl_contacts_map.isEmpty();

    if( (!keys.left) || (!keys.right) )
    {
        float32 force = slippery_surface ?
                               (physics_cur.walk_force/
                                physics_cur.slippery_c) :
                                physics_cur.walk_force;

        if(keys.left) direction=-1;
        if(keys.right) direction=1;

        if(!ducking || !onGround)
        {
            //If left key is pressed
            if(keys.right)
            {
                if(climbing)
                {
                    physBody->SetLinearVelocity(b2Vec2(physics_cur.velocity_climb,
                                                       physBody->GetLinearVelocity().y));
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
                    physBody->SetLinearVelocity(b2Vec2(-physics_cur.velocity_climb,
                                                       physBody->GetLinearVelocity().y));
                }
                else
                {
                    if(physBody->GetLinearVelocity().x >= -curHMaxSpeed)
                        physBody->ApplyForceToCenter(b2Vec2(-force, 0.0f), true);
                }
            }
        }
    }

    if( keys.alt_jump )
    {
        //Temporary it is ability to fly up!
        physBody->SetLinearVelocity(b2Vec2(physBody->GetLinearVelocity().x,
                                           -physics_cur.velocity_jump));
    }

    if( keys.jump )
    {
        if(!JumpPressed)
        {
            if(environment!=LVL_PhysEnv::Env_Water)
                { if(climbing || onGround || (environment==LVL_PhysEnv::Env_Quicksand))
                    PGE_Audio::playSoundByRole(obj_sound_role::PlayerJump); }
            else
                PGE_Audio::playSound(72);//temporary!
        }

        if((environment==LVL_PhysEnv::Env_Water)||(environment==LVL_PhysEnv::Env_Quicksand))
        {
            if(!JumpPressed)
            {
                if(environment==LVL_PhysEnv::Env_Water)
                {
                    if(!ducking)
                        animator.playOnce(MatrixAnimator::SwimUp, direction, 75);
                }
                else
                if(environment==LVL_PhysEnv::Env_Quicksand)
                {
                    if(!ducking) animator.playOnce(MatrixAnimator::JumpFloat, direction, 64);
                }

                JumpPressed=true;
                jumpForce=jumpForce_default;
                floating_timer = floating_maxtime;
                physBody->SetLinearVelocity(b2Vec2(physBody->GetLinearVelocity().x,
                                                   physBody->GetLinearVelocity().y
                                                  -physics_cur.velocity_jump));
            }
        }
        else
        if(!JumpPressed)
        {
            JumpPressed=true;
            if(onGround || climbing)
            {
                climbing=false;
                jumpForce=jumpForce_default;
                floating_timer = floating_maxtime;
                physBody->SetLinearVelocity(b2Vec2(physBody->GetLinearVelocity().x,
                                                   -physics_cur.velocity_jump
                                                   -fabs(physBody->GetLinearVelocity().x/6)));
            }
            else
            if((floating_allow)&&(floating_timer>0))
            {
                floating_isworks=true;

                //if true - do floating with sin, if false - do with cos.
                floating_start_type=(physBody->GetLinearVelocity().y<0);

                physBody->SetLinearVelocity(b2Vec2(physBody->GetLinearVelocity().x, 0));
                physBody->SetGravityScale(0);
            }
        }
        else
        {
            if(jumpForce>0)
            {
                jumpForce -= ticks;
                physBody->SetLinearVelocity(b2Vec2(physBody->GetLinearVelocity().x,
                                                   -physics_cur.velocity_jump
                                                   -fabs(physBody->GetLinearVelocity().x/6)));
            }

            if(floating_isworks)
            {
                floating_timer -= ticks;
                if(floating_start_type)
                    physBody->SetLinearVelocity(b2Vec2(physBody->GetLinearVelocity().x,
                                                   3.5*(-cos(floating_timer/80.0)) ) );
                else
                    physBody->SetLinearVelocity(b2Vec2(physBody->GetLinearVelocity().x,
                                                   3.5*cos(floating_timer/80.0)) );
                if(floating_timer<=0)
                {
                    floating_timer=0;
                    floating_isworks=false;
                    physBody->SetGravityScale(climbing?0:physics_cur.gravity_scale);
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
            if(floating_allow)
            {
                if(floating_isworks)
                {
                    floating_timer=0;
                    floating_isworks=false;
                    physBody->SetGravityScale(climbing?0:physics_cur.gravity_scale);
                }
            }
        }
    }

    refreshAnimation();
    animator.tickAnimation(ticks);

    PGE_RectF sBox = section->sectionLimitBox();

    //Return player to start position on fall down
    if( posY() > sBox.bottom()+height )
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
    if(section->isWarp())
    {
        if(posX() < sBox.left()-width-1 )
            physBody->SetTransform(b2Vec2(
                 PhysUtil::pix2met(sBox.right()+posX_coefficient-1),
                 physBody->GetPosition().y), 0.0f);
        else
        if(posX() > sBox.right() + 1 )
            physBody->SetTransform(b2Vec2(
                 PhysUtil::pix2met(sBox.left()-posX_coefficient+1 ),
                 physBody->GetPosition().y), 0.0f
                                   );
    }
    else
    {

        if(section->ExitOffscreen())
        {
            if(section->RightOnly())
            {
                if( posX() < sBox.left())
                {
                    physBody->SetTransform(b2Vec2(
                         PhysUtil::pix2met(sBox.left() + posX_coefficient),
                            physBody->GetPosition().y), 0.0f);

                    physBody->SetLinearVelocity(b2Vec2(0, physBody->GetLinearVelocity().y));
                }
            }

            if((posX() < sBox.left()-width-1 ) || (posX() > sBox.right() + 1 ))
            {
                isInited=false;
                physBody->SetAwake(false);
                physBody->SetGravityScale(0);
                LvlSceneP::s->setExiting(1000, LvlExit::EXIT_OffScreen);
                return;
            }
        }
        else
        {
            //Prevent moving of player away from screen
            if( posX() < sBox.left())
            {
                physBody->SetTransform(b2Vec2(
                     PhysUtil::pix2met(sBox.left() + posX_coefficient),
                        physBody->GetPosition().y), 0.0f);

                physBody->SetLinearVelocity(b2Vec2(0, physBody->GetLinearVelocity().y));
            }
            else
            if( posX()+width > sBox.right())
            {
                physBody->SetTransform(b2Vec2(
                     PhysUtil::pix2met(sBox.right()-posX_coefficient ),
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
                            WarpTo(contactedWarp->data);
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
                        WarpTo(contactedWarp->data);
                        wasEntered = true;
                    }
                }
                break;
            case 0://Instant
            default:
                if(!wasTeleported && !wasEntered)
                {
                    WarpTo(contactedWarp->data.ox, contactedWarp->data.oy, contactedWarp->data.type);
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


    updateCamera();
}

void LVL_Player::update()
{
    update(1.0);
}

void LVL_Player::updateCamera()
{
    camera->setPos( round(posX()) - camera->w()/2 + posX_coefficient,
                    round(bottom()) - camera->h()/2-state_cur.height/2 );
}

Uint32 slideTicks=0;
void LVL_Player::refreshAnimation()
{
    /**********************************Animation switcher**********************************/
        if(climbing)
        {
            if((physBody->GetLinearVelocity().y<0.0)||(physBody->GetLinearVelocity().x!=0.0))
                animator.switchAnimation(MatrixAnimator::Climbing, direction, 128);
            else
                animator.switchAnimation(MatrixAnimator::Climbing, direction, -1);
        }
        else
        if(ducking)
        {
            animator.switchAnimation(MatrixAnimator::SitDown, direction, 128);
        }
        else
        if(!onGround)
        {
            if(animator.curAnimation()==MatrixAnimator::RacoonTail) return;

            if(environment==LVL_PhysEnv::Env_Water)
            {
                animator.switchAnimation(MatrixAnimator::Swim, direction, 128);
            }
            else if(environment==LVL_PhysEnv::Env_Quicksand)
            {
                animator.switchAnimation(MatrixAnimator::Idle, direction, 64);
            }
            else
            {
                if(physBody->GetLinearVelocity().y<0)
                    animator.switchAnimation(MatrixAnimator::JumpFloat, direction, 64);
                else if(physBody->GetLinearVelocity().y>0)
                    animator.switchAnimation(MatrixAnimator::JumpFall, direction, 64);
            }
        }
        else
        {
            bool busy=false;

            if((physBody->GetLinearVelocity().x<-1)&&(direction>0))
                if(keys.right)
                {
                    if(SDL_GetTicks()-slideTicks>100)
                    {
                        PGE_Audio::playSoundByRole(obj_sound_role::PlayerSlide);
                        slideTicks=SDL_GetTicks();
                    }
                    animator.switchAnimation(MatrixAnimator::Sliding, direction, 64);
                    busy=true;
                }

            if(!busy)
            {
                if((physBody->GetLinearVelocity().x>1)&&(direction<0))
                    if(keys.left)
                    {
                        if(SDL_GetTicks()-slideTicks>100)
                        {
                            PGE_Audio::playSoundByRole(obj_sound_role::PlayerSlide);
                            slideTicks=SDL_GetTicks();
                        }
                        animator.switchAnimation(MatrixAnimator::Sliding, direction, 64);
                        busy=true;
                    }
            }

            if(!busy)
            {
                float velX = physBody->GetLinearVelocity().x;
                if( ((!slippery_surface)&&(velX>0.0))||((slippery_surface)&&(keys.right)&&(velX>0.0)) )
                    animator.switchAnimation(MatrixAnimator::Run, direction, (128-((velX*4)<100?velX*4:100)));
                else if( ((!slippery_surface)&& (velX<0.0))||((slippery_surface)&&(keys.left)&&(velX<0.0)) )
                    animator.switchAnimation(MatrixAnimator::Run, direction, (128-((-velX*4)<100?-velX*4:100)));
                else
                    animator.switchAnimation(MatrixAnimator::Idle, direction, 64);
            }
        }
    /**********************************Animation switcher**********************************/
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

void LVL_Player::attack(LVL_Player::AttackDirection _dir)
{
    PGE_RectF attackZone;

    switch(_dir)
    {
        case Attack_Up:
            attackZone.setRect(left()+posX_coefficient-5, top()-17, 10, 5);
        break;
        case Attack_Down:
            attackZone.setRect(left()+posX_coefficient-5, bottom(), 10, 5);
        break;
        case Attack_Forward:
            if(direction>=0)
                attackZone.setRect(right(), bottom()-32, 10, 10);
            else
                attackZone.setRect(left()-10, bottom()-32, 10, 10);
        break;
    }

    CollidablesInRegionQueryCallback cb = CollidablesInRegionQueryCallback();
    b2AABB aabb;
    aabb.lowerBound.Set(PhysUtil::pix2met(attackZone.x()), PhysUtil::pix2met(attackZone.y()));
    aabb.upperBound.Set(PhysUtil::pix2met(attackZone.right()), PhysUtil::pix2met(attackZone.bottom()));
    worldPtr->QueryAABB(&cb, aabb);
    int contacts = 0;

    QList<LVL_Block *> target_blocks;
    QList<LVL_Npc*> target_npcs;
    for(int i=0; i<cb.foundBodies.size();i++)
    {
        contacts++;
        PGE_Phys_Object * visibleBody;
        if(cb.foundBodies[i]==physBody) continue;

        visibleBody = static_cast<PGE_Phys_Object *>(cb.foundBodies[i]->GetUserData());

        if(visibleBody==NULL)
            continue;

        switch(visibleBody->type)
        {
            case PGE_Phys_Object::LVLBlock:
                target_blocks.push_back(static_cast<LVL_Block*>(visibleBody));
                break;
            case PGE_Phys_Object::LVLNPC:
                target_npcs.push_back(static_cast<LVL_Npc*>(visibleBody));
                break;
            case PGE_Phys_Object::LVLPlayer:
                default:break;
        }
    }

    foreach(LVL_Block *x, target_blocks)
    {
        if(!x) continue;
        if(x->destroyed) continue;
        if(x->sizable && _dir==Attack_Forward)
            continue;
        x->hit();
        if(!x->destroyed)
            PGE_Audio::playSoundByRole(obj_sound_role::WeaponExplosion);
        x->destroyed=true;
    }
    foreach(LVL_Npc *x, target_npcs)
    {
        if(!x) continue;
        if(x->killed) continue;
        x->kill();
            PGE_Audio::playSoundByRole(obj_sound_role::PlayerStomp);
    }
}

void LVL_Player::setDuck(bool duck)
{
    if(!duck_allow) return;
    if(duck==ducking) return;

    if(duck)
    {
        physBody->DestroyFixture(f_player);
        b2PolygonShape shape;
        setSize(state_cur.width-state_cur.width%2, state_cur.duck_height-state_cur.duck_height%2);
        shape.SetAsBox(PhysUtil::pix2met(posX_coefficient),
                       PhysUtil::pix2met(posY_coefficient));
        b2FixtureDef fixtureDef;
        fixtureDef.shape = &shape;
        fixtureDef.density = 1.0f; fixtureDef.friction = 0.3f;
        f_player = physBody->CreateFixture(&fixtureDef);
        this->setPos(posX(), posY()+(state_cur.height/2-state_cur.duck_height/2)-0.01);
    } else {
        physBody->DestroyFixture(f_player);
        b2PolygonShape shape;
        setSize(state_cur.width-state_cur.width%2, state_cur.height-state_cur.height%2);
        shape.SetAsBox(PhysUtil::pix2met(posX_coefficient),
                       PhysUtil::pix2met(posY_coefficient));
        b2FixtureDef fixtureDef;
        fixtureDef.shape = &shape;
        fixtureDef.density = 1.0f; fixtureDef.friction = 0.3f;
        f_player = physBody->CreateFixture(&fixtureDef);
        this->setPos(posX(), posY()+(state_cur.duck_height/2-state_cur.height/2)-0.01);
    }

    ducking=duck;
}




//Enter player to level
void LVL_Player::WarpTo(float x, float y, int warpType, int warpDirection)
{
    warpFrameW = texture.w;
    warpFrameH = texture.h;

    isInited=true;
    switch(warpType)
    {
        case 2://door
        {
                physBody->SetGravityScale(0);
                physBody->SetLinearVelocity(b2Vec2(0, 0));
                EventQueueEntry<LVL_Player >event2;
                event2.makeCaller([this,x,y]()->void{
                                      isWarping=true;
                                      warpPipeOffset=0.0;
                                      warpDirectO=0;
                                      teleport(x+16-posX_coefficient,
                                                     y+32-height);
                                      animator.switchAnimation(MatrixAnimator::PipeUpDown, direction, 115);
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
                                      isWarping=false;
                                      physBody->SetLinearVelocity(b2Vec2(0, -0.00001));
                                      last_environment=-1;//!< Forcing to refresh physical environment
                                  }, 200);
                event_queue.events.push_back(event3);
        }
        break;
    case 1://Pipe
        {
            // Exit direction: [1] down [3] up [4] left [2] right
            physBody->SetGravityScale(0);
            physBody->SetLinearVelocity(b2Vec2(0, 0));

            EventQueueEntry<LVL_Player >eventPipeEnter;
            eventPipeEnter.makeCaller([this,warpDirection]()->void{
                                    isWarping=true;
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
                                direction=1;
                                animator.switchAnimation(MatrixAnimator::Run, direction, 115);
                                teleport(x, y+32-height);
                                          }, 0);
                        event_queue.events.push_back(eventX);
                    }
                    break;
                case 1://down
                    {
                        EventQueueEntry<LVL_Player >eventX;
                        eventX.makeCaller([this,x,y]()->void{
                                animator.switchAnimation(MatrixAnimator::PipeUpDown, direction, 115);
                                teleport(x+16-posX_coefficient, y);
                                          }, 0);
                        event_queue.events.push_back(eventX);
                    }
                    break;
                case 4://left
                    {
                        EventQueueEntry<LVL_Player >eventX;
                        eventX.makeCaller([this,x, y]()->void{
                                direction=-1;
                                animator.switchAnimation(MatrixAnimator::Run, direction, 115);
                                teleport(x+32-width, y+32-height);
                                          }, 0);
                        event_queue.events.push_back(eventX);
                    }
                    break;
                case 3://up
                    {
                        EventQueueEntry<LVL_Player >eventX;
                        eventX.makeCaller([this,x,y]()->void{
                                animator.switchAnimation(MatrixAnimator::PipeUpDown, direction, 115);
                                teleport(x+16-posX_coefficient,
                                         y+32-height);
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

            float pStep = 1.5f/((float)PGE_Window::PhysStep);

            EventQueueEntry<LVL_Player >warpOut;
            warpOut.makeWaiterCond([this, pStep]()->bool{
                                      warpPipeOffset -= pStep;
                                      return warpPipeOffset<=0.0f;
                                  }, false, 0);
            event_queue.events.push_back(warpOut);

            EventQueueEntry<LVL_Player >endWarping;
            endWarping.makeCaller([this]()->void{
                                  isWarping=false;
                                  physBody->SetLinearVelocity(b2Vec2(0, -0.00001));
                                  last_environment=-1;//!< Forcing to refresh physical environment
                              }, 0);
            event_queue.events.push_back(endWarping);
        }
        break;
    case 0://Instant
        teleport(x+16-posX_coefficient,
                     y+32-height);
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
            physBody->SetLinearVelocity(b2Vec2(0, 0));

            //Create events
            EventQueueEntry<LVL_Player >event1;
            event1.makeCaller([this]()->void{
                                physBody->SetLinearVelocity(b2Vec2(0, 0));
                                isWarping=true;
                                warpPipeOffset=0.0f;
                                setDuck(false);
                                physBody->SetGravityScale(0);
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
                                  direction=1;
                                  animator.switchAnimation(MatrixAnimator::Run, direction, 115);
                                  setPos(warp.ix+32-width, posY());
                                          }, 0);
                        event_queue.events.push_back(eventX);
                    }
                break;
                case 3:
                    {
                        EventQueueEntry<LVL_Player >eventX;
                        eventX.makeCaller([this,warp]()->void{
                                  warpDirectO=3;
                                  animator.switchAnimation(MatrixAnimator::PipeUpDown, direction, 115);
                                  setPos(posX(), warp.iy+32-height);
                                          }, 0);
                        event_queue.events.push_back(eventX);
                    }
                break;
                case 2://Left
                    {
                        EventQueueEntry<LVL_Player >eventX;
                        eventX.makeCaller([this,warp]()->void{
                                  warpDirectO=2;
                                  direction=-1;
                                  animator.switchAnimation(MatrixAnimator::Run, direction, 115);
                                  setPos(warp.ix, posY());
                                          }, 0);
                        event_queue.events.push_back(eventX);
                    }
                break;
                case 1:
                    {
                        EventQueueEntry<LVL_Player >eventX;
                        eventX.makeCaller([this,warp]()->void{
                                  warpDirectO=1;
                                  animator.switchAnimation(MatrixAnimator::PipeUpDown, direction, 115);
                                  setPos(posX(), warp.iy);
                                          }, 0);
                        event_queue.events.push_back(eventX);
                    }
                break;
            }

            float pStep = 1.5f/((float)PGE_Window::PhysStep);
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
                int sID = LvlSceneP::s->findNearSection(warp.ox, warp.oy);
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
            //Create events
            EventQueueEntry<LVL_Player >event1;
            event1.makeCaller([this]()->void{
                                physBody->SetLinearVelocity(b2Vec2(0, 0));
                                isWarping=true;
                                warpPipeOffset=0.0;
                                warpDirectO=0;
                                setDuck(false);
                                physBody->SetGravityScale(0);
                                animator.switchAnimation(MatrixAnimator::PipeUpDownRear, direction, 115);
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
                int sID = LvlSceneP::s->findNearSection(warp.ox, warp.oy);
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
}


void LVL_Player::teleport(float x, float y)
{
    if(!LvlSceneP::s) return;

    this->setPos(x, y);

    int sID = LvlSceneP::s->findNearSection(x, y);
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
    isLive = false;
    //physBody->SetActive(false);
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



void LVL_Player::kill_npc(LVL_Npc *target, LVL_Player::kill_npc_reasons reason)
{
    if(!target) return;
    //npc_queue.enqueue(target);
    switch(reason)
    {
        case NPC_Unknown:
            break;
        case NPC_Stomped:
            PGE_Audio::playSoundByRole(obj_sound_role::PlayerStomp); break;
        case NPC_Kicked:
            break;
        case NPC_Taked_Coin:
            PGE_Audio::playSoundByRole(obj_sound_role::BonusCoin); break;
        case NPC_Taked_Powerup:
            break;
    }
}



void LVL_Player::render(double camX, double camY)
{
    if(!isLive) return;
    if(!isInited) return;

    PGE_RectF tPos = animator.curFrame();
    PGE_PointF Ofs = animator.curOffset();

    PGE_RectF player;
    player.setRect(round(posX()-camX)-Ofs.x(),
                   round(posY()-Ofs.y())-camY,
                            frameW,
                            frameH
                         );

    if(isWarping)
    {
        if(warpPipeOffset>=1.0)
            return;
        //     Exit direction: [1] down  [3] up  [4] left  [2] right
        // Entrance direction: [3] down, [1] up, [2] left, [4] right
        switch(warpDirectO)
        {
            case 2://Left entrance, right Exit
                {
                    float wOfs = Ofs.x()/warpFrameW;//Relative X offset
                    float wOfsF = width/warpFrameW; //Relative hitbox width
                    tPos.setLeft(tPos.left()+wOfs+(warpPipeOffset*wOfsF));
                    player.setLeft( player.left()+Ofs.x() );
                    player.setRight( player.right()-(warpPipeOffset*width) );
                }
                break;
            case 1://Up entrance, down exit
                {
                    float hOfs = Ofs.y()/warpFrameH;//Relative Y offset
                    float hOfsF = height/warpFrameH; //Relative hitbox Height
                    tPos.setTop(tPos.top()+hOfs+(warpPipeOffset*hOfsF));
                    player.setTop( player.top()+Ofs.y() );
                    player.setBottom( player.bottom()-(warpPipeOffset*height) );
                }
                break;
            case 4://right emtramce. left exit
                {
                    float wOfs =  Ofs.x()/warpFrameW;               //Relative X offset
                    float fWw =   animator.sizeOfFrame().w();   //Relative width of frame
                    float wOfHB = width/warpFrameW;                 //Relative width of hitbox
                    float wWAbs = warpFrameW*fWw;                   //Absolute width of frame
                    tPos.setRight(tPos.right()-(fWw-wOfHB-wOfs)-(warpPipeOffset*wOfHB));
                    player.setLeft( player.left()+(warpPipeOffset*width) );
                    player.setRight( player.right()-(wWAbs-Ofs.x()-width) );
                }
                break;
            case 3://down entrance, up exit
                {
                    float hOfs =  Ofs.y()/warpFrameH;               //Relative Y offset
                    float fHh =   animator.sizeOfFrame().h();  //Relative height of frame
                    float hOfHB = height/warpFrameH;                //Relative height of hitbox
                    float hHAbs = warpFrameH*fHh;                   //Absolute height of frame
                    tPos.setBottom(tPos.bottom()-(fHh-hOfHB-hOfs)-(warpPipeOffset*hOfHB));
                    player.setTop( player.top()+(warpPipeOffset*height) );
                    player.setBottom( player.bottom()-(hHAbs-Ofs.y()-height) );
                }
                break;
            default:
                break;
        }
    }


    GlRenderer::renderTexture(&texture,
                              player.x(),
                              player.y(),
                              player.width(),
                              player.height(),
                              tPos.top(),
                              tPos.bottom(),
                              tPos.left(),
                              tPos.right());
}

bool LVL_Player::locked()
{
    return isLocked;
}

void LVL_Player::setLocked(bool lock)
{
    isLocked=lock;
    if(physBody)
    {
        physBody->SetAwake(!lock);
    }
}

