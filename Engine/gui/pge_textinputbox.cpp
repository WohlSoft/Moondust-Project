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

#include "pge_textinputbox.h"

#include "../fontman/font_manager.h"
#include "../graphics/window.h"

#include "../scenes/scene_level.h"
#include "../scenes/scene_world.h"

#include <common_features/app_path.h>
#include <common_features/logger.h>
#include <audio/pge_audio.h>
#include <settings/global_settings.h>

PGE_TextInputBox::PGE_TextInputBox()
    : PGE_BoxBase(nullptr)
{
    width = 0;
    height = 0;
    message = "Message box works fine!";
    construct(message, type);
}

PGE_TextInputBox::PGE_TextInputBox(Scene *_parentScene, std::string msg, msgType _type,
                                   PGE_Point boxCenterPos,
                                   double _padding, std::string texture)
    : PGE_BoxBase(_parentScene)
{
    construct(msg, _type, boxCenterPos, _padding, texture);
}

PGE_TextInputBox::PGE_TextInputBox(const PGE_TextInputBox &mb)
    : PGE_BoxBase(mb)
{
    _page   = mb._page;
    running = mb.running;
    fontID  = mb.fontID;
    fontRgba = mb.fontRgba;
    keys    = mb.keys;
    type    = mb.type;
    m_sizeRect = mb.m_sizeRect;
    message = mb.message;
    width   = mb.width;
    height  = mb.height;
    padding = mb.padding;
    bg_color = mb.bg_color;
    blink_shown = mb.blink_shown;
    blink_timeout = mb.blink_timeout;
    cursor = mb.cursor;
    selection_len = mb.selection_len;
    _text_input_h_offset = mb._text_input_h_offset;
}


void PGE_TextInputBox::construct(std::string msg, PGE_TextInputBox::msgType _type, PGE_Point pos, double _padding, std::string texture)
{
    loadTexture(texture);
    updateTickValue();
    _page = 0;
    message = msg;
    type = _type;
    running = false;
    cursor = 0;
    selection_len = 0;
    blink_shown = true;
    blink_timeout = 250.0;
    keys = Controller::noKeys();
    fontID   = FontManager::getFontID(ConfigManager::setup_message_box.font_name);
    fontRgba = ConfigManager::setup_message_box.font_rgba;
    m_borderWidth = ConfigManager::setup_message_box.borderWidth;

    if(_padding < 0)
        _padding = ConfigManager::setup_message_box.box_padding;

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

    /****************Word wrap*********************/
    FontManager::optimizeText(message, 27);
    /****************Word wrap*end*****************/
    PGE_Size boxSize = FontManager::textSize(message, fontID, 27);
    std::string w27 = "XXXXXXXXXXXXXXXXXXXXXXXXXXX";
    PGE_Size textinputSize = FontManager::textSize(w27, fontID, 27);
    _text_input_h_offset = boxSize.h();
    boxSize.setWidth(textinputSize.w());
    boxSize.setHeight(static_cast<int>(boxSize.h() + textinputSize.h() + _padding));
    setBoxSize(boxSize.w() / 2, boxSize.h() / 2, _padding);

    if((pos.x() == -1) && (pos.y() == -1))
    {
        m_sizeRect.setLeft(static_cast<int>(PGE_Window::Width / 2 - width - padding));
        m_sizeRect.setTop(static_cast<int>(PGE_Window::Height / 3 - height - padding));
        m_sizeRect.setRight(static_cast<int>(PGE_Window::Width / 2 + width + padding));
        m_sizeRect.setBottom(static_cast<int>(PGE_Window::Height / 3 + height + padding));

        if(m_sizeRect.top() < padding)
            m_sizeRect.setY(static_cast<int>(padding));
    }
    else
    {
        m_sizeRect.setLeft(static_cast<int>(pos.x() - width - padding));
        m_sizeRect.setTop(static_cast<int>(pos.y() - height - padding));
        m_sizeRect.setRight(static_cast<int>(pos.x() + width + padding));
        m_sizeRect.setBottom(static_cast<int>(pos.y() + height + padding));
    }
}

