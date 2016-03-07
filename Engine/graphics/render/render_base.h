#ifndef RENDER_BASE_H
#define RENDER_BASE_H

#include <QString>
#include <common_features/pge_texture.h>
#include <common_features/point.h>
#include <common_features/pointf.h>

class QImage;
/*!
 * \brief A base class for various rendering devices
 */
class Render_Base
{
public:
    Render_Base();
    Render_Base(QString renderer_name);

    /*!
     * \brief Sets default properties for SDL Window before window creation that required for needed renderer type
     */
    virtual void set_SDL_settings();

    /*!
     * \brief Initializes renderer
     * \return true if successfully initialized
     */
    virtual bool init();
    /*!
     * \brief Uninitializes renderer
     * \return
     */
    virtual bool uninit();
    /*!
     * \brief Initializes a "dummy" texture which will be used as replacement for broken textures
     */
    virtual void initDummyTexture();

    virtual PGE_Texture getDummyTexture();
    /*!
     * \brief Loades a texture and merges with a bit mask image if needed
     * \param target reference to resulted texture
     * \param path source image file path
     * \param maskPath mask image file path
     */
    virtual void loadTexture(PGE_Texture &target, int width, int height, unsigned char* RGBApixels);
    /*!
     * \brief Deletes target texture
     * \param tx texture to delete
     */
    virtual void deleteTexture(PGE_Texture &tx);
    /*!
     * \brief Delete texture by GL Index
     * \param tx Index of texture to delete
     */
    virtual void deleteTexture(GLuint tx);
    /*!
     * \brief Captures screen surface into 32-bit pixel array
     * \param x Capture at position x of left side
     * \param y Capture at position y of top side
     * \param w Width from left to right of surface to capture
     * \param h Height from top to bottom of surface to capture
     * \param pixels
     */
    virtual void getScreenPixels(int x, int y, int w, int h, unsigned char *pixels);
    /*!
     * \brief Sets viewport rectangle
     * \param x
     * \param y
     * \param w
     * \param h
     */
    virtual void setViewport(int x, int y, int w, int h);
    /*!
     * \brief Resets viewport to default position
     */
    virtual void resetViewport();
    virtual void setViewportSize(int w, int h);
    virtual void setWindowSize(int w, int h);

    virtual void renderRect(float x, float y, float w, float h, GLfloat red=1.f, GLfloat green=1.f, GLfloat blue=1.f, GLfloat alpha=1.f, bool filled=true);
    virtual void renderRectBR(float _left, float _top, float _right, float _bottom, GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha);
    virtual void renderTexture(PGE_Texture *texture, float x, float y);
    virtual void renderTexture(PGE_Texture *texture, float x, float y, float w, float h, float ani_top=0, float ani_bottom=1, float ani_left=0, float ani_right=1);

    virtual void BindTexture(PGE_Texture *texture);
    virtual void BindTexture(GLuint &texture_id);

    virtual void setRGB(float Red, float Green, float Blue, float Alpha=1.0);
    virtual void resetRGB();

    virtual void setTextureColor(float Red, float Green, float Blue, float Alpha=1.0f);
    virtual void renderTextureCur(float x, float y, float w, float h, float ani_top=0, float ani_bottom=1, float ani_left=0, float ani_right=1);
    virtual void renderTextureCur(float x, float y);
    virtual void getCurWidth(GLint &w);
    virtual void getCurHeight(GLint &h);
    virtual void UnBindTexture();

    virtual PGE_PointF MapToGl(PGE_Point point);
    virtual PGE_PointF MapToGl(float x, float y);
    virtual PGE_Point  MapToScr(PGE_Point point);
    virtual PGE_Point  MapToScr(int x, int y);
    virtual int  alignToCenter(int x, int w);
    /*!
     * \brief Get name of renrering device
     * \return name of rendering device
     */
    const QString &name();
private:
    QString m_renderer_name;
};

#endif // RENDER_BASE_H
