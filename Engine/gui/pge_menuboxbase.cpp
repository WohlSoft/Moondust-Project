/*
 * Moondust, a free game engine for platform game making
 * Copyright (c) 2014-2019 Vitaly Novichkov <admin@wohlnet.ru>
 *
 * This software is licensed under a dual license system (MIT or GPL version 3 or later).
 * This means you are free to choose with which of both licenses (MIT or GPL version 3 or later)
 * you want to use this software.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *
 * You can see text of MIT license in the LICENSE.mit file you can see in Engine folder,
 * or see https://mit-license.org/.
 *
 * You can see text of GPLv3 license in the LICENSE.gpl3 file you can see in Engine folder,
 * or see <http://www.gnu.org/licenses/>.
 */

#include "pge_menuboxbase.h"

#include <fontman/font_manager.h>
#include <graphics/window.h>

#include <common_features/app_path.h>
#include <audio/pge_audio.h>
#include <settings/global_settings.h>
#include <common_features/logger.h>

#include "../data_configs/config_select_scene/scene_config_select.h"
#include "../scenes/scene_level.h"
#include "../scenes/scene_world.h"
#include "../scenes/scene_gameover.h"
#include "../scenes/scene_title.h"

#include <utility>

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

PGE_MenuBoxBase::PGE_MenuBoxBase(Scene *_parentScene, PGE_Menu::menuAlignment alignment, int gapSpace, std::string _title, msgType _type,
                                 PGE_Point boxCenterPos, double _padding, std::string texture)
    : PGE_BoxBase(_parentScene), m_menu(alignment, gapSpace)
{
    construct(std::move(_title), _type, boxCenterPos, _padding, std::move(texture));
}

PGE_MenuBoxBase::PGE_MenuBoxBase(const PGE_MenuBoxBase &mb)
    : PGE_BoxBase(mb), m_menu(mb.m_menu)
{
    m_answerId =  mb.m_answerId;
    m_rejectSnd = mb.m_rejectSnd;
    m_pos =       mb.m_pos;
    m_title     = mb.m_title;
    m_titleSize = mb.m_titleSize;
}


void PGE_MenuBoxBase::construct(std::string _title, PGE_MenuBoxBase::msgType _type,
                                PGE_Point pos, double _padding, std::string texture)
{
    if(!texture.empty())
        loadTexture(texture);

    m_fontRgba.setRgba(1.0, 1.0, 1.0, 1.0);
    m_borderWidth = ConfigManager::setup_menu_box.borderWidth;

    updateTickValue();
    PGE_BoxBase::restart();

    m_answerId = PGE_Menu::npos;
    m_pos = pos;
    m_menu.setTextLenLimit(30, true);
    setTitleFont(ConfigManager::setup_menu_box.title_font_name);
    setTitleFontColor(ConfigManager::setup_menu_box.title_font_rgba);
    /****************Word wrap*********************/
    m_title = _title;
    FontManager::optimizeText(m_title, 27);
    m_titleSize = FontManager::textSize(_title, m_fontID, 27);
    /****************Word wrap*end*****************/
    setPadding(_padding);
    setType(_type);
    updateSize();
}

void PGE_MenuBoxBase::setTitleFont(std::string fontName)
{
    m_fontID   = FontManager::getFontID(std::move(fontName));
}

void PGE_MenuBoxBase::setTitleFontColor(GlColor color)
{
    m_fontRgba = color;
}

void PGE_MenuBoxBase::setTitleText(std::string text)
{
    m_title = text;
    FontManager::optimizeText(m_title, 27);
    m_titleSize = FontManager::textSize(text, m_fontID, 27);
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
    m_padding = _padding;
}

void PGE_MenuBoxBase::setPos(double x, double y)
{
    m_pos.setX(static_cast<int>(x));
    m_pos.setY(static_cast<int>(y));
    updateSize();
}

void PGE_MenuBoxBase::setMaxMenuItems(size_t items)
{
    m_menu.setItemsNumber(items);
    updateSize();
}

void PGE_MenuBoxBase::updateSize()
{
    PGE_Rect menuRect = m_menu.rectFull();

    if(menuRect.width() > m_titleSize.w())
        m_width = std::round(menuRect.width() / 2.0);
    else
        m_width = std::round(m_titleSize.w() / 2.0);

    m_height = std::round((m_titleSize.h() + menuRect.height()) / 2.0);
    int pad = static_cast<int>(m_padding);
    int w = static_cast<int>(m_width);
    int h = static_cast<int>(m_height);

    if((m_pos.x() == -1) && (m_pos.y() == -1))
    {
        m_sizeRect.setLeft(PGE_Window::Width / 2 - w - pad);
        m_sizeRect.setTop(PGE_Window::Height / 2 - h - pad);
        m_sizeRect.setRight(PGE_Window::Width / 2 + w + pad);
        m_sizeRect.setBottom(PGE_Window::Height / 2 + h + pad);

        if(m_sizeRect.top() < m_padding)
            m_sizeRect.setY(pad);
    }
    else
    {
        m_sizeRect.setLeft(m_pos.x() - w - pad);
        m_sizeRect.setTop(m_pos.y() - h - pad);
        m_sizeRect.setRight(m_pos.x() + w + pad);
        m_sizeRect.setBottom(m_pos.y() + h + pad);
    }

    m_menu.setPos(m_sizeRect.right() - (menuRect.width() > m_titleSize.w() ? menuRect.width() : m_titleSize.w()),
                 m_sizeRect.bottom() - menuRect.height() + m_menu.topOffset() - pad);
}



void PGE_MenuBoxBase::clearMenu()
{
    m_menu.clear();
    updateSize();
}

