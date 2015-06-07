#include "pge_boxbase.h"

#undef main
#include <SDL2/SDL_opengl.h>
#undef main
#include <common_features/graphics_funcs.h>
#include <graphics/gl_renderer.h>
#include <graphics/window.h>


PGE_BoxBase::PGE_BoxBase()
{
    construct(NULL);
}

PGE_BoxBase::PGE_BoxBase(Scene *_parentScene)
{
    construct(_parentScene);
}

PGE_BoxBase::PGE_BoxBase(const PGE_BoxBase &bb)
{
    fader_opacity=  bb.fader_opacity;
    target_opacity= bb.target_opacity;
    fade_step=      bb.fade_step;
    fadeSpeed=      bb.fadeSpeed;
    manual_ticks=   bb.manual_ticks;
    parentScene=    bb.parentScene;
    uTickf=         bb.uTickf;
    uTick=          bb.uTick;
    _textureUsed=   bb._textureUsed;
    styleTexture=   bb.styleTexture;
}

void PGE_BoxBase::construct(Scene *_parentScene)
{
    fader_opacity = 0.0f;
    fade_step = 0.02f;
    target_opacity= 0.0f;
    fadeSpeed=10;
    _textureUsed=false;
    setParentScene(_parentScene);
}

PGE_BoxBase::~PGE_BoxBase()
{
    if(_textureUsed)
    {
        GlRenderer::deleteTexture( styleTexture );
    }

}

void PGE_BoxBase::setParentScene(Scene *_parentScene)
{
    parentScene = _parentScene;
    if(!parentScene) glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
}

void PGE_BoxBase::exec()
{}

void PGE_BoxBase::update(float)
{}

void PGE_BoxBase::render()
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
    manual_ticks = speed;
}

bool PGE_BoxBase::tickFader(float ticks)
{
    if(fadeSpeed<1) return true; //Idling animation

    manual_ticks-=fabs(ticks);
        while(manual_ticks<=0.0f)
        {
            fadeStep();
            manual_ticks+=fadeSpeed;
        }
    return (fader_opacity==target_opacity);
}

void PGE_BoxBase::fadeStep()
{
    if(fader_opacity < target_opacity)
        fader_opacity+=fade_step;
    else
        fader_opacity-=fade_step;

    if(fader_opacity>1.0f) fader_opacity = 1.0f;
    else
        if(fader_opacity<0.0f) fader_opacity = 0.0f;
}
/**************************Fader**end**************************/

/********************************Texture************************************/
void PGE_BoxBase::loadTexture(QString path)
{
    if(path.isEmpty()) return;

    if(_textureUsed)
    {   //remove old texture
        GlRenderer::deleteTexture( styleTexture );
    }
    GlRenderer::loadTextureP(styleTexture, path);
    if(styleTexture.texture>0)
        _textureUsed=true;
}

void PGE_BoxBase::updateTickValue()
{
    uTickf = 1000.0f/(float)PGE_Window::PhysStep;
    uTick = round(uTickf);
    if(uTick<=0) uTick=1;
    if(uTickf<=0) uTickf=1.0;
}

void PGE_BoxBase::drawTexture(int left, int top, int right, int bottom, int border)
{
    PGE_Rect x;
    x.setLeft(left);
    x.setTop(top);
    x.setRight(right);
    x.setBottom(bottom);
    drawTexture(x, border);
}

