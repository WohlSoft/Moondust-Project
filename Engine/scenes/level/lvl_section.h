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

#ifndef LVL_SECTION_H
#define LVL_SECTION_H

#include <PGE_File_Formats/lvl_filedata.h>
#include <common_features/RTree/RTree.h>
#include <common_features/rectf.h>

#include "lvl_backgrnd.h"


typedef QVector<PGE_Phys_Object* > R_itemList;

class PGE_Phys_Object;
class PGE_LevelCamera;

class LVL_Section
{
    friend class PGE_Phys_Object;
    friend class PGE_LevelCamera;
public:
    LVL_Section();
    LVL_Section(const LVL_Section& _sct);
    ~LVL_Section();
    void setData(LevelSection _d);
    LevelSection data;
    void registerElement(PGE_Phys_Object* item);
    void unregisterElement(PGE_Phys_Object* item);
    void queryItems(PGE_RectF zone, R_itemList* resultList);
    void queryItems(double x, double y, R_itemList* resultList);

    void changeSectionBorders(long left, long top, long right, long bottom);
    void changeLimitBorders(long left, long top, long right, long bottom);
    void resetLimits();

    void initBG();
    void setMusicRoot(QString _path);
    int  getBgId();

    void playMusic();
    void resetMusic();
    void setMusic(int musID);
    void setMusic(QString musFile);

    void renderBG(float x, float y, float w, float h);

    void setBG(int bgID);
    void resetBG();

    bool isAutoscroll;
    float _autoscrollVelocityX;
    float _autoscrollVelocityY;

    PGE_RectF sectionRect();
    PGE_RectF sectionLimitBox();

    bool isWarp();
    bool RightOnly();
    bool ExitOffscreen();

    int  getPhysicalEnvironment();

private:
    typedef double RPoint[2];
    typedef RTree<PGE_Phys_Object*, double, 2, double > IndexTree;
    IndexTree tree;

    QString        music_root;
    int            curMus;
    QString        curCustomMus;
    int            curBgID;
    LVL_Background _background;

    bool isInit;

    /// Limits of section motion
    PGE_RectF limitBox;

    /// Default section box
    PGE_RectF sectionBox;
};

#endif // LVL_SECTION_H
