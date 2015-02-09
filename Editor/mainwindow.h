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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtWidgets>
#include <QMdiArea>
#include <QPixmap>
#include <QAbstractListModel>
#include <QList>
#include <QPoint>
#include <QString>
#include <QStringList>
#include <QFileDialog>
#include <QFile>
#include <QSettings>
#include <QTranslator>
#include <QLocale>
#include <QSplashScreen>
#ifdef Q_OS_WIN
#include <QWinThumbnailToolBar>
#endif

#include <file_formats/lvl_filedata.h>
#include <file_formats/wld_filedata.h>
#include <file_formats/npc_filedata.h>

#include <editing/edit_level/level_edit.h>
#include <editing/edit_npc/npcedit.h>
#include <editing/edit_world/world_edit.h>

#include <data_configs/data_configs.h>

#include <common_features/logger.h>

#include <tools/tilesets/tileset.h>
#include <tools/tilesets/tilesetgroupeditor.h>


QT_BEGIN_NAMESPACE
    class QMimeData;
QT_END_NAMESPACE


/*************************Dock widgets***************************/
#include <main_window/dock/toolboxes_protos.h>
/*************************Dock widgets*end***********************/


namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
    /************************Friend classes***************************/
    friend class TilesetItemBox;
    friend class LvlItemProperties;
    friend class LvlWarpBox;
    /************************Friend classes***************************/

public:
    explicit MainWindow(QMdiArea *parent = 0);
    ~MainWindow();

    dataconfigs configs;        // Global game configucrations

/* //////////////////////Contents/////////////////////////////
 * COMMON
 * - Misc
 * - Miltilanguage
 * - Recent Files
 * - Sub Windows
 * -- Latest Active Window (See sub_windows.cpp for comments)
 * - Dock widgwets
 * - Editing features
 * - Clipboard
 * - EditMode switch
 * - Placing tools
 * - Resize ask
 * - Tileset box
 * - Debugger box
 * - Help
 * - Configuration manager
 * - External tools
 * - Other Tools
 * - Search Boxes common
 * - Music Player
 * - Bookmarks
 * - Windows Extras
 *
 * Level Editing
 * - Level Properties
 * - Section toobar
 * - Level Item toolbox
 * - Level Item Properties box
 * - Section Settings box
 * - Layers toolbox
 * - Level Events toolbox
 * - Warps toolbox
 * - Level Search box
 * - Locks
 * - Script
 *
 * World Editing
 * - World Settings toolbox
 * - World Item toolbox
 * - World Item Properties box
 * - World Search box
 * - Locks
 *
 * Testing
 *
 */




// ////////////////////////////////////////////////////////////////////////////////
// /////////////////////////////////COMMON/////////////////////////////////////////
// ////////////////////////////////////////////////////////////////////////////////

// //////////////////////// Misc /////////////////////////////
    public:
        ///
        /// \brief openFilesByArgs Open files which gived to editor from command line
        /// \param args - list of file paths
        ///
        void openFilesByArgs(QStringList args);

        void showStatusMsg(QString msg, int time=2000); //Send status message
        void showToolTipMsg(QString msg, QPoint pos, int time); //Show tooltip msg

        void applyTheme(QString themeDir="");

        bool continueLoad; //!< Is need to continue or abort loading operation and close editor

        ///
        /// \brief loadSettings load settings from configuration file
        ///
        void loadSettings();
        ///
        /// \brief saveSettings save settings into configuration file
        ///
        void saveSettings();

        ///
        /// \brief getCurrentSceneCoordinates Returns the scene coordinates either from level window or world window
        /// \param x The current x-coordinate.
        /// \param y The current y-coordinate.
        /// \return True, if the current window is either a level window or a world window.
        ///
        bool getCurrentSceneCoordinates(qreal &x, qreal &y);
    private:
        ///
        /// \brief setDefaults Init settings on start application
        ///
        void setDefaults();
        ///
        /// \brief setUiDefults Init UI settings of application on start
        ///
        void setUiDefults();

    public slots:
        void save();         //!< Save current file
        void save_as();      //!< Save current file with asking for save path
        void close_sw();     //!< Close current sub window
        void save_all();     //!< Save all opened files

        ///
        /// \brief OpenFile - Open file in the editor
        /// \param FilePath - path to file
        ///
        void OpenFile(QString FilePath);

        ///
        /// \brief on_actionReload_triggered - Reload/Reopen current file
        ///
        void on_actionReload_triggered();

        ///
        /// \brief on_actionRefresh_menu_and_toolboxes_triggered - Refresh item boxes
        ///
        void on_actionRefresh_menu_and_toolboxes_triggered();

        ///
        /// \brief updateMenus Refresh the menubars
        /// \param force Refresh menubar even if window is not active
        ///
        void updateMenus(bool force=false);
        void updateWindowMenu();   //!< Update "Window" Menu

    private slots:
        void on_actionShow_Development_Console_triggered();     //!< Open Developement Console
        void on_actionSwitch_to_Fullscreen_triggered(bool checked);

        //New file
        void on_actionNewLevel_triggered();
        void on_actionNewNPC_config_triggered();
        void on_actionNewWorld_map_triggered();

        //File menu
        void on_actionNew_triggered();
        void on_OpenFile_triggered();
        void on_actionSave_triggered();
        void on_actionSave_as_triggered();
        void on_actionClose_triggered();
        void on_actionSave_all_triggered();

        void on_actionExport_to_image_triggered();      //!< Export current workspace into image
        void on_actionExport_to_image_section_triggered();

        void on_actionApplication_settings_triggered(); //!< Open application settings

        void on_Exit_triggered();                       //!< Exit from application

        void on_MainWindow_customContextMenuRequested(const QPoint &pos);
