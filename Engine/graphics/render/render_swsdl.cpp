#include "render_swsdl.h"

#include "../window.h"
#include <common_features/graphics_funcs.h>
#include <common_features/logger.h>

#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h> // SDL 2 Library

#include "../gl_debug.h"

#ifdef _WIN32
#define FREEIMAGE_LIB
#endif
#include <FreeImageLite.h>

Render_SW_SDL::Render_SW_SDL() : Render_Base("Software SDL"),
    screenSurface(NULL),
    m_gRenderer(NULL),
    m_clearColor{0,0,0,0},
    m_currentTexture(NULL),
    //Virtual resolution of renderable zone
    window_w(800),
    window_h(600),
    //Scale of virtual and window resolutuins
    scale_x(1.0f),
    scale_y(1.0f),
    //Side offsets to keep ratio
    offset_x(0.0f),
    offset_y(0.0f),
    //current viewport
    viewport_x(0.0f),
    viewport_y(0.0f),
    //Need to calculate relative viewport position when screen was scaled
    viewport_scale_x(1.0f),
    viewport_scale_y(1.0f),
    //Resolution of viewport
    viewport_w(800.0f),
    viewport_h(600.0f),
    //Half values of viewport Resolution
    viewport_w_half(400.0f),
    viewport_h_half(300.0f),
    //Texture render color levels
    color_level_red(1.0f),
    color_level_green(1.0f),
    color_level_blue(1.0f),
    color_level_alpha(1.0f),
    color_binded_texture{1.0f, 1.0f, 1.0f, 1.0f,
                         1.0f, 1.0f, 1.0f, 1.0f,
                         1.0f, 1.0f, 1.0f, 1.0f,
                         1.0f, 1.0f, 1.0f, 1.0f}
{
    m_textureBank.push_back(NULL);
}

void Render_SW_SDL::set_SDL_settings()
{
    SDL_GL_ResetAttributes();
}

bool Render_SW_SDL::init()
{
    //Initialize clear color
    setClearColor( 0.f, 0.f, 0.f, 1.f );
    //Get window surface
    screenSurface = SDL_GetWindowSurface( PGE_Window::window );
    if(!screenSurface)
    {
        LogWarning("SW SDL: Failed to initialize screen surface!");
        return false;
    }

    //Create renderer for window
    m_gRenderer = SDL_CreateRenderer( PGE_Window::window, -1, SDL_RENDERER_SOFTWARE );
    if(!m_gRenderer)
    {
        LogWarning("SW SDL: Failed to initialize screen surface!");
        return false;
    }

    SDL_SetRenderDrawBlendMode(m_gRenderer, SDL_BLENDMODE_BLEND);

    return true;
}

bool Render_SW_SDL::uninit()
{
    deleteTexture( _dummyTexture );
    SDL_DestroyRenderer( m_gRenderer );
    return true;
}

void Render_SW_SDL::initDummyTexture()
{
    FIBITMAP* image = GraphicsHelps::loadImageRC("://images/_broken.png");
    if(!image)
    {
        QMessageBox::warning(nullptr, "OpenGL Error",
                             QString("Can't initialize dummy texture!\n"
                                     "In file: %1:%2").arg(__FILE__).arg(__LINE__));
        abort();
    }
    int w = FreeImage_GetWidth(image);
    int h = FreeImage_GetHeight(image);

    _dummyTexture.nOfColors = GL_RGBA;
    _dummyTexture.format = GL_BGRA;
    _dummyTexture.w = w;
    _dummyTexture.h = h;

    GLubyte* textura = (GLubyte*)FreeImage_GetBits(image);
    loadTexture(_dummyTexture, w, h, textura);
    GraphicsHelps::closeImage(image);
}

PGE_Texture Render_SW_SDL::getDummyTexture()
{
    return _dummyTexture;
}

void Render_SW_SDL::loadTexture(PGE_Texture &target, int width, int height, unsigned char *RGBApixels)
{
    SDL_Surface * surface;
    SDL_Texture * texture;
    surface = SDL_CreateRGBSurfaceFrom(RGBApixels, width, height, 32, width*4,
                                         FI_RGBA_RED_MASK,
                                         FI_RGBA_GREEN_MASK,
                                         FI_RGBA_BLUE_MASK,
                                         FI_RGBA_ALPHA_MASK );
    texture = SDL_CreateTextureFromSurface(m_gRenderer, surface);
    SDL_FreeSurface(surface);
    target.texture = m_textureBank.size();
    //SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);
    m_textureBank.push_back(texture);
    target.inited = true;
}

