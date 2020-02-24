/*
 * Moondust, a free game engine for platform game making
 * Copyright (c) 2014-2020 Vitaly Novichkov <admin@wohlnet.ru>
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

#ifndef RENDER_OPENGL21_H
#define RENDER_OPENGL21_H

#include "render_base.h"
#include "render_platform_support.h"
#include <common_features/rectf.h>

#ifdef RENDER_SUPORT_OPENGL2
class Render_OpenGL21 : public Render_Base
{
public:
    Render_OpenGL21();
    ~Render_OpenGL21() override = default;
    void set_SDL_settings() override;
    unsigned int SDL_InitFlags() override;
    bool init() override;
    bool uninit() override;
    void initDummyTexture() override;
    PGE_Texture getDummyTexture() override;
    void loadTexture(PGE_Texture &target, uint32_t width, uint32_t height, uint8_t *RGBApixels) override;
    void deleteTexture(PGE_Texture &tx) override;
    bool isTopDown() override;
    void getScreenPixels(int x, int y, int w, int h, unsigned char *pixels) override;
    void getScreenPixelsRGBA(int x, int y, int w, int h, unsigned char *pixels) override;
    void getPixelData(const PGE_Texture *tx, unsigned char *pixelData) override;
    void setViewport(int x, int y, int w, int h) override;

    void resetViewport() override;
    void setViewportSize(int w, int h) override;
    void setViewportSize(float w, float h);
    void setVirtualSurfaceSize(int w, int h) override;

    void flush() override;
    void repaint() override;

    /*!
     * \brief Changed color to clear screen
     * \param r level of red (from 0.0 to 1.0)
     * \param g level of green (from 0.0 to 1.0)
     * \param b level of blue (from 0.0 to 1.0)
     * \param a level of alpha (from 0.0 to 1.0)
     */
    void setClearColor(float r, float g, float b, float a) override;
    /*!
     * \brief Clear screen with pre-defined clear color
     */
    void clearScreen() override;

    void renderRect(float x, float y, float w, float h, GLfloat red = 1.f, GLfloat green = 1.f, GLfloat blue = 1.f, GLfloat alpha = 1.f, bool filled = true) override;
    void renderRectBR(float _left, float _top, float _right, float _bottom, GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha) override;
    void renderTexture(PGE_Texture *texture, float x, float y) override;
    void renderTexture(PGE_Texture *texture, float x, float y, float w, float h, float ani_top = 0.0f, float ani_bottom = 1.0f, float ani_left = 0.0f, float ani_right = 1.0f) override;
    void renderTextureCur(float x, float y, float w, float h, float ani_top = 0.0f, float ani_bottom = 1.0f, float ani_left = 0.0f, float ani_right = 1.0f) override;

    void BindTexture(PGE_Texture *texture) override;
    void setTextureColor(float Red, float Green, float Blue, float Alpha = 1.0f) override;
    void UnBindTexture() override;

    PGE_RectF MapToGl(float x, float y, float w, float h);
    PGE_RectF MapToGlSI(float left, float top, float right, float bottom);

    PGE_Point MapToScr(PGE_Point point) override;
    PGE_Point MapToScr(int x, int y) override;
    int  alignToCenterW(int x, int w) override;
    int  alignToCenterH(int y, int h) override;
private:
    PGE_Texture _dummyTexture;

    //Virtual resolution of renderable zone
    float window_w = 800.0f;
    float window_h = 600.0f;

    //Scale of virtual and window resolutuins (Real)
    float scale_x = 1.0f;
    float scale_y = 1.0f;
    //Scale of virtual and window resolutions (Renderable)
    float scale_x_draw = 1.0f;
    float scale_y_draw = 1.0f;
    //Side offsets to keep ratio (Real)
    float offset_x = 0.0f;
    float offset_y = 0.0f;
    //Side offsets (Renderable)
    float offset_x_draw = 0.0f;
    float offset_y_draw = 0.0f;
    //current viewport (internal values)
    float viewport_x = 0.0f;
    float viewport_y = 0.0f;
    //Need to calculate relative viewport position when screen has scaled (Real)
    float viewport_scale_x = 1.0f;
    float viewport_scale_y = 1.0f;
    //Need to calculate relative viewport position when screen has scaled (Renderable)
    float viewport_scale_x_draw = 1.0f;
    float viewport_scale_y_draw = 1.0f;
    //Resolution of viewport
    float viewport_w = 800.0f;
    float viewport_h = 600.0f;
    //Half values of viewport Resolution
    float viewport_w_half = 400.0f;
    float viewport_h_half = 300.0f;

    float color_binded_texture[4];
};

#else //RENDER_SUPORT_OPENGL2

class Render_OpenGL21 : public Render_dummy
{
public:
    bool init() override;
    bool uninit() override;
};

#endif //RENDER_SUPORT_OPENGL2

#endif // RENDER_OPENGL21_H
