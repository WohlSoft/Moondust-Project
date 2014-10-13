#ifndef GRAPHICS_H
#define GRAPHICS_H

#include <QPoint>
#include <QPointF>
#include <QString>

#include <SDL2/SDL.h> // SDL 2 Library
#include <SDL2/SDL_opengl.h>

#undef main

#include "../common_features/pge_texture.h"

class LVL_Background
{
public:
    LVL_Background();
    ~LVL_Background();
    enum type
    {
        single_row=0,
        double_row,
        tiled,
        multi_layered
    };

    std::vector<PGE_Texture* > bg_textures;
};


//PGE_Texture loadTexture(std::string path);
//SDL_Surface *load_image( std::string filename );

QPointF mapToOpengl(QPoint s);


#endif // GRAPHICS_H