// ///////////////////////////////////////////////////////////


// //////////////////Miltilanguage///////////////////////////
    private slots:
        void slotLanguageChanged(QAction *action);  //!< Change language to selected in the language menu
    private:
        void loadLanguage(const QString& rLanguage);
        void setDefLang();
        bool switchTranslator(QTranslator &translator, const QString &filename);
        void langListSync();
        QTranslator     m_translator;   /**< contains the translations for this application */
        QTranslator     m_translatorQt; /**< contains the translations for qt */
        QString         m_currLang;     /**< contains the currently loaded language */
        QString         m_langPath;     /**< Path of language files. This is always fixed to /languages. */
// ///////////////////////////////////////////////////////////


// /////////////// Recent Files ///////////////////////////
    public slots:
        void SyncRecentFiles();     //!< Refresh recent file list from list

        ///
        /// \brief AddToRecentFiles Add item into recent file list
        /// \param FilePath Absolute path to the file
        ///
        void AddToRecentFiles(QString FilePath);

        //RecentFiles Menu Items
        void on_action_recent1_triggered();
        void on_action_recent2_triggered();
        void on_action_recent3_triggered();
        void on_action_recent4_triggered();
        void on_action_recent5_triggered();
        void on_action_recent6_triggered();
        void on_action_recent7_triggered();
        void on_action_recent8_triggered();
        void on_action_recent9_triggered();
        void on_action_recent10_triggered();
    private:
        QList<QString> recentOpen;  //!< Recent files list
// ////////////////////////////////////////////////////////


// ///////////////////// Sub Windows /////////////////////////
    public:
        ///
        /// \brief activeChildWindow returns type of active sub window
        /// \return Active Window type (0 - nothing, 1 - level, 2 - NPC, 3 - World)
        ///
        int activeChildWindow();
        int activeChildWindow(QMdiSubWindow* wnd);
        LevelEdit   *activeLvlEditWin();    //!< Active Window type 1
        LevelEdit   *activeLvlEditWin(QMdiSubWindow *wnd);
        NpcEdit     *activeNpcEditWin();    //!< Active Window type 2
        NpcEdit     *activeNpcEditWin(QMdiSubWindow *wnd);
        WorldEdit   *activeWldEditWin();    //!< Active Window type 3
        WorldEdit   *activeWldEditWin(QMdiSubWindow *wnd);
        int subWins();              //!< Returns number of opened subwindows

    public slots:
        LevelEdit   *createLvlChild();  //!< Create empty Level Editing subWindow
        NpcEdit     *createNPCChild();  //!< Create empty NPC config Editing subWindow
        WorldEdit   *createWldChild();  //!< Create empty World map Editing subWindow

        void setActiveSubWindow(QWidget *window);  //!< Switch to target subWindow

        void SWCascade();   //!< Cascade subWindows
        void SWTile();      //!< Tile subWindows
        void setSubView();  //!< Switch Sub Windows view mode
        void setTabView();  //!< Cascade Tabs view mode


        // //////////////////SubWindows mapper///////////////////
    private:
        ///
        /// \brief findOpenedFileWin Finding the subWindow with already opened file
        /// \param fileName target file
        /// \return pointer to subWindow which case with target file
        ///
        QMdiSubWindow *findOpenedFileWin(const QString &fileName);
        QSignalMapper *windowMapper;
        // //////////////////////////////////////////////////////

        // /////////////// Latest Active Window ///////////////////
    public slots:
        void recordSwitchedWindow(QMdiSubWindow * window);
        void recordRemovedWindow(QObject* possibleDeletedWindow);
    private:
        QMdiSubWindow* LastActiveSubWindow;
        // ////////////////////////////////////////////////////////
