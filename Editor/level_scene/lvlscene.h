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

#include <QGraphicsSceneMouseEvent>

#include <QGraphicsItemAnimation>

#include <QKeyEvent>
#include <QBitmap>
#include <QPainter>
#include <QMessageBox>
#include <QApplication>
#include <QtCore>
#include <QDebug>


#include "lvl_filedata.h"
#include "dataconfigs.h"

#include "logger.h"

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

struct LevelEditingSettings
{
    bool animationEnabled;
    bool collisionsEnabled;
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
                     // 3 - drawing water/sand zone, 4 - placing from Buffer
    bool EraserEnabled;
    bool PasteFromBuffer;

    bool disableMoveItems;

    //Event Flags
    bool wasPasted;
    bool doCopy;
    bool doCut;
    bool historyChanged;

    //Copy function
    LevelData copy(bool cut = false);
    void paste(LevelData BufferIn, QPoint pos);

    LevelEditingSettings opts;

    //void makeSectionBG(int x, int y, int h, int w);
    void makeSectionBG(LevelData FileData, QProgressDialog &progress);

    void drawSpace(LevelData FileData);
    void ChangeSectionBG(int BG_Id, LevelData &FileData);

    void loadUserData(LevelData FileData, QProgressDialog &progress);

    void setBlocks(LevelData FileData, QProgressDialog &progress);
    void setBGO(LevelData FileData, QProgressDialog &progress);
    void setNPC(LevelData FileData, QProgressDialog &progress);
    void setWaters(LevelData FileData, QProgressDialog &progress);
    void setDoors(LevelData FileData, QProgressDialog &progress);
    void setPlayerPoints();

    void startBlockAnimation();
    void stopAnimation();

    void setLocked(int type, bool lock);

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

    LevelData LvlBuffer;

    dataconfigs * pConfigs;

    //Object Indexing:
    QVector<blocksIndexes > index_blocks;
    QVector<bgoIndexes > index_bgo;

    bool lock_bgo;
    bool lock_block;
    bool lock_npc;
    bool lock_door;
    bool lock_water;

    bool IsMoved;
    bool haveSelected;

    //default objects Z value
    int blockZ; // standart block
    int blockZs; // sizeble block
    int blockZl; // lava block
    int playerZ; //playerPointZ
    int bgoZf; // foreground BGO
    int bgoZb; // backround BGO
    int npcZf; // foreground NPC
    int npcZb; // standart NPC
    int doorZ;
    int waterZ;
    int bgZ;
    int spaceZ1; // interSection space layer
    int spaceZ2;

    // ////////////HistoryManager///////////////////
    struct HistoryOperation{
        enum HistoryType{
            LEVELHISTORY_REMOVE = 0, //Removed from map
            LEVELHISTORY_PLACE,      //Placed new
            LEVELHISTORY_MODIFY      //(moved, changed settings of items)
        };
        HistoryType type;
        //used most of Operations
        LevelData data;
        LevelData data_mod;
    };
    void addRemoveHistory(LevelData removedItems);
	void addPlaceHistory(LevelData placedItems);
    void historyBack();
    void historyForward();
    int getHistroyIndex();
    void cleanupRedoElements();
    bool canUndo();
    bool canRedo();
    // ////////////////////////////////////////////

protected:
    //void contextMenuEvent(QGraphicsSceneContextMenuEvent *event);
    void mousePressEvent(QGraphicsSceneMouseEvent *mouseEvent);
    void mouseMoveEvent(QGraphicsSceneMouseEvent *mouseEvent);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *mouseEvent);
    void keyReleaseEvent ( QKeyEvent * keyEvent );

private:

    QGraphicsItem * itemCollidesCursor(QGraphicsItem * item);

    void placeBlock(LevelBlock &block, bool toGrid=false);
    void placeBGO(LevelBGO &bgo, bool toGrid=false);
    void placeDoor(LevelDoors &door, bool toGrid=false);
    void placeNPC(LevelNPC &npc, bool toGrid=false);
    void placeWater(LevelWater &water, bool toGrid=false);

    void removeItemUnderCursor();

    QPoint applyGrid(QPoint source, int gridSize, QPoint gridOffset=QPoint(0,0) );

    void setSectionBG(LevelSection section);

    QGraphicsItem * cursor;

    QPixmap uBlockImg;

    QBitmap npcmask;
    QPixmap uNpcImg;

    QPixmap uBgoImg;

    QPixmap tImg;//Tempotary buffer

    QVector<qreal > Z;
    //qreal sbZ;

    QMenu blockMenu;
    QMenu bgoMenu;
    QMenu npcMenu;
    QMenu waterMenu;
    QMenu DoorMenu;

    // The item being dragged.
    QGraphicsItem *mDragged;
    // The distance from the top left of the item to the mouse position.
    QPointF mDragOffset;

    // ////////////////HistoryManager///////////////////
    int historyIndex;
    QList<HistoryOperation> operationList;
    // /////////////////////////////////////////////////

};

#endif // LVLSCENE_H
