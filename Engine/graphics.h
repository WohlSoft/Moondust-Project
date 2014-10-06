#ifndef GRAPHICS_H
#define GRAPHICS_H

#include <QPoint>
#include <QPointF>
#include <QString>
#include <SDL2/SDL.h> // SDL 2 Library
#include <SDL2/SDL_opengl.h>

void initSDL();
void initOpenGL();

extern const int screen_width; // Width of Window
extern const int screen_height; // Height of Window
extern SDL_Window *window;

struct PGE_Texture
{
    GLuint texture; // Texture object handle
    int w; //Width of the texture.
    int h; //Height of the texture.
    GLubyte *texture_layout;
    GLenum format;
    GLint  nOfColors;
};

PGE_Texture loadTexture(std::string path);


QPointF mapToOpengl(QPoint s);
SDL_Surface *load_image( std::string filename );

#endif // GRAPHICS_H