// ///////////////////////////////////////////////////////////




// ////////////////////Editing features////////////////////
    public slots:
        void applyTextZoom();         //!< Set zoom which defined in the "zoom" field in percents

        void refreshHistoryButtons(); //!< Refreshing state of history undo/redo buttons

        void on_actionAlign_selected_triggered();
        void on_actionRotateLeft_triggered();
        void on_actionRotateRight_triggered();
        void on_actionFlipHorizontal_triggered();
        void on_actionFlipVertical_triggered();

    private slots:
        void on_actionZoomIn_triggered();    //!< Zoom in
        void on_actionZoomOut_triggered();   //!< Zoom Out
        void on_actionZoomReset_triggered(); //!< Reset Zoom to default (1:1)

        //History
        void on_actionUndo_triggered(); //!< Undo last changes
        void on_actionRedo_triggered(); //!< Restore undone changes
    private:
        QLineEdit* zoom; //!< Zoom value field
// ////////////////////////////////////////////////////////


// /////////////////// Clipboard //////////////////////////
    public slots:
        void on_actionCopy_triggered();  //!< Copy selected items into clipboard
        void on_actionCut_triggered();   //!< Move selected items into clipboard
        void on_actionPaste_triggered(); //!< Switch paste mode of clipboard items

    private:
        LevelData LvlBuffer; //!< The Clipboarc for Level objects
        WorldData WldBuffer; //!< The Clipboarc for World map objects

// ////////////////////////////////////////////////////////


// ////////////////// EditMode switch /////////////////////
    public slots:
        void on_actionSelect_triggered();
        void on_actionSelectOnly_triggered();
        void on_actionEriser_triggered();
        void on_actionHandScroll_triggered();

        void on_actionSetFirstPlayer_triggered();
        void on_actionSetSecondPlayer_triggered();

        void on_actionDrawWater_triggered();
        void on_actionDrawSand_triggered();
    private:
        void resetEditmodeButtons();
// ////////////////////////////////////////////////////////

// //////////////// Placing tools /////////////////////////
    public slots:
        void SwitchPlacingItem(int itemType, unsigned long itemID);
        void on_actionSquareFill_triggered(bool checked);
        void on_actionLine_triggered(bool checked);
        void on_actionOverwriteMode_triggered(bool checked);
        void on_actionFill_triggered(bool checked);
        void on_actionFloodSectionOnly_triggered(bool checked);

        void on_action_Placing_ShowProperties_triggered(bool checked);
    private:
        int  Placing_ShowProperties_lastType;

// ////////////////////////////////////////////////////////

// ////////////////// Resize ask //////////////////////////
    public slots:
        void resizeToolbarVisible(bool vis);

        void on_applyResize_clicked();
        void on_cancelResize_clicked();

        void on_actionResizeApply_triggered();
        void on_actionResizeCancel_triggered();
// ////////////////////////////////////////////////////////

// ////////////////// Tileset box /////////////////////////
    public:
        TilesetItemBox *dock_TilesetBox;
    public slots:
        void setTileSetBox(); //!< Refresh tileset box's data

    private slots:
        void on_actionConfigure_Tilesets_triggered();
        void on_actionTileset_groups_editor_triggered();
        void on_actionTilesetBox_triggered(bool checked);

// ////////////////////////////////////////////////////////


// ////////////////////Debugger box////////////////////////
    public slots:
        void Debugger_UpdateMousePosition(QPoint p, bool isOffScreen=false);
        void Debugger_UpdateItemList(QString list);

        void Debugger_loadCustomCounters();
        void Debugger_saveCustomCounters();

    private slots:
        void on_actionDebugger_triggered(bool checked);

        void on_debuggerBox_visibilityChanged(bool visible);
        void on_DEBUG_GotoPoint_clicked();

        void on_DEBUG_AddCustomCounter_clicked();
        void on_DEBUG_RefreshCoutners_clicked();

        void on_DEBUG_CustomCountersList_itemClicked(QListWidgetItem *item);
        void on_DEBUG_CustomCountersList_customContextMenuRequested(const QPoint &pos);
// ////////////////////////////////////////////////////////


// /////////////////////// Help ///////////////////////////
    private slots:
        void on_actionContents_triggered();
        void on_actionAbout_triggered();
        void on_actionSMBX_like_GUI_triggered();
        void on_actionChange_log_triggered();
        void on_actionCheckUpdates_triggered();
// ////////////////////////////////////////////////////////

