#include "pge_boxbase.h"

#undef main
#include <SDL2/SDL_opengl.h>
#undef main
#include <common_features/graphics_funcs.h>


PGE_BoxBase::PGE_BoxBase()
{
    construct(NULL);
}

PGE_BoxBase::PGE_BoxBase(Scene *_parentScene)
{
    construct(_parentScene);
}

void PGE_BoxBase::construct(Scene *_parentScene)
{
    fader_opacity = 0.0f;
    fade_step = 0.02f;
    target_opacity=0.0f;
    fadeSpeed=10;
    _textureUsed=false;
    parentScene = _parentScene;
    if(!parentScene) glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
}

PGE_BoxBase::~PGE_BoxBase()
{
    if(_textureUsed)
    {
        glDisable(GL_TEXTURE_2D);
        glDeleteTextures(1, &styleTexture.texture);
    }

}

void PGE_BoxBase::exec()
{
    if(parentScene)
        parentScene->render();
    else
    {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glLoadIdentity();
    }
}



/**************************Fader*******************************/
void PGE_BoxBase::setFade(int speed, float target, float step)
{
    fade_step = fabs(step);
    target_opacity = target;
    fadeSpeed = speed;

    fader_timer_id = SDL_AddTimer(speed, &PGE_BoxBase::nextOpacity, this);
    if(!fader_timer_id) fader_opacity = target_opacity;
}

unsigned int PGE_BoxBase::nextOpacity(unsigned int x, void *p)
{
    Q_UNUSED(x);
    PGE_BoxBase *self = reinterpret_cast<PGE_BoxBase *>(p);
    self->fadeStep();
    return 0;
}

void PGE_BoxBase::fadeStep()
{
    if(fader_opacity < target_opacity)
        fader_opacity+=fade_step;
    else
        fader_opacity-=fade_step;

    if(fader_opacity>=1.0f || fader_opacity<=0.0f)
        SDL_RemoveTimer(fader_timer_id);
    else
        {
            fader_timer_id = SDL_AddTimer(fadeSpeed, &PGE_BoxBase::nextOpacity, this);
            if(!fader_timer_id) fader_opacity = target_opacity;
        }

    if(fader_opacity>1.0f) fader_opacity = 1.0f;
    else
        if(fader_opacity<0.0f) fader_opacity = 0.0f;
}

void PGE_BoxBase::loadTexture(QString path)
{
    styleTexture = GraphicsHelps::loadTexture(styleTexture, path);
    if(styleTexture.texture>0)
        _textureUsed=true;
}

void PGE_BoxBase::drawTexture(int left, int top, int right, int bottom, int border)
{
    QRect x;
    x.setLeft(left);
    x.setTop(top);
    x.setRight(right);
    x.setBottom(bottom);
    drawTexture(x, border);
}

/**************************Fader**end**************************/

