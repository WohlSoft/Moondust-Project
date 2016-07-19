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

#ifndef LVLSCENE_H
#define LVLSCENE_H

#include <QGraphicsScene>
#include <QGraphicsItem>
#include <QProgressDialog>
#include <QMenu>
#include <QGraphicsSceneMouseEvent>
#include <QKeyEvent>
#include <QBitmap>
#include <QPainter>
#include <QMessageBox>
#include <QList>

#include <common_features/logger.h>
#include <common_features/simple_animator.h>
#include <common_features/npc_animator.h>
#include <common_features/resizer/item_resizer.h>
#include <common_features/graphicsworkspace.h>
#include <common_features/edit_mode_base.h>
#include <main_window/global_settings.h>
#include <data_configs/data_configs.h>
#include <data_configs/custom_data.h>
#include <PGE_File_Formats/lvl_filedata.h>
#include <PGE_File_Formats/npc_filedata.h>

#include <editing/_components/history/ihistoryelement.h>

#include <common_features/RTree.h>

class LevelEdit;
class LvlHistoryManager;
class MainWindow;

class LvlScene : public QGraphicsScene
{
    Q_OBJECT
    friend class EditMode;
    friend class LevelEdit;
    friend class LvlHistoryManager;
public:
    LvlScene(MainWindow* mw, GraphicsWorkspace * parentView, dataconfigs &configs, LevelData &FileData, QObject *parent = 0);
    ~LvlScene();

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
 * LEVEL SETTINGS
 * - Sections
 * - Layers
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
        LevelData         * m_data;
        //! Pointer to parent graphics view
        GraphicsWorkspace * m_viewPort;
        //! Pointer to parent edit sub-window;
        LevelEdit         * m_subWindow;

        //! Data buffer
        LevelData m_dataBuffer;

    public:
        /*********** Default objects Z value **************/
        //! Z-Layer of background images
        qreal Z_backImage;

        //! Z-Layer of Background-2 BGO layer
        qreal Z_BGOBack2;
        //! Z-Layer of sizable blocks
        qreal Z_blockSizable;
        //! Z-Layer of Background-1 BGO layer
        qreal Z_BGOBack1;
        //! Z-Layer of Background NPCs
        qreal Z_npcBack;
        //! Z-Layer of regular blocks
        qreal Z_Block;
        //! Z-Layer of regular NPCs
        qreal Z_npcStd;
        //! Z-Layer of playable characters
        qreal Z_Player;
        //! Z-Layer of Foreground-1 BGO layer
        qreal Z_BGOFore1;
        //! Z-Layer of Foreground NPCs
        qreal Z_npcFore;
        //! Z-Layer of Foreground blocks
        qreal Z_BlockFore;
        //! Z-Layer of Foreground-2 BGO layer
        qreal Z_BGOFore2;

        //! Z-Layer of warp points
        qreal Z_sys_door;
        //! Z-Layer of physical environment zones
        qreal Z_sys_PhysEnv;
        //! Z-Layer of the intersection space filter
        qreal Z_sys_interspace1;
        //! Z-Layer of the section border rectangle
        qreal Z_sys_sctBorder;

    // ///////////////////Miscellaneous////////////////////////
    public:
        EditingSettings m_opts;

        //! Returns current position of viewport
        QPoint getViewportPos();
        //! Returns current rectangle of viewport
        QRect  getViewportRect();

        //Clipboard
        LevelData copy(bool cut = false);
        void paste(LevelData BufferIn, QPoint pos);

        /**
         * @brief Open properties box of selected items
         */
        void openProps();
        /**
         * @brief Refresh debugger box
         */
        void Debugger_updateItemList();

    protected:
        void drawForeground(QPainter *painter, const QRectF &rect);

    // ///////////////////GFX Manager////////////////////////
    public:
        //! Common container of pre-loaded images
        QList<QPixmap> m_localImages;

        //! Container of local backgrounds configs
        QHash<int, obj_BG > m_localConfigBackgrounds;

        //! Container of local background objects configs
        PGE_DataArray<obj_bgo > m_localConfigBGOs;
        //! List of customized background objects
        QList<obj_bgo* > m_customBGOs;

        //! Container of local blocks configs
        PGE_DataArray<obj_block > m_localConfigBlocks;
        //! List of customized Blocks
        QList<obj_block* > m_customBlocks;

        //! Container of local NPCs configs
        PGE_DataArray<obj_npc > m_localConfigNPCs;
        //! List of customized NPCs
        QList<obj_npc* > m_customNPCs;

        //! Container of local background objects animators
        QList<SimpleAnimator * > m_animatorsBGO;
        //! Container of local blocks animators
        QList<SimpleAnimator * > m_animatorsBlocks;
        //! Container of local NPCs animators
        QList<AdvNpcAnimator * > m_animatorsNPC;

