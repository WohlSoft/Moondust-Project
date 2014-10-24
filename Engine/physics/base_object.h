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

    float top();
    float bottom();
    float left();
    float right();

    void setSize(float w, float h);
    virtual void setPos(long x, long y);

    float width;  //!< Width
    float height; //!< Height

    float posX_coefficient;
    float posY_coefficient;

    int type;

    enum CollisionType{
        COLLISION_NONE = 0,
        COLLISION_ANY = 1,
        COLLISION_TOP = 2,
        COLLISION_BOTTOM = 3
    };

    int collide;

    bool isRectangle;
    bool _player_moveup; //Protection from wrong collision

    b2Body* physBody;
    b2World * worldPtr;

    PGE_Texture texture;
    GLuint texId;
    GLdouble z_index;

    virtual void nextFrame();
    //Later add animator here
    /*
    glTexCoord2i( 0, AnimationPos );
    glVertex2f( blockG.left(), blockG.top());

    glTexCoord2i( 1, 0 );
    glVertex2f(  blockG.right(), blockG.top());

    glTexCoord2i( 1, AnimationPos+1/frames );
    glVertex2f(  blockG.right(),  blockG.bottom());

    glTexCoord2i( 0, AnimationPos+1/frames );
    glVertex2f( blockG.left(),  blockG.bottom());
    */

public:

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
    virtual void render(float x, float y);
};

#endif // BASE_OBJECT_H
