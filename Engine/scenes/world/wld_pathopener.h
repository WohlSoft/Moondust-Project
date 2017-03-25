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

#ifndef WLDPATHOPENER_H
#define WLDPATHOPENER_H

#include <deque>
#include <vector>
#include <utility>
#include <cstdint>
#include <common_features/pointf.h>

class WorldNode;
class WorldScene;

class WldPathOpener
{
    public:
        WldPathOpener();
        WldPathOpener(WorldScene *_s);
        void setScene(WorldScene *_s);
        void setInterval(double _ms);
        void startAt(PGE_PointF pos);
        bool processOpener(double tickTime, bool *tickHappen = nullptr);

        void initFetcher();
        void setForce();
        void skipAnimation();

        PGE_PointF curPos();

        enum SideExitCode
        {
            SIDE_AllowAny   = -1,
            SIDE_DenyAny    = 0
        };
        bool isAllowedSide(int SideCode, int ExitCode);
        void debugRender(double camX, double camY);

    private:
        void fetchSideNodes(bool &side, std::vector<WorldNode *> &nodes, double cx, double cy);
        void doFetch();
        bool findAndHideSceneries();
        void popProcessed();

        PGE_PointF _start_at;
        PGE_PointF _current_pos;
        PGE_PointF _search_pos;
        typedef std::pair<PGE_PointF /*Point position*/, uint8_t /*Not-opened Branches*/> WalkBranch;
        std::deque<WalkBranch>  need_to_walk;
        std::deque<WorldNode *> next;

        //! Allows to immediately skip long path opening animation
        bool    m_skipAnimation = false;
        //! Interval between every cell opening
        double  m_interval = 1.0;
        //! Wait timer betweeen cell opening
        double  m_time    = 0.0;
        //! Force opening of the closed cells without exit code checking
        bool    m_forceOpen    = false;
        //! Pointer to the parent world map scene
        WorldScene  *m_s   = nullptr;
};

#endif // WLDPATHOPENER_H
