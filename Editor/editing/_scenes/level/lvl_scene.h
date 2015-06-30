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
#include <QApplication>
#include <QtCore>
#include <QDebug>
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

class LvlScene : public QGraphicsScene
{
    Q_OBJECT
    friend class EditMode;
    friend class LevelEdit;
public:
    LvlScene(GraphicsWorkspace * parentView, dataconfigs &configs, LevelData &FileData, QObject *parent = 0);
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
        dataconfigs       * pConfigs;       //!< Pointer to global configuration in the main window
        LevelData         * LvlData;         //!< Pointer to level data storage in the sub-window class
        GraphicsWorkspace *_viewPort; //!< Pointer to parent graphics view
        LevelEdit         *_edit;       //!< Pointer to parent edit sub-window;

        LevelData LvlBuffer;    //!< Data buffer

        //Array Sort functions
        void sortBlockArray(QVector<LevelBlock > &blocks);
        void sortBlockArrayByPos(QVector<LevelBlock > &blocks);
        void sortBGOArray(QVector<LevelBGO > &bgos);

    public:
        //default objects Z value
        int Z_backImage;

        int Z_BGOBack2; // backround BGO
        int Z_blockSizable; // sizeble block
        int Z_BGOBack1; // backround BGO
        int Z_npcBack; // background NPC (vines)
        int Z_Block; // standart block
        int Z_npcStd; // standart NPC
        int Z_Player; //playerPointZ
        int Z_BGOFore1; // foreground BGO
        int Z_BlockFore; // lava block
        int Z_npcFore; // foreground NPC
        int Z_BGOFore2; // foreground BGO


        int Z_sys_door;
        int Z_sys_PhysEnv;
        int Z_sys_interspace1; // interSection space layer
        int Z_sys_sctBorder;


    // ///////////////////Miscellaneous////////////////////////
    public:
        LevelEditingSettings opts;

        QPoint getViewportPos(); //!< Returns current position of viewport
        QRect getViewportRect(); //!< Returns current rectangle of viewport

        //Clipboard
        LevelData copy(bool cut = false);
        void paste(LevelData BufferIn, QPoint pos);

        void openProps();               //!< Open properties box of selected items
        void Debugger_updateItemList(); //!< Refresh debugger box



    // ///////////////////GFX Manager////////////////////////
    public:
        //Object Indexing:
        QList<blocksIndexes > index_blocks;
        QList<bgoIndexes > index_bgo;
        QList<npcIndexes > index_npc;

        //Custom data containers
        QList<UserBGs > uBGs;
        QList<UserBGOs > uBGOs;
        QList<UserBlocks > uBlocks;
        QList<UserNPCs > uNPCs;

        //Animators
        QList<SimpleAnimator * > animates_BGO;
        QList<SimpleAnimator * > animates_Blocks;
        QList<AdvNpcAnimator * > animates_NPC;

        void buildAnimators();

        void startAnimation();
        void stopAnimation();

        void getConfig_Block(unsigned long item_id, long &array_index, long &animator_id, obj_block &mergedSet, bool *ok=0);
        void getConfig_BGO(unsigned long item_id, long &array_index, long &animator_id, obj_bgo &mergedSet, bool *ok=0);
        void getConfig_NPC(unsigned long item_id, long &array_index, long &animator_id, obj_npc &mergedSet, bool *ok=0);

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
        QPixmap uBlockImg;
        QBitmap npcmask;
        QPixmap uNpcImg;
        QPixmap uBgoImg;

        QPixmap tImg;   //!<Tempotary buffer


// ////////////////////////////////////////////////////////////////////////////////
// /////////////////////////////////ITEMS//////////////////////////////////////////
// ////////////////////////////////////////////////////////////////////////////////

    // ///////////////////Init Items/////////////////////////
    public:
        qlonglong last_block_arrayID;
        qlonglong last_bgo_arrayID;
        qlonglong last_npc_arrayID;

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

        long IncrementingNpcSpecialSpin;

        void placeBlock(LevelBlock &block, bool toGrid=false);
        void placeBGO(LevelBGO &bgo, bool toGrid=false);
        void placeNPC(LevelNPC &npc, bool toGrid=false);
        void placeWater(LevelPhysEnv &water, bool toGrid=false);
        void placePlayerPoint(PlayerPoint plr, bool init=false);

