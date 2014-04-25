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
#include <QGraphicsItem>
#include <QProgressDialog>
#include <QMenu>

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
    unsigned int q;//0 - only first; 1 - only second; 2 - fitst and seconf
};

class LvlScene : public QGraphicsScene
{
    Q_OBJECT
public:
    LvlScene(dataconfigs &configs, LevelData &FileData, QObject *parent = 0);
    ~LvlScene();

    QList<QGraphicsPixmapItem *> BgItem;

    bool grid;
    int EditingMode; // 0 - selecting,  1 - erasing, 2 - placeNewObject
    bool EraserEnabled;

    //void makeSectionBG(int x, int y, int h, int w);
    void makeSectionBG(LevelData FileData, QProgressDialog &progress);

    void drawSpace(LevelData FileData);
    void ChangeSectionBG(int BG_Id, LevelData &FileData);

    void loadUserData(LevelData FileData, QProgressDialog &progress, dataconfigs &configs);
    void setBlocks(LevelData FileData, QProgressDialog &progress, dataconfigs &configs);
    void setBGO(LevelData FileData, QProgressDialog &progress);
    void setNPC(LevelData FileData, QProgressDialog &progress);
    void setWaters(LevelData FileData, QProgressDialog &progress);
    void setDoors(LevelData FileData, QProgressDialog &progress);
    void startBlockAnimation();

    //QPixmap drawSizebleBlock(int w, int h, QPixmap srcimg);
    void DrawBG(int x, int y, int w, int h, QPixmap srcimg, QPixmap srcimg2, obj_BG &bgsetup, QGraphicsPixmapItem * &target);

    //Array Sort functions
    void sortBlockArray(QVector<LevelBlock > &blocks);
    void sortBGOArray(QVector<LevelBGO > &bgos);

    QVector<UserBGs > uBGs;
    QVector<UserBGOs > uBGOs;
    QVector<UserBlocks > uBlocks;
    QGraphicsItem * itemCollidesWith(QGraphicsItem * item);

    LevelData  * LvlData;

    dataconfigs * pConfigs;

protected:
    //void contextMenuEvent(QGraphicsSceneContextMenuEvent *event);
    void mousePressEvent(QGraphicsSceneMouseEvent *mouseEvent);
    void mouseMoveEvent(QGraphicsSceneMouseEvent *mouseEvent);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *mouseEvent);

private:
    QGraphicsItem * itemCollidesCursor(QGraphicsItem * item);
    void placeBox(float x, float y);
    void placeBlock(LevelBlock &block, dataconfigs &configs);
    void placeBGO(LevelBGO &bgo);
    void placeDoor(LevelDoors &door);

    void setSectionBG(LevelSection section);

    QGraphicsItem * cursor;

    QPixmap uBlockImg;

    QBitmap npcmask;
    QPixmap uNpcImg;

    QPixmap uBgoImg;

    QPixmap tImg;//Tempotary buffer

    //default objects Z value
    int blockZ; // standart block
    int blockZs; // sizeble block
    int blockZl; // lava block
    int bgoZf; // foreground BGO
    int bgoZb; // backround BGO
    int npcZf; // foreground NPC
    int npcZb; // standart NPC
    int doorZ;
    int waterZ;
    int bgZ;
    int spaceZ1; // interSection space layer
    int spaceZ2;

    QVector<qreal > Z;
    qreal sbZ;

    QMenu blockMenu;
    QMenu bgoMenu;
    QMenu npcMenu;
    QMenu waterMenu;
    QMenu DoorMenu;

    // The item being dragged.
    QGraphicsItem *mDragged;
    // The distance from the top left of the item to the mouse position.
    QPointF mDragOffset;

};

#endif // LVLSCENE_H
