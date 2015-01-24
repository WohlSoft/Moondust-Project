/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2014-2015 Vitaly Novichkov <admin@wohlnet.ru>
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

#include <common_features/simple_animator.h>
#include <common_features/graphicsworkspace.h>
#include <common_features/edit_mode_base.h>
#include <common_features/logger.h>
#include <common_features/resizer/item_resizer.h>
#include <main_window/global_settings.h>
#include <data_configs/data_configs.h>
#include <data_configs/custom_data.h>
#include <file_formats/wld_filedata.h>

#include <editing/_components/history/ihistoryelement.h>


class WldScene : public QGraphicsScene
{
    Q_OBJECT
    friend class EditMode;
    friend class WorldEdit;
public:
    WldScene(GraphicsWorkspace * parentView, dataconfigs &configs, WorldData &FileData, QObject *parent = 0);
    ~WldScene();

/* //////////////////////Contents/////////////////////////////
 *
 * COMMON
 * - Common
 * - Miscellaneous
 * - GFX Manager
 *
 * ITEMS
 * - Init Items
 * - Item Locks
 * - Item Modifying
 * - Aligning
 * - Collisions
 *
 * EDITING
 * - Edit modes
 * - Placing Mode settings
 * - Mouse Events
 *
 * WORLD SETTINGS
 * - Resizers
 *
 * HISTORY
 * - History Manager
 *
 */


// ////////////////////////////////////////////////////////////////////////////////
// /////////////////////////////////COMMON/////////////////////////////////////////
// ////////////////////////////////////////////////////////////////////////////////

    // ///////////////////Common////////////////////////
    public:
        dataconfigs * pConfigs;       //!< Pointer to global configuration in the main window
        WorldData  * WldData;         //!< Pointer to level data storage in the sub-window class
        GraphicsWorkspace *_viewPort; //!< Pointer to parent graphics view

        WorldData WldBuffer;    //!< Data buffer

    public:
        //default objects Z value
        int tileZ; // tiles
        int sceneZ; // scenery
        int pathZ; // paths
        int levelZ; // levels
        int musicZ; // musicboxes

    // ///////////////////Miscellaneous////////////////////////
    public:
        LevelEditingSettings opts;

        QPoint getViewportPos(); //!< Returns current position of viewport
        QRect getViewportRect(); //!< Returns current rectangle of viewport

        //Clipboard
        WorldData copy(bool cut = false);
        void paste(WorldData BufferIn, QPoint pos);

        void openProps();                //!< Open properties box of selected items
        void Debugger_updateItemList();  //!< Refresh debugger box

    // ///////////////////GFX Manager////////////////////////
    public:
        //Object Indexing:
        QList<wTileIndexes > index_tiles;
        QList<wSceneIndexes > index_scenes;
        QList<wPathIndexes > index_paths;
        QList<wLevelIndexes > index_levels;

        //Custom data containers
        QList<UserIMGs > uTiles;
        QList<UserIMGs > uScenes;
        QList<UserIMGs > uPaths;
        QList<UserIMGs > uLevels;

        //Animators
        QList<SimpleAnimator * > animates_Tiles;
        QList<SimpleAnimator * > animates_Scenery;
        QList<SimpleAnimator * > animates_Paths;
        QList<SimpleAnimator * > animates_Levels;

        void buildAnimators();

        void startAnimation();
        void stopAnimation();

        void getConfig_Tile(unsigned long item_id, long &array_index, long &animator_id, obj_w_tile &mergedSet, bool *ok=0);
        void getConfig_Scenery(unsigned long item_id, long &array_index, long &animator_id, obj_w_scenery &mergedSet, bool *ok=0);
        void getConfig_Path(unsigned long item_id, long &array_index, long &animator_id, obj_w_path &mergedSet, bool *ok=0);
        void getConfig_Level(unsigned long item_id, long &array_index, long &animator_id, obj_w_level &mergedSet, bool *ok=0);

        // ///////////////////Init World/////////////////////////
        void loadUserData(QProgressDialog &progress);

        void setTiles(QProgressDialog &progress);
        void setSceneries(QProgressDialog &progress);
        void setPaths(QProgressDialog &progress);
        void setLevels(QProgressDialog &progress);
        void setMusicBoxes(QProgressDialog &progress);
        // ///////////////////Init World//End////////////////////

