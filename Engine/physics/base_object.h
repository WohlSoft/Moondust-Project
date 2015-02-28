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

    bool slippery_surface;
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
        LVLWarp,
        LVLSpecial,
        LVLPhysEnv
    };

    virtual void update();
    virtual void render(float x, float y);
};

#endif // BASE_OBJECT_H
