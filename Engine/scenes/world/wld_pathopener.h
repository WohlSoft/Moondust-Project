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

#ifndef WLDPATHOPENER_H
#define WLDPATHOPENER_H

#include <QStack>
#include <common_features/pointf.h>

class WorldNode;
class WorldScene;

class WldPathOpener
{
    void construct();
public:
    WldPathOpener();
    WldPathOpener(WorldScene * _s);
    void setScene(WorldScene * _s);
    void setInterval(float _ms);
    void startAt(PGE_PointF pos);
    bool processOpener(float tickTime);
    void initFetcher();
    void setForce();

private:
    void fetchSideNodes(bool &side, QVector<WorldNode *> &nodes, float cx, float cy);
    void doFetch();
    void findAndHideSceneries(WorldNode *relativeTo);

    PGE_PointF _start_at;
    PGE_PointF _current_pos;
    PGE_PointF _search_pos;
    QStack<PGE_PointF> need_to_walk;
    QStack<WorldNode*> next;

    float interval;
    float _time;
    bool force;
    WorldScene *s;
};

#endif // WLDPATHOPENER_H