    private:
        ///
        /// Dummy images for items which got errors: out of range for ID value, wrong image file, etc.
        ///
        QPixmap uTileImg;
        QPixmap uSceneImg;
        QPixmap uPathImg;
        QPixmap uLevelImg;

        QPixmap tImg;   //!Tempotary buffer


// ////////////////////////////////////////////////////////////////////////////////
// /////////////////////////////////ITEMS//////////////////////////////////////////
// ////////////////////////////////////////////////////////////////////////////////

    // ///////////////////Init Items/////////////////////////
    public:

        //the last Array ID's, which used before hold mouse key
        qlonglong last_tile_arrayID;
        qlonglong last_scene_arrayID;
        qlonglong last_path_arrayID;
        qlonglong last_level_arrayID;
        qlonglong last_musicbox_arrayID;

        //Defining indexes for data values of items
        #define ITEM_TYPE                    0 //String
        #define ITEM_ID                      1 //int
        #define ITEM_ARRAY_ID                2 //int
        #define ITEM_BLOCK_IS_SIZABLE        3 //bool
        #define ITEM_NPC_BLOCK_COLLISION     7 //bool
        #define ITEM_NPC_NO_NPC_COLLISION    8 //bool
        #define ITEM_WIDTH                   9 //int
        #define ITEM_HEIGHT                  10 //int
        #define ITEM_IS_ITEM                 24 //bool
        #define ITEM_IS_CURSOR               25 //bool

        void placeTile(WorldTiles &tile, bool toGrid=false);
        void placeScenery(WorldScenery &scenery, bool toGrid=false);
        void placePath(WorldPaths &pathItem, bool toGrid=false);
        void placeLevel(WorldLevels &level, bool toGrid=false);
        void placeMusicbox(WorldMusic &musicbox, bool toGrid=false);

    // ///////////////////Point selector/////////////////////////
    public:
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

    signals:
        void pointSelected(QPoint point);


    // ///////////////////Item Locks////////////////////////////
    public:
        bool lock_tile;
        bool lock_scene;
        bool lock_path;
        bool lock_level;
        bool lock_musbox;
        void setLocked(int type, bool lock);

    // ///////////////////Item Modifying/////////////////////////
    public:
        void placeItemUnderCursor();
        void placeItemsByRectArray();

        void applyArrayForItemGroup(QList<QGraphicsItem * >items);
        void applyArrayForItem(QGraphicsItem * item);
        void collectDataFromItem(WorldData& dataToStore, QGraphicsItem* item);
        void placeAll(const WorldData &data);

        void returnItemBackGroup(QList<QGraphicsItem * >items);
        void returnItemBack(QGraphicsItem * item);

        void removeSelectedWldItems();
        void removeItemUnderCursor();
        void removeWldItems(QList<QGraphicsItem * > items, bool globalHistory=false);
        void removeWldItem(QGraphicsItem * item, bool globalHistory=false);

    // ///////////////////Aligning///////////////////////////
    public:
        bool grid;

        QPoint applyGrid(QPoint source, int gridSize, QPoint gridOffset=QPoint(0,0) );
        void applyGridToEach(QList<QGraphicsItem *> items);
        void applyGroupGrid(QList<QGraphicsItem *> items, bool force=false);

        void flipGroup(QList<QGraphicsItem *> items, bool vertical, bool recordHistory=true);
        void rotateGroup(QList<QGraphicsItem *> items, bool byClockwise, bool recordHistory=true);

        enum rotateActions
        {
            RT_RotateLeft=0,
            RT_RotateRight,
            RT_FlipH,
            RT_FlipV
        };
        void applyRotationTable(QGraphicsItem * item, rotateActions act);
        QMap<long, obj_rotation_table > local_rotation_table_tiles;
        QMap<long, obj_rotation_table > local_rotation_table_sceneries;
        QMap<long, obj_rotation_table > local_rotation_table_paths;
        QMap<long, obj_rotation_table > local_rotation_table_levels;

    // ///////////////////Collisions///////////////////////////
    public:
        QList<QGraphicsItem *> collisionCheckBuffer;
        bool emptyCollisionCheck;
        void prepareCollisionBuffer();

