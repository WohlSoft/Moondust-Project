/*
 * Moondust, a free game engine for platform game making
 * Copyright (c) 2014-2021 Vitaly Novichkov <admin@wohlnet.ru>
 *
 * This software is licensed under a dual license system (MIT or GPL version 3 or later).
 * This means you are free to choose with which of both licenses (MIT or GPL version 3 or later)
 * you want to use this software.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *
 * You can see text of MIT license in the LICENSE.mit file you can see in Engine folder,
 * or see https://mit-license.org/.
 *
 * You can see text of GPLv3 license in the LICENSE.gpl3 file you can see in Engine folder,
 * or see <http://www.gnu.org/licenses/>.
 */

#ifndef RENDER_BASE_H
#define RENDER_BASE_H

#include <string>
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
    Render_Base(std::string renderer_name);
    virtual ~Render_Base();

    /*!
     * \brief Sets default properties for SDL Window before window creation that required for needed renderer type
     */
    virtual void set_SDL_settings() = 0;

    virtual unsigned int SDL_InitFlags() = 0;

    /*!
     * \brief Initializes renderer
     * \return true if successfully initialized
     */
    virtual bool init() = 0;
    /*!
     * \brief Uninitializes renderer
     * \return
     */
    virtual bool uninit() = 0;
    /*!
     * \brief Initializes a "dummy" texture which will be used as replacement for broken textures
     */
    virtual void initDummyTexture() = 0;

    /**
     * @brief Returns a dummy texture which replaces every broken image by itself
     * @return texture
     */
    virtual PGE_Texture getDummyTexture() = 0;
    /*!
     * \brief Loades a texture and merges with a bit mask image if needed
     * \param target reference to resulted texture
     * \param path source image file path
     * \param maskPath mask image file path
     */
    virtual void loadTexture(PGE_Texture &target, uint32_t width, uint32_t height, uint8_t *RGBApixels) = 0;
    /*!
     * \brief Deletes target texture
     * \param tx texture to delete
     */
    virtual void deleteTexture(PGE_Texture &tx) = 0;
    /**
     * @brief Are pixel arrays top-down directed?
     * @return true if texture is top-down directed
     */
    virtual bool isTopDown() = 0;
    /*!
     * \brief Captures screen surface into 24-bit pixel array
     * \param [__in] x Capture at position x of left side
     * \param [__in] y Capture at position y of top side
     * \param [__in] w Width from left to right of surface to capture
     * \param [__in] h Height from top to bottom of surface to capture
     * \param [__out] pixels
     */
    virtual void getScreenPixels(int x, int y, int w, int h, unsigned char *pixels) = 0;
    /*!
     * \brief Captures screen surface into 32-bit pixel array
     * \param [__in] x Capture at position x of left side
     * \param [__in] y Capture at position y of top side
     * \param [__in] w Width from left to right of surface to capture
     * \param [__in] h Height from top to bottom of surface to capture
     * \param [__out] pixels
     */
    virtual void getScreenPixelsRGBA(int x, int y, int w, int h, unsigned char *pixels) = 0;
    /*!
     * \brief [__in] Returns pixel data from texture into target array
     * \param [__in] tx Pointer to texture
     * \param [__in] bufferSize Size of pixel data array
     * \param [__out] pixelData Output pixel data
     */
    virtual void getPixelData(const PGE_Texture *tx, unsigned char *pixelData) = 0;
    /*!
     * \brief Sets viewport rectangle
     * \param x
     * \param y
     * \param w
     * \param h
     */
    virtual void setViewport(int x, int y, int w, int h) = 0;
    /*!
     * \brief Resets viewport to default position
     */
    virtual void resetViewport() = 0;
    virtual void setViewportSize(int w, int h) = 0;
    virtual void setVirtualSurfaceSize(int w, int h) = 0;

    virtual void flush() = 0;
    virtual void repaint() = 0;

    /*!
     * \brief Changed color to clear screen
     * \param r level of red (from 0.0 to 1.0)
     * \param g level of green (from 0.0 to 1.0)
     * \param b level of blue (from 0.0 to 1.0)
     * \param a level of alpha (from 0.0 to 1.0)
     */
    virtual void setClearColor(float r, float g, float b, float a) = 0;
    /*!
     * \brief Clear screen with pre-defined clear color
     */
    virtual void clearScreen() = 0;

    virtual void renderRect(float x, float y, float w, float h, GLfloat red = 1.f, GLfloat green = 1.f, GLfloat blue = 1.f, GLfloat alpha = 1.f, bool filled = true) = 0;
    virtual void renderRectBR(float _left, float _top, float _right, float _bottom, GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha) = 0;
    virtual void renderTexture(PGE_Texture *texture, float x, float y) = 0;
    virtual void renderTexture(PGE_Texture *texture, float x, float y, float w, float h, float ani_top = 0.0f, float ani_bottom = 1.0f, float ani_left = 0.0f, float ani_right = 1.0f) = 0;
    virtual void renderTextureCur(float x, float y, float w, float h, float ani_top = 0.0f, float ani_bottom = 1.0f, float ani_left = 0.0f, float ani_right = 1.0f) = 0;

    virtual void BindTexture(PGE_Texture *texture) = 0;
    virtual void setTextureColor(float Red, float Green, float Blue, float Alpha = 1.0f) = 0;
    virtual void UnBindTexture() = 0;

    virtual PGE_Point MapToScr(PGE_Point point) = 0;
    virtual PGE_Point MapToScr(int x, int y) = 0;
    virtual int  alignToCenterW(int x, int w) = 0;
    virtual int  alignToCenterH(int y, int h) = 0;
    /*!
     * \brief Get name of renrering device
     * \return name of rendering device
     */
    const std::string &name();
