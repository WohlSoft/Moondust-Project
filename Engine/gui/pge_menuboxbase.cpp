#include "pge_menuboxbase.h"

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

#include <fontman/font_manager.h>
#include <graphics/window.h>

#include <common_features/app_path.h>
#include <audio/pge_audio.h>
#include <settings/global_settings.h>

#include "../data_configs/config_select_scene/scene_config_select.h"
#include "../scenes/scene_level.h"
#include "../scenes/scene_world.h"
#include "../scenes/scene_gameover.h"


#include <QFontMetrics>

PGE_MenuBoxBase::PGE_MenuBoxBase(Scene *_parentScene, PGE_Menu::menuAlignment alignment, int gapSpace, std::string _title, msgType _type,
                                 PGE_Point boxCenterPos, double _padding, std::string texture)
    : PGE_BoxBase(_parentScene), _menu(alignment, gapSpace)
{
    setParentScene(_parentScene);
    construct(_title, _type, boxCenterPos, _padding, texture);
}

PGE_MenuBoxBase::PGE_MenuBoxBase(const PGE_MenuBoxBase &mb)
    : PGE_BoxBase(mb), _menu(mb._menu)
{
    _page     = mb._page;
    running   = mb.running;
    fontID    = mb.fontID;
    fontRgba  = mb.fontRgba;
    _answer_id = mb._answer_id;
    reject_snd = mb.reject_snd;
    _ctrl1    = mb._ctrl1;
    _ctrl2    = mb._ctrl2;
    type      = mb.type;
    _pos =       mb._pos;
    _sizeRect = mb._sizeRect;
    title     = mb.title;
    title_size = mb.title_size;
    //_menu     = mb._menu;
    width     = mb.width;
    height    = mb.height;
    padding   = mb.padding;
    bg_color  = mb.bg_color;
}


void PGE_MenuBoxBase::construct(std::string _title, PGE_MenuBoxBase::msgType _type,
                                PGE_Point pos, double _padding, std::string texture)
{
    if(!texture.empty())
        loadTexture(texture);

    _ctrl1 = nullptr;
    _ctrl2 = nullptr;
    fontRgba.setRgba(qRgba(255, 255, 255, 255));
    updateTickValue();
    _page = 0;
    running = false;
    _answer_id = -1;
    _pos = pos;
    _menu.setTextLenLimit(30, true);
    setTitleFont(ConfigManager::setup_menu_box.title_font_name);
    setTitleFontColor(ConfigManager::setup_menu_box.title_font_rgba);
    /****************Word wrap*********************/
    title = _title;
    FontManager::optimizeText(title, 27);
    title_size = FontManager::textSize(_title, fontID, 27);
    /****************Word wrap*end*****************/
    setPadding(_padding);
    setType(_type);
    updateSize();
}

PGE_MenuBoxBase::~PGE_MenuBoxBase()
{}

void PGE_MenuBoxBase::setParentScene(Scene *_parentScene)
{
    PGE_BoxBase::setParentScene(_parentScene);
    initControllers();
}

void PGE_MenuBoxBase::setType(PGE_MenuBoxBase::msgType _type)
{
    switch(type)
    {
    case msg_info:
        bg_color =       GlColor(0, 0, 0);
        break;
    case msg_info_light:
        bg_color =      GlColor(0, 0, 0.490196078);
        break;
    case msg_warn:
        bg_color =       GlColor(1.0, 0.788235294, 0.054901961);
        break;
    case msg_error:
        bg_color =      GlColor(0.490196078, 0, 0);
        break;
    case msg_fatal:
        bg_color =      GlColor(1.0, 0, 0);
        break;
    default:
        bg_color =      GlColor(0, 0, 0);
        break;
    }

    type = _type;
}

void PGE_MenuBoxBase::setTitleFont(std::string fontName)
{
    fontID   = FontManager::getFontID(fontName);
}

void PGE_MenuBoxBase::setTitleFontColor(GlColor color)
{
    fontRgba = color;
}