void PGE_TextInputBox::updatePrintable()
{
    size_t len = FontManager::utf8_strlen(_inputText);
    if(len > 25)
        _inputText_printable = FontManager::utf8_substr(_inputText, len - 25, 25);
    else
        _inputText_printable = _inputText;
}

void PGE_TextInputBox::setBoxSize(double _Width, double _Height, double _padding)
{
    width = _Width;
    height = _Height;
    padding = _padding;
}

void PGE_TextInputBox::update(double tickTime)
{
    switch(_page)
    {
    case 0:
        setFade(10, 1.0, 0.05);
        _page++;
        break;

    case 1:
        processLoader(tickTime);
        break;

    case 2:
        processBox(tickTime);
        break;

    case 3:
        processUnLoader(tickTime);
        break;

    case 4:
    default:
        running = false;
        break;
    }
}

void PGE_TextInputBox::render()
{
    if(_page == 2)
    {
        if(m_textureUsed)
            drawTexture(m_sizeRect, m_borderWidth, static_cast<float>(m_faderOpacity));
        else
        {
            GlRenderer::renderRect(m_sizeRect.left(), m_sizeRect.top(),
                                   m_sizeRect.width(), m_sizeRect.height(),
                                   bg_color.Red(), bg_color.Green(), bg_color.Blue(), static_cast<float>(m_faderOpacity));
        }

        FontManager::printText(message, static_cast<int>(m_sizeRect.left() + padding),
                               static_cast<int>(m_sizeRect.top() + padding), fontID,
                               fontRgba.Red(), fontRgba.Green(), fontRgba.Blue(), fontRgba.Alpha());
        FontManager::printText(_inputText_printable + (blink_shown ? "_" : ""),
                               static_cast<int>(m_sizeRect.left() + padding),
                               static_cast<int>(m_sizeRect.top() + _text_input_h_offset + padding * 2), fontID,
                               fontRgba.Red(), fontRgba.Green(), fontRgba.Blue(), fontRgba.Alpha());
    }
    else
    {
        if(m_textureUsed)
        {
            drawTexture(static_cast<int>(m_sizeRect.center().x() - (width + padding) * m_faderOpacity),
                        static_cast<int>(m_sizeRect.center().y() - (height + padding) * m_faderOpacity),
                        static_cast<int>(m_sizeRect.center().x() + (width + padding) * m_faderOpacity),
                        static_cast<int>(m_sizeRect.center().y() + (height + padding) * m_faderOpacity),
                        m_borderWidth, static_cast<float>(m_faderOpacity));
        }
        else
        {
            GlRenderer::renderRectBR(m_sizeRect.center().x() - (width + padding)*m_faderOpacity ,
                                     m_sizeRect.center().y() - (height + padding)*m_faderOpacity,
                                     m_sizeRect.center().x() + (width + padding)*m_faderOpacity,
                                     m_sizeRect.center().y() + (height + padding)*m_faderOpacity,
                                     bg_color.Red(), bg_color.Green(), bg_color.Blue(), static_cast<float>(m_faderOpacity));
        }
    }
}

void PGE_TextInputBox::restart()
{
    PGE_Audio::playSoundByRole(obj_sound_role::MenuMessageBox);
    running = true;
    _page = 0;
}

bool PGE_TextInputBox::isRunning()
{
    return running;
}

void PGE_TextInputBox::exec()
{
    updateControllers();
    restart();

    while(running)
    {
        Uint32 start_render = SDL_GetTicks();
        update(m_uTickf);
        PGE_BoxBase::render();
        render();
        GlRenderer::flush();
        GlRenderer::repaint();

        if((!PGE_Window::vsync) && (m_uTick > static_cast<signed>(SDL_GetTicks() - start_render)))
        {
            Uint32 delay = static_cast<Uint32>(m_uTick) - (SDL_GetTicks() - start_render);
            SDL_assert(delay < 2000u);
            SDL_Delay(delay);
        }
    }
}

