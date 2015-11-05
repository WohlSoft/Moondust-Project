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

#include "wld_pathopener.h"
#include "wld_tilebox.h"
#include <audio/pge_audio.h>

#include "../scene_world.h"

void WldPathOpener::construct()
{
    interval=1.f;
    _time=0.f;
    force=false;
}

WldPathOpener::WldPathOpener()
{
    s=NULL;
    construct();
}

WldPathOpener::WldPathOpener(WorldScene *_s)
{
    s=_s;
    construct();
}

void WldPathOpener::setScene(WorldScene *_s)
{
    s=_s;
}

void WldPathOpener::setInterval(float _ms)
{
    if(_ms<=0) _ms=1.0f;
    interval=_ms;
}

void WldPathOpener::startAt(PGE_PointF pos)
{
    need_to_walk.clear();
    _current_pos=pos;
    _start_at=pos;
    initFetcher();
}

bool WldPathOpener::processOpener(float tickTime)
{
    _time-=tickTime;
    while(_time<0.0f)
    {
        _time+=interval;
        doFetch();
        if(need_to_walk.isEmpty() && next.isEmpty()) return false;
    }
    return true;
}

void WldPathOpener::fetchSideNodes(bool &side, QVector<WorldNode* > &nodes, float cx, float cy)
{
    side=false;
    foreach (WorldNode* x, nodes)
    {
        if(x->type==WorldNode::path)
        {
            side=x->collidePoint(cx, cy);
            if(side && !x->vizible)
            {
                if(!need_to_walk.contains(_search_pos)) need_to_walk.push_back(_search_pos);
                next.push(x);
            }
        }
        if(x->type==WorldNode::level)
        {
            side=x->collidePoint(cx, cy);
            if(side && !x->vizible) next.push(x);
        }
    }
}

void WldPathOpener::doFetch()
{
    QVector<WorldNode* > nodes;

    bool found=false;
    long x,y;

    if(!next.isEmpty())
    {
        findAndHideSceneries(next.pop());
        PGE_Audio::playSoundByRole(obj_sound_role::WorldOpenPath);
    }

    if(need_to_walk.isEmpty()) return;
    _current_pos=need_to_walk.pop();

    //left
    _search_pos.setX(_current_pos.x()-s->_indexTable.grid());
    _search_pos.setY(_current_pos.y());
    x=_current_pos.x()+s->_indexTable.grid_half()-s->_indexTable.grid();
    y=_current_pos.y()+s->_indexTable.grid_half();
    s->_indexTable.query(x,y, nodes);
    fetchSideNodes(found, nodes, x,y);
    nodes.clear();

    //Right
    _search_pos.setX(_current_pos.x()+s->_indexTable.grid());
    _search_pos.setY(_current_pos.y());
    x=_current_pos.x()+s->_indexTable.grid_half()+s->_indexTable.grid();
    y=_current_pos.y()+s->_indexTable.grid_half();
    s->_indexTable.query(x,y, nodes);
    fetchSideNodes(found, nodes, x, y);
    nodes.clear();

    //Top
    _search_pos.setX(_current_pos.x());
    _search_pos.setY(_current_pos.y()-s->_indexTable.grid());
    x=_current_pos.x()+s->_indexTable.grid_half();
    y=_current_pos.y()+s->_indexTable.grid_half()-s->_indexTable.grid();
    s->_indexTable.query(x, y, nodes);
    fetchSideNodes(found, nodes, x, y);
    nodes.clear();

    //Bottom
    _search_pos.setX(_current_pos.x());
    _search_pos.setY(_current_pos.y()+s->_indexTable.grid());
    x=_current_pos.x()+s->_indexTable.grid_half();
    y=_current_pos.y()+s->_indexTable.grid_half()+s->_indexTable.grid();
    s->_indexTable.query(x,y, nodes);
    fetchSideNodes(found, nodes, x, y);
    nodes.clear();
}

void WldPathOpener::findAndHideSceneries(WorldNode *relativeTo)
{
    //Set element to be vizible
    if(relativeTo->type==WorldNode::path)
    {
        WldPathItem * y=dynamic_cast<WldPathItem *>(relativeTo);
        if(y) y->vizible=true;
    } else if(relativeTo->type==WorldNode::level) {
        WldLevelItem * y=dynamic_cast<WldLevelItem *>(relativeTo);
        if(y) y->vizible=true;
    }

    //Set all sceneries under this item to be invizible
    QVector<WorldNode* > nodes;
    s->_indexTable.query(relativeTo->x,
                         relativeTo->y,
                         relativeTo->x+relativeTo->w,
                         relativeTo->y+relativeTo->h,
                         nodes);
    foreach (WorldNode* x, nodes)
    {
        if(x->type==WorldNode::scenery)
        {
            WldSceneryItem * y=dynamic_cast<WldSceneryItem *>(x);
            if(y && y->collideWith(relativeTo)) y->vizible=false;
        }
    }
}



void WldPathOpener::initFetcher()
{
    QVector<WorldNode* > nodes;
    int exitCode=s->gameState->_recent_ExitCode_level, x, y;
    x=_current_pos.x()+s->_indexTable.grid_half();
    y=_current_pos.y()+s->_indexTable.grid_half();

    s->_indexTable.query(x, y, nodes);
    foreach (WorldNode* x, nodes)
    {
        if(x->type==WorldNode::level)
        {
            WldLevelItem * wl=dynamic_cast<WldLevelItem *>(x);
            if(wl)
            {
                wl->vizible=true;
                _current_pos.setX(wl->x);
                _current_pos.setY(wl->y);

                if(force || ((wl->data.left_exit!=0)&&((wl->data.left_exit==exitCode) || (wl->data.left_exit<0))) )
                {
                    _search_pos.setX(_current_pos.x()-s->_indexTable.grid());
                    _search_pos.setY(_current_pos.y());
                    need_to_walk.push_back(_search_pos);
                }
                if(force || ((wl->data.right_exit!=0)&&((wl->data.right_exit==exitCode) || (wl->data.right_exit<0))) )
                {
                    _search_pos.setX(_current_pos.x()+s->_indexTable.grid());
                    _search_pos.setY(_current_pos.y());
                    need_to_walk.push_back(_search_pos);
                }
                if(force || ((wl->data.top_exit!=0)&&( (wl->data.top_exit==exitCode) || (wl->data.top_exit<0))) )
                {
                    _search_pos.setX(_current_pos.x());
                    _search_pos.setY(_current_pos.y()-s->_indexTable.grid());
                    need_to_walk.push_back(_search_pos);
                }
                if(force || ((wl->data.bottom_exit!=0)&&( (wl->data.bottom_exit==exitCode) || (wl->data.bottom_exit<0))) )
                {
                    _search_pos.setX(_current_pos.x());
                    _search_pos.setY(_current_pos.y()+s->_indexTable.grid());
                    need_to_walk.push_back(_search_pos);
                }
                break;
            }
        }
    }
}

void WldPathOpener::setForce()
{
    force=true;
}