void PGE_MenuBoxBase::setTitleText(std::string text)
{
    title = text;
    FontManager::optimizeText(title, 27);
    title_size = FontManager::textSize(text, fontID, 27);
    updateSize();
}

void PGE_MenuBoxBase::setPadding(int _padding)
{
    setPadding(static_cast<double>(_padding));
}

void PGE_MenuBoxBase::setPadding(double _padding)
{
    if(_padding < 0.0)
        _padding = ConfigManager::setup_menu_box.box_padding;

    padding = _padding;
}

void PGE_MenuBoxBase::setPos(double x, double y)
{
    _pos.setX(static_cast<int>(x));
    _pos.setY(static_cast<int>(y));
    updateSize();
}

void PGE_MenuBoxBase::setMaxMenuItems(int items)
{
    _menu.setItemsNumber(items);
    updateSize();
}

void PGE_MenuBoxBase::setBoxSize(double _Width, double _Height, double _padding)
{
    width = _Width;
    height = _Height;
    padding = _padding;
}

void PGE_MenuBoxBase::updateSize()
{
    PGE_Rect menuRect = _menu.rectFull();

    if(menuRect.width() > title_size.w())
        width = menuRect.width() / 2;
    else
        width = title_size.w() / 2;

    height = (title_size.h() + menuRect.height()) / 2;
    int pad = static_cast<int>(padding);
    int w = static_cast<int>(width);
    int h = static_cast<int>(height);

    if((_pos.x() == -1) && (_pos.y() == -1))
    {
        _sizeRect.setLeft(PGE_Window::Width / 2 - w - pad);
        _sizeRect.setTop(PGE_Window::Height / 2 - h - pad);
        _sizeRect.setRight(PGE_Window::Width / 2 + w + pad);
        _sizeRect.setBottom(PGE_Window::Height / 2 + h + pad);

        if(_sizeRect.top() < padding)
            _sizeRect.setY(pad);
    }
    else
    {
        _sizeRect.setLeft(_pos.x() - w - pad);
        _sizeRect.setTop(_pos.y() - h - pad);
        _sizeRect.setRight(_pos.x() + w + pad);
        _sizeRect.setBottom(_pos.y() + h + pad);
    }

    _menu.setPos(_sizeRect.right() - (menuRect.width() > title_size.w() ? menuRect.width() : title_size.w()),
                 _sizeRect.bottom() - menuRect.height() + _menu.topOffset() - pad);
}



void PGE_MenuBoxBase::clearMenu()
{
    _menu.clear();
    updateSize();
}

void PGE_MenuBoxBase::addMenuItem(std::string& menuitem)
{
    _menu.addMenuItem(menuitem, menuitem);
    updateSize();
}

void PGE_MenuBoxBase::addMenuItems(std::vector<std::string> &menuitems)
{
    for(std::string &menuitem : menuitems)
        _menu.addMenuItem(menuitem, menuitem);

    updateSize();
}


void PGE_MenuBoxBase::update(double ticks)
{
    switch(_page)
    {
    case 0:
        setFade(10, 1.0, 0.05);
        _page++;
        break;

    case 1:
        processLoader(ticks);
        break;

    case 2:
        processBox(ticks);
        break;

    case 3:
        processUnLoader(ticks);
        break;

    case 4:
    default:
        running = false;
        break;
    }
}

