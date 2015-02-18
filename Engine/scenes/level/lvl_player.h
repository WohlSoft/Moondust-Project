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

#ifndef LVL_PLAYER_H
#define LVL_PLAYER_H

#include "../../graphics/lvl_camera.h"
#include "lvl_warp.h"

#include "../../physics/base_object.h"
#include "../../controls/controllable_object.h"
#include <file_formats.h>
#include <QMap>

struct Plr_EnvironmentPhysics
{
    inline void make() {} //!< Dummy function
    float32 walk_force; //!< Move force
    float32 slippery_c; //!< Slippery coefficien
    float32 gravity_scale; //!< Gravity scale
    float32 velocity_jump; //!< Jump velocity
    float32 velocity_climb; //!< Climbing velocity
    float32 MaxSpeed_walk; //!< Max walk speed
    float32 MaxSpeed_run; //!< Max run speed
    float32 MaxSpeed_up; //!< Fly UP Max fall speed
    float32 MaxSpeed_down; //!< Max fall down speed
    bool    zero_speed_on_enter;
};

struct Plr_State
{
    inline void make() {} //!< Dummy function
    int width;
    int height;
   bool duck_allow;
    int duck_height;
};

class LVL_Player :
        public PGE_Phys_Object,
        public ControllableObject
{
    public:
        LVL_Player();
        ~LVL_Player();
        void init();
        void initSize();
        void update(float ticks=1.0f);

        int playerID;

        QMap<int, Plr_State > states;
        int stateID;

        QMap<int, Plr_EnvironmentPhysics > physics;
        QMap<int, int > environments_map;
        int environment;
        int last_environment;

        float32 curHMaxSpeed; //!< Current moving max speed
        bool isRunning;

        bool JumpPressed;
        bool onGround;

        bool bumpDown;
        bool bumpUp;
        float bumpVelocity;
        void bump(bool _up=false);

        int foot_contacts;
        QMap<int, int > foot_contacts_map;   //!< staying on ground surfaces
        QMap<int, int > foot_sl_contacts_map;//!< Slipery surfaces
        int jumpForce;

        QMap<int, int > climbable_map;
        bool climbing;

        bool isLive;
        enum deathReason
        {
            DEAD_fall=0,
            DEAD_burn,
            DEAD_killed
        };
        void kill(deathReason reason=DEAD_killed);
        bool doKill;
        deathReason kill_reason;

        int health;
        bool doHarm;
        int doHarm_damage;
        void harm(int _damage=1);

        bool contactedWithWarp;
        LVL_Warp * contactedWarp;
        bool wasTeleported;
        bool wasEntered;
        int warpsTouched;

        float32 gscale_Backup;

        bool   isWarping;
        bool   warpDo;
        int    warpDirect;
        uint32 warpWaitTicks;

        //floating
        bool allowFloat;
        bool isFloating;
        float timeToFloat;
        float maxFloatTime;

        PlayerPoint data;

        PGE_LevelCamera * camera;

        void teleport(float x, float y);
        void exitFromLevel(QString levelFile, int targetWarp);

        void render(float camX, float camY);
};

#endif // LVL_PLAYER_H
