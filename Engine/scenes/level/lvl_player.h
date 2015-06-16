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

        void updateCollisions();
        void solveCollision(PGE_Phys_Object *collided);
        LVL_Block *nearestBlock(QVector<LVL_Block *> &blocks);
        LVL_Block *nearestBlockY(QVector<LVL_Block *> &blocks);
        bool isWall(QVector<LVL_Block *> &blocks);
        bool isFloor(QVector<LVL_Block *> &blocks);
        bool forceCollideCenter;//!< collide with invizible blocks at center
        float _heightDelta; //Delta of changing height. Need to protect going through block on character switching

        /*****************NPC's and blocks******************/
        typedef QHash<int, PGE_Phys_Object*> PlayerColliders;
        QHash<int, PGE_Phys_Object*> collided_top;
        QHash<int, PGE_Phys_Object*> collided_left;
        QHash<int, PGE_Phys_Object*> collided_right;
        QHash<int, PGE_Phys_Object*> collided_bottom;
        QHash<int, PGE_Phys_Object*> collided_center;
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
        bool    isRunning;
        int     direction;
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
        bool    doHarm;
        int     doHarm_damage;
        void harm(int _damage=1);
        /*******************Life and Death*****************/

        float   gscale_Backup; //!< BackUP of last gravity scale

        /********************Jumps***************************/
        bool    JumpPressed;
        bool    onGround;
        int     foot_contacts;
        bool    on_slippery_surface;
        QHash<int, int > foot_contacts_map;   //!< staying on ground surfaces
        QHash<int, int > foot_sl_contacts_map;//!< Slipery surfaces
        float   jumpTime;
        float   jumpTime_default;
        /********************Jumps***************************/

        /********************Bump***************************/
        bool    bumpDown;
        bool    bumpUp;
        float   bumpVelocity;
        void    bump(bool _up=false);
        /********************Bump***************************/

        /********************Climbing***************************/
        QHash<int, int > climbable_map;
        bool      climbing;
        double    climbableHeight;
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
