//#include "mainwindow.h"
#include <QCoreApplication>
#include <QElapsedTimer>
#include <QFileInfo>
#include <QDir>

#include "common_features/app_path.h"
#include "common_features/graphics_funcs.h"

#include "graphics/window.h"
#include "graphics/gl_renderer.h"
#undef main

#include <Box2D/Box2D.h>

#include <file_formats.h>

#include "graphics/graphics.h"

#include <iostream>
using namespace std;

#include <QtDebug>

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
    bool go_r = false;
    bool go_l = false;
    bool jump = false;
    bool run= false;
};

keysForTest myKeys;

void resetKeys()
{
    myKeys.move_r = false;
    myKeys.move_l = false;
    myKeys.go_r = false;
    myKeys.go_l = false;
    myKeys.jump = false;
    myKeys.run = false;
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

b2Body* playerBody;
b2World* world;

const float pixMeter=10.0f; // Pixels per meter
float met2pix(float met)
{
    return met * pixMeter;
}
float pix2met(float pix)
{
    return pix / pixMeter;
}


int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);


    ApplicationPath = QCoreApplication::applicationDirPath();
    ApplicationPath_x = QCoreApplication::applicationDirPath();

    #ifdef __APPLE__
    //Application path relative bundle folder of application
    QString osX_bundle = QCoreApplication::applicationName()+".app/Contents/MacOS";
    if(ApplicationPath.endsWith(osX_bundle, Qt::CaseInsensitive))
        ApplicationPath.remove(ApplicationPath.length()-osX_bundle.length()-1, osX_bundle.length()+1);
    #endif






    if(!PGE_Window::init("PGE Engine - dummy tester")) exit(1);
    if(!GlRenderer::init()) exit(1);







    QString fileToPpen = ApplicationPath+"/physics.lvl";
    if(a.arguments().size()>1)
        fileToPpen = a.arguments()[1];


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




    TextureBuffer[0] = GraphicsHelps::loadTexture(ApplicationPath+"/block-223.bmp");
    TextureBuffer[1] = GraphicsHelps::loadTexture(ApplicationPath+"/background2-14.png");
    TextureBuffer[2] = GraphicsHelps::loadTexture(ApplicationPath+"/background-103.gif",
                                                  ApplicationPath+"/background-103m.gif");






    b2Vec2 gravity(0.0f, 150.0f);
    world = new b2World(gravity);





    //blocks
    for(int i=0; i<level.blocks.size(); i++)
    {
        b2BodyDef bodyDef;
        bodyDef.type = b2_staticBody;
        bodyDef.position.Set( pix2met(level.blocks[i].x+(level.blocks[i].w/2) ),
            pix2met(level.blocks[i].y + (level.blocks[i].h/2)) );
        //bodyDef.userData <- I will use them as Pointer to array with settings
        b2Body* body = world->CreateBody(&bodyDef);
        b2PolygonShape shape;
        shape.SetAsBox(pix2met(level.blocks[i].w+1)/2, pix2met(level.blocks[i].h+1)/2);
        b2Fixture * block = body->CreateFixture(&shape, 1.0f);
        block->SetFriction(level.blocks[i].slippery? 0.04f : 0.25f );
    }






    //players

    b2BodyDef bodyDef;
    bodyDef.type = b2_dynamicBody;
    bodyDef.position.Set(pix2met(level.players[0].x + (level.players[0].w/2)),
            pix2met(level.players[0].y + (level.players[0].h/2) ) );
    bodyDef.fixedRotation = true;
    bodyDef.bullet = true;
    //bodyDef.userData <- I will use them as Pointer to array with settings
    playerBody = world->CreateBody(&bodyDef);

//    int32 polygonCount = 8;
//    b2Vec2 polygonArray[polygonCount];
//    float pW = pix2met(level.players[0].w)/2;
//    float pH = pix2met(level.players[0].h)/2;

//    polygonArray[0].Set(-pW, -pH+0.5f);
//    polygonArray[1].Set(-pW+0.5f, -pH);
//    polygonArray[2].Set(pW-0.5f, -pH);
//    polygonArray[3].Set(pW, -pH+0.5f);

