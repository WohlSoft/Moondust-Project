/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2014-2026 Vitaly Novichkov <admin@wohlnet.ru>
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

#pragma once
#ifndef LVLSCENE_H
#define LVLSCENE_H

#include <QGraphicsScene>
#include <QGraphicsItem>
#include <QProgressDialog>
#include <QMenu>
#include <QMap>
#include <QSet>
#include <QHash>
#include <QGraphicsSceneMouseEvent>
#include <QKeyEvent>
#include <QBitmap>
#include <QPainter>
#include <QMessageBox>
#include <QList>
#include <Utils/vptrlist.h>

#include "../common/base_scene.h"
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

class LvlBaseItem;
class ItemBlock;
class ItemBGO;
class ItemNPC;
class ItemPhysEnv;
class ItemDoor;
class ItemPlayerPoint;

class LvlScene : public MoondustBaseScene
{
    Q_OBJECT
    friend class EditMode;
    friend class LevelEdit;
    friend class LvlHistoryManager;
public:
    LvlScene(MainWindow *mw, GraphicsWorkspace *parentView,
             DataConfig &configs, LevelData &FileData, QObject *parent = nullptr);
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
    //! Pointer to global configuration in the main window
    DataConfig        *m_configs = nullptr;
    //! Pointer to level data storage in the sub-window class
    LevelData          *m_data = nullptr;
    //! Pointer to parent edit sub-window;
    LevelEdit          *m_subWindow = nullptr;

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
    QPoint  getViewportPos();
    //! Returns current rectangle of viewport
    QRect   getViewportRect();

    /**********Clipboard*************/
    /**
     * @brief Copy selected elements inti clipboard
     * @param cut Delete elements from map after copying
     * @return copied data buffer
     */
    LevelData copy(bool cut = false);
    /**
     * @brief Paste elements in the clipboard
     * @param BufferIn Input clipboard contents
     * @param pos Position of left-top corner where paste
     */
    void paste(LevelData &BufferIn, QPoint pos);

    /**
     * @brief Open properties box of selected items
     */
    void openProps();
    /**
     * @brief Refresh debugger box
     */
    void Debugger_updateItemList();

protected:
    void drawForeground(QPainter *painter, const QRectF &rect) override;

    // ///////////////////GFX Manager////////////////////////
public:
    //! Pre-loaded custom images bank
    VPtrList<QPixmap> m_localImages;

    //! Container of local backgrounds configs
    QHash<int, obj_BG > m_localConfigBackgrounds;

    //! Container of local background objects configs
    PGE_DataArray<obj_bgo > m_localConfigBGOs;
    //! List of customized background objects
    QList<obj_bgo * > m_customBGOs;

    //! Container of local blocks configs
    PGE_DataArray<obj_block > m_localConfigBlocks;
    //! List of customized Blocks
    QList<obj_block * > m_customBlocks;

    //! Container of local NPCs configs
    PGE_DataArray<obj_npc > m_localConfigNPCs;
    //! List of customized NPCs
    QList<obj_npc * > m_customNPCs;

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

    /**
     * @brief Set visibility state to elements meta-signs
     * @param visible true to show meta-signs, false to hide them
     */
    void setMetaSignsVisibility(bool visible);

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

    QPixmap getNPCimg(unsigned long npcID, int Direction = 0); //!< Returns the first frame of selected NPC

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

    long m_IncrementingNpcSpecialSpin;

    ItemBlock *placeBlock(LevelBlock &block, bool toGrid = false);
    ItemBGO *placeBGO(LevelBGO &bgo, bool toGrid = false);
    ItemNPC *placeNPC(LevelNPC &npc, bool toGrid = false);
    ItemPhysEnv *placeEnvironmentZone(LevelPhysEnv &water, bool toGrid = false);
    ItemPlayerPoint *placePlayerPoint(PlayerPoint plr, bool init = false);

