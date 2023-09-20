/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2014-2023 Vitaly Novichkov <admin@wohlnet.ru>
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
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <memory>

#include <QMainWindow>
#include <QMdiArea>
#include <QPixmap>
#include <QAbstractListModel>
#include <QProcess>
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
#include <QAtomicInteger>
#ifdef Q_OS_WIN
#include <windows.h>
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
#include <common_features/safe_msg_box.h>

#include <tools/tilesets/tileset.h>
#include <tools/tilesets/tilesetgroupeditor.h>

#include <main_window/dock/toolboxes_protos.h>
#include <main_window/dock/_dock_vizman.h>

#include <main_window/plugins/pge_editorpluginmanager.h>
#include <testing/abstract_engine.h>

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

    /**
     * @brief Process initialization on application startup
     * @param configDir Path to configuration package directory
     * @param themePack Path to theme pack to load
     * @return true if everything has been successfully initialized, false if fatal error has occouped
     */
    bool initEverything(const QString &configDir, const QString &themePack, const QString &configAppPath = QString());

    //! Global game configucrations
    DataConfig configs;

/* //////////////////////Contents/////////////////////////////
 * COMMON
 * - Misc
 * - Miltilanguage
 * - Recent Files
 * - Sub-windows
 * -- Latest Active Window (See sub_windows.cpp for comments)
 * - Toolbars
 * - Dock widgets
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
 * Plugins
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
        void openFilesByArgs(QStringList args, int startAt=1);

        /*!
         * \brief Show a status bar message text
         * \param msg Message to show
         * \param time time in milliseconds how long message must be shown
         */
        void showStatusMsg(QString msg, int time=2000); //Send status message

        /**
         * @brief Apply currently set theme or load a default one
         */
        void applyCurrentTheme();

        /*!
         * \brief Apply icons/cusors/images theme from selected path (where valid theme.ini is located)
         * \param themeDir Path to the valid PGE Editor theme package
         */
        void applyTheme(QString themeDir = QString());

        //! Is everything has been successfuly initialized
        bool m_isAppInited;

        //! When application will be closed, restart it
        bool m_isAppRestartRequested = false;

        bool isAppRestartRequested();

        /**
         * @brief Partially load settings from a configuration file
         */
        void loadBasicSettings();

        /**
         * @brief Load settings from configuration file
         */
        void loadSettings();

        /*!
         * \brief Save settings into configuration file
         */
        void saveSettings();


        void applySetup(bool startup = false);

        ///
        /// \brief getCurrentSceneCoordinates Returns the scene coordinates either from level window or world window
        /// \param x The current x-coordinate.
        /// \param y The current y-coordinate.
        /// \return True, if the current window is either a level window or a world window.
        ///
        bool getCurrentSceneCoordinates(qreal &x, qreal &y);

    public:
        //! Thread-safe message box factory
        SafeMsgBox m_messageBoxer;

    signals:
        /*!
         * \brief Set SMBX64 strict mode (unsupported properties are will be greyed/disabled)
         * \param enabled SMBX64 strict mode enable state
         */
        void setSMBX64Strict(bool enabled);

    private:
        ///
        /// \brief formatErrorMsgBox Prints file format error message box
        /// \param filePath path to broken file
        /// \param lineNum number Number of line where error found. If <0 - don't show line information.
        /// \param lineContents contents of line with error. If empty string - don't show field.
        ///
        void formatErrorMsgBox(QString filePath, QString errorMessage, long lineNum=-1, QString lineContents="");
        ///
        /// \brief setDefaults Init settings on start application
        ///
        void setDefaults();
        ///
        /// \brief setUiDefults Init UI settings of application on start
        ///
        void setUiDefults();
        /**
         * @brief Initialize config pack side UI defaults
         */
        void setUiDefultsConfigPack();

        //! Manager of level specific toolboxes
        DockVizibilityManager docks_level;
        //! Manager of world specific toolboxes
        DockVizibilityManager docks_world;
        //! Manager of level and world editors toolboxes
        DockVizibilityManager docks_level_and_world;

    public slots:
        /*!
         * \brief Save current file
         */
        void save();
        /*!
         * \brief Save current file with asking for save path
         */
        void save_as();
        /*!
         * \brief Close current sub window
         */
        void close_sw();
        /*!
         * \brief Save all opened files
         */
        void save_all();

        ///
        /// \brief OpenFile - Open file in the editor
        /// \param FilePath - path to file
        ///
        void OpenFile(QString FilePath, bool addToRecentList = true);

        ///
        /// \brief on_actionReload_triggered - Reload/Reopen current file
        ///
        void on_actionReload_triggered();

        /*!
         * \brief Open folder where current file is located
         */
        void on_action_openEpisodeFolder_triggered();
        /*!
         * \brief Open folder which contains level/worldmap specific stuff
         */
        void on_action_openCustomFolder_triggered();
    private:

        //! Is file reloading process
        QAtomicInteger<bool> m_isFileReloading;
    public slots:

        ///
        /// \brief on_actionRefresh_menu_and_toolboxes_triggered - Refresh item boxes
        ///
        void on_actionRefresh_menu_and_toolboxes_triggered();

        ///
        /// \brief updateMenus Refresh the menubars
        /// \param force Refresh menubar even if window is not active
        ///
        void updateMenus(QMdiSubWindow *subWindow=nullptr, bool force=false);
        /*!
         * \brief Update "Window" Menu
         */
        void updateWindowMenu();
    public:
        /* Menu binds */

        QMenu *getFileMenu();
        QMenu *getEditMenu();
        QMenu *getViewMenu();
        QMenu *getLevelMenu();
        QMenu *getLevelCurrSectionMenu();
        QMenu *getLevelModSectionMenu();
        QMenu *getWorldMenu();
        QMenu *getTestMenu();
        QMenu *getScriptMenu();
        QMenu *getConfigMenu();
        QMenu *getToolsMenu();
        QMenu *getWindowMenu();
        QMenu *getPluginsMenu();
        QMenu *getHelpMenu();

    private:
        QTimer m_autoSaveTimer;

    private slots:
        /*!
         * \brief Open Developement Console
         */
        void on_actionShow_Development_Console_triggered();
        /*!
         * \brief Toggle full-screen mode
         * \param checked Full screen mode state
         */
        void on_actionSwitch_to_Fullscreen_triggered(bool checked);

        //New file
        /*!
         * \brief Dialog to choice what new format to create
         */
        void on_actionNewFile_triggered();
        /*!
         * \brief Open level editing sub-window with a blank file
         */
        void on_actionNewLevel_triggered();
        /*!
         * \brief Open NPC.TXT editing sub-window with a blank file
         */
        void on_actionNewNPC_config_triggered();
        /*!
         * \brief Open world map editing sub-window with a blank file
         */
        void on_actionNewWorld_map_triggered();

        //File menu
        /*!
         * \brief Show open dialog to select exist file to open
         */
        void on_OpenFile_triggered();
        /*!
         * \brief Save current file, or save as if file is untitled
         */
        void on_actionSave_triggered();
        /*!
         * \brief Open save dialog to save current file in custom place or in different file format
         */
        void on_actionSave_as_triggered();
        /*!
         * \brief Close current file
         */
        void on_actionClose_triggered();
        /*!
         * \brief Save all unsaved files
         */
        void on_actionSave_all_triggered();

        /*!
         * \brief Synchronize autosave settings
         */
        void updateAutoSaver();

        /*!
         * \brief Run the auto-save itself
         */
        void runAutoSave();

        /*!
         * \brief Export current workspace into image file
         */
        void on_actionExport_to_image_triggered();
        /*!
         * \brief Export level section of current file into image file
         */
        void on_actionExport_to_image_section_triggered();
        /*!
         * \brief Open application settings
         */
        void on_actionApplication_settings_triggered();
        /*!
         * \brief Exit from application
         */
        void on_Exit_triggered();
        /*!
         * \brief Custom context menu of the main window
         * \param pos Position of the mouse cursor
         */
        void on_MainWindow_customContextMenuRequested(const QPoint &pos);

        /*!
         * \brief A script editor box
         */
        void on_actionScriptEditor_triggered();
