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

#include "pge_msgbox.h"

#include "../fontman/font_manager.h"
#include "../graphics/window.h"

#include "../scenes/scene_level.h"
#include "../scenes/scene_world.h"

#include <common_features/app_path.h>
#include <audio/pge_audio.h>
#include <settings/global_settings.h>

#include <QFontMetrics>
#include <QMessageBox>
#include <QFileInfo>
#include <QTranslator>
#include <QApplication>

PGE_MsgBox::PGE_MsgBox()
    : PGE_BoxBase(0)
{
    width=0;
    height=0;
    message = "Message box works fine!";
    construct(message, type);
}

PGE_MsgBox::PGE_MsgBox(Scene *_parentScene, QString msg, msgType _type,
                       PGE_Point boxCenterPos,
                       float _padding, QString texture)
    : PGE_BoxBase(_parentScene)
{
    construct(msg,_type, boxCenterPos, _padding, texture);
}

PGE_MsgBox::PGE_MsgBox(const PGE_MsgBox &mb)
    : PGE_BoxBase(mb)
{
    _page   = mb._page;
    running = mb.running;
    fontID  = mb.fontID;
    fontRgba= mb.fontRgba;

    keys    = mb.keys;
    _exit_key_lock = mb._exit_key_lock;

    type    = mb.type;
    _sizeRect=mb._sizeRect;
    message = mb.message;
    width   = mb.width;
    height  = mb.height;
    padding = mb.padding;
    bg_color= mb.bg_color;
}


void PGE_MsgBox::construct(QString msg, PGE_MsgBox::msgType _type, PGE_Point pos, float _padding, QString texture)
{
    loadTexture(texture);

    updateTickValue();
    _page=0;
    message = msg;
    type = _type;
    running=false;
    _exit_key_lock=true;

    keys = Controller::noKeys();

    fontID   = FontManager::getFontID(ConfigManager::setup_message_box.font_name);
    fontRgba = ConfigManager::setup_message_box.font_rgba;
    if(_padding<0)
        _padding = ConfigManager::setup_message_box.box_padding;

    switch(type)
    {
        case msg_info: bg_color =       QColor(qRgb(0,0,0)); break;
        case msg_info_light: bg_color = QColor(qRgb(0,0,125)); break;
        case msg_warn: bg_color =       QColor(qRgb(255,201,14)); break;
        case msg_error: bg_color =      QColor(qRgb(125,0,0)); break;
        case msg_fatal: bg_color =      QColor(qRgb(255,0,0)); break;
        default:  bg_color =            QColor(qRgb(0,0,0)); break;
    }

    /****************Word wrap*********************/
    FontManager::optimizeText(message, 27);
    /****************Word wrap*end*****************/
    PGE_Size boxSize = FontManager::textSize(message, fontID, 27);
    setBoxSize(boxSize.w()/2, boxSize.h()/2, _padding);

    if((pos.x()==-1)&&(pos.y()==-1))
    {
        _sizeRect.setLeft(PGE_Window::Width/2-width-padding);
        _sizeRect.setTop(PGE_Window::Height/3-height-padding);
        _sizeRect.setRight(PGE_Window::Width/2 + width + padding);
        _sizeRect.setBottom(PGE_Window::Height/3+height + padding);
        if(_sizeRect.top() < padding)
            _sizeRect.setY(padding);
    }
    else
    {
        _sizeRect.setLeft( pos.x() - width-padding);
        _sizeRect.setTop(pos.y() - height-padding);
        _sizeRect.setRight(pos.x() + width + padding);
        _sizeRect.setBottom(pos.y() + height + padding);
    }
}

PGE_MsgBox::~PGE_MsgBox()
{}

void PGE_MsgBox::setBoxSize(float _Width, float _Height, float _padding)
{
    width = _Width;
    height = _Height;
    padding = _padding;
}

void PGE_MsgBox::update(float ticks)
{
    switch(_page)
    {
        case 0: setFade(10, 1.0f, 0.05f); _page++; break;
        case 1: processLoader(ticks); break;
        case 2: processBox(ticks); break;
        case 3: processUnLoader(ticks); break;
        case 4:
        default: running=false; break;
    }
}

void PGE_MsgBox::render()
{
    if(_page==2)
    {
        if(_textureUsed)
        {
            drawTexture(_sizeRect);
        }
        else
        {
            GlRenderer::renderRect(_sizeRect.left(), _sizeRect.top(),
                                   _sizeRect.width(), _sizeRect.height(),
                                   bg_color.red()/255.0f, bg_color.green()/255.0f, bg_color.blue()/255.0f, fader_opacity);
        }
        FontManager::printText(message, _sizeRect.left()+padding, _sizeRect.top()+padding, fontID,
                               fontRgba.Red(), fontRgba.Green(), fontRgba.Blue(), fontRgba.Alpha());
//        FontManager::SDL_string_render2D(_sizeRect.left()+padding,
//                                         _sizeRect.top()+padding,
//                                         &textTexture);
    }
    else
    {
        if(_textureUsed)
        {
            drawTexture(_sizeRect.center().x()-(width + padding)*fader_opacity,
                        _sizeRect.center().y()-(height + padding)*fader_opacity,
                        _sizeRect.center().x()+(width + padding)*fader_opacity,
                        _sizeRect.center().y()+(height + padding)*fader_opacity);
        }
        else
        {
            GlRenderer::renderRectBR(_sizeRect.center().x() - (width+padding)*fader_opacity ,
                                   _sizeRect.center().y() - (height+padding)*fader_opacity,
                                     _sizeRect.center().x() + (width+padding)*fader_opacity,
                                   _sizeRect.center().y() + (height+padding)*fader_opacity,
                                   bg_color.red()/255.0f, bg_color.green()/255.0f, bg_color.blue()/255.0f, fader_opacity);
        }
    }
}

