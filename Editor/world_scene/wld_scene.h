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

#ifndef WldScene_H
#define WldScene_H

#include <QGraphicsScene>
#include <QGraphicsItem>
#include <QProgressDialog>
#include <QMenu>

#include <QGraphicsSceneMouseEvent>

#include <QKeyEvent>
#include <QBitmap>
#include <QPainter>
#include <QMessageBox>
#include <QApplication>
#include <QtCore>
#include <QDebug>

#include "../common_features/simple_animator.h"

#include "../file_formats/wld_filedata.h"
#include "../data_configs/data_configs.h"

#include "../common_features/logger.h"

#include "../common_features/resizer/item_resizer.h"

#include "../data_configs/custom_data.h"
#include "../main_window/global_settings.h"

class WldScene : public QGraphicsScene
{
    Q_OBJECT
public:
    WldScene(dataconfigs &configs, WorldData &FileData, QObject *parent = 0);
    ~WldScene();

    bool grid;

    void SwitchEditingMode(int EdtMode);
    int EditingMode; // 0 - selecting,  1 - erasing, 2 - placeNewObject
                     // 3 - drawing water/sand zone, 4 - placing from Buffer
    bool EraserEnabled;
    bool PasteFromBuffer;

    bool DrawMode; //Placing/drawing on map, disable selecting and dragging items
    bool disableMoveItems;
    bool contextMenuOpened;

    QPixmap musicBoxImg;

    enum EditMode
    {
        MODE_Selecting=0,
        MODE_Erasing,
        MODE_PlacingNew,
        MODE_DrawSquare,
        MODE_PasteFromClip,
        MODE_Resizing,
        MODE_SelectingOnly,
	MODE_CheckSpot
    };


    // ////////////ItemPlacers///////////
    void setItemPlacer(int itemType, unsigned long itemID=1);
    void updateCursoredNpcDirection();
    void setSquareDrawer();
    enum placingItemType
    {
        PLC_Tile=0,
        PLC_Scene,
        PLC_Path,
        PLC_Level,
        PLC_Musicbox
    };
    int placingItem;
    QGraphicsItem * cursor;
    void placeItemUnderCursor();
    void setItemSourceData(QGraphicsItem *it, QString ObjType);
    void resetCursor();

    //Copy function
    WorldData copy(bool cut = false);
    void paste(WorldData BufferIn, QPoint pos);

    LevelEditingSettings opts;

    //void makeSectionBG(int x, int y, int h, int w);
    //void makeSectionBG(QProgressDialog &progress);
    //void InitSection(int sect);
    //void drawSpace();
    //void ChangeSectionBG(int BG_Id, int SectionID=-1);
    //void DrawBG(int x, int y, int w, int h, int sctID, QPixmap &srcimg, QPixmap &srcimg2, obj_BG &bgsetup);

    void buildAnimators();
    void loadUserData(QProgressDialog &progress);

    void setTiles(QProgressDialog &progress);
    void setSceneries(QProgressDialog &progress);
    void setPaths(QProgressDialog &progress);
    void setLevels(QProgressDialog &progress);
    void setMusicBoxes(QProgressDialog &progress);

    void startAnimation();
    void stopAnimation();
    void hideWarpsAndDoors(bool visible);

    void setLocked(int type, bool lock);

    QVector<UserIMGs > uTiles;
    QVector<UserIMGs > uScenes;
    QVector<UserIMGs > uPaths;
    QVector<UserIMGs > uLevels;

    QVector<SimpleAnimator * > animates_Tiles;
    QVector<SimpleAnimator * > animates_Scenery;
    QVector<SimpleAnimator * > animates_Paths;
    QVector<SimpleAnimator * > animates_Levels;

    QGraphicsItem * itemCollidesWith(QGraphicsItem * item);

    WorldData  * WldData;

    WorldData WldBuffer;

    dataconfigs * pConfigs;

    //Object Indexing:
    QVector<wTileIndexes > index_tiles;
    QVector<wSceneIndexes > index_scenes;
    QVector<wPathIndexes > index_paths;
    QVector<wLevelIndexes > index_levels;

    bool lock_tile;
    bool lock_scene;
    bool lock_path;
    bool lock_level;
    bool lock_musbox;

    bool IsMoved;
    bool haveSelected;

    //default objects Z value
    int tileZ; // tiles
    int sceneZ; // scenery
    int pathZ; // paths
    int levelZ; // levels
    int musicZ; // musicboxes

