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

#include "lvl_camera.h"
#include "lvl_backgrnd.h"
#include "lvl_section.h"

#include <graphics/window.h>

#include <data_configs/config_manager.h>
#include <audio/SdlMusPlayer.h>
#include <graphics/gl_renderer.h>

#include "lvl_scene_ptr.h"

#include <QtDebug>
#include <QStack>

const float PGE_LevelCamera::_smbxTickTime=1000.0f/65.f;

PGE_LevelCamera::PGE_LevelCamera()
{
    posRect.setWidth(800);
    posRect.setHeight(600);
    offset_x=0;
    offset_y=0;
    section = 0;
    cur_section = NULL;
    fader.setNull();

    isAutoscroll=false;
    _autoscrollVelocityX_max=0.0f;
    _autoscrollVelocityY_max=0.0f;
    _autoscrollVelocityX=0.0f;
    _autoscrollVelocityY=0.0f;

    _objects_to_render_max=1000;
    _objects_to_render=(PGE_Phys_Object**)malloc(sizeof(PGE_Phys_Object*)*_objects_to_render_max);
    if(!_objects_to_render)
    {
        throw("Out of memory");
    }
    _objects_to_render_stored=0;
    _objects_to_render_recent=0;
    _disable_cache_mode=false;
}

PGE_LevelCamera::PGE_LevelCamera(const PGE_LevelCamera &cam)
{
    offset_x=cam.offset_x;
    offset_y=cam.offset_y;

    posRect = cam.posRect;

    //objects_to_render = cam.objects_to_render;

    section = cam.section;
    cur_section = cam.cur_section;

    fader = cam.fader;

    isAutoscroll=cam.isAutoscroll;
    _autoscrollVelocityX_max=cam._autoscrollVelocityX_max;
    _autoscrollVelocityY_max=cam._autoscrollVelocityY_max;
    _autoscrollVelocityX=cam._autoscrollVelocityX;
    _autoscrollVelocityY=cam._autoscrollVelocityY;

    _objects_to_render_max = cam._objects_to_render_max;
    _objects_to_render_stored = cam._objects_to_render_stored;
    _objects_to_render_recent = cam._objects_to_render_recent;
    _objects_to_render=(PGE_Phys_Object**)malloc(sizeof(PGE_Phys_Object*)*_objects_to_render_max);
    memcpy(_objects_to_render, cam._objects_to_render, _objects_to_render_max);
    _disable_cache_mode=cam._disable_cache_mode;
}

PGE_LevelCamera::~PGE_LevelCamera()
{
    free(_objects_to_render);
}

void PGE_LevelCamera::init(float x, float y, float w, float h)
{
    posRect.setRect(x, y, w, h);
}


int PGE_LevelCamera::w()
{
    return posRect.width();
}

int PGE_LevelCamera::h()
{
    return posRect.height();
}

qreal PGE_LevelCamera::posX()
{
    return posRect.x()+offset_x;
}

qreal PGE_LevelCamera::posY()
{
    return posRect.y()+offset_y;
}

void PGE_LevelCamera::setPos(float x, float y)
{
    posRect.setPos(round(x), round(y));
    _applyLimits();
}

void PGE_LevelCamera::setCenterPos(float x, float y)
{
    posRect.setPos(round(x-posRect.width()/2.0), round(y-posRect.height()/2.0));
    _applyLimits();
}

void PGE_LevelCamera::setSize(int w, int h)
{
    posRect.setSize(w, h);
}

void PGE_LevelCamera::setOffset(int x, int y)
{
    offset_x=x;
    offset_y=y;
}