        //! Main animation processor
        AnimationTimer      m_animationTimer;
        /**
         * @brief Build animators without using of custom graphics
         */
        void buildAnimators();
        /**
         * @brief Start animation timer
         */
        void startAnimation();
        /**
         * @brief Stop animation processing
         */
        void stopAnimation();

        // ///////////////////Init Level/////////////////////////
        void loadUserData(QProgressDialog &progress);  //!< Load custom graphics and item settings

        void makeSectionBG(QProgressDialog &progress); //!< Drawing backgrounds of all sections
        void setBlocks(QProgressDialog &progress);     //!< Placing of exists blocks to their placed
        void setBGO(QProgressDialog &progress);        //!< Placing of exists BGO's to their placed
        void setNPC(QProgressDialog &progress);        //!< Placing of exists NPC's to their placed
        void setWaters(QProgressDialog &progress);     //!< Placing of exists Physical environment zones to their placed
        void setDoors(QProgressDialog &progress);      //!< Placing of exists Warps and doors to their placed
        void setPlayerPoints();                        //!< Placing of exists player start points.
        // ///////////////////Init Level//End////////////////////

        QPixmap getNPCimg(unsigned long npcID, int Direction=0); //!< Returns the first frame of selected NPC

    private:
        ///
        /// Dummy images for items which got errors: out of range for ID value, wrong image file, etc.
        ///
        QPixmap m_dummyBlockImg;
        QPixmap m_dummyNpcImg;
        QPixmap m_dummyBgoImg;


// ////////////////////////////////////////////////////////////////////////////////
// /////////////////////////////////ITEMS//////////////////////////////////////////
// ////////////////////////////////////////////////////////////////////////////////

    // ///////////////////Init Items/////////////////////////
    public:
        qlonglong m_lastBlockArrayID;
        qlonglong m_lastBgoArrayID;
        qlonglong m_lastNpcArrayID;

        //Defining indexes for data values of items
        #define ITEM_TYPE                    0 //String
        #define ITEM_ID                      1 //int
        #define ITEM_ARRAY_ID                2 //int
        #define ITEM_BLOCK_IS_SIZABLE        3 //bool
        #define ITEM_BLOCK_SHAPE             5 //int
        #define ITEM_NPC_BLOCK_COLLISION     7 //bool
        #define ITEM_NPC_NO_NPC_COLLISION    8 //bool
        #define ITEM_WIDTH                   9 //int
        #define ITEM_HEIGHT                  10 //int
        #define ITEM_IS_ITEM                 24 //bool
        #define ITEM_IS_CURSOR               25 //bool
        #define ITEM_LAST_POS                26 //QPointF
        #define ITEM_LAST_SIZE               27 //QSizeF

        long m_IncrementingNpcSpecialSpin;

        void placeBlock(LevelBlock &block, bool toGrid=false);
        void placeBGO(LevelBGO &bgo, bool toGrid=false);
        void placeNPC(LevelNPC &npc, bool toGrid=false);
        void placeEnvironmentZone(LevelPhysEnv &water, bool toGrid=false);
        void placePlayerPoint(PlayerPoint plr, bool init=false);

        void placeDoor(LevelDoor &door, bool toGrid=false);
        void placeDoorEnter(LevelDoor &door, bool toGrid=false, bool init=false);
        void placeDoorExit(LevelDoor &door, bool toGrid=false, bool init=false);


    // ///////////////////Item Locks////////////////////////////
    public:
        bool m_lockBgo;
        bool m_lockBlock;
        bool m_lockNpc;
        bool m_lockDoor;
        bool m_lockPhysenv;
        void setLocked(int type, bool lock);


    // ///////////////////Item Modifying/////////////////////////
    public:
        void placeItemUnderCursor();
        void placeItemsByRectArray();

        void applyArrayForItemGroup(QList<QGraphicsItem * >items);
        void applyArrayForItem(QGraphicsItem * item);
        void doorPointsSync(long arrayID, bool remove=false);
        void collectDataFromItem(LevelData& dataToStore, QGraphicsItem* item);
        void collectDataFromItems(LevelData &dataToStore, QList<QGraphicsItem *> items);
        void placeAll(const LevelData &data);

        void returnItemBackGroup(QList<QGraphicsItem * >items);
        void returnItemBack(QGraphicsItem * item);

        void removeSelectedLvlItems();
        void removeItemUnderCursor();
        void removeLvlItems(QList<QGraphicsItem * > items, bool globalHistory=false);
        void removeLvlItem(QGraphicsItem * item, bool globalHistory=false);


    // ///////////////////Aligning///////////////////////////
    public:
        QPoint applyGrid(QPoint source, int gridSize, QPoint gridOffset=QPoint(0,0) );
        void applyGroupGrid(QList<QGraphicsItem *> items, bool force=false);
        void applyGridToEach(QList<QGraphicsItem *> items);