// ///////////////////////////////////////////////////////////


// //////////////////Miltilanguage///////////////////////////
    private slots:
        /*!
         * \brief Change language to selected in the language menu
         * \param action Menuitem onject pointer
         */
        void slotLanguageChanged(QAction *action);
    private:
        /*!
         * \brief Load language by two-letter code (en, ru, de, it, es, etc.)
         * \param rLanguage Two-letter language name (en, ru, de, it, es, etc.)
         */
        void loadLanguage(const QString& rLanguage);
        /*!
         * \brief Init settings of the translator and initialize default language
         */
        void setDefLang();
        /*!
         * \brief Switch translator module
         * \param translator Translator module
         * \param filename Path to the translation file
         * \return true if successfully loaded, false on any error
         */
        bool switchTranslator(QTranslator &translator, const QString &filename);
        /*!
         * \brief Syncronize state of the languages menu
         */
        void langListSync();
        //! contains the translations for this application
        QTranslator     m_translator;
        //! contains the translations for common modules
        QTranslator     m_translatorCommon;
        //! contains the translations for qt
        QTranslator     m_translatorQt;
        //! contains the currently loaded language
        QString         m_currLang;
        //! Path of language files. This is always fixed to /languages
        QString         m_langPath;
    signals:
        //! Is editor language was switched
        void languageSwitched();
