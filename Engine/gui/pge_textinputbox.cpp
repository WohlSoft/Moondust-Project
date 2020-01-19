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

#include "pge_textinputbox.h"

#include "../fontman/font_manager.h"
#include "../graphics/window.h"

#include "../data_configs/config_select_scene/scene_config_select.h"
#include "../scenes/scene_level.h"
#include "../scenes/scene_world.h"
#include "../scenes/scene_title.h"
#include "../scenes/scene_gameover.h"

#include <common_features/app_path.h>
#include <common_features/logger.h>
#include <audio/pge_audio.h>
#include <settings/global_settings.h>

#include <utility>


PGE_TextInputBox::PGE_TextInputBox()
    : PGE_BoxBase(nullptr)
{
    message = "Message box works fine!";
    construct(message, m_type);
}

PGE_TextInputBox::PGE_TextInputBox(Scene *_parentScene, std::string msg, msgType _type,
                                   PGE_Point boxCenterPos,
                                   double _padding, std::string texture)
    : PGE_BoxBase(_parentScene)
{
    construct(std::move(msg), _type, boxCenterPos, _padding, std::move(texture));
}

PGE_TextInputBox::PGE_TextInputBox(const PGE_TextInputBox &mb)
    : PGE_BoxBase(mb)
{
    message = mb.message;
    m_blinkShown = mb.m_blinkShown;
    m_blinkTimeout = mb.m_blinkTimeout;
    m_cursor = mb.m_cursor;
    m_selectionLength = mb.m_selectionLength;
    m_textInput_h_offset = mb.m_textInput_h_offset;
}


void PGE_TextInputBox::construct(std::string msg, PGE_TextInputBox::msgType _type, PGE_Point pos, double _padding, std::string texture)
{
    loadTexture(std::move(texture));
    updateTickValue();
    PGE_BoxBase::restart();
    message = std::move(msg);
    setType(_type);

    m_cursor = 0;
    m_selectionLength = 0;
    m_blinkShown = true;
    m_blinkTimeout = 250.0;

    m_fontID   = FontManager::getFontID(ConfigManager::setup_message_box.font_name);
    m_fontRgba = ConfigManager::setup_message_box.font_rgba;
    m_borderWidth = ConfigManager::setup_message_box.borderWidth;

    if(_padding < 0)
        _padding = ConfigManager::setup_message_box.box_padding;

    /****************Word wrap*********************/
    FontManager::optimizeText(message, 27);
    /****************Word wrap*end*****************/
    PGE_Size boxSize = FontManager::textSize(message, m_fontID, 27);
    std::string w27 = "XXXXXXXXXXXXXXXXXXXXXXXXXXX";
    PGE_Size textinputSize = FontManager::textSize(w27, m_fontID, 27);
    m_textInput_h_offset = boxSize.h();
    boxSize.setWidth(textinputSize.w());
    boxSize.setHeight(static_cast<int>(boxSize.h() + textinputSize.h() + _padding));
    setBoxSize(boxSize.w() / 2, boxSize.h() / 2, _padding);

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

void PGE_TextInputBox::updatePrintable()
{
    size_t len = FontManager::utf8_strlen(m_inputText);
    if(len > 25)
        m_inputTextPrintable = FontManager::utf8_substr(m_inputText, len - 25, 25);
    else
        m_inputTextPrintable = m_inputText;
}

void PGE_TextInputBox::render()
{
    if(m_page == PageRunning)
    {
        if(m_textureUsed)
            drawTexture(m_sizeRect, m_borderWidth, static_cast<float>(m_faderOpacity));
        else
        {
            GlRenderer::renderRect(m_sizeRect.left(), m_sizeRect.top(),
                                   m_sizeRect.width(), m_sizeRect.height(),
                                   m_bgColor.Red(), m_bgColor.Green(), m_bgColor.Blue(), static_cast<float>(m_faderOpacity));
        }

        FontManager::printText(message, static_cast<int>(m_sizeRect.left() + m_padding),
                               static_cast<int>(m_sizeRect.top() + m_padding), m_fontID,
                               m_fontRgba.Red(), m_fontRgba.Green(), m_fontRgba.Blue(), m_fontRgba.Alpha());
        FontManager::printText(m_inputTextPrintable + (m_blinkShown ? "_" : ""),
                               static_cast<int>(m_sizeRect.left() + m_padding),
                               static_cast<int>(m_sizeRect.top() + m_textInput_h_offset + m_padding * 2), m_fontID,
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
                        m_borderWidth, static_cast<float>(m_faderOpacity));
        }
        else
        {
            GlRenderer::renderRectBR(m_sizeRect.center().x() - (m_width + m_padding)*m_faderOpacity ,
                                     m_sizeRect.center().y() - (m_height + m_padding)*m_faderOpacity,
                                     m_sizeRect.center().x() + (m_width + m_padding)*m_faderOpacity,
                                     m_sizeRect.center().y() + (m_height + m_padding)*m_faderOpacity,
                                     m_bgColor.Red(), m_bgColor.Green(), m_bgColor.Blue(), static_cast<float>(m_faderOpacity));
        }
    }
}

