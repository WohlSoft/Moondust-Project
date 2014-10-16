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
    virtual ~PGE_Phys_Object();
    virtual float posX(); //!< Position X
    virtual float posY(); //!< Position Y
    void setSize(float w, float h);
    virtual void setPos(long x, long y);

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
        LVLUnknown=0,
        LVLBlock,
        LVLBGO,
        LVLNPC,
        LVLPlayer,
        LVLEffect,
        LVLSpecial
    };

    virtual void update();
    virtual void render();
};

#endif // BASE_OBJECT_H
