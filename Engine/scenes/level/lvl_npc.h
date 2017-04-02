#ifndef LVL_NPC_H
#define LVL_NPC_H

#include "lvl_base_object.h"
#include <data_configs/obj_npc.h>
#include <PGE_File_Formats/file_formats.h>
#include <common_features/npc_animator.h>
#include <common_features/event_queue.h>
#include <common_features/pointf.h>
#include "npc_detectors/lvl_base_detector.h"
#include "npc_detectors/lvl_dtc_player_pos.h"
#include "npc_detectors/lvl_dtc_contact.h"
#include "npc_detectors/lvl_dtc_inarea.h"
#include <Utils/vptrlist.h>

#include <luabind/luabind.hpp>
#include <lua_inclues/lua.hpp>
#include <set>
#include <unordered_set>

class LVL_Player;
class LVL_Npc : public PGE_Phys_Object
{
public:
    LVL_Npc(LevelScene *_parent = NULL);
    LVL_Npc(const LVL_Npc &) = default;
    virtual ~LVL_Npc();
    void init();

    void setScenePointer(LevelScene *_pointer);

    LevelNPC getData();

    unsigned long _npc_id;//Current NPC-ID
    LevelNPC data; //Local settings

    //! Default GFX offset
    PGE_PointF offset;

    //! Configurable GFX Offset from lua scripts
    PGE_PointF lua_offset;

    PGE_Size frameSize;
    AdvNpcAnimator animator;

    void setDirection(int dir);
    int  direction();
    int _direction = 0;
    double motionSpeed = 0.0;
    bool  is_scenery = false;
    bool  is_activity = false;
    bool  is_shared_animation = false;
    bool  keep_position_on_despawn = false;

    bool animated = false;
    long animator_ID = 0;

    obj_npc *setup = nullptr;//Global config
    bool isKilled();
    bool killed = false;
    enum DamageReason
    {
        DAMAGE_NOREASON = 0,
        DAMAGE_STOMPED,   //on stomping to head
        DAMAGE_BY_KICK,   //caused by contact with throwned NPC's
        DAMAGE_BY_PLAYER_ATTACK, //Caused by attaking by player
        //(for example, by sword, by fists, by teeth sting, by blow claws, by whip, etc.)
        DAMAGE_TAKEN, //is Power up taken
        DAMAGE_LAVABURN, //Does NPC burn in lava
        DAMAGE_PITFALL, //Does NPC fell into the pit
        DAMAGE_CUSTOM_REASON
    };
    void doHarm(int damageReason);
    void harm(int damage = 1, int damageReason = DAMAGE_NOREASON);

    void talkWith();

    void kill(int damageReason);//! In-Game destroying of NPC with triggering of specific events
    void unregister();          //! Sielent destroying of NPC without triggering of the events

    int taskToTransform = 0;
    int taskToTransform_t = 0;
    void transformTo(unsigned long id, int type = 0);
    void transformTo_x(unsigned long id);
    LVL_Block *transformedFromBlock = nullptr;
    unsigned long transformedFromBlockID = 0;
    unsigned long transformedFromNpcID = 0;

    void update(double tickTime);
    void render(double camX, double camY);
    void setDefaults();
    void Activate();
    void deActivate();

    void processContacts();
    void preCollision();
    void postCollision();
    void collisionHitBlockTop(std::vector<PGE_Phys_Object *> &blocksHit);
    bool preCollisionCheck(PGE_Phys_Object *body);

    bool onCliff();

    /*****************NPC's and blocks******************/
    bool onGround();

    typedef std::unordered_set<PGE_Phys_Object *> CollisionTable;
    std::unordered_set<PGE_Phys_Object *> contacted_blocks;
    std::unordered_set<PGE_Phys_Object *> contacted_bgos;
    std::unordered_set<PGE_Phys_Object *> contacted_npc;
    std::unordered_set<PGE_Phys_Object *> contacted_players;
    inline void l_pushBlk(PGE_Phys_Object *ob)
    {
        contacted_blocks.insert(ob);
    }
    inline void l_pushBgo(PGE_Phys_Object *ob)
    {
        contacted_bgos.insert(ob);
    }
    inline void l_pushNpc(PGE_Phys_Object *ob)
    {
        contacted_npc.insert(ob);
    }
    inline void l_pushPlr(PGE_Phys_Object *ob)
    {
        contacted_players.insert(ob);
    }

