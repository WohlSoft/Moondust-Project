#include "pge_menuboxbase.h"

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

#include <fontman/font_manager.h>
#include <graphics/window.h>

#include <common_features/app_path.h>
#include <audio/pge_audio.h>
#include <settings/global_settings.h>

#include "../scenes/scene_level.h"
#include "../scenes/scene_world.h"
#include "../scenes/scene_gameover.h"

#include <QFontMetrics>
#include <QMessageBox>
#include <QApplication>
#include <QTranslator>


PGE_MenuBoxBase::PGE_MenuBoxBase(Scene *_parentScene, PGE_Menu::menuAlignment alignment, int gapSpace, QString _title, msgType _type,
                       PGE_Point boxCenterPos, float _padding, QString texture)
    : PGE_BoxBase(_parentScene), _menu(alignment, gapSpace)
{
    setParentScene(_parentScene);
    construct(_title,_type, boxCenterPos, _padding, texture);
}

PGE_MenuBoxBase::PGE_MenuBoxBase(const PGE_MenuBoxBase &mb)
    : PGE_BoxBase(mb), _menu(mb._menu)
{
    _page     = mb._page;
    running   = mb.running;
    fontID    = mb.fontID;
    fontRgba  = mb.fontRgba;
    _answer_id= mb._answer_id;

    reject_snd= mb.reject_snd;
    _ctrl1    = mb._ctrl1;
    _ctrl2    = mb._ctrl2;
    type      = mb.type;
    _pos=       mb._pos;

    _sizeRect = mb._sizeRect;
    title     = mb.title;
    title_size= mb.title_size;
    //_menu     = mb._menu;
    width     = mb.width;

    height    = mb.height;
    padding   = mb.padding;
    bg_color  = mb.bg_color;
}


void PGE_MenuBoxBase::construct(QString _title, PGE_MenuBoxBase::msgType _type,
                            PGE_Point pos, float _padding, QString texture)
{
    if(!texture.isEmpty())
        loadTexture(texture);

    updateTickValue();
    _page=0;
    running=false;
    _answer_id = -1;
    _pos=pos;
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
        case msg_info: bg_color =       QColor(qRgb(0,0,0)); break;
        case msg_info_light: bg_color = QColor(qRgb(0,0,125)); break;
        case msg_warn: bg_color =       QColor(qRgb(255,201,14)); break;
        case msg_error: bg_color =      QColor(qRgb(125,0,0)); break;
        case msg_fatal: bg_color =      QColor(qRgb(255,0,0)); break;
        default:  bg_color =            QColor(qRgb(0,0,0)); break;
    }
    type=_type;
}

void PGE_MenuBoxBase::setTitleFont(QString fontName)
{
    fontID   = FontManager::getFontID(fontName);
}

void PGE_MenuBoxBase::setTitleFontColor(GlColor color)
{
    fontRgba=color;
}

void PGE_MenuBoxBase::setTitleText(QString text)
{
    title = text;
    FontManager::optimizeText(title, 27);
    title_size = FontManager::textSize(text, fontID, 27);
    updateSize();
}

void PGE_MenuBoxBase::setPadding(int _padding)
{
    if(_padding<0)
        _padding = ConfigManager::setup_menu_box.box_padding;
    padding=_padding;
}

void PGE_MenuBoxBase::setPos(float x, float y)
{
    _pos.setX(x);
    _pos.setY(y);
    updateSize();
}

void PGE_MenuBoxBase::setMaxMenuItems(int items)
{
    _menu.setItemsNumber(items);
    updateSize();
}

void PGE_MenuBoxBase::setBoxSize(float _Width, float _Height, float _padding)
{
    width = _Width;
    height = _Height;
    padding = _padding;
}