void Render_SW_SDL::deleteTexture(PGE_Texture &tx)
{
    if( tx.texture >= m_textureBank.size() )
    {
        tx.inited = false;
        return;
    }
    SDL_Texture* corpse = m_textureBank[tx.texture];
    SDL_DestroyTexture( corpse );
    m_textureBank.erase(m_textureBank.begin()+tx.texture);
    tx.texture = 0;
    tx.inited=false;
}

void Render_SW_SDL::deleteTexture(GLuint tx)
{
    SDL_Texture* corpse = m_textureBank[tx];
    SDL_DestroyTexture(corpse);
    m_textureBank.erase(m_textureBank.begin()+tx);
}

void Render_SW_SDL::getScreenPixels(int x, int y, int w, int h, unsigned char *pixels)
{
    SDL_Rect rect;
    rect.x = x;
    rect.y = y;
    rect.w = w;
    rect.h = h;
    SDL_RenderReadPixels( m_gRenderer,
                          &rect,
                          SDL_PIXELFORMAT_BGR24,
                          pixels,
                          w*3 + (w%4) );
}

void Render_SW_SDL::setViewport(int x, int y, int w, int h)
{
//    glViewport(offset_x+x*viewport_scale_x,
//               offset_y+(window_h-(y+h))*viewport_scale_y,
//               w*viewport_scale_x, h*viewport_scale_y);  GLERRORCHECK();
    SDL_Rect topLeftViewport;
    topLeftViewport.x = offset_x + x*viewport_scale_x;
    topLeftViewport.y = offset_y + (window_h-(y+h))*viewport_scale_y;
    topLeftViewport.w = w*viewport_scale_x;
    topLeftViewport.h = h*viewport_scale_y;
    SDL_RenderSetViewport( m_gRenderer, &topLeftViewport );
    viewport_x=x;
    viewport_y=y;
    setViewportSize(w, h);
}

void Render_SW_SDL::resetViewport()
{
    float w, w1, h, h1;
    int   wi, hi;
    SDL_GetWindowSize(PGE_Window::window, &wi, &hi);
    w=wi; h=hi; w1=w; h1=h;
    scale_x=(float)((float)(w)/(float)window_w);
    scale_y=(float)((float)(h)/(float)window_h);
    viewport_scale_x = scale_x;
    viewport_scale_y = scale_y;
    if(scale_x>scale_y)
    {
        w1=scale_y*window_w;
        viewport_scale_x=w1/window_w;
    }
    else if(scale_x<scale_y)
    {
        h1=scale_x*window_h;
        viewport_scale_y=h1/window_h;
    }

    offset_x=(w-w1)/2;
    offset_y=(h-h1)/2;
    //glViewport(offset_x, offset_y, (GLsizei)w1, (GLsizei)h1); GLERRORCHECK();
    SDL_Rect topLeftViewport;
    topLeftViewport.x = offset_x;
    topLeftViewport.y = offset_y;
    topLeftViewport.w = w1;
    topLeftViewport.h = h1;
    SDL_RenderSetViewport( m_gRenderer, &topLeftViewport );
    setViewportSize(window_w, window_h);
}

void Render_SW_SDL::setViewportSize(int w, int h)
{
    viewport_w=w;
    viewport_h=h;
    viewport_w_half=w/2;
    viewport_h_half=h/2;
}

void Render_SW_SDL::setWindowSize(int w, int h)
{
    window_w=w;
    window_h=h;
    resetViewport();
}

void Render_SW_SDL::flush()
{}

void Render_SW_SDL::repaint()
{
    SDL_RenderPresent( m_gRenderer );
}

void Render_SW_SDL::setClearColor(float r, float g, float b, float a)
{
    m_clearColor[0] = (unsigned char)(255.f*r);
    m_clearColor[1] = (unsigned char)(255.f*g);
    m_clearColor[2] = (unsigned char)(255.f*b);
    m_clearColor[3] = (unsigned char)(255.f*a);
}

void Render_SW_SDL::clearScreen()
{
    SDL_SetRenderDrawColor( m_gRenderer, m_clearColor[0],
                                         m_clearColor[1],
                                         m_clearColor[2],
                                         m_clearColor[3]);
    SDL_RenderClear( m_gRenderer );
}

