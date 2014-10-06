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

#include "graphics.h"

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

PGE_Texture TextureBuffer[3];

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


SDL_bool IsFullScreen(SDL_Window *win)
{
   Uint32 flags = SDL_GetWindowFlags(win);

    if (flags & SDL_WINDOW_FULLSCREEN) return SDL_TRUE; // return SDL_TRUE if fullscreen

   return SDL_FALSE; // Return SDL_FALSE if windowed
}

/// Toggles On/Off FullScreen
/// @returns -1 on error, 1 on Set fullscreen successful, 0 on Set Windowed successful
int SDL_ToggleFS(SDL_Window *win)
{
    if (IsFullScreen(win))
    {
        // Swith to WINDOWED mode
        if (SDL_SetWindowFullscreen(win, SDL_FALSE) < 0)
      {
         std::cout<<"Setting windowed failed : "<<SDL_GetError()<<std::endl;
         return -1;
      }

        return 0;
    }

    // Swith to FULLSCREEN mode
    if (SDL_SetWindowFullscreen(win, SDL_TRUE) < 0)
   {
      std::cout<<"Setting fullscreen failed : "<<SDL_GetError()<<std::endl;
      return -1;
   }

   return 1;
}

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    Q_UNUSED(a);
    //MainWindow w;
    //w.show();

    //a.exec();
    initSDL();
    initOpenGL();


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

    TextureBuffer[0] = loadTexture(a.applicationDirPath().toStdString()+"/block-223.bmp");
    TextureBuffer[1] = loadTexture(a.applicationDirPath().toStdString()+"/bg.bmp");
    TextureBuffer[2] = loadTexture(a.applicationDirPath().toStdString()+"/bgo.bmp");

    //b2Vec2 gravity(0.0f, 1.0f);
    //b2World world(gravity);

    Uint32 start;

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

                    //case SDL_WINDOWEVENT_RESIZED:
                        //screen_width = ;
                        //screen_height = event.resize.h;
                    //break;

                    case SDL_KEYDOWN: // If pressed key
                        switch(event.key.keysym.sym)
                        { // Check which
                        case SDLK_ESCAPE: // ESC
                                running = false; // End work of program
                            break;
                        case SDLK_t:
                            SDL_ToggleFS(window);
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
    glDeleteTextures( 1, &TextureBuffer[0].texture );
    glDeleteTextures( 1, &TextureBuffer[1].texture );
    glDeleteTextures( 1, &TextureBuffer[2].texture );

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

    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE,GL_MODULATE);

    int imgPos = ((level.sections[0].size_left+pos_x)/2) % TextureBuffer[1].w;
    QRectF blockG = QRectF(mapToOpengl(QPoint(imgPos, 0)), mapToOpengl(QPoint(imgPos+TextureBuffer[1].w,TextureBuffer[1].h)) );

    glColor4f( 1.f, 1.f, 1.f, 1.f);
    glEnable(GL_TEXTURE_2D);
    glBindTexture( GL_TEXTURE_2D, TextureBuffer[1].texture );
    glBegin( GL_QUADS );
        glTexCoord2i( 0, 0 );
        glVertex2f( blockG.left(), blockG.top());

        glTexCoord2i( 1, 0 );
        glVertex2f(  blockG.right(), blockG.top());

        glTexCoord2i( 1, 1 );
        glVertex2f(  blockG.right(),  blockG.bottom());

        glTexCoord2i( 0, 1 );
        glVertex2f( blockG.left(),  blockG.bottom());
    glEnd();

    imgPos += TextureBuffer[1].w;

    blockG = QRectF(mapToOpengl(QPoint(imgPos, 0)), mapToOpengl(QPoint(imgPos+TextureBuffer[1].w,TextureBuffer[1].h)) );
    glBindTexture( GL_TEXTURE_2D, TextureBuffer[1].texture );
    glBegin( GL_QUADS );
        glTexCoord2i( 0, 0 );
        glVertex2f( blockG.left(), blockG.top());

        glTexCoord2i( 1, 0 );
        glVertex2f(  blockG.right(), blockG.top());

        glTexCoord2i( 1, 1 );
        glVertex2f(  blockG.right(),  blockG.bottom());

        glTexCoord2i( 0, 1 );
        glVertex2f( blockG.left(),  blockG.bottom());
    glEnd();
    glDisable(GL_TEXTURE_2D);





    foreach(LevelBGO b, level.bgo)
    {
        QRect bgo = QRect(b.x+pos_x, b.y+pos_y, TextureBuffer[2].w, TextureBuffer[2].h);

        QRectF bgoG = QRectF(mapToOpengl(QPoint(bgo.x(), bgo.y())),
                               mapToOpengl(QPoint(bgo.x()+TextureBuffer[2].w,bgo.y()+TextureBuffer[2].h)) );

        //            qDebug() << "Point " << blockG.topLeft() << " size "
        //                     << blockG.bottomRight();
        glColor4f( 1.f, 1.f, 1.f, 0.3f);
        glEnable(GL_TEXTURE_2D);
        glBindTexture( GL_TEXTURE_2D, TextureBuffer[2].texture );
        //glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
        glBegin( GL_QUADS );
            glTexCoord2i( 0, 0 );
            glVertex3f( bgoG.left(), bgoG.top(), 0 );

            glTexCoord2i( 1, 0 );
            glVertex3f(  bgoG.right(), bgoG.top(), 0 );

            glTexCoord2i( 1, 1 );
            glVertex3f(  bgoG.right(),  bgoG.bottom(), 0 );

            glTexCoord2i( 0, 1 );
            glVertex3f( bgoG.left(),  bgoG.bottom(), 0 );
        glEnd();
        glDisable(GL_TEXTURE_2D);
    }

    foreach(LevelBlock b, level.blocks)
    {
        QRect block = QRect(b.x+pos_x, b.y+pos_y, b.w, b.h);

        QRectF blockG = QRectF(mapToOpengl(QPoint(block.x(), block.y())),
                               mapToOpengl(QPoint(block.x()+b.w, block.y()+b.h)) );

        //            qDebug() << "Point " << blockG.topLeft() << " size "
        //                     << blockG.bottomRight();

        // Bind the texture to which subsequent calls refer to

        glColor4f( 1.f, 1.f, 1.f, 1.f);
        glEnable(GL_TEXTURE_2D);
        glBindTexture( GL_TEXTURE_2D, TextureBuffer[0].texture );
        glBegin( GL_QUADS );
            glTexCoord2i( 0, 0 );
            glVertex2f( blockG.left(), blockG.top());

            glTexCoord2i( 1, 0 );
            glVertex2f(  blockG.right(), blockG.top());

            glTexCoord2i( 1, 1 );
            glVertex2f(  blockG.right(),  blockG.bottom());

            glTexCoord2i( 0, 1 );
            glVertex2f( blockG.left(),  blockG.bottom());
        glEnd();
        glDisable(GL_TEXTURE_2D);
    }
}
