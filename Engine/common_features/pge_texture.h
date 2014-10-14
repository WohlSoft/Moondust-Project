#ifndef PGE_TEXTURE_H
#define PGE_TEXTURE_H

#include <SDL2/SDL_opengl.h>
#undef main

struct PGE_Texture
{
    GLuint texture; // Texture object handle
    int w; //Width of the texture.
    int h; //Height of the texture.
    GLubyte *texture_layout;
    GLenum format;
    GLint  nOfColors;
};


#endif // PGE_TEXTURE_H
