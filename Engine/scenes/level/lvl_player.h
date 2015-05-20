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

#include "../../physics/base_object.h"
#include "../../controls/controllable_object.h"
#include <data_configs/obj_player.h>
#include <common_features/matrix_animator.h>
#include <PGE_File_Formats/file_formats.h>
#include <common_features/event_queue.h>
#include <QMap>

class LVL_Player :
        public PGE_Phys_Object,
        public ControllableObject
{
    public:
        LVL_Player();
        ~LVL_Player();
        void init();
        void initSize();
        void update(float ticks);
        void update();

        int playerID;
        obj_player setup;
        PlayerPoint data;
        PGE_LevelCamera * camera;//!< Connected camera

        void teleport(float x, float y);
        void exitFromLevel(QString levelFile, int targetWarp, long wX=-1, long wY=-1);

        QHash<int, obj_player_state > states;
        int     stateID;
        obj_player_state state_cur;

        /*******************Environmept*********************/
        QHash<int, obj_player_physics > physics;
        QHash<int, int > environments_map;

        obj_player_physics physics_cur;
        int     environment;
        int     last_environment;
        /*******************Environmept*********************/

        /*******************Motion*************************/
        float32 curHMaxSpeed; //!< Current moving max speed
        bool    isRunning;
        int     direction;
        /*******************Motion*************************/

        /*******************Life and Death*****************/
        bool isLive;
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
        bool    doHarm;
        int     doHarm_damage;
        void harm(int _damage=1);
        /*******************Life and Death*****************/

        float32 gscale_Backup; //!< BackUP of last gravity scale

        /********************Jumps***************************/
        bool    JumpPressed;
        bool    onGround;
        int     foot_contacts;
        QHash<int, int > foot_contacts_map;   //!< staying on ground surfaces
        QHash<int, int > foot_sl_contacts_map;//!< Slipery surfaces
        float   jumpForce;
        float   jumpForce_default;
        /********************Jumps***************************/

        /********************Bump***************************/
        bool    bumpDown;
        bool    bumpUp;
        float   bumpVelocity;
        void    bump(bool _up=false);
        /********************Bump***************************/

        /********************Climbing***************************/
        QHash<int, int > climbable_map;
        bool climbing;
        /********************Climbing***************************/

        /*******************Warps*********************/
        bool    contactedWithWarp;
        LVL_Warp * contactedWarp;
        bool    wasTeleported;
        bool    wasEntered;
        int     warpsTouched;
        bool    isWarping;
        bool    warpDo;

        int     warpDirect;

        int     warpDirectO;
        float   warpPipeOffset;
        uint32  warpWaitTicks;
        float   warpFrameW;
        float   warpFrameH;

        EventQueue<LVL_Player > event_queue;
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
        b2Fixture     *f_player;
        /******************Duck*************************/

        void render(double camX, double camY);
        MatrixAnimator animator;
        int frameW;
        int frameH;
        bool locked();
        void setLocked(bool lock);

private:
        bool isLocked;
        bool isInited;
        void refreshAnimation();
};

#endif // LVL_PLAYER_H