        bool checkGroupCollisions(QList<QGraphicsItem *> *items);
        QGraphicsItem * itemCollidesWith(QGraphicsItem * item, QList<QGraphicsItem *> *itemgrp = 0);
        QGraphicsItem * itemCollidesCursor(QGraphicsItem * item);
        // //////////////////////////////////



// ////////////////////////////////////////////////////////////////////////////////
// /////////////////////////////////EDITING////////////////////////////////////////
// ////////////////////////////////////////////////////////////////////////////////


    // ///////////////////Edit modes///////////////////////////
    public:
        int EditingMode; // 0 - selecting,  1 - erasing, 2 - placeNewObject
                         // 3 - drawing water/sand zone, 4 - placing from Buffer
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
        void SwitchEditingMode(int EdtMode);

    // ///////////////////Placing Mode settings///////////////////////////
    public:
        enum placingItemType
        {
            PLC_Tile=0,
            PLC_Scene,
            PLC_Path,
            PLC_Level,
            PLC_Musicbox
        };
        int placingItem;

        WorldData placingItems;
        WorldData overwritedItems;

        ///
        /// \brief cursor
        /// Abstact item which using to check collision before place item. Using in the placing and erasing modes
        ///
        QGraphicsItem * cursor;
        void resetCursor();

        void setItemPlacer(int itemType, unsigned long itemID=1);
        void updateCursoredNpcDirection();

        void setSquareDrawer();
        void setLineDrawer();
        void setFloodFiller();

        QGraphicsSimpleTextItem * messageBox;
        void setMessageBoxItem(bool show=false, QPointF pos=QPointF(), QString text="");

    // ///////////////////Mouse Events///////////////////////////
    public:
        bool IsMoved;
        bool haveSelected;

        bool EraserEnabled;
        bool PasteFromBuffer;

        bool DrawMode; //Placing/drawing on map, disable selecting and dragging items
        bool disableMoveItems;
        bool contextMenuOpened;

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

// ////////////////////////////////////////////////////////////////////////////////
// ////////////////////////////WORLD SETTINGS//////////////////////////////////////
// ////////////////////////////////////////////////////////////////////////////////

    // ///////////////////Resizers///////////////////////////
    public:
        ItemResizer * pResizer; //reisizer pointer

        QRectF captutedSize;
        void setScreenshotSelector(bool enabled, bool accept = false);
        void setSemiTransparentPaths(bool semiTransparent);
        void hideMusicBoxes(bool visible);
        void hidePathAndLevels(bool visible);
    signals:
        void screenshotSizeCaptured();

    public:
        void resetResizers();
        void applyResizers();



// ////////////////////////////////////////////////////////////////////////////////
// //////////////////////////////HISTORY///////////////////////////////////////////
// ////////////////////////////////////////////////////////////////////////////////

    // //////////////////History Manager/////////////////////////
    public:
        struct HistoryOperation{
            enum HistoryType{
                WORLDHISTORY_REMOVE = 0,               //Removed from map
                WORLDHISTORY_PLACE,                    //Placed new
                WORLDHISTORY_OVERWRITE,
                WORLDHISTORY_MOVE,                     //moved
                WORLDHISTORY_CHANGEDSETTINGSWORLD,
                WORLDHISTORY_CHANGEDSETTINGSWORLDITEM,
                WORLDHISTORY_ROTATE,
                WORLDHISTORY_FLIP
            };
            HistoryType type;
            //used most of Operations
            WorldData data;
            WorldData data_mod;
            //subtype (if needed)
            int subtype;
            //misc
            QVariant extraData;
            //new System
            QSharedPointer<IHistoryElement> newElement;
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
        void addRotateHistory(WorldData rotatedItems, bool byClockwise);
        void addFlipHistory(WorldData flippedItems, bool vertical);

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

        QList<QGraphicsItem*> findGraphicsItems(WorldData& toFind, ItemTypes::itemTypesMultiSelectable findingFilter);
        QString getHistoryText(HistoryOperation operation);
        QString getHistorySettingText(SettingSubType subType);
    private:
        int historyIndex;
        QList<HistoryOperation> operationList;

// ////////////////////Unsorted slots/////////////////////////////
// ///////Please move them into it's category/////////////////////

public:

protected:

private:

public slots:

private slots:

signals:

public slots:
    void selectionChanged();
};

#endif // WldScene_H