void Render_SW_SDL::renderRect(float x, float y, float w, float h, GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha, bool filled)
{
//    PGE_PointF point;
//        point = MapToGl(x, y);
//    float left = point.x();
//    float top = point.y();
//        point = MapToGl(x+w, y+h);
//    float right = point.x();
//    float bottom = point.y();

//    setRenderColors();
    SDL_Rect aRect = { (int)x, (int)y, (int)w, (int)h };
    SDL_SetRenderDrawColor( m_gRenderer,
                            (unsigned char)(255.f*red),
                            (unsigned char)(255.f*green),
                            (unsigned char)(255.f*blue),
                            (unsigned char)(255.f*alpha)
                            );
    if(filled)
    {
        SDL_RenderFillRect( m_gRenderer, &aRect );
    } else {
        SDL_RenderDrawRect( m_gRenderer, &aRect );
    }
}

void Render_SW_SDL::renderRectBR(float _left, float _top, float _right, float _bottom, GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha)
{
//    PGE_PointF point;
//        point = MapToGl(_left, _top);
//    float left = point.x();
//    float top = point.y();
//        point = MapToGl(_right, _bottom);
//    float right = point.x();
//    float bottom = point.y();
    SDL_Rect aRect = { (int)_left, (int)_top, (int)_right-(int)_left, (int)_bottom-(int)_top };
    SDL_SetRenderDrawColor( m_gRenderer,
                            (unsigned char)(255.f*red),
                            (unsigned char)(255.f*green),
                            (unsigned char)(255.f*blue),
                            (unsigned char)(255.f*alpha)
                            );
    SDL_RenderFillRect( m_gRenderer, &aRect );
}

void Render_SW_SDL::renderTexture(PGE_Texture *texture, float x, float y)
{
    if(!texture) return;
//    PGE_PointF point;
//        point = MapToGl(x, y);
//    float left = point.x();
//    float top = point.y();
//        point = MapToGl(x+texture->w, y+texture->h);
//    float right = point.x();
//    float bottom = point.y();
    setRenderTexture( texture->texture );
    m_currentTextureRect.setRect( 0, 0, texture->w, texture->h );

    if(!m_currentTexture)
    {
        renderRect(x, y, texture->w, texture->h,
                   (unsigned char)(255.f*color_binded_texture[0]),
                   (unsigned char)(255.f*color_binded_texture[1]),
                   (unsigned char)(255.f*color_binded_texture[2]),
                   (unsigned char)(255.f*color_binded_texture[3]) );
    }

    SDL_Rect aRect = { (int)x, (int)y, texture->w, texture->h };
    SDL_SetTextureColorMod( m_currentTexture,
                            (unsigned char)(255.f*color_binded_texture[0]),
                            (unsigned char)(255.f*color_binded_texture[1]),
                            (unsigned char)(255.f*color_binded_texture[2]));
    SDL_SetTextureAlphaMod( m_currentTexture, (unsigned char)(255.f*color_binded_texture[3]));

    SDL_RenderCopy( m_gRenderer, m_currentTexture, NULL, &aRect );
    setUnbindTexture();
}

void Render_SW_SDL::renderTexture(PGE_Texture *texture, float x, float y, float w, float h, float ani_top, float ani_bottom, float ani_left, float ani_right)
{
    if(!texture) return;
//    PGE_PointF point;
//        point = MapToGl(x, y);
//    float left = point.x();
//    float top = point.y();
//        point = MapToGl(x+w, y+h);
//    float right = point.x();
//    float bottom = point.y();

    setRenderTexture( texture->texture );
    m_currentTextureRect.setRect( 0, 0, texture->w, texture->h );

    if(!m_currentTexture)
    {
        renderRect(x, y, w, h,
                   (unsigned char)(255.f*color_binded_texture[0]),
                   (unsigned char)(255.f*color_binded_texture[1]),
                   (unsigned char)(255.f*color_binded_texture[2]),
                   (unsigned char)(255.f*color_binded_texture[3]) );
    }

    SDL_Rect sourceRect = { (int)roundf((float)texture->w*ani_left), (int)roundf((float)texture->h*ani_top),
                            (int)roundf((float)texture->w*ani_right)-(int)roundf((float)texture->w*ani_left),
                            (int)roundf((float)texture->h*ani_bottom)-(int)roundf((float)texture->h*ani_top)
    };
    SDL_Rect destRect = { (int)x, (int)y, (int)w, (int)h };
    SDL_SetTextureColorMod( m_currentTexture,
                            (unsigned char)(255.f*color_binded_texture[0]),
                            (unsigned char)(255.f*color_binded_texture[1]),
                            (unsigned char)(255.f*color_binded_texture[2]));
    SDL_SetTextureAlphaMod( m_currentTexture, (unsigned char)(255.f*color_binded_texture[3]));

    SDL_RenderCopy( m_gRenderer, m_currentTexture, &sourceRect, &destRect );
    setUnbindTexture();
}