//    polygonArray[4].Set(pW, pH-0.5f);
//    polygonArray[5].Set(pW-0.5f, pH);
//    polygonArray[6].Set(-pW+0.5f, pH);
//    polygonArray[7].Set(-pW, pH-0.5f);

    b2PolygonShape shape;
    shape.SetAsBox(pix2met(level.players[0].w)/2-0.1, pix2met(level.players[0].h)/2-0.1);
    //shape.Set(polygonArray,polygonCount);
    b2FixtureDef fixtureDef;
    fixtureDef.shape = &shape;
    fixtureDef.density = 1.0f; fixtureDef.friction = 0.3f;
    playerBody->CreateFixture(&fixtureDef);







    //Main Loop


    Uint32 start;
    bool running = true;
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
                        //PGE_Window::Width = ;
                        //PGE_Window::Height = event.resize.h;
                    //break;

                    case SDL_KEYDOWN: // If pressed key
                      switch(event.key.keysym.sym)
                      { // Check which
                        case SDLK_ESCAPE: // ESC
                                running = false; // End work of program
                            break;
                        case SDLK_t:
                            SDL_ToggleFS(PGE_Window::window);
                        break;
                        case SDLK_m:
                            myKeys.move_r = true;
                        break;
                        case SDLK_n:
                            myKeys.move_l = true;
                        break;

                        case SDLK_z:
                            if(!myKeys.jump)
                             playerBody->SetLinearVelocity(b2Vec2(playerBody->GetLinearVelocity().x, -65.0f-fabs(playerBody->GetLinearVelocity().x/5)));
                            myKeys.jump=true;
                        break;

                        case SDLK_x:
                             myKeys.run=true;
                        break;

                        case SDLK_RIGHT:
                                myKeys.go_r=true;
                        break;

                        case SDLK_LEFT:
                                myKeys.go_l=true;
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

                            case SDLK_z:
                                myKeys.jump=false;
                            break;

                            case SDLK_x:
                                 myKeys.run=false;
                            break;

                            case SDLK_RIGHT:
                                myKeys.go_r=false;
                                break;
                            case SDLK_LEFT:
                                myKeys.go_l=false;
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
            SDL_GL_SwapWindow(PGE_Window::window);

        }
        doUpdate-=10;

        world->Step(1.0f / 100.0f, 1, 1);

        if(myKeys.move_r && ! myKeys.move_l)
            pos_x-=1;
        if(myKeys.move_l && ! myKeys.move_r)
            pos_x+=1;


        //playerBody->SetLinearVelocity(b2Vec2(30,playerBody->GetLinearVelocity().y));
        //if(playerBody->GetLinearVelocity().x < -30)
        //playerBody->SetLinearVelocity(b2Vec2(-30,playerBody->GetLinearVelocity().y));


        float32 force=950.0f;
        float32 hMaxSpeed=24.0f;
        float32 hRunningMaxSpeed=48.0f;
        float32 fallMaxSpeed=720.0f;

        float32 curHMaxSpeed=hMaxSpeed;


        if(myKeys.run)
        {
            curHMaxSpeed = hRunningMaxSpeed;
        }
        else
        {
            curHMaxSpeed = hMaxSpeed;
        }

        if(playerBody->GetLinearVelocity().y > fallMaxSpeed/2)
            playerBody->SetLinearVelocity(b2Vec2(playerBody->GetLinearVelocity().x, fallMaxSpeed/2));

        if(myKeys.go_r)
            if(playerBody->GetLinearVelocity().x <= curHMaxSpeed)
                playerBody->ApplyForceToCenter(b2Vec2(force, 0.0f), true);
        if(myKeys.go_l)
            if(playerBody->GetLinearVelocity().x >= -curHMaxSpeed)
                playerBody->ApplyForceToCenter(b2Vec2(-force, 0.0f), true);

        //Return to start if player was fall
        if(met2pix(playerBody->GetPosition().y) > level.sections[0].size_bottom+30)
            playerBody->SetTransform(b2Vec2(pix2met(level.players[0].x + (level.players[0].w/2)),
                    pix2met(level.players[0].y + (level.players[0].h/2) )), 0.0f);


        //Connect sized
        if(level.sections[0].IsWarp)
        {
            if(met2pix(playerBody->GetPosition().x) < level.sections[0].size_left-29)
                playerBody->SetTransform(b2Vec2(
                     pix2met(level.sections[0].size_right + (level.players[0].w/2)),
                        playerBody->GetPosition().y), 0.0f);
            else
            if(met2pix(playerBody->GetPosition().x) > level.sections[0].size_right+30)
                playerBody->SetTransform(b2Vec2(
                     pix2met(level.sections[0].size_left-29 + (level.players[0].w/2)),
                        playerBody->GetPosition().y), 0.0f);
        }


        SDL_Delay(10);
    }




    delete world;



    // Now we can delete the OpenGL texture and close down SDL
    glDeleteTextures( 1, &TextureBuffer[0].texture );
    glDeleteTextures( 1, &TextureBuffer[1].texture );
    glDeleteTextures( 1, &TextureBuffer[2].texture );




    PGE_Window::uninit();





    return 0;
    //return
}