    bool  m_disableBlockCollision = false;
    bool  disableNpcCollision = false;
    bool  enablePlayerCollision = false;
    bool _stucked = false;

    bool    bumpDown = false;
    bool    bumpUp = false;
    /***************************************************/
    /*******************Environmept*********************/
    std::unordered_map<intptr_t, intptr_t> environments_map;
    int     environment = 0;
    int     last_environment = 0;
    /*******************Environmept*********************/

    bool reSpawnable = false;
    bool isActivated = false;
    bool deActivatable = false;
    bool wasDeactivated = false;
    bool offSectionDeactivate = false;
    int  activationTimeout = 0;
    int  m_spawnedGeneratorType = 0;
    int  m_spawnedGeneratorDirection = 0;

    /********************Detectors**********************/
    //!< dummy detectors made directly from a base class, for a some tests
    VPtrList<BasicDetector>         detectors_dummy;
    //! Player position detectors (should have alone copy!)
    PlayerPosDetector               detector_player_pos;
    //! Detects position and direction of nearest player
    PlayerPosDetector *lua_installPlayerPosDetector();
    //! Is player touches selected relative area;
    VPtrList<InAreaDetector >       detectors_inarea;
    //! Detects is player(s) are enters into specific area relative to NPC's center
    InAreaDetector     *lua_installInAreaDetector(double left, double top, double right, double bottom, luabind::adl::object filters);
    //! Entire list of all detectors
    VPtrList<ContactDetector>       detectors_contact;
    //! Detects contacted elements
    ContactDetector    *lua_installContactDetector();

    //! Entire list of all detectors
    std::set<BasicDetector*>        detectors;

    /***************************************************/

    /*****Warp*Sprite*****/
    enum WarpingSide
    {
        WARP_TOP = 1,
        WARP_LEFT = 2,
        WARP_BOTTOM = 3,
        WARP_RIGHT = 4,
    };
    ///
    /// \brief setSpriteWarp Changes warping state of a sprite
    /// \param depth
    /// \param direction
    ///
    void setSpriteWarp(float depth, WarpingSide _direction = WARP_BOTTOM, bool resizedBody = false);
    void resetSpriteWarp();
    bool    isWarping = false;
    int     warpDirectO = 0.0;
    bool    warpResizedBody = false;
    float   warpSpriteOffset = 0.0f;
    float   warpFrameW = 0.0f;
    float   warpFrameH = 0.0f;
    /*********************/

    /***************************************************/
    void setWarpSpawn(WarpingSide side = WARP_TOP);
    bool warpSpawing = false;
    EventQueue<LVL_Npc> event_queue;
    /***************************************************/

    /***********************Generator*******************/
    bool  m_isGenerator = false;
    float generatorTimeLeft = 0.0f;
    int   generatorType = 0;
    int   generatorDirection = 0;
    void  updateGenerator(double tickTime);
    /***************************************************/

    /*******************Throwned*by*********************/
    void        resetThrowned();
    void        setThrownedByNpc(long npcID,    LVL_Npc *npcObj);
    void        setThrownedByPlayer(long playerID, LVL_Player *npcObj);
    long        thrownedByNpc();
    LVL_Npc    *thrownedByNpcObj();
    long        thrownedByPlayer();
    LVL_Player *thrownedByPlayerObj();
    long        throwned_by_npc = 0;
    LVL_Npc    *throwned_by_npc_obj = nullptr;
    long        throwned_by_player = 0;
    LVL_Player *throwned_by_player_obj = nullptr;
    /***************************************************/

    /*******************Buddies********************/
    //Allows communication between neighour NPC's of same type.
    void             buildBuddieGroup();
    void             updateBuddies(double tickTime);
    void             buildLeaf(std::vector<LVL_Npc *> &needtochec, std::vector<LVL_Npc *> *&list, LVL_Npc *leader);
    std::vector<LVL_Npc *> *buddies_list; //Destroys when was killed last NPC in this group
    bool             buddies_updated = false;
    LVL_Npc         *buddies_leader = nullptr;
    /**********************************************/

