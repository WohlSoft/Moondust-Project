//#include "mainwindow.h"
#include <QCoreApplication>
#include <QElapsedTimer>
#include <QFileInfo>
#include <QDir>
#include <SDL2/SDL.h> // SDL 2 Library
#include <SDL2/SDL_opengl.h>

#undef main

#include <Box2D/Box2D.h>

#include <file_formats.h>

#include <iostream>
using namespace std;

#include <QtDebug>

SDL_Window *window; // Creating window for SDL

const int screen_width = 800; // Width of Window
const int screen_height = 600; // Height of Window

int pos;

int pos_x=199200;
int pos_y=200600;


LevelData level;

void drawQuads();

QPointF mapToOpengl(QPoint s)
{
    qreal nx  =  s.x() - qreal(screen_width)  /  2;
    qreal ny  =  s.y() - qreal(screen_height)  /  2;
    //qreal( qreal(screen_height) - qreal(s.y())  -  1)  /  qreal(screen_height  /  2  -  1;
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



struct keysForTest
{
    bool move_r = false;
    bool move_l = false;
};

keysForTest myKeys;

void resetKeys()
{
    myKeys.move_r = false;
    myKeys.move_l = false;
}

void init()
{

    // Initalizing SDL

    if ( SDL_Init(SDL_INIT_VIDEO) < 0 ){
        cout << "Unable to init SDL, error: " << SDL_GetError() << endl;
        exit(1);
    }

    // Enabling double buffer, setting up colors...

    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    //SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 5);
    //SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 6);
    //SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 5);

    // Creating window with QString title, with size 800x600 and placing to screen center

    window = SDL_CreateWindow(QString("Title - is a QString from Qt, working in the SDL with OpenGL!!!").toStdString().c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                              screen_width, screen_height, SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL);

    SDL_GLContext glcontext = SDL_GL_CreateContext(window); // Creating of the OpenGL Context
    Q_UNUSED(glcontext);

    if(window == NULL){	// If failed to create window - exiting
        exit(1);
    }

    // Initializing OpenGL

    glEnable( GL_TEXTURE_2D ); // Need this to display a texture

    glViewport( 0.f, 0.f, screen_width, screen_height );

    glMatrixMode( GL_PROJECTION );
    glPushMatrix();
    glLoadIdentity();

    glOrtho( 0.0, screen_width, screen_height, 0.0, 1.0, -1.0 );

    //Initialize Modelview Matrix
    glMatrixMode( GL_MODELVIEW );
    glPushMatrix();
    glLoadIdentity();

    //Initialize clear color
    glClearColor( 0.f, 0.f, 0.f, 1.f );

    glEnable( GL_BLEND );
    glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

    //Check for error
    //GLenum error = glGetError();
//    if( error != GL_NO_ERROR )
//    {
//        printf( "Error initializing OpenGL! %s\n", gluErrorString( error ) );
//        //return false;
//    }

    glClearColor(0.0f, 0.0f, 0.0f, 0.0f); // Black background color


    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
}


SDL_Surface *surface;
GLuint texture; // Texture object handle
int texture_height; //Height of the texture.
int texture_width; //Width of the texture.
GLubyte *texture_layout;
GLenum texture_format;
GLint  nOfColors;

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    Q_UNUSED(a);
    //MainWindow w;
    //w.show();

    //a.exec();

    QString fileToPpen = a.applicationDirPath()+"/physics.lvl";

    QFile file(fileToPpen );

    QFileInfo in_1(fileToPpen);

    if (file.open(QIODevice::ReadOnly))
    {
        level = FileFormats::ReadLevelFile(file);
        level.filename = in_1.baseName();
        level.path = in_1.absoluteDir().absolutePath();
        if(level.ReadFileValid)
            qDebug() << "Opened!";
        else
            qDebug() << "Wrong!";

        pos_x = level.sections[0].size_left * -1;
        pos_y = level.sections[0].size_top * -1;

        qDebug() << "blocks "<< level.blocks.size();
    }


    // Load the OpenGL texture
    surface = load_image(a.applicationDirPath().toStdString()+"/block-223.bmp"); // Gives us the information to make the texture


    if ( surface )
    {

        // Check that the image's width is a power of 2
        if ( (surface->w & (surface->w - 1)) != 0 ) {
            printf("warning: image.bmp's width is not a power of 2\n");
        }

        // Also check if the height is a power of 2
        if ( (surface->h & (surface->h - 1)) != 0 )
        {
            printf("warning: image.bmp's height is not a power of 2\n");
        }

            // get the number of channels in the SDL surface
            nOfColors = surface->format->BytesPerPixel;
            if (nOfColors == 4)     // contains an alpha channel
            {
                    if (surface->format->Rmask == 0x000000ff)
                            texture_format = GL_RGBA;
                    else
                            texture_format = GL_BGRA;
            } else if (nOfColors == 3)     // no alpha channel
            {
                    if (surface->format->Rmask == 0x000000ff)
                            texture_format = GL_RGB;
                    else
                            texture_format = GL_BGR;
            } else {
                    printf("warning: the image is not truecolor..  this will probably break\n");
                    // this error should not go unhandled
            }

        // Have OpenGL generate a texture object handle for us
        glGenTextures( 1, &texture );

        // Bind the texture object
        glBindTexture( GL_TEXTURE_2D, texture );

        // Edit the texture object's image data using the information SDL_Surface gives us
        texture_width = surface->w;
        texture_height = surface->h;
        // Set the texture's stretching properties
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );

        glTexImage2D( GL_TEXTURE_2D, 0, nOfColors, surface->w, surface->h, 0,
                          texture_format, GL_UNSIGNED_BYTE, surface->pixels );

        glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

        qDebug() << "width " << texture_width << " height " << texture_height;

    }
    else
    {
        printf("SDL could not load image.bmp: %s\n", SDL_GetError());
        SDL_Quit();
        return 1;
    }


    if ( surface )
    {
        SDL_FreeSurface( surface );
    }




    //b2Vec2 gravity(0.0f, 1.0f);
    //b2World world(gravity);



    Uint32 start;

    init(); // Initializing
    bool running = true;

    //float xrf = 0, yrf = 0, zrf = 0; // Rotating angles

    int doUpdate=0;
    while(running)
    {


        if(doUpdate<=0)
        {

            start=SDL_GetTicks();

            SDL_Event event; //  Events of SDL
            while ( SDL_PollEvent(&event) )
            {

                switch(event.type)
                {
                    case SDL_QUIT:
                        running = false;
                    break;

                    case SDL_KEYDOWN: // If pressed key
                        switch(event.key.keysym.sym)
                        { // Check which
                        case SDLK_ESCAPE: // ESC
                                running = false; // End work of program
                            break;
                        case SDLK_m:
                            myKeys.move_r = true;
                        break;
                        case SDLK_n:
                            myKeys.move_l = true;
                        break;

                        case SDLK_LEFT:
                        case SDLK_a:
                            pos_x += 32;
                            break;
                        case SDLK_RIGHT:
                        case SDLK_d:
                            pos_x -= 32;
                            break;
                        case SDLK_UP:
                        case SDLK_w:
                                pos_y += 32;
                            break;
                        case SDLK_DOWN:
                        case SDLK_s:
                                pos_y -= 32;
                            break;
                        }
                    break;

                    case SDL_KEYUP:
                        switch(event.key.keysym.sym)
                        { // Check which
                            case SDLK_m:
                                myKeys.move_r = false;
                            break;
                            case SDLK_n:
                                myKeys.move_l = false;
                            break;
                        }
                    break;
                }
            }

            if(1000.0/1000>SDL_GetTicks()-start)
                    //SDL_Delay(1000.0/1000-(SDL_GetTicks()-start));
                    doUpdate = 1000.0/1000-(SDL_GetTicks()-start);
        // While program is running, changing loop

        //xrf -= 1;
        //yrf -= 1;
        //zrf -= 1;

            drawQuads();
            //drawCube(xrf, yrf, zrf); // Draw cube with current rotating conrers
            // Updating screen
            glFlush();
            SDL_GL_SwapWindow(window);

        }
        doUpdate--;

        if(myKeys.move_r && ! myKeys.move_l)
            pos_x-=1;
        if(myKeys.move_l && ! myKeys.move_r)
            pos_x+=1;
        SDL_Delay(1);
    }

    // Now we can delete the OpenGL texture and close down SDL
    glDeleteTextures( 1, &texture );

    SDL_Quit(); // Ending work of the SDL and exiting
    return 0;
    //return
}



