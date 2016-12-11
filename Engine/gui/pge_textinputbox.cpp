/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2016 Vitaly Novichkov <admin@wohlnet.ru>
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

#include "pge_textinputbox.h"

#include "../fontman/font_manager.h"
#include "../graphics/window.h"

#include "../scenes/scene_level.h"
#include "../scenes/scene_world.h"

#include <common_features/app_path.h>
#include <audio/pge_audio.h>
#include <settings/global_settings.h>

#include <QFontMetrics>
#include <QFileInfo>

PGE_TextInputBox::PGE_TextInputBox()
    : PGE_BoxBase(0)
{
    width = 0;
    height = 0;
    message = "Message box works fine!";
    construct(message, type);
}

PGE_TextInputBox::PGE_TextInputBox(Scene *_parentScene, QString msg, msgType _type,
                                   PGE_Point boxCenterPos,
                                   double _padding, QString texture)
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
    _sizeRect = mb._sizeRect;
    message = mb.message;
    width   = mb.width;
    height  = mb.height;
    padding = mb.padding;
    bg_color = mb.bg_color;
    blink_shown = mb.blink_shown;
    blink_timeout = mb.blink_timeout;
}


void PGE_TextInputBox::construct(QString msg, PGE_TextInputBox::msgType _type, PGE_Point pos, double _padding, QString texture)
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
    blink_timeout = 250.0f;
    keys = Controller::noKeys();
    fontID   = FontManager::getFontID(ConfigManager::setup_message_box.font_name);
    fontRgba = ConfigManager::setup_message_box.font_rgba;

    if(_padding < 0)
        _padding = ConfigManager::setup_message_box.box_padding;

    switch(type)
    {
    case msg_info:
        bg_color =       QColor(qRgb(0, 0, 0));
        break;

    case msg_info_light:
        bg_color = QColor(qRgb(0, 0, 125));
        break;

    case msg_warn:
        bg_color =       QColor(qRgb(255, 201, 14));
        break;

    case msg_error:
        bg_color =      QColor(qRgb(125, 0, 0));
        break;

    case msg_fatal:
        bg_color =      QColor(qRgb(255, 0, 0));
        break;

    default:
        bg_color =            QColor(qRgb(0, 0, 0));
        break;
    }

    /****************Word wrap*********************/
    FontManager::optimizeText(message, 27);
    /****************Word wrap*end*****************/
    PGE_Size boxSize = FontManager::textSize(message, fontID, 27);
    QString w27 = "XXXXXXXXXXXXXXXXXXXXXXXXXXX";
    PGE_Size textinputSize = FontManager::textSize(w27, fontID, 27);
    _text_input_h_offset = boxSize.h();
    boxSize.setWidth(textinputSize.w());
    boxSize.setHeight(boxSize.h() + textinputSize.h() + _padding);
    setBoxSize(boxSize.w() / 2, boxSize.h() / 2, _padding);

    if((pos.x() == -1) && (pos.y() == -1))
    {
        _sizeRect.setLeft(PGE_Window::Width / 2 - width - padding);
        _sizeRect.setTop(PGE_Window::Height / 3 - height - padding);
        _sizeRect.setRight(PGE_Window::Width / 2 + width + padding);
        _sizeRect.setBottom(PGE_Window::Height / 3 + height + padding);

        if(_sizeRect.top() < padding)
            _sizeRect.setY(padding);
    }
    else
    {
        _sizeRect.setLeft(pos.x() - width - padding);
        _sizeRect.setTop(pos.y() - height - padding);
        _sizeRect.setRight(pos.x() + width + padding);
        _sizeRect.setBottom(pos.y() + height + padding);
    }
}

PGE_TextInputBox::~PGE_TextInputBox()
{}

void PGE_TextInputBox::setBoxSize(double _Width, double _Height, double _padding)
{
    width = _Width;
    height = _Height;
    padding = _padding;
}

