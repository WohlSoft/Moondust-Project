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

#include "wld_pathopener.h"
#include "wld_tilebox.h"
#include <audio/pge_audio.h>
#include <common_features/logger.h>
#include <Utils/maths.h>

#include "../scene_world.h"

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

        if(need_to_walk.empty() && next.empty())
            return false;
    }

    return true;
}

void WldPathOpener::fetchSideNodes(bool &side, std::vector<WorldNode * > &nodes, double cx, double cy)
{
    side = false;
    long lcx = Maths::lRound(cx);
    long lcy = Maths::lRound(cy);

    for(WorldNode *x : nodes)
    {
        if(x->type == WorldNode::path)
        {
            bool contact = x->collidePoint(lcx, lcy);
            D_pLogDebug("Level node collision %s...", (contact ? "passed" : "missed"));

            if(contact && !x->vizible)
            {
                auto it = need_to_walk.begin();
                for(; it != need_to_walk.end(); it++)
                {
                    if(_search_pos == (*it))
                        break;
                }
                if(it == need_to_walk.end())
                    need_to_walk.push_back(_search_pos);

                next.push_back(x);
                side |= contact;
            }
        }
        else if(x->type == WorldNode::level)
        {
            bool contact = x->collidePoint(lcx, lcy);
            D_pLogDebug("Path node collision %s...", (contact ? "passed" : "missed"));

            if(contact && !x->vizible)
            {
                next.push_back(x);
                side |= contact;
            }
        }
    }
}

void WldPathOpener::doFetch()
{
    std::vector<WorldNode * > nodes;
    bool found = false;
    double x, y;
    #ifdef DEBUG_BUILD
    LogDebug("Fetch leaf....");
    #endif

    if(!next.empty())
    {
        findAndHideSceneries(next.back());
        next.pop_back();
        PGE_Audio::playSoundByRole(obj_sound_role::WorldOpenPath);
    }

    if(need_to_walk.empty())
        return;

    _current_pos = need_to_walk.back();
    need_to_walk.pop_back();

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
    std::vector<WorldNode * > nodes;
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
            if(y && y->collideWith(relativeTo))
                y->vizible = false;
        }
    }
}


void WldPathOpener::initFetcher()
{
    std::vector<WorldNode * > lvlnodes;
    int exitCode = s->gameState->_recent_ExitCode_level;
    long lx, ly;
    lx = Maths::lRound(_current_pos.x() + s->m_indexTable.grid_half());
    ly = Maths::lRound(_current_pos.y() + s->m_indexTable.grid_half());
    D_pLogDebug("Initialization of the path opener....");
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
                std::vector<WorldNode * > nodes;
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
                    if(nodes.empty())
                        pLogDebug("No nodes at left");
                    #endif
                    fetchSideNodes(found, nodes, x, y);
                    nodes.clear();

                    #ifdef DEBUG_BUILD
                    if(found)
                        pLogDebug("Objects are been detected at left");
                    #endif
                }

                #ifdef DEBUG_BUILD
                else
                    pLogDebug("Left side skipped");
                #endif

                D_pLogDebug("FETCH RIGHT");
                if(isAllowedSide(wl->data.right_exit, exitCode))
                {
                    //Right
                    _search_pos.setX(_current_pos.x() + s->m_indexTable.grid());
                    _search_pos.setY(_current_pos.y());
                    x = _current_pos.x() + s->m_indexTable.grid_half() + s->m_indexTable.grid();
                    y = _current_pos.y() + s->m_indexTable.grid_half();
                    s->m_indexTable.query(Maths::lRound(x), Maths::lRound(y), nodes);
                    #ifdef DEBUG_BUILD
                    if(nodes.empty())
                        pLogDebug("No nodes at right");
                    #endif
                    fetchSideNodes(found, nodes, x, y);
                    nodes.clear();
                    #ifdef DEBUG_BUILD
                    if(found)
                        pLogDebug("Objects are been detected at right");
                    #endif
                }

                #ifdef DEBUG_BUILD
                else
                    LogDebug("Right side skipped");

                #endif
                D_pLogDebug("FETCH TOP");

                if(isAllowedSide(wl->data.top_exit, exitCode))
                {
                    //Top
                    _search_pos.setX(_current_pos.x());
                    _search_pos.setY(_current_pos.y() - s->m_indexTable.grid());
                    x = _current_pos.x() + s->m_indexTable.grid_half();
                    y = _current_pos.y() + s->m_indexTable.grid_half() - s->m_indexTable.grid();
                    s->m_indexTable.query(Maths::lRound(x), Maths::lRound(y), nodes);
                    #ifdef DEBUG_BUILD
                    if(nodes.empty())
                        D_pLogDebug("No nodes at top");
                    #endif
                    fetchSideNodes(found, nodes, x, y);
                    nodes.clear();

                    #ifdef DEBUG_BUILD
                    if(found)
                        D_pLogDebug("Objects are been detected at top");
                    #endif
                }

                #ifdef DEBUG_BUILD
                else
                    D_pLogDebug("Top side skipped");
                #endif
                D_pLogDebug("FETCH BOTTOM");

                if(isAllowedSide(wl->data.bottom_exit, exitCode))
                {
                    //Bottom
                    _search_pos.setX(_current_pos.x());
                    _search_pos.setY(_current_pos.y() + s->m_indexTable.grid());
                    x = _current_pos.x() + s->m_indexTable.grid_half();
                    y = _current_pos.y() + s->m_indexTable.grid_half() + s->m_indexTable.grid();
                    s->m_indexTable.query(Maths::lRound(x), Maths::lRound(y), nodes);

                    #ifdef DEBUG_BUILD
                    if(nodes.empty())
                        D_pLogDebug("No nodes at bottom");
                    #endif
                    fetchSideNodes(found, nodes, x, y);
                    nodes.clear();
                    #ifdef DEBUG_BUILD
                    if(found)
                        pLogDebug("Objects are been detected at bottom");
                    #endif
                }

                #ifdef DEBUG_BUILD
                else
                    pLogDebug("Bottom side skipped");
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
