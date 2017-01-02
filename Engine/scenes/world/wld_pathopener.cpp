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

#include "wld_pathopener.h"
#include "wld_tilebox.h"
#include <audio/pge_audio.h>
#include <Utils/maths.h>

#include "../scene_world.h"

#ifdef DEBUG_BUILD
#include <common_features/logger.h>
#endif

void WldPathOpener::construct()
{
    interval = 1.0;
    _time = 0.0;
    force = false;
}

WldPathOpener::WldPathOpener()
{
    s = nullptr;
    construct();
}

WldPathOpener::WldPathOpener(WorldScene *_s)
{
    s = _s;
    construct();
}

void WldPathOpener::setScene(WorldScene *_s)
{
    s = _s;
}

void WldPathOpener::setInterval(double _ms)
{
    if(_ms <= 0.0)
        _ms = 1.0;

    interval = _ms;
}

void WldPathOpener::startAt(PGE_PointF pos)
{
    need_to_walk.clear();
    _current_pos = pos;
    _start_at    = pos;
    initFetcher();
}

bool WldPathOpener::processOpener(double tickTime)
{
    _time -= tickTime;

    while(_time < 0.0)
    {
        _time += interval;
        doFetch();

        if(need_to_walk.isEmpty() && next.isEmpty())
            return false;
    }

    return true;
}

void WldPathOpener::fetchSideNodes(bool &side, QVector<WorldNode * > &nodes, double cx, double cy)
{
    side = false;
    long lcx = Maths::lRound(cx);
    long lcy = Maths::lRound(cy);

    for(WorldNode *x : nodes)
    {
        if(x->type == WorldNode::path)
        {
            bool contact = x->collidePoint(lcx, lcy);
            #ifdef DEBUG_BUILD
            LogDebug(QString("Level node collision %1...").arg(contact ? "passed" : "missed"));
            #endif

            if(contact && !x->vizible)
            {
                if(!need_to_walk.contains(_search_pos))
                    need_to_walk.push_back(_search_pos);

                next.push(x);
                side |= contact;
            }
        }
        else if(x->type == WorldNode::level)
        {
            bool contact = x->collidePoint(lcx, lcy);
            #ifdef DEBUG_BUILD
            LogDebug(QString("Path node collision %1...").arg(contact ? "passed" : "missed"));
            #endif

            if(contact && !x->vizible)
            {
                next.push(x);
                side |= contact;
            }
        }
    }
}

void WldPathOpener::doFetch()
{
    QVector<WorldNode * > nodes;
    bool found = false;
    double x, y;
    #ifdef DEBUG_BUILD
    LogDebug("Fetch leaf....");
    #endif

    if(!next.isEmpty())
    {
        findAndHideSceneries(next.pop());
        PGE_Audio::playSoundByRole(obj_sound_role::WorldOpenPath);
    }

    if(need_to_walk.isEmpty())
        return;

    _current_pos = need_to_walk.pop();
    //left
    _search_pos.setX(_current_pos.x() - s->m_indexTable.grid());
    _search_pos.setY(_current_pos.y());
    x = _current_pos.x() + s->m_indexTable.grid_half() - s->m_indexTable.grid();
    y = _current_pos.y() + s->m_indexTable.grid_half();
    s->m_indexTable.query(Maths::lRound(x), Maths::lRound(y), nodes);
    fetchSideNodes(found, nodes, x, y);
    nodes.clear();
    //Right
    _search_pos.setX(_current_pos.x() + s->m_indexTable.grid());
    _search_pos.setY(_current_pos.y());
    x = _current_pos.x() + s->m_indexTable.grid_half() + s->m_indexTable.grid();
    y = _current_pos.y() + s->m_indexTable.grid_half();
    s->m_indexTable.query(Maths::lRound(x), Maths::lRound(y), nodes);
    fetchSideNodes(found, nodes, x, y);
    nodes.clear();
    //Top
    _search_pos.setX(_current_pos.x());
    _search_pos.setY(_current_pos.y() - s->m_indexTable.grid());
    x = _current_pos.x() + s->m_indexTable.grid_half();
    y = _current_pos.y() + s->m_indexTable.grid_half() - s->m_indexTable.grid();
    s->m_indexTable.query(Maths::lRound(x), Maths::lRound(y), nodes);
    fetchSideNodes(found, nodes, x, y);
    nodes.clear();
    //Bottom
    _search_pos.setX(_current_pos.x());
    _search_pos.setY(_current_pos.y() + s->m_indexTable.grid());
    x = _current_pos.x() + s->m_indexTable.grid_half();
    y = _current_pos.y() + s->m_indexTable.grid_half() + s->m_indexTable.grid();
    s->m_indexTable.query(Maths::lRound(x), Maths::lRound(y), nodes);
    fetchSideNodes(found, nodes, x, y);
    nodes.clear();
}

