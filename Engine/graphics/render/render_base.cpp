#include "render_base.h"

Render_Base::Render_Base()
{}

Render_Base::Render_Base(QString renderer_name) :
    m_renderer_name(renderer_name)
{}

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

void Render_Base::loadTextureP(PGE_Texture &/*target*/, QString /*path*/, QString /*maskPath*/)
{}

GLuint Render_Base::QImage2Texture(QImage */*img*/)
{
    return 0;
}

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

void Render_Base::BindTexture(PGE_Texture */*texture*/)
{}

void Render_Base::BindTexture(GLuint &/*texture_id*/)
{}

void Render_Base::renderTextureCur(float /*x*/, float /*y*/, float /*w*/, float /*h*/, float /*ani_top*/, float /*ani_bottom*/, float /*ani_left*/, float /*ani_right*/)
{}

void Render_Base::renderTextureCur(float /*x*/, float /*y*/)
{}

void Render_Base::getCurWidth(GLint &/*w*/)
{}

void Render_Base::getCurHeight(GLint &/*h*/)
{}

void Render_Base::UnBindTexture()
{}

const QString &Render_Base::name()
{
    return m_renderer_name;
}