void PGE_LevelCamera::update(float ticks)
{
    //objects_to_render.clear();

    if(!cur_section) return;
    fader.tickFader(ticks);

    if(isAutoscroll) processAutoscroll(ticks);

    if(_disable_cache_mode)
    {
        _objects_to_render_stored=0;
        _objects_to_render_recent=0;
    } else {
        //Check exists items and remove invizible
        int offset=0;
        for(int i=0; i<_objects_to_render_stored; i++)
        {
            if(offset > 0)
            {
                _objects_to_render[i] = _objects_to_render[i+offset];
            }
            if(_objects_to_render[i]->_vizible_on_screen && _objects_to_render[i]->isVisible())
            {
                _objects_to_render[i]->_vizible_on_screen=false;
            } else {
                _objects_to_render[i]->_render_list=false;
                _objects_to_render[i]->_vizible_on_screen=false;
                offset++;
                _objects_to_render_stored--;
                i--;
            }
        }
        if(offset > 0)
        {
            _objects_to_render_recent=_objects_to_render_stored;
        }
    }

    queryItems(posRect);

    //Sort array if modified
    if(_objects_to_render_recent!=_objects_to_render_stored)
    {
        sortElements();
        _objects_to_render_recent=_objects_to_render_stored;
    }
}

void PGE_LevelCamera::sortElements()
{
    if(_objects_to_render_stored<=1) return; //Nothing to sort!
    QStack<int> beg;
    QStack<int> end;
    PGE_Phys_Object* piv;
    int i=0, L, R, swapv;
    beg.push_back(0);
    end.push_back(_objects_to_render_stored);
    while (i>=0)
    {
        L=beg[i]; R=end[i]-1;
        if (L<R)
        {
            piv=_objects_to_render[L];
            while (L<R)
            {
                while ((_objects_to_render[R]->zIndex()>=piv->zIndex()) && (L<R)) R--;
                if (L<R) _objects_to_render[L++]=_objects_to_render[R];

                while ((_objects_to_render[L]->zIndex()<=piv->zIndex()) && (L<R)) L++;
                if (L<R) _objects_to_render[R--]=_objects_to_render[L];
            }
            _objects_to_render[L]=piv; beg.push_back(L+1); end.push_back(end[i]); end[i++]=(L);
            if((end[i]-beg[i]) > (end[i-1]-beg[i-1]))
            {
                swapv=beg[i]; beg[i]=beg[i-1]; beg[i-1]=swapv;
                swapv=end[i]; end[i]=end[i-1]; end[i-1]=swapv;
            }
        }
        else
        {
            i--;
            beg.pop_back();
            end.pop_back();
        }
    }
}

void PGE_LevelCamera::changeSectionBorders(long left, long top, long right, long bottom)
{
    if(cur_section)
        cur_section->changeSectionBorders(left, top, right, bottom);
}

void PGE_LevelCamera::resetLimits()
{
    if(cur_section)
        cur_section->resetLimits();
}

//PGE_RenderList &PGE_LevelCamera::renderObjects()
//{
//    return objects_to_render;
//}

PGE_Phys_Object **PGE_LevelCamera::renderObjects_arr()
{
    return _objects_to_render;
}

int PGE_LevelCamera::renderObjects_max()
{
    return _objects_to_render_max;
}

int PGE_LevelCamera::renderObjects_count()
{
    return _objects_to_render_stored;
}

void PGE_LevelCamera::setRenderObjects_count(int count)
{
    _objects_to_render_stored=count;
    _objects_to_render_recent=count;
}

void PGE_LevelCamera::setRenderObjectsCacheEnabled(bool enabled)
{
    _disable_cache_mode=!enabled;
}

