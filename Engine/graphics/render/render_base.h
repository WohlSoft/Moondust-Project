/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2016 Vitaly Novichkov <admin@wohlnet.ru>
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

    virtual unsigned int SDL_InitFlags();

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
     * \brief Captures screen surface into 32-bit pixel array
     * \param [__in] x Capture at position x of left side
     * \param [__in] y Capture at position y of top side
     * \param [__in] w Width from left to right of surface to capture
     * \param [__in] h Height from top to bottom of surface to capture
     * \param [__out] pixels
     */
    virtual void getScreenPixels(int x, int y, int w, int h, unsigned char *pixels);
    /*!
     * \brief [__in] Returns pixel data from texture into target array
     * \param [__in] tx Pointer to texture
     * \param [__in] bufferSize Size of pixel data array
     * \param [__out] pixelData Output pixel data
     */
    virtual void getPixelData(const PGE_Texture *tx, unsigned char* pixelData);
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

    virtual void flush();
    virtual void repaint();

    /*!
     * \brief Changed color to clear screen
     * \param r level of red (from 0.0 to 1.0)
     * \param g level of green (from 0.0 to 1.0)
     * \param b level of blue (from 0.0 to 1.0)
     * \param a level of alpha (from 0.0 to 1.0)
     */
    virtual void setClearColor(float r, float g, float b, float a);
    /*!
     * \brief Clear screen with pre-defined clear color
     */
    virtual void clearScreen();

    virtual void renderRect(float x, float y, float w, float h, GLfloat red=1.f, GLfloat green=1.f, GLfloat blue=1.f, GLfloat alpha=1.f, bool filled=true);
    virtual void renderRectBR(float _left, float _top, float _right, float _bottom, GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha);
    virtual void renderTexture(PGE_Texture *texture, float x, float y);
    virtual void renderTexture(PGE_Texture *texture, float x, float y, float w, float h, float ani_top=0.0f, float ani_bottom=1.0f, float ani_left=0.0f, float ani_right=1.0f);
    virtual void renderTextureCur(float x, float y, float w, float h, float ani_top=0.0f, float ani_bottom=1.0f, float ani_left=0.0f, float ani_right=1.0f);

    virtual void BindTexture(PGE_Texture *texture);
    virtual void setTextureColor(float Red, float Green, float Blue, float Alpha=1.0f);
    virtual void UnBindTexture();

    virtual PGE_Point MapToScr(PGE_Point point);
    virtual PGE_Point MapToScr(int x, int y);
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