void PGE_BoxBase::drawTexture(PGE_Rect _rect, int border)
{
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

        int pWidth = styleTexture.w-x2;//Width of center piece
        int pHeight = styleTexture.h-y2;//Height of center piece

        int fLnt = 0; // Free Lenght
        int fWdt = 0; // Free Width

        int dX=0; //Draw Offset. This need for crop junk on small sizes
        int dY=0;

        if(w < x2) dX = (x2-w)/2; else dX=0;
        if(h < y2) dY = (y2-h)/2; else dY=0;

        int totalW=((w-x2)/pWidth);
        int totalH=((h-y2)/pHeight);

        //L Draw left border
        if(h > (y2))
        {
            hc=0;
            for(i=0; i<totalH; i++ )
            {
                drawPiece(_rect, PGE_RectF(0, x+hc, x-dX, pHeight), PGE_RectF(0, y, x-dX, pHeight));
                hc+=pHeight;
            }
                fLnt = (h-y2)%pHeight;
                if( fLnt != 0)
                    drawPiece(_rect, PGE_RectF(0, x+hc, x-dX, fLnt), PGE_RectF(0, y, x-dX, fLnt));
        }

        //T Draw top border
        if(w > (x2))
        {
            hc=0;
            for(i=0; i<totalW; i++ )
            {
                drawPiece(_rect, PGE_RectF(x+hc, 0, pWidth, y-dY), PGE_RectF(x, 0, pWidth, y-dY));
                    hc+=pWidth;
            }
                fLnt = (w-(x2))%pWidth;
                if( fLnt != 0)
                    drawPiece(_rect, PGE_RectF(x+hc, 0, fLnt, y-dY), PGE_RectF(x, 0, fLnt, y-dY));
        }

        //B Draw bottom border
        if(w > (x2))
        {
            hc=0;
            for(i=0; i< totalW; i++)
            {
                drawPiece(_rect, PGE_RectF(x+hc, h-y+dY, pWidth, y-dY), PGE_RectF(x, styleTexture.h-y+dY, pWidth, y-dY));
                    hc+=pWidth;
            }
            fLnt = (w-x2)%pWidth;
            if( fLnt != 0)
                drawPiece(_rect, PGE_RectF(x+hc, h-y+dY, fLnt, y-dY), PGE_RectF(x, styleTexture.h-y+dY, fLnt, y-dY));
        }

        //R Draw right border
        if(h > (y2))
        {
            hc=0;
            for(i=0; i< totalH; i++ )
            {
                drawPiece(_rect, PGE_RectF(w-x+dX, y+hc, x-dX, pHeight), PGE_RectF(styleTexture.w-x+dX, y, x-dX, pHeight));
                    hc+=pHeight;
            }
                fLnt = (h-y2)%pHeight;
                if( fLnt != 0 )
                    drawPiece(_rect, PGE_RectF(w-x+dX, y+hc, x-dX, fLnt), PGE_RectF(styleTexture.w-x+dX, y, x-dX, fLnt));
        }


        //C Draw center
        if( (w > (x2)) && (h > (y2)))
        {
            hc=0;
            wc=0;
            for(i=0; i< totalH; i++ )
            {
                hc=0;
                for(j=0; j< totalW; j++ )
                {
                    drawPiece(_rect, PGE_RectF(x+hc, y+wc, pWidth, pHeight), PGE_RectF(x, y, pWidth, pHeight));
                    hc+=pWidth;
                }
                    fLnt = (w-x2)%pWidth;
                    if(fLnt != 0 )
                        drawPiece(_rect, PGE_RectF(x+hc, y+wc, fLnt, pHeight), PGE_RectF(x, y, fLnt, pHeight));
                wc+=pHeight;
            }

            fWdt = (h-y2)%pHeight;
            if(fWdt !=0)
            {
                hc=0;
                for(j=0; j<totalW; j++ )
                {
                    drawPiece(_rect, PGE_RectF(x+hc, y+wc, pWidth, fWdt), PGE_RectF(x, y, pWidth, fWdt));
                    hc+=pWidth;
                }
                    fLnt = (w-x2)%pWidth;
                    if(fLnt != 0 )
                        drawPiece(_rect, PGE_RectF(x+hc, y+wc, fLnt, fWdt), PGE_RectF(x, y, fLnt, fWdt));
            }

        }

        //Draw corners
         //1 Left-top
        drawPiece(_rect, PGE_RectF(0,0,x-dX,y-dY), PGE_RectF(0,0,x-dX, y-dY));
         //2 Right-top
        drawPiece(_rect, PGE_RectF(w-x+dX, 0, x-dX, y-dY), PGE_RectF(styleTexture.w-x+dX, 0, x-dX, y-dY));
         //3 Right-bottom
        drawPiece(_rect, PGE_RectF(w-x+dX, h-y+dY, x-dX, y-dY), PGE_RectF(styleTexture.w-x+dX, styleTexture.h-y+dY, x-dX, y-dY));
         //4 Left-bottom
        drawPiece(_rect, PGE_RectF(0, h-y+dY, x-dX, y-dY), PGE_RectF(0, styleTexture.h-y+dY, x-dX, y-dY));
    }
}

void PGE_BoxBase::drawPiece(PGE_RectF target, PGE_RectF block, PGE_RectF texture)
{
    PGE_RectF tx;
    tx.setLeft(texture.left()/this->styleTexture.w);
    tx.setRight(texture.right()/this->styleTexture.w);
    tx.setTop(texture.top()/this->styleTexture.h);
    tx.setBottom(texture.bottom()/this->styleTexture.h);

    PGE_RectF blockG;
    blockG.setTopLeft( GlRenderer::MapToGl(target.x()+block.x(), target.y()+block.y()) );
    blockG.setBottomRight( GlRenderer::MapToGl(target.x()+block.x()+block.width(), target.y()+block.y()+block.height() ) );

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
