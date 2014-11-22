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

#include "lvl_block.h"
#include "../../data_configs/config_manager.h"

#include "lvl_scene_ptr.h"

LVL_Block::LVL_Block()
{
    type = LVLBlock;
    data = NULL;
    animated=false;
    sizable=false;
    animator_ID=0;

    offset_x = 0.f;
    offset_y = 0.f;

    isHidden=false;
    destroyed=false;
}

LVL_Block::~LVL_Block()
{
    if(physBody && worldPtr)
    {
      worldPtr->DestroyBody(physBody);
      physBody->SetUserData(NULL);
      physBody = NULL;
    }
}

//float LVL_Block::posX()
//{
//    return data->x;
//}

//float LVL_Block::posY()
//{
//    return data->y;
//}

void LVL_Block::init()
{
    if(!worldPtr) return;
    setSize(data->w, data->h);

    slippery = data->slippery;

    sizable = setup->sizable;

    isHidden = data->invisible;

    if((setup->sizable) || (setup->collision==2))
    {
        collide = COLLISION_TOP;
    }
    else
    if(setup->collision==0)
    {
        collide = COLLISION_NONE;
    }


    b2BodyDef bodyDef;
    bodyDef.type = b2_staticBody;
    bodyDef.position.Set( PhysUtil::pix2met( data->x+posX_coefficient ),
        PhysUtil::pix2met(data->y + posY_coefficient ) );
    bodyDef.userData = (void*)dynamic_cast<PGE_Phys_Object *>(this);
    physBody = worldPtr->CreateBody(&bodyDef);

    b2PolygonShape shape;

    switch(setup->phys_shape)
    {
    //triangles
    case 1: //up-left
        {
          b2Vec2 vertices[3];
          //Left-top
          vertices[0].Set(PhysUtil::pix2met(-posX_coefficient),  PhysUtil::pix2met(-posY_coefficient));
          //right-bottom
          vertices[1].Set(PhysUtil::pix2met(posX_coefficient),  PhysUtil::pix2met(posY_coefficient));
          //left-bottom
          vertices[2].Set(PhysUtil::pix2met(-posX_coefficient), PhysUtil::pix2met(posY_coefficient));
          shape.Set(vertices, 3);
          isRectangle = false;
          break;
        }
    case -1: // Up-right
        {
          b2Vec2 vertices[3];
          //Left-bottom
          vertices[0].Set(PhysUtil::pix2met(-posX_coefficient),  PhysUtil::pix2met(posY_coefficient));
          //right-top
          vertices[1].Set(PhysUtil::pix2met(posX_coefficient),  PhysUtil::pix2met(-posY_coefficient));
          //right-bottom
          vertices[2].Set(PhysUtil::pix2met(posX_coefficient), PhysUtil::pix2met(posY_coefficient));
          shape.Set(vertices, 3);
          isRectangle = false;
          break;
        }
    case 2: //right
        {
          b2Vec2 vertices[3];
          //Left-top
          vertices[0].Set(PhysUtil::pix2met(-posX_coefficient),  PhysUtil::pix2met(-posY_coefficient));
          //right-top
          vertices[1].Set(PhysUtil::pix2met(posX_coefficient),  PhysUtil::pix2met(-posY_coefficient));
          //left-bottom
          vertices[2].Set(PhysUtil::pix2met(-posX_coefficient), PhysUtil::pix2met(posY_coefficient));
          shape.Set(vertices, 3);
          isRectangle = false;
          break;
        }
    case -2: //left
        {
          b2Vec2 vertices[3];
          //Left-top
          vertices[0].Set(PhysUtil::pix2met(-posX_coefficient),  PhysUtil::pix2met(-posY_coefficient));
          //right-top
          vertices[1].Set(PhysUtil::pix2met(posX_coefficient),  PhysUtil::pix2met(-posY_coefficient));
          //right-bottom
          vertices[2].Set(PhysUtil::pix2met(posX_coefficient), PhysUtil::pix2met(posY_coefficient));
          shape.Set(vertices, 3);
          isRectangle = false;
          break;
        }
    case 3:// map shape from texture
        //Is not implemented, create the box:

    //rectangle box
    default: //Box shape
        shape.SetAsBox(PhysUtil::pix2met(posX_coefficient), PhysUtil::pix2met(posY_coefficient) );
    }

    b2Fixture * block = physBody->CreateFixture(&shape, 1.0f);

    if(setup->algorithm==3)
        {
            block->SetSensor(true);
            ConfigManager::Animator_Blocks[animator_ID].setFrames(1, -1);
        }

    if(collide==COLLISION_NONE)// || collide==COLLISION_TOP)
        block->SetSensor(true);

    block->SetFriction(data->slippery? 0.04f : 0.25f );

}


