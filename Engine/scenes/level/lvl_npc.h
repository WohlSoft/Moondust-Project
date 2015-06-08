#ifndef LVL_NPC_H
#define LVL_NPC_H

#include "../../physics/base_object.h"
#include "../../data_configs/obj_npc.h"
#include <PGE_File_Formats/file_formats.h>
#include <common_features/npc_animator.h>

#include <QHash>

/*********************Dummy NPC*************************
 * Before new physical engine will be implemented, this pseudo-NPC will provide
 * level event control system and exit items
 */

class LVL_Npc : public PGE_Phys_Object
{
public:
    LVL_Npc();
    ~LVL_Npc();
    void init();

    LevelNPC data; //Local settings
    PGE_Size offset;
    PGE_Size frameSize;
    AdvNpcAnimator animator;

    b2Fixture *f_npc;
    b2Fixture *f_edge;

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

    //float posX();
    //float posY();
    void update(float ticks);
    void render(double camX, double camY);
    void setDefaults();
    void Activate();
    void deActivate();

    bool isActivated;
    int timeout;
};

#endif // LVL_NPC_H
