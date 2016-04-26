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

#ifndef RENDER_OPENGL31_H
#define RENDER_OPENGL31_H

#include "render_base.h"
#include <common_features/rectf.h>

class Render_OpenGL31 : public Render_Base
{
public:
    Render_OpenGL31();
    virtual void set_SDL_settings();
    virtual unsigned int SDL_InitFlags();
    virtual bool init();
    virtual bool uninit();
    virtual void initDummyTexture();
    virtual PGE_Texture getDummyTexture();
    virtual void loadTexture(PGE_Texture &target, int width, int height, unsigned char* RGBApixels);
    virtual void deleteTexture(PGE_Texture &tx);
    virtual void getScreenPixels(int x, int y, int w, int h, unsigned char *pixels);
    virtual void setViewport(int x, int y, int w, int h);

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
    virtual void renderTextureCur(float x, float y, float w, float h, float ani_top=0.0f, float ani_bottom=1.0f, float ani_left=0, float ani_right=1.0f);

    virtual void BindTexture(PGE_Texture *texture);
    virtual void setTextureColor(float Red, float Green, float Blue, float Alpha=1.0f);
    virtual void UnBindTexture();

    PGE_RectF MapToGl(float x, float y, float w, float h);
    PGE_RectF MapToGlSI(float left, float top, float right, float bottom);

    PGE_Point MapToScr(PGE_Point point);
    PGE_Point MapToScr(int x, int y);
    int  alignToCenter(int x, int w);
private:
    PGE_Texture _dummyTexture;

    //Virtual resolution of renderable zone
    int window_w;
    int window_h;

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
    float color_binded_texture[16];
};

#endif // RENDER_OPENGL31_H