void LVL_Block::render(float camX, float camY)
{
    //Don't draw hidden block before it will be hitten
    if(isHidden) return;
    if(destroyed) return;

    QRectF blockG = QRectF(posX()-camX+offset_x,
                           posY()-camY+offset_y,
                           width,
                           height);


    AniPos x(0,1);

    if(animated) //Get current animated frame
        x = ConfigManager::Animator_Blocks[animator_ID].image();

    glEnable(GL_TEXTURE_2D);
    glColor4f( 1.f, 1.f, 1.f, 1.f);

    glBindTexture( GL_TEXTURE_2D, texId );
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    if(sizable)
    {
        int w = width;
        int h = height;

        int x,y, i, j;
        int hc, wc;

        x = qRound(qreal(texture.w)/3);  // Width of one piece
        y = qRound(qreal(texture.h)/3); // Height of one piece

        int fLnt = 0; // Free Lenght
        int fWdt = 0; // Free Width

        int dX=0; //Draw Offset. This need for crop junk on small sizes
        int dY=0;

        if(w < 2*x) dX = (2*x-w)/2; else dX=0;
        if(h < 2*y) dY = (2*y-h)/2; else dY=0;

        //L Draw left border
        if(h > 2*y)
        {
            hc=0;
            for(i=0; i<((h-2*y) / y); i++ )
            {
                drawPiece(blockG, QRectF(0, x+hc, x-dX, y), QRectF(0, y, x-dX, y));
                hc+=x;
            }
                fLnt = (h-2*y)%y;
                if( fLnt != 0)
                    drawPiece(blockG, QRectF(0, x+hc, x-dX, fLnt), QRectF(0, y, x-dX, fLnt));
        }

        //T Draw top border
        if(w > 2*x)
        {
            hc=0;
            for(i=0; i<( (w-2*x) / x); i++ )
            {
                drawPiece(blockG, QRectF(x+hc, 0, x, y-dY), QRectF(x, 0, x, y-dY));
                    hc+=x;
            }
                fLnt = (w-2*x)%x;
                if( fLnt != 0)
                    drawPiece(blockG, QRectF(x+hc, 0, fLnt, y-dY), QRectF(x, 0, fLnt, y-dY));
        }

        //B Draw bottom border
        if(w > 2*x)
        {
            hc=0;
            for(i=0; i< ( (w-2*x) / x); i++ )
            {
                drawPiece(blockG, QRectF(x+hc, h-y+dY, x, y-dY), QRectF(x, texture.w-y+dY, x, y-dY));
                    hc+=x;
            }
                fLnt = (w-2*x)%x;
                if( fLnt != 0)
                    drawPiece(blockG, QRectF(x+hc, h-y+dY, fLnt, y-dY), QRectF(x, texture.w-y+dY, fLnt, y-dY));
        }

        //R Draw right border
        if(h > 2*y)
        {
            hc=0;
            for(i=0; i<((h-2*y) / y); i++ )
            {
                drawPiece(blockG, QRectF(w-x+dX, y+hc, x-dX, y), QRectF(texture.w-x+dX, y, x-dX, y));
                    hc+=x;
            }
                fLnt = (h-2*y)%y;
                if( fLnt != 0)
                    drawPiece(blockG, QRectF(w-x+dX, y+hc, x-dX, fLnt), QRectF(texture.w-x+dX, y, x-dX, fLnt));
        }


        //C Draw center
        if( w > 2*x && h > 2*y)
        {
            hc=0;
            wc=0;
            for(i=0; i<((h-2*y) / y); i++ )
            {
                hc=0;
                for(j=0; j<((w-2*x) / x); j++ )
                {
                    drawPiece(blockG, QRectF(x+hc, y+wc, x, y), QRectF(x, y, x, y));
                    hc+=x;
                }
                    fLnt = (w-2*x)%x;
                    if(fLnt != 0 )
                        drawPiece(blockG, QRectF(x+hc, y+wc, fLnt, y), QRectF(x, y, fLnt, y));
                wc+=y;
            }

            fWdt = (h-2*y)%y;
            if(fWdt !=0)
            {
                hc=0;
                for(j=0; j<((w-2*x) / x); j++ )
                {
                    drawPiece(blockG, QRectF(x+hc, y+wc, x, y), QRectF(x, y, x, y));
                    hc+=x;
                }
                    fLnt = (w-2*x)%x;
                    if(fLnt != 0 )
                        drawPiece(blockG, QRectF(x+hc, y+wc, fLnt, fWdt), QRectF(x, y, fLnt, fWdt));
            }

        }

        //Draw corners
         //1 Left-top
        drawPiece(blockG, QRectF(0,0,x-dX,y-dY), QRectF(0,0,x-dX, y-dY));
         //2 Right-top
        drawPiece(blockG, QRectF(w-x+dX, 0, x-dX, y-dY), QRectF(texture.w-x+dX, 0, x-dX, y-dY));
         //3 Right-bottom
        drawPiece(blockG, QRectF(w-x+dX, h-y+dY, x-dX, y-dY), QRectF(texture.w-x+dX, texture.h-y+dY, x-dX, y-dY));
         //4 Left-bottom
        drawPiece(blockG, QRectF(0, h-y+dY, x-dX, y-dY), QRectF(0, texture.h-y+dY, x-dX, y-dY));

    }
    else
    {
        glBegin( GL_QUADS );
            glTexCoord2f( 0, x.first );
            glVertex2f( blockG.left(), blockG.top());

            glTexCoord2f( 1, x.first );
            glVertex2f(  blockG.right(), blockG.top());

            glTexCoord2f( 1, x.second );
            glVertex2f(  blockG.right(),  blockG.bottom());

            glTexCoord2f( 0, x.second );
            glVertex2f( blockG.left(),  blockG.bottom());
        glEnd();
    }

    glDisable(GL_TEXTURE_2D);
}