// ///////////////////////////////////////////////////////////


// /////////////// Recent Files ///////////////////////////
    public slots:
        /*!
         * \brief Refresh recent file list from list
         */
        void SyncRecentFiles();

        ///
        /// \brief AddToRecentFiles Add item into recent file list
        /// \param FilePath Absolute path to the file
        ///
        void AddToRecentFiles(QString FilePath);

        //RecentFiles Menu Items
        /*!
         * \brief Open recent file #1
         */
        void on_action_recent1_triggered();
        /*!
         * \brief Open recent file #2
         */
        void on_action_recent2_triggered();
        /*!
         * \brief Open recent file #3
         */
        void on_action_recent3_triggered();
        /*!
         * \brief Open recent file #4
         */
        void on_action_recent4_triggered();
        /*!
         * \brief Open recent file #5
         */
        void on_action_recent5_triggered();
        /*!
         * \brief Open recent file #6
         */
        void on_action_recent6_triggered();
        /*!
         * \brief Open recent file #7
         */
        void on_action_recent7_triggered();
        /*!
         * \brief Open recent file #8
         */
        void on_action_recent8_triggered();
        /*!
         * \brief Open recent file #9
         */
        void on_action_recent9_triggered();
        /*!
         * \brief Open recent file #10
         */
        void on_action_recent10_triggered();
    private:
        //! List of recent files
        QList<QString> recentOpen;
// ////////////////////////////////////////////////////////


