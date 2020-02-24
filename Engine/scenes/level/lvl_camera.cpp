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

#include "lvl_camera.h"
#include "lvl_backgrnd.h"
#include "lvl_section.h"

#include <graphics/window.h>

#include <data_configs/config_manager.h>
#include <audio/play_music.h>
#include <graphics/gl_renderer.h>
#include <Utils/maths.h>

#include "../scene_level.h"

const double PGE_LevelCamera::_smbxTickTime = 15.285; //1000.0f/65.f;

PGE_LevelCamera::PGE_LevelCamera(LevelScene *_parent) : _scene(_parent)
{
    posRect.setWidth(800);
    posRect.setHeight(600);
    render_x = 0.0;
    render_y = 0.0;
    offset_x = 0.0;
    offset_y = 0.0;
    focus_x = 0.0;
    focus_y = 0.0;
    playerID = 0;
    section = 0;
    cur_section = nullptr;
    fader.setNull();
    m_autoScrool.camera = this;
    _objects_to_render_max = 1000;
    _objects_to_render = reinterpret_cast<PGE_Phys_Object **>(malloc(sizeof(PGE_Phys_Object *) * static_cast<size_t>(_objects_to_render_max)));
    SDL_assert(_objects_to_render);//Out of memory?
    _objects_to_render_stored = 0;
    _objects_to_render_recent = 0;
    _disable_cache_mode = false;
    shake_enabled_x = false;
    shake_enabled_y = false;
    shake_force_decelerate_x = 0.0;
    shake_force_decelerate_y = 0.0;
    shake_force_x = 0.0;
    shake_force_y = 0.0;
}

PGE_LevelCamera::PGE_LevelCamera(const PGE_LevelCamera &cam) : _scene(cam._scene)
{
    render_x = cam.render_x;
    render_y = cam.render_y;
    offset_x = cam.offset_x;
    offset_y = cam.offset_y;
    focus_x = cam.focus_x;
    focus_y = cam.focus_y;
    playerID = cam.playerID;
    posRect = cam.posRect;
    section = cam.section;
    cur_section = cam.cur_section;
    fader = cam.fader;
    m_autoScrool = cam.m_autoScrool;
    m_autoScrool.camera = this;
    _objects_to_render_max = cam._objects_to_render_max;
    _objects_to_render_stored = cam._objects_to_render_stored;
    _objects_to_render_recent = cam._objects_to_render_recent;
    _objects_to_render = static_cast<PGE_Phys_Object **>(malloc(sizeof(PGE_Phys_Object *) * static_cast<size_t>(_objects_to_render_max)));
    memcpy(_objects_to_render, cam._objects_to_render, static_cast<size_t>(_objects_to_render_max));
    _disable_cache_mode = cam._disable_cache_mode;
    shake_enabled_x = cam.shake_enabled_x;
    shake_enabled_y = cam.shake_enabled_y;
    shake_force_decelerate_x = cam.shake_force_decelerate_x;
    shake_force_decelerate_y = cam.shake_force_decelerate_y;
    shake_force_x = cam.shake_force_x;
    shake_force_y = cam.shake_force_y;
}

PGE_LevelCamera::~PGE_LevelCamera()
{
    free(_objects_to_render);
}

void PGE_LevelCamera::init(double x, double y, double w, double h)
{
    posRect.setRect(x, y, w, h);
}


int PGE_LevelCamera::w()
{
    return static_cast<int>(posRect.width());
}

int PGE_LevelCamera::h()
{
    return static_cast<int>(posRect.height());
}

double PGE_LevelCamera::posX()
{
    return posRect.x() + offset_x;
}

double PGE_LevelCamera::posY()
{
    return posRect.y() + offset_y;
}

double PGE_LevelCamera::renderX()
{
    return render_x;
}

double PGE_LevelCamera::renderY()
{
    return render_y;
}

double PGE_LevelCamera::centerX()
{
    return render_x + (posRect.width() / 2.0);
}

