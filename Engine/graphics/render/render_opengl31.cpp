#include "render_opengl31.h"

#include "../window.h"
#include <common_features/graphics_funcs.h>

#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h> // SDL 2 Library
#include <SDL2/SDL_opengl.h>
#include <SDL2/SDL_thread.h>

#include "../gl_debug.h"

Render_OpenGL31::Render_OpenGL31() : Render_Base("OpenGL 3.1"),
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
{}

void Render_OpenGL31::set_SDL_settings()
{
    // Enabling double buffer, setting up colors...
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_RED_SIZE,            8);
    SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE,          8);
    SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE,           8);
    SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE,          8);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);//3
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);//1
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);  //for GL 3.1
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);

    //SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_COMPATIBILITY);//FOR GL 2.1
    //SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG);

    //  SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE,          16);
    //  SDL_GL_SetAttribute(SDL_GL_BUFFER_SIZE,         32);
    //  SDL_GL_SetAttribute(SDL_GL_ACCUM_RED_SIZE,      0);
    //  SDL_GL_SetAttribute(SDL_GL_ACCUM_GREEN_SIZE,    0);
    //  SDL_GL_SetAttribute(SDL_GL_ACCUM_BLUE_SIZE,     0);
    //  SDL_GL_SetAttribute(SDL_GL_ACCUM_ALPHA_SIZE,    0);
    //  SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS,  1);
    //  SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES,  2);
    //  SDL_GL_SetAttribute(SDL_GL_RETAINED_BACKING, 0);
        //SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);
}

bool Render_OpenGL31::init()
{
    glViewport( 0.f, 0.f, PGE_Window::Width, PGE_Window::Height ); GLERRORCHECK();

    //Initialize clear color
    glClearColor( 0.f, 0.f, 0.f, 1.f ); GLERRORCHECK();
    glDisable( GL_DEPTH_TEST ); GLERRORCHECK();
    glDepthFunc(GL_NEVER); GLERRORCHECK(); //Ignore depth values (Z) to cause drawing bottom to top
    glEnable(GL_BLEND); GLERRORCHECK();
    glEnable(GL_TEXTURE_2D); GLERRORCHECK();
    return true;
}

bool Render_OpenGL31::uninit()
{
    glDeleteTextures( 1, &(_dummyTexture.texture) );
    return true;
}

void Render_OpenGL31::initDummyTexture()
{
    QImage image = GraphicsHelps::convertToGLFormat(QImage("://images/_broken.png")).mirrored(false, true);

    int w = image.width();//FreeImage_GetWidth(sourceImage);
    int h = image.height();//FreeImage_GetHeight(sourceImage);

    _dummyTexture.nOfColors = GL_RGBA;
    _dummyTexture.format = GL_BGRA;
    _dummyTexture.w = w;
    _dummyTexture.h = h;
    GLubyte* textura = (GLubyte*)image.bits();//FreeImage_GetBits(sourceImage);

    glGenTextures( 1, &(_dummyTexture.texture) ); GLERRORCHECK();
    glBindTexture( GL_TEXTURE_2D, _dummyTexture.texture ); GLERRORCHECK();
    glTexImage2D( GL_TEXTURE_2D, 0, _dummyTexture.nOfColors, w, h, 0, _dummyTexture.format, GL_UNSIGNED_BYTE, textura ); GLERRORCHECK();
    glBindTexture( GL_TEXTURE_2D, 0); GLERRORCHECK();
    _dummyTexture.inited = true;
}

PGE_Texture Render_OpenGL31::getDummyTexture()
{
    return _dummyTexture;
}

void Render_OpenGL31::loadTexture(PGE_Texture &target, int width, int height, unsigned char *RGBApixels)
{
    // Have OpenGL generate a texture object handle for us
    glGenTextures( 1, &(target.texture) ); GLERRORCHECK();
    // Bind the texture object
    glBindTexture( GL_TEXTURE_2D, target.texture ); GLERRORCHECK();
    glTexImage2D(GL_TEXTURE_2D, 0, target.nOfColors, width, height,
           0, target.format, GL_UNSIGNED_BYTE, (GLubyte*)RGBApixels); GLERRORCHECK();
    glBindTexture( GL_TEXTURE_2D, 0); GLERRORCHECK();
    target.inited = true;
}

void Render_OpenGL31::deleteTexture(PGE_Texture &tx)
{
    glDeleteTextures( 1, &(tx.texture) );
}

