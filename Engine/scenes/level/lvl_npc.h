#ifndef LVL_NPC_H
#define LVL_NPC_H

#include "lvl_base_object.h"
#include <data_configs/obj_npc.h>
#include <PGE_File_Formats/file_formats.h>
#include <common_features/npc_animator.h>
#include <common_features/pointf.h>

#include <luabind/luabind.hpp>
#include <lua_inclues/lua.hpp>

#include <QHash>

/*********************Dummy NPC*************************
 * Before new physical engine will be implemented, this pseudo-NPC will provide
 * level event control system and exit items
 */

class LVL_Npc : public PGE_Phys_Object
{
public:
    LVL_Npc();
    virtual ~LVL_Npc();
    void init();

    LevelNPC data; //Local settings
    PGE_PointF offset;
    PGE_Size frameSize;
    AdvNpcAnimator animator;

    int direction;
    QHash<int, int> blocks_left;
    QHash<int, int> blocks_right;
    float motionSpeed;
    bool  is_scenery;

    bool animated;
    long animator_ID;

    obj_npc *setup;//Global config
    bool killed;
    void kill();

    int taskToTransform;
    int taskToTransform_t;
    void transformTo(long id, int type=0);
    void transformTo_x(long id);

    void update(float tickTime);
    void render(double camX, double camY);
    void setDefaults();
    void Activate();
    void deActivate();

    bool isActivated;
    int timeout;

    /*****Warp*Sprite*****/
    enum WarpingSide{
        WARP_TOP=1,
        WARP_LEFT=2,
        WARP_BOTTOM=3,
        WARP_RIGHT=4,
    };
    ///
    /// \brief setSpriteWarp Changes warping state of a sprite
    /// \param depth
    /// \param direction
    ///
    void setSpriteWarp(float depth, WarpingSide direction=WARP_BOTTOM);
    void resetSpriteWarp();
    bool    isWarping;
    int     warpDirectO;
    float   warpSpriteOffset;
    float   warpFrameW;
    float   warpFrameH;
    /*********************/

    //Additional lua events
    virtual void lua_onActivated() {}
    virtual void lua_onLoop(float) {}
    virtual void lua_onInit() {}
    //Additional lua functions
    void lua_setSequenceLeft(luabind::object frames);
    void lua_setSequenceRight(luabind::object frames);
    void lua_setSequence(luabind::object frames);


    inline int getID() { return setup->id; }
    bool isLuaNPC;


    bool isInited();
private:
    bool _isInited;

};

#endif // LVL_NPC_H
