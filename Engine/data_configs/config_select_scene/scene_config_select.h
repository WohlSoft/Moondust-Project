/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2017 Vitaly Novichkov <admin@wohlnet.ru>
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

#ifndef SCENE_CONFIGSELECT_H
#define SCENE_CONFIGSELECT_H

#include <scenes/scene.h>
#include <controls/controller.h>
#include <gui/pge_menu.h>
#include <graphics/gl_color.h>

#include <QList>

class ConfigSelectScene : public Scene
{
public:
    ConfigSelectScene();
    ~ConfigSelectScene();

    QString currentConfig;
    QString currentConfigPath;
    QString themePack;

    bool    hasConfigPacks();
    QString isPreLoaded(QString openConfig="");

    void onKeyboardPressedSDL(SDL_Keycode sdl_key, Uint16 modifier); //!< Triggering when pressed any key on keyboard
    void onMouseMoved(SDL_MouseMotionEvent &mmevent);
    void onMousePressed(SDL_MouseButtonEvent &mbevent);
    void onMouseWheel(SDL_MouseWheelEvent &wheelevent);

    void update();
    void render();
    void renderMouse();
    int  exec();

    void processMenu();

    void processEvents();

    Controller* controller;

    void setLabel(QString label);

private:
    struct ConfigPackEntry
    {
        PGE_Texture image;
        QString fullname;
        QString description;
        QString key;
        QString path;
    };
    int ret;//!< Exit code
    QString  m_label;
    QString  m_waterMark;
    PGE_Rect m_waterMarkRect;
    GlColor  m_waterMarkColor;
    int      m_waterMarkFontSize;
    PGE_Point mousePos;
    QList<ConfigPackEntry> m_availablePacks;
    PGE_Menu menu;
    PGE_Texture cursor;
    PGEColor    bgcolor;
};

#endif // SCENE_CONFIGSELECT_H