// //////////////// Configuration manager /////////////////
    private slots:
        void on_actionLoad_configs_triggered(); //!< Reload config
        void on_actionCurConfig_triggered();    //!< Config status
        void on_actionChangeConfig_triggered(); //!< Change configuration
    private:
        QString currentConfigDir;
        bool askConfigAgain;
// ////////////////////////////////////////////////////////

// //////////////////External tools////////////////////////
    private slots:
        void on_actionLazyFixTool_triggered();
        void on_actionGIFs2PNG_triggered();
        void on_actionPNG2GIFs_triggered();
// ////////////////////////////////////////////////////////



// //////////////////Other Tools////////////////////////
    private slots:
        void on_actionCDATA_clear_unused_triggered();
        void on_actionCDATA_Import_triggered();
        void on_actionSprite_editor_triggered();

        void on_actionFixWrongMasks_triggered();
// ////////////////////////////////////////////////////////




// /////////////Search Boxes common ///////////////////////
    private:
        enum currentSearch{
            SEARCH_BLOCK = 1 << 0,
            SEARCH_BGO = 1 << 1,
            SEARCH_NPC = 1 << 2,
            SEARCH_TILE = 1 << 3,
            SEARCH_SCENERY = 1 << 4,
            SEARCH_PATH = 1 << 5,
            SEARCH_LEVEL = 1 << 6,
            SEARCH_MUSICBOX = 1 << 7
        };
        int currentSearches;
        void resetAllSearchFields();
// ///////////////////////////////////////////////////////


// ///////////////// Music Player ////////////////////////
    public slots:
        void setMusic(bool checked);
        void setMusicButton(bool checked);
        void on_actionPlayMusic_triggered(bool checked);

    private:
        QSlider* muVol;
// ///////////////////////////////////////////////////////


// ///////////////////// Bookmarks ////////////////////////        
    private slots:
        void on_actionBookmarkBox_triggered(bool checked);
        void on_bookmarkBox_visibilityChanged(bool visible);
        void on_bookmarkList_customContextMenuRequested(const QPoint &pos);
        void DragAndDroppedBookmark(QModelIndex /*sourceParent*/,int sourceStart,int sourceEnd,QModelIndex /*destinationParent*/,int destinationRow);
        void on_bookmarkList_doubleClicked(const QModelIndex &index);
    //Modificators:
        void on_bookmarkAdd_clicked();
        void on_bookmarkRemove_clicked();
        void on_bookmarkList_itemChanged(QListWidgetItem *item);
    //Go To...
        void on_bookmarkGoto_clicked();
    public:
        void updateBookmarkBoxByList();
        void updateBookmarkBoxByData();
// ////////////////////////////////////////////////////////


#ifdef Q_OS_WIN
// ////////////////// Windows Extras //////////////////////
    public:
        void initWindowsThumbnail();
    public slots:
        void updateWindowsExtrasPixmap();
        void drawWindowsDefaults();
    private:
        QWinThumbnailToolBar* pge_thumbbar;
// ////////////////////////////////////////////////////////
#endif


// ////////////////////////////////////////////////////////////////////////////////
// ////////////////////////////Level Editing///////////////////////////////////////
// ////////////////////////////////////////////////////////////////////////////////


// ////////////////////Level Properties///////////////////////
    private slots:
        void on_actionLevelProp_triggered();
// ////////////////////////////////////////////////////////


// ///////////////////Section toobar///////////////////////
    public slots:
        //Switch section
        void SetCurrentLevelSection(int SctId, int open=0);
        void on_actionReset_position_triggered();
        void on_ResizeSection_clicked();

    private slots:
        void on_actionGo_to_Section_triggered();

        void on_actionSection_1_triggered();
        void on_actionSection_2_triggered();
        void on_actionSection_3_triggered();
        void on_actionSection_4_triggered();
        void on_actionSection_5_triggered();
        void on_actionSection_6_triggered();
        void on_actionSection_7_triggered();
        void on_actionSection_8_triggered();
        void on_actionSection_9_triggered();
        void on_actionSection_10_triggered();
        void on_actionSection_11_triggered();
        void on_actionSection_12_triggered();
        void on_actionSection_13_triggered();
        void on_actionSection_14_triggered();
        void on_actionSection_15_triggered();
        void on_actionSection_16_triggered();
        void on_actionSection_17_triggered();
        void on_actionSection_18_triggered();
        void on_actionSection_19_triggered();
        void on_actionSection_20_triggered();
        void on_actionSection_21_triggered();

        //Modify actions
        void on_actionCloneSectionTo_triggered();
        void on_actionSCT_Delete_triggered();
        void on_actionSCT_RotateLeft_triggered();
        void on_actionSCT_RotateRight_triggered();
        void on_actionSCT_FlipHorizontal_triggered();
        void on_actionSCT_FlipVertical_triggered();