    class KillEvent
    {
    public:
        KillEvent();
        KillEvent(const KillEvent &ke);
        bool cancel = false;
        int  reason_code = 0;
        enum killedBy
        {
            self = 0,
            player,
            otherNPC
        };
        int         killed_by = self;
        LVL_Player *killer_p = nullptr;
        LVL_Npc    *killer_n = nullptr;
    };

    class HarmEvent
    {
    public:
        HarmEvent();
        HarmEvent(const HarmEvent &he);
        bool cancel = false;
        int  damage = 0;
        int  reason_code = 0;
        enum killedBy
        {
            self = 0,
            player,
            otherNPC
        };
        int         killed_by = self;
        LVL_Player *killer_p = nullptr;
        LVL_Npc    *killer_n = nullptr;
    };
    //Additional lua enums

    /********************Lua Stuff*******************
                        .-""""-
                       F   .-'
                      F   J
                     I    I
                      L   `.
                       L    `-._,
                        `-.__.-'
     ***********************************************/
    //Additional lua events
    virtual void lua_onActivated() {}
    virtual void lua_onLoop(double) {}
    virtual void lua_onInit() {}
    virtual void lua_onKill(KillEvent *) {}
    virtual void lua_onHarm(HarmEvent *) {}
    virtual void lua_onTransform(unsigned long) {}

    //Additional lua functions
    void lua_setSequenceLeft(luabind::object frames);
    void lua_setSequenceRight(luabind::object frames);
    void lua_setSequence(luabind::object frames);
    void lua_setOnceAnimation(bool en);
    bool lua_animationIsFinished();
    int  lua_frameDelay();
    void lua_setFrameDelay(int ms);
    int lua_activate_neighbours();
    LVL_Npc *lua_spawnNPC(int npcID, int sp_type, int sp_dir, bool reSpawnable = false);

    inline void lua_setGfxOffsetX(double x)
    {
        lua_offset.setX(x);
    }
    inline void lua_setGfxOffsetY(double y)
    {
        lua_offset.setY(y);
    }
    inline void lua_setGfxOffset(double x, double y)
    {
        lua_offset.setPoint(x, y);
    }

    inline bool not_movable()
    {
        return data.nomove;
    }
    inline void setNot_movable(bool n)
    {
        data.nomove = n;
    }
    inline long contents()
    {
        return data.contents;
    }
    inline void setContents(long c)
    {
        data.contents = c;
    }
    inline long special1()
    {
        return data.special_data;
    }
    inline void setSpecial1(long s)
    {
        data.special_data = s;
    }
    inline long special2()
    {
        return data.special_data2;
    }
    inline void setSpecial2(long s)
    {
        data.special_data2 = s;
    }
    inline bool isBoss()
    {
        return data.is_boss;
    }
    inline unsigned long getID()
    {
        return _npc_id;
    }
    inline long getHealth()
    {
        return health;
    }
    inline void setHealth(int _health)
    {
        health = _health;
    }
    inline bool getCollideWithBlocks()
    {
        return !m_disableBlockCollision;
    }
    inline void setCollideWithBlocks(bool blkcol)
    {
        m_disableBlockCollision = !blkcol;
    }

    inline bool getKillByFire()
    {
        return setup->setup.kill_by_fireball;
    }
    inline bool getKillByIce()
    {
        return setup->setup.freeze_by_iceball;
    }
    inline bool getKillByHammer()
    {
        return setup->setup.kill_hammer;
    }
    inline bool getKillByForcejump()
    {
        return setup->setup.kill_by_spinjump;
    }
    inline bool getKillByStatue()
    {
        return setup->setup.kill_by_statue;
    }
    inline bool getKillByVehicle()
    {
        return setup->setup.kill_by_mounted_item;
    }
    bool isLuaNPC = false;
    /********************Lua Stuff******************/

    int health = 1;

    /**Layers***/
    void show();
    void hide();
    void setVisible(bool visible);

    bool isInited();
private:
    bool m_isInited = false;

};

#endif // LVL_NPC_H