void PGE_MenuBoxBase::updateSize()
{
    PGE_Rect menuRect = _menu.rectFull();
    if(menuRect.width()>title_size.w())
        width = menuRect.width()/2;
    else
        width = title_size.w()/2;
    height=(title_size.h()+menuRect.height())/2;

    if((_pos.x()==-1)&&(_pos.y()==-1))
    {
        _sizeRect.setLeft(PGE_Window::Width/2-width-padding);
        _sizeRect.setTop(PGE_Window::Height/2-height-padding);
        _sizeRect.setRight(PGE_Window::Width/2 + width + padding);
        _sizeRect.setBottom(PGE_Window::Height/2+height + padding);
        if(_sizeRect.top() < padding)
            _sizeRect.setY(padding);
    }
    else
    {
        _sizeRect.setLeft( _pos.x() - width-padding);
        _sizeRect.setTop(_pos.y() - height-padding);
        _sizeRect.setRight(_pos.x() + width + padding);
        _sizeRect.setBottom(_pos.y() + height + padding);
    }

    _menu.setPos(_sizeRect.right() - ( menuRect.width()>title_size.w() ? menuRect.width() : title_size.w() ),
                 _sizeRect.bottom() - menuRect.height() + _menu.topOffset() - padding);
}



void PGE_MenuBoxBase::clearMenu()
{
    _menu.clear();
    updateSize();
}

void PGE_MenuBoxBase::addMenuItem(QString &menuitem)
{
    _menu.addMenuItem(menuitem.simplified().replace(' ', '_'), menuitem);
    updateSize();
}

void PGE_MenuBoxBase::addMenuItems(QStringList &menuitems)
{
    foreach(QString menuitem, menuitems)
        _menu.addMenuItem(menuitem.simplified().replace(' ', '_'), menuitem);
    updateSize();
}


void PGE_MenuBoxBase::update(float ticks)
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

void PGE_MenuBoxBase::render()
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
        FontManager::printText(title,
                               _sizeRect.center().x()-title_size.w()/2,
                               _sizeRect.top()+padding,
                               fontID,
                               fontRgba.Red(), fontRgba.Green(), fontRgba.Blue(), fontRgba.Alpha());
        _menu.render();
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

void PGE_MenuBoxBase::restart()
{
    _page=0;
    _menu.resetState();
    _menu.reset();
    running=true;
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
        Uint32 start_render=SDL_GetTicks();

        updateControllers();
        update(uTickf);
        PGE_BoxBase::render();
        render();
        glFlush();
        PGE_Window::rePaint();

        if((!PGE_Window::vsync)&&(uTick > (signed)(SDL_GetTicks() - start_render)))
                SDL_Delay(uTick - (SDL_GetTicks()-start_render) );
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
    _answer_id=-1; _page++; setFade(10, 0.0f, 0.05f);
    if(reject_snd>0)
        PGE_Audio::playSound(reject_snd);
}

void PGE_MenuBoxBase::processKeyEvent(SDL_Keycode &key)
{
    if(_page!=2) return;

    if(_ctrl1)
    {
        if(_ctrl1->keys.up) {
            onUpButton(); return;
        } else if(_ctrl1->keys.down) {
            onDownButton(); return;
        } else if(_ctrl1->keys.left) {
            onLeftButton(); return;
        } else if(_ctrl1->keys.right) {
            onRightButton(); return;
        } else if(_ctrl1->keys.jump) {
            onJumpButton(); return;
        } else if(_ctrl1->keys.alt_jump) {
            onAltJumpButton(); return;
        } else if(_ctrl1->keys.run) {
            onRunButton(); return;
        } else if(_ctrl1->keys.alt_run){
            onAltRunButton(); return;
        } else if(_ctrl1->keys.start) {
            onStartButton(); return;
        } else if(_ctrl1->keys.drop) {
           onDropButton(); return;
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
         onDropButton(); return;
      break;
      default:
        break;
    }
}

void PGE_MenuBoxBase::processLoader(float ticks)
{
    SDL_Event event;
    while ( SDL_PollEvent(&event) ) {
        PGE_Window::processEvents(event);
        if(event.type==SDL_QUIT)
            fader_opacity=1.0;
    }
    tickFader(ticks);
    if(fader_opacity>=1.0f) _page++;
}

void PGE_MenuBoxBase::processBox(float)
{
    #ifndef __APPLE__
    if(AppSettings.interprocessing)
        qApp->processEvents();
    #endif
    SDL_Event event;
    while ( SDL_PollEvent(&event) )
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
        _page++; setFade(10, 0.0f, 0.05f);return;
    }
}