// ////////////////////////////////////////////////////////

// ////////////////////Level Item toolbox /////////////////
    // update data of the toolboxes
    public slots:
        void setLvlItemBoxes(bool setGrp=false, bool setCat=false);
        void UpdateLvlCustomItems();

        void updateFilters();
        void clearFilter();

    private slots:
        void on_LevelToolBox_visibilityChanged(bool visible);
        void on_actionLVLToolBox_triggered(bool checked);

        void on_BlockUniform_clicked(bool checked);
        void on_BGOUniform_clicked(bool checked);
        void on_NPCUniform_clicked(bool checked);

        void on_BlockGroupList_currentIndexChanged(const QString &arg1);
        void on_BGOGroupList_currentIndexChanged(const QString &arg1);
        void on_NPCGroupList_currentIndexChanged(const QString &arg1);

        void on_BlockCatList_currentIndexChanged(const QString &arg1);
        void on_BGOCatList_currentIndexChanged(const QString &arg1);
        void on_NPCCatList_currentIndexChanged(const QString &arg1);

        void on_BlockFilterField_textChanged(const QString &arg1);
        void on_BGOFilterField_textChanged(const QString &arg1);
        void on_NPCFilterField_textChanged(const QString &arg1);

        void on_BlockFilterType_currentIndexChanged(int index);
        void on_BGOFilterType_currentIndexChanged(int index);
        void on_NPCFilterType_currentIndexChanged(int index);

        //Item was clicked
        void on_BlockItemsList_itemClicked(QListWidgetItem *item);
        void on_BGOItemsList_itemClicked(QListWidgetItem *item);
        void on_NPCItemsList_itemClicked(QListWidgetItem *item);

    private:
        QString cat_blocks; //!< Category
        QString cat_bgos;
        QString cat_npcs;
// ////////////////////////////////////////////////////////

// ///////////////Level Item Properties box //////////////////
    public:
        LvlItemProperties *dock_LvlItemProps;
// ///////////////////////////////////////////////////////////


// ///////////////// Section Settings box /////////////////
    public slots:
        void setLevelSectionData();

        void on_LVLPropsMusicCustom_editingFinished();
        void on_actionGridEn_triggered(bool checked);
        void on_LVLPropsBackImage_currentIndexChanged(int index);



    private slots:
        void on_LevelSectionSettings_visibilityChanged(bool visible);
        void on_actionSection_Settings_triggered(bool checked);

        void on_LVLPropsLevelWarp_clicked(bool checked);
        void on_actionLevWarp_triggered(bool checked);

        void on_LVLPropsOffScr_clicked(bool checked);
        void on_actionLevOffScr_triggered(bool checked);

        void on_LVLPropsNoTBack_clicked(bool checked);
        void on_actionLevNoBack_triggered(bool checked);

        void on_LVLPropsUnderWater_clicked(bool checked);
        void on_actionLevUnderW_triggered(bool checked);

        void on_actionAnimation_triggered(bool checked);
        void on_actionCollisions_triggered(bool checked);

        void on_actionVBAlphaEmulate_toggled(bool arg1);

        void on_LVLPropsMusicNumber_currentIndexChanged(int index);
        void on_LVLPropsMusicCustomEn_toggled(bool checked);
        void on_LVLPropsMusicCustomBrowse_clicked();
// ////////////////////////////////////////////////////////



// //////////////// Layers toolbox /////////////////////////
    public slots:
        void setLayersBox();
        void setLayerLists();
        void ModifyLayer(QString layerName, QString newLayerName);
        void setLayerToolsLocked(bool locked);

        void DragAndDroppedLayer(QModelIndex sourceParent, int sourceStart, int sourceEnd, QModelIndex destinationParent, int destinationRow);

    private slots:
        void on_LevelLayers_visibilityChanged(bool visible);
        void on_actionLayersBox_triggered(bool checked);

        void on_AddLayer_clicked();
        void on_LvlLayerList_itemChanged(QListWidgetItem *item);

        void on_RemoveLayer_clicked();
        void on_LvlLayerList_customContextMenuRequested(const QPoint &pos);

        void on_LvlLayerList_itemClicked(QListWidgetItem *item);
        void on_LvlLayerList_itemSelectionChanged();

    private:
        void RemoveCurrentLayer(bool moveToDefault);
        void RemoveLayerItems(QString layerName);
        void RemoveLayerFromListAndData(QListWidgetItem * layerItem);
        void ModifyLayer(QString layerName, bool visible);
        void ModifyLayer(QString layerName, QString newLayerName, bool visible, int historyRecord = -1);

        //Direct List Functions
        void AddNewLayer(QString layerName, bool setEdited);
        void ModifyLayerItem(QListWidgetItem *item, QString oldLayerName, QString newLayerName, bool visible);
