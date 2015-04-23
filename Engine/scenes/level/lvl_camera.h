/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2014 Vitaly Novichkov <admin@wohlnet.ru>
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

#include <Box2D/Box2D.h>
#include <physics/base_object.h>
#include <graphics/graphics.h>

#include <vector>
#include <PGE_File_Formats/file_formats.h>

typedef QList<PGE_Phys_Object *> PGE_RenderList;

class PGE_LevelCamera
{
public:
    PGE_LevelCamera();
    ~PGE_LevelCamera();
    int w(); //!< Width
    int h(); //!< Height
    qreal posX(); //!< Position X
    qreal posY(); //!< Position Y

    void setWorld(b2World * wld);
    void init(float x, float y, float w, float h);

    void setPos(float x, float y);
    void setSize(int w, int h);
    void setOffset(int x, int y);
    void update();
    void drawBackground();

    void changeSection(LevelSection &sct);
    void changeSectionBorders(long left, long top, long right, long bottom);
    void resetLimits();

    PGE_RenderList renderObjects();

    LevelSection *section;

    bool isWarp;
    bool RightOnly;
    bool ExitOffscreen;

    int limitLeft;
    int limitRight;
    int limitTop;
    int limitBottom;

    /// Limits of section motion
    int s_top;
    int s_bottom;
    int s_left;
    int s_right;
    long BackgroundID;

    int offset_x;
    int offset_y;

    float pos_x;
    float pos_y;

    void setMusicRoot(QString dir);

    /**************Fader**************/
    float fader_opacity;
    float target_opacity;
    float fade_step;
    int fadeSpeed;
    void setFade(int speed, float target, float step);
    static unsigned int nextOpacity(unsigned int x, void *p);
    void fadeStep();
    SDL_TimerID fader_timer_id;
    /**************Fader**************/

private:
    PGE_RenderList objects_to_render;

    int width;
    int height;
    QString musicRootDir;

    b2World * worldPtr;
};

#endif // LVL_CAMERA_H
