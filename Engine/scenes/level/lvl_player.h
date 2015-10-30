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

#ifndef LVL_PLAYER_H
#define LVL_PLAYER_H

#include "lvl_camera.h"
#include "lvl_warp.h"
#include "lvl_npc.h"

#include "lvl_base_object.h"
#include <controls/controllable_object.h>
#include <data_configs/obj_player.h>
#include <common_features/matrix_animator.h>
#include <PGE_File_Formats/file_formats.h>
#include <common_features/event_queue.h>

#include <QHash>
#include <QVector>

class LVL_Section;
class LVL_Block;

class LVL_Player :
        public PGE_Phys_Object,
        public ControllableObject
{
    public:
        LVL_Player();
        virtual ~LVL_Player();
        void setCharacter(int CharacterID, int _stateID);
        void setCharacterSafe(int CharacterID, int _stateID);
    private:
        bool _doSafeSwitchCharacter;
    public:
        void setPlayerPointInfo(PlayerPoint pt);
        void init();
        void update(float ticks);
        void updateCamera();

        void refreshEnvironmentState();

        void updateCollisions();
        void solveCollision(PGE_Phys_Object *collided);
        bool forceCollideCenter;//!< collide with invizible blocks at center
        float _heightDelta; //Delta of changing height. Need to protect going through block on character switching

        /*****************NPC's and blocks******************/
        typedef QHash<int, PGE_Phys_Object*> PlayerColliders;
        QHash<int, PGE_Phys_Object*> collided_top;
        QHash<int, PGE_Phys_Object*> collided_left;
        QHash<int, PGE_Phys_Object*> collided_right;
        QHash<int, PGE_Phys_Object*> collided_bottom;
        QHash<int, PGE_Phys_Object*> collided_center;
        LVL_Npc *  collided_talkable_npc;
        bool _stucked;
        /***************************************************/

        int playerID;
        obj_player setup;
        PlayerPoint data;
        PGE_LevelCamera * camera;//!< Connected camera

        void teleport(float x, float y);
        void exitFromLevel(QString levelFile, int targetWarp, long wX=-1, long wY=-1);

        QHash<int, obj_player_state > states;
        int     characterID;
        int     stateID;
        obj_player_state state_cur;


        enum kill_npc_reasons
        {
            NPC_Unknown=-1,
            NPC_Stomped=0,
            NPC_Kicked,
            NPC_Taked_Coin,
            NPC_Taked_Powerup
        };

        void kill_npc(LVL_Npc*target, kill_npc_reasons reason);
        //QQueue<LVL_Npc*> npc_queue;

        /*******************Environmept*********************/
        QHash<int, obj_player_physics > physics;
        QHash<int, int > environments_map;

        obj_player_physics physics_cur;
        int     environment;
        int     last_environment;
        /*******************Environmept*********************/

        /*******************Motion*************************/
        bool isRunning();
        bool    _isRunning;

        int  direction();
        int     _direction;
        /*******************Motion*************************/

        /*******************Life and Death*****************/
        bool isAlive;
        enum deathReason
        {
            DEAD_fall=0,
            DEAD_burn,
            DEAD_killed
        };
        void    kill(deathReason reason=DEAD_killed);
        bool    doKill;
        deathReason kill_reason;

        int     health;
        bool    invincible;
        float   invincible_delay;
        bool    blink_screen;
        bool    blink_screen_state;
        bool    doHarm;
        int     doHarm_damage;
        void    harm(int _damage=1);
        void    setInvincible(bool state, float delay, bool enableScreenBlink=false);
        /*******************Life and Death*****************/

        float   gscale_Backup; //!< BackUP of last gravity scale

        /********************Jumps***************************/
        bool    JumpPressed;
        bool    onGround();
        bool    _onGround;
        bool    on_slippery_surface;
        QHash<intptr_t, PGE_Phys_Object* > foot_contacts_map;   //!< staying on ground surfaces
        QHash<intptr_t, PGE_Phys_Object* > foot_sl_contacts_map;//!< Slipery surfaces
        float   jumpTime;
        float   jumpVelocity;
        /********************Jumps***************************/

        /********************Bump***************************/
        bool    bumpDown;
        bool    bumpUp;
        float   bumpVelocity;//!< down velocity
        float   bumpJumpVelocity;//! Up jump velocity
        int     bumpJumpTime;    //!< Up jump time
        void    bump(bool _up=false, double bounceSpeed=0.0, int timeToJump=0);
        inline void bumpf(bool _up, float bounceSpeed=0.0f, int timeToJump=0)
        {
            bump(_up, (double)bounceSpeed, timeToJump);
        }
        /********************Bump***************************/

        /********************Climbing***************************/
        QHash<intptr_t, PGE_Phys_Object* > climbable_map;
        bool        climbing;
        double      climbableHeight;
        /********************Climbing***************************/

        /*******************Warps*********************/
        bool    contactedWithWarp;
        LVL_Warp * contactedWarp;
        bool    wasEntered;
        int     wasEnteredTimeout;
        bool    isWarping;

        int     warpDirectO;
        float   warpPipeOffset;
        float   warpFrameW;
        float   warpFrameH;

        EventQueue<LVL_Player > event_queue;
        void    processWarpChecking();
        void    WarpTo(float x, float y, int warpType, int warpDirection=1);
        void    WarpTo(LevelDoor warp);
        /*******************Warps*********************/\

        /******************floating*******************/
        bool    floating_allow;
        bool    floating_isworks;
        float   floating_timer;
        float   floating_maxtime;
        bool    floating_start_type;//!< true= sin(time), false= cos(time)
        /******************floating*******************/

        /******************Attack*******************/
        /*This feature will provide teporary ability to break any nearest blocks*/
        enum AttackDirection
        {
            Attack_Forward=0,
            Attack_Up,
            Attack_Down
        };
        bool    attack_enabled;
        bool    attack_pressed;
        void    attack(AttackDirection _dir);
        /******************Attack*******************/

        /************************************************
                                    __
                                  /` ,\__
                                 |    ).-'
                                / .--'
                               / /
                 ,      _.==''`  \
               .'(  _.='         |
              {   ``  _.='       |
               {    \`     ;    /
                `.   `'=..'  .='
                  `=._    .='
               duck '-`\\`__
                        `-._{
        ************************************************/
        bool duck_allow;
        bool ducking;
        void setDuck(bool duck);
private:
        void _collideUnduck();//!< Re-checks collisions after player disabled duck.
                                                    //!< Pre-velocity needs if you call "setCharacter" function before iterate physics
        /******************Duck*************************/
public:
        void render(double camX, double camY);
        MatrixAnimator animator;
        int frameW;
        int frameH;
        bool locked();
        void setLocked(bool lock);
        bool isExiting;
        int  _exiting_swimTimer;

        /********************Lua Stuff*******************
                            .-""""-
                           F   .-'
                          F   J
                         I    I
                          L   `.
                           L    `-._,
                            `-.__.-'
         ***********************************************/
        virtual void lua_onLoop(){}
        bool isLuaPlayer;
        /********************Lua Stuff******************/

        bool isInited();
private:
        bool _no_render;
        bool isLocked;
        bool _isInited;
        void refreshAnimation();
};

#endif // LVL_PLAYER_H
