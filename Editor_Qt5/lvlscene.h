/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2014 Vitaly Novichkov <admin@wohlnet.ru>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA
 */

#ifndef LVLSCENE_H
#define LVLSCENE_H

#include <QGraphicsScene>
#include <QProgressDialog>
#include "lvl_filedata.h"
#include "dataconfigs.h"

struct UserBGOs
{
    QPixmap image;
    QBitmap mask;
    unsigned long id;
};

struct UserBlocks
{
    QPixmap image;
    QBitmap mask;
    unsigned long id;
};

struct UserBGs
{
    QPixmap image;
    QPixmap second_image;
    unsigned long id;
};

class LvlScene : public QGraphicsScene
{
    Q_OBJECT
public:
    LvlScene(QObject *parent = 0);

    QGraphicsItemGroup *bgoback;
    QGraphicsItemGroup *npcback;
    QGraphicsItemGroup *blocks;
    QGraphicsItemGroup *npcfore;
    QGraphicsItemGroup *bgofore;
    QGraphicsItemGroup *cursor;

    void loadUserData(LevelData FileData, QProgressDialog &progress, dataconfigs &configs);
    void setBlocks(LevelData FileData, QProgressDialog &progress, dataconfigs &configs);
    void setBGO(LevelData FileData, QProgressDialog &progress, dataconfigs &configs);
    void setNPC(LevelData FileData, QProgressDialog &progress);
    void setWaters(LevelData FileData, QProgressDialog &progress);
    void setDoors(LevelData FileData, QProgressDialog &progress);

    //void makeSectionBG(int x, int y, int h, int w);
    void makeSectionBG(LevelData FileData, QProgressDialog &progress, dataconfigs &configs);
    QPixmap drawSizebleBlock(int w, int h, QPixmap srcimg);
    void sortBlockArray(QVector<LevelBlock > &blocks);

    QVector<UserBGs > uBGs;
    QVector<UserBGOs > uBGOs;
    QVector<UserBlocks > uBlocks;

private:
    QGraphicsItem * itemCollidesWith(QGraphicsItem * item);
    void placeBox(float x, float y);
    void placeBlock(LevelBlock block, dataconfigs &configs);

};

#endif // LVLSCENE_H