void PGE_MsgBox::restart()
{
    PGE_Audio::playSoundByRole(obj_sound_role::MenuMessageBox);
    running=true;
    _page=0;
}

bool PGE_MsgBox::isRunning()
{
    return running;
}

void PGE_MsgBox::exec()
{
    updateControllers();
    restart();
    while(running)
    {
        Uint32 start_render=SDL_GetTicks();

        update(uTickf);
        PGE_BoxBase::render();
        render();
        glFlush();
        PGE_Window::rePaint();

        if((!PGE_Window::vsync) && (uTick > (signed)(SDL_GetTicks() - start_render)))
                SDL_Delay(uTick - (SDL_GetTicks()-start_render) );
    }
}

void PGE_MsgBox::processLoader(float ticks)
{
    SDL_Event event;
    while ( SDL_PollEvent(&event) ) {
        PGE_Window::processEvents(event);
        if(event.type==SDL_QUIT)
            fader_opacity=1.0;
    }
    updateControllers();
    tickFader(ticks);

    if(fader_opacity>=1.0f) _page++;
}

void PGE_MsgBox::processBox(float)
{
    #ifndef __APPLE__
    if(AppSettings.interprocessing)
        qApp->processEvents();
    #endif
    updateControllers();

    if(_exit_key_lock && !keys.jump && !keys.run && !keys.alt_run)
        _exit_key_lock=false;

    if((!_exit_key_lock)&&(keys.jump || keys.run || keys.alt_run))
    {
        _page++;
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
                _page++; setFade(10, 0.0f, 0.05f);
            break;
            case SDL_KEYDOWN: // If pressed key
                switch(event.key.keysym.sym)
                { // Check which
                case SDLK_ESCAPE: // ESC
                case SDLK_RETURN:// Enter
                case SDLK_KP_ENTER:
                {
                    _page++;
                    setFade(10, 0.0f, 0.05f);
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
                        _page++;
                        setFade(10, 0.0f, 0.05f);
                    }
                    break;
                }
            break;
            default:
              break;
        }
    }

}



void PGE_MsgBox::processUnLoader(float ticks)
{
    SDL_Event event;
    while ( SDL_PollEvent(&event) ) {
        PGE_Window::processEvents(event);
        if(event.type==SDL_QUIT)
            fader_opacity=0.0;
    }

    updateControllers();
    tickFader(ticks);
    if(fader_opacity<=0.0f) _page++;
}


void PGE_MsgBox::updateControllers()
{
    if(parentScene!=NULL)
    {
        if(parentScene->type()==Scene::Level)
        {
            LevelScene * s = dynamic_cast<LevelScene *>(parentScene);
            if(s)
            {
                s->tickAnimations(uTickf);
                s->fader.tickFader(uTickf);
                s->player1Controller->update();
                s->player1Controller->sendControls();
                s->player2Controller->update();
                s->player2Controller->sendControls();
                keys=s->player1Controller->keys;
            }
        }
        else if(parentScene->type()==Scene::World)
        {
            WorldScene * s = dynamic_cast<WorldScene *>(parentScene);
            if(s)
            {
                s->tickAnimations(uTickf);
                s->fader.tickFader(uTickf);
                s->player1Controller->update();
                s->player1Controller->sendControls();
                keys=s->player1Controller->keys;
            }
        }
    }
}



void PGE_MsgBox::info(QString msg)
{
    if(GlRenderer::ready())
    {
        PGE_MsgBox msgBox(NULL, msg,
                          PGE_MsgBox::msg_info_light);
        msgBox.exec();
    }
    else
    {
        QMessageBox::information(NULL, QTranslator::tr("Information"), msg, QMessageBox::Ok);
    }
}
//void PGE_MsgBox::info(std::string msg)
//{
//    PGE_MsgBox::info(QString::fromStdString(msg));
//}

void PGE_MsgBox::warn(QString msg)
{
    if(GlRenderer::ready())
    {
        PGE_MsgBox msgBox(NULL, msg,
                          PGE_MsgBox::msg_warn);
        msgBox.exec();
    }
    else
    {
        QMessageBox::warning(NULL, QTranslator::tr("Warning"), msg, QMessageBox::Ok);
    }
}
//void PGE_MsgBox::warn(std::string msg)
//{
//    PGE_MsgBox::warn(QString::fromStdString(msg));
//}


void PGE_MsgBox::error(QString msg)
{
    if(GlRenderer::ready())
    {
        PGE_MsgBox msgBox(NULL, msg,
                          PGE_MsgBox::msg_error);
        msgBox.exec();
    }
    else
    {
        QMessageBox::critical(NULL, QTranslator::tr("Error"), msg, QMessageBox::Ok);
    }
}
//void PGE_MsgBox::error(std::string msg)
//{
//    PGE_MsgBox::error(QString::fromStdString(msg));
//}


void PGE_MsgBox::fatal(QString msg)
{
    if(GlRenderer::ready())
    {
        PGE_MsgBox msgBox(NULL, msg,
                          PGE_MsgBox::msg_fatal);
        msgBox.exec();
    }
    else
    {
        QMessageBox::critical(NULL, QTranslator::tr("Fatal"), msg, QMessageBox::Ok);
    }
}
//void PGE_MsgBox::fatal(std::string msg)
//{
//    PGE_MsgBox::fatal(QString::fromStdString(msg));
//}

