/*
 * Moondust, a free game engine for platform game making
 * Copyright (c) 2014-2025 Vitaly Novichkov <admin@wohlnet.ru>
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

#ifndef RENDER_SW_SDL_H
#define RENDER_SW_SDL_H

#include <vector>
#include <stack>
#include <common_features/rect.h>
#include "render_base.h"

#include <SDL2/SDL_rect.h>

struct SDL_Surface;
struct SDL_Renderer;
struct SDL_Texture;

class Render_SW_SDL : public Render_Base
{
public:
    Render_SW_SDL();
    ~Render_SW_SDL() override = default;
    void set_SDL_settings() override;
    unsigned int SDL_InitFlags() override
    {
        return 0;
    }
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

    SDL_Rect   scaledRectIS(float x, float y, int w, int h);
    SDL_Rect   scaledRect(float x, float y, float w, float h);
    SDL_Rect   scaledRectS(float left, float top, float right, float bottom);
    PGE_Point  MapToScr(PGE_Point point) override;
    PGE_Point  MapToScr(int x, int y) override;
    int  alignToCenterW(int x, int w) override;
    int  alignToCenterH(int y, int h) override;
private:
    PGE_Texture _dummyTexture;

    //!The window renderer
    SDL_Renderer *m_gRenderer;

    unsigned char m_clearColor[4];

    SDL_Texture *m_currentTexture;
    PGE_Rect     m_currentTextureRect;

    //! Internal bank of textures
    std::vector<SDL_Texture *> m_textureBank;
    //! Collector of free numbers at middle
    std::stack<GLuint>        m_textureFreeNumbers;

    inline void setRenderTexture(GLuint &tID)
    {
        m_currentTexture = (tID >= m_textureBank.size()) ? nullptr : m_textureBank[tID];
    }

    inline void setUnbindTexture()
    {
        m_currentTexture = nullptr;
    }

    inline void setRenderColors()
    {
        setUnbindTexture();
    }

    //Virtual resolution of renderable zone
    float window_w;
    float window_h;

    //Scale of virtual and window resolutuins
    float scale_x;
    float scale_y;
    //Side offsets to keep ratio
    float offset_x;
    float offset_y;
    //current viewport
    float viewport_x;
    float viewport_y;
    //Need to calculate relative viewport position when screen was scaled
    float viewport_scale_x;
    float viewport_scale_y;
    //Resolution of viewport
    float viewport_w;
    float viewport_h;
    //Half values of viewport Resolution
    float viewport_w_half;
    float viewport_h_half;

    //Texture render color levels
    float color_binded_texture[4];
};

#endif // RENDER_SW_SDL_H
