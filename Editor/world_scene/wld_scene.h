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

#include "../common_features/graphicsworkspace.h"

#include "../common_features/edit_mode_base.h"

class WldScene : public QGraphicsScene
{
    Q_OBJECT
    friend class EditMode;
    friend class WorldEdit;
public:
    WldScene(GraphicsWorkspace * parentView, dataconfigs &configs, WorldData &FileData, QObject *parent = 0);
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


    SimpleAnimator * pointAnimation;
    QPixmap pointImg;
    bool isSelectionDialog; // If scene created in the point selection dialog
                            // disable all cols via activeWldEditWin()-> and disable rightclick
                            // for change mode into "select"

    QPoint selectedPoint; // SELECTING Point on the map
    bool selectedPointNotUsed; //point is not selected (used because QPoint::isNull()
                               //will work in the x0-y0, but this point are usable)
                               //If value true, initial position will be 0x0, else already placed point

    QPixmap musicBoxImg;
    void setPoint(QPoint p);   //Set Point item
    void unserPointSelector(); //remove point item from world map

    QGraphicsItem * pointTarget;

    QList<EditMode *> EditModes;
    EditMode * CurrentMode;
    enum EditModeID
    {
        MODE_Selecting=0,
        MODE_HandScroll,
        MODE_Erasing,
        MODE_PlacingNew,
        MODE_DrawSquare,
        MODE_PasteFromClip,
        MODE_Resizing,
        MODE_SelectingOnly,
        MODE_Line,
        MODE_SetPoint,
        MODE_Fill
    };
    void switchMode(QString title);


    // ////////////ItemPlacers/////////////////////////////
    void setItemPlacer(int itemType, unsigned long itemID=1);
    void updateCursoredNpcDirection();
    void setSquareDrawer();
    void setLineDrawer();
    void setFloodFiller();
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
    QGraphicsSimpleTextItem * messageBox;
    void setMessageBoxItem(bool show=false, QPointF pos=QPointF(), QString text="");
    void placeItemUnderCursor();
    void placeItemsByRectArray();
    WorldData placingItems;
    WorldData overwritedItems;
    //void setItemSourceData(QGraphicsItem *it, QString ObjType);
    void resetCursor();

    bool mouseLeft; //Left mouse key is pressed
    bool mouseMid;  //Middle mouse key is pressed
    bool mouseRight;//Right mouse key is pressed