double PGE_LevelCamera::centerY()
{
    return render_y + (posRect.height() / 2.0);
}

void PGE_LevelCamera::setPos(double x, double y)
{
    posRect.setPos(round(x), round(y));
    _applyLimits();
}

void PGE_LevelCamera::setRenderPos(double x, double y)
{
    render_x = x;
    render_y = y;
}

void PGE_LevelCamera::setCenterPos(double x, double y)
{
    focus_x = x;
    focus_y = y;
    posRect.setPos(round(x - posRect.width() / 2.0), round(y - posRect.height() / 2.0));
    _applyLimits();
}

void PGE_LevelCamera::setSize(int w, int h)
{
    posRect.setSize(w, h);
}

void PGE_LevelCamera::setOffset(int x, int y)
{
    offset_x = x;
    offset_y = y;
}

void PGE_LevelCamera::updatePre(double frameDelay)
{
    if(!cur_section)
        return;

    // Process built-in background's algorithms
    // (for example, auto-scrollers per every layer)
    cur_section->m_background.process(frameDelay);

    // Process fade in/out effect if requested
    fader.tickFader(frameDelay);

    // Process camera autoscroll in the section
    if(m_autoScrool.enabled)
        m_autoScrool.processAutoscroll(frameDelay);
}

void PGE_LevelCamera::updatePost(double frameDelay)
{
    if(!cur_section)
        return;

    if(_disable_cache_mode)
    {
        _objects_to_render_stored = 0;
        _objects_to_render_recent = 0;
    }
    else
    {
        //Check exists items and remove invizible
        int offset = 0;

        for(int i = 0; i < _objects_to_render_stored; i++)
        {
            if(offset > 0)
                _objects_to_render[i] = _objects_to_render[i + offset];
            PGE_Phys_Object *obj = _objects_to_render[i];
            PGE_Phys_Object::metaCamera *meta = &obj->m_camera_meta;

            if(meta->isVizibleOnScreen && obj->isVisible())
                meta->isVizibleOnScreen = false;
            else
            {
                meta->isInIenderList = false;
                meta->isVizibleOnScreen = false;
                offset++;
                _objects_to_render_stored--;
                i--;
            }
        }

        if(offset > 0)
            _objects_to_render_recent = _objects_to_render_stored;
    }

    queryItems(posRect);

    while(!npcs_to_activate.empty())
    {
        LVL_Npc *npc = dynamic_cast<LVL_Npc *>(npcs_to_activate.front());

        if(npc && npc->isVisible())
        {
            if(!npc->isActivated && !npc->wasDeactivated)
            {
                npc->Activate();
                _scene->m_npcActive.insert(npc);
            }
            else
            {
                if(npc->wasDeactivated)
                    npc->activationTimeout = 0;
                else
                {
                    if(npc->is_activity)
                        npc->activationTimeout = static_cast<int>(npc->setup->setup.deactivationDelay);
                    else
                        npc->activationTimeout = 150;
                }
            }
        }

        npcs_to_activate.pop();
    }

    //Sort array if modified
    if(_objects_to_render_recent != _objects_to_render_stored)
    {
        sortElements();
        _objects_to_render_recent = _objects_to_render_stored;
    }

    // Draw in-scene backgrounds
    cur_section->m_background.drawInScene(posRect.x() + offset_x, posRect.y() + offset_y, posRect.width(), posRect.height());

    /*****************************Screen shaking*******************************/
    if(shake_enabled_x)
    {
        int force_x = static_cast<int>(round(shake_force_x));

        if(force_x != 0)
            offset_x = static_cast<double>(Maths::rand32() % (force_x) * (Maths::rand32() % 2 ? -1 : 1));
        else
            offset_x = 0.0;

        if(shake_force_x > 0.0)
            shake_force_x -= frameDelay * shake_force_decelerate_x;

        if(shake_force_x <= 0.0)
        {
            shake_force_x = 0.0;
            offset_x = 0.0;
            shake_enabled_x = false;
        }
    }

    if(shake_enabled_y)
    {
        int force_y = static_cast<int>(round(shake_force_y));

        if(force_y != 0)
            offset_y = static_cast<double>(Maths::rand32() % (force_y) * (Maths::rand32() % 2 ? -1 : 1));
        else
            offset_y = 0.0;

        if(shake_force_y > 0.0)
            shake_force_y -= frameDelay * shake_force_decelerate_y;

        if(shake_force_y <= 0.0)
        {
            shake_force_y = 0.0;
            offset_y = 0.0;
            shake_enabled_y = false;
        }
    }

    /*****************************Screen shaking**End**************************/
}

