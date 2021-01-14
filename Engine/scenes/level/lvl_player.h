/*
 * Moondust, a free game engine for platform game making
 * Copyright (c) 2014-2021 Vitaly Novichkov <admin@wohlnet.ru>
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

#ifndef LVL_PLAYER_H
#define LVL_PLAYER_H

#include "lvl_camera.h"
#include "lvl_warp.h"
#include "lvl_npc.h"

#include "lvl_base_object.h"
#include <PGE_File_Formats/file_formats.h>
#include <controls/controllable_object.h>
#include <data_configs/obj_player.h>
#include <common_features/matrix_animator.h>
#include <common_features/point_mover.h>
#include <common_features/event_queue.h>
#include <common_features/data_array.h>
#include <vector>

class LVL_Player_harm_event
{
public:
    LVL_Player_harm_event();
    bool    doHarm;
    int     doHarm_damage;
};

class LVL_Section;
class LVL_Block;
class lua_LevelPlayerState;

class LVL_Player :
    public PGE_Phys_Object,
    public ControllableObject
{
public:
    LVL_Player(LevelScene *_parent = NULL);
    virtual ~LVL_Player();
    bool setCharacter(unsigned long CharacterID, unsigned long _stateID);
    void setCharacterSafe(unsigned long CharacterID, unsigned long _stateID);
private:
    bool _doSafeSwitchCharacter;
public:
    void setPlayerPointInfo(PlayerPoint pt);
    bool init();
    void update(double tickTime);
    void updateCamera();

    void refreshEnvironmentState();

    void processContacts();
    void preCollision();
    void postCollision();
    void collisionHitBlockTop(std::vector<PGE_Phys_Object *> &blocksHit);
    bool preCollisionCheck(PGE_Phys_Object *body);

    bool forceCollideCenter;//!< collide with invizible blocks at center
    double _heightDelta; //Delta of changing height. Need to protect going through block on character switching

    /*****************NPC's and blocks******************/
    typedef std::unordered_map<int, PGE_Phys_Object *> PlayerColliders;
    LVL_Npc   *collided_talkable_npc;
    bool _stucked;
    /***************************************************/

    int playerID;
    obj_player setup;
    PlayerPoint data;
    PGE_LevelCamera *camera; //!< Connected camera

    void teleport(double x, double y);
    void exitFromLevel(std::string levelFile, unsigned long targetWarp, long wX = -1, long wY = -1);

    PGE_DataArray<obj_player_state > states;
    unsigned long   characterID;
    unsigned long   stateID;
    obj_player_state state_cur;


    enum kill_npc_reasons
    {
        NPC_Unknown = -1,
        NPC_Stomped = 0,
        NPC_Kicked,
        NPC_Taked_Coin,
        NPC_Taked_Powerup
    };

    void kill_npc(LVL_Npc *target, kill_npc_reasons reason);
    //QQueue<LVL_Npc*> npc_queue;

    /*******************Environmept*********************/
    PGE_DataArray<obj_player_physics > physics;
    std::unordered_map<int, int> environments_map;

    obj_player_physics  physics_cur;
    int                 environment = 0;
    int                 last_environment = 0;
    /*******************Environmept*********************/

    /*******************Motion*************************/
    bool    isRunning();
    bool    m_isRunning;

    int     direction();
    int     m_direction;
    /*******************Motion*************************/

    /*******************Life and Death*****************/
    bool isAlive;
    enum deathReason
    {
        DEAD_fall = 0,
        DEAD_burn,
        DEAD_killed
    };
    void    kill(deathReason reason = DEAD_killed);
    void    unregister();

    bool    m_killDo;
    deathReason m_killReason;

    int     m_health;
    bool    m_invincible;
    double  m_invincibleDelay;
    bool    m_blinkScreen = false;
    void    harm(int _damage = 1);
    void    setInvincible(bool state, double delay, bool enableScreenBlink = false);
    /*******************Life and Death*****************/

    double  m_gscale_Backup; //!< BackUP of last gravity scale

    /********************Jumps***************************/
    bool    m_jumpPressed = false;
    bool    onGround();
    std::unordered_map<intptr_t, PGE_Phys_Object * > foot_contacts_map;  //!< staying on ground surfaces
    std::unordered_map<intptr_t, PGE_Phys_Object * > foot_sl_contacts_map; //!< Slipery surfaces
    double  m_jumpTime;
    double  m_jumpVelocity;
    /********************Jumps***************************/

    /********************Bump***************************/
    bool    m_bumpDown = false;
    bool    m_bumpUp = false;
    double  m_bumpVelocity;//!< down velocity
    double  m_bumpJumpVelocity;//! Up jump velocity
    int     m_bumpJumpTime;    //!< Up jump time
    void    bump(bool _up = false, double bounceSpeed = 0.0, int timeToJump = 0);
    inline void bumpf(bool _up, float bounceSpeed = 0.0f, int timeToJump = 0)
    {
        bump(_up, static_cast<double>(bounceSpeed), timeToJump);
    }
    /********************Bump***************************/

    /********************Climbing***************************/
    std::unordered_map<intptr_t, PGE_Phys_Object * > climbable_map;
    bool        m_climbing = false;
    double      m_climbableHeight;
    /********************Climbing***************************/

    /*******************Warps*********************/
    PointMover  m_cameraMover;
    bool        m_contactedWithWarp;
    LVL_Warp    *m_contactedWarp;
    bool        m_wasEntered;
    int         m_wasEnteredTimeout;
    bool        m_isWarping;

    int         m_warpDirectO;
    double      m_warpPipeOffset;
    double      m_warpFrameW;
    double      m_warpFrameH;

    EventQueue<LVL_Player > m_eventQueue;
    void    processWarpChecking();
    void    WarpTo(double x, double y, int warpType, int warpDirection = 1, bool cannon = false, double cannon_speed = 10.0);
    void    WarpTo(const LevelDoor &warp);
    /*******************Warps*********************/\

    /******************floating*******************/
    bool    m_floatingAllow;
    bool    m_floatingIsWorks;
    double  m_floatingTimer;
    double  m_floatingMaxtime;
    bool    m_floatingStartType;//!< true= sin(time), false= cos(time)
    /******************floating*******************/

    /******************Attack*******************/
    /*This feature will provide teporary ability to break any nearest blocks*/
    enum AttackDirection
    {
        Attack_Forward = 0,
        Attack_Up,
        Attack_Down
    };
    bool    m_attackEnabled;
    bool    m_attackPressed;
    enum AttackType
    {
        //! Regular hit
        AttackType_Hit = 0,
        //! Force-destroy found objects, used by "Chunk Norris" cheat attack
        AttackType_ForceDestroy,
        //! User-defined types (must be last in the list!)
        AttackType_User
    };

    /**
     * @brief "Chuck Norris" Attack [cheat code]
     * @param _dir Direction of attack (Enum AttackDirection)
     */
    void    attackCN(AttackDirection _dir);

    /**
     * @brief Attack the given area relative to playable character
     * @param area area relative to playable character's face
     * @param type Type of attack (AttackType Enum)
     */
    void    attackArea(PGE_RectF area, int action, int type = LVL_Npc::DAMAGE_BY_PLAYER_ATTACK, int filters = F_LVLBlock|F_LVLNPC);
    void    lua_attackArea(double left, double top, double right, double bottom, int action, int type, luabind::adl::object filters);
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
    bool m_duckAllow;
    bool m_ducking;
    void setDuck(bool duck);
    /******************Duck*************************/