void PGE_TextInputBox::restart()
{
    PGE_BoxBase::restart();
    PGE_Audio::playSoundByRole(obj_sound_role::MenuMessageBox);
}

void PGE_TextInputBox::processBox(double tickTime)
{
    PGE_BoxBase::processBox(tickTime);

    bool wasExitKeyPress = m_keys.run_pressed || m_keys.alt_run_pressed || m_keys.start_pressed;
    if(wasExitKeyPress)
    {
        if(m_keys.start_pressed || m_keys.jump_pressed || m_keys.alt_jump_pressed)
            m_inputTextSrc = m_inputText;
        nextPage();
        setFade(10, 0.0, 0.05);
        SDL_StopTextInput();
        return;
    }

    m_blinkTimeout -= tickTime;
    if(m_blinkTimeout < 0.0)
    {
        m_blinkShown = !m_blinkShown;
        m_blinkTimeout += (tickTime < 250.0) ? 250.0 : tickTime + 250.0;
    }

    SDL_StartTextInput();
    SDL_Event event;

    while(SDL_PollEvent(&event))
    {
        PGE_Window::processEvents(event);

        switch(event.type)
        {
        case SDL_QUIT:
            nextPage();
            setFade(10, 0.0, 0.05);
            SDL_StopTextInput();
            break;

        case SDL_KEYDOWN: // If pressed key
        {
            switch(event.key.keysym.sym)
            {
            // Check which
            case SDLK_ESCAPE: // ESC
            case SDLK_RETURN:// Enter
            case SDLK_KP_ENTER:
            case SDLK_AC_BACK://Android "back" key
            {
                if(event.key.keysym.sym != SDLK_ESCAPE && event.key.keysym.sym != SDLK_AC_BACK)
                    m_inputTextSrc = m_inputText;
                nextPage();
                setFade(10, 0.0, 0.05);
                SDL_StopTextInput();
                break;
            }

            case SDLK_BACKSPACE:
            {
                if(m_inputText.length() > 0)
                {
                    FontManager::utf8_pop_back(m_inputText);
                    updatePrintable();
                }
                break;
            }

            default:
                break;
            }

            switch(event.key.keysym.scancode)
            {
            case SDL_SCANCODE_V:
            {
                if((event.key.keysym.mod & KMOD_CTRL) && (SDL_HasClipboardText() == SDL_TRUE))
                {
                    m_inputText.append(SDL_GetClipboardText());
                    std::remove(m_inputText.begin(), m_inputText.end(), '\r');
                    std::replace(m_inputText.begin(), m_inputText.end(), '\n', ' ');
                    updatePrintable();
                }
                break;
            }
            default:
                break;
            }

            break;
        }

        case SDL_TEXTINPUT:
        {
            D_pLogDebug("TEXT INPUT EVENT %s", event.text.text);
            m_inputText.append(event.text.text);
            updatePrintable();
        }
        break;

        case SDL_TEXTEDITING:
        {
            D_pLogDebug("TEXT EDIT EVENT start %d, %s", event.edit.start, event.edit.text);
            m_inputText      = event.edit.text;
            m_cursor          = event.edit.start;
            m_selectionLength   = event.edit.length;
            updatePrintable();
        }
        break;

        //            case SDL_MOUSEBUTTONDOWN:
        //                switch(event.button.button)
        //                {
        //                    case SDL_BUTTON_LEFT:
        //                    {
        //                        _page++;
        //                        setFade(10, 0.0f, 0.05f);
        //                    }
        //                    break;
        //                }
        //            break;
        default:
            break;
        }
    }
}

void PGE_TextInputBox::setInputText(std::string text)
{
    m_inputTextSrc = text;
    m_inputText = text;
    m_inputTextPrintable = m_inputText;

    if(m_inputTextPrintable.size() > 25)
        m_inputTextPrintable.erase(0, 25);
}

std::string PGE_TextInputBox::inputText()
{
    return m_inputTextSrc;
}
