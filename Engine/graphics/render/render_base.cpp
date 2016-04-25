#include "render_base.h"

Render_Base::Render_Base() :
    m_renderer_name("Unknown")
{}

Render_Base::Render_Base(QString renderer_name) :
    m_renderer_name(renderer_name)
{}

void Render_Base::set_SDL_settings()
{}

unsigned int Render_Base::SDL_InitFlags()
{
    return 0;
}

bool Render_Base::init()
{
    return false;
}

bool Render_Base::uninit()
{
    return false;
}

void Render_Base::initDummyTexture()
{}

PGE_Texture Render_Base::getDummyTexture()
{
    return PGE_Texture();
}

void Render_Base::loadTexture(PGE_Texture &/*target*/, int /*width*/, int /*height*/, unsigned char */*RGBApixels*/)
{}

void Render_Base::deleteTexture(PGE_Texture &/*tx*/)
{}

void Render_Base::deleteTexture(GLuint /*tx*/)
{}

void Render_Base::getScreenPixels(int /*x*/, int /*y*/, int /*w*/, int /*h*/, unsigned char */*pixels*/)
{}

void Render_Base::setViewport(int /*x*/, int /*y*/, int /*w*/, int /*h*/)
{}

void Render_Base::resetViewport()
{}

void Render_Base::setViewportSize(int /*w*/, int /*h*/)
{}

void Render_Base::setWindowSize(int /*w*/, int /*h*/)
{}

void Render_Base::flush()
{}

void Render_Base::repaint()
{}

void Render_Base::setClearColor(float /*r*/, float /*g*/, float /*b*/, float /*a*/)
{}

void Render_Base::clearScreen()
{}

void Render_Base::renderRect(float /*x*/, float /*y*/, float /*w*/, float /*h*/, GLfloat /*red*/, GLfloat /*green*/, GLfloat /*blue*/, GLfloat /*alpha*/, bool /*filled*/)
{}

void Render_Base::renderRectBR(float /*_left*/, float /*_top*/, float /*_right*/, float /*_bottom*/, GLfloat /*red*/, GLfloat /*green*/, GLfloat /*blue*/, GLfloat /*alpha*/)
{}

void Render_Base::renderTexture(PGE_Texture */*texture*/, float /*x*/, float /*y*/)
{}

void Render_Base::renderTexture(PGE_Texture */*texture*/, float /*x*/,
                                float /*y*/,
                                float /*w*/,
                                float /*h*/,
                                float /*ani_top*/,
                                float /*ani_bottom*/,
                                float /*ani_left*/,
                                float /*ani_right*/)
{}

void Render_Base::renderTextureCur(float /*x*/, float /*y*/, float /*w*/, float /*h*/, float /*ani_top*/, float /*ani_bottom*/, float /*ani_left*/, float /*ani_right*/)
{}

void Render_Base::BindTexture(PGE_Texture */*texture*/)
{}

void Render_Base::setTextureColor(float /*Red*/, float /*Green*/, float /*Blue*/, float /*Alpha*/)
{}

void Render_Base::UnBindTexture()
{}

PGE_Point Render_Base::MapToScr(PGE_Point point)
{
    return point;
}

PGE_Point Render_Base::MapToScr(int x, int y)
{
    return PGE_Point(x, y);
}

int Render_Base::alignToCenter(int x, int /*w*/)
{
    return x;
}

const QString &Render_Base::name()
{
    return m_renderer_name;
}