void PGE_LevelCamera::shakeScreen(double forceX, double forceY, double dec_step_x, double dec_step_y)
{
    shakeScreenX(forceX, dec_step_x);
    shakeScreenY(forceY, dec_step_y);
}

void PGE_LevelCamera::shakeScreenX(double forceX, double dec_step_x)
{
    if(forceX <= 0.0)
    {
        offset_x = 0.0;
        shake_enabled_x = false;
        shake_force_x = 0.0;
    }
    else
    {
        shake_enabled_x = true;
        shake_force_x = forceX;
        shake_force_decelerate_x = dec_step_x;
    }
}

void PGE_LevelCamera::shakeScreenY(double forceY, double dec_step_y)
{
    if(forceY <= 0.0)
    {
        offset_y = 0.0;
        shake_enabled_y = false;
        shake_force_x = 0.0;
    }
    else
    {
        shake_enabled_y = true;
        shake_force_y = forceY;
        shake_force_decelerate_y = dec_step_y;
    }
}

void PGE_LevelCamera::sortElements()
{
    if(_objects_to_render_stored <= 1) return;   //Nothing to sort!

    std::vector<int> beg;
    std::vector<int> end;
    beg.reserve(static_cast<size_t>(_objects_to_render_stored));
    end.reserve(static_cast<size_t>(_objects_to_render_stored));

    PGE_Phys_Object *piv;
    int i = 0, L, R, swapv;
    beg.push_back(0);
    end.push_back(_objects_to_render_stored);

//Macros to fix "int to size_t" warning conversion
#define Beg(x) beg[static_cast<size_t>(x)]
#define End(y) end[static_cast<size_t>(y)]

    while(i >= 0)
    {
        L = Beg(i);
        R = End(i) - 1;

        if(L < R)
        {
            piv = _objects_to_render[L];

            while(L < R)
            {
                while((_objects_to_render[R]->zIndex() >= piv->zIndex())  && (L < R)) R--;
                if(L < R) _objects_to_render[L++] = _objects_to_render[R];

                while((_objects_to_render[L]->zIndex() <= piv->zIndex()) && (L < R)) L++;
                if(L < R) _objects_to_render[R--] = _objects_to_render[L];
            }

            _objects_to_render[L] = piv;
            beg.push_back(L + 1);
            end.push_back(End(i));
            End(i++) = (L);

            if((End(i) - Beg(i)) > (End(i - 1) - Beg(i - 1)))
            {
                swapv = Beg(i);
                Beg(i) = Beg(i - 1);
                Beg(i - 1) = swapv;
                swapv = End(i);
                End(i) = End(i - 1);
                End(i - 1) = swapv;
            }
        }
        else
        {
            i--;
            beg.pop_back();
            end.pop_back();
        }
    }

//Remove those macros
#undef Beg
#undef End
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
    _objects_to_render_stored = count;
    _objects_to_render_recent = count;
}

void PGE_LevelCamera::setRenderObjectsCacheEnabled(bool enabled)
{
    _disable_cache_mode = !enabled;
}

struct _TreeRenderSearchData
{
    PGE_LevelCamera *list;
    PGE_RectF *zone;
};

