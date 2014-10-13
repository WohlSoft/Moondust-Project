#include <SDL2/SDL.h> // SDL 2 Library
#include <SDL2/SDL_opengl.h>
#include <QtDebug>
#include "graphics.h"
#include "window.h"

QPointF mapToOpengl(QPoint s)
{
    qreal nx  =  s.x() - qreal(PGE_Window::Width)  /  2;
    qreal ny  =  s.y() - qreal(PGE_Window::Height)  /  2;
    //qreal( qreal(PGE_Window::Height) - qreal(s.y())  -  1)  /  qreal(PGE_Window::Height  /  2  -  1;
    return QPointF(nx, ny);
}


SDL_Surface *load_image( std::string filename )
{
    //Temporary storage for the image that's loaded
    SDL_Surface* loadedImage = NULL;

    //The optimized image that will be used
    SDL_Surface* optimizedImage = NULL;

    //Load the image
    loadedImage = SDL_LoadBMP( filename.c_str() );

    //If nothing went wrong in loading the image
    if( loadedImage != NULL )
    {
        //Create an optimized image
        optimizedImage = SDL_ConvertSurfaceFormat( loadedImage, SDL_PIXELFORMAT_ARGB8888, 0 );

        //Free the old image
        SDL_FreeSurface( loadedImage );
    }

    //Return the optimized image
    return optimizedImage;
}





PGE_Texture loadTexture(std::string path)
{
    PGE_Texture target;

    SDL_Surface *surface;
    // Load the OpenGL texture
    surface = load_image(path); // Gives us the information to make the texture

    if ( surface )
    {

        // Check that the image's width is a power of 2
        if ( (surface->w & (surface->w - 1)) != 0 ) {
            //printf("warning: image.bmp's width is not a power of 2\n");
        }

        // Also check if the height is a power of 2
        if ( (surface->h & (surface->h - 1)) != 0 )
        {
            //printf("warning: image.bmp's height is not a power of 2\n");
        }

            // get the number of channels in the SDL surface
            target.nOfColors = surface->format->BytesPerPixel;
            if (target.nOfColors == 4)     // contains an alpha channel
            {
                    if (surface->format->Rmask == 0x000000ff)
                            target.format = GL_RGBA;
                    else
                            target.format = GL_BGRA;
            } else if (target.nOfColors == 3)     // no alpha channel
            {
                    if (surface->format->Rmask == 0x000000ff)
                            target.format = GL_RGB;
                    else
                            target.format = GL_BGR;
            } else {
                    printf("warning: the image is not truecolor..  this will probably break\n");
                    // this error should not go unhandled
            }

        glEnable(GL_TEXTURE_2D);
        // Have OpenGL generate a texture object handle for us
        glGenTextures( 1, &target.texture );

        // Bind the texture object
        glBindTexture( GL_TEXTURE_2D, target.texture );

        // Edit the texture object's image data using the information SDL_Surface gives us
        target.w = surface->w;
        target.h = surface->h;
        // Set the texture's stretching properties

        // Bind the texture object
        glBindTexture( GL_TEXTURE_2D, target.texture );

        glTexImage2D(GL_TEXTURE_2D, 0, target.nOfColors, surface->w, surface->h,
                        0, target.format, GL_UNSIGNED_BYTE, surface->pixels);

        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );

        glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

        glDisable(GL_TEXTURE_2D);

        qDebug() << "width " << target.w << " height " << target.h;
    }
    else
    {
        printf("SDL could not load image.bmp: %s\n", SDL_GetError());
        return target;
    }

    if ( surface )
    {
        SDL_FreeSurface( surface );
    }

    return target;
}