void LVL_Block::drawPiece(QRectF target, QRectF block, QRectF texture)
{
    QRectF tx;
    tx.setLeft(texture.left()/this->texture.w);
    tx.setRight(texture.right()/this->texture.w);
    tx.setTop(texture.top()/this->texture.h);
    tx.setBottom(texture.bottom()/this->texture.h);

    QRectF blockG;
    blockG.setX(target.x()+block.x());
    blockG.setY(target.y()+block.y());
    blockG.setRight(target.x()+block.x()+block.width());
    blockG.setBottom(target.y()+block.y()+block.height());

    glBegin( GL_QUADS );
        glTexCoord2f( tx.left(), tx.top() );
        glVertex2f( blockG.left(), blockG.top());

        glTexCoord2f( tx.right(), tx.top() );
        glVertex2f(  blockG.right(), blockG.top());

        glTexCoord2f( tx.right(), tx.bottom() );
        glVertex2f(  blockG.right(),  blockG.bottom());

        glTexCoord2f( tx.left(), tx.bottom() );
        glVertex2f( blockG.left(),  blockG.bottom());
    glEnd();

}




void LVL_Block::hit(LVL_Block::directions _dir)
{
    hitDirection = _dir;
    isHidden=false;

    if(setup->destroyable)
    {
        destroyed=true;
        return;
    }

    if(setup->hitable)
    {
        fadeOffset=0.f;
        setFade(20, 1.0f, 0.2f);
    }
}


/**************************Fader*******************************/
void LVL_Block::setFade(int speed, float target, float step)
{
    fade_step = fabs(step);
    targetOffset = target;
    fadeSpeed = speed;

    fader_timer_id = SDL_AddTimer(speed, &LVL_Block::nextOpacity, this);
    if(!fader_timer_id) fadeOffset = targetOffset;
}

unsigned int LVL_Block::nextOpacity(unsigned int x, void *p)
{
    Q_UNUSED(x);
    LVL_Block *self = reinterpret_cast<LVL_Block *>(p);
    self->fadeStep();
    return 0;
}

void LVL_Block::fadeStep()
{
    if(fadeOffset < targetOffset)
        fadeOffset+=fade_step;
    else
        fadeOffset-=fade_step;

    if(fadeOffset>=1.0f || fadeOffset<=0.0f)
    {
        SDL_RemoveTimer(fader_timer_id);
        if(fadeOffset>=1.0f)
            setFade(fadeSpeed, 0.0f, fade_step);
    }
    else
        {
            fader_timer_id = SDL_AddTimer(fadeSpeed, &LVL_Block::nextOpacity, this);
            if(!fader_timer_id) fadeOffset = targetOffset;
        }

    if(fadeOffset>1.0f) fadeOffset = 1.0f;
    else
    if(fadeOffset<0.0f) fadeOffset = 0.0f;

    switch(hitDirection)
    {
        case up:      offset_y = -16*fadeOffset; break;
        case down:    offset_y = 16*fadeOffset; break;
        case left:    offset_x = -16*fadeOffset; break;
        case right:   offset_x = 16*fadeOffset; break;
        default: break;
    }

}
/**************************Fader**end**************************/