//Cube draw (only test of the OpenGL works)

void drawQuads()
{


    //Change camera position
    pos_x = -1*(met2pix(playerBody->GetPosition().x) - PGE_Window::Width/2);
    pos_y = -1*(met2pix(playerBody->GetPosition().y) - PGE_Window::Height/2);

    if(-pos_x < level.sections[0].size_left)
        pos_x = -level.sections[0].size_left;
    if(-(pos_x-PGE_Window::Width) > level.sections[0].size_right)
        pos_x = -level.sections[0].size_right+PGE_Window::Width;

    if(-pos_y < level.sections[0].size_top)
        pos_y = -level.sections[0].size_top;
    if(-(pos_y-PGE_Window::Height) > level.sections[0].size_bottom)
        pos_y = -level.sections[0].size_bottom+PGE_Window::Height;


    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    //Reset modelview matrix
    glLoadIdentity();

    //Move to center of the screen
    glTranslatef( PGE_Window::Width / 2.f, PGE_Window::Height / 2.f, 0.f );

    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE,GL_MODULATE);

    int imgPos_X = ((level.sections[0].size_left+pos_x)/2) % TextureBuffer[1].w;


    int imgPos_Y;

    double sHeight = fabs(level.sections[0].size_top-level.sections[0].size_bottom);

    if(sHeight > (double)TextureBuffer[1].h)
        imgPos_Y =
                (level.sections[0].size_top+pos_y)
                /
                (
                    (sHeight - PGE_Window::Height)/
                    (TextureBuffer[1].h - PGE_Window::Height)
                );
    else if(sHeight == (double)TextureBuffer[1].h)
        imgPos_Y = level.sections[0].size_top+pos_y;
    else
        imgPos_Y =
                level.sections[0].size_bottom+pos_y
                - TextureBuffer[1].h;

    //fabs(level.sections[0].size_top-level.sections[0].size_bottom)
    //TextureBuffer[1].h
    //PGE_Window::Height
    //pos_y

    QRectF blockG = QRectF(mapToOpengl(QPoint(imgPos_X, imgPos_Y)), mapToOpengl(QPoint(imgPos_X+TextureBuffer[1].w, imgPos_Y+TextureBuffer[1].h)) );

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

    imgPos_X += TextureBuffer[1].w;

    blockG = QRectF(mapToOpengl(QPoint(imgPos_X, imgPos_Y)), mapToOpengl(QPoint(imgPos_X+TextureBuffer[1].w,imgPos_Y+TextureBuffer[1].h)) );
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
        glColor4f( 1.f, 1.f, 1.f, 1.0f);
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

    //qDebug()<< playerBody->GetPosition().x<< "x" << playerBody->GetPosition().y;
    QRect pl = QRect( met2pix(playerBody->GetPosition().x)-(level.players[0].w/2) +pos_x, met2pix(playerBody->GetPosition().y)-(level.players[0].h/2)+pos_y,
                      level.players[0].w, level.players[0].h);

    QRectF player = QRectF(mapToOpengl(QPoint(pl.x(), pl.y())),
                           mapToOpengl(QPoint(pl.x()+pl.width(), pl.y()+pl.height())) );
    glColor4f( 0.f, 0.f, 1.f, 1.f);
    glBegin( GL_QUADS );
        glVertex2f( player.left(), player.top());
        glVertex2f(  player.right(), player.top());
        glVertex2f(  player.right(),  player.bottom());
        glVertex2f( player.left(),  player.bottom());
    glEnd();

}
