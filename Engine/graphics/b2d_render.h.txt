#ifndef B2D_RENDER_H
#define B2D_RENDER_H

#include "gl_renderer.h"
#include <Box2D/Box2D.h>
#include <SDL2/SDL_types.h>
#include <scenes/level/lvl_camera.h>

class b2d_render
{
public:
    static void DrawBody(b2Body *body, float posX, float posY, float red=1.0f, float green=0.0f, float blue=0.0f, float alpha=1.0f, float width=1.0, bool filled=false);
};

#endif // B2D_RENDER_H
