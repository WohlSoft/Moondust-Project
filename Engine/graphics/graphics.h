#ifndef GRAPHICS_H
#define GRAPHICS_H

#include <QPoint>
#include <QPointF>
#include <QString>

#include <SDL2/SDL.h> // SDL 2 Library
#include <SDL2/SDL_opengl.h>
#undef main

#include "../common_features/pge_texture.h"

QPointF mapToOpengl(QPoint s);


#endif // GRAPHICS_H
