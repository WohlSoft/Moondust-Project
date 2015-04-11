/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2014 Vitaly Novichkov <admin@wohlnet.ru>
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

Scene::Scene()
{
    sceneType = _Unknown;
    /*********Fader*************/
    fader_opacity=1.0f;
    target_opacity=1.0f;
    fade_step=0.0f;
    fadeSpeed=25;
    fadeRunned=false;
    /*********Fader*************/
}

Scene::Scene(TypeOfScene _type)
{
    sceneType = _type;
}

Scene::~Scene()
{
    if(fader_timer_id)
        SDL_RemoveTimer(fader_timer_id);
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f); // Black background color
    //Clear screen
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    //Reset modelview matrix
    glLoadIdentity();
}

void Scene::update()
{}

void Scene::render()
{
    if(fader_opacity>0.0f)
    {
        glDisable(GL_TEXTURE_2D);
        glColor4f( 0.f, 0.f, 0.f, fader_opacity);
        glBegin( GL_QUADS );
            glVertex2f( 0, 0);
            glVertex2f( PGE_Window::Width, 0);
            glVertex2f( PGE_Window::Width, PGE_Window::Height);
            glVertex2f( 0, PGE_Window::Height);
        glEnd();
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

/**************************Fader*******************************/
bool Scene::isOpacityFadding()
{
    return fadeRunned;
}

void Scene::setFade(int speed, float target, float step)
{
    fade_step = fabs(step);
    target_opacity = target;
    fadeSpeed = speed;
    fadeRunned=true;
    fader_timer_id = SDL_AddTimer(speed, &Scene::nextOpacity, this);
}

unsigned int Scene::nextOpacity(unsigned int x, void *p)
{
    Q_UNUSED(x);
    Scene *self = reinterpret_cast<Scene *>(p);
    self->fadeStep();
    return 0;
}

void Scene::fadeStep()
{
    if(fader_opacity < target_opacity)
        fader_opacity+=fade_step;
    else
        fader_opacity-=fade_step;

    if(fader_opacity>=1.0 || fader_opacity<=0.0)
    {
        SDL_RemoveTimer(fader_timer_id);
        fadeRunned=false;
    }
    else
        fader_timer_id = SDL_AddTimer(fadeSpeed, &Scene::nextOpacity, this);
}
/**************************Fader**end**************************/
