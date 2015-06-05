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

#include "scene.h"
#include <QString>
#include <graphics/window.h>
#include <graphics/gl_renderer.h>

void Scene::construct()
{
    fader.setFull();
    fader.setFade(10, 0.0f, 0.02f); //!< Fade in scene when it was started
    running=true;
    doExit=false;
    _doShutDown=false;
    __waiting_step=0;
    updateTickValue();    
}

void Scene::updateTickValue()
{
    uTickf = 1000.0f/(float)PGE_Window::PhysStep;
    uTick = round(uTickf);
    if(uTick<=0) uTick=1;
    if(uTickf<=0) uTickf=1.0;
}

Scene::Scene()
{
    sceneType = _Unknown;
    construct();
}

Scene::Scene(TypeOfScene _type)
{
    sceneType = _type;
    construct();
}

Scene::~Scene()
{
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f); // Black background color
    //Clear screen
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    //Reset modelview matrix
    glLoadIdentity();
}

void Scene::onKeyInput(int)
{}

void Scene::onKeyboardPressed(SDL_Scancode)
{}

void Scene::onKeyboardPressedSDL(SDL_Keycode, Uint16)
{}

void Scene::onKeyboardReleased(SDL_Scancode)
{}

void Scene::onKeyboardReleasedSDL(SDL_Keycode, Uint16)
{}

void Scene::onMouseMoved(SDL_MouseMotionEvent &)
{}

void Scene::onMousePressed(SDL_MouseButtonEvent &)
{}

void Scene::onMouseReleased(SDL_MouseButtonEvent &)
{}

void Scene::onMouseWheel(SDL_MouseWheelEvent &)
{}


void Scene::processEvents()
{
    SDL_Event event; //  Events of SDL
    while ( SDL_PollEvent(&event) )
    {
        if(PGE_Window::processEvents(event)!=0) continue;
        switch(event.type)
        {
            case SDL_QUIT:
                {
                    doExit          = true;
                    running         = false;
                    _doShutDown = true;
                    break;
                }// End work of program
            break;
            case SDL_KEYDOWN: // If pressed key
                onKeyboardPressedSDL(event.key.keysym.sym, event.key.keysym.mod);
                onKeyboardPressed(event.key.keysym.scancode);
            break;
            case SDL_KEYUP: // If released key
                onKeyboardReleasedSDL(event.key.keysym.sym, event.key.keysym.mod);
                onKeyboardReleased(event.key.keysym.scancode);
            break;
            case SDL_MOUSEBUTTONDOWN:
                onMousePressed(event.button);
            break;
            case SDL_MOUSEBUTTONUP:
                onMouseReleased(event.button);
            break;
            case SDL_MOUSEWHEEL:
                onMouseWheel(event.wheel);
            break;
            case SDL_MOUSEMOTION:
                onMouseMoved(event.motion);
            break;
        }
    }
}

void Scene::update()
{
    fader.tickFader(uTickf);
}

void Scene::render()
{
    if(!fader.isNull())
    {
        GlRenderer::renderRect(0, 0, PGE_Window::Width, PGE_Window::Height, 0.f, 0.f, 0.f, fader.fadeRatio());
    }
}

void Scene::renderMouse()
{}

int Scene::exec()
{
    return 0;
}

Scene::TypeOfScene Scene::type()
{
    return sceneType;
}

bool Scene::isExiting()
{
    return doExit;
}

bool Scene::doShutDown()
{
    return _doShutDown;
}

/**************************Fader*******************************/
bool Scene::isOpacityFadding()
{
    return fader.isFading();
}

void Scene::setFade(int speed, float target, float step)
{
    fader.setFade(speed, target, step);
}
/**************************Fader**end**************************/

/************waiting timer************/
void Scene::wait(float ms)
{
    while(__waiting_step < floor(ms) )
    {
        __waiting_step+=ms;
        SDL_Delay((int)floor(ms));
    }
    while(__waiting_step > 0)
        __waiting_step-=ms;
}
/************waiting timer************/