// ////////////////////////////////////////////////////////


// //////////////// Level Events toolbox //////////////////
    public slots:
        void eventSectionSettingsSync();
        void setSoundList();

        void DragAndDroppedEvent(QModelIndex sourceParent, int sourceStart, int sourceEnd, QModelIndex destinationParent, int destinationRow);

        void EventListsSync();
        void setEventsBox();
        void setEventData(long index=-1);
    public:
        bool LvlEventBoxLock;
    public slots:
        void ModifyEvent(QString eventName, QString newEventName);

        QListWidget* getEventList();
        void setEventToolsLocked(bool locked);
        long getEventArrayIndex();

    private slots:
        void on_actionLevelEvents_triggered(bool checked);
        void on_LevelEventsToolBox_visibilityChanged(bool visible);

        void on_LVLEvents_List_itemSelectionChanged();
        void on_LVLEvents_List_itemChanged(QListWidgetItem *item);

        void on_LVLEvent_Cmn_Msg_clicked();
        void on_LVLEvent_Cmn_PlaySnd_currentIndexChanged(int index);
        void on_LVLEvent_playSnd_clicked();
        void on_LVLEvent_Cmn_EndGame_currentIndexChanged(int index);

        void on_LVLEvents_add_clicked();
        void on_LVLEvents_del_clicked();
        void on_LVLEvents_duplicate_clicked();
        void on_LVLEvent_AutoStart_clicked(bool checked);

        void on_LVLEvent_disableSmokeEffect_clicked(bool checked);

        void eventLayerVisiblySyncList();

        void on_LVLEvent_Layer_HideAdd_clicked();
        void on_LVLEvent_Layer_HideDel_clicked();

        void on_LVLEvent_Layer_ShowAdd_clicked();
        void on_LVLEvent_Layer_ShowDel_clicked();

        void on_LVLEvent_Layer_TogAdd_clicked();
        void on_LVLEvent_Layer_TogDel_clicked();

        void on_LVLEvent_LayerMov_List_currentIndexChanged(int index);
        void on_LVLEvent_LayerMov_spX_valueChanged(double arg1);
        void on_LVLEvent_LayerMov_spY_valueChanged(double arg1);

        void on_LVLEvent_Scroll_Sct_valueChanged(int arg1);
        void on_LVLEvent_Scroll_spX_valueChanged(double arg1);
        void on_LVLEvent_Scroll_spY_valueChanged(double arg1);

        void on_LVLEvent_Sct_list_currentIndexChanged(int index);

        void on_LVLEvent_SctSize_none_clicked();
        void on_LVLEvent_SctSize_reset_clicked();
        void on_LVLEvent_SctSize_define_clicked();
        void on_LVLEvent_SctSize_left_textEdited(const QString &arg1);
        void on_LVLEvent_SctSize_top_textEdited(const QString &arg1);
        void on_LVLEvent_SctSize_bottom_textEdited(const QString &arg1);
        void on_LVLEvent_SctSize_right_textEdited(const QString &arg1);
        void on_LVLEvent_SctSize_Set_clicked();

        void on_LVLEvent_SctMus_none_clicked();
        void on_LVLEvent_SctMus_reset_clicked();
        void on_LVLEvent_SctMus_define_clicked();
        void on_LVLEvent_SctMus_List_currentIndexChanged(int index);

        void on_LVLEvent_SctBg_none_clicked();
        void on_LVLEvent_SctBg_reset_clicked();
        void on_LVLEvent_SctBg_define_clicked();
        void on_LVLEvent_SctBg_List_currentIndexChanged(int index);

        void on_LVLEvent_Key_Up_clicked(bool checked);
        void on_LVLEvent_Key_Down_clicked(bool checked);
        void on_LVLEvent_Key_Left_clicked(bool checked);
        void on_LVLEvent_Key_Right_clicked(bool checked);
        void on_LVLEvent_Key_Run_clicked(bool checked);
        void on_LVLEvent_Key_AltRun_clicked(bool checked);
        void on_LVLEvent_Key_Jump_clicked(bool checked);
        void on_LVLEvent_Key_AltJump_clicked(bool checked);
        void on_LVLEvent_Key_Drop_clicked(bool checked);
        void on_LVLEvent_Key_Start_clicked(bool checked);

        void on_LVLEvent_TriggerEvent_currentIndexChanged(int index);
        void on_LVLEvent_TriggerDelay_valueChanged(double arg1);

        void on_bps_LayerMov_horSpeed_clicked();
        void on_bps_LayerMov_vertSpeed_clicked();
        void on_bps_Scroll_horSpeed_clicked();
        void on_bps_Scroll_vertSpeed_clicked();

    private:
        void AddNewEvent(QString eventName, bool setEdited);
        void ModifyEventItem(QListWidgetItem *item, QString oldEventName, QString newEventName);

        void RemoveEvent(QString eventName);