void PGE_MenuBoxBase::render()
{
    if(_page == 2)
    {
        if(m_textureUsed)
            drawTexture(_sizeRect, 32, static_cast<float>(m_faderOpacity));
        else
        {
            GlRenderer::renderRect(_sizeRect.left(), _sizeRect.top(),
                                   _sizeRect.width(), _sizeRect.height(),
                                   bg_color.Red(),
                                   bg_color.Green(),
                                   bg_color.Blue(),
                                   static_cast<float>(m_faderOpacity));
        }

        FontManager::printText(title,
                               _sizeRect.center().x() - title_size.w() / 2,
                               _sizeRect.top() + static_cast<int>(padding),
                               fontID,
                               fontRgba.Red(), fontRgba.Green(), fontRgba.Blue(), fontRgba.Alpha());
        _menu.render();
        //        FontManager::SDL_string_render2D(_sizeRect.left()+padding,
        //                                         _sizeRect.top()+padding,
        //                                         &textTexture);
    }
    else
    {
        if(m_textureUsed)
        {
            drawTexture(_sizeRect.center().x() - static_cast<int>((width + padding)*m_faderOpacity),
                        _sizeRect.center().y() - static_cast<int>((height + padding)*m_faderOpacity),
                        _sizeRect.center().x() + static_cast<int>((width + padding)*m_faderOpacity),
                        _sizeRect.center().y() + static_cast<int>((height + padding)*m_faderOpacity),
                        32, static_cast<float>(m_faderOpacity));
        }
        else
        {
            GlRenderer::renderRectBR(_sizeRect.center().x() - static_cast<int>((width + padding)*m_faderOpacity),
                                     _sizeRect.center().y() - static_cast<int>((height + padding)*m_faderOpacity),
                                     _sizeRect.center().x() + static_cast<int>((width + padding)*m_faderOpacity),
                                     _sizeRect.center().y() + static_cast<int>((height + padding)*m_faderOpacity),
                                     bg_color.Red(),
                                     bg_color.Green(),
                                     bg_color.Blue(), static_cast<float>(m_faderOpacity));
        }
    }
}

void PGE_MenuBoxBase::restart()
{
    _page = 0;
    _menu.resetState();
    _menu.reset();
    running = true;
}

bool PGE_MenuBoxBase::isRunning()
{
    return running;
}

void PGE_MenuBoxBase::exec()
{
    restart();

    while(running)
    {
        Uint32 start_render = SDL_GetTicks();
        updateControllers();
        update(m_uTickf);
        PGE_BoxBase::render();
        render();
        GlRenderer::flush();
        GlRenderer::repaint();

        if((!PGE_Window::vsync) && (m_uTick > static_cast<int>(SDL_GetTicks() - start_render)))
            SDL_Delay(static_cast<unsigned int>(m_uTick) - (SDL_GetTicks() - start_render));
    }
}

void PGE_MenuBoxBase::setRejectSnd(long sndRole)
{
    obj_sound_role::roles _sndRole =  static_cast<obj_sound_role::roles>(sndRole);
    reject_snd = ConfigManager::getSoundByRole(_sndRole);
}

int PGE_MenuBoxBase::answer()
{
    return _answer_id;
}

void PGE_MenuBoxBase::reject()
{
    _answer_id = -1;
    _page++;
    setFade(10, 0.0, 0.05);

    if(reject_snd > 0)
        PGE_Audio::playSound(reject_snd);
}

void PGE_MenuBoxBase::processKeyEvent(SDL_Keycode &key)
{
    if(_page != 2) return;

    if(_ctrl1)
    {
        if(_ctrl1->keys.up)
        {
            onUpButton();
            return;
        }
        else if(_ctrl1->keys.down)
        {
            onDownButton();
            return;
        }
        else if(_ctrl1->keys.left)
        {
            onLeftButton();
            return;
        }
        else if(_ctrl1->keys.right)
        {
            onRightButton();
            return;
        }
        else if(_ctrl1->keys.jump)
        {
            onJumpButton();
            return;
        }
        else if(_ctrl1->keys.alt_jump)
        {
            onAltJumpButton();
            return;
        }
        else if(_ctrl1->keys.run)
        {
            onRunButton();
            return;
        }
        else if(_ctrl1->keys.alt_run)
        {
            onAltRunButton();
            return;
        }
        else if(_ctrl1->keys.start)
        {
            onStartButton();
            return;
        }
        else if(_ctrl1->keys.drop)
        {
            onDropButton();
            return;
        }
    }

    switch(key)
    {
    case SDLK_UP:
        onUpButton();
        break;

    case SDLK_DOWN:
        onDownButton();
        break;

    case SDLK_LEFT:
        onLeftButton();
        break;

    case SDLK_RIGHT:
        onRightButton();
        break;

    case SDLK_RETURN:
    case SDLK_KP_ENTER:
        onStartButton();
        break;

    case SDLK_ESCAPE:
        onDropButton();
        return;

    default:
        break;
    }
}