bool PGE_LevelCamera::_TreeSearchCallback(PGE_Phys_Object *item, void *arg)
{
    _TreeRenderSearchData *d = static_cast<_TreeRenderSearchData*>(arg);
    if(!d || !d->list)
        return false;

    PGE_LevelCamera *list = d->list;
    if(list)
    {
        if(item)
        {
            if(!list->_disable_cache_mode)
                item->m_camera_meta.isVizibleOnScreen = true;

            bool renderable = false;

            if(!item->isVisible())
                goto checkRenderability;

            switch(item->type)
            {
            case PGE_Phys_Object::LVLSubTree:
            {
                LVL_SubTree *st = dynamic_cast<LVL_SubTree *>(item);
                if(st)
                {
                    PGE_RectF newRect = *d->zone;
                    newRect.setPos(newRect.x() + st->m_offsetX, newRect.y() + st->m_offsetY);
                    _TreeRenderSearchData dd{d->list, &newRect};
                    st->query(newRect, _TreeSearchCallback, (void*)&dd);
                    renderable = true;//keep for a debug render
                }
                break;
            }
            case PGE_Phys_Object::LVLNPC:
                list->npcs_to_activate.push(item);
                renderable = true;
                break;
            case PGE_Phys_Object::LVLBlock:
            case PGE_Phys_Object::LVLBGO:
            case PGE_Phys_Object::LVLPlayer:
            case PGE_Phys_Object::LVLEffect:
            case PGE_Phys_Object::LVLPhysEnv:
            case PGE_Phys_Object::LVLWarp:
                renderable = true;
            }

checkRenderability:
            if(renderable)
            {
                if(item->m_parent)
                {
                    LVL_SubTree *st = dynamic_cast<LVL_SubTree *>(item->m_parent);
                    if(st)
                    {
                        item->m_momentum = item->m_momentum_relative;
                        item->m_momentum.x -= st->m_offsetX;
                        item->m_momentum.y -= st->m_offsetY;
                    }
                }
            }

            if(renderable && (!item->m_camera_meta.isInIenderList || list->_disable_cache_mode))
            {
                if(list->_objects_to_render_stored >= list->_objects_to_render_max - 2)
                {
                    list->_objects_to_render_max += 1000;
                    list->_objects_to_render = static_cast<PGE_Phys_Object **>(realloc(list->_objects_to_render, sizeof(PGE_Phys_Object *) * static_cast<size_t>(list->_objects_to_render_max)));

                    if(!list->_objects_to_render)
                    {
                        pLogFatal("PGE_LevelCamera: Memory overflow!");
                        abort();
                    }
                }
                list->_objects_to_render[list->_objects_to_render_stored] = item;
                list->_objects_to_render_stored++;
                if(!list->_disable_cache_mode)
                    item->m_camera_meta.isInIenderList = true;
            }
        }
    }

    return true;
}

void PGE_LevelCamera::queryItems(PGE_RectF &zone)
{
    _TreeRenderSearchData d{this, &zone};
    _scene->m_qtree.query(zone, _TreeSearchCallback, (void*)&d /*reinterpret_cast<void *>(this)*/ );
}

void PGE_LevelCamera::AutoScrooler::resetAutoscroll()
{
    velX = 0.0;
    velY = 0.0;

    if(!camera->cur_section)
        return;

    camera->limitBox = camera->cur_section->sectionLimitBox();
    velXmax  = camera->cur_section->m_autoscrollVelocityX;
    velYmax  = camera->cur_section->m_autoscrollVelocityY;
}

