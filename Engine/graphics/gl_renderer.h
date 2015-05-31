/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2015 Vitaly Novichkov <admin@wohlnet.ru>
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

#ifndef GL_RENDERER_H
#define GL_RENDERER_H

#include <QString>
#include <common_features/pge_texture.h>
#include <common_features/rectf.h>
#include <common_features/point.h>
#include <common_features/pointf.h>

struct SDL_Thread;
class GlRenderer
{
public:
    static bool init();
    static bool uninit();
    static PGE_PointF mapToOpengl(PGE_Point s);

    static QString ScreenshotPath;
    static void makeShot();
    static int makeShot_action(void *_pixels);
    static bool ready();
    static void setRGB(float Red, float Green, float Blue, float Alpha=1.0);
    static void resetRGB();

    static void renderTexture(PGE_Texture *texture, float x, float y); //!<Render texture as-is
    static void renderTexture(PGE_Texture *texture, float x, float y, float w, float h, float ani_top=0, float ani_bottom=1, float ani_left=0, float ani_right=1);//!<Render matrix animation fragment
    static void renderTextureCur(float x, float y, float w, float h, float ani_top=0, float ani_bottom=1, float ani_left=0, float ani_right=1);//!< Draw currently binded texture
    static void renderRect(float x, float y, float w, float h, GLfloat red=1.f, GLfloat green=1.f, GLfloat blue=1.f, GLfloat alpha=1.f);
    static void renderRectBR(float _left, float _top, float _right, float _bottom, GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha);

    static PGE_PointF MapToGl(PGE_Point point);
    static PGE_PointF MapToGl(float x, float y);
    static PGE_Point  MapToScr(PGE_Point point);
    static PGE_Point  MapToScr(int x, int y);
    static void setViewport(int x, int y, int w, int h);
    static void resetViewport();
    static void setViewportSize(int w, int h);
    static void setWindowSize(int w, int h);


private:
    //Virtual resolution of renderable zone
    static int window_w;
    static int window_h;

    //Scale of virtual and window resolutuins
    static float scale_x;
    static float scale_y;
    //Side offsets to keep ratio
    static float offset_x;
    static float offset_y;
    //current viewport
    static float viewport_x;
    static float viewport_y;
    //Need to calculate relative viewport position when screen was scaled
    static float viewport_scale_x;
    static float viewport_scale_y;
    //Resolution of viewport
    static float viewport_w;
    static float viewport_h;
    //Half values of viewport Resolution
    static float viewport_w_half;
    static float viewport_h_half;

    //Texture render color levels
    static float color_level_red;
    static float color_level_green;
    static float color_level_blue;
    static float color_level_alpha;

    static SDL_Thread *thread;
    static bool _isReady;
};

#endif // GL_RENDERER_H