public:
    void render(double camX, double camY);
    MatrixAnimator m_animator;
    int m_frameW;
    int m_frameH;
    bool locked();
    void setLocked(bool lock);
    bool m_isExiting;
    int  m_exiting_swimTimer;

    /********************Lua Stuff*******************
                        .-""""-
                       F   .-'
                      F   J
                     I    I
                      L   `.
                       L    `-._,
                        `-.__.-'
     ***********************************************/

    lua_LevelPlayerState *m_global_state;

    /*Controller processing*/
    void lua_processKeyEvents();
    void lua_updateKey(bool &target_key, ControllableObject::KeyType ktype, bool &state);
    controller_keys m_keysPrev;
    inline bool lua_getKeyState(int keyType)
    {
        return  getKeyState(keyType);
    }
    virtual void lua_onKeyPressed(ControllableObject::KeyType) {}
    virtual void lua_onKeyReleased(ControllableObject::KeyType) {}
    /*---------------------*/

    virtual void lua_onInit() {}
    virtual void lua_onLoop(double) {}
    virtual void lua_onHarm(LVL_Player_harm_event *) {}
    virtual void lua_onTransform(unsigned long, unsigned long) {}
    virtual void lua_onTakeNpc(LVL_Npc *) {}
    virtual void lua_onKillNpc(LVL_Npc *) {}
    inline void  lua_playAnimationOnce(int animationID,
                                       int speed,
                                       bool fixed_speed,
                                       bool locked,
                                       int skipLastFrames)
    {
        m_animator.playOnce(static_cast<MatrixAnimator::MatrixAnimates>(animationID),
                          m_direction, speed, fixed_speed,
                          locked, skipLastFrames);
    }
    inline void lua_setAnimation(int animationID, int framespeed)
    {
        m_animator.switchAnimation(static_cast<MatrixAnimator::MatrixAnimates>(animationID),
                                 m_direction,
                                 framespeed);
    }
    inline int getHealth()
    {
        return m_health;
    }
    void setHealth(int _health);
    inline void setCharacterID(unsigned long _character)
    {
        setCharacterSafe(_character, stateID);
    }
    inline void setState(unsigned long _state)
    {
        setCharacterSafe(characterID, _state);
    }
    LVL_Npc *lua_spawnNPC(unsigned long npcID, int sp_type, int sp_dir, bool reSpawnable);
    bool m_isLuaPlayer;
    /********************Lua Stuff******************/

    bool isInited();

private:
    bool m_noRender;
    bool m_isLocked;
    bool m_isInited;
    void refreshAnimation();
};

#endif // LVL_PLAYER_H