void PGE_LevelCamera::AutoScrooler::processAutoscroll(double tickTime)
{
    if(!enabled) return;

    if(!camera->cur_section) return;

    double coff = tickTime / _smbxTickTime;
    PGE_RectF  sectionBox   = camera->cur_section->sectionRect();
    PGE_RectF &limitBox     = camera->limitBox;

    if((velXmax > 0.0) && (velX < velXmax))
    {
        velX += 0.05 * coff;

        if(velX > velXmax)
            velX = velXmax;
    }
    else if((velXmax < 0.0) && (velX > velXmax))
    {
        velX -= 0.05 * coff;

        if(velX < velXmax)
            velX = velXmax;
    }

    if((velYmax > 0.0) && (velY < velYmax))
    {
        velY += 0.05 * coff;

        if(velY > velYmax)
            velY = velYmax;
    }
    else if((velYmax < 0.0) && (velY > velYmax))
    {
        velY -= 0.05 * coff;

        if(velY < velYmax)
            velY = velYmax;
    }

    if(((velXmax < 0.0) && (limitBox.left() <= sectionBox.left())) ||
       ((velXmax > 0.0) && (limitBox.right() >= sectionBox.right())))
    {
        velX = 0.0;
        velXmax = 0.0;
    }

    if(((velYmax > 0.0) && (limitBox.bottom() >= sectionBox.bottom())) ||
       ((velYmax < 0.0) && (limitBox.top() <= sectionBox.top())))
    {
        velY = 0.0;
        velYmax = 0.0;
    }

    limitBox.setX(limitBox.x() + velX * coff);

    if(limitBox.left()  < sectionBox.left())  limitBox.setX(sectionBox.x());

    if(limitBox.right() > sectionBox.right()) limitBox.setX(sectionBox.right() - limitBox.width());

    limitBox.setY(limitBox.y() + velY * coff);

    if(limitBox.top()    < sectionBox.top())    limitBox.setY(sectionBox.y());

    if(limitBox.bottom() > sectionBox.bottom()) limitBox.setY(sectionBox.bottom() - limitBox.height());

    camera->_applyLimits();
}

void PGE_LevelCamera::_applyLimits()
{
    if(m_autoScrool.enabled)
    {
        if(posRect.width() > limitBox.width())
            posRect.setX(limitBox.left() + (limitBox.width() / 2.0) - (posRect.width() / 2.0));
        else
        {
            if(posRect.left() < limitBox.left())
                posRect.setX(round(limitBox.left()));

            if(posRect.right() > limitBox.right())
                posRect.setX(round(limitBox.right() - posRect.width()));
        }

        if(posRect.height() > limitBox.height())
            posRect.setY(limitBox.top() + (limitBox.height() / 2.0) - (posRect.height() / 2.0));
        else
        {
            if(posRect.top() < limitBox.top())
                posRect.setY(round(limitBox.top()));

            if(posRect.bottom() > limitBox.bottom())
                posRect.setY(round(limitBox.bottom() - posRect.height()));
        }

        return;
    }

    if(!cur_section)
        return;

    PGE_RectF &limBox = cur_section->m_limitBox;

    if(posRect.width() > limBox.width())
        posRect.setX(limBox.left() + (limBox.width() / 2.0) - (posRect.width() / 2.0));
    else
    {
        if(posRect.left() < cur_section->m_limitBox.left())
            posRect.setX(cur_section->m_limitBox.left());

        if(posRect.right() > cur_section->m_limitBox.right())
            posRect.setX(cur_section->m_limitBox.right() - posRect.width());
    }

    if(posRect.height() > limBox.height())
        posRect.setY(limBox.top() + (limBox.height() / 2.0) - (posRect.height() / 2.0));
    else
    {
        if(posRect.top() < limBox.top())
            posRect.setY(limBox.top());

        if(posRect.bottom() > limBox.bottom())
            posRect.setY(limBox.bottom() - posRect.height());
    }

    if(cur_section->RightOnly())
    {
        if(posRect.left() > limBox.left())
            limBox.setLeft(posRect.left());
    }
}


void PGE_LevelCamera::drawBackground()
{
    if(cur_section)
        cur_section->renderBackground(posRect.x() + offset_x, posRect.y() + offset_y, posRect.width(), posRect.height());
}