    // //////////////////////Resizer////////////////////////
    ItemResizer * pResizer; //reisizer pointer
    //void setSectionResizer(bool enabled, bool accept=false);
    //void setEventSctSizeResizer(long event, bool enabled, bool accept=false);
    //void setBlockResizer(QGraphicsItem *targetBlock, bool enabled, bool accept=false);
    //void setPhysEnvResizer(QGraphicsItem * targetRect, bool enabled, bool accept=false);

    // ////////////HistoryManager///////////////////
    struct HistoryOperation{
        enum HistoryType{
            WORLDHISTORY_REMOVE = 0,               //Removed from map
            WORLDHISTORY_PLACE,                    //Placed new
            WORLDHISTORY_MOVE                     //moved
        };
        HistoryType type;
        //used most of Operations
        WorldData data;
        WorldData data_mod;
        //subtype (if needed)
        int subtype;
        //misc
        QVariant extraData;
    };
    struct CallbackData{
        QGraphicsItem* item;
        HistoryOperation* hist;
        //custom data
        long x, y;
    };
    //typedefs
    typedef void (WldScene::*callBackWorldTiles)(CallbackData, WorldTiles);
    typedef void (WldScene::*callBackWorldPaths)(CallbackData, WorldPaths);
    typedef void (WldScene::*callBackWorldScenery)(CallbackData, WorldScenery);
    typedef void (WldScene::*callBackWorldLevels)(CallbackData, WorldLevels);

    void addMoveHistory(WorldData sourceMovedItems, WorldData targetMovedItems);

    //history modifiers
    void historyBack();
    void historyForward();
    void cleanupRedoElements();
    //history information
    int getHistroyIndex();
    bool canUndo();
    bool canRedo();
    //Callback
    void historyRedoMoveTile(CallbackData cbData, WorldTiles data);
    void historyRedoMovePath(CallbackData cbData, WorldPaths data);
    void historyRedoMoveScenery(CallbackData cbData, WorldScenery data);
    void historyRedoMoveLevels(CallbackData cbData, WorldLevels data);
    void historyUndoMoveTile(CallbackData cbData, WorldTiles data);
    void historyUndoMovePath(CallbackData cbData, WorldPaths data);
    void historyUndoMoveScenery(CallbackData cbData, WorldScenery data);
    void historyUndoMoveLevels(CallbackData cbData, WorldLevels data);
    //miscellaneous
    void findGraphicsItem(WorldData toFind, HistoryOperation * operation, CallbackData customData,
                          callBackWorldTiles clbTiles, callBackWorldPaths clbPaths,
                          callBackWorldScenery clbScenery, callBackWorldLevels clbLevels,
                          bool ignoreTiles = false,
                          bool ignorePaths = false,
                          bool ignoreScenery = false,
                          bool ignoreLevels = false);
    QPoint calcTopLeftCorner(WorldData* data);
    QString getHistoryText(HistoryOperation operation);
    //QString getHistorySettingText(SettingSubType subType);

    void openProps();


public slots:
    void selectionChanged();


protected:
    //void contextMenuEvent(QGraphicsSceneContextMenuEvent *event);
    void mousePressEvent(QGraphicsSceneMouseEvent *mouseEvent);
    void mouseMoveEvent(QGraphicsSceneMouseEvent *mouseEvent);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *mouseEvent);
    void keyReleaseEvent ( QKeyEvent * keyEvent );

private:

    QGraphicsItem * itemCollidesCursor(QGraphicsItem * item);

    void placeTile(WorldTiles &tile, bool toGrid=false);
    void placeScenery(WorldScenery &scenery, bool toGrid=false);
    void placePath(WorldPaths &pathItem, bool toGrid=false);
    void placeLevel(WorldLevels &level, bool toGrid=false);
    void placeMusicbox(WorldMusic &musicbox, bool toGrid=false);

    void removeItemUnderCursor();

    QPoint applyGrid(QPoint source, int gridSize, QPoint gridOffset=QPoint(0,0) );

    //void setSectionBG(LevelSection section);

    QPixmap uTileImg;
    QPixmap uSceneImg;
    QPixmap uPathImg;
    QPixmap uLevelImg;

    QPixmap tImg;//Tempotary buffer

    QVector<qreal > Z;
    //qreal sbZ;

    QMenu tileMenu;
    QMenu sceneMenu;
    QMenu pathMenu;
    QMenu levelMenu;
    QMenu musicMenu;

    // The item being dragged.
    QGraphicsItem *mDragged;
    // The distance from the top left of the item to the mouse position.
    QPointF mDragOffset;

    // ////////////////HistoryManager///////////////////
    int historyIndex;
    QList<HistoryOperation> operationList;
    // /////////////////////////////////////////////////

};

#endif // WldScene_H
