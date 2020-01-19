/*
 * Moondust, a free game engine for platform game making
 * Copyright (c) 2014-2020 Vitaly Novichkov <admin@wohlnet.ru>
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

#include "pge_msgbox.h"

#include "../fontman/font_manager.h"
#include "../graphics/window.h"

#include "../data_configs/config_select_scene/scene_config_select.h"
#include "../scenes/scene_level.h"
#include "../scenes/scene_world.h"
#include "../scenes/scene_title.h"
#include "../scenes/scene_gameover.h"

#include <common_features/app_path.h>
#include <common_features/tr.h>
#include <audio/pge_audio.h>
#include <settings/global_settings.h>

#include <utility>

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

PGE_MsgBox::PGE_MsgBox()
    : PGE_BoxBase(nullptr)
{
    m_message = "Message box works fine!";
    construct(m_message, m_type);
}

PGE_MsgBox::PGE_MsgBox(Scene *_parentScene,
                       std::string msg,
                       msgType _type,
                       PGE_Point boxCenterPos,
                       double _padding,
                       std::string texture)
    : PGE_BoxBase(_parentScene)
{
    construct(std::move(msg), _type, boxCenterPos, _padding, std::move(texture));
}

PGE_MsgBox::PGE_MsgBox(const PGE_MsgBox &mb)
    : PGE_BoxBase(mb)
{
    m_exitKeyLock = mb.m_exitKeyLock;
    m_message = mb.m_message;
}


void PGE_MsgBox::construct(std::string msg,
                           PGE_MsgBox::msgType _type,
                           PGE_Point pos,
                           double _padding,
                           std::string texture)
{
    loadTexture(std::move(texture));

    updateTickValue();
    PGE_BoxBase::restart();

    m_message = std::move(msg);
    setType(_type);

    m_exitKeyLock = true;

    m_fontID   = FontManager::getFontID(ConfigManager::setup_message_box.font_name);
    m_fontRgba = ConfigManager::setup_message_box.font_rgba;
    m_borderWidth = ConfigManager::setup_message_box.borderWidth;

    if(_padding < 0)
        _padding = ConfigManager::setup_message_box.box_padding;

    /****************Word wrap*********************/
    FontManager::optimizeText(m_message, 27);
    /****************Word wrap*end*****************/
    PGE_Size boxSize = FontManager::textSize(m_message, m_fontID, 27);
    setBoxSize(boxSize.w() / 2, boxSize.h() / 2, _padding);

    D_pLogDebug("Message box title sizes: %d x %d (len: %zu)\n"
                "-------------\n"
                "%s\n"
                "-------------", boxSize.w(), boxSize.h(), m_message.size(), m_message.c_str());

    if((pos.x() == -1) && (pos.y() == -1))
    {
        m_sizeRect.setLeft(static_cast<int>(PGE_Window::Width / 2 - m_width - m_padding));
        m_sizeRect.setTop(static_cast<int>(PGE_Window::Height / 3 - m_height - m_padding));
        m_sizeRect.setRight(static_cast<int>(PGE_Window::Width / 2 + m_width + m_padding));
        m_sizeRect.setBottom(static_cast<int>(PGE_Window::Height / 3 + m_height + m_padding));

        if(m_sizeRect.top() < m_padding)
            m_sizeRect.setY(static_cast<int>(m_padding));
    }
    else
    {
        m_sizeRect.setLeft(static_cast<int>(pos.x() - m_width - m_padding));
        m_sizeRect.setTop(static_cast<int>(pos.y() - m_height - m_padding));
        m_sizeRect.setRight(static_cast<int>(pos.x() + m_width + m_padding));
        m_sizeRect.setBottom(static_cast<int>(pos.y() + m_height + m_padding));
    }
}