/********************************Texture************************************/
void PGE_BoxBase::drawTexture(QRect _rect, int border)
{
    QRectF blockG = QRectF(_rect.x(),
                           _rect.y(),
                           _rect.width(),
                           _rect.height());

    if(_textureUsed)
    {
        glEnable(GL_TEXTURE_2D);
        glColor4f( 1.f, 1.f, 1.f, 1.f);
        glBindTexture(GL_TEXTURE_2D, styleTexture.texture);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

        int w = _rect.width();
        int h = _rect.height();

        int x,y, x2, y2, i, j;
        int hc, wc;

        x = border; // Width of one piece
        y = border; // Height of one piece

        //Double size
        x2 = x<<1;
        y2 = y<<1;

        int fLnt = 0; // Free Lenght
        int fWdt = 0; // Free Width

        int dX=0; //Draw Offset. This need for crop junk on small sizes
        int dY=0;

        if(w < x2) dX = (x2-w)/2; else dX=0;
        if(h < y2) dY = (y2-h)/2; else dY=0;

        //L Draw left border
        if(h > (y2))
        {
            hc=0;
            for(i=0; i<((h-y2) / y); i++ )
            {
                drawPiece(blockG, QRectF(0, x+hc, x-dX, y), QRectF(0, y, x-dX, y));
                hc+=x;
            }
                fLnt = (h-y2)%y;
                if( fLnt != 0)
                    drawPiece(blockG, QRectF(0, x+hc, x-dX, fLnt), QRectF(0, y, x-dX, fLnt));
        }

        //T Draw top border
        if(w > (x2))
        {
            hc=0;
            for(i=0; i<( (w-(x2)) / x); i++ )
            {
                drawPiece(blockG, QRectF(x+hc, 0, x, y-dY), QRectF(x, 0, x, y-dY));
                    hc+=x;
            }
                fLnt = (w-(x2))%x;
                if( fLnt != 0)
                    drawPiece(blockG, QRectF(x+hc, 0, fLnt, y-dY), QRectF(x, 0, fLnt, y-dY));
        }

        //B Draw bottom border
        if(w > (x2))
        {
            hc=0;
            for(i=0; i< ( (w-(x2)) / x); i++ )
            {
                drawPiece(blockG, QRectF(x+hc, h-y+dY, x, y-dY), QRectF(x, styleTexture.h-y+dY, x, y-dY));
                    hc+=x;
            }
            fLnt = (w-(x2))%x;
            if( fLnt != 0)
                drawPiece(blockG, QRectF(x+hc, h-y+dY, fLnt, y-dY), QRectF(x, styleTexture.h-y+dY, fLnt, y-dY));
        }

        //R Draw right border
        if(h > (y2))
        {
            hc=0;
            for(i=0; i<((h-(y2)) / y); i++ )
            {
                drawPiece(blockG, QRectF(w-x+dX, y+hc, x-dX, y), QRectF(styleTexture.w-x+dX, y, x-dX, y));
                    hc+=x;
            }
                fLnt = (h-y2)%y;
                if( fLnt != 0)
                    drawPiece(blockG, QRectF(w-x+dX, y+hc, x-dX, fLnt), QRectF(styleTexture.w-x+dX, y, x-dX, fLnt));
        }


        //C Draw center
        if( (w > (x2)) && (h > (y2)))
        {
            hc=0;
            wc=0;
            for(i=0; i<((h-y2) / y); i++ )
            {
                hc=0;
                for(j=0; j<((w-x2) / x); j++ )
                {
                    drawPiece(blockG, QRectF(x+hc, y+wc, x, y), QRectF(x, y, x, y));
                    hc+=x;
                }
                    fLnt = (w-x2)%x;
                    if(fLnt != 0 )
                        drawPiece(blockG, QRectF(x+hc, y+wc, fLnt, y), QRectF(x, y, fLnt, y));
                wc+=y;
            }

            fWdt = (h-y2)%y;
            if(fWdt !=0)
            {
                hc=0;
                for(j=0; j<((w-x2) / x); j++ )
                {
                    drawPiece(blockG, QRectF(x+hc, y+wc, x, y), QRectF(x, y, x, y));
                    hc+=x;
                }
                    fLnt = (w-x2)%x;
                    if(fLnt != 0 )
                        drawPiece(blockG, QRectF(x+hc, y+wc, fLnt, fWdt), QRectF(x, y, fLnt, fWdt));
            }

        }

        //Draw corners
         //1 Left-top
        drawPiece(blockG, QRectF(0,0,x-dX,y-dY), QRectF(0,0,x-dX, y-dY));
         //2 Right-top
        drawPiece(blockG, QRectF(w-x+dX, 0, x-dX, y-dY), QRectF(styleTexture.w-x+dX, 0, x-dX, y-dY));
         //3 Right-bottom
        drawPiece(blockG, QRectF(w-x+dX, h-y+dY, x-dX, y-dY), QRectF(styleTexture.w-x+dX, styleTexture.h-y+dY, x-dX, y-dY));
         //4 Left-bottom
        drawPiece(blockG, QRectF(0, h-y+dY, x-dX, y-dY), QRectF(0, styleTexture.h-y+dY, x-dX, y-dY));
    }
}

void PGE_BoxBase::drawPiece(QRectF target, QRectF block, QRectF texture)
{
    QRectF tx;
    tx.setLeft(texture.left()/this->styleTexture.w);
    tx.setRight(texture.right()/this->styleTexture.w);
    tx.setTop(texture.top()/this->styleTexture.h);
    tx.setBottom(texture.bottom()/this->styleTexture.h);

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