// ////////////////////////////////////////////////////////



// //////////////// Warps toolbox /////////////////////////

    public:
        LvlWarpBox * dock_LvlWarpProps;

    private slots:
        void on_actionWarpsAndDoors_triggered(bool checked);

// ////////////////////////////////////////////////////////


// //////////////////// Level Search box /////////////////////////
    public slots:
        void toggleNewWindowLVL(QMdiSubWindow *window);
        void resetBlockSearch();
        void resetBGOSearch();
        void resetNPCSearch();

    private slots:
        void on_actionLVLSearchBox_triggered(bool checked);
        void on_FindDock_visibilityChanged(bool visible);

        void on_FindStartNPC_clicked();
        void on_Find_Button_TypeBlock_clicked();
        void on_Find_Button_TypeBGO_clicked();
        void on_Find_Button_TypeNPC_clicked();
        void on_Find_Button_ResetBlock_clicked();
        void on_Find_Button_ResetBGO_clicked();
        void on_Find_Button_ResetNPC_clicked();
        void on_FindStartBlock_clicked();
        void on_FindStartBGO_clicked();
        void on_Find_Button_ContainsNPCBlock_clicked();
    private:
        LevelBlock curSearchBlock;
        LevelBGO curSearchBGO;
        LevelNPC curSearchNPC;

        bool doSearchBlock(LevelEdit* edit);
        bool doSearchBGO(LevelEdit* edit);
        bool doSearchNPC(LevelEdit* edit);
// ///////////////////////////////////////////////////////////////


// ///////////////////// Locks ////////////////////////////
    private slots:
        void on_actionLockBlocks_triggered(bool checked);
        void on_actionLockBGO_triggered(bool checked);
        void on_actionLockNPC_triggered(bool checked);
        void on_actionLockWaters_triggered(bool checked);
        void on_actionLockDoors_triggered(bool checked);
// ////////////////////////////////////////////////////////

// ///////////////////// Script ///////////////////////////
    private slots:
        void on_actionAdditional_Settings_triggered();
        void on_actionCompile_To_triggered();
        void on_actionAutocode_Lunadll_Original_Language_triggered();
        void on_actionLunaLua_triggered();
// ////////////////////////////////////////////////////////


// ////////////////////////////////////////////////////////////////////////////////
// ////////////////////////////World Editing///////////////////////////////////////
// ////////////////////////////////////////////////////////////////////////////////

// ////////////////////World Settings toolbox /////////////////
    private slots:
        void on_actionWorld_settings_triggered(bool checked);
        void on_WorldSettings_visibilityChanged(bool visible);

        void on_WLD_Title_editingFinished();
        void on_WLD_NoWorldMap_clicked(bool checked);
        void on_WLD_RestartLevel_clicked(bool checked);
        void on_WLD_AutostartLvl_editingFinished();
        void on_WLD_AutostartLvlBrowse_clicked();
        void on_WLD_Stars_valueChanged(int arg1);
        void on_WLD_DoCountStars_clicked();
        void on_WLD_Credirs_textChanged();
        void characterActivated(bool checked);

        void on_actionSemi_transparent_paths_triggered(bool checked);
// ////////////////////////////////////////////////////////

// ////////////////////World Item toolbox /////////////////
    public slots:
        void setWldItemBoxes(bool setGrp=false, bool setCat=false);

    private slots:
        void on_WorldToolBox_visibilityChanged(bool visible);
        void on_actionWLDToolBox_triggered(bool checked);

        void on_WLD_TilesList_itemClicked(QTableWidgetItem *item);
        void on_WLD_SceneList_itemClicked(QListWidgetItem *item);
        void on_WLD_PathsList_itemClicked(QTableWidgetItem *item);
        void on_WLD_LevelList_itemClicked(QListWidgetItem *item);
        void on_WLD_MusicList_itemClicked(QListWidgetItem *item);
// ////////////////////////////////////////////////////////