void Render_OpenGL31::deleteTexture(GLuint tx)
{
    glDeleteTextures( 1, &tx );
}

void Render_OpenGL31::getScreenPixels(int x, int y, int w, int h, unsigned char *pixels)
{
    glReadPixels(x, y, w, h, GL_BGR, GL_UNSIGNED_BYTE, pixels);
}

void Render_OpenGL31::setViewport(int x, int y, int w, int h)
{
    glViewport(offset_x+x*viewport_scale_x,
               offset_y+(window_h-(y+h))*viewport_scale_y,
               w*viewport_scale_x, h*viewport_scale_y);  GLERRORCHECK();
    viewport_x=x;
    viewport_y=y;
    setViewportSize(w, h);
}

void Render_OpenGL31::resetViewport()
{
    float w, w1, h, h1;
    int   wi, hi;
    SDL_GetWindowSize(PGE_Window::window, &wi, &hi);
    w=wi;h=hi; w1=w;h1=h;
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
    glViewport(offset_x, offset_y, (GLsizei)w1, (GLsizei)h1); GLERRORCHECK();
    setViewportSize(window_w, window_h);
}

void Render_OpenGL31::setViewportSize(int w, int h)
{
    viewport_w=w;
    viewport_h=h;
    viewport_w_half=w/2;
    viewport_h_half=h/2;
}

void Render_OpenGL31::setWindowSize(int w, int h)
{
    window_w=w;
    window_h=h;
    resetViewport();
}



static inline void setRenderColors()
{
    glBindTexture( GL_TEXTURE_2D, 0 );  GLERRORCHECK();
    glEnableClientState(GL_VERTEX_ARRAY); GLERRORCHECK();
    glEnableClientState(GL_COLOR_ARRAY); GLERRORCHECK();
    glDisableClientState(GL_TEXTURE_COORD_ARRAY); GLERRORCHECK();
}

static inline void setRenderTexture(GLuint &tID)
{
    glBindTexture( GL_TEXTURE_2D, tID ); GLERRORCHECK();
    glEnableClientState(GL_VERTEX_ARRAY); GLERRORCHECK();
    glEnableClientState(GL_COLOR_ARRAY); GLERRORCHECK();
    glEnableClientState(GL_TEXTURE_COORD_ARRAY); GLERRORCHECK();

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);  GLERRORCHECK();
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);  GLERRORCHECK();
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST); GLERRORCHECK();
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST); GLERRORCHECK();
}

static inline void setUnbindTexture()
{
    glBindTexture( GL_TEXTURE_2D, 0 ); GLERRORCHECK();
}

static inline void setAlphaBlending()
{
    #ifdef GL_GLEXT_PROTOTYPES
    glBlendEquation(GL_FUNC_ADD); GLERRORCHECK();
    #endif
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); GLERRORCHECK();
}


void Render_OpenGL31::renderRect(float x, float y, float w, float h, GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha, bool filled)
{
    PGE_PointF point;
        point = MapToGl(x, y);
    float left = point.x();
    float top = point.y();
        point = MapToGl(x+w, y+h);
    float right = point.x();
    float bottom = point.y();

    setRenderColors();
    setAlphaBlending();

    GLfloat Vertices[] = {
        left, top, 0,
        right, top, 0,
        right, bottom, 0,
        left, bottom, 0
    };
    GLfloat Colors[] = { red, green, blue, alpha,
                         red, green, blue, alpha,
                         red, green, blue, alpha,
                         red, green, blue, alpha };

    glVertexPointer(3, GL_FLOAT, 0, Vertices); GLERRORCHECK();
    glColorPointer(4, GL_FLOAT, 0, Colors); GLERRORCHECK();
    if(filled)
    {
        GLubyte indices[] = {
            0, 1, 2, // (bottom left - top left - top right)
            0, 2, 3  // (bottom left - top right - bottom right)
        };
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_BYTE, indices); GLERRORCHECK();
    } else {
        glDrawArrays(GL_LINE_LOOP, 0, 4); GLERRORCHECK();
    }
}