// ///////////////////// Sub-windows /////////////////////////
    public:
        enum WindowType
        {
            WND_NoWindow    = 0,
            WND_Level       = 1,
            WND_NpcTxt      = 2,
            WND_World       = 3
        };

        ///
        /// \brief activeChildWindow returns type of active sub window
        /// \return Active Window type (0 - nothing, 1 - level, 2 - NPC, 3 - World)
        ///
        int activeChildWindow();
        int activeChildWindow(QMdiSubWindow* wnd);

        /*!
         * \brief Retreives pointer to generic file editing window if current window is generic file editor sub-window
         * \return Pointer to generic sub-window, NULL if current window is not a generic file editor window
         */
        EditBase    *activeBaseEditWin();
        /*!
         * \brief Casts generic subwindow class into Generic file edit sub-window type
         * \param wnd Generic sub-window pointer
         * \return Generic file edit sub-window or NULL if given pointer is not a level editor
         */
        EditBase    *activeBaseEditWin(QMdiSubWindow *wnd);
        /*!
         * \brief Retreives pointer to level editing window if current window is level editor sub-window
         * \return Pointer to level sub-window, NULL if current window is not a level editor window
         */
        LevelEdit   *activeLvlEditWin();
        /*!
         * \brief Casts generic subwindow class into Level Editor sub-window type
         * \param wnd Generic sub-window pointer
         * \return Level Editor sub-window or NULL if given pointer is not a level editor
         */
        LevelEdit   *activeLvlEditWin(QMdiSubWindow *wnd);
        /*!
         * \brief Retreives pointer to NPC.TXT editing window if current window is NPC.TXT editor sub-window
         * \return Pointer to NPC.TXT sub-window, NULL if current window is not a NPC.TXT editor window
         */
        NpcEdit     *activeNpcEditWin();
        /*!
         * \brief Casts generic subwindow class into NPC.TXT Editor sub-window type
         * \param wnd Generic sub-window pointer
         * \return NPC.TXT Editor sub-window or NULL if given pointer is not a NPC.TXT editor
         */
        NpcEdit     *activeNpcEditWin(QMdiSubWindow *wnd);
        /*!
         * \brief Retreives pointer to world map editing window if current window is world map editor sub-window
         * \return Pointer to world map sub-window, NULL if current window is not a world map editor window
         */
        WorldEdit   *activeWldEditWin();
        /*!
         * \brief Casts generic subwindow class into World map Editor sub-window type
         * \param wnd Generic sub-window pointer
         * \return World map Editor sub-window or NULL if given pointer is not a World map editor
         */
        WorldEdit   *activeWldEditWin(QMdiSubWindow *wnd);
        /*!
         * \brief Count of existing sub-windows
         * \return Number of currently opened sub-windows
         */
        int subWins();
        /*!
         * \brief Returns all opened sub-windows
         * \return List of currently opened sub-windows
         */
        QList<QMdiSubWindow*> allEditWins();

    signals:
        /**
         * @brief is level window activated or activated window of another type
         * @param state true if level window activated
         */
        void windowActiveLevel(bool state);
        /**
         * @brief is NPC editor window activated or activated window of another type
         * @param state true if NPC editor window activated
         */
        void windowActiveNPC(bool state);
        /**
         * @brief is world map window activated or activated window of another type
         * @param state true if world map window activated
         */
        void windowActiveWorld(bool state);
        /**
         * @brief if level or world map window activated or another type
         * @param state true if editor or world map window activated
         */
        void windowActiveLevelWorld(bool state);

    public slots:
        /*!
         * \brief Create empty Level Editing sub-window
         * \return Level Editor pointer or NULL if creating of sub-window failed
         */
        LevelEdit   *createLvlChild(QMdiSubWindow **levelWindow_out = 0);
        /*!
         * \brief Create empty NPC.TXT Editing sub-window
         * \return NPC.TXT Editor pointer or NULL if creating of sub-window failed
         */
        NpcEdit     *createNPCChild(QMdiSubWindow **npcWindow_out = 0);
        /*!
         * \brief Create empty World Map Editing sub-window
         * \return World Map Editor pointer or NULL if creating of sub-window failed
         */
        WorldEdit   *createWldChild(QMdiSubWindow **worldWindow_out = 0);
        /*!
         * \brief Switch specific sub-window
         * \param window Genertic QWidget pointer to sub-window to activated
         */
        void setActiveSubWindow(QWidget *window);
        /*!
         * \brief Switch specific sub-window
         * \param window QMdiSubWindow pointer to sub-window to activated
         */
        void setActiveSubWindow(QMdiSubWindow *window);

        /*!
         * \brief Cascade sub-windows
         */
        void SWCascade();
        /*!
         * \brief Tile sub-windows
         */
        void SWTile();
        /*!
         * \brief Switch Separate Sub-windows view mode
         */
        void setSubView();
        /*!
         * \brief Switch Tabs view mode
         */
        void setTabView();


        // //////////////////SubWindows mapper///////////////////
    private:
        ///
        /// \brief findOpenedFileWin Finding the subWindow with already opened file
        /// \param fileName target file
        /// \return pointer to subWindow which case with target file
        ///
        QMdiSubWindow *findOpenedFileWin(const QString &fileName);
        // //////////////////////////////////////////////////////

        // /////////////// Latest Active Window ///////////////////