bool PGE_LevelCamera::_TreeSearchCallback(PGE_Phys_Object* item, void* arg)
{
    PGE_LevelCamera* list = static_cast<PGE_LevelCamera* >(arg);
    if(list)
    {
        if( item )
        {
            if(!list->_disable_cache_mode)
            {
                item->_vizible_on_screen = true;
            }
            bool renderable=false;
            if(!item->isVisible())
            {
                goto checkRenderability;
            }
            switch(item->type)
            {
                case PGE_Phys_Object::LVLBlock:
                case PGE_Phys_Object::LVLBGO:
                case PGE_Phys_Object::LVLNPC:
                case PGE_Phys_Object::LVLPlayer:
                case PGE_Phys_Object::LVLEffect:
                case PGE_Phys_Object::LVLPhysEnv:
                case PGE_Phys_Object::LVLWarp:
                    renderable=true;
            }
            if(item->type==PGE_Phys_Object::LVLNPC)
            {
                LVL_Npc *npc = dynamic_cast<LVL_Npc*>(item);
                if(npc && npc->isVisible())
                {
                    if(!npc->isActivated && !npc->wasDeactivated)
                    {
                        npc->Activate();
                        LvlSceneP::s->active_npcs.push_back(npc);
                    }
                    else
                    {
                        if(npc->wasDeactivated)
                            npc->activationTimeout=0;
                        else
                        {
                            if(npc->is_activity)
                                npc->activationTimeout = npc->setup->deactivetionDelay;
                            else
                                npc->activationTimeout = 150;
                        }
                    }
                }
            }

        checkRenderability:
            if(renderable && (!item->_render_list || list->_disable_cache_mode))
            {
                if(list->_objects_to_render_stored >= list->_objects_to_render_max-2)
                {
                    list->_objects_to_render_max += 1000;
                    list->_objects_to_render=(PGE_Phys_Object**)realloc(list->_objects_to_render, sizeof(PGE_Phys_Object*)*list->_objects_to_render_max);
                    if(!list->_objects_to_render)
                    {
                        throw("Memory overflow!");
                    }
                }
                list->_objects_to_render[list->_objects_to_render_stored]=item;
                list->_objects_to_render_stored++;
                if(!list->_disable_cache_mode)
                {
                    item->_render_list=true;
                }
            }
        }
    }
    return true;
}

void PGE_LevelCamera::queryItems(PGE_RectF &zone)
{
    double lt[2] = { zone.left(),  zone.top() };
    double rb[2] = { zone.right(), zone.bottom() };
    LvlSceneP::s->tree.Search(lt, rb, _TreeSearchCallback, (void*)this);
}

void PGE_LevelCamera::resetAutoscroll()
{
    _autoscrollVelocityX=0.0f;
    _autoscrollVelocityY=0.0f;
    if(!cur_section) return;
    limitBox=cur_section->sectionLimitBox();
    _autoscrollVelocityX_max = cur_section->_autoscrollVelocityX;
    _autoscrollVelocityY_max = cur_section->_autoscrollVelocityY;
}

void PGE_LevelCamera::processAutoscroll(float tickTime)
{
    if(!isAutoscroll) return;
    if(!cur_section) return;

    float coff=tickTime/_smbxTickTime;
    PGE_RectF sectionBox = cur_section->sectionRect();

    limitBox.setX(limitBox.x() + _autoscrollVelocityX*coff);
        if(limitBox.left()<sectionBox.left()) limitBox.setX(sectionBox.x());
        if(limitBox.right()>sectionBox.right()) limitBox.setX(sectionBox.right()-limitBox.width());
    limitBox.setY(limitBox.y() + _autoscrollVelocityY*coff);
        if(limitBox.top()<sectionBox.top()) limitBox.setY(sectionBox.y());
        if(limitBox.bottom()>sectionBox.bottom()) limitBox.setY(sectionBox.bottom()-limitBox.height());

    if((_autoscrollVelocityX_max>0)&&(_autoscrollVelocityX<_autoscrollVelocityX_max)) {
        _autoscrollVelocityX+=0.05f*coff;
        if(_autoscrollVelocityX>_autoscrollVelocityX_max)
           _autoscrollVelocityX=_autoscrollVelocityX_max;
    } else if((_autoscrollVelocityX_max<0)&&(_autoscrollVelocityX>_autoscrollVelocityX_max)) {
        _autoscrollVelocityX-=0.05f*coff;
        if(_autoscrollVelocityX<_autoscrollVelocityX_max)
           _autoscrollVelocityX=_autoscrollVelocityX_max;
    }

    if((_autoscrollVelocityY_max>0)&&(_autoscrollVelocityY<_autoscrollVelocityY_max)) {
        _autoscrollVelocityY+=0.05f*coff;
        if(_autoscrollVelocityY>_autoscrollVelocityY_max)
           _autoscrollVelocityY=_autoscrollVelocityY_max;
    } else if((_autoscrollVelocityY_max<0)&&(_autoscrollVelocityY>_autoscrollVelocityY_max)) {
        _autoscrollVelocityY-=0.05f*coff;
        if(_autoscrollVelocityY<_autoscrollVelocityY_max)
           _autoscrollVelocityY=_autoscrollVelocityY_max;
    }
}