void Render_OpenGL31::renderRectBR(float _left, float _top, float _right, float _bottom, GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha)
{
    PGE_PointF point;
        point = MapToGl(_left, _top);
    float left = point.x();
    float top = point.y();
        point = MapToGl(_right, _bottom);
    float right = point.x();
    float bottom = point.y();

    setRenderColors();
    setAlphaBlending();

    GLfloat Vertices[] = {
        left, top, 0,
        right, top, 0,
        right, bottom, 0,
        left, bottom, 0
    };

    GLfloat Colors[] = { red, green, blue, alpha,
                         red, green, blue, alpha,
                         red, green, blue, alpha,
                         red, green, blue, alpha };

    GLubyte indices[] = {
        0, 1, 2, // (bottom left - top left - top right)
        0, 2, 3  // (bottom left - top right - bottom right)
    };
    glVertexPointer(3, GL_FLOAT, 0, Vertices); GLERRORCHECK();
    glColorPointer(4, GL_FLOAT, 0, Colors); GLERRORCHECK();
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_BYTE, indices); GLERRORCHECK();
}

void Render_OpenGL31::renderTexture(PGE_Texture *texture, float x, float y)
{
    if(!texture) return;

    PGE_PointF point;
        point = MapToGl(x, y);
    float left = point.x();
    float top = point.y();
        point = MapToGl(x+texture->w, y+texture->h);
    float right = point.x();
    float bottom = point.y();

    setRenderTexture( texture->texture );
    setAlphaBlending();

    glColor4f( color_level_red, color_level_green, color_level_blue, color_level_alpha);
    GLfloat Vertices[] = {
        left, top, 0,
        right, top, 0,
        right, bottom, 0,
        left, bottom, 0
    };
    GLfloat TexCoord[] = {
        0.0f, 0.0f,
        1.0f, 0.0f,
        1.0f, 1.0f,
        0.0f, 1.0f
    };
    GLubyte indices[] = {
        0, 1, 2, // (bottom left - top left - top right)
        0, 2, 3  // (bottom left - top right - bottom right)
    };

    GLfloat Colors[] = { 1.0f, 1.0f, 1.0f, 1.0f,
                         1.0f, 1.0f, 1.0f, 1.0f,
                         1.0f, 1.0f, 1.0f, 1.0f,
                         1.0f, 1.0f, 1.0f, 1.0f };
    glColorPointer(4, GL_FLOAT, 0, Colors); GLERRORCHECK();
    glVertexPointer(3, GL_FLOAT, 0, Vertices); GLERRORCHECK();
    glTexCoordPointer(2, GL_FLOAT, 0, TexCoord); GLERRORCHECK();
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_BYTE, indices); GLERRORCHECK();

    setUnbindTexture();
}

void Render_OpenGL31::renderTexture(PGE_Texture *texture, float x, float y, float w, float h, float ani_top, float ani_bottom, float ani_left, float ani_right)
{
    if(!texture) return;
    PGE_PointF point;
        point = MapToGl(x, y);
    float left = point.x();
    float top = point.y();
        point = MapToGl(x+w, y+h);
    float right = point.x();
    float bottom = point.y();

    setRenderTexture( texture->texture );
    setAlphaBlending();

    glColor4f( color_level_red, color_level_green, color_level_blue, color_level_alpha);
    GLfloat Vertices[] = {
        left, top, 0,
        right, top, 0,
        right, bottom, 0,
        left, bottom, 0
    };
    GLfloat TexCoord[] = {
        ani_left, ani_top,
        ani_right, ani_top,
        ani_right, ani_bottom,
        ani_left, ani_bottom
    };
    GLubyte indices[] = {
        0, 1, 2, // (bottom left - top left - top right)
        0, 2, 3  // (bottom left - top right - bottom right)
    };

    GLfloat Colors[] = { color_level_red, color_level_green, color_level_blue, color_level_alpha,
                         color_level_red, color_level_green, color_level_blue, color_level_alpha,
                         color_level_red, color_level_green, color_level_blue, color_level_alpha,
                         color_level_red, color_level_green, color_level_blue, color_level_alpha };

    glColorPointer(4, GL_FLOAT, 0, Colors); GLERRORCHECK();
    glVertexPointer(3, GL_FLOAT, 0, Vertices); GLERRORCHECK();
    glTexCoordPointer(2, GL_FLOAT, 0, TexCoord); GLERRORCHECK();
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_BYTE, indices); GLERRORCHECK();

    setUnbindTexture();
}

void Render_OpenGL31::BindTexture(PGE_Texture *texture)
{
    setRenderTexture( texture->texture );
    setAlphaBlending();
}