void PGE_TextInputBox::update(double ticktime)
{
    switch(_page)
    {
    case 0:
        setFade(10, 1.0f, 0.05f);
        _page++;
        break;

    case 1:
        processLoader(ticktime);
        break;

    case 2:
        processBox(ticktime);
        break;

    case 3:
        processUnLoader(ticktime);
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
            drawTexture(_sizeRect, 32, m_faderOpacity);
        else
        {
            GlRenderer::renderRect(_sizeRect.left(), _sizeRect.top(),
                                   _sizeRect.width(), _sizeRect.height(),
                                   bg_color.red() / 255.0f, bg_color.green() / 255.0f, bg_color.blue() / 255.0f, m_faderOpacity);
        }

        FontManager::printText(message, _sizeRect.left() + padding, _sizeRect.top() + padding, fontID,
                               fontRgba.Red(), fontRgba.Green(), fontRgba.Blue(), fontRgba.Alpha());
        FontManager::printText(_inputText_printable + (blink_shown ? "_" : ""), _sizeRect.left() + padding, _sizeRect.top() + _text_input_h_offset + padding * 2, fontID,
                               fontRgba.Red(), fontRgba.Green(), fontRgba.Blue(), fontRgba.Alpha());
    }
    else
    {
        if(m_textureUsed)
        {
            drawTexture(_sizeRect.center().x() - (width + padding)*m_faderOpacity,
                        _sizeRect.center().y() - (height + padding)*m_faderOpacity,
                        _sizeRect.center().x() + (width + padding)*m_faderOpacity,
                        _sizeRect.center().y() + (height + padding)*m_faderOpacity,
                        32, m_faderOpacity);
        }
        else
        {
            GlRenderer::renderRectBR(_sizeRect.center().x() - (width + padding)*m_faderOpacity ,
                                     _sizeRect.center().y() - (height + padding)*m_faderOpacity,
                                     _sizeRect.center().x() + (width + padding)*m_faderOpacity,
                                     _sizeRect.center().y() + (height + padding)*m_faderOpacity,
                                     bg_color.red() / 255.0f, bg_color.green() / 255.0f, bg_color.blue() / 255.0f, m_faderOpacity);
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
            assert(delay < 2000u);
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

    if(m_faderOpacity >= 1.0f) _page++;
}

void PGE_TextInputBox::processBox(double tickTime)
{
    //    #ifndef __APPLE__
    //    if(g_AppSettings.interprocessing)
    //        qApp->processEvents();
    //    #endif
    updateControllers();
    blink_timeout -= tickTime;

    if(blink_timeout < 0.0f)
    {
        blink_shown = !blink_shown;
        blink_timeout += (tickTime < 250.0f) ? 250.0f : tickTime + 250.0f;
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
            setFade(10, 0.0f, 0.05f);
            SDL_StopTextInput();
            break;

        case SDL_KEYDOWN: // If pressed key
            switch(event.key.keysym.sym)
            {
            // Check which
            case SDLK_ESCAPE: // ESC
            case SDLK_RETURN:// Enter
            case SDLK_KP_ENTER:
            {
                if(event.key.keysym.sym != SDLK_ESCAPE)
                    _inputText_src = _inputText;

                _page++;
                setFade(10, 0.0f, 0.05f);
                SDL_StopTextInput();
            }
            break;

            case SDLK_BACKSPACE:
                if(_inputText.length() > 0)
                {
                    //lop off character
                    _inputText.remove(_inputText.size() - 1, 1);
                    _inputText_printable = _inputText;

                    if(_inputText_printable.size() > 25) _inputText_printable.remove(0, _inputText_printable.size() - 26);
                }

            default:
                break;
            }

            break;

        case SDL_TEXTINPUT:
        {
            _inputText.append(event.text.text);
            _inputText_printable = _inputText;

            if(_inputText_printable.size() > 25) _inputText_printable.remove(0, _inputText_printable.size() - 26);
        }
        break;

        case SDL_TEXTEDITING:
        {
            _inputText = event.edit.text;
            cursor     = event.edit.start;
            selection_len = event.edit.length;
            _inputText_printable = _inputText;

            if(_inputText_printable.size() > 25) _inputText_printable.remove(0, _inputText_printable.size() - 26);
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

    if(m_faderOpacity <= 0.0f) _page++;
}

void PGE_TextInputBox::setInputText(QString text)
{
    _inputText_src = text;
    _inputText = text;
    _inputText_printable = _inputText;

    if(_inputText_printable.size() > 25) _inputText_printable.remove(0, _inputText_printable.size() - 26);
}

QString PGE_TextInputBox::inputText()
{
    return _inputText_src;
}


void PGE_TextInputBox::updateControllers()
{
    if(m_parentScene != nullptr)
    {
        if(m_parentScene->type() == Scene::Level)
        {
            LevelScene *s = dynamic_cast<LevelScene *>(m_parentScene);

            if(s)
            {
                s->tickAnimations(m_uTickf);
                s->m_fader.tickFader(m_uTickf);
                s->player1Controller->update();
                s->player1Controller->sendControls();
                s->player2Controller->update();
                s->player2Controller->sendControls();
                keys = s->player1Controller->keys;
            }
        }
        else if(m_parentScene->type() == Scene::World)
        {
            WorldScene *s = dynamic_cast<WorldScene *>(m_parentScene);

            if(s)
            {
                s->tickAnimations(m_uTickf);
                s->m_fader.tickFader(m_uTickf);
                s->player1Controller->update();
                s->player1Controller->sendControls();
                keys = s->player1Controller->keys;
            }
        }
    }
}