static void spawnSmoke(WorldScene *s, WorldNode *at)
{
    SpawnEffectDef smoke = ConfigManager::g_setup_effects.m_smoke;
    smoke.startX = static_cast<double>(at->x + at->w / 2);
    smoke.startY = static_cast<double>(at->y + at->h / 2);
    s->launchEffect(smoke, true);
}

void WldPathOpener::findAndHideSceneries(WorldNode *relativeTo)
{
    //Set element to be vizible
    if(relativeTo->type == WorldNode::path)
    {
        WldPathItem *y = dynamic_cast<WldPathItem *>(relativeTo);

        if(y)
        {
            y->vizible = true;
            spawnSmoke(s, y);
        }
    }
    else if(relativeTo->type == WorldNode::level)
    {
        WldLevelItem *y = dynamic_cast<WldLevelItem *>(relativeTo);

        if(y)
        {
            y->vizible = true;
            spawnSmoke(s, y);
        }
    }

    //Set all sceneries under this item to be invizible
    QVector<WorldNode * > nodes;
    s->m_indexTable.query(relativeTo->x,
                         relativeTo->y,
                         relativeTo->x + relativeTo->w,
                         relativeTo->y + relativeTo->h,
                         nodes);

    for(WorldNode *x : nodes)
    {
        if(x->type == WorldNode::scenery)
        {
            WldSceneryItem *y = dynamic_cast<WldSceneryItem *>(x);

            if(y && y->collideWith(relativeTo)) y->vizible = false;
        }
    }
}