    bool mouseMoved; //Mouse was moved with right mouseKey
    void mousePressEvent(QGraphicsSceneMouseEvent *mouseEvent);
    bool MousePressEventOnly;
    void mouseMoveEvent(QGraphicsSceneMouseEvent *mouseEvent);
    bool MouseMoveEventOnly;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *mouseEvent);
    bool MouseReleaseEventOnly;

    void keyPressEvent ( QKeyEvent * keyEvent );
    void keyReleaseEvent ( QKeyEvent * keyEvent );

    // ////////////ItemPlacers/////////////////////////////

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
    void setSemiTransparentPaths(bool semiTransparent);

    //void hideWarpsAndDoors(bool visible);
    void hideMusicBoxes(bool visible);
    void hidePathAndLevels(bool visible);

    void setLocked(int type, bool lock);

    //Debugger box
    void Debugger_updateItemList();

    QVector<UserIMGs > uTiles;
    QVector<UserIMGs > uScenes;
    QVector<UserIMGs > uPaths;
    QVector<UserIMGs > uLevels;

    QVector<SimpleAnimator * > animates_Tiles;
    QVector<SimpleAnimator * > animates_Scenery;
    QVector<SimpleAnimator * > animates_Paths;
    QVector<SimpleAnimator * > animates_Levels;

    // //////////////////////////////////
    QList<QGraphicsItem *> collisionCheckBuffer;
    bool emptyCollisionCheck;
    void prepareCollisionBuffer();

    bool checkGroupCollisions(QList<QGraphicsItem *> *items);
    QGraphicsItem * itemCollidesWith(QGraphicsItem * item, QList<QGraphicsItem *> *itemgrp = 0);
    QGraphicsItem * itemCollidesCursor(QGraphicsItem * item);

    void placeTile(WorldTiles &tile, bool toGrid=false);
    void placeScenery(WorldScenery &scenery, bool toGrid=false);
    void placePath(WorldPaths &pathItem, bool toGrid=false);
    void placeLevel(WorldLevels &level, bool toGrid=false);
    void placeMusicbox(WorldMusic &musicbox, bool toGrid=false);

    //the last Array ID's, which used before hold mouse key
    qlonglong last_tile_arrayID;
    qlonglong last_scene_arrayID;
    qlonglong last_path_arrayID;
    qlonglong last_level_arrayID;
    qlonglong last_musicbox_arrayID;

    void removeItemUnderCursor();

    QPoint applyGrid(QPoint source, int gridSize, QPoint gridOffset=QPoint(0,0) );
    void applyGroupGrid(QList<QGraphicsItem *> items, bool force=false);

    void applyArrayForItemGroup(QList<QGraphicsItem * >items);
    void applyArrayForItem(QGraphicsItem * item);

    void returnItemBackGroup(QList<QGraphicsItem * >items);
    void returnItemBack(QGraphicsItem * item);
    // //////////////////////////////////

    WorldData  * WldData;

    WorldData WldBuffer;

    dataconfigs * pConfigs;
    GraphicsWorkspace *_viewPort;

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
    void resetResizers();
    void applyResizers();
    //void setSectionResizer(bool enabled, bool accept=false);
    //void setEventSctSizeResizer(long event, bool enabled, bool accept=false);
    //void setBlockResizer(QGraphicsItem *targetBlock, bool enabled, bool accept=false);
    //void setPhysEnvResizer(QGraphicsItem * targetRect, bool enabled, bool accept=false);
    void setScreenshotSelector(bool enabled, bool accept = false);

    QRectF captutedSize;

    // ////////////HistoryManager///////////////////
    struct HistoryOperation{
        enum HistoryType{
            WORLDHISTORY_REMOVE = 0,               //Removed from map
            WORLDHISTORY_PLACE,                    //Placed new
            WORLDHISTORY_OVERWRITE,
            WORLDHISTORY_MOVE,                     //moved
            WORLDHISTORY_CHANGEDSETTINGSWORLD,
            WORLDHISTORY_CHANGEDSETTINGSWORLDITEM
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

    enum SettingSubType{
        SETTING_HUB = 0,
        SETTING_RESTARTAFTERFAIL,
        SETTING_TOTALSTARS,
        SETTING_INTROLEVEL,
        SETTING_PATHBACKGROUND,
        SETTING_BIGPATHBACKGROUND,
        SETTING_ALWAYSVISIBLE,
        SETTING_GAMESTARTPOINT,
        SETTING_LEVELFILE,
        SETTING_LEVELTITLE,
        SETTING_DOORID,
        SETTING_PATHBYTOP,
        SETTING_PATHBYRIGHT,
        SETTING_PATHBYBOTTOM,
        SETTING_PATHBYLEFT,
        SETTING_GOTOX,
        SETTING_GOTOY,
        SETTING_CHARACTER,
        SETTING_WORLDTITLE
    };

    //typedefs
    typedef void (WldScene::*callBackWorldTiles)(CallbackData, WorldTiles);
    typedef void (WldScene::*callBackWorldPaths)(CallbackData, WorldPaths);
    typedef void (WldScene::*callBackWorldScenery)(CallbackData, WorldScenery);
    typedef void (WldScene::*callBackWorldLevels)(CallbackData, WorldLevels);
    typedef void (WldScene::*callBackWorldMusicbox)(CallbackData, WorldMusic);

    void addRemoveHistory(WorldData removedItems);
    void addPlaceHistory(WorldData placedItems);
    void addOverwriteHistory(WorldData removedItems, WorldData placedItems);
    void addMoveHistory(WorldData sourceMovedItems, WorldData targetMovedItems);
    void addChangeWorldSettingsHistory(SettingSubType subtype, QVariant extraData);
    void addChangeSettingsHistory(WorldData modifiedItems, SettingSubType subType, QVariant extraData);

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
    void historyRedoMoveMusic(CallbackData cbData, WorldMusic data);
    void historyUndoMoveTile(CallbackData cbData, WorldTiles data);
    void historyUndoMovePath(CallbackData cbData, WorldPaths data);
    void historyUndoMoveScenery(CallbackData cbData, WorldScenery data);
    void historyUndoMoveLevels(CallbackData cbData, WorldLevels data);
    void historyUndoMoveMusic(CallbackData cbData, WorldMusic data);
    //Callbackfunctions: Remove
    void historyRemoveTiles(CallbackData cbData, WorldTiles data);
    void historyRemovePath(CallbackData cbData, WorldPaths data);
    void historyRemoveScenery(CallbackData cbData, WorldScenery data);
    void historyRemoveLevels(CallbackData cbData, WorldLevels data);
    void historyRemoveMusic(CallbackData cbData, WorldMusic data);
    //Callbackfunctions: Levels
    void historyUndoSettingPathBackgroundLevel(CallbackData cbData, WorldLevels data);
    void historyRedoSettingPathBackgroundLevel(CallbackData cbData, WorldLevels data);
    void historyUndoSettingBigPathBackgroundLevel(CallbackData cbData, WorldLevels data);
    void historyRedoSettingBigPathBackgroundLevel(CallbackData cbData, WorldLevels data);
    void historyUndoSettingAlwaysVisibleLevel(CallbackData cbData, WorldLevels data);
    void historyRedoSettingAlwaysVisibleLevel(CallbackData cbData, WorldLevels data);
    void historyUndoSettingGameStartPointLevel(CallbackData cbData, WorldLevels data);
    void historyRedoSettingGameStartPointLevel(CallbackData cbData, WorldLevels data);
    void historyUndoSettingLevelfileLevel(CallbackData cbData, WorldLevels data);
    void historyRedoSettingLevelfileLevel(CallbackData cbData, WorldLevels data);
    void historyUndoSettingLeveltitleLevel(CallbackData cbData, WorldLevels data);
    void historyRedoSettingLeveltitleLevel(CallbackData cbData, WorldLevels data);
    void historyUndoSettingDoorIDLevel(CallbackData cbData, WorldLevels data);
    void historyRedoSettingDoorIDLevel(CallbackData cbData, WorldLevels data);
    void historyUndoSettingPathByTopLevel(CallbackData cbData, WorldLevels data);
    void historyRedoSettingPathByTopLevel(CallbackData cbData, WorldLevels data);
    void historyUndoSettingPathByRightLevel(CallbackData cbData, WorldLevels data);
    void historyRedoSettingPathByRightLevel(CallbackData cbData, WorldLevels data);
    void historyUndoSettingPathByBottomLevel(CallbackData cbData, WorldLevels data);
    void historyRedoSettingPathByBottomLevel(CallbackData cbData, WorldLevels data);
    void historyUndoSettingPathByLeftLevel(CallbackData cbData, WorldLevels data);
    void historyRedoSettingPathByLeftLevel(CallbackData cbData, WorldLevels data);
    void historyUndoSettingGotoXLevel(CallbackData cbData, WorldLevels data);
    void historyRedoSettingGotoXLevel(CallbackData cbData, WorldLevels data);
    void historyUndoSettingGotoYLevel(CallbackData cbData, WorldLevels data);
    void historyRedoSettingGotoYLevel(CallbackData cbData, WorldLevels data);

    //miscellaneous
    void findGraphicsItem(WorldData toFind, HistoryOperation * operation, CallbackData customData,
                          callBackWorldTiles clbTiles, callBackWorldPaths clbPaths,
                          callBackWorldScenery clbScenery, callBackWorldLevels clbLevels, callBackWorldMusicbox clbMusic,
                          bool ignoreTiles = false,
                          bool ignorePaths = false,
                          bool ignoreScenery = false,
                          bool ignoreLevels = false,
                          bool ignoreMusicbox = false);
    QPoint calcTopLeftCorner(WorldData* data);
    QString getHistoryText(HistoryOperation operation);
    QString getHistorySettingText(SettingSubType subType);

    void openProps();

    void removeSelectedWldItems();
    void removeWldItems(QList<QGraphicsItem * > items, bool globalHistory=false);
    void removeWldItem(QGraphicsItem * item, bool globalHistory=false);

public slots:
    void selectionChanged();

signals:
    void pointSelected(QPoint point);
    void screenshotSizeCaptured();

private:

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