    void placeDoor(LevelDoor &door, bool toGrid = false);
    void placeDoorEnter(LevelDoor &door, bool toGrid = false, bool init = false);
    void placeDoorExit(LevelDoor &door, bool toGrid = false, bool init = false);


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
    void applyArrayForItem(QGraphicsItem *item);
    void doorPointsSync(unsigned int arrayID, bool remove = false);
    void collectDataFromItem(LevelData &dataToStore, QGraphicsItem *item);
    void collectDataFromItems(LevelData &dataToStore, QList<QGraphicsItem *> items);
    void placeAll(const LevelData &data);

    void returnItemBackGroup(QList<QGraphicsItem * >items);
    void returnItemBack(QGraphicsItem *item);

    void removeSelectedLvlItems();
    void removeItemUnderCursor();
    void removeLvlItems(QList<QGraphicsItem * > items, bool globalHistory = false, bool forceInvis = false);
    void removeLvlItem(QGraphicsItem *item, bool globalHistory = false);


    // ///////////////////Aligning///////////////////////////
public:
    QPoint applyGrid(QPoint source, int gridSize, QPoint gridOffset = QPoint(0, 0));
    void applyGroupGrid(QList<QGraphicsItem *> items, bool force = false);
    void applyGridToEach(QList<QGraphicsItem *> items);

    void flipGroup(QList<QGraphicsItem *> items, bool vertical, bool recordHistory = true, bool flipSection = false);
    void rotateGroup(QList<QGraphicsItem *> items, bool byClockwise, bool recordHistory = true, bool rotateSection = false);

    enum rotateActions
    {
        RT_RotateLeft = 0,
        RT_RotateRight,
        RT_FlipH,
        RT_FlipV
    };
    void applyRotationTable(QGraphicsItem *item, rotateActions act);
    QMap<long, obj_rotation_table > local_rotation_table_blocks;
    QMap<long, obj_rotation_table > local_rotation_table_bgo;
    QMap<long, obj_rotation_table > local_rotation_table_npc;

    // ///////////////////Collisions///////////////////////////
public:
    QList<QGraphicsItem *> collisionCheckBuffer;
    bool m_emptyCollisionCheck;
    void prepareCollisionBuffer();

    QGraphicsItem *itemCollidesWith(const QGraphicsItem *item, PGE_ItemList *itemgrp = nullptr, PGE_ItemList *allCollisions = nullptr) override;
    QGraphicsItem *itemCollidesCursor(const QGraphicsItem *item) override;

    // //////////////////////////////////

    // Items list, key is array_id!
    QMap<unsigned int, ItemPlayerPoint*> m_itemsPlayers;

    QMap<unsigned int, ItemBlock*> m_itemsBlocks;
    QMap<unsigned int, ItemBGO*> m_itemsBGO;
    QMap<unsigned int, ItemNPC*> m_itemsNPC;
    QMap<unsigned int, ItemPhysEnv*> m_itemsPhysEnv;

    QMap<unsigned int, ItemDoor*> m_itemsDoorEnters;
    QMap<unsigned int, ItemDoor*> m_itemsDoorExits;

    // Variouis background related objects
    QSet<QGraphicsItem*> m_itemsBG;

    /**
     * @brief Collects all items on the scene and puts them to the *m_data store
     */
    void sceneItemsToData(LevelData &data);

    // ////////////////////////////////////////////////////////////////////////////////
    // /////////////////////////////////EDITING////////////////////////////////////////
    // ////////////////////////////////////////////////////////////////////////////////


    // ///////////////////Placing Mode settings///////////////////////////
public:
    enum placingItemType
    {
        PLC_Block = 0,
        PLC_BGO,
        PLC_NPC,
        PLC_Water,
        PLC_Door,
        PLC_PlayerPoint,
        PLC_Section
    };
    int m_placingItemType;

    LevelData m_placingItems;
    LevelData m_overwritedItems;

    ///
    /// \brief cursor
    /// Abstact item which using to check collision before place item. Using in the placing and erasing modes
    ///
    QGraphicsItem *m_cursorItemImg;
    void resetCursor();

    void setItemPlacer(int itemType, unsigned long itemID = 1, int dType = 0);
    void updateCursoredNpcDirection();