void PGE_TextInputBox::processLoader(double ticks)
{
    SDL_Event event;

    while(SDL_PollEvent(&event))
    {
        PGE_Window::processEvents(event);

        if(event.type == SDL_QUIT)
            m_faderOpacity = 1.0;
    }

    updateControllers();
    tickFader(ticks);

    if(m_faderOpacity >= 1.0)
        _page++;
}

void PGE_TextInputBox::processBox(double tickTime)
{
    //    #ifndef __APPLE__
    //    if(g_AppSettings.interprocessing)
    //        qApp->processEvents();
    //    #endif
    updateControllers();
    blink_timeout -= tickTime;
    if(blink_timeout < 0.0)
    {
        blink_shown = !blink_shown;
        blink_timeout += (tickTime < 250.0) ? 250.0 : tickTime + 250.0;
    }

    SDL_StartTextInput();
    SDL_Event event;

    while(SDL_PollEvent(&event))
    {
        PGE_Window::processEvents(event);

        switch(event.type)
        {
        case SDL_QUIT:
            _page++;
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
                    _inputText_src = _inputText;
                _page++;
                setFade(10, 0.0, 0.05);
                SDL_StopTextInput();
                break;
            }

            case SDLK_BACKSPACE:
            {
                if(_inputText.length() > 0)
                {
                    FontManager::utf8_pop_back(_inputText);
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
                    _inputText.append(SDL_GetClipboardText());
                    std::remove(_inputText.begin(), _inputText.end(), '\r');
                    std::replace(_inputText.begin(), _inputText.end(), '\n', ' ');
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
            _inputText.append(event.text.text);
            updatePrintable();
        }
        break;

        case SDL_TEXTEDITING:
        {
            D_pLogDebug("TEXT EDIT EVENT start %d, %s", event.edit.start, event.edit.text);
            _inputText      = event.edit.text;
            cursor          = event.edit.start;
            selection_len   = event.edit.length;
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



void PGE_TextInputBox::processUnLoader(double ticks)
{
    SDL_Event event;

    while(SDL_PollEvent(&event))
    {
        PGE_Window::processEvents(event);

        if(event.type == SDL_QUIT)
            m_faderOpacity = 0.0;
    }

    updateControllers();
    tickFader(ticks);

    if(m_faderOpacity <= 0.0)
        _page++;
}

void PGE_TextInputBox::setInputText(std::string text)
{
    _inputText_src = text;
    _inputText = text;
    _inputText_printable = _inputText;

    if(_inputText_printable.size() > 25)
        _inputText_printable.erase(0, 25);
}

std::string PGE_TextInputBox::inputText()
{
    return _inputText_src;
}


void PGE_TextInputBox::updateControllers()
{
    if(m_parentScene != nullptr)
    {
        if(m_parentScene->type() == Scene::Level)
        {
            auto *s = dynamic_cast<LevelScene *>(m_parentScene);
            if(s)
            {
                s->tickAnimations(m_uTickf);
                s->m_fader.tickFader(m_uTickf);
                s->m_player1Controller->update();
                s->m_player1Controller->sendControls();
                s->m_player2Controller->update();
                s->m_player2Controller->sendControls();
                keys = s->m_player1Controller->keys;
            }
        }
        else if(m_parentScene->type() == Scene::World)
        {
            auto *s = dynamic_cast<WorldScene *>(m_parentScene);
            if(s)
            {
                s->tickAnimations(m_uTickf);
                s->m_fader.tickFader(m_uTickf);
                s->m_player1Controller->update();
                s->m_player1Controller->sendControls();
                keys = s->m_player1Controller->keys;
            }
        }
    }
}
