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

Scene::Scene()
{
    sceneType = _Unknown;
    fader.setFull();
}

Scene::Scene(TypeOfScene _type)
{
    sceneType = _type;
}

Scene::~Scene()
{
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