void WldPathOpener::initFetcher()
{
    QVector<WorldNode * > lvlnodes;
    int exitCode = s->gameState->_recent_ExitCode_level;
    long lx, ly;
    lx = Maths::lRound(_current_pos.x() + s->m_indexTable.grid_half());
    ly = Maths::lRound(_current_pos.y() + s->m_indexTable.grid_half());
    #ifdef DEBUG_BUILD
    LogDebug("Initialization of the path opener....");
    #endif
    s->m_indexTable.query(lx, ly, lvlnodes);

    for(WorldNode *n : lvlnodes)
    {
        if(!n->collidePoint(lx, ly))
            continue;

        if(n->type == WorldNode::level)
        {
            WldLevelItem *wl = dynamic_cast<WldLevelItem *>(n);

            if(wl)
            {
                wl->vizible = true;
                _current_pos.setX(wl->x);
                _current_pos.setY(wl->y);
                QVector<WorldNode * > nodes;
                bool found = false;
                double x, y;
                #ifdef DEBUG_BUILD
                LogDebug("FETCH LEFT");
                #endif

                if(isAllowedSide(wl->data.left_exit, exitCode))
                {
                    //left
                    _search_pos.setX(_current_pos.x() - s->m_indexTable.grid());
                    _search_pos.setY(_current_pos.y());
                    x = _current_pos.x() + s->m_indexTable.grid_half() - s->m_indexTable.grid();
                    y = _current_pos.y() + s->m_indexTable.grid_half();
                    s->m_indexTable.query(Maths::lRound(x), Maths::lRound(y), nodes);
                    #ifdef DEBUG_BUILD

                    if(nodes.isEmpty())
                        LogDebug("No nodes at left");

                    #endif
                    fetchSideNodes(found, nodes, x, y);
                    nodes.clear();
                    #ifdef DEBUG_BUILD

                    if(found)
                        LogDebug("Objects are been detected at left");

                    #endif
                }

                #ifdef DEBUG_BUILD
                else
                    LogDebug("Left side skipped");

                #endif
                #ifdef DEBUG_BUILD
                LogDebug("FETCH RIGHT");
                #endif

                if(isAllowedSide(wl->data.right_exit, exitCode))
                {
                    //Right
                    _search_pos.setX(_current_pos.x() + s->m_indexTable.grid());
                    _search_pos.setY(_current_pos.y());
                    x = _current_pos.x() + s->m_indexTable.grid_half() + s->m_indexTable.grid();
                    y = _current_pos.y() + s->m_indexTable.grid_half();
                    s->m_indexTable.query(Maths::lRound(x), Maths::lRound(y), nodes);
                    #ifdef DEBUG_BUILD

                    if(nodes.isEmpty())
                        LogDebug("No nodes at right");

                    #endif
                    fetchSideNodes(found, nodes, x, y);
                    nodes.clear();
                    #ifdef DEBUG_BUILD

                    if(found)
                        LogDebug("Objects are been detected at right");

                    #endif
                }

                #ifdef DEBUG_BUILD
                else
                    LogDebug("Right side skipped");

                #endif
                #ifdef DEBUG_BUILD
                LogDebug("FETCH TOP");
                #endif

                if(isAllowedSide(wl->data.top_exit, exitCode))
                {
                    //Top
                    _search_pos.setX(_current_pos.x());
                    _search_pos.setY(_current_pos.y() - s->m_indexTable.grid());
                    x = _current_pos.x() + s->m_indexTable.grid_half();
                    y = _current_pos.y() + s->m_indexTable.grid_half() - s->m_indexTable.grid();
                    s->m_indexTable.query(Maths::lRound(x), Maths::lRound(y), nodes);
                    #ifdef DEBUG_BUILD

                    if(nodes.isEmpty())
                        LogDebug("No nodes at top");

                    #endif
                    fetchSideNodes(found, nodes, x, y);
                    nodes.clear();
                    #ifdef DEBUG_BUILD

                    if(found)
                        LogDebug("Objects are been detected at top");

                    #endif
                }

                #ifdef DEBUG_BUILD
                else
                    LogDebug("Top side skipped");

                #endif
                #ifdef DEBUG_BUILD
                LogDebug("FETCH BOTTOM");
                #endif

                if(isAllowedSide(wl->data.bottom_exit, exitCode))
                {
                    //Bottom
                    _search_pos.setX(_current_pos.x());
                    _search_pos.setY(_current_pos.y() + s->m_indexTable.grid());
                    x = _current_pos.x() + s->m_indexTable.grid_half();
                    y = _current_pos.y() + s->m_indexTable.grid_half() + s->m_indexTable.grid();
                    s->m_indexTable.query(Maths::lRound(x), Maths::lRound(y), nodes);
                    #ifdef DEBUG_BUILD

                    if(nodes.isEmpty())
                        LogDebug("No nodes at bottom");

                    #endif
                    fetchSideNodes(found, nodes, x, y);
                    nodes.clear();
                    #ifdef DEBUG_BUILD

                    if(found)
                        LogDebug("Objects are been detected at bottom");

                    #endif
                }

                #ifdef DEBUG_BUILD
                else
                    LogDebug("Bottom side skipped");

                #endif
                break;
            }

            force = false; // Reset "force"
        }
    }
}

void WldPathOpener::setForce()
{
    force = true;
}

bool WldPathOpener::isAllowedSide(int SideCode, int ExitCode)
{
    if(force)
        return true;

    if((ExitCode <= 0) || (SideCode == SIDE_DenyAny))
        return false;

    if((SideCode <= SIDE_AllowAny) || (SideCode == ExitCode))
        return true;

    return false;
}