    void setRectDrawer();
    void setCircleDrawer();
    void setLineDrawer();
    void setFloodFiller();

    QGraphicsSimpleTextItem *m_labelBox;
    void setLabelBoxItem(bool show = false, QPointF pos = QPointF(), QString text = "");

    // ///////////////////Mouse Events///////////////////////////
public:
    bool m_mouseIsMovedAfterKey;

    bool m_contextMenuIsOpened;

    bool m_mouseLeftPressed; //Left mouse key is pressed
    bool m_mouseMidPressed;  //Middle mouse key is pressed
    bool m_mouseRightPressed;//Right mouse key is pressed

    bool m_mouseIsMoved; //Mouse was moved with right mouseKey

    bool m_keyCtrlPressed = false;

    //void contextMenuEvent(QGraphicsSceneContextMenuEvent *event);
    void mousePressEvent(QGraphicsSceneMouseEvent *mouseEvent) override;
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *mouseEvent) override;
    bool m_skipChildMousePressEvent;
    void mouseMoveEvent(QGraphicsSceneMouseEvent *mouseEvent) override;
    bool m_skipChildMouseMoveEvent;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *mouseEvent) override;
    bool m_skipChildMousReleaseEvent;
    void keyPressEvent(QKeyEvent *keyEvent) override;
    void keyReleaseEvent(QKeyEvent *keyEvent) override;

    void focusInEvent(QFocusEvent* event) override;
    void focusOutEvent(QFocusEvent* event) override;


    // ////////////////////////////////////////////////////////////////////////////////
    // ////////////////////////////LEVEL SETTINGS//////////////////////////////////////
    // ////////////////////////////////////////////////////////////////////////////////

    // ///////////////////Sections///////////////////////////

public:
    void InitSection(int sect);

    bool isInSection(long x, long y, int sectionIndex, int padding = 0);
    bool isInSection(long x, long y, int width, int height, int sectionIndex, int padding = 0);

    void ChangeSectionBG(int BG_Id, int SectionID = -1, bool forceTiled = false);

    void drawSpace();
    void DrawBG(int x, int y, int w, int h, int sctID, QPixmap &srcimg, QPixmap &srcimg2, obj_BG &bgsetup, bool forceTiled = false);

private:
    void setSectionBG(LevelSection section, bool forceTiled = false);


    // ///////////////////Layers///////////////////////////
public:
    void applyLayersVisible();
    void setLayerToSelected();
    void setLayerToSelected(const QString &lName, bool isNew = false);


    // ///////////////////Resizers///////////////////////////
public:
    ItemResizer *m_resizeBox;  //reisizer pointer

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
    void setSectionResizer(bool enabled, bool accept = false);                           //!< Resizing of section
    void setEventSctSizeResizer(long event, bool enabled, bool accept = false);          //!< Resizing of target sectopn size by event
    void setBlockResizer(QGraphicsItem *targetBlock, bool enabled, bool accept = false); //!< Resizing of block
    void setPhysEnvResizer(QGraphicsItem *targetRect, bool enabled, bool accept = false); //!< Resizing of physical environment zone

    void applyResizers();   //!< Apply current state of resizer
    void resetResizers();   //!< Discard resizing action

    // ////////////////////////////////////////////////////////////////////////////////
    // //////////////////////////////HISTORY///////////////////////////////////////////
    // ////////////////////////////////////////////////////////////////////////////////

    // //////////////////History Manager/////////////////////////
public:
    //history modifiers
    /**
     * @brief Undo recent action
     */
    void historyBack();
    /**
     * @brief Redo recently undone action
     */
    void historyForward();
    //history information
    /**
     * @brief Current history index
     * @return how many history entries stored or which state on history is declared
     */
    int  getHistroyIndex();
    /**
     * @brief Is possible to undo?
     * @return true if undo is possible
     */
    bool canUndo();
    /**
     * @brief Is possible to redo?
     * @return true if redo is possible
     */
    bool canRedo();
    //! History manager
    LvlHistoryManager *m_history = nullptr;
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