void PGE_MsgBox::render()
{
    if(m_page == 2)
    {
        if(m_textureUsed)
        {
            drawTexture(m_sizeRect, m_borderWidth, static_cast<float>(m_faderOpacity));
        }
        else
        {
            GlRenderer::renderRect(m_sizeRect.left(), m_sizeRect.top(),
                                   m_sizeRect.width(), m_sizeRect.height(),
                                   m_bgColor.Red(), m_bgColor.Green(), m_bgColor.Blue(), static_cast<float>(m_faderOpacity));
        }
        FontManager::printText(m_message,
                               static_cast<int>(m_sizeRect.left() + m_padding),
                               static_cast<int>(m_sizeRect.top() + m_padding), m_fontID,
                               m_fontRgba.Red(), m_fontRgba.Green(), m_fontRgba.Blue(), m_fontRgba.Alpha());
    }
    else
    {
        if(m_textureUsed)
        {
            drawTexture(static_cast<int>(m_sizeRect.center().x() - (m_width + m_padding) * m_faderOpacity),
                        static_cast<int>(m_sizeRect.center().y() - (m_height + m_padding) * m_faderOpacity),
                        static_cast<int>(m_sizeRect.center().x() + (m_width + m_padding) * m_faderOpacity),
                        static_cast<int>(m_sizeRect.center().y() + (m_height + m_padding) * m_faderOpacity),
                        m_borderWidth,
                        static_cast<float>(m_faderOpacity));
        }
        else
        {
            GlRenderer::renderRectBR(m_sizeRect.center().x() - (m_width + m_padding) * m_faderOpacity ,
                                     m_sizeRect.center().y() - (m_height + m_padding) * m_faderOpacity,
                                     m_sizeRect.center().x() + (m_width + m_padding) * m_faderOpacity,
                                     m_sizeRect.center().y() + (m_height + m_padding) * m_faderOpacity,
                                     m_bgColor.Red(),
                                     m_bgColor.Green(),
                                     m_bgColor.Blue(),
                                     static_cast<float>(m_faderOpacity));
        }
    }
}

void PGE_MsgBox::restart()
{
    PGE_BoxBase::restart();
    PGE_Audio::playSoundByRole(obj_sound_role::MenuMessageBox);
}

void PGE_MsgBox::processBox(double tickTime)
{
    PGE_BoxBase::processBox(tickTime);

    bool wasExitKeyPress = m_keys.jump_pressed || m_keys.run_pressed || m_keys.alt_run_pressed || m_keys.start_pressed;

    if(m_exitKeyLock && !wasExitKeyPress)
        m_exitKeyLock = false;

    if(!m_exitKeyLock && wasExitKeyPress)
    {
        nextPage();
        setFade(10, 0.0, 0.05);
        return;
    }

    SDL_Event event;
    while(SDL_PollEvent(&event))
    {
        PGE_Window::processEvents(event);
        switch(event.type)
        {
        case SDL_QUIT:
            nextPage();
            setFade(10, 0.0, 0.05);
            break;
        case SDL_KEYDOWN: // If pressed key
            switch(event.key.keysym.sym)
            {
            // Check which
            case SDLK_ESCAPE: // ESC
            case SDLK_RETURN:// Enter
            case SDLK_KP_ENTER:
            case SDLK_AC_BACK://Android "back" key
            {
                nextPage();
                setFade(10, 0.0, 0.05);
            }
            break;

            default:
                break;
            }
            break;
        case SDL_MOUSEBUTTONDOWN:
            switch(event.button.button)
            {
            case SDL_BUTTON_LEFT:
                {
                    nextPage();
                    setFade(10, 0.0, 0.05);
                }
                break;
            default:
                break;
            }
            break;
        default:
            break;
        }
    }

}


void PGE_MsgBox::info(std::string msg)
{
    if(GlRenderer::ready())
    {
        PGE_MsgBox msgBox(nullptr, msg,
                          PGE_MsgBox::msg_info_light);
        msgBox.exec();
    }
    else
    {
        //% "Information"
        PGE_Window::msgBoxInfo(qtTrId("MSGBOX_INFO"), msg);
    }
}

void PGE_MsgBox::warn(std::string msg)
{
    if(GlRenderer::ready())
    {
        PGE_MsgBox msgBox(nullptr, msg,
                          PGE_MsgBox::msg_warn);
        msgBox.exec();
    }
    else
    {
        //% "Warning"
        PGE_Window::msgBoxWarning(qtTrId("MSGBOX_WARN"), msg);
    }
}

void PGE_MsgBox::error(std::string msg)
{
    if(GlRenderer::ready())
    {
        PGE_MsgBox msgBox(nullptr, msg,
                          PGE_MsgBox::msg_error);
        msgBox.exec();
    }
    else
    {
        //% "Error"
        PGE_Window::msgBoxCritical(qtTrId("MSGBOX_ERROR"), msg);
    }
}

void PGE_MsgBox::fatal(std::string msg)
{
    if(GlRenderer::ready())
    {
        PGE_MsgBox msgBox(nullptr, msg,
                          PGE_MsgBox::msg_fatal);
        msgBox.exec();
    }
    else
    {
        //% "Fatal error"
        PGE_Window::msgBoxCritical(qtTrId("MSGBOX_FATAL"), msg);
    }
}