void Render_SW_SDL::BindTexture(PGE_Texture *texture)
{
    setRenderTexture( texture->texture );
    m_currentTextureRect.setRect( 0, 0, texture->w, texture->h );
}

void Render_SW_SDL::setRGB(float Red, float Green, float Blue, float Alpha)
{
    color_level_red=Red;
    color_level_green=Green;
    color_level_blue=Blue;
    color_level_alpha=Alpha;
}

void Render_SW_SDL::resetRGB()
{
    color_level_red=1.f;
    color_level_green=1.f;
    color_level_blue=1.f;
    color_level_alpha=1.f;
}

void Render_SW_SDL::setTextureColor(float Red, float Green, float Blue, float Alpha)
{
    color_binded_texture[0]=Red;
    color_binded_texture[1]=Green;
    color_binded_texture[2]=Blue;
    color_binded_texture[3]=Alpha;

    color_binded_texture[4]=Red;
    color_binded_texture[5]=Green;
    color_binded_texture[6]=Blue;
    color_binded_texture[7]=Alpha;

    color_binded_texture[8]=Red;
    color_binded_texture[9]=Green;
    color_binded_texture[10]=Blue;
    color_binded_texture[11]=Alpha;

    color_binded_texture[12]=Red;
    color_binded_texture[13]=Green;
    color_binded_texture[14]=Blue;
    color_binded_texture[15]=Alpha;
}

void Render_SW_SDL::renderTextureCur(float x, float y, float w, float h, float ani_top, float ani_bottom, float ani_left, float ani_right)
{
//    PGE_PointF point;
//        point = MapToGl(x, y);
//    float left = point.x();
//    float top = point.y();
//        point = MapToGl(x+w, y+h);
//    float right = point.x();
//    float bottom = point.y();
    if(!m_currentTexture)
    {
        renderRect(x, y, w, h,
                   (unsigned char)(255.f*color_binded_texture[0]),
                   (unsigned char)(255.f*color_binded_texture[1]),
                   (unsigned char)(255.f*color_binded_texture[2]),
                   (unsigned char)(255.f*color_binded_texture[3]) );
    }
    SDL_Rect sRect = {
        (int)roundf((float)m_currentTextureRect.width()*ani_left),
        (int)roundf((float)m_currentTextureRect.height()*ani_top),
        abs((int)roundf((float)m_currentTextureRect.width()*ani_right)-(int)roundf((float)m_currentTextureRect.width()*ani_left)),
        abs((int)roundf((float)m_currentTextureRect.height()*ani_bottom)-(int)roundf((float)m_currentTextureRect.height()*ani_top))
    };
    SDL_Rect aRect = { (int)x, (int)y, (int)w, (int)h };
    SDL_SetTextureColorMod( m_currentTexture,
                            (unsigned char)(255.f*color_binded_texture[0]),
                            (unsigned char)(255.f*color_binded_texture[1]),
                            (unsigned char)(255.f*color_binded_texture[2]));
    SDL_SetTextureAlphaMod( m_currentTexture, (unsigned char)(255.f*color_binded_texture[3]));

    SDL_RenderCopy( m_gRenderer, m_currentTexture, &sRect, &aRect );
}

void Render_SW_SDL::getCurWidth(GLint &w)
{
    w = 0;
    //glGetTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_WIDTH, &w); GLERRORCHECK();
}

void Render_SW_SDL::getCurHeight(GLint &h)
{
    h = 0;
    //glGetTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_HEIGHT, &h); GLERRORCHECK();
}

void Render_SW_SDL::UnBindTexture()
{
    setUnbindTexture();
}

PGE_PointF Render_SW_SDL::MapToGl(PGE_Point point)
{
    return MapToGl(point.x(), point.y());
}

PGE_PointF Render_SW_SDL::MapToGl(float x, float y)
{
    double nx1 = roundf(x)/(viewport_w_half)-1.0;
    double ny1 = (viewport_h-(roundf(y)))/viewport_h_half-1.0;
    return PGE_PointF(nx1, ny1);
}

PGE_Point Render_SW_SDL::MapToScr(PGE_Point point)
{
    return MapToScr(point.x(), point.y());
}

PGE_Point Render_SW_SDL::MapToScr(int x, int y)
{
    return PGE_Point(((float(x))/viewport_scale_x)-offset_x, ((float(y))/viewport_scale_y)-offset_y);
}

int Render_SW_SDL::alignToCenter(int x, int w)
{
    return x+(viewport_w_half-(w/2));
}