void Render_OpenGL31::BindTexture(GLuint &texture_id)
{
    setRenderTexture( texture_id );
    setAlphaBlending();
}

void Render_OpenGL31::setRGB(float Red, float Green, float Blue, float Alpha)
{
    color_level_red=Red;
    color_level_green=Green;
    color_level_blue=Blue;
    color_level_alpha=Alpha;
}

void Render_OpenGL31::resetRGB()
{
    color_level_red=1.f;
    color_level_green=1.f;
    color_level_blue=1.f;
    color_level_alpha=1.f;
}

void Render_OpenGL31::setTextureColor(float Red, float Green, float Blue, float Alpha)
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

void Render_OpenGL31::renderTextureCur(float x, float y, float w, float h, float ani_top, float ani_bottom, float ani_left, float ani_right)
{
    PGE_PointF point;
        point = MapToGl(x, y);
    float left = point.x();
    float top = point.y();
        point = MapToGl(x+w, y+h);
    float right = point.x();
    float bottom = point.y();

    GLfloat Vertices[] = {
        left, top, 0,
        right, top, 0,
        right, bottom, 0,
        left, bottom, 0
    };

    GLfloat TexCoord[] = {
        ani_left, ani_top,
        ani_right, ani_top,
        ani_right, ani_bottom,
        ani_left, ani_bottom
    };

    GLubyte indices[] = {
        0, 1, 2, // (bottom left - top left - top right)
        0, 2, 3  // (bottom left - top right - bottom right)
    };

    glColorPointer(4, GL_FLOAT, 0, color_binded_texture); GLERRORCHECK();
    glVertexPointer(3, GL_FLOAT, 0, Vertices); GLERRORCHECK();
    glTexCoordPointer(2, GL_FLOAT, 0, TexCoord); GLERRORCHECK();
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_BYTE, indices); GLERRORCHECK();
}

void Render_OpenGL31::renderTextureCur(float x, float y)
{
    GLint w;
    GLint h;
    glGetTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_WIDTH, &w); GLERRORCHECK();
    glGetTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_HEIGHT,&h); GLERRORCHECK();
    if(w<0) return;
    if(h<0) return;

    PGE_PointF point;
        point = MapToGl(x, y);
    float left = point.x();
    float top = point.y();
        point = MapToGl(x+w, y+h);
    float right = point.x();
    float bottom = point.y();

    GLfloat Vertices[] = {
        left, top, 0,
        right, top, 0,
        right, bottom, 0,
        left, bottom, 0
    };
    GLfloat TexCoord[] = {
        0.f, 0.f,
        1.f, 0.f,
        1.f, 1.f,
        0.f, 1.f
    };
    GLubyte indices[] = {
        0, 1, 2, // (bottom left - top left - top right)
        0, 2, 3  // (bottom left - top right - bottom right)
    };

    glColorPointer(4, GL_FLOAT, 0, color_binded_texture); GLERRORCHECK();
    glVertexPointer(3, GL_FLOAT, 0, Vertices); GLERRORCHECK();
    glTexCoordPointer(2, GL_FLOAT, 0, TexCoord); GLERRORCHECK();
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_BYTE, indices); GLERRORCHECK();
}

void Render_OpenGL31::getCurWidth(GLint &w)
{
    glGetTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_WIDTH, &w); GLERRORCHECK();
}

void Render_OpenGL31::getCurHeight(GLint &h)
{
    glGetTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_HEIGHT, &h); GLERRORCHECK();
}

void Render_OpenGL31::UnBindTexture()
{
    setUnbindTexture();
}

PGE_PointF Render_OpenGL31::MapToGl(PGE_Point point)
{
    return MapToGl(point.x(), point.y());
}

PGE_PointF Render_OpenGL31::MapToGl(float x, float y)
{
    double nx1 = roundf(x)/(viewport_w_half)-1.0;
    double ny1 = (viewport_h-(roundf(y)))/viewport_h_half-1.0;
    return PGE_PointF(nx1, ny1);
}

PGE_Point Render_OpenGL31::MapToScr(PGE_Point point)
{
    return MapToScr(point.x(), point.y());
}

PGE_Point Render_OpenGL31::MapToScr(int x, int y)
{
    return PGE_Point(((float(x))/viewport_scale_x)-offset_x, ((float(y))/viewport_scale_y)-offset_y);
}

int Render_OpenGL31::alignToCenter(int x, int w)
{
    return x+(viewport_w_half-(w/2));
}

