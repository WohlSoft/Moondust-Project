/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2014-2016 Vitaly Novichkov <admin@wohlnet.ru>
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
class MainWindow;

class WldScene : public QGraphicsScene
{
    Q_OBJECT
    friend class EditMode;
    friend class WorldEdit;
public:
    WldScene(MainWindow* mw, GraphicsWorkspace * parentView, dataconfigs &configs, WorldData &FileData, QObject *parent = 0);
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
        //! Main window pointer
        MainWindow        * m_mw;
        //! Pointer to global configuration in the main window
        dataconfigs       * m_configs;
        //! Pointer to level data storage in the sub-window class
        WorldData         * m_data;
        //! Pointer to parent graphics view
        GraphicsWorkspace * m_viewPort;
        //! Pointer to parent edit sub-window;
        WorldEdit         * m_subWindow;

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
        EditingSettings opts;

        QPoint getViewportPos(); //!< Returns current position of viewport
        QRect getViewportRect(); //!< Returns current rectangle of viewport

        //Clipboard
        WorldData copy(bool cut = false);
        void paste(WorldData BufferIn, QPoint pos);

        void openProps();                //!< Open properties box of selected items
        void Debugger_updateItemList();  //!< Refresh debugger box
    protected:
        void drawForeground(QPainter *painter, const QRectF &rect);

    // ///////////////////GFX Manager////////////////////////
    public:
        //! Common container of pre-loaded custom images
        QList<QPixmap> custom_images;

        PGE_DataArray<obj_w_tile > uTiles;
        QList<obj_w_tile* > custom_Tiles;

        PGE_DataArray<obj_w_scenery > uScenes;
        QList<obj_w_scenery* > custom_Scenes;

        PGE_DataArray<obj_w_path > uPaths;
        QList<obj_w_path* > custom_Paths;

        PGE_DataArray<obj_w_level > uLevels;
        QList<obj_w_level* > custom_Levels;

        //!Terrain tiles animators
        QList<SimpleAnimator* > m_animatorsTerrain;
        //!Scenery animators
        QList<SimpleAnimator* > m_animatorsScenery;
        //!Paths animators
        QList<SimpleAnimator* > m_animatorsPaths;
        //!Levels animators
        QList<SimpleAnimator* > m_animatorsLevels;

        //! Main animation processor
        AnimationTimer      animator;

        void buildAnimators();

        void startAnimation();
        void stopAnimation();

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
        QPixmap m_dummyTerrainImg;
        QPixmap m_dummySceneryImg;
        QPixmap m_dummyPathImg;
        QPixmap m_dummyLevelImg;

        QPixmap tImg;   //!Tempotary buffer


// ////////////////////////////////////////////////////////////////////////////////
// /////////////////////////////////ITEMS//////////////////////////////////////////
// ////////////////////////////////////////////////////////////////////////////////

    // ///////////////////Init Items/////////////////////////
    public:

        //the last Array ID's, which used before hold mouse key
        qlonglong m_lastTerrainArrayID;
        qlonglong m_lastSceneryArrayID;
        qlonglong m_lastPathArrayID;
        qlonglong m_lastLevelArrayID;
        qlonglong m_lastMusicBoxArrayID;

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

        void placeTile(WorldTerrainTile &tile, bool toGrid=false);
        void placeScenery(WorldScenery &scenery, bool toGrid=false);
        void placePath(WorldPathTile &pathItem, bool toGrid=false);
        void placeLevel(WorldLevelTile &level, bool toGrid=false);
        void placeMusicbox(WorldMusicBox &musicbox, bool toGrid=false);

    // ///////////////////Point selector/////////////////////////
    public:
        /******************************************************************/
        /* TODO: Make a separated structure/class to inject this into it  */
        /******************************************************************/
        SimpleAnimator pointAnimation;
        QPixmap pointImg;
        bool isSelectionDialog; // If scene created in the point selection dialog
                                // disable all cols via activeWldEditWin()-> and disable rightclick
                                // for change mode into "select"

        QPoint selectedPoint; // SELECTING Point on the map
        bool selectedPointNotUsed; //point is not selected (used because QPoint::isNull()
                                   //will work in the x0-y0, but this point are usable)
                                   //If value true, initial position will be 0x0, else already placed point

        QPixmap m_musicBoxImg;
        void setPoint(QPoint p);   //Set Point item
        void unserPointSelector(); //remove point item from world map

        QGraphicsItem * pointTarget;

    signals:
        void pointSelected(QPoint point);


    // ///////////////////Item Locks////////////////////////////
    public:
        bool m_lockTerrain;
        bool m_lockScenery;
        bool m_lockPath;
        bool m_lockLevel;
        bool m_lockMusicBox;
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
        bool m_emptyCollisionCheck;
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
        int m_editMode; // 0 - selecting,  1 - erasing, 2 - placeNewObject
                         // 3 - drawing water/sand zone, 4 - placing from Buffer
        QList<EditMode *> m_editModes;
        EditMode * m_editModeObj;
        enum EditModeID
        {
            MODE_Selecting=0,
            MODE_HandScroll,
            MODE_Erasing,
            MODE_PlacingNew,
            MODE_DrawRect,
            MODE_DrawCircle,
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
            PLC_Terrain=0,
            PLC_Scene,
            PLC_Path,
            PLC_Level,
            PLC_Musicbox
        };
        int m_placingItemType;

        WorldData m_placingItems;
        WorldData m_overwritedItems;

        ///
        /// \brief cursor
        /// Abstact item which using to check collision before place item. Using in the placing and erasing modes
        ///
        QGraphicsItem * m_cursorItemImg;
        void resetCursor();

        void setItemPlacer(int itemType, unsigned long itemID=1);
        void updateCursoredNpcDirection();

        void setRectDrawer();
        void setCircleDrawer();
        void setLineDrawer();
        void setFloodFiller();

        QGraphicsSimpleTextItem * m_labelBox;
        void setMessageBoxItem(bool show=false, QPointF pos=QPointF(), QString text="");

    // ///////////////////Mouse Events///////////////////////////
    public:
        bool m_mouseIsMovedAfterKey;

        bool m_eraserIsEnabled;
        bool m_pastingMode;

        bool m_busyMode; //Placing/drawing on map, disable selecting and dragging items
        bool m_disableMoveItems;
        bool m_contextMenuIsOpened;

        bool m_mouseLeftPressed; //Left mouse key is pressed
        bool m_mouseMidPressed;  //Middle mouse key is pressed
        bool m_mouseRightPressed;//Right mouse key is pressed

        bool m_mouseIsMoved; //Mouse was moved with right mouseKey

        void mousePressEvent(QGraphicsSceneMouseEvent *mouseEvent);
        void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *mouseEvent);
        bool m_skipChildMousePressEvent;
        void mouseMoveEvent(QGraphicsSceneMouseEvent *mouseEvent);
        bool m_skipChildMouseMoveEvent;
        void mouseReleaseEvent(QGraphicsSceneMouseEvent *mouseEvent);
        bool m_skipChildMousReleaseEvent;
        void keyPressEvent ( QKeyEvent * keyEvent );
        void keyReleaseEvent ( QKeyEvent * keyEvent );

// ////////////////////////////////////////////////////////////////////////////////
// ////////////////////////////WORLD SETTINGS//////////////////////////////////////
// ////////////////////////////////////////////////////////////////////////////////

    // ///////////////////Resizers///////////////////////////
    public:
        ItemResizer * m_resizeBox; //reisizer pointer

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