        void placeDoor(LevelDoor &door, bool toGrid=false);
        void placeDoorEnter(LevelDoor &door, bool toGrid=false, bool init=false);
        void placeDoorExit(LevelDoor &door, bool toGrid=false, bool init=false);


    // ///////////////////Item Locks////////////////////////////
    public:
        bool lock_bgo;
        bool lock_block;
        bool lock_npc;
        bool lock_door;
        bool lock_water;
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
        bool grid; //!< Is grid aligning enabled or disabled by menubar button

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
        bool emptyCollisionCheck;
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
        int placingItem;

        LevelData placingItems;
        LevelData overwritedItems;

        ///
        /// \brief cursor
        /// Abstact item which using to check collision before place item. Using in the placing and erasing modes
        ///
        QGraphicsItem * cursor;
        void resetCursor();

        void setItemPlacer(int itemType, unsigned long itemID=1, int dType=0);
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

        //void contextMenuEvent(QGraphicsSceneContextMenuEvent *event);
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
        ItemResizer * pResizer; //reisizer pointer

        /* Image exporting */
        QRectF captutedSize;  //!<Rectangle of scene
        bool isFullSection;   //!<Selected fragment or whole section
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
        bool historyChanged;

        //add historys
        /*
         * NOTE: when use History with Doors, LevelDoors MUST be posted individual.
         * If Door Entrance: LevelDoors.isSetIn = true and LevelDoors.isSetOut = false
         * If Door Exit: LevelDoors.isSetOut = true and LevelDoors.isSetIn = false
         *
         */
        void addRemoveHistory(LevelData removedItems);
        void addPlaceHistory(LevelData placedItems);
        void addOverwriteHistory(LevelData removedItems, LevelData placedItems);
        void addPlaceDoorHistory(LevelDoor door, bool isEntrance);
        void addMoveHistory(LevelData sourceMovedItems, LevelData targetMovedItems);
        void addChangeSettingsHistory(LevelData modifiedItems, HistorySettings::LevelSettingSubType subType, QVariant extraData);
        void addResizeSectionHistory(int sectionID, long oldLeft, long oldTop, long oldRight, long oldBottom,
                                     long newLeft, long newTop, long newRight, long newBottom);
        void addChangedLayerHistory(LevelData changedItems, QString newLayerName);
        void addResizeBlockHistory(LevelBlock bl, long oldLeft, long oldTop, long oldRight, long oldBottom,
                                   long newLeft, long newTop, long newRight, long newBottom);
        void addResizeWaterHistory(LevelPhysEnv wt, long oldLeft, long oldTop, long oldRight, long oldBottom,
                                   long newLeft, long newTop, long newRight, long newBottom);
        void addAddWarpHistory(int array_id, int listindex, int doorindex);
        void addRemoveWarpHistory(LevelDoor removedDoor);
        void addChangeWarpSettingsHistory(int array_id, HistorySettings::LevelSettingSubType subtype, QVariant extraData);
        void addAddEventHistory(LevelSMBX64Event ev);
        void addRemoveEventHistory(LevelSMBX64Event ev);
        void addDuplicateEventHistory(LevelSMBX64Event newDuplicate);
        void addChangeEventSettingsHistory(int array_id, HistorySettings::LevelSettingSubType subtype, QVariant extraData);
        void addChangedNewLayerHistory(LevelData changedItems, LevelLayer newLayer);
        void addAddLayerHistory(int array_id, QString name);
        void addRemoveLayerHistory(LevelData modData);
        void addRenameEventHistory(int array_id, QString oldName, QString newName);
        void addRenameLayerHistory(int array_id, QString oldName, QString newName);
        void addRemoveLayerAndSaveItemsHistory(LevelData modData);
        void addMergeLayer(LevelData mergedData, QString newLayerName);
        void addChangeSectionSettingsHistory(int sectionID, HistorySettings::LevelSettingSubType subtype, QVariant extraData);
        void addChangeLevelSettingsHistory(HistorySettings::LevelSettingSubType subtype, QVariant extraData);
        void addPlacePlayerPointHistory(PlayerPoint plr, QVariant oldPos);
        void addRotateHistory(LevelData rotatedItems, LevelData unrotatedItems);
        void addFlipHistory(LevelData flippedItems, LevelData unflippedItems);
        void addTransformHistory(LevelData transformedItems, LevelData sourceItems);
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

#endif // LVLSCENE_H