        void flipGroup(QList<QGraphicsItem *> items, bool vertical, bool recordHistory = true, bool flipSection=false);
        void rotateGroup(QList<QGraphicsItem *> items, bool byClockwise, bool recordHistory = true, bool rotateSection=false);

        enum rotateActions
        {
            RT_RotateLeft=0,
            RT_RotateRight,
            RT_FlipH,
            RT_FlipV
        };
        void applyRotationTable(QGraphicsItem * item, rotateActions act);
        QMap<long, obj_rotation_table > local_rotation_table_blocks;
        QMap<long, obj_rotation_table > local_rotation_table_bgo;
        QMap<long, obj_rotation_table > local_rotation_table_npc;

    // ///////////////////Collisions///////////////////////////
    public:
        QList<QGraphicsItem *> collisionCheckBuffer;
        bool m_emptyCollisionCheck;
        void prepareCollisionBuffer();

        typedef QList<QGraphicsItem *> PGE_ItemList;
        bool checkGroupCollisions(PGE_ItemList *items);
        QGraphicsItem * itemCollidesWith(QGraphicsItem * item, PGE_ItemList *itemgrp = 0);
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
            MODE_Fill
        };
        void switchMode(QString title);
        void SwitchEditingMode(int EdtMode);



    // ///////////////////Placing Mode settings///////////////////////////
    public:
        enum placingItemType
        {
            PLC_Block=0,
            PLC_BGO,
            PLC_NPC,
            PLC_Water,
            PLC_Door,
            PLC_PlayerPoint,
            PLC_Section
        };
        int m_placingItemType;

        LevelData placingItems;
        LevelData overwritedItems;

        ///
        /// \brief cursor
        /// Abstact item which using to check collision before place item. Using in the placing and erasing modes
        ///
        QGraphicsItem * m_cursorItemImg;
        void resetCursor();

        void setItemPlacer(int itemType, unsigned long itemID=1, int dType=0);
        void updateCursoredNpcDirection();

        void setRectDrawer();
        void setCircleDrawer();
        void setLineDrawer();
        void setFloodFiller();

        QGraphicsSimpleTextItem * m_labelBox;
        void setLabelBoxItem(bool show=false, QPointF pos=QPointF(), QString text="");

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

        //void contextMenuEvent(QGraphicsSceneContextMenuEvent *event);
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
// ////////////////////////////LEVEL SETTINGS//////////////////////////////////////
// ////////////////////////////////////////////////////////////////////////////////

    // ///////////////////Sections///////////////////////////

    public:
        void InitSection(int sect);

        bool isInSection(long x, long y, int sectionIndex, int padding = 0);
        bool isInSection(long x, long y, int width, int height, int sectionIndex, int padding = 0);

        void ChangeSectionBG(int BG_Id, int SectionID=-1, bool forceTiled=false);

        void drawSpace();
        void DrawBG(int x, int y, int w, int h, int sctID, QPixmap &srcimg, QPixmap &srcimg2, obj_BG &bgsetup, bool forceTiled=false);

    private:
        void setSectionBG(LevelSection section, bool forceTiled=false);


    // ///////////////////Layers///////////////////////////
    public:
        void applyLayersVisible();
        void setLayerToSelected();
        void setLayerToSelected(QString lName, bool isNew=false);


    // ///////////////////Resizers///////////////////////////
    public:
        ItemResizer * m_resizeBox; //reisizer pointer

        /* Image exporting */
        QRectF captutedSize;  //!<Rectangle of scene
        bool m_captureFullSection;   //!<Selected fragment or whole section
        void setScreenshotSelector();                       //!<Capture full section size
        void setScreenshotSelector(bool enabled, bool accept = false); //!<Capture fragment
        void hideWarpsAndDoors(bool visible);
        void setTiledBackground(bool forceTiled);
    signals:
        void screenshotSizeCaptured();
        /* Image exporting  *//* end */

    public:
        void setSectionResizer(bool enabled, bool accept=false);                             //!< Resizing of section
        void setEventSctSizeResizer(long event, bool enabled, bool accept=false);            //!< Resizing of target sectopn size by event
        void setBlockResizer(QGraphicsItem *targetBlock, bool enabled, bool accept=false);   //!< Resizing of block
        void setPhysEnvResizer(QGraphicsItem * targetRect, bool enabled, bool accept=false); //!< Resizing of physical environment zone

        void applyResizers();   //!< Apply current state of resizer
        void resetResizers();   //!< Discard resizing action



// ////////////////////////////////////////////////////////////////////////////////
// //////////////////////////////HISTORY///////////////////////////////////////////
// ////////////////////////////////////////////////////////////////////////////////

    // //////////////////History Manager/////////////////////////
    public:
        //history modifiers
        void historyBack();
        void historyForward();
        void updateHistoryBuffer();
        //history information
        int  getHistroyIndex();
        bool canUndo();
        bool canRedo();

        LvlHistoryManager* m_history;
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

#endif // LVLSCENE_H
