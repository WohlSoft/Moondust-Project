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

#include "pge_msgbox.h"

#include "../fontman/font_manager.h"
#include "../graphics/window.h"

#include "../scenes/scene_level.h"
#include "../scenes/scene_world.h"

#include <common_features/app_path.h>
#include <common_features/tr.h>
#include <audio/pge_audio.h>
#include <settings/global_settings.h>

PGE_MsgBox::PGE_MsgBox()
    : PGE_BoxBase(0)
{
    width=0;
    height=0;
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
    construct(msg,_type, boxCenterPos, _padding, texture);
}

PGE_MsgBox::PGE_MsgBox(const PGE_MsgBox &mb)
    : PGE_BoxBase(mb)
{
    m_page   = mb.m_page;
    m_running = mb.m_running;
    fontID  = mb.fontID;
    fontRgba= mb.fontRgba;

    keys    = mb.keys;
    m_exitKeyLock = mb.m_exitKeyLock;

    m_type    = mb.m_type;
    m_sizeRect=mb.m_sizeRect;
    m_message = mb.m_message;
    width   = mb.width;
    height  = mb.height;
    padding = mb.padding;
    bg_color= mb.bg_color;
}


void PGE_MsgBox::construct(std::string msg,
                           PGE_MsgBox::msgType _type,
                           PGE_Point pos,
                           double _padding,
                           std::string texture)
{
    loadTexture(texture);

    updateTickValue();
    m_page = 0;
    m_message = msg;
    m_type = _type;
    m_running = false;
    m_exitKeyLock = true;

    keys = Controller::noKeys();

    fontID   = FontManager::getFontID(ConfigManager::setup_message_box.font_name);
    fontRgba = ConfigManager::setup_message_box.font_rgba;
    if(_padding < 0)
        _padding = ConfigManager::setup_message_box.box_padding;

    switch(m_type)
    {
        case msg_info: bg_color =       GlColor(0, 0, 0); break;
        case msg_info_light: bg_color = GlColor(0, 0, 0.490196078); break;
        case msg_warn: bg_color =       GlColor(1.0, 0.788235294, 0.054901961); break;
        case msg_error: bg_color =      GlColor(0.490196078, 0, 0); break;
        case msg_fatal: bg_color =      GlColor(1.0, 0, 0); break;
        default:  bg_color =            GlColor(0, 0, 0); break;
    }

    /****************Word wrap*********************/
    FontManager::optimizeText(m_message, 27);
    /****************Word wrap*end*****************/
    PGE_Size boxSize = FontManager::textSize(m_message, fontID, 27);
    setBoxSize(boxSize.w()/2, boxSize.h()/2, _padding);

    if((pos.x() == -1) && (pos.y() == -1))
    {
        m_sizeRect.setLeft(static_cast<int>(PGE_Window::Width/2 - width-padding));
        m_sizeRect.setTop(static_cast<int>(PGE_Window::Height/3 - height-padding));
        m_sizeRect.setRight(static_cast<int>(PGE_Window::Width/2 + width + padding));
        m_sizeRect.setBottom(static_cast<int>(PGE_Window::Height/3 + height + padding));

        if(m_sizeRect.top() < padding)
            m_sizeRect.setY(static_cast<int>(padding));
    }
    else
    {
        m_sizeRect.setLeft(static_cast<int>(pos.x() - width-padding));
        m_sizeRect.setTop(static_cast<int>(pos.y() - height-padding));
        m_sizeRect.setRight(static_cast<int>(pos.x() + width + padding));
        m_sizeRect.setBottom(static_cast<int>(pos.y() + height + padding));
    }
}

PGE_MsgBox::~PGE_MsgBox()
{}

void PGE_MsgBox::setBoxSize(double _Width, double _Height, double _padding)
{
    width = _Width;
    height = _Height;
    padding = _padding;
}

void PGE_MsgBox::update(double ticks)
{
    switch(m_page)
    {
        case 0: setFade(10, 1.0, 0.05); m_page++; break;
        case 1: processLoader(ticks); break;
        case 2: processBox(ticks); break;
        case 3: processUnLoader(ticks); break;
        case 4:
        default: m_running=false; break;
    }
}

