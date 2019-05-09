/*
 * Moondust, a free game engine for platform game making
 * Copyright (c) 2014-2019 Vitaly Novichkov <admin@wohlnet.ru>
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

#include "../../scene_level.h"
#include "../lvl_player.h"
#include "../lvl_section.h"

#include <audio/pge_audio.h>
#include <settings/debugger.h>
#include <common_features/logger.h>

void LVL_Player::lua_updateKey(bool &target_key, ControllableObject::KeyType ktype, bool &state)
{
    if(target_key != state)
    {
        if(state)
            lua_onKeyPressed(ktype);
        else
            lua_onKeyReleased(ktype);

        target_key = state;
    }
}

void LVL_Player::lua_processKeyEvents()
{
    try
    {
        lua_updateKey(m_keysPrev.left, KEY_LEFT, keys.left);
        lua_updateKey(m_keysPrev.right, KEY_RIGHT, keys.right);
        lua_updateKey(m_keysPrev.up, KEY_UP, keys.up);
        lua_updateKey(m_keysPrev.down, KEY_DOWN, keys.down);
        lua_updateKey(m_keysPrev.run, KEY_RUN, keys.run);
        lua_updateKey(m_keysPrev.jump, KEY_JUMP, keys.jump);
        lua_updateKey(m_keysPrev.alt_run, KEY_ALT_RUN, keys.alt_run);
        lua_updateKey(m_keysPrev.alt_jump, KEY_ALT_JUMP, keys.alt_jump);
        lua_updateKey(m_keysPrev.drop, KEY_DROP, keys.drop);
        lua_updateKey(m_keysPrev.start, KEY_DROP, keys.start);
    }
    catch(luabind::error &e)
    {
        m_scene->getLuaEngine()->postLateShutdownError(e);
    }
}

void LVL_Player::update(double tickTime)
{
    if(m_isLocked) return;

    if(!m_isInited) return;

    if(!camera) return;

    LVL_Section *section = sct();

    if(!section) return;

    m_eventQueue.processEvents(tickTime);

    if((m_isWarping) || (!isAlive))
    {
        m_animator.tickAnimation(tickTime);
        updateCamera();
        return;
    }

    if(m_invincible)
    {
        m_invincibleDelay -= tickTime;
        if(m_invincibleDelay < 0.0)
        {
            m_invincible = false;
            m_blinkScreen = false;
        }
    }

    //_onGround = !foot_contacts_map.empty();
    //on_slippery_surface = !foot_sl_contacts_map.empty();
    bool climbableUp  = !climbable_map.empty();
    bool climbableDown = climbableUp && !m_stand;
    m_climbing = (climbableUp && m_climbing && !m_stand && (m_momentum.centerY() >= (m_climbableHeight - physics_cur.velocity_climb_y_up)));

    if(m_stand)
    {
        phys_setup.decelerate_x =
            (fabs(speedX()) <= physics_cur.MaxSpeed_walk) ?
            (m_onSlippery ? physics_cur.decelerate_stop / physics_cur.slippery_c : physics_cur.decelerate_stop) :
            (m_onSlippery ? physics_cur.decelerate_run / physics_cur.slippery_c : physics_cur.decelerate_run);

        if(physics_cur.strict_max_speed_on_ground)
        {
            if((speedX() > 0) && (speedX() > phys_setup.max_vel_x))
                setSpeedX(phys_setup.max_vel_x);
            else if((speedX() < 0) && (speedX() < phys_setup.min_vel_x))
                setSpeedX(phys_setup.min_vel_x);
        }
    }
    else
        phys_setup.decelerate_x = physics_cur.decelerate_air;

    if(m_climbing)
    {
        PGE_Phys_Object *climbableItem = (climbable_map.begin())->second;
        if(climbableItem)
        {
            m_momentum.velX = climbableItem->speedX();
            m_momentum.velY = climbableItem->speedY();
        }
        if(m_gscale_Backup != 1.0)
        {
            setGravityScale(0);
            m_gscale_Backup = 1.0;
        }
    }
    else
    {
        if(m_gscale_Backup != 0.0)
        {
            setGravityScale(physics_cur.gravity_scale);
            m_gscale_Backup = 0.0;
        }
    }

    if(m_climbing)
        setSpeed(0, 0);

    if(environments_map.empty())
    {
        if(last_environment != section->getPhysicalEnvironment())
            environment = section->getPhysicalEnvironment();
    }
    else
    {
        int newEnv = section->getPhysicalEnvironment();
        for(const std::pair<const int, int> &x : environments_map)
            newEnv = x.second;

        if((newEnv != LVL_PhysEnv::Env_SameAsAround) && (last_environment != newEnv))
        {
            D_pLogDebug("Enter to environment: %d", newEnv);
            environment = newEnv;
        }
    }

    refreshEnvironmentState();

    if(m_stand)
    {
        if(!m_floatingIsWorks)
            m_floatingTimer = m_floatingMaxtime;
    }

    //Processing lua key events
    lua_processKeyEvents();

    //Running key
    if(keys.run || keys.alt_run)
    {
        if(!m_isRunning)
        {
            phys_setup.max_vel_x = physics_cur.MaxSpeed_run;
            phys_setup.min_vel_x = -physics_cur.MaxSpeed_run;
            m_isRunning = true;
        }
    }
    else
    {
        if(m_isRunning)
        {
            phys_setup.max_vel_x = physics_cur.MaxSpeed_walk;
            phys_setup.min_vel_x = -physics_cur.MaxSpeed_walk;
            m_isRunning = false;
        }
    }

    if((physics_cur.ground_c_max != 1.0))
    {
        phys_setup.max_vel_x = fabs(m_isRunning ?
                                    physics_cur.MaxSpeed_run :
                                    physics_cur.MaxSpeed_walk) * (m_stand ? physics_cur.ground_c_max : 1.0);
        phys_setup.min_vel_x = -fabs(m_isRunning ?
                                     physics_cur.MaxSpeed_run :
                                     physics_cur.MaxSpeed_walk) * (m_stand ? physics_cur.ground_c_max : 1.0);
    }

    if(keys.alt_run)
    {
        if(PGE_Debugger::cheat_chucknorris)
        {
            if(m_attackEnabled && !m_attackPressed && !m_climbing)
            {
                m_attackPressed = true;

                if(keys.up)
                    attackCN(Attack_Up);
                else if(keys.down)
                    attackCN(Attack_Down);
                else
                {
                    attackCN(Attack_Forward);
                    PGE_Audio::playSoundByRole(obj_sound_role::PlayerTail);
                    m_animator.playOnce(MatrixAnimator::RacoonTail, m_direction, 75, true, true, 1);
                }
            }
        }
    }
    else
    {
        if(m_attackPressed)
            m_attackPressed = false;
    }

    //  if(!keys.up && !keys.down && !keys.left && !keys.right)
    //  {
    //      if(wasEntered)
    //      {
    //          wasEntered = false;
    //          wasEnteredTimeout=0;
    //      }
    //  }

    //Reset state
    if(m_wasEntered)
    {
        m_wasEnteredTimeout -= tickTime;

        if(m_wasEnteredTimeout < 0)
        {
            m_wasEnteredTimeout = 0;
            m_wasEntered = false;
        }
    }

    if(keys.up)
    {
        if(climbableUp && (m_jumpTime <= 0))
        {
            setDuck(false);
            m_climbing = true;
            m_floatingIsWorks = false; //!< Reset floating on climbing start
        }

        if(m_climbing)
        {
            if(m_momentum.centerY() >= m_climbableHeight)
                setSpeedY(-physics_cur.velocity_climb_y_up);
        }
        else
        {
            if(collided_talkable_npc)
                collided_talkable_npc->talkWith();
        }
    }

    if(keys.down)
    {
        if(climbableDown && (m_jumpTime <= 0))
        {
            setDuck(false);
            m_climbing = true;
            m_floatingIsWorks = false; //!< Reset floating on climbing start
        }
        else
        {
            if((m_duckAllow & !m_ducking) && ((m_animator.curAnimation() != MatrixAnimator::RacoonTail)))
                setDuck(true);
        }

        if(m_climbing)
            setSpeedY(physics_cur.velocity_climb_y_down);
    }
    else
    {
        if(m_ducking)
            setDuck(false);
    }

    if((!keys.left) || (!keys.right))
    {
        bool turning = (((speedX() > 0) && (m_direction < 0)) || ((speedX() < 0) && (m_direction > 0)));
        double force = turning ?
                       physics_cur.decelerate_turn :
                       (fabs(speedX()) > physics_cur.MaxSpeed_walk) ? physics_cur.run_force : physics_cur.walk_force;

        if(m_onSlippery)
            force = force / physics_cur.slippery_c;
        else if((m_stand) && (physics_cur.ground_c != 1.0))
            force = force * physics_cur.ground_c;

        if(keys.left) m_direction = -1;

        if(keys.right) m_direction = 1;

        if(!m_ducking || !m_stand)
        {
            //If left key is pressed
            if(keys.right)
            {
                if(m_climbing)
                    setSpeedX(physics_cur.velocity_climb_x);
                else
                    applyAccel(force, 0.0);
            }

            //If right key is pressed
            if(keys.left)
            {
                if(m_climbing)
                    setSpeedX(-physics_cur.velocity_climb_x);
                else
                    applyAccel(-force, 0.0);
            }

            if((keys.left || keys.right) && turning && m_stand)
            {
                SpawnEffectDef effect = setup.slide_effect;
                effect.startX = posCenterX();
                effect.startY = bottom() + 2.0;
                m_scene->launchEffect(effect, true);
            }
        }
    }

    m_moveLeft =  keys.left;
    m_moveRight = keys.right;

    if(keys.alt_jump && PGE_Debugger::cheat_superman)
    {
        //Temporary it is ability to fly up!
        if(!m_bumpDown && !m_bumpUp)
            setSpeedY(-physics_cur.velocity_jump);
    }

    if(keys.jump || keys.alt_jump)
    {
        if(!m_jumpPressed)
        {
            if(environment != LVL_PhysEnv::Env_Water)
            {
                if(m_climbing || m_stand || (environment == LVL_PhysEnv::Env_Quicksand))
                    PGE_Audio::playSoundByRole(obj_sound_role::PlayerJump);
            }
            else
                PGE_Audio::playSoundByRole(obj_sound_role::PlayerWaterSwim);
        }

        if((environment == LVL_PhysEnv::Env_Water) || (environment == LVL_PhysEnv::Env_Quicksand))
        {
            if(!m_jumpPressed)
            {
                if(environment == LVL_PhysEnv::Env_Water)
                {
                    if(!m_ducking)
                        m_animator.playOnce(MatrixAnimator::SwimUp, m_direction, 75);
                }
                else if(environment == LVL_PhysEnv::Env_Quicksand)
                {
                    if(!m_ducking)
                        m_animator.playOnce(MatrixAnimator::JumpFloat, m_direction, 64);
                }

                m_jumpPressed   = true;
                m_jumpTime      = physics_cur.jump_time;
                m_jumpVelocity  = physics_cur.velocity_jump;
                m_floatingTimer = m_floatingMaxtime;
                setSpeedY(speedY() - m_jumpVelocity);
            }
        }
        else if(!m_jumpPressed)
        {
            m_jumpPressed = true;

            if(m_stand || m_climbing)
            {
                m_climbing = false;
                m_jumpTime = physics_cur.jump_time;
                m_jumpVelocity = physics_cur.velocity_jump;
                m_floatingTimer = m_floatingMaxtime;
                //LEGACY_m_velocityY_add = 0;//Remove Y speed-add when player jumping
                setSpeedY(-m_jumpVelocity - fabs(speedX() / physics_cur.velocity_jump_c));
            }
            else if((m_floatingAllow) && (m_floatingTimer > 0.0))
            {
                m_floatingIsWorks = true;
                //if true - do floating with sin, if false - do with cos.
                m_floatingStartType = (speedY() < 0.0);
                setSpeedY(0.0);
                setGravityScale(0.0);
            }
        }
        else
        {
            if(m_jumpTime > 0)
            {
                m_jumpTime -= tickTime;
                setSpeedY(-m_jumpVelocity - fabs(speedX() / physics_cur.velocity_jump_c));
            }

            if(m_floatingIsWorks)
            {
                m_floatingTimer -= tickTime;

                if(m_floatingStartType)
                    setSpeedY(state_cur.floating_amplitude * (-cos(m_floatingTimer / 80.0)));
                else
                    setSpeedY(state_cur.floating_amplitude * (cos(m_floatingTimer / 80.0)));

                if(m_floatingTimer <= 0)
                {
                    m_floatingTimer = 0;
                    m_floatingIsWorks = false;
                    setGravityScale(m_climbing ? 0.0 : physics_cur.gravity_scale);
                }
            }
        }
    }
    else
    {
        m_jumpTime = 0;

        if(m_jumpPressed)
        {
            m_jumpPressed = false;

            if(m_floatingAllow)
            {
                if(m_floatingIsWorks)
                {
                    m_floatingTimer = 0;
                    m_floatingIsWorks = false;
                    setGravityScale(m_climbing ? 0 : physics_cur.gravity_scale);
                }
            }
        }
    }

    refreshAnimation();
    m_animator.tickAnimation(tickTime);
    PGE_RectF sBox = section->sectionLimitBox();
    struct
    {
        double speedX;
        double speedY;
    }
    sAS{camera->m_autoScrool.velX, camera->m_autoScrool.velY};

    if((section->m_isAutoscroll) && (camera->m_autoScrool.enabled))
        sBox = camera->limitBox;

    //Return player to start position on fall down
    if(section->isWrapV())
    {
        if(posY() < sBox.top() - m_momentum.h - 1)
            setPosY(sBox.bottom() - 1);
        else if(posY() > sBox.bottom() + 1)
            setPosY(sBox.top() - m_momentum.h + 1);
    }
    else
    {
        if(posY() > sBox.bottom() + m_momentum.h)
            kill(DEAD_fall);
        else if((sAS.speedY > 0.0)
                && (m_momentum.bottom() < (sBox.top() - 32.0))
                && m_stand)
            kill(DEAD_fall);
        else if((m_momentum.bottom() < (sBox.top() - 64.0)) && (!m_stand))
            m_momentum.setYatBottom(sBox.top() - 64.0);
    }

    //Connection of section opposite sides
    if(m_isExiting) // Allow walk offscreen if exiting
    {
        if((posX() < sBox.left() - m_momentum.w - 1) || (posX() > sBox.right() + 1))
        {
            setGravityScale(0.0);//Prevent falling [we anyway exited from this level, isn't it?]
            setSpeedY(0.0);
        }

        if(keys.left || keys.right)
        {
            if((environment == LVL_PhysEnv::Env_Water) || (environment == LVL_PhysEnv::Env_Quicksand))
            {
                keys.run = true;

                if(m_exiting_swimTimer < 0 && !keys.jump)
                    keys.jump = true;
                else if(m_exiting_swimTimer < 0 && keys.jump)
                {
                    keys.jump = false;
                    m_exiting_swimTimer = (environment == LVL_PhysEnv::Env_Quicksand) ? 1 : 500;
                }

                m_exiting_swimTimer -= tickTime;
            }
            else keys.run = false;
        }
    }
    else if(section->isWrapH())
    {
        if(posX() < sBox.left() - m_momentum.w - 1.0)
            setPosX(sBox.right() + 1.0);
        else if(posX() > sBox.right() + 1.0)
            setPosX(sBox.left() - m_momentum.w - 1.0);
    }
    else
    {
        if(section->ExitOffscreen())
        {
            if(section->LeftOnly() ^ section->RightOnly())
            {
                if(section->RightOnly())
                {
                    if(posX() < sBox.left())
                    {
                        setPosX(sBox.left());

                        if(sAS.speedX == 0.0)
                            setSpeedX(0.0);
                        else
                        {
                            setSpeedX(sAS.speedX >= speedX() ? sAS.speedX : speedX());

                            if(m_blockedAtRight)
                                kill(DEAD_killed);
                        }
                    }
                }

                if(section->LeftOnly())
                {
                    if(posX() + m_momentum.w > sBox.right())
                    {
                        setPosX(sBox.right() - m_momentum.w);

                        if(sAS.speedX == 0.0)
                            setSpeedX(0.0);
                        else
                        {
                            setSpeedX(sAS.speedX <= speedX() ? sAS.speedX : speedX());

                            if(m_blockedAtLeft)
                                kill(DEAD_killed);
                        }
                    }
                }
            }

            if((posX() < sBox.left() - m_momentum.w - 1.0) || (posX() > sBox.right() + 1.0))
            {
                setLocked(true);
                m_noRender = true;
                m_scene->setExiting(1000, LvlExit::EXIT_OffScreen);
                return;
            }
        }
        else
        {
            //Add extra pixel limit to allow touching instant warps placed offscreen
            double leftBorder = sBox.left() - 1.0;
            double rightBorder = sBox.right() + 1.0;

            //Prevent moving of player away from screen
            if(posX() < leftBorder)
            {
                setPosX(leftBorder);

                if(sAS.speedX == 0.0)
                    setSpeedX(0.0);
                else
                {
                    setSpeedX(sAS.speedX >= speedX() ? sAS.speedX : speedX());

                    if(m_blockedAtRight)
                        kill(DEAD_killed);
                }
            }
            else if(posX() + m_momentum.w > rightBorder)
            {
                setPosX(rightBorder - m_momentum.w);

                if(sAS.speedX == 0.0)
                    setSpeedX(0.0);
                else
                {
                    setSpeedX(sAS.speedX <= speedX() ? sAS.speedX : speedX());

                    if(m_blockedAtLeft)
                        kill(DEAD_killed);
                }
            }
        }
    }

    /*
    if(m_crushed && m_crushedOld)
    {
        m_momentum.x -= _direction*4;
        applyAccel(0.0, 0.0);
    }*/
    processWarpChecking();

    if(_doSafeSwitchCharacter)
        setCharacter(characterID, stateID);

    try
    {
        lua_onLoop(tickTime);
    }
    catch(luabind::error &e)
    {
        m_scene->getLuaEngine()->postLateShutdownError(e);
    }

    m_treemap.updatePos();
}

void LVL_Player::updateCamera()
{
    camera->setCenterPos(round(posCenterX()), round(bottom()) - state_cur.height / 2);
}
