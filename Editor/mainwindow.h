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

#include <PGE_File_Formats/lvl_filedata.h>
#include <PGE_File_Formats/wld_filedata.h>
#include <PGE_File_Formats/npc_filedata.h>

#include <editing/edit_level/level_edit.h>
#include <editing/edit_npc/npcedit.h>
#include <editing/edit_world/world_edit.h>

#include <data_configs/data_configs.h>

#include <common_features/logger.h>

#include <tools/tilesets/tileset.h>
#include <tools/tilesets/tilesetgroupeditor.h>

#include <main_window/dock/toolboxes_protos.h>
#include <main_window/dock/_dock_vizman.h>


QT_BEGIN_NAMESPACE
    class QMimeData;
QT_END_NAMESPACE



namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
    MainWindowFriends
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

        DockVizibilityManager docks_level; //!< Manager of level specific toolboxes
        DockVizibilityManager docks_world; //!< Manager of world specific toolboxes
        DockVizibilityManager docks_level_and_world; //!< Manager of level and world editors toolboxes

    public slots:
        void save();         //!< Save current file
        void save_as();      //!< Save current file with asking for save path
        void close_sw();     //!< Close current sub window
        void save_all();     //!< Save all opened files

        ///
        /// \brief OpenFile - Open file in the editor
        /// \param FilePath - path to file
        ///
        void OpenFile(QString FilePath, bool addToRecentList = true);

        ///
        /// \brief on_actionReload_triggered - Reload/Reopen current file
        ///
        void on_actionReload_triggered();
    private:
        bool _is_reloading;
    public slots:

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
    signals:
        void languageSwitched();
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
        LevelEdit   *activeLvlEditWin();                        //!< Active Window type 1
        LevelEdit   *activeLvlEditWin(QMdiSubWindow *wnd);
        NpcEdit     *activeNpcEditWin();                        //!< Active Window type 2
        NpcEdit     *activeNpcEditWin(QMdiSubWindow *wnd);
        WorldEdit   *activeWldEditWin();                        //!< Active Window type 3
        WorldEdit   *activeWldEditWin(QMdiSubWindow *wnd);
        int subWins();                                          //!< Returns number of opened subwindows
        QList<QMdiSubWindow*> allEditWins();                    //!< Returns all opened subwindows

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
    public:
        DebuggerBox* dock_DebuggerBox;

    private slots:
        void on_actionDebugger_triggered(bool checked);
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
        void on_actionAudioCvt_triggered();
// ////////////////////////////////////////////////////////



// //////////////////Other Tools////////////////////////
    private slots:
        void on_actionCDATA_clear_unused_triggered();
        void on_actionCDATA_Import_triggered();
        void on_actionSprite_editor_triggered();

        void on_actionFixWrongMasks_triggered();
        void on_actionClear_NPC_garbadge_triggered();
// ////////////////////////////////////////////////////////



// ///////////////// Music Player ////////////////////////
    public:
        int musicVolume();
    public slots:
        void setMusic(bool checked=false);
        void setMusicButton(bool checked);
        void on_actionPlayMusic_triggered(bool checked);

    private:
        QSlider* muVol;
// ///////////////////////////////////////////////////////


// ///////////////////// Bookmarks ////////////////////////        
    public:
        BookmarksBox * dock_BookmarksBox;

    private slots:
        void on_actionBookmarkBox_triggered(bool checked);
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
    public:
        LevelItemBox *dock_LvlItemBox;

    // update data of the toolboxes
    public slots:
        void UpdateCustomItems();

    private slots:
        void on_actionLVLToolBox_triggered(bool checked);

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
    public:
        LvlSectionProps *dock_LvlSectionProps;
    public slots:
        void on_actionGridEn_triggered(bool checked);

    private slots:
        void on_actionSection_Settings_triggered(bool checked);
        void on_actionLevWarp_triggered(bool checked);
        void on_actionLevOffScr_triggered(bool checked);
        void on_actionLevNoBack_triggered(bool checked);
        void on_actionLevUnderW_triggered(bool checked);

        void on_actionAnimation_triggered(bool checked);
        void on_actionCollisions_triggered(bool checked);

        void on_actionVBAlphaEmulate_toggled(bool arg1);
// ////////////////////////////////////////////////////////



// //////////////// Layers toolbox /////////////////////////
    public:
        LvlLayersBox* dock_LvlLayers;

    public slots:
        void LayerListsSync();

    private slots:
        void on_actionLayersBox_triggered(bool checked);
// ////////////////////////////////////////////////////////


// //////////////// Level Events toolbox //////////////////
    public:
        LvlEventsBox *dock_LvlEvents;
    public slots:
        void EventListsSync();
        void setEventsBox();

    private slots:
        void on_actionLevelEvents_triggered(bool checked);
// ////////////////////////////////////////////////////////



// //////////////// Warps toolbox /////////////////////////

    public:
        LvlWarpBox * dock_LvlWarpProps;

    private slots:
        void on_actionWarpsAndDoors_triggered(bool checked);

// ////////////////////////////////////////////////////////


// //////////////////// Level Search box /////////////////////////
    public:
        LvlSearchBox * dock_LvlSearchBox;

    private slots:
        void on_actionLVLSearchBox_triggered(bool checked);
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
    public:
        WorldSettingsBox* dock_WldSettingsBox;

    private slots:
        void on_actionWLDDisableMap_triggered(bool checked);
        void on_actionWLDFailRestart_triggered(bool checked);
        void on_actionWorld_settings_triggered(bool checked);
        void on_actionWLDProperties_triggered();
        void on_actionSemi_transparent_paths_triggered(bool checked);
// ////////////////////////////////////////////////////////

// ////////////////////World Item toolbox /////////////////
    public:
        WorldItemBox * dock_WldItemBox;

    private slots:
        void on_actionWLDToolBox_triggered(bool checked);
// ////////////////////////////////////////////////////////

// ///////////////World Item Properties box //////////////////
    public:
        WLD_ItemProps * dock_WldItemProps;
// ///////////////////////////////////////////////////////////


// //////////////////// World Search box /////////////////////////
    public:
        WldSearchBox* dock_WldSearchBox;

    private slots:
        void on_actionWLD_SearchBox_triggered(bool checked);

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