private:
    std::string m_renderer_name;
};


class Render_dummy : public Render_Base
{
public:
    Render_dummy() : Render_Base("Dummy") {}
    ~Render_dummy() {}

    virtual void set_SDL_settings() {}
    virtual unsigned int SDL_InitFlags()
    {
        return 0;
    }
    virtual bool init()
    {
        return false;
    }
    virtual bool uninit()
    {
        return false;
    }
    virtual void initDummyTexture() {}
    virtual PGE_Texture getDummyTexture()
    {
        return PGE_Texture();
    }
    virtual void loadTexture(PGE_Texture &, uint32_t, uint32_t, uint8_t *) {}
    virtual void deleteTexture(PGE_Texture &) {}
    virtual bool isTopDown()
    {
        return false;
    }
    virtual void getScreenPixels(int, int, int, int, unsigned char *) {}
    virtual void getScreenPixelsRGBA(int, int, int, int, unsigned char *) {}
    virtual void getPixelData(const PGE_Texture *, unsigned char *) {}
    virtual void setViewport(int, int, int, int) {}
    virtual void resetViewport() {}
    virtual void setViewportSize(int, int) {}
    virtual void setVirtualSurfaceSize(int, int) {}
    virtual void flush() {}
    virtual void repaint() {}
    virtual void setClearColor(float, float, float, float) {}
    virtual void clearScreen() {}

    virtual void renderRect(float, float, float, float, GLfloat, GLfloat, GLfloat, GLfloat, bool) {}
    virtual void renderRectBR(float, float, float, float, GLfloat, GLfloat, GLfloat, GLfloat) {}
    virtual void renderTexture(PGE_Texture *, float, float) {}
    virtual void renderTexture(PGE_Texture *, float, float, float, float, float, float, float, float) {}
    virtual void renderTextureCur(float, float, float, float, float, float, float, float) {}

    virtual void BindTexture(PGE_Texture *) {}
    virtual void setTextureColor(float, float, float, float) {}
    virtual void UnBindTexture() {}

    virtual PGE_Point MapToScr(PGE_Point)
    {
        return PGE_Point();
    }
    virtual PGE_Point MapToScr(int, int)
    {
        return PGE_Point();
    }
    virtual int  alignToCenterW(int, int)
    {
        return 0;
    }
    virtual int  alignToCenterH(int, int)
    {
        return 0;
    }
};

#endif // RENDER_BASE_H