void PGE_MenuBoxBase::processLoader(double ticks)
{
    SDL_Event event;

    while(SDL_PollEvent(&event))
    {
        PGE_Window::processEvents(event);

        if(event.type == SDL_QUIT)
            m_faderOpacity = 1.0;
    }

    tickFader(ticks);

    if(m_faderOpacity >= 1.0) _page++;
}

void PGE_MenuBoxBase::processBox(double)
{
    //    #ifndef __APPLE__
    //    if(g_AppSettings.interprocessing)
    //        qApp->processEvents();
    //    #endif
    SDL_Event event;

    while(SDL_PollEvent(&event))
    {
        PGE_Window::processEvents(event);

        switch(event.type)
        {
        case SDL_QUIT:
            reject();
            break;

        case SDL_KEYDOWN: // If pressed key
            processKeyEvent(event.key.keysym.sym);
            break;

        default:
            break;
        }
    }

    if(_menu.isSelected())
    {
        if(_menu.isAccepted())
            _answer_id = _menu.currentItemI();
        else
            _answer_id = -1;

        _page++;
        setFade(10, 0.0, 0.05);
        return;
    }
}



void PGE_MenuBoxBase::processUnLoader(double ticks)
{
    SDL_Event event;

    while(SDL_PollEvent(&event))
    {
        PGE_Window::processEvents(event);

        if(event.type == SDL_QUIT)
            m_faderOpacity = 0.0;
    }

    tickFader(ticks);

    if(m_faderOpacity <= 0.0) _page++;
}


void PGE_MenuBoxBase::initControllers()
{
    if(m_parentScene != nullptr)
    {
        if(m_parentScene->type() == Scene::Level)
        {
            LevelScene *s = dynamic_cast<LevelScene *>(m_parentScene);

            if(s)
            {
                _ctrl1 = s->player1Controller;
                _ctrl2 = s->player2Controller;
            }
        }
        else if(m_parentScene->type() == Scene::World)
        {
            WorldScene *s = dynamic_cast<WorldScene *>(m_parentScene);

            if(s)
            {
                _ctrl1 = s->player1Controller;
                _ctrl2 = nullptr;
            }
        }
        else if(m_parentScene->type() == Scene::GameOver)
        {
            GameOverScene *s = dynamic_cast<GameOverScene *>(m_parentScene);

            if(s)
            {
                _ctrl1 = s->player1Controller;
                _ctrl2 = nullptr;
            }
        }
        else if(m_parentScene->type() == Scene::ConfigSelect)
        {
            ConfigSelectScene *s = dynamic_cast<ConfigSelectScene *>(m_parentScene);

            if(s)
            {
                _ctrl1 = s->controller;
                _ctrl2 = nullptr;
            }
        }
        else
        {
            _ctrl1 = nullptr;
            _ctrl2 = nullptr;
        }
    }
    else
    {
        _ctrl1 = nullptr;
        _ctrl2 = nullptr;
    }
}

void PGE_MenuBoxBase::updateControllers()
{
    SDL_PumpEvents();

    if(_ctrl1)
    {
        _ctrl1->update();
        _ctrl1->sendControls();
    }

    if(_ctrl2)
    {
        _ctrl2->update();
        _ctrl2->sendControls();
    }

    if(m_parentScene != NULL)
    {
        if(m_parentScene->type() == Scene::Level)
        {
            LevelScene *s = dynamic_cast<LevelScene *>(m_parentScene);

            if(s)
                s->tickAnimations(m_uTickf);
        }
        else if(m_parentScene->type() == Scene::World)
        {
            WorldScene *s = dynamic_cast<WorldScene *>(m_parentScene);

            if(s)
                s->tickAnimations(m_uTickf);
        }
    }
}
