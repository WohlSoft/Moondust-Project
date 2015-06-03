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

#ifndef LVL_CAMERA_H
#define LVL_CAMERA_H

#include <physics/base_object.h>
#include <graphics/graphics.h>
#include <common_features/fader.h>

#include <QList>
#include <PGE_File_Formats/file_formats.h>

class   PGE_Phys_Object;
typedef QVector<PGE_Phys_Object *> PGE_RenderList;

class LVL_Background;
class LVL_Section;

class PGE_LevelCamera
{
    friend class LVL_Background;
    friend class LVL_Section;
public:
    PGE_LevelCamera();
    PGE_LevelCamera(const PGE_LevelCamera &cam);
    ~PGE_LevelCamera();
    int w(); //!< Width
    int h(); //!< Height
    qreal posX(); //!< Position X
    qreal posY(); //!< Position Y

    void init(float x, float y, float w, float h);

    void setPos(float x, float y);
    void setSize(int w, int h);
    void setOffset(int x, int y);
    void update(float ticks);
    void drawBackground();
    void drawForeground();

    void changeSection(LVL_Section *sct);
    void changeSectionBorders(long left, long top, long right, long bottom);
    void resetLimits();

    PGE_RenderList &renderObjects();

    LevelSection* section;
    LVL_Section * cur_section;

    int offset_x;
    int offset_y;

    float pos_x;
    float pos_y;

    /**************Fader**************/
    PGE_Fader fader;
    /**************Fader**************/
private:
    void sortElements();
    PGE_RenderList objects_to_render;

    int width;
    int height;
};

#endif // LVL_CAMERA_H