void PGE_MenuBoxBase::processUnLoader(float ticks)
{
    SDL_Event event;
    while ( SDL_PollEvent(&event) ) {
        PGE_Window::processEvents(event);
        if(event.type==SDL_QUIT)
            fader_opacity=0.0;
    }
    tickFader(ticks);
    if(fader_opacity<=0.0f) _page++;
}


void PGE_MenuBoxBase::initControllers()
{
    if(parentScene!=NULL)
    {
        if(parentScene->type()==Scene::Level)
        {
            LevelScene * s = dynamic_cast<LevelScene *>(parentScene);
            if(s)
            {
                _ctrl1 = s->player1Controller;
                _ctrl2 = s->player2Controller;
            }
        }
        else if(parentScene->type()==Scene::World)
        {
            WorldScene * s = dynamic_cast<WorldScene *>(parentScene);
            if(s)
            {
                _ctrl1 = s->player1Controller;
                _ctrl2=NULL;
            }
        }
        else if(parentScene->type()==Scene::GameOver)
        {
            GameOverScene * s = dynamic_cast<GameOverScene *>(parentScene);
            if(s)
            {
                _ctrl1 = s->player1Controller;
                _ctrl2=NULL;
            }
        }
        else
        {
            _ctrl1=NULL;
            _ctrl2=NULL;
        }
    }
    else
    {
        _ctrl1=NULL;
        _ctrl2=NULL;
    }
}

void PGE_MenuBoxBase::updateControllers()
{
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
    if(parentScene!=NULL)
    {
        if(parentScene->type()==Scene::Level)
        {
            LevelScene * s = dynamic_cast<LevelScene *>(parentScene);
            if(s)
            {
                s->tickAnimations(uTickf);
            }
        }
        else if(parentScene->type()==Scene::World)
        {
            WorldScene * s = dynamic_cast<WorldScene *>(parentScene);
            if(s)
            {
                s->tickAnimations(uTickf);
            }
        }
    }
}



void PGE_MenuBoxBase::info(QString msg)
{
    if(GlRenderer::ready())
    {
        PGE_MsgBox msgBox(NULL, msg,
                          PGE_MenuBox::msg_info_light);
        msgBox.exec();
    }
    else
    {
        QMessageBox::information(NULL, QTranslator::tr("Information"), msg, QMessageBox::Ok);
    }
}
//void PGE_MenuBox::info(std::string msg)
//{
//    PGE_MenuBox::info(QString::fromStdString(msg));
//}

void PGE_MenuBoxBase::warn(QString msg)
{
    if(GlRenderer::ready())
    {
        PGE_MsgBox msgBox(NULL, msg,
                          PGE_MenuBox::msg_warn);
        msgBox.exec();
    }
    else
    {
        QMessageBox::warning(NULL, QTranslator::tr("Warning"), msg, QMessageBox::Ok);
    }
}
//void PGE_MenuBox::warn(std::string msg)
//{
//    PGE_MenuBox::warn(QString::fromStdString(msg));
//}


void PGE_MenuBoxBase::error(QString msg)
{
    if(GlRenderer::ready())
    {
        PGE_MsgBox msgBox(NULL, msg,
                          PGE_MenuBox::msg_error);
        msgBox.exec();
    }
    else
    {
        QMessageBox::critical(NULL, QTranslator::tr("Error"), msg, QMessageBox::Ok);
    }
}
//void PGE_MenuBox::error(std::string msg)
//{
//    PGE_MenuBox::error(QString::fromStdString(msg));
//}


void PGE_MenuBoxBase::fatal(QString msg)
{
    if(GlRenderer::ready())
    {
        PGE_MsgBox msgBox(NULL, msg,
                          PGE_MenuBox::msg_fatal);
        msgBox.exec();
    }
    else
    {
        QMessageBox::critical(NULL, QTranslator::tr("Fatal"), msg, QMessageBox::Ok);
    }
}
//void PGE_MenuBox::fatal(std::string msg)
//{
//    PGE_MenuBox::fatal(QString::fromStdString(msg));
//}