// ///////////////World Item Properties box //////////////////
    public:
        void WldItemProps(int Type, WorldLevels level, bool newItem=false);
        void WldItemProps_hide();
        long wlvlPtr;   //!< ArrayID of editing item

    public slots:
        void setCurrentWorldSettings();
        void WldLvlExitTypeListReset();

        // accept point from world map into a level properties
        void WLD_returnPointToLevelProperties(QPoint p);

    private slots:
        void on_WLD_ItemProps_visibilityChanged(bool visible);

        void on_WLD_PROPS_PathBG_clicked(bool checked);
        void on_WLD_PROPS_BigPathBG_clicked(bool checked);
        void on_WLD_PROPS_AlwaysVis_clicked(bool checked);
        void on_WLD_PROPS_GameStart_clicked(bool checked);
        void on_WLD_PROPS_LVLFile_editingFinished();
        void on_WLD_PROPS_LVLTitle_editingFinished();
        void on_WLD_PROPS_EnterTo_valueChanged(int arg1);
        void on_WLD_PROPS_LVLBrowse_clicked();
        void on_WLD_PROPS_ExitTop_currentIndexChanged(int index);
        void on_WLD_PROPS_ExitLeft_currentIndexChanged(int index);
        void on_WLD_PROPS_ExitRight_currentIndexChanged(int index);
        void on_WLD_PROPS_ExitBottom_currentIndexChanged(int index);
        void on_WLD_PROPS_GotoX_editingFinished();
        void on_WLD_PROPS_GotoY_editingFinished();
        void on_WLD_PROPS_GetPoint_clicked();
// ///////////////////////////////////////////////////////////


// //////////////////// World Search box /////////////////////////
    public slots:
        void toggleNewWindowWLD(QMdiSubWindow *window);
        void resetAllSearches();
        void resetAllSearchFieldsWLD();
        void resetAllSearchesWLD();
        void selectLevelForSearch();

        void resetTileSearch();
        void resetScenerySearch();
        void resetPathSearch();
        void resetLevelSearch();
        void resetMusicSearch();

private slots:
        void on_actionWLDDisableMap_triggered(bool checked);
        void on_actionWLDFailRestart_triggered(bool checked);
        void on_actionWLDProperties_triggered();

        void on_actionWLD_SearchBox_triggered(bool checked);
        void on_WorldFindDock_visibilityChanged(bool visible);
        void on_FindStartLevel_clicked();
        void on_FindStartTile_clicked();
        void on_FindStartScenery_clicked();
        void on_FindStartPath_clicked();
        void on_FindStartMusic_clicked();

        void on_Find_Button_TypeLevel_clicked();
        void on_Find_Button_TypeTile_clicked();
        void on_Find_Button_TypeScenery_clicked();
        void on_Find_Button_TypePath_clicked();
        void on_Find_Button_TypeMusic_clicked();
        void on_Find_Button_ResetLevel_clicked();
        void on_Find_Button_ResetMusic_clicked();
        void on_Find_Button_ResetPath_clicked();
        void on_Find_Button_ResetScenery_clicked();
        void on_Find_Button_ResetTile_clicked();

    private:
        WorldTiles curSearchTile;
        WorldScenery curSearchScenery;
        WorldPaths curSearchPath;
        WorldLevels curSearchLevel;
        WorldMusic curSearchMusic;

        bool doSearchTile(WorldEdit *edit);
        bool doSearchScenery(WorldEdit *edit);
        bool doSearchPath(WorldEdit *edit);
        bool doSearchLevel(WorldEdit* edit);
        bool doSearchMusic(WorldEdit *edit);
// ///////////////////////////////////////////////////////////////


// ///////////////////// Locks ////////////////////////////
    private slots:
        void on_actionLockTiles_triggered(bool checked);
        void on_actionLockScenes_triggered(bool checked);
        void on_actionLockPaths_triggered(bool checked);
        void on_actionLockLevels_triggered(bool checked);
        void on_actionLockMusicBoxes_triggered(bool checked);
// ////////////////////////////////////////////////////////



// ////////////////////////////////////////////////////////////////////////////////
// /////////////////////////////////Testing////////////////////////////////////////
// ////////////////////////////////////////////////////////////////////////////////
    private slots:
        void on_action_doTest_triggered();
        void on_action_doSafeTest_triggered();

        void on_action_testSettings_triggered();

    public slots:
        void on_actionRunTestSMBX_triggered();

// ////////////////////Unsorted slots/////////////////////////////
// ///////Please move them into it's category/////////////////////
public:
public slots:
    protected:
    private slots:

signals:
    void closeEditor();

protected:
    void closeEvent(QCloseEvent *event);
    void dragEnterEvent(QDragEnterEvent *e);
    void dropEvent(QDropEvent *e);

private:
    void setPointer();  //!< Set the self pointer into the MainWinConnect class

    Ui::MainWindow *ui;
};


#endif // MAINWINDOW_H