//    public slots://Junk functions, causes shit with music
//        void recordSwitchedWindow(QMdiSubWindow * window);
//        void recordRemovedWindow(QObject* possibleDeletedWindow);
    private:
        //! Last activated sub-window pointer
        QMdiSubWindow* LastActiveSubWindow;
        // ////////////////////////////////////////////////////////
// ///////////////////////////////////////////////////////////

// ///////////////////// Toolbars /////////////////////////
    private:
        //! Vanilla editor's toolbar
        QToolBar* m_toolbarVanilla;

// ////////////////////////////////////////////////////////

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
        void SwitchPlacingItem(int itemType, unsigned long itemID, bool dont_reset_props=false);
        void on_actionRectFill_triggered(bool checked);
        void on_actionCircleFill_triggered(bool checked);
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
    public slots:
        void showTipOfDay();
        void showWelcomeDialog();
    private slots:
        void on_actionContents_triggered();
        void on_actionTipOfDay_triggered();
        void on_actionWelcome_triggered();
        void on_actionAbout_triggered();
        void on_actionSMBX_like_GUI_triggered();
        void on_actionModern_GUI_triggered();
        void on_actionChange_log_triggered();
        void on_actionCheckUpdates_triggered();
// ////////////////////////////////////////////////////////

// //////////////// Configuration manager /////////////////
    private slots:
        void on_actionLoad_configs_triggered(); //!< Reload config
        void on_actionCurConfig_triggered();    //!< Config status
        void on_actionReConfigure_triggered();  //!< Run configure tool
        void on_actionChangeConfig_triggered(); //!< Change configuration
    private:
        QString currentConfigDir;
        bool askConfigAgain;
    signals:
        void configPackReloadBegin();
        void configPackReloaded();
// ////////////////////////////////////////////////////////

// //////////////////External tools////////////////////////
    private slots:
        void on_actionLazyFixTool_triggered();
        void on_actionGIFs2PNG_triggered();
        void on_actionPNG2GIFs_triggered();
        void on_actionAudioCvt_triggered();
        void on_actionExternalPalleteFilter_triggered();
        void on_actionExternalLevelWorldConverter_triggered();
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
        QSlider* m_ui_musicVolume;
// ///////////////////////////////////////////////////////


// ///////////////////// Bookmarks ////////////////////////
    public:
        BookmarksBox * dock_BookmarksBox;

    private slots:
        void on_actionBookmarkBox_triggered(bool checked);
// ////////////////////////////////////////////////////////

// ///////////////////// Variables ////////////////////////
    public:
        VariablesBox * dock_VariablesBox;

    private slots:
        void on_actionVariables_triggered(bool checked);
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
    public slots:
        void on_actionLevelProp_triggered();
// ////////////////////////////////////////////////////////


// ///////////////////Section toobar///////////////////////
    public:
        const size_t m_sectionButtonsCount = 21;
        QAction *m_sectionButtons[21] = {0};

    public slots:
        //Switch section
        void setCurrentLevelSection(int sectionId, int open = 0);
        void on_actionGotoLeftBottom_triggered();
        void on_actionGotoLeftTop_triggered();
        void on_actionGotoTopRight_triggered();
        void on_actionGotoRightBottom_triggered();

    private slots:
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
        void on_actionSectionMore_triggered();

        //Modify actions
        void on_actionCloneSectionTo_triggered();
        void on_actionSCT_Delete_triggered();
        void on_actionSCT_RotateLeft_triggered();
        void on_actionSCT_RotateRight_triggered();
        void on_actionSCT_FlipHorizontal_triggered();
        void on_actionSCT_FlipVertical_triggered();
    private:
        void deleteLevelSection(LevelEdit *edit, int section, long margin);

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
        void on_actionShowGrid_triggered(bool checked);
        void on_actionShowCameraGrid_triggered(bool checked);
        void customGrid(bool checked=false);

    private slots:
        void on_actionSection_Settings_triggered(bool checked);

        void on_actionAnimation_triggered(bool checked);
        void on_actionCollisions_triggered(bool checked);
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
        void refreshLunaLUAMenuItems();
        void on_actionCreateScriptLocal_triggered();
        void on_actionCreateScriptEpisode_triggered();
        void on_actionLunaLUA_lvl_triggered();
        void on_actionLunaLUA_eps_triggered();
        void on_actionLunaLUA_wld_triggered();

