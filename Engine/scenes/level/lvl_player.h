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
        void update(float ticks=1.0f);

        int playerID;

        obj_player setup;

        QHash<int, obj_player_state > states;
        int stateID;
        obj_player_state state_cur;

        QHash<int, obj_player_physics > physics;
        QHash<int, int > environments_map;

        obj_player_physics physics_cur;
        int environment;
        int last_environment;

        float32 curHMaxSpeed; //!< Current moving max speed
        bool isRunning;

        int direction;

        bool JumpPressed;
        bool onGround;

        bool bumpDown;
        bool bumpUp;
        float bumpVelocity;
        void bump(bool _up=false);

        int foot_contacts;
        QHash<int, int > foot_contacts_map;   //!< staying on ground surfaces
        QHash<int, int > foot_sl_contacts_map;//!< Slipery surfaces
        int jumpForce;

        QHash<int, int > climbable_map;
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

        float32 gscale_Backup; //!< BackUP of last gravity scale

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
        void exitFromLevel(QString levelFile, int targetWarp, long wX=-1, long wY=-1);

        void render(float camX, float camY);
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