void PGE_LevelCamera::drawForeground()
{
    //Draw foreground layers of background
    if(cur_section)
        cur_section->renderForeground(posRect.x() + offset_x, posRect.y() + offset_y, posRect.width(), posRect.height());

    //Draw screen fade rectangle
    if(!fader.isNull())
        GlRenderer::renderRect(0, 0,
                               static_cast<float>(posRect.width()),
                               static_cast<float>(posRect.height()),
                               0.0f, 0.0f, 0.0f,
                               static_cast<float>(fader.fadeRatio()));

    //Fill out of section space (In cases when section is smaller than size of the screen)
    if(cur_section)
    {
        PGE_RectF *limBox = m_autoScrool.enabled ? &limitBox : &(cur_section->m_limitBox);
        double left   = posRect.left() + offset_x;
        double top    = posRect.top() + offset_y;
        double right  = posRect.right() + offset_x;
        double bottom = posRect.bottom() + offset_y;

        if(left < limBox->left())
            GlRenderer::renderRect(0, 0,
                                   static_cast<float>(std::fabs(left - limBox->left())),
                                   static_cast<float>(posRect.height()),
                                   0.0f, 0.0f, 0.0f, 1.0f);

        if(top < limBox->top())
            GlRenderer::renderRect(0, 0,
                                   static_cast<float>(posRect.width()),
                                   static_cast<float>(std::fabs(top - limBox->top())),
                                   0.0f, 0.0f, 0.0f, 1.0f);

        if(right > limBox->right())
        {
            double width = fabs(limBox->right() - right);
            GlRenderer::renderRect(static_cast<float>(posRect.width() - width),
                                   0.0f,
                                   static_cast<float>(width),
                                   static_cast<float>(posRect.height()),
                                   0.0f, 0.0f, 0.0f, 1.0f);
        }

        if(bottom > limBox->bottom())
        {
            double height = fabs(limBox->bottom() - bottom);
            GlRenderer::renderRect(0.0f,
                                   static_cast<float>(posRect.height() - height),
                                   static_cast<float>(posRect.width()),
                                   static_cast<float>(height),
                                   0.0f, 0.0f, 0.0f, 1.0f);
        }
    }
}


void PGE_LevelCamera::changeSection(LVL_Section *sct, bool isInit)
{
    if(!sct) return;

    cur_section = sct;
    section = &sct->m_data;

    if(!isInit)
    {
        cur_section->playMusic();//Play current section music
        cur_section->initBG();   //Init background if not initialized
        m_autoScrool.enabled = cur_section->m_isAutoscroll;

        if(cur_section->m_isAutoscroll) m_autoScrool.resetAutoscroll();
    }
}


bool LevelScene::isVizibleOnScreen(PGE_Phys_Object::Momentum &momentum)
{
    PGE_RectF screen(0, 0, PGE_Window::Width, PGE_Window::Height);

    for(LevelScene::LVL_CameraList::iterator it = m_cameras.begin(); it != m_cameras.end(); it++)
    {
        screen.setPos((*it).posX(), (*it).posY());

        if(screen.collideRect(momentum.x, momentum.y, momentum.w, momentum.h))
            return true;
    }

    return false;
}

bool LevelScene::isVizibleOnScreen(PGE_RectF &rect)
{
    PGE_RectF screen(0, 0, PGE_Window::Width, PGE_Window::Height);

    for(LevelScene::LVL_CameraList::iterator it = m_cameras.begin(); it != m_cameras.end(); it++)
    {
        screen.setPos((*it).posX(), (*it).posY());

        if(screen.collideRect(rect)) return true;
    }

    return false;
}

bool LevelScene::isVizibleOnScreen(double x, double y, double w, double h)
{
    PGE_RectF screen(0, 0, PGE_Window::Width, PGE_Window::Height);

    for(LevelScene::LVL_CameraList::iterator it = m_cameras.begin(); it != m_cameras.end(); it++)
    {
        screen.setPos((*it).posX(), (*it).posY());

        if(screen.collideRect(x, y, w, h)) return true;
    }

    return false;
}