// ////////////////////////////////////////////////////////


// ////////////////////////////////////////////////////////////////////////////////
// ////////////////////////////World Editing///////////////////////////////////////
// ////////////////////////////////////////////////////////////////////////////////

// ////////////////////World Settings toolbox /////////////////
    public:
        //! World settings dock widget
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
        //! World map item box
        WorldItemBox * dock_WldItemBox;

    private slots:
        void on_actionWLDToolBox_triggered(bool checked);
// ////////////////////////////////////////////////////////

// ////////////////////World MusicBoxes toolbox /////////////////
    public:
        //! World map music boxes toolbox
        WorldMusicBoxItemBox * dock_WldMusicBoxes;

    private slots:
        void on_actionMusicBoxes_triggered(bool checked);
// ////////////////////////////////////////////////////////

// ///////////////World Item Properties box //////////////////
    public:
        //! World map item properties box
        WLD_ItemProps * dock_WldItemProps;
// ///////////////////////////////////////////////////////////


// //////////////////// World Search box /////////////////////////
    public:
        //! World map search box
        WldSearchBox* dock_WldSearchBox;

    private slots:
        /*!
         * \brief Show/Hide world map search box
         * \param checked Visibility state
         */
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
    private:
        void initTesting();
        void updateTestingCaps();
        void closeTesting();

    private slots:
        /*!
         * \brief Unlocks music button and starts music if that was started pre-testing state
         */
        void testingFinished();
        /*!
         * \brief Stops music playing and locks music button
         */
        void stopMusicForTesting();

        /*!
         * \brief Starts level testing with interprocess communication (File saving is not needed)
         */
        void on_action_doTest_triggered();
        /*!
         * \brief Starts world map testing with interprocess communication (File saving is not needed)
         */
        void on_action_doTestWld_triggered();
        /*!
         * \brief Starts level testing without interprocess communication (File saving is needed)
         */
        void on_action_doSafeTest_triggered();
        /*!
         * \brief Starts world map testing without interprocess communication (File saving is needed)
         */
        void on_action_doSafeTestWld_triggered();

        /*!
         * \brief Starts PGE Engine with current configuration package selected
         */
        void on_action_Start_Engine_triggered();
        /*!
         * \brief Settings of testing
         */
        void on_action_testSettings_triggered();

    private:
        friend class PgeEngine;
        friend class LunaTesterEngine;
        friend class TheXTechEngine;
        friend class SanBaEiRuntimeEngine;
        //! Default testing engine
        AbstractRuntimeEngine *m_testEngine = nullptr;
        //! All available engines
        QVector<std::unique_ptr<AbstractRuntimeEngine> *> m_testAllEngines;
        //! PGE Engine
        std::unique_ptr<AbstractRuntimeEngine> m_testPGE;
        //! LunaTester
        std::unique_ptr<AbstractRuntimeEngine> m_testLunaTester;
        //! TheXTech
        std::unique_ptr<AbstractRuntimeEngine> m_testTheXTech;
        //! SMBX-38A
        std::unique_ptr<AbstractRuntimeEngine> m_testSMBX38A;

// ////////////////////////////////////////////////////////////////////////////////
// /////////////////////////////////Plugins////////////////////////////////////////
// ////////////////////////////////////////////////////////////////////////////////
private:
    PGE_EditorPluginManager m_pluginManager;

    void initPlugins();
    void initPluginsInDir(const QDir& dir);

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
    Ui::MainWindow *ui;
};


#endif // MAINWINDOW_H
