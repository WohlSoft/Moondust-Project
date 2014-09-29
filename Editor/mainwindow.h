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

#include "file_formats/lvl_filedata.h"
#include "file_formats/wld_filedata.h"
#include "file_formats/npc_filedata.h"

#include "edit_level/level_edit.h"
#include "edit_npc/npcedit.h"
#include "edit_world/world_edit.h"

#include "about_dialog/aboutdialog.h"
#include "edit_level/levelprops.h"

#include "data_configs/data_configs.h"

#include "common_features/musicfilelist.h"
#include "common_features/logger.h"

#include "tilesets/tileset.h"
#include "tilesets/tilesetgroupeditor.h"


QT_BEGIN_NAMESPACE
    class QMimeData;
QT_END_NAMESPACE

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
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
 * - Search Boxes common
 * - Music Player
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
 *
 * World Editing
 * - World Settings toolbox
 * - World Item toolbox
 * - World Item Properties box
 * - World Search box
 * - Locks
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

        void applyTheme(QString themeDir="");

        ///
        /// \brief loadSettings load settings from configuration file
        ///
        void loadSettings();
        ///
        /// \brief saveSettings save settings into configuration file
        ///
        void saveSettings();

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
        leveledit   *activeLvlEditWin();    //!< Active Window type 1
        npcedit     *activeNpcEditWin();    //!< Active Window type 2
        WorldEdit   *activeWldEditWin();    //!< Active Window type 3
        int subWins();              //!< Returns number of opened subwindows

    public slots:
        leveledit   *createLvlChild();  //!< Create empty Level Editing subWindow
        npcedit     *createNPCChild();  //!< Create empty NPC config Editing subWindow
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
// ///////////////////////////////////////////////////////////


// ////////////////////Editing features////////////////////
    public slots:
        void applyTextZoom();         //!< Set zoom which defined in the "zoom" field in percents

        void refreshHistoryButtons(); //!< Refreshing state of history undo/redo buttons

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
    public slots:
        void setTileSetBox(); //!< Refresh tileset box's data

        void prepareTilesetGroup(const SimpleTilesetGroup &tilesetGroups);
        QWidget *findTabWidget(const QString &categoryItem);
        QWidget *makeCategory(const QString &categoryItem);
        QScrollArea *getFrameTilesetOfTab(QWidget *catTab);
        QComboBox *getGroupComboboxOfTab(QWidget *catTab);
        void clearTilesetGroups();
        void makeCurrentTileset();
        void makeSelectedTileset(int tabIndex);
        void makeAllTilesets();
        void editSelectedTileset();
        QVector<SimpleTileset> loadCustomTilesets();

    private slots:
        void on_actionConfigure_Tilesets_triggered();
        void on_actionTileset_groups_editor_triggered();

        void on_tilesetGroup_currentIndexChanged(int index);
        void on_newTileset_clicked();
        void on_Tileset_Item_Box_visibilityChanged(bool visible);
        void on_actionTilesetBox_triggered(bool checked);
    private:
        bool lockTilesetBox;

// ////////////////////////////////////////////////////////


// ////////////////////Debugger box////////////////////////
    public slots:
        void Debugger_UpdateMousePosition(QPoint p, bool isOffScreen=false);
        void Debugger_UpdateItemList(QString list);

    private slots:
        void on_actionDebugger_triggered(bool checked);

        void on_debuggerBox_visibilityChanged(bool visible);
        void on_DEBUG_GotoPoint_clicked();
// ////////////////////////////////////////////////////////


// /////////////////////// Help ///////////////////////////
    private slots:
        void on_actionContents_triggered();
        void on_actionAbout_triggered();
// ////////////////////////////////////////////////////////

// //////////////// Configuration manager /////////////////
    private slots:
        void on_actionLoad_configs_triggered(); //!< Reload config
        void on_actionCurConfig_triggered();    //!< Config status
        void on_actionChangeConfig_triggered(); //!< Change configuration
    private:
        QString currentConfigDir;
// ////////////////////////////////////////////////////////