void PGE_MenuBoxBase::addMenuItem(std::string& menuitem)
{
    m_menu.addMenuItem(menuitem, menuitem);
    updateSize();
}

void PGE_MenuBoxBase::addMenuItems(std::vector<std::string> &menuItems)
{
    for(std::string &menuitem : menuItems)
        m_menu.addMenuItem(menuitem, menuitem);

    updateSize();
}

void PGE_MenuBoxBase::render()
{
    if(m_page == PageRunning)
    {
        if(m_textureUsed)
            drawTexture(m_sizeRect, m_borderWidth, static_cast<float>(m_faderOpacity));
        else
        {
            GlRenderer::renderRect(m_sizeRect.left(), m_sizeRect.top(),
                                   m_sizeRect.width(), m_sizeRect.height(),
                                   m_bgColor.Red(),
                                   m_bgColor.Green(),
                                   m_bgColor.Blue(),
                                   static_cast<float>(m_faderOpacity));
        }

        FontManager::printText(m_title,
                               m_sizeRect.center().x() - m_titleSize.w() / 2,
                               m_sizeRect.top() + static_cast<int>(m_padding),
                               m_fontID,
                               m_fontRgba.Red(), m_fontRgba.Green(), m_fontRgba.Blue(), m_fontRgba.Alpha());
        m_menu.render();
        //        FontManager::SDL_string_render2D(m_sizeRect.left()+padding,
        //                                         m_sizeRect.top()+padding,
        //                                         &textTexture);
    }
    else
    {
        if(m_textureUsed)
        {
            drawTexture(m_sizeRect.center().x() - static_cast<int>((m_width + m_padding)*m_faderOpacity),
                        m_sizeRect.center().y() - static_cast<int>((m_height + m_padding)*m_faderOpacity),
                        m_sizeRect.center().x() + static_cast<int>((m_width + m_padding)*m_faderOpacity),
                        m_sizeRect.center().y() + static_cast<int>((m_height + m_padding)*m_faderOpacity),
                        m_borderWidth, static_cast<float>(m_faderOpacity));
        }
        else
        {
            GlRenderer::renderRectBR(m_sizeRect.center().x() - static_cast<int>((m_width + m_padding)*m_faderOpacity),
                                     m_sizeRect.center().y() - static_cast<int>((m_height + m_padding)*m_faderOpacity),
                                     m_sizeRect.center().x() + static_cast<int>((m_width + m_padding)*m_faderOpacity),
                                     m_sizeRect.center().y() + static_cast<int>((m_height + m_padding)*m_faderOpacity),
                                     m_bgColor.Red(),
                                     m_bgColor.Green(),
                                     m_bgColor.Blue(), static_cast<float>(m_faderOpacity));
        }
    }
}

void PGE_MenuBoxBase::restart()
{
    PGE_BoxBase::restart();
    m_menu.resetState();
    m_menu.reset();
}

void PGE_MenuBoxBase::setRejectSnd(long sndRole)
{
    auto _sndRole =  static_cast<obj_sound_role::roles>(sndRole);
    m_rejectSnd = ConfigManager::getSoundByRole(_sndRole);
}

size_t PGE_MenuBoxBase::answer()
{
    return m_answerId;
}

void PGE_MenuBoxBase::reject()
{
    m_answerId = PGE_Menu::npos;
    nextPage();
    setFade(10, 0.0, 0.05);

    if(m_rejectSnd > 0)
        PGE_Audio::playSound(m_rejectSnd);
}

void PGE_MenuBoxBase::processKeyEvent(SDL_Keycode &key)
{
    if(m_page != PageRunning)
        return;

    D_pLogDebug("== MenuBox key %d from ::processKeyEvent ==", key);

    if(m_keys.any_key_pressed)
    {
        D_pLogDebug("== MenuBox key %d from ::processKeyEvent [REJECTED by controller] ==", key);
        return;
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
    case SDLK_AC_BACK:
        onDropButton();
        return;

    default:
        break;
    }
}

void PGE_MenuBoxBase::processController()
{
    if(m_page != PageRunning)
        return;

    if(!m_keys.any_key_pressed)
        return; // Nothing to do

    D_pLogDebugNA("== MenuBox key from controller ==");

    if(m_keys.up_pressed)
    {
        onUpButton();
        return;
    }
    else if(m_keys.down_pressed)
    {
        onDownButton();
        return;
    }
    else if(m_keys.left_pressed)
    {
        onLeftButton();
        return;
    }
    else if(m_keys.right_pressed)
    {
        onRightButton();
        return;
    }
    else if(m_keys.jump_pressed)
    {
        onJumpButton();
        return;
    }
    else if(m_keys.alt_jump_pressed)
    {
        onAltJumpButton();
        return;
    }
    else if(m_keys.run_pressed)
    {
        onRunButton();
        return;
    }
    else if(m_keys.alt_run_pressed)
    {
        onAltRunButton();
        return;
    }
    else if(m_keys.start_pressed)
    {
        onStartButton();
        return;
    }
    else if(m_keys.drop_pressed)
    {
        onDropButton();
        return;
    }
}

void PGE_MenuBoxBase::processBox(double tickDelay)
{
    PGE_BoxBase::processBox(tickDelay);
    processController();

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
            D_pLogDebug("== MenuBox key %d from poll Event ==", event.key.keysym.sym);
            processKeyEvent(event.key.keysym.sym);
            break;

        default:
            break;
        }
    }

    if(m_menu.isSelected())
    {
        if(m_menu.isAccepted())
            m_answerId = m_menu.currentItemI();
        else
            m_answerId = PGE_Menu::npos;

        nextPage();
        setFade(10, 0.0, 0.05);
        return;
    }
}

