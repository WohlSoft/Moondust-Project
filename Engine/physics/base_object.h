#ifndef BASE_OBJECT_H
#define BASE_OBJECT_H

#include <Box2D/Box2D.h>
#include "phys_util.h"
#include "../graphics/graphics.h"

///
/// \brief The PGE_Phys_Object class
///
/// Is a base of any physical objects
class PGE_Phys_Object
{
public:
    PGE_Phys_Object();
    ~PGE_Phys_Object();
    long posX(); //!< Position X
    long posY(); //!< Position Y
    long setSize(float w, float h);

    float width;  //!< Width
    float height; //!< Height

    float posX_coefficient;
    float posY_coefficient;

    int type;

    b2Body* physBody;
    b2World * worldPtr;

    PGE_Texture * texture;

    enum types
    {
        LVL_Unknown=0,
        LVL_Block,
        LVL_BGO,
        LVL_NPC,
        LVL_Player,
        LVL_Effect,
        LVL_Special
    };

    virtual void update();
    virtual void render();
};

#endif // BASE_OBJECT_H