// //////////////////External tools////////////////////////
    private slots:
        void on_actionLazyFixTool_triggered();
        void on_actionGIFs2PNG_triggered();
        void on_actionPNG2GIFs_triggered();
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
        void LvlItemProps(int Type, LevelBlock block, LevelBGO bgo, LevelNPC npc, bool newItem=false);

        long blockPtr;  //!< ArrayID of editing item (-1 - use system)
        long bgoPtr;    //!< ArrayID of editing item
        long npcPtr;    //!< ArrayID of editing item

        bool LvlItemPropsLock; //!< Protector for allow apply changes only if filed was edit by human

    private slots:
        void on_PROPS_BlockResize_clicked();

        void on_PROPS_BlockSquareFill_clicked(bool checked);
        void on_PROPS_BlockInvis_clicked(bool checked);
        void on_PROPS_BlkSlippery_clicked(bool checked);
        void on_PROPS_BlockIncludes_clicked();
        void on_PROPS_BlockLayer_currentIndexChanged(const QString &arg1);
        void on_PROPS_BlkEventDestroy_currentIndexChanged(const QString &arg1);
        void on_PROPS_BlkEventHited_currentIndexChanged(const QString &arg1);
        void on_PROPS_BlkEventLayerEmpty_currentIndexChanged(const QString &arg1);

        void on_PROPS_BGOLayer_currentIndexChanged(const QString &arg1);
        void on_PROPS_BGOSquareFill_clicked(bool checked);
        void on_PROPS_BGO_smbx64_sp_valueChanged(int arg1);

        void on_PROPS_NPCDirLeft_clicked();
        void on_PROPS_NPCDirRand_clicked();
        void on_PROPS_NPCDirRight_clicked();
        void on_PROPS_NpcFri_clicked(bool checked);
        void on_PROPS_NPCNoMove_clicked(bool checked);
        void on_PROPS_NpcBoss_clicked(bool checked);
        void on_PROPS_NpcTMsg_clicked();
        void on_PROPS_NPCSpecialSpin_valueChanged(int arg1);
        void on_PROPS_NPCContaiter_clicked();
        void on_PROPS_NPCSpecialBox_currentIndexChanged(int index);
        void on_PROPS_NPCSpecial2Spin_valueChanged(int arg1);
        void on_PROPS_NPCSpecial2Box_currentIndexChanged(int index);
        void on_PROPS_NpcGenerator_clicked(bool checked);
        void on_PROPS_NPCGenType_currentIndexChanged(int index);
        void on_PROPS_NPCGenTime_valueChanged(double arg1);
        void on_PROPS_NPCGenUp_clicked();
        void on_PROPS_NPCGenLeft_clicked();
        void on_PROPS_NPCGenDown_clicked();
        void on_PROPS_NPCGenRight_clicked();
        void on_PROPS_NpcLayer_currentIndexChanged(const QString &arg1);
        void on_PROPS_NpcAttachLayer_currentIndexChanged(const QString &arg1);
        void on_PROPS_NpcEventActivate_currentIndexChanged(const QString &arg1);
        void on_PROPS_NpcEventDeath_currentIndexChanged(const QString &arg1);
        void on_PROPS_NpcEventTalk_currentIndexChanged(const QString &arg1);
        void on_PROPS_NpcEventEmptyLayer_currentIndexChanged(const QString &arg1);
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

        void ModifyEvent(QString eventName, QString newEventName);

        QListWidget* getEventList();
        void setEventToolsLocked(bool locked);
        long getEventArrayIndex();

    private slots:
        void on_actionLevelEvents_triggered(bool checked);
        void on_LevelEventsToolBox_visibilityChanged(bool visible);

        void refreshSecondSpecialOption(long npcID, long spcOpts, long spcOpts2, bool newItem=false);

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

    private:
        void AddNewEvent(QString eventName, bool setEdited);
        void ModifyEventItem(QListWidgetItem *item, QString oldEventName, QString newEventName);

        void RemoveEvent(QString eventName);
// ////////////////////////////////////////////////////////



// //////////////// Warps toolbox /////////////////////////

    public slots:
        // Warps and doors
        void setDoorsToolbox();
        void setDoorData(long index=-1);
        void SwitchToDoor(long arrayID);
        QComboBox* getWarpList();
        void setWarpRemoveButtonEnabled(bool isEnabled);
        void removeItemFromWarpList(int index);

    private slots:
        void on_actionWarpsAndDoors_triggered(bool checked);
        void on_DoorsToolbox_visibilityChanged(bool visible);

        void on_WarpList_currentIndexChanged(int index); //Door list

        void on_WarpAdd_clicked();
        void on_WarpRemove_clicked();
        void on_WarpSetEntrance_clicked();
        void on_WarpSetExit_clicked();
        void on_WarpNoYoshi_clicked(bool checked);
        void on_WarpAllowNPC_clicked(bool checked);
        void on_WarpLock_clicked(bool checked);
        void on_WarpType_currentIndexChanged(int index);
        void on_WarpNeedAStars_valueChanged(int arg1);
        void on_Entr_Down_clicked();
        void on_Entr_Right_clicked();
        void on_Entr_Up_clicked();
        void on_Entr_Left_clicked();
        void on_Exit_Up_clicked();
        void on_Exit_Left_clicked();
        void on_Exit_Right_clicked();
        void on_Exit_Down_clicked();
        void on_WarpToMapX_editingFinished();
        void on_WarpToMapY_editingFinished();
        void on_WarpGetXYFromWorldMap_clicked();
        void on_WarpLevelExit_clicked(bool checked);
        void on_WarpLevelEntrance_clicked(bool checked);
        void on_WarpLevelFile_editingFinished();
        void on_WarpToExitNu_valueChanged(int arg1);
        void on_WarpBrowseLevels_clicked();
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

        bool doSearchBlock(leveledit* edit);
        bool doSearchBGO(leveledit* edit);
        bool doSearchNPC(leveledit* edit);
// ///////////////////////////////////////////////////////////////


// ///////////////////// Locks ////////////////////////////
    private slots:
        void on_actionLockBlocks_triggered(bool checked);
        void on_actionLockBGO_triggered(bool checked);
        void on_actionLockNPC_triggered(bool checked);
        void on_actionLockWaters_triggered(bool checked);
        void on_actionLockDoors_triggered(bool checked);
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
        long wlvlPtr;   //!< ArrayID of editing item

    public slots:
        void setCurrentWorldSettings();
        void WldLvlExitTypeListReset();

        // accept point from world map into a level properties
        void WLD_returnPointToLevelProperties(QPoint p);

    private slots:
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

        void on_actionExport_to_image_section_triggered();

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