//Cube draw (only test of the OpenGL works)

void drawQuads()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    //Reset modelview matrix
    glLoadIdentity();

    //Move to center of the screen
    glTranslatef( screen_width / 2.f, screen_height / 2.f, 0.f );

    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE,GL_REPLACE);

    foreach(LevelBGO b, level.bgo)
    {
        QRect bgo = QRect(b.x+pos_x, b.y+pos_y, 32, 32);

        QRectF bgoG = QRectF(mapToOpengl(QPoint(bgo.x(), bgo.y())),
                               mapToOpengl(QPoint(bgo.x()+32, bgo.y()+32)) );

        //            qDebug() << "Point " << blockG.topLeft() << " size "
        //                     << blockG.bottomRight();

        glColor4f( 0.f, 1.f, 0.f, 0.3f);
        glBegin( GL_QUADS );
            glVertex3f( bgoG.left(), bgoG.top(), 0 );
            glVertex3f(  bgoG.right(), bgoG.top(), 0 );
            glVertex3f(  bgoG.right(),  bgoG.bottom(), 0 );
            glVertex3f( bgoG.left(),  bgoG.bottom(), 0 );
        glEnd();
    }

    foreach(LevelBlock b, level.blocks)
    {
        QRect block = QRect(b.x+pos_x, b.y+pos_y, b.w, b.h);

        QRectF blockG = QRectF(mapToOpengl(QPoint(block.x(), block.y())),
                               mapToOpengl(QPoint(block.x()+b.w, block.y()+b.h)) );

        //            qDebug() << "Point " << blockG.topLeft() << " size "
        //                     << blockG.bottomRight();

        // Bind the texture to which subsequent calls refer to

        glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE,GL_REPLACE);
        glColor4f( 1.f, 0.f, 0.f, 1.f);
        glBindTexture( GL_TEXTURE_2D, texture );

        glBegin( GL_QUADS );
            glTexCoord2i( 0, 0 );
            glVertex3f( blockG.left(), blockG.top(), 0);

            glTexCoord2i( blockG.width(), 0 );
            glVertex3f(  blockG.right(), blockG.top(), 0 );

            glTexCoord2i( blockG.width(), blockG.height() );
            glVertex3f(  blockG.right(),  blockG.bottom(), 0);

            glTexCoord2i( 0, blockG.height() );
            glVertex3f( blockG.left(),  blockG.bottom(), 0 );
        glEnd();
    }

}
