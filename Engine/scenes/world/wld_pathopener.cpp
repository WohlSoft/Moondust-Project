/*
 * Moondust, a free game engine for platform game making
 * Copyright (c) 2014-2018 Vitaly Novichkov <admin@wohlnet.ru>
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

#include "wld_pathopener.h"
#include "wld_tilebox.h"
#include <audio/pge_audio.h>
#include <common_features/logger.h>
#include <Utils/maths.h>

#include "../scene_world.h"

WldPathOpener::WldPathOpener()
{}

WldPathOpener::WldPathOpener(WorldScene *_s)
{
    m_s = _s;
}

void WldPathOpener::setScene(WorldScene *_s)
{
    m_s = _s;
}

void WldPathOpener::setInterval(double _ms)
{
    if(_ms <= 0.0)
        _ms = 1.0;
    m_interval = _ms;
}

void WldPathOpener::startAt(PGE_PointF pos)
{
    need_to_walk.clear();
    _current_pos = pos;
    _start_at    = pos;
    initFetcher();
}

bool WldPathOpener::processOpener(double tickTime, bool *tickHappen)
{
    m_time -= tickTime;

    while((m_time < 0.0) || m_skipAnimation)
    {
        if(tickHappen)
            *tickHappen = true;
        m_time += m_interval;
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
                auto it = need_to_walk.rbegin();
                for(; it != need_to_walk.rend(); it++)
                {
                    if(_search_pos == (*it).first)
                    {
                        (*it).second = 1;
                        break;
                    }
                }
                if(it == need_to_walk.rend())
                {
                    need_to_walk.push_back({_search_pos, 0});
                    next.push_back(x);
                }
                side |= contact;
            }
        }
        else if(x->type == WorldNode::level)
        {
            bool contact = x->collidePoint(lcx, lcy);
            D_pLogDebug("Path node collision %s...", (contact ? "passed" : "missed"));

            if(contact && !x->vizible)
            {
                auto it = need_to_walk.rbegin();
                for(; it != need_to_walk.rend(); it++)
                {
                    if(_search_pos == (*it).first)
                        break;
                }
                if(it == need_to_walk.rend())
                {
                    need_to_walk.push_back({_search_pos, 1});
                    next.push_back(x);
                }
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

    while(!next.empty())
    {
        if(findAndHideSceneries())
        {
            PGE_Audio::playSoundByRole(obj_sound_role::WorldOpenPath);
            break;
        }
    }

    if(need_to_walk.empty())
        return;

    WalkBranch *branch = &need_to_walk.back();
    _current_pos = branch->first;
    need_to_walk.pop_back();
    if(branch->second == 1)
        return;

    //left
    _search_pos.setX(_current_pos.x() - m_s->m_indexTable.grid());
    _search_pos.setY(_current_pos.y());
    x = _current_pos.x() + m_s->m_indexTable.grid_half() - m_s->m_indexTable.grid();
    y = _current_pos.y() + m_s->m_indexTable.grid_half();
    m_s->m_indexTable.query(Maths::lRound(x), Maths::lRound(y), nodes);
    fetchSideNodes(found, nodes, x, y);
    nodes.clear();

    //Right
    _search_pos.setX(_current_pos.x() + m_s->m_indexTable.grid());
    _search_pos.setY(_current_pos.y());
    x = _current_pos.x() + m_s->m_indexTable.grid_half() + m_s->m_indexTable.grid();
    y = _current_pos.y() + m_s->m_indexTable.grid_half();
    m_s->m_indexTable.query(Maths::lRound(x), Maths::lRound(y), nodes);
    fetchSideNodes(found, nodes, x, y);
    nodes.clear();

    //Top
    _search_pos.setX(_current_pos.x());
    _search_pos.setY(_current_pos.y() - m_s->m_indexTable.grid());
    x = _current_pos.x() + m_s->m_indexTable.grid_half();
    y = _current_pos.y() + m_s->m_indexTable.grid_half() - m_s->m_indexTable.grid();
    m_s->m_indexTable.query(Maths::lRound(x), Maths::lRound(y), nodes);
    fetchSideNodes(found, nodes, x, y);
    nodes.clear();

    //Bottom
    _search_pos.setX(_current_pos.x());
    _search_pos.setY(_current_pos.y() + m_s->m_indexTable.grid());
    x = _current_pos.x() + m_s->m_indexTable.grid_half();
    y = _current_pos.y() + m_s->m_indexTable.grid_half() + m_s->m_indexTable.grid();
    m_s->m_indexTable.query(Maths::lRound(x), Maths::lRound(y), nodes);
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

bool WldPathOpener::findAndHideSceneries()
{
    bool processed = false;
    WorldNode *relativeTo = next.back();

    //Set element to be vizible
    if(relativeTo->type == WorldNode::path)
    {
        WldPathItem *y = dynamic_cast<WldPathItem *>(relativeTo);

        if(y && !y->vizible)
        {
            y->vizible = true;
            processed = true;
            spawnSmoke(m_s, y);
        }
    }
    else if(relativeTo->type == WorldNode::level)
    {
        WldLevelItem *y = dynamic_cast<WldLevelItem *>(relativeTo);

        if(y && !y->vizible)
        {
            y->vizible = true;
            processed = true;
            spawnSmoke(m_s, y);
        }
    }

    //Set all sceneries under this item to be invizible
    std::vector<WorldNode * > nodes;
    m_s->m_indexTable.query(relativeTo->x, relativeTo->y,
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

    popProcessed();
    return processed;
}

void WldPathOpener::popProcessed()
{
    while(!next.empty())
    {
        WorldNode *relativeTo = next.back();
        if(relativeTo->type == WorldNode::path)
        {
            WldPathItem *y = dynamic_cast<WldPathItem *>(relativeTo);
            if(y && !y->vizible)
                break;
        }
        else if(relativeTo->type == WorldNode::level)
        {
            WldLevelItem *y = dynamic_cast<WldLevelItem *>(relativeTo);
            if(y && !y->vizible)
                break;
        }
        next.pop_back();
    }
}

void WldPathOpener::initFetcher()
{
    m_skipAnimation = false;
    m_time = 0.0;
    std::vector<WorldNode * > lvlnodes;
    int exitCode = m_s->m_gameState->_recent_ExitCode_level;
    long lx, ly;
    lx = Maths::lRound(_current_pos.x() + m_s->m_indexTable.grid_half());
    ly = Maths::lRound(_current_pos.y() + m_s->m_indexTable.grid_half());

    D_pLogDebugNA("Initialization of the path opener....");
    m_s->m_indexTable.query(lx, ly, lvlnodes);

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
                    _search_pos.setX(_current_pos.x() - m_s->m_indexTable.grid());
                    _search_pos.setY(_current_pos.y());
                    x = _current_pos.x() + m_s->m_indexTable.grid_half() - m_s->m_indexTable.grid();
                    y = _current_pos.y() + m_s->m_indexTable.grid_half();
                    m_s->m_indexTable.query(Maths::lRound(x), Maths::lRound(y), nodes);
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

                D_pLogDebugNA("FETCH RIGHT");
                if(isAllowedSide(wl->data.right_exit, exitCode))
                {
                    //Right
                    _search_pos.setX(_current_pos.x() + m_s->m_indexTable.grid());
                    _search_pos.setY(_current_pos.y());
                    x = _current_pos.x() + m_s->m_indexTable.grid_half() + m_s->m_indexTable.grid();
                    y = _current_pos.y() + m_s->m_indexTable.grid_half();
                    m_s->m_indexTable.query(Maths::lRound(x), Maths::lRound(y), nodes);
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
                D_pLogDebugNA("FETCH TOP");

                if(isAllowedSide(wl->data.top_exit, exitCode))
                {
                    //Top
                    _search_pos.setX(_current_pos.x());
                    _search_pos.setY(_current_pos.y() - m_s->m_indexTable.grid());
                    x = _current_pos.x() + m_s->m_indexTable.grid_half();
                    y = _current_pos.y() + m_s->m_indexTable.grid_half() - m_s->m_indexTable.grid();
                    m_s->m_indexTable.query(Maths::lRound(x), Maths::lRound(y), nodes);
                    #ifdef DEBUG_BUILD
                    if(nodes.empty())
                        D_pLogDebugNA("No nodes at top");
                    #endif
                    fetchSideNodes(found, nodes, x, y);
                    nodes.clear();

                    #ifdef DEBUG_BUILD
                    if(found)
                        D_pLogDebugNA("Objects are been detected at top");
                    #endif
                }

                #ifdef DEBUG_BUILD
                else
                    D_pLogDebugNA("Top side skipped");
                #endif
                D_pLogDebugNA("FETCH BOTTOM");

                if(isAllowedSide(wl->data.bottom_exit, exitCode))
                {
                    //Bottom
                    _search_pos.setX(_current_pos.x());
                    _search_pos.setY(_current_pos.y() + m_s->m_indexTable.grid());
                    x = _current_pos.x() + m_s->m_indexTable.grid_half();
                    y = _current_pos.y() + m_s->m_indexTable.grid_half() + m_s->m_indexTable.grid();
                    m_s->m_indexTable.query(Maths::lRound(x), Maths::lRound(y), nodes);

                    #ifdef DEBUG_BUILD
                    if(nodes.empty())
                        D_pLogDebugNA("No nodes at bottom");
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

            m_forceOpen = false; // Reset "force"
        }
    }
}

void WldPathOpener::setForce()
{
    m_forceOpen = true;
}

void WldPathOpener::skipAnimation()
{
    m_skipAnimation = true;
}

PGE_PointF WldPathOpener::curPos()
{
    return _current_pos;
}

bool WldPathOpener::isAllowedSide(int SideCode, int ExitCode)
{
    if(m_forceOpen)
        return true;

    if((ExitCode <= 0) || (SideCode == SIDE_DenyAny))
        return false;

    if((SideCode <= SIDE_AllowAny) || (SideCode == ExitCode))
        return true;

    return false;
}

void WldPathOpener::debugRender(double camX, double camY)
{
    for(WorldNode *w : next)
    {
        GlRenderer::renderRect(float(w->x - camX),
                               float(w->y - camY),
                               w->w, w->h,
                               1.0f, 0.0f, 0.0f, 0.5f);
    }
}