void PGE_MsgBox::render()
{
    if(m_page==2)
    {
        if(m_textureUsed)
        {
            drawTexture(m_sizeRect, 32, static_cast<float>(m_faderOpacity));
        }
        else
        {
            GlRenderer::renderRect(m_sizeRect.left(), m_sizeRect.top(),
                                   m_sizeRect.width(), m_sizeRect.height(),
                                   bg_color.Red(), bg_color.Green(), bg_color.Blue(), static_cast<float>(m_faderOpacity));
        }
        FontManager::printText(m_message, m_sizeRect.left() + padding, m_sizeRect.top()+padding, fontID,
                               fontRgba.Red(), fontRgba.Green(), fontRgba.Blue(), fontRgba.Alpha());
//        FontManager::SDL_string_render2D(_sizeRect.left()+padding,
//                                         _sizeRect.top()+padding,
//                                         &textTexture);
    }
    else
    {
        if(m_textureUsed)
        {
            drawTexture(m_sizeRect.center().x()-(width + padding)*m_faderOpacity,
                        m_sizeRect.center().y()-(height + padding)*m_faderOpacity,
                        m_sizeRect.center().x()+(width + padding)*m_faderOpacity,
                        m_sizeRect.center().y()+(height + padding)*m_faderOpacity,
                        32,
                        static_cast<float>(m_faderOpacity));
        }
        else
        {
            GlRenderer::renderRectBR(m_sizeRect.center().x() - (width+padding)*m_faderOpacity ,
                                     m_sizeRect.center().y() - (height+padding)*m_faderOpacity,
                                     m_sizeRect.center().x() + (width+padding)*m_faderOpacity,
                                     m_sizeRect.center().y() + (height+padding)*m_faderOpacity,
                                    bg_color.Red(),
                                    bg_color.Green(),
                                    bg_color.Blue(),
                                    static_cast<float>(m_faderOpacity));
        }
    }
}

void PGE_MsgBox::restart()
{
    PGE_Audio::playSoundByRole(obj_sound_role::MenuMessageBox);
    m_running = true;
    m_page = 0;
}

bool PGE_MsgBox::isRunning()
{
    return m_running;
}

void PGE_MsgBox::exec()
{
    updateControllers();
    restart();
    while(m_running)
    {
        Uint32 start_render=SDL_GetTicks();

        update(m_uTickf);
        PGE_BoxBase::render();
        render();
        GlRenderer::flush();
        GlRenderer::repaint();

        if((!PGE_Window::vsync) && (m_uTick > static_cast<Sint32>(SDL_GetTicks() - start_render)))
            SDL_Delay(static_cast<Uint32>(m_uTick) - (SDL_GetTicks() - start_render) );
    }
}

void PGE_MsgBox::processLoader(double ticks)
{
    SDL_Event event;
    while ( SDL_PollEvent(&event) ) {
        PGE_Window::processEvents(event);
        if(event.type==SDL_QUIT)
            m_faderOpacity=1.0;
    }
    updateControllers();
    tickFader(ticks);

    if(m_faderOpacity>=1.0) m_page++;
}

void PGE_MsgBox::processBox(double)
{
//    #ifndef __APPLE__
//    if(g_AppSettings.interprocessing)
//        qApp->processEvents();
//    #endif
    updateControllers();

    if(m_exitKeyLock && !keys.jump && !keys.run && !keys.alt_run)
        m_exitKeyLock=false;

    if((!m_exitKeyLock)&&(keys.jump || keys.run || keys.alt_run))
    {
        m_page++;
        setFade(10, 0.0f, 0.05f);
        return;
    }

    SDL_Event event;
    while ( SDL_PollEvent(&event) )
    {
        PGE_Window::processEvents(event);
        switch(event.type)
        {
            case SDL_QUIT:
                m_page++; setFade(10, 0.0, 0.05);
            break;
            case SDL_KEYDOWN: // If pressed key
                switch(event.key.keysym.sym)
                { // Check which
                case SDLK_ESCAPE: // ESC
                case SDLK_RETURN:// Enter
                case SDLK_KP_ENTER:
                {
                    m_page++;
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
                        m_page++;
                        setFade(10, 0.0, 0.05);
                    }
                    break;
                }
            break;
            default:
              break;
        }
    }

}



void PGE_MsgBox::processUnLoader(double ticks)
{
    SDL_Event event;
    while ( SDL_PollEvent(&event) )
    {
        PGE_Window::processEvents(event);
        if(event.type == SDL_QUIT)
            m_faderOpacity = 0.0;
    }

    updateControllers();
    tickFader(ticks);
    if(m_faderOpacity <= 0.0) m_page++;
}


void PGE_MsgBox::updateControllers()
{
    if(m_parentScene != nullptr)
    {
        if(m_parentScene->type() == Scene::Level)
        {
            LevelScene * s = dynamic_cast<LevelScene *>(m_parentScene);
            if(s)
            {
                s->tickAnimations(m_uTickf);
                s->m_fader.tickFader(m_uTickf);
                s->m_player1Controller->update();
                s->m_player1Controller->sendControls();
                s->m_player2Controller->update();
                s->m_player2Controller->sendControls();
                keys=s->m_player1Controller->keys;
            }
        }
        else if(m_parentScene->type() == Scene::World)
        {
            WorldScene * s = dynamic_cast<WorldScene *>(m_parentScene);
            if(s)
            {
                s->tickAnimations(m_uTickf);
                s->m_fader.tickFader(m_uTickf);
                s->m_player1Controller->update();
                s->m_player1Controller->sendControls();
                keys=s->m_player1Controller->keys;
            }
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

