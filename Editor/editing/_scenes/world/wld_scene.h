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
#include <common_features/RTree.h>
#include <main_window/global_settings.h>
#include <data_configs/data_configs.h>
#include <data_configs/custom_data.h>
#include <PGE_File_Formats/wld_filedata.h>

#include <editing/_components/history/ihistoryelement.h>

class WorldEdit;

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
        dataconfigs       * pConfigs;   //!< Pointer to global configuration in the main window
        WorldData         * WldData;    //!< Pointer to level data storage in the sub-window class
        GraphicsWorkspace *_viewPort;   //!< Pointer to parent graphics view
        WorldEdit         *_edit;       //!< Pointer to parent edit sub-window;

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
        #define ITEM_LAST_POS                26 //QPointF
        #define ITEM_LAST_SIZE               27 //QSizeF

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
        void collectDataFromItems(WorldData &dataToStore, QList<QGraphicsItem*> items);
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

        void flipGroup(QList<QGraphicsItem *> items, bool vertical);
        void rotateGroup(QList<QGraphicsItem *> items, bool byClockwise);

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

        typedef QList<QGraphicsItem *> PGE_ItemList;
        bool checkGroupCollisions(QList<QGraphicsItem *> *items);
        QGraphicsItem * itemCollidesWith(QGraphicsItem * item, QList<QGraphicsItem *> *itemgrp = 0);
        QGraphicsItem * itemCollidesCursor(QGraphicsItem * item);

        typedef RTree<QGraphicsItem*, double, 2, double > IndexTree;
        typedef double RPoint[2];
        IndexTree tree;
        void queryItems(QRectF &zone, PGE_ItemList *resultList);
        void queryItems(double x, double y, PGE_ItemList *resultList);
        void registerElement(QGraphicsItem *item);
        void unregisterElement(QGraphicsItem *item);
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
        void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *mouseEvent);
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
        void addRemoveHistory(WorldData removedItems);
        void addPlaceHistory(WorldData placedItems);
        void addOverwriteHistory(WorldData removedItems, WorldData placedItems);
        void addMoveHistory(WorldData sourceMovedItems, WorldData targetMovedItems);
        void addChangeWorldSettingsHistory(HistorySettings::WorldSettingSubType subtype, QVariant extraData);
        void addChangeSettingsHistory(WorldData modifiedItems, HistorySettings::WorldSettingSubType subType, QVariant extraData);
        void addRotateHistory(WorldData rotatedItems, WorldData unrotatedItems);
        void addFlipHistory(WorldData flippedItems, WorldData unflippedItems);
        void addTransformHistory(WorldData transformedItems, WorldData sourceItems);

        //history modifiers
        void historyBack();
        void historyForward();
        void updateHistoryBuffer();
        //history information
        int getHistroyIndex();
        bool canUndo();
        bool canRedo();
    private:
        int historyIndex;
        QList<QSharedPointer<IHistoryElement> > operationList;

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