void PGE_LevelCamera::_applyLimits()
{
    if(isAutoscroll)
    {
        if(posRect.left() < limitBox.left())
            posRect.setX(round(limitBox.left()));

        if(posRect.right() > limitBox.right())
            posRect.setX(round(limitBox.right()-posRect.width()));

        if(posRect.top() < limitBox.top())
            posRect.setY(round(limitBox.top()));

        if(posRect.bottom()>limitBox.bottom())
            posRect.setY(round(limitBox.bottom()-posRect.height()));
        return;
    }

    if(!cur_section) return;
    if(posRect.left() < cur_section->limitBox.left())
        posRect.setX(cur_section->limitBox.left());

    if(posRect.right() > cur_section->limitBox.right())
        posRect.setX(cur_section->limitBox.right()-posRect.width());

    if(posRect.top() < cur_section->limitBox.top())
        posRect.setY(cur_section->limitBox.top());

    if(posRect.bottom()>cur_section->limitBox.bottom())
        posRect.setY(cur_section->limitBox.bottom()-posRect.height());

    if(cur_section->RightOnly())
    {
        if(posRect.left() > cur_section->limitBox.left())
            cur_section->limitBox.setLeft(posRect.left());
    }
}


void PGE_LevelCamera::drawBackground()
{
    if(cur_section)
    {
        cur_section->renderBG(posRect.x(), posRect.y(), posRect.width(), posRect.height());
    }
}

void PGE_LevelCamera::drawForeground()
{
    if(!fader.isNull())
        GlRenderer::renderRect(0, 0, posRect.width(), posRect.height(), 0.0f, 0.0f, 0.0f, fader.fadeRatio());
}


void PGE_LevelCamera::changeSection(LVL_Section *sct, bool isInit)
{
    if(!sct) return;

    cur_section=sct;
    section = &sct->data;
    if(!isInit)
    {
        cur_section->playMusic();//Play current section music
        cur_section->initBG();   //Init background if not initialized
        isAutoscroll=cur_section->isAutoscroll;
        if(cur_section->isAutoscroll) resetAutoscroll();
    }
}



bool LevelScene::isVizibleOnScreen(PGE_RectF &rect)
{
    PGE_RectF screen(0, 0, PGE_Window::Width, PGE_Window::Height);
    for(LevelScene::LVL_CameraList::iterator it=LvlSceneP::s->cameras.begin();it!=LvlSceneP::s->cameras.end();it++)
    {
        screen.setPos((*it).posX(), (*it).posY());
        if(screen.collideRect(rect)) return true;
    }
    return false;
}

bool LevelScene::isVizibleOnScreen(double x, double y, double w, double h)
{
    PGE_RectF screen(0, 0, PGE_Window::Width, PGE_Window::Height);
    for(LevelScene::LVL_CameraList::iterator it=LvlSceneP::s->cameras.begin();it!=LvlSceneP::s->cameras.end();it++)
    {
        screen.setPos((*it).posX(), (*it).posY());
        if(screen.collideRect(x, y, w, h)) return true;
    }
    return false;
}
