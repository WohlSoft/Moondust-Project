/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 5.2.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QLocale>
#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QDockWidget>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QListView>
#include <QtWidgets/QListWidget>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMdiArea>
#include <QtWidgets/QMenu>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QRadioButton>
#include <QtWidgets/QScrollArea>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QTabWidget>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QAction *OpenFile;
    QAction *Exit;
    QAction *actionNewLevel;
    QAction *actionNewWorld_map;
    QAction *actionNPC_config;
    QAction *actionSave;
    QAction *actionSave_as;
    QAction *actionClose;
    QAction *actionSave_all;
    QAction *actionAbout;
    QAction *actionContents;
    QAction *actionSection_1;
    QAction *actionSection_2;
    QAction *actionSection_3;
    QAction *actionSection_4;
    QAction *actionSection_5;
    QAction *actionSection_6;
    QAction *actionSection_7;
    QAction *actionSection_8;
    QAction *actionSection_9;
    QAction *actionSection_10;
    QAction *actionSection_11;
    QAction *actionSection_12;
    QAction *actionSection_13;
    QAction *actionSection_14;
    QAction *actionSection_15;
    QAction *actionSection_16;
    QAction *actionSection_17;
    QAction *actionSection_18;
    QAction *actionSection_19;
    QAction *actionSection_20;
    QAction *actionSection_21;
    QAction *actionLevWarp;
    QAction *actionLevOffScr;
    QAction *actionLevNoBack;
    QAction *actionLevUnderW;
    QAction *actionExport_to_image;
    QAction *actionLevelProp;
    QAction *actionWLDDisableMap;
    QAction *actionWLDFailRestart;
    QAction *actionWLDNoChar1;
    QAction *actionWLDNoChar2;
    QAction *actionWLDNoChar3;
    QAction *actionWLDNoChar4;
    QAction *actionWLDNoChar5;
    QAction *actionWLDProperties;
    QAction *actionLVLToolBox;
    QAction *actionWLDToolBox;
    QAction *actionSelect;
    QAction *actionEriser;
    QAction *actionSection_Settings;
    QAction *actionLoad_configs;
    QAction *actionReset_position;
    QAction *actionGridEn;
    QAction *actionLockBlocks;
    QAction *actionLockBGO;
    QAction *actionLockNPC;
    QAction *actionLockDoors;
    QAction *actionLockWaters;
    QAction *actionSetFirstPlayer;
    QAction *actionSetSecondPlayer;
    QAction *actionPlayMusic;
    QAction *actionReload;
    QAction *actionHandScroll;
    QAction *actionUndo;
    QAction *actionRedo;
    QAction *actionWarpsAndDoors;
    QAction *actionAnimation;
    QAction *action_recent1;
    QAction *action_recent2;
    QAction *action_recent3;
    QAction *action_recent4;
    QAction *action_recent5;
    QAction *action_recent6;
    QAction *action_recent7;
    QAction *action_recent8;
    QAction *action_recent9;
    QAction *action_recent10;
    QAction *actionCopy;
    QAction *actionCollisions;
    QAction *actionDrawWater;
    QAction *actionDrawSand;
    QAction *actionPaste;
    QAction *actionLayersBox;
    QAction *actionEmpty;
    QMdiArea *centralWidget;
    QMenuBar *menuBar;
    QMenu *menu;
    QMenu *menuNew;
    QMenu *menuOpenRecent;
    QMenu *menuHelp;
    QMenu *menuLevel;
    QMenu *menuCurrent_section;
    QMenu *menuWorld;
    QMenu *menuDisable_characters;
    QMenu *menuView;
    QMenu *menuWindow;
    QMenu *menuTools;
    QMenu *menuEdit;
    QToolBar *mainToolBar;
    QStatusBar *statusBar;
    QDockWidget *LevelToolBox;
    QTabWidget *LevelToolBoxTabs;
    QWidget *Blocks;
    QGridLayout *BlocksG;
    QLabel *BlockCatLabel;
    QListWidget *BlockItemsList;
    QComboBox *BlockCatList;
    QCheckBox *BlockUniform;
    QWidget *BGOs;
    QGridLayout *BGOsG;
    QLabel *BGOCatLabel;
    QListWidget *BGOItemsList;
    QComboBox *BGOCatList;
    QCheckBox *BGOUniform;
    QScrollArea *npc;
    QWidget *npcscroll;
    QToolBar *EditionToolBar;
    QToolBar *LevelObjectToolbar;
    QToolBar *LevelSectionsToolBar;
    QDockWidget *WorldToolBox;
    QTabWidget *WorldToolBoxTabs;
    QScrollArea *Tiles;
    QListView *TilesItemBox;
    QScrollArea *Scenery;
    QWidget *SceneryScroll;
    QScrollArea *Level;
    QWidget *WLDLevelScroll;
    QScrollArea *WldMusicBox;
    QWidget *WLDMusicScroll;
    QGroupBox *groupBox_6;
    QPushButton *pushButton_2;
    QComboBox *comboBox_3;
    QLabel *label_12;
    QPushButton *pushButton_3;
    QDockWidget *LevelSectionSettings;
    QWidget *LevelSection;
    QGroupBox *LVLPropMusicGr;
    QComboBox *LVLPropsMusicNumber;
    QLabel *LVLPropCMusicLbl;
    QLineEdit *LVLPropsMusicCustom;
    QCheckBox *LVLPropsMusicCustomEn;
    QPushButton *LVLPropsMusicCustomBrowse;
    QGroupBox *LVLPropSettingsGrp;
    QVBoxLayout *verticalLayout_3;
    QCheckBox *LVLPropsLevelWarp;
    QCheckBox *LVLPropsOffScr;
    QCheckBox *LVLPropsNoTBack;
    QCheckBox *LVLPropsUnderWater;
    QGroupBox *LVLPropStyleGr;
    QVBoxLayout *verticalLayout;
    QLabel *LVLPropBckrImg;
    QComboBox *LVLPropsBackImage;
    QGroupBox *groupBox_5;
    QLabel *label_9;
    QLabel *LVLProp_CurSect;
    QPushButton *ResizeSection;
    QDockWidget *DoorsToolbox;
    QWidget *dockWidgetContents;
    QGroupBox *groupBox;
    QGroupBox *groupBox_4;
    QCheckBox *WarpLevelEntrance;
    QCheckBox *WarpLevelExit;
    QGroupBox *groupBox_7;
    QGroupBox *WarpEntranceGrp;
    QRadioButton *Entr_Left;
    QRadioButton *Entr_Right;
    QRadioButton *Entr_Up;
    QRadioButton *Entr_Down;
    QLabel *label_2;
    QGroupBox *WarpExitGrp;
    QRadioButton *Exit_Left;
    QRadioButton *Exit_Right;
    QRadioButton *Exit_Up;
    QRadioButton *Exit_Down;
    QLabel *label_7;
    QGroupBox *groupBox_8;
    QPushButton *WarpGetXYFromWorldMap;
    QLabel *label_6;
    QLineEdit *WarpToMapX;
    QLabel *label_5;
    QLineEdit *WarpToMapY;
    QGroupBox *groupBox_9;
    QLabel *label_8;
    QSpinBox *WarpToExitNu;
    QLineEdit *WarpLevelFile;
    QLabel *label_11;
    QPushButton *WarpBrowseLevels;
    QGroupBox *groupBox_10;
    QComboBox *WarpType;
    QLabel *label_3;
    QCheckBox *WarpNoYoshi;
    QLabel *label_4;
    QSpinBox *WarpNeedAStars;
    QCheckBox *WarpLock;
    QCheckBox *WarpAllowNPC;
    QPushButton *WarpSetEntrance;
    QPushButton *WarpSetExit;
    QCheckBox *WarpEntrancePlaced;
    QCheckBox *WarpExitPlaced;
    QComboBox *WarpList;
    QPushButton *WarpAdd;
    QPushButton *WarpRemove;
    QDockWidget *LevelLayers;
    QWidget *LevelLayersBox;
    QGridLayout *gridLayout;
    QPushButton *RemoveLayer;
    QPushButton *AddLayer;
    QPushButton *LockLayer;
    QListWidget *LvlLayerList;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QStringLiteral("MainWindow"));
        MainWindow->setWindowModality(Qt::NonModal);
        MainWindow->resize(1189, 1078);
        QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
        sizePolicy.setHorizontalStretch(4);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(MainWindow->sizePolicy().hasHeightForWidth());
        MainWindow->setSizePolicy(sizePolicy);
        MainWindow->setMinimumSize(QSize(0, 508));
        MainWindow->setFocusPolicy(Qt::NoFocus);
        MainWindow->setContextMenuPolicy(Qt::NoContextMenu);
        MainWindow->setAcceptDrops(true);
        QIcon icon;
        icon.addFile(QStringLiteral(":/images/mushroom16.png"), QSize(), QIcon::Normal, QIcon::Off);
        MainWindow->setWindowIcon(icon);
        MainWindow->setLocale(QLocale(QLocale::English, QLocale::UnitedStates));
        MainWindow->setTabShape(QTabWidget::Triangular);
        MainWindow->setDockOptions(QMainWindow::AllowTabbedDocks|QMainWindow::AnimatedDocks|QMainWindow::ForceTabbedDocks);
        OpenFile = new QAction(MainWindow);
        OpenFile->setObjectName(QStringLiteral("OpenFile"));
        OpenFile->setCheckable(false);
        QIcon icon1;
        icon1.addFile(QStringLiteral(":/images/open.png"), QSize(), QIcon::Normal, QIcon::Off);
        OpenFile->setIcon(icon1);
        OpenFile->setIconVisibleInMenu(true);
        Exit = new QAction(MainWindow);
        Exit->setObjectName(QStringLiteral("Exit"));
        actionNewLevel = new QAction(MainWindow);
        actionNewLevel->setObjectName(QStringLiteral("actionNewLevel"));
        actionNewWorld_map = new QAction(MainWindow);
        actionNewWorld_map->setObjectName(QStringLiteral("actionNewWorld_map"));
        actionNPC_config = new QAction(MainWindow);
        actionNPC_config->setObjectName(QStringLiteral("actionNPC_config"));
        actionSave = new QAction(MainWindow);
        actionSave->setObjectName(QStringLiteral("actionSave"));
        actionSave->setEnabled(false);
        QIcon icon2;
        icon2.addFile(QStringLiteral(":/images/save.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionSave->setIcon(icon2);
        actionSave->setIconVisibleInMenu(true);
        actionSave_as = new QAction(MainWindow);
        actionSave_as->setObjectName(QStringLiteral("actionSave_as"));
        actionSave_as->setEnabled(false);
        QIcon icon3;
        icon3.addFile(QStringLiteral(":/images/saveas.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionSave_as->setIcon(icon3);
        actionSave_as->setIconVisibleInMenu(true);
        actionClose = new QAction(MainWindow);
        actionClose->setObjectName(QStringLiteral("actionClose"));
        actionClose->setEnabled(false);
        actionSave_all = new QAction(MainWindow);
        actionSave_all->setObjectName(QStringLiteral("actionSave_all"));
        actionSave_all->setEnabled(false);
        actionAbout = new QAction(MainWindow);
        actionAbout->setObjectName(QStringLiteral("actionAbout"));
        actionAbout->setShortcutContext(Qt::ApplicationShortcut);
        actionAbout->setMenuRole(QAction::NoRole);
        actionContents = new QAction(MainWindow);
        actionContents->setObjectName(QStringLiteral("actionContents"));
        actionContents->setEnabled(false);
        actionSection_1 = new QAction(MainWindow);
        actionSection_1->setObjectName(QStringLiteral("actionSection_1"));
        actionSection_1->setCheckable(true);
        actionSection_1->setEnabled(false);
        QIcon icon4;
        icon4.addFile(QStringLiteral(":/images/01.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionSection_1->setIcon(icon4);
        actionSection_1->setIconVisibleInMenu(false);
        actionSection_2 = new QAction(MainWindow);
        actionSection_2->setObjectName(QStringLiteral("actionSection_2"));
        actionSection_2->setCheckable(true);
        actionSection_2->setEnabled(false);
        QIcon icon5;
        icon5.addFile(QStringLiteral(":/images/02.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionSection_2->setIcon(icon5);
        actionSection_2->setIconVisibleInMenu(false);
        actionSection_3 = new QAction(MainWindow);
        actionSection_3->setObjectName(QStringLiteral("actionSection_3"));
        actionSection_3->setCheckable(true);
        actionSection_3->setEnabled(false);
        QIcon icon6;
        icon6.addFile(QStringLiteral(":/images/03.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionSection_3->setIcon(icon6);
        actionSection_3->setIconVisibleInMenu(false);
        actionSection_4 = new QAction(MainWindow);
        actionSection_4->setObjectName(QStringLiteral("actionSection_4"));
        actionSection_4->setCheckable(true);
        actionSection_4->setEnabled(false);
        QIcon icon7;
        icon7.addFile(QStringLiteral(":/images/04.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionSection_4->setIcon(icon7);
        actionSection_4->setIconVisibleInMenu(false);
        actionSection_5 = new QAction(MainWindow);
        actionSection_5->setObjectName(QStringLiteral("actionSection_5"));
        actionSection_5->setCheckable(true);
        actionSection_5->setEnabled(false);
        QIcon icon8;
        icon8.addFile(QStringLiteral(":/images/05.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionSection_5->setIcon(icon8);
        actionSection_5->setIconVisibleInMenu(false);
        actionSection_6 = new QAction(MainWindow);
        actionSection_6->setObjectName(QStringLiteral("actionSection_6"));
        actionSection_6->setCheckable(true);
        actionSection_6->setEnabled(false);
        QIcon icon9;
        icon9.addFile(QStringLiteral(":/images/06.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionSection_6->setIcon(icon9);
        actionSection_6->setIconVisibleInMenu(false);
        actionSection_7 = new QAction(MainWindow);
        actionSection_7->setObjectName(QStringLiteral("actionSection_7"));
        actionSection_7->setCheckable(true);
        actionSection_7->setEnabled(false);
        QIcon icon10;
        icon10.addFile(QStringLiteral(":/images/07.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionSection_7->setIcon(icon10);
        actionSection_7->setIconVisibleInMenu(false);
        actionSection_8 = new QAction(MainWindow);
        actionSection_8->setObjectName(QStringLiteral("actionSection_8"));
        actionSection_8->setCheckable(true);
        actionSection_8->setEnabled(false);
        QIcon icon11;
        icon11.addFile(QStringLiteral(":/images/08.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionSection_8->setIcon(icon11);
        actionSection_8->setIconVisibleInMenu(false);
        actionSection_9 = new QAction(MainWindow);
        actionSection_9->setObjectName(QStringLiteral("actionSection_9"));
        actionSection_9->setCheckable(true);
        actionSection_9->setEnabled(false);
        QIcon icon12;
        icon12.addFile(QStringLiteral(":/images/09.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionSection_9->setIcon(icon12);
        actionSection_9->setIconVisibleInMenu(false);
        actionSection_10 = new QAction(MainWindow);
        actionSection_10->setObjectName(QStringLiteral("actionSection_10"));
        actionSection_10->setCheckable(true);
        actionSection_10->setEnabled(false);
        QIcon icon13;
        icon13.addFile(QStringLiteral(":/images/10.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionSection_10->setIcon(icon13);
        actionSection_10->setIconVisibleInMenu(false);
        actionSection_11 = new QAction(MainWindow);
        actionSection_11->setObjectName(QStringLiteral("actionSection_11"));
        actionSection_11->setCheckable(true);
        actionSection_11->setEnabled(false);
        QIcon icon14;
        icon14.addFile(QStringLiteral(":/images/11.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionSection_11->setIcon(icon14);
        actionSection_11->setIconVisibleInMenu(false);
        actionSection_12 = new QAction(MainWindow);
        actionSection_12->setObjectName(QStringLiteral("actionSection_12"));
        actionSection_12->setCheckable(true);
        actionSection_12->setEnabled(false);
        QIcon icon15;
        icon15.addFile(QStringLiteral(":/images/12.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionSection_12->setIcon(icon15);
        actionSection_12->setIconVisibleInMenu(false);
        actionSection_13 = new QAction(MainWindow);
        actionSection_13->setObjectName(QStringLiteral("actionSection_13"));
        actionSection_13->setCheckable(true);
        actionSection_13->setEnabled(false);
        QIcon icon16;
        icon16.addFile(QStringLiteral(":/images/13.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionSection_13->setIcon(icon16);
        actionSection_13->setIconVisibleInMenu(false);
        actionSection_14 = new QAction(MainWindow);
        actionSection_14->setObjectName(QStringLiteral("actionSection_14"));
        actionSection_14->setCheckable(true);
        actionSection_14->setEnabled(false);
        QIcon icon17;
        icon17.addFile(QStringLiteral(":/images/14.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionSection_14->setIcon(icon17);
        actionSection_14->setIconVisibleInMenu(false);
        actionSection_15 = new QAction(MainWindow);
        actionSection_15->setObjectName(QStringLiteral("actionSection_15"));
        actionSection_15->setCheckable(true);
        actionSection_15->setEnabled(false);
        QIcon icon18;
        icon18.addFile(QStringLiteral(":/images/15.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionSection_15->setIcon(icon18);
        actionSection_15->setIconVisibleInMenu(false);
        actionSection_16 = new QAction(MainWindow);
        actionSection_16->setObjectName(QStringLiteral("actionSection_16"));
        actionSection_16->setCheckable(true);
        actionSection_16->setEnabled(false);
        QIcon icon19;
        icon19.addFile(QStringLiteral(":/images/16.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionSection_16->setIcon(icon19);
        actionSection_16->setIconVisibleInMenu(false);
        actionSection_17 = new QAction(MainWindow);
        actionSection_17->setObjectName(QStringLiteral("actionSection_17"));
        actionSection_17->setCheckable(true);
        actionSection_17->setEnabled(false);
        QIcon icon20;
        icon20.addFile(QStringLiteral(":/images/17.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionSection_17->setIcon(icon20);
        actionSection_17->setIconVisibleInMenu(false);
        actionSection_18 = new QAction(MainWindow);
        actionSection_18->setObjectName(QStringLiteral("actionSection_18"));
        actionSection_18->setCheckable(true);
        actionSection_18->setEnabled(false);
        QIcon icon21;
        icon21.addFile(QStringLiteral(":/images/18.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionSection_18->setIcon(icon21);
        actionSection_18->setIconVisibleInMenu(false);
        actionSection_19 = new QAction(MainWindow);
        actionSection_19->setObjectName(QStringLiteral("actionSection_19"));
        actionSection_19->setCheckable(true);
        actionSection_19->setEnabled(false);
        QIcon icon22;
        icon22.addFile(QStringLiteral(":/images/19.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionSection_19->setIcon(icon22);
        actionSection_19->setIconVisibleInMenu(false);
        actionSection_20 = new QAction(MainWindow);
        actionSection_20->setObjectName(QStringLiteral("actionSection_20"));
        actionSection_20->setCheckable(true);
        actionSection_20->setEnabled(false);
        QIcon icon23;
        icon23.addFile(QStringLiteral(":/images/20.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionSection_20->setIcon(icon23);
        actionSection_20->setIconVisibleInMenu(false);
        actionSection_21 = new QAction(MainWindow);
        actionSection_21->setObjectName(QStringLiteral("actionSection_21"));
        actionSection_21->setCheckable(true);
        actionSection_21->setEnabled(false);
        QIcon icon24;
        icon24.addFile(QStringLiteral(":/images/21.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionSection_21->setIcon(icon24);
        actionSection_21->setIconVisibleInMenu(false);
        actionLevWarp = new QAction(MainWindow);
        actionLevWarp->setObjectName(QStringLiteral("actionLevWarp"));
        actionLevWarp->setCheckable(true);
        actionLevWarp->setEnabled(false);
        actionLevWarp->setIconVisibleInMenu(false);
        actionLevOffScr = new QAction(MainWindow);
        actionLevOffScr->setObjectName(QStringLiteral("actionLevOffScr"));
        actionLevOffScr->setCheckable(true);
        actionLevOffScr->setEnabled(false);
        actionLevNoBack = new QAction(MainWindow);
        actionLevNoBack->setObjectName(QStringLiteral("actionLevNoBack"));
        actionLevNoBack->setCheckable(true);
        actionLevNoBack->setEnabled(false);
        actionLevUnderW = new QAction(MainWindow);
        actionLevUnderW->setObjectName(QStringLiteral("actionLevUnderW"));
        actionLevUnderW->setCheckable(true);
        actionLevUnderW->setEnabled(false);
        actionExport_to_image = new QAction(MainWindow);
        actionExport_to_image->setObjectName(QStringLiteral("actionExport_to_image"));
        actionExport_to_image->setEnabled(false);
        actionLevelProp = new QAction(MainWindow);
        actionLevelProp->setObjectName(QStringLiteral("actionLevelProp"));
        actionLevelProp->setEnabled(false);
        actionWLDDisableMap = new QAction(MainWindow);
        actionWLDDisableMap->setObjectName(QStringLiteral("actionWLDDisableMap"));
        actionWLDDisableMap->setCheckable(true);
        actionWLDFailRestart = new QAction(MainWindow);
        actionWLDFailRestart->setObjectName(QStringLiteral("actionWLDFailRestart"));
        actionWLDFailRestart->setCheckable(true);
        actionWLDNoChar1 = new QAction(MainWindow);
        actionWLDNoChar1->setObjectName(QStringLiteral("actionWLDNoChar1"));
        actionWLDNoChar1->setCheckable(true);
        actionWLDNoChar2 = new QAction(MainWindow);
        actionWLDNoChar2->setObjectName(QStringLiteral("actionWLDNoChar2"));
        actionWLDNoChar2->setCheckable(true);
        actionWLDNoChar3 = new QAction(MainWindow);
        actionWLDNoChar3->setObjectName(QStringLiteral("actionWLDNoChar3"));
        actionWLDNoChar3->setCheckable(true);
        actionWLDNoChar4 = new QAction(MainWindow);
        actionWLDNoChar4->setObjectName(QStringLiteral("actionWLDNoChar4"));
        actionWLDNoChar4->setCheckable(true);
        actionWLDNoChar5 = new QAction(MainWindow);
        actionWLDNoChar5->setObjectName(QStringLiteral("actionWLDNoChar5"));
        actionWLDNoChar5->setCheckable(true);
        actionWLDProperties = new QAction(MainWindow);
        actionWLDProperties->setObjectName(QStringLiteral("actionWLDProperties"));
        actionLVLToolBox = new QAction(MainWindow);
        actionLVLToolBox->setObjectName(QStringLiteral("actionLVLToolBox"));
        actionLVLToolBox->setCheckable(true);
        QIcon icon25;
        icon25.addFile(QStringLiteral(":/images/level.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionLVLToolBox->setIcon(icon25);
        actionLVLToolBox->setIconVisibleInMenu(true);
        actionWLDToolBox = new QAction(MainWindow);
        actionWLDToolBox->setObjectName(QStringLiteral("actionWLDToolBox"));
        actionWLDToolBox->setCheckable(true);
        QIcon icon26;
        icon26.addFile(QStringLiteral(":/images/world.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionWLDToolBox->setIcon(icon26);
        actionWLDToolBox->setIconVisibleInMenu(false);
        actionSelect = new QAction(MainWindow);
        actionSelect->setObjectName(QStringLiteral("actionSelect"));
        actionSelect->setCheckable(true);
        actionSelect->setChecked(false);
        actionSelect->setEnabled(false);
        QIcon icon27;
        icon27.addFile(QStringLiteral(":/images/arrow.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionSelect->setIcon(icon27);
        actionSelect->setIconVisibleInMenu(false);
        actionEriser = new QAction(MainWindow);
        actionEriser->setObjectName(QStringLiteral("actionEriser"));
        actionEriser->setCheckable(true);
        actionEriser->setEnabled(false);
        QIcon icon28;
        icon28.addFile(QStringLiteral(":/images/rubber.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionEriser->setIcon(icon28);
        actionSection_Settings = new QAction(MainWindow);
        actionSection_Settings->setObjectName(QStringLiteral("actionSection_Settings"));
        actionSection_Settings->setCheckable(true);
        QIcon icon29;
        icon29.addFile(QStringLiteral(":/images/section.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionSection_Settings->setIcon(icon29);
        actionSection_Settings->setVisible(true);
        actionSection_Settings->setIconVisibleInMenu(true);
        actionLoad_configs = new QAction(MainWindow);
        actionLoad_configs->setObjectName(QStringLiteral("actionLoad_configs"));
        actionLoad_configs->setIconVisibleInMenu(false);
        actionReset_position = new QAction(MainWindow);
        actionReset_position->setObjectName(QStringLiteral("actionReset_position"));
        actionReset_position->setEnabled(false);
        QIcon icon30;
        icon30.addFile(QStringLiteral(":/images/reset_pos.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionReset_position->setIcon(icon30);
        actionGridEn = new QAction(MainWindow);
        actionGridEn->setObjectName(QStringLiteral("actionGridEn"));
        actionGridEn->setCheckable(true);
        actionGridEn->setEnabled(false);
        QIcon icon31;
        icon31.addFile(QStringLiteral(":/images/grid.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionGridEn->setIcon(icon31);
        actionLockBlocks = new QAction(MainWindow);
        actionLockBlocks->setObjectName(QStringLiteral("actionLockBlocks"));
        actionLockBlocks->setCheckable(true);
        QIcon icon32;
        icon32.addFile(QStringLiteral(":/locks/block_op.png"), QSize(), QIcon::Normal, QIcon::Off);
        icon32.addFile(QStringLiteral(":/locks/block_cl.png"), QSize(), QIcon::Normal, QIcon::On);
        actionLockBlocks->setIcon(icon32);
        actionLockBGO = new QAction(MainWindow);
        actionLockBGO->setObjectName(QStringLiteral("actionLockBGO"));
        actionLockBGO->setCheckable(true);
        QIcon icon33;
        icon33.addFile(QStringLiteral(":/locks/bgo_op.png"), QSize(), QIcon::Normal, QIcon::Off);
        icon33.addFile(QStringLiteral(":/locks/bgo_cl.png"), QSize(), QIcon::Normal, QIcon::On);
        actionLockBGO->setIcon(icon33);
        actionLockNPC = new QAction(MainWindow);
        actionLockNPC->setObjectName(QStringLiteral("actionLockNPC"));
        actionLockNPC->setCheckable(true);
        QIcon icon34;
        icon34.addFile(QStringLiteral(":/locks/npc_op.png"), QSize(), QIcon::Normal, QIcon::Off);
        icon34.addFile(QStringLiteral(":/locks/npc_cl.png"), QSize(), QIcon::Normal, QIcon::On);
        actionLockNPC->setIcon(icon34);
        actionLockDoors = new QAction(MainWindow);
        actionLockDoors->setObjectName(QStringLiteral("actionLockDoors"));
        actionLockDoors->setCheckable(true);
        QIcon icon35;
        icon35.addFile(QStringLiteral(":/locks/door_op.png"), QSize(), QIcon::Normal, QIcon::Off);
        icon35.addFile(QStringLiteral(":/locks/door_cl.png"), QSize(), QIcon::Normal, QIcon::On);
        actionLockDoors->setIcon(icon35);
        actionLockWaters = new QAction(MainWindow);
        actionLockWaters->setObjectName(QStringLiteral("actionLockWaters"));
        actionLockWaters->setCheckable(true);
        QIcon icon36;
        icon36.addFile(QStringLiteral(":/locks/water_op.png"), QSize(), QIcon::Normal, QIcon::Off);
        icon36.addFile(QStringLiteral(":/locks/water_cl.png"), QSize(), QIcon::Normal, QIcon::On);
        actionLockWaters->setIcon(icon36);
        actionSetFirstPlayer = new QAction(MainWindow);
        actionSetFirstPlayer->setObjectName(QStringLiteral("actionSetFirstPlayer"));
        actionSetFirstPlayer->setCheckable(true);
        QIcon icon37;
        icon37.addFile(QStringLiteral(":/images/player1_start.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionSetFirstPlayer->setIcon(icon37);
        actionSetSecondPlayer = new QAction(MainWindow);
        actionSetSecondPlayer->setObjectName(QStringLiteral("actionSetSecondPlayer"));
        actionSetSecondPlayer->setCheckable(true);
        QIcon icon38;
        icon38.addFile(QStringLiteral(":/images/player2_start.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionSetSecondPlayer->setIcon(icon38);
        actionPlayMusic = new QAction(MainWindow);
        actionPlayMusic->setObjectName(QStringLiteral("actionPlayMusic"));
        actionPlayMusic->setCheckable(true);
        QIcon icon39;
        icon39.addFile(QStringLiteral(":/images/playmusic.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionPlayMusic->setIcon(icon39);
        actionReload = new QAction(MainWindow);
        actionReload->setObjectName(QStringLiteral("actionReload"));
        actionReload->setEnabled(false);
        QIcon icon40;
        icon40.addFile(QStringLiteral(":/reload.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionReload->setIcon(icon40);
        actionHandScroll = new QAction(MainWindow);
        actionHandScroll->setObjectName(QStringLiteral("actionHandScroll"));
        actionHandScroll->setCheckable(true);
        actionHandScroll->setEnabled(false);
        QIcon icon41;
        icon41.addFile(QStringLiteral(":/hand.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionHandScroll->setIcon(icon41);
        actionUndo = new QAction(MainWindow);
        actionUndo->setObjectName(QStringLiteral("actionUndo"));
        actionUndo->setEnabled(false);
        actionRedo = new QAction(MainWindow);
        actionRedo->setObjectName(QStringLiteral("actionRedo"));
        actionRedo->setEnabled(false);
        actionWarpsAndDoors = new QAction(MainWindow);
        actionWarpsAndDoors->setObjectName(QStringLiteral("actionWarpsAndDoors"));
        actionWarpsAndDoors->setCheckable(true);
        QIcon icon42;
        icon42.addFile(QStringLiteral(":/doors.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionWarpsAndDoors->setIcon(icon42);
        actionAnimation = new QAction(MainWindow);
        actionAnimation->setObjectName(QStringLiteral("actionAnimation"));
        actionAnimation->setCheckable(true);
        action_recent1 = new QAction(MainWindow);
        action_recent1->setObjectName(QStringLiteral("action_recent1"));
        action_recent1->setEnabled(false);
        action_recent2 = new QAction(MainWindow);
        action_recent2->setObjectName(QStringLiteral("action_recent2"));
        action_recent2->setEnabled(false);
        action_recent3 = new QAction(MainWindow);
        action_recent3->setObjectName(QStringLiteral("action_recent3"));
        action_recent3->setEnabled(false);
        action_recent4 = new QAction(MainWindow);
        action_recent4->setObjectName(QStringLiteral("action_recent4"));
        action_recent4->setEnabled(false);
        action_recent5 = new QAction(MainWindow);
        action_recent5->setObjectName(QStringLiteral("action_recent5"));
        action_recent5->setEnabled(false);
        action_recent6 = new QAction(MainWindow);
        action_recent6->setObjectName(QStringLiteral("action_recent6"));
        action_recent6->setEnabled(false);
        action_recent7 = new QAction(MainWindow);
        action_recent7->setObjectName(QStringLiteral("action_recent7"));
        action_recent7->setEnabled(false);
        action_recent8 = new QAction(MainWindow);
        action_recent8->setObjectName(QStringLiteral("action_recent8"));
        action_recent8->setEnabled(false);
        action_recent9 = new QAction(MainWindow);
        action_recent9->setObjectName(QStringLiteral("action_recent9"));
        action_recent9->setEnabled(false);
        action_recent10 = new QAction(MainWindow);
        action_recent10->setObjectName(QStringLiteral("action_recent10"));
        action_recent10->setEnabled(false);
        actionCopy = new QAction(MainWindow);
        actionCopy->setObjectName(QStringLiteral("actionCopy"));
        actionCollisions = new QAction(MainWindow);
        actionCollisions->setObjectName(QStringLiteral("actionCollisions"));
        actionCollisions->setCheckable(true);
        actionDrawWater = new QAction(MainWindow);
        actionDrawWater->setObjectName(QStringLiteral("actionDrawWater"));
        actionDrawWater->setCheckable(true);
        QIcon icon43;
        icon43.addFile(QStringLiteral(":/drawWater.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionDrawWater->setIcon(icon43);
        actionDrawSand = new QAction(MainWindow);
        actionDrawSand->setObjectName(QStringLiteral("actionDrawSand"));
        actionDrawSand->setCheckable(true);
        QIcon icon44;
        icon44.addFile(QStringLiteral(":/drawQuickSand.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionDrawSand->setIcon(icon44);
        actionPaste = new QAction(MainWindow);
        actionPaste->setObjectName(QStringLiteral("actionPaste"));
        actionLayersBox = new QAction(MainWindow);
        actionLayersBox->setObjectName(QStringLiteral("actionLayersBox"));
        actionLayersBox->setCheckable(true);
        QIcon icon45;
        icon45.addFile(QStringLiteral(":/layers.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionLayersBox->setIcon(icon45);
        actionEmpty = new QAction(MainWindow);
        actionEmpty->setObjectName(QStringLiteral("actionEmpty"));
        actionEmpty->setEnabled(false);
        centralWidget = new QMdiArea(MainWindow);
        centralWidget->setObjectName(QStringLiteral("centralWidget"));
        centralWidget->setContextMenuPolicy(Qt::NoContextMenu);
        centralWidget->setAcceptDrops(true);
        centralWidget->setFrameShape(QFrame::Panel);
        centralWidget->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
        centralWidget->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
        QBrush brush(QColor(132, 132, 132, 255));
        brush.setStyle(Qt::SolidPattern);
        centralWidget->setBackground(brush);
        centralWidget->setViewMode(QMdiArea::SubWindowView);
        centralWidget->setDocumentMode(true);
        centralWidget->setTabPosition(QTabWidget::North);
        MainWindow->setCentralWidget(centralWidget);
        menuBar = new QMenuBar(MainWindow);
        menuBar->setObjectName(QStringLiteral("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 1189, 21));
        menuBar->setContextMenuPolicy(Qt::NoContextMenu);
        menu = new QMenu(menuBar);
        menu->setObjectName(QStringLiteral("menu"));
        menuNew = new QMenu(menu);
        menuNew->setObjectName(QStringLiteral("menuNew"));
        menuOpenRecent = new QMenu(menu);
        menuOpenRecent->setObjectName(QStringLiteral("menuOpenRecent"));
        menuHelp = new QMenu(menuBar);
        menuHelp->setObjectName(QStringLiteral("menuHelp"));
        menuLevel = new QMenu(menuBar);
        menuLevel->setObjectName(QStringLiteral("menuLevel"));
        menuLevel->setEnabled(true);
        menuCurrent_section = new QMenu(menuLevel);
        menuCurrent_section->setObjectName(QStringLiteral("menuCurrent_section"));
        menuCurrent_section->setEnabled(true);
        menuWorld = new QMenu(menuBar);
        menuWorld->setObjectName(QStringLiteral("menuWorld"));
        menuWorld->setEnabled(true);
        menuDisable_characters = new QMenu(menuWorld);
        menuDisable_characters->setObjectName(QStringLiteral("menuDisable_characters"));
        menuView = new QMenu(menuBar);
        menuView->setObjectName(QStringLiteral("menuView"));
        menuWindow = new QMenu(menuBar);
        menuWindow->setObjectName(QStringLiteral("menuWindow"));
        menuTools = new QMenu(menuBar);
        menuTools->setObjectName(QStringLiteral("menuTools"));
        menuEdit = new QMenu(menuBar);
        menuEdit->setObjectName(QStringLiteral("menuEdit"));
        MainWindow->setMenuBar(menuBar);
        mainToolBar = new QToolBar(MainWindow);
        mainToolBar->setObjectName(QStringLiteral("mainToolBar"));
        mainToolBar->setEnabled(true);
        mainToolBar->setMinimumSize(QSize(0, 0));
        mainToolBar->setAllowedAreas(Qt::TopToolBarArea);
        mainToolBar->setIconSize(QSize(24, 24));
        MainWindow->addToolBar(Qt::TopToolBarArea, mainToolBar);
        statusBar = new QStatusBar(MainWindow);
        statusBar->setObjectName(QStringLiteral("statusBar"));
        MainWindow->setStatusBar(statusBar);
        LevelToolBox = new QDockWidget(MainWindow);
        LevelToolBox->setObjectName(QStringLiteral("LevelToolBox"));
        LevelToolBox->setEnabled(true);
        QSizePolicy sizePolicy1(QSizePolicy::Preferred, QSizePolicy::Preferred);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(LevelToolBox->sizePolicy().hasHeightForWidth());
        LevelToolBox->setSizePolicy(sizePolicy1);
        LevelToolBox->setMinimumSize(QSize(200, 200));
        LevelToolBox->setMaximumSize(QSize(524287, 524287));
        LevelToolBox->setFocusPolicy(Qt::NoFocus);
        QIcon icon46;
        icon46.addFile(QStringLiteral(":/images/level16.png"), QSize(), QIcon::Normal, QIcon::Off);
        LevelToolBox->setWindowIcon(icon46);
        LevelToolBox->setStyleSheet(QStringLiteral("font: 8pt \"Liberation Sans\";"));
        LevelToolBox->setAllowedAreas(Qt::BottomDockWidgetArea|Qt::LeftDockWidgetArea|Qt::RightDockWidgetArea);
        LevelToolBoxTabs = new QTabWidget();
        LevelToolBoxTabs->setObjectName(QStringLiteral("LevelToolBoxTabs"));
        Blocks = new QWidget();
        Blocks->setObjectName(QStringLiteral("Blocks"));
        BlocksG = new QGridLayout(Blocks);
        BlocksG->setSpacing(0);
        BlocksG->setContentsMargins(11, 11, 11, 11);
        BlocksG->setObjectName(QStringLiteral("BlocksG"));
        BlocksG->setContentsMargins(0, 0, 0, 0);
        BlockCatLabel = new QLabel(Blocks);
        BlockCatLabel->setObjectName(QStringLiteral("BlockCatLabel"));

        BlocksG->addWidget(BlockCatLabel, 0, 0, 1, 1);

        BlockItemsList = new QListWidget(Blocks);
        QIcon icon47;
        icon47.addFile(QStringLiteral(":/images/mushroom.png"), QSize(), QIcon::Normal, QIcon::Off);
        QListWidgetItem *__qlistwidgetitem = new QListWidgetItem(BlockItemsList);
        __qlistwidgetitem->setIcon(icon47);
        __qlistwidgetitem->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);
        BlockItemsList->setObjectName(QStringLiteral("BlockItemsList"));
        BlockItemsList->setLineWidth(1);
        BlockItemsList->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
        BlockItemsList->setSizeAdjustPolicy(QAbstractScrollArea::AdjustToContents);
        BlockItemsList->setEditTriggers(QAbstractItemView::NoEditTriggers);
        BlockItemsList->setProperty("showDropIndicator", QVariant(false));
        BlockItemsList->setDragEnabled(false);
        BlockItemsList->setDragDropOverwriteMode(false);
        BlockItemsList->setDragDropMode(QAbstractItemView::DragDrop);
        BlockItemsList->setSelectionMode(QAbstractItemView::SingleSelection);
        BlockItemsList->setSelectionBehavior(QAbstractItemView::SelectItems);
        BlockItemsList->setIconSize(QSize(48, 48));
        BlockItemsList->setTextElideMode(Qt::ElideNone);
        BlockItemsList->setMovement(QListView::Snap);
        BlockItemsList->setResizeMode(QListView::Adjust);
        BlockItemsList->setLayoutMode(QListView::Batched);
        BlockItemsList->setSpacing(20);
        BlockItemsList->setViewMode(QListView::IconMode);
        BlockItemsList->setUniformItemSizes(false);
        BlockItemsList->setBatchSize(150);
        BlockItemsList->setWordWrap(true);
        BlockItemsList->setSortingEnabled(true);

        BlocksG->addWidget(BlockItemsList, 1, 0, 1, 3);

        BlockCatList = new QComboBox(Blocks);
        BlockCatList->setObjectName(QStringLiteral("BlockCatList"));

        BlocksG->addWidget(BlockCatList, 0, 1, 1, 1);

        BlockUniform = new QCheckBox(Blocks);
        BlockUniform->setObjectName(QStringLiteral("BlockUniform"));

        BlocksG->addWidget(BlockUniform, 0, 2, 1, 1);

        BlocksG->setColumnStretch(1, 90);
        BlocksG->setColumnMinimumWidth(0, 50);
        LevelToolBoxTabs->addTab(Blocks, QString());
        BGOs = new QWidget();
        BGOs->setObjectName(QStringLiteral("BGOs"));
        BGOsG = new QGridLayout(BGOs);
        BGOsG->setSpacing(0);
        BGOsG->setContentsMargins(11, 11, 11, 11);
        BGOsG->setObjectName(QStringLiteral("BGOsG"));
        BGOsG->setContentsMargins(0, 0, 0, 0);
        BGOCatLabel = new QLabel(BGOs);
        BGOCatLabel->setObjectName(QStringLiteral("BGOCatLabel"));

        BGOsG->addWidget(BGOCatLabel, 0, 0, 1, 1);

        BGOItemsList = new QListWidget(BGOs);
        QListWidgetItem *__qlistwidgetitem1 = new QListWidgetItem(BGOItemsList);
        __qlistwidgetitem1->setIcon(icon47);
        __qlistwidgetitem1->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);
        BGOItemsList->setObjectName(QStringLiteral("BGOItemsList"));
        BGOItemsList->setLineWidth(1);
        BGOItemsList->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
        BGOItemsList->setSizeAdjustPolicy(QAbstractScrollArea::AdjustToContents);
        BGOItemsList->setEditTriggers(QAbstractItemView::NoEditTriggers);
        BGOItemsList->setProperty("showDropIndicator", QVariant(false));
        BGOItemsList->setDragEnabled(false);
        BGOItemsList->setDragDropOverwriteMode(false);
        BGOItemsList->setDragDropMode(QAbstractItemView::DragDrop);
        BGOItemsList->setSelectionMode(QAbstractItemView::SingleSelection);
        BGOItemsList->setSelectionBehavior(QAbstractItemView::SelectItems);
        BGOItemsList->setIconSize(QSize(48, 48));
        BGOItemsList->setTextElideMode(Qt::ElideNone);
        BGOItemsList->setMovement(QListView::Snap);
        BGOItemsList->setResizeMode(QListView::Adjust);
        BGOItemsList->setLayoutMode(QListView::Batched);
        BGOItemsList->setSpacing(20);
        BGOItemsList->setViewMode(QListView::IconMode);
        BGOItemsList->setUniformItemSizes(false);
        BGOItemsList->setBatchSize(150);
        BGOItemsList->setWordWrap(true);
        BGOItemsList->setSortingEnabled(true);

        BGOsG->addWidget(BGOItemsList, 1, 0, 1, 3);

        BGOCatList = new QComboBox(BGOs);
        BGOCatList->setObjectName(QStringLiteral("BGOCatList"));

        BGOsG->addWidget(BGOCatList, 0, 1, 1, 1);

        BGOUniform = new QCheckBox(BGOs);
        BGOUniform->setObjectName(QStringLiteral("BGOUniform"));

        BGOsG->addWidget(BGOUniform, 0, 2, 1, 1);

        BGOsG->setColumnStretch(1, 95);
        BGOsG->setColumnMinimumWidth(0, 50);
        BGOsG->setColumnMinimumWidth(2, 15);
        LevelToolBoxTabs->addTab(BGOs, QString());
        npc = new QScrollArea();
        npc->setObjectName(QStringLiteral("npc"));
        npc->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
        npc->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        npc->setWidgetResizable(true);
        npcscroll = new QWidget();
        npcscroll->setObjectName(QStringLiteral("npcscroll"));
        npcscroll->setGeometry(QRect(0, 0, 239, 389));
        npcscroll->setStyleSheet(QStringLiteral("background-color: rgb(255, 255, 255);"));
        npc->setWidget(npcscroll);
        LevelToolBoxTabs->addTab(npc, QString());
        LevelToolBox->setWidget(LevelToolBoxTabs);
        MainWindow->addDockWidget(static_cast<Qt::DockWidgetArea>(1), LevelToolBox);
        EditionToolBar = new QToolBar(MainWindow);
        EditionToolBar->setObjectName(QStringLiteral("EditionToolBar"));
        EditionToolBar->setEnabled(true);
        EditionToolBar->setContextMenuPolicy(Qt::NoContextMenu);
        EditionToolBar->setAllowedAreas(Qt::AllToolBarAreas);
        EditionToolBar->setIconSize(QSize(24, 24));
        MainWindow->addToolBar(Qt::TopToolBarArea, EditionToolBar);
        LevelObjectToolbar = new QToolBar(MainWindow);
        LevelObjectToolbar->setObjectName(QStringLiteral("LevelObjectToolbar"));
        LevelObjectToolbar->setFloatable(true);
        MainWindow->addToolBar(Qt::TopToolBarArea, LevelObjectToolbar);
        LevelSectionsToolBar = new QToolBar(MainWindow);
        LevelSectionsToolBar->setObjectName(QStringLiteral("LevelSectionsToolBar"));
        LevelSectionsToolBar->setAcceptDrops(false);
        LevelSectionsToolBar->setAllowedAreas(Qt::AllToolBarAreas);
        MainWindow->addToolBar(Qt::TopToolBarArea, LevelSectionsToolBar);
        MainWindow->insertToolBarBreak(LevelSectionsToolBar);
        WorldToolBox = new QDockWidget(MainWindow);
        WorldToolBox->setObjectName(QStringLiteral("WorldToolBox"));
        WorldToolBox->setEnabled(true);
        sizePolicy1.setHeightForWidth(WorldToolBox->sizePolicy().hasHeightForWidth());
        WorldToolBox->setSizePolicy(sizePolicy1);
        WorldToolBox->setMinimumSize(QSize(200, 119));
        QIcon icon48;
        icon48.addFile(QStringLiteral(":/images/world16.png"), QSize(), QIcon::Normal, QIcon::Off);
        WorldToolBox->setWindowIcon(icon48);
        WorldToolBox->setStyleSheet(QStringLiteral("font: 8pt \"Liberation Sans\";"));
        WorldToolBox->setAllowedAreas(Qt::BottomDockWidgetArea|Qt::LeftDockWidgetArea|Qt::RightDockWidgetArea);
        WorldToolBoxTabs = new QTabWidget();
        WorldToolBoxTabs->setObjectName(QStringLiteral("WorldToolBoxTabs"));
        Tiles = new QScrollArea();
        Tiles->setObjectName(QStringLiteral("Tiles"));
        Tiles->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
        Tiles->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
        Tiles->setWidgetResizable(true);
        TilesItemBox = new QListView();
        TilesItemBox->setObjectName(QStringLiteral("TilesItemBox"));
        TilesItemBox->setGeometry(QRect(0, 0, 256, 481));
        TilesItemBox->setStyleSheet(QLatin1String(" Item {\n"
"   Image {\n"
"       id: pic\n"
"   }\n"
"   Text {\n"
"       id: label\n"
"       anchors.horizontalCenter: pic.horizontalCenter\n"
"       anchors.top: pic.bottom\n"
"       anchors.topMargin: 5\n"
"   }\n"
"}"));
        TilesItemBox->setDragEnabled(true);
        TilesItemBox->setResizeMode(QListView::Adjust);
        TilesItemBox->setGridSize(QSize(128, 128));
        TilesItemBox->setViewMode(QListView::IconMode);
        TilesItemBox->setModelColumn(0);
        TilesItemBox->setUniformItemSizes(true);
        TilesItemBox->setBatchSize(128);
        Tiles->setWidget(TilesItemBox);
        WorldToolBoxTabs->addTab(Tiles, QString());
        Scenery = new QScrollArea();
        Scenery->setObjectName(QStringLiteral("Scenery"));
        Scenery->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
        Scenery->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        Scenery->setWidgetResizable(true);
        SceneryScroll = new QWidget();
        SceneryScroll->setObjectName(QStringLiteral("SceneryScroll"));
        SceneryScroll->setGeometry(QRect(0, 0, 239, 481));
        Scenery->setWidget(SceneryScroll);
        WorldToolBoxTabs->addTab(Scenery, QString());
        Level = new QScrollArea();
        Level->setObjectName(QStringLiteral("Level"));
        Level->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
        Level->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        Level->setWidgetResizable(true);
        WLDLevelScroll = new QWidget();
        WLDLevelScroll->setObjectName(QStringLiteral("WLDLevelScroll"));
        WLDLevelScroll->setGeometry(QRect(0, 0, 239, 481));
        Level->setWidget(WLDLevelScroll);
        WorldToolBoxTabs->addTab(Level, QString());
        WldMusicBox = new QScrollArea();
        WldMusicBox->setObjectName(QStringLiteral("WldMusicBox"));
        WldMusicBox->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
        WldMusicBox->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        WldMusicBox->setWidgetResizable(true);
        WLDMusicScroll = new QWidget();
        WLDMusicScroll->setObjectName(QStringLiteral("WLDMusicScroll"));
        WLDMusicScroll->setGeometry(QRect(0, 0, 239, 481));
        groupBox_6 = new QGroupBox(WLDMusicScroll);
        groupBox_6->setObjectName(QStringLiteral("groupBox_6"));
        groupBox_6->setGeometry(QRect(10, 10, 141, 131));
        pushButton_2 = new QPushButton(groupBox_6);
        pushButton_2->setObjectName(QStringLiteral("pushButton_2"));
        pushButton_2->setGeometry(QRect(10, 20, 121, 23));
        pushButton_2->setCheckable(true);
        comboBox_3 = new QComboBox(groupBox_6);
        comboBox_3->setObjectName(QStringLiteral("comboBox_3"));
        comboBox_3->setGeometry(QRect(10, 70, 121, 22));
        label_12 = new QLabel(groupBox_6);
        label_12->setObjectName(QStringLiteral("label_12"));
        label_12->setGeometry(QRect(10, 50, 121, 16));
        pushButton_3 = new QPushButton(groupBox_6);
        pushButton_3->setObjectName(QStringLiteral("pushButton_3"));
        pushButton_3->setGeometry(QRect(10, 100, 121, 23));
        pushButton_3->setCheckable(true);
        WldMusicBox->setWidget(WLDMusicScroll);
        WorldToolBoxTabs->addTab(WldMusicBox, QString());
        WorldToolBox->setWidget(WorldToolBoxTabs);
        MainWindow->addDockWidget(static_cast<Qt::DockWidgetArea>(1), WorldToolBox);
        LevelSectionSettings = new QDockWidget(MainWindow);
        LevelSectionSettings->setObjectName(QStringLiteral("LevelSectionSettings"));
        QSizePolicy sizePolicy2(QSizePolicy::Minimum, QSizePolicy::Preferred);
        sizePolicy2.setHorizontalStretch(0);
        sizePolicy2.setVerticalStretch(0);
        sizePolicy2.setHeightForWidth(LevelSectionSettings->sizePolicy().hasHeightForWidth());
        LevelSectionSettings->setSizePolicy(sizePolicy2);
        LevelSectionSettings->setMinimumSize(QSize(246, 360));
        LevelSectionSettings->setMaximumSize(QSize(246, 360));
        QFont font;
        font.setFamily(QStringLiteral("Liberation Sans"));
        font.setPointSize(8);
        font.setBold(false);
        font.setItalic(false);
        font.setWeight(50);
        LevelSectionSettings->setFont(font);
        QIcon icon49;
        icon49.addFile(QStringLiteral(":/images/section16.png"), QSize(), QIcon::Normal, QIcon::Off);
        LevelSectionSettings->setWindowIcon(icon49);
        LevelSectionSettings->setStyleSheet(QStringLiteral("font: 8pt \"Liberation Sans\";"));
        LevelSectionSettings->setFloating(false);
        LevelSectionSettings->setFeatures(QDockWidget::AllDockWidgetFeatures);
        LevelSectionSettings->setAllowedAreas(Qt::LeftDockWidgetArea|Qt::RightDockWidgetArea);
        LevelSection = new QWidget();
        LevelSection->setObjectName(QStringLiteral("LevelSection"));
        LVLPropMusicGr = new QGroupBox(LevelSection);
        LVLPropMusicGr->setObjectName(QStringLiteral("LVLPropMusicGr"));
        LVLPropMusicGr->setGeometry(QRect(10, 160, 221, 91));
        LVLPropMusicGr->setMinimumSize(QSize(0, 91));
        LVLPropMusicGr->setFont(font);
        LVLPropsMusicNumber = new QComboBox(LVLPropMusicGr);
        LVLPropsMusicNumber->setObjectName(QStringLiteral("LVLPropsMusicNumber"));
        LVLPropsMusicNumber->setGeometry(QRect(10, 20, 131, 21));
        LVLPropsMusicNumber->setFont(font);
        LVLPropsMusicNumber->setFocusPolicy(Qt::WheelFocus);
        LVLPropCMusicLbl = new QLabel(LVLPropMusicGr);
        LVLPropCMusicLbl->setObjectName(QStringLiteral("LVLPropCMusicLbl"));
        LVLPropCMusicLbl->setGeometry(QRect(10, 46, 47, 16));
        LVLPropCMusicLbl->setFont(font);
        LVLPropCMusicLbl->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
        LVLPropsMusicCustom = new QLineEdit(LVLPropMusicGr);
        LVLPropsMusicCustom->setObjectName(QStringLiteral("LVLPropsMusicCustom"));
        LVLPropsMusicCustom->setGeometry(QRect(10, 60, 171, 21));
        LVLPropsMusicCustom->setFont(font);
        LVLPropsMusicCustomEn = new QCheckBox(LVLPropMusicGr);
        LVLPropsMusicCustomEn->setObjectName(QStringLiteral("LVLPropsMusicCustomEn"));
        LVLPropsMusicCustomEn->setGeometry(QRect(150, 20, 61, 21));
        LVLPropsMusicCustomEn->setFont(font);
        LVLPropsMusicCustomEn->setTristate(false);
        LVLPropsMusicCustomBrowse = new QPushButton(LVLPropMusicGr);
        LVLPropsMusicCustomBrowse->setObjectName(QStringLiteral("LVLPropsMusicCustomBrowse"));
        LVLPropsMusicCustomBrowse->setGeometry(QRect(180, 60, 31, 21));
        LVLPropsMusicCustomBrowse->setFont(font);
        LVLPropSettingsGrp = new QGroupBox(LevelSection);
        LVLPropSettingsGrp->setObjectName(QStringLiteral("LVLPropSettingsGrp"));
        LVLPropSettingsGrp->setGeometry(QRect(10, 40, 221, 121));
        LVLPropSettingsGrp->setMinimumSize(QSize(0, 121));
        LVLPropSettingsGrp->setFont(font);
        verticalLayout_3 = new QVBoxLayout(LVLPropSettingsGrp);
        verticalLayout_3->setSpacing(6);
        verticalLayout_3->setContentsMargins(11, 11, 11, 11);
        verticalLayout_3->setObjectName(QStringLiteral("verticalLayout_3"));
        LVLPropsLevelWarp = new QCheckBox(LVLPropSettingsGrp);
        LVLPropsLevelWarp->setObjectName(QStringLiteral("LVLPropsLevelWarp"));
        LVLPropsLevelWarp->setFont(font);
        LVLPropsLevelWarp->setTristate(false);

        verticalLayout_3->addWidget(LVLPropsLevelWarp);

        LVLPropsOffScr = new QCheckBox(LVLPropSettingsGrp);
        LVLPropsOffScr->setObjectName(QStringLiteral("LVLPropsOffScr"));
        LVLPropsOffScr->setFont(font);
        LVLPropsOffScr->setTristate(false);

        verticalLayout_3->addWidget(LVLPropsOffScr);

        LVLPropsNoTBack = new QCheckBox(LVLPropSettingsGrp);
        LVLPropsNoTBack->setObjectName(QStringLiteral("LVLPropsNoTBack"));
        LVLPropsNoTBack->setFont(font);
        LVLPropsNoTBack->setTristate(false);

        verticalLayout_3->addWidget(LVLPropsNoTBack);

        LVLPropsUnderWater = new QCheckBox(LVLPropSettingsGrp);
        LVLPropsUnderWater->setObjectName(QStringLiteral("LVLPropsUnderWater"));
        LVLPropsUnderWater->setFont(font);
        LVLPropsUnderWater->setTristate(false);

        verticalLayout_3->addWidget(LVLPropsUnderWater);

        LVLPropStyleGr = new QGroupBox(LevelSection);
        LVLPropStyleGr->setObjectName(QStringLiteral("LVLPropStyleGr"));
        LVLPropStyleGr->setGeometry(QRect(10, 260, 221, 70));
        LVLPropStyleGr->setMinimumSize(QSize(0, 70));
        LVLPropStyleGr->setFont(font);
        verticalLayout = new QVBoxLayout(LVLPropStyleGr);
        verticalLayout->setSpacing(6);
        verticalLayout->setContentsMargins(11, 11, 11, 11);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        LVLPropBckrImg = new QLabel(LVLPropStyleGr);
        LVLPropBckrImg->setObjectName(QStringLiteral("LVLPropBckrImg"));
        LVLPropBckrImg->setFont(font);

        verticalLayout->addWidget(LVLPropBckrImg);

        LVLPropsBackImage = new QComboBox(LVLPropStyleGr);
        LVLPropsBackImage->setObjectName(QStringLiteral("LVLPropsBackImage"));
        LVLPropsBackImage->setMinimumSize(QSize(0, 20));
        LVLPropsBackImage->setFont(font);
        LVLPropsBackImage->setFocusPolicy(Qt::WheelFocus);

        verticalLayout->addWidget(LVLPropsBackImage);

        groupBox_5 = new QGroupBox(LevelSection);
        groupBox_5->setObjectName(QStringLiteral("groupBox_5"));
        groupBox_5->setGeometry(QRect(10, 0, 91, 41));
        groupBox_5->setFont(font);
        label_9 = new QLabel(groupBox_5);
        label_9->setObjectName(QStringLiteral("label_9"));
        label_9->setGeometry(QRect(10, 20, 51, 16));
        label_9->setFont(font);
        label_9->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
        LVLProp_CurSect = new QLabel(groupBox_5);
        LVLProp_CurSect->setObjectName(QStringLiteral("LVLProp_CurSect"));
        LVLProp_CurSect->setGeometry(QRect(70, 20, 16, 16));
        LVLProp_CurSect->setFont(font);
        ResizeSection = new QPushButton(LevelSection);
        ResizeSection->setObjectName(QStringLiteral("ResizeSection"));
        ResizeSection->setGeometry(QRect(110, 10, 111, 23));
        ResizeSection->setFont(font);
        ResizeSection->setCheckable(true);
        LevelSectionSettings->setWidget(LevelSection);
        MainWindow->addDockWidget(static_cast<Qt::DockWidgetArea>(2), LevelSectionSettings);
        DoorsToolbox = new QDockWidget(MainWindow);
        DoorsToolbox->setObjectName(QStringLiteral("DoorsToolbox"));
        DoorsToolbox->setMinimumSize(QSize(246, 460));
        DoorsToolbox->setMaximumSize(QSize(246, 460));
        DoorsToolbox->setFloating(false);
        DoorsToolbox->setAllowedAreas(Qt::LeftDockWidgetArea|Qt::RightDockWidgetArea);
        dockWidgetContents = new QWidget();
        dockWidgetContents->setObjectName(QStringLiteral("dockWidgetContents"));
        groupBox = new QGroupBox(dockWidgetContents);
        groupBox->setObjectName(QStringLiteral("groupBox"));
        groupBox->setGeometry(QRect(0, 0, 241, 431));
        groupBox->setAutoFillBackground(true);
        groupBox->setStyleSheet(QStringLiteral("border-color: rgb(0, 0, 0);"));
        groupBox->setFlat(false);
        groupBox_4 = new QGroupBox(groupBox);
        groupBox_4->setObjectName(QStringLiteral("groupBox_4"));
        groupBox_4->setGeometry(QRect(10, 330, 101, 61));
        WarpLevelEntrance = new QCheckBox(groupBox_4);
        WarpLevelEntrance->setObjectName(QStringLiteral("WarpLevelEntrance"));
        WarpLevelEntrance->setGeometry(QRect(10, 40, 71, 17));
        WarpLevelExit = new QCheckBox(groupBox_4);
        WarpLevelExit->setObjectName(QStringLiteral("WarpLevelExit"));
        WarpLevelExit->setGeometry(QRect(10, 20, 71, 17));
        groupBox_7 = new QGroupBox(groupBox);
        groupBox_7->setObjectName(QStringLiteral("groupBox_7"));
        groupBox_7->setGeometry(QRect(10, 160, 221, 121));
        WarpEntranceGrp = new QGroupBox(groupBox_7);
        WarpEntranceGrp->setObjectName(QStringLiteral("WarpEntranceGrp"));
        WarpEntranceGrp->setGeometry(QRect(10, 10, 91, 101));
        WarpEntranceGrp->setAlignment(Qt::AlignCenter);
        WarpEntranceGrp->setFlat(false);
        Entr_Left = new QRadioButton(WarpEntranceGrp);
        Entr_Left->setObjectName(QStringLiteral("Entr_Left"));
        Entr_Left->setGeometry(QRect(70, 50, 16, 16));
        Entr_Left->setChecked(false);
        Entr_Right = new QRadioButton(WarpEntranceGrp);
        Entr_Right->setObjectName(QStringLiteral("Entr_Right"));
        Entr_Right->setGeometry(QRect(10, 50, 16, 16));
        Entr_Up = new QRadioButton(WarpEntranceGrp);
        Entr_Up->setObjectName(QStringLiteral("Entr_Up"));
        Entr_Up->setGeometry(QRect(40, 80, 16, 16));
        Entr_Up->setChecked(false);
        Entr_Down = new QRadioButton(WarpEntranceGrp);
        Entr_Down->setObjectName(QStringLiteral("Entr_Down"));
        Entr_Down->setGeometry(QRect(40, 20, 16, 16));
        Entr_Down->setChecked(true);
        label_2 = new QLabel(WarpEntranceGrp);
        label_2->setObjectName(QStringLiteral("label_2"));
        label_2->setGeometry(QRect(25, 38, 41, 41));
        WarpExitGrp = new QGroupBox(groupBox_7);
        WarpExitGrp->setObjectName(QStringLiteral("WarpExitGrp"));
        WarpExitGrp->setEnabled(true);
        WarpExitGrp->setGeometry(QRect(120, 10, 91, 101));
        WarpExitGrp->setAlignment(Qt::AlignCenter);
        Exit_Left = new QRadioButton(WarpExitGrp);
        Exit_Left->setObjectName(QStringLiteral("Exit_Left"));
        Exit_Left->setGeometry(QRect(10, 50, 16, 16));
        Exit_Left->setChecked(false);
        Exit_Right = new QRadioButton(WarpExitGrp);
        Exit_Right->setObjectName(QStringLiteral("Exit_Right"));
        Exit_Right->setGeometry(QRect(70, 50, 16, 16));
        Exit_Up = new QRadioButton(WarpExitGrp);
        Exit_Up->setObjectName(QStringLiteral("Exit_Up"));
        Exit_Up->setGeometry(QRect(40, 20, 16, 16));
        Exit_Up->setChecked(true);
        Exit_Down = new QRadioButton(WarpExitGrp);
        Exit_Down->setObjectName(QStringLiteral("Exit_Down"));
        Exit_Down->setGeometry(QRect(40, 80, 16, 16));
        label_7 = new QLabel(WarpExitGrp);
        label_7->setObjectName(QStringLiteral("label_7"));
        label_7->setGeometry(QRect(20, 37, 51, 41));
        groupBox_8 = new QGroupBox(groupBox);
        groupBox_8->setObjectName(QStringLiteral("groupBox_8"));
        groupBox_8->setGeometry(QRect(10, 280, 221, 51));
        WarpGetXYFromWorldMap = new QPushButton(groupBox_8);
        WarpGetXYFromWorldMap->setObjectName(QStringLiteral("WarpGetXYFromWorldMap"));
        WarpGetXYFromWorldMap->setGeometry(QRect(150, 20, 41, 21));
        label_6 = new QLabel(groupBox_8);
        label_6->setObjectName(QStringLiteral("label_6"));
        label_6->setGeometry(QRect(90, 20, 16, 16));
        WarpToMapX = new QLineEdit(groupBox_8);
        WarpToMapX->setObjectName(QStringLiteral("WarpToMapX"));
        WarpToMapX->setGeometry(QRect(30, 20, 51, 20));
        label_5 = new QLabel(groupBox_8);
        label_5->setObjectName(QStringLiteral("label_5"));
        label_5->setGeometry(QRect(10, 20, 16, 16));
        WarpToMapY = new QLineEdit(groupBox_8);
        WarpToMapY->setObjectName(QStringLiteral("WarpToMapY"));
        WarpToMapY->setGeometry(QRect(100, 20, 51, 20));
        groupBox_9 = new QGroupBox(groupBox);
        groupBox_9->setObjectName(QStringLiteral("groupBox_9"));
        groupBox_9->setGeometry(QRect(120, 330, 111, 91));
        label_8 = new QLabel(groupBox_9);
        label_8->setObjectName(QStringLiteral("label_8"));
        label_8->setGeometry(QRect(10, 60, 51, 21));
        WarpToExitNu = new QSpinBox(groupBox_9);
        WarpToExitNu->setObjectName(QStringLiteral("WarpToExitNu"));
        WarpToExitNu->setGeometry(QRect(60, 60, 41, 22));
        WarpLevelFile = new QLineEdit(groupBox_9);
        WarpLevelFile->setObjectName(QStringLiteral("WarpLevelFile"));
        WarpLevelFile->setGeometry(QRect(10, 40, 71, 20));
        label_11 = new QLabel(groupBox_9);
        label_11->setObjectName(QStringLiteral("label_11"));
        label_11->setGeometry(QRect(10, 20, 91, 16));
        WarpBrowseLevels = new QPushButton(groupBox_9);
        WarpBrowseLevels->setObjectName(QStringLiteral("WarpBrowseLevels"));
        WarpBrowseLevels->setGeometry(QRect(80, 40, 21, 20));
        groupBox_10 = new QGroupBox(groupBox);
        groupBox_10->setObjectName(QStringLiteral("groupBox_10"));
        groupBox_10->setGeometry(QRect(10, 40, 221, 121));
        WarpType = new QComboBox(groupBox_10);
        WarpType->setObjectName(QStringLiteral("WarpType"));
        WarpType->setGeometry(QRect(10, 90, 101, 22));
        label_3 = new QLabel(groupBox_10);
        label_3->setObjectName(QStringLiteral("label_3"));
        label_3->setGeometry(QRect(10, 70, 101, 16));
        WarpNoYoshi = new QCheckBox(groupBox_10);
        WarpNoYoshi->setObjectName(QStringLiteral("WarpNoYoshi"));
        WarpNoYoshi->setGeometry(QRect(120, 10, 81, 17));
        label_4 = new QLabel(groupBox_10);
        label_4->setObjectName(QStringLiteral("label_4"));
        label_4->setGeometry(QRect(130, 70, 61, 16));
        WarpNeedAStars = new QSpinBox(groupBox_10);
        WarpNeedAStars->setObjectName(QStringLiteral("WarpNeedAStars"));
        WarpNeedAStars->setGeometry(QRect(130, 90, 61, 22));
        WarpLock = new QCheckBox(groupBox_10);
        WarpLock->setObjectName(QStringLiteral("WarpLock"));
        WarpLock->setGeometry(QRect(120, 50, 81, 17));
        WarpAllowNPC = new QCheckBox(groupBox_10);
        WarpAllowNPC->setObjectName(QStringLiteral("WarpAllowNPC"));
        WarpAllowNPC->setGeometry(QRect(120, 30, 81, 17));
        WarpSetEntrance = new QPushButton(groupBox_10);
        WarpSetEntrance->setObjectName(QStringLiteral("WarpSetEntrance"));
        WarpSetEntrance->setGeometry(QRect(30, 20, 81, 21));
        WarpSetExit = new QPushButton(groupBox_10);
        WarpSetExit->setObjectName(QStringLiteral("WarpSetExit"));
        WarpSetExit->setGeometry(QRect(30, 40, 81, 21));
        WarpEntrancePlaced = new QCheckBox(groupBox_10);
        WarpEntrancePlaced->setObjectName(QStringLiteral("WarpEntrancePlaced"));
        WarpEntrancePlaced->setEnabled(false);
        WarpEntrancePlaced->setGeometry(QRect(10, 20, 20, 20));
        WarpExitPlaced = new QCheckBox(groupBox_10);
        WarpExitPlaced->setObjectName(QStringLiteral("WarpExitPlaced"));
        WarpExitPlaced->setEnabled(false);
        WarpExitPlaced->setGeometry(QRect(10, 40, 20, 20));
        WarpList = new QComboBox(groupBox);
        WarpList->setObjectName(QStringLiteral("WarpList"));
        WarpList->setGeometry(QRect(10, 20, 161, 21));
        WarpAdd = new QPushButton(groupBox);
        WarpAdd->setObjectName(QStringLiteral("WarpAdd"));
        WarpAdd->setGeometry(QRect(170, 20, 31, 21));
        WarpRemove = new QPushButton(groupBox);
        WarpRemove->setObjectName(QStringLiteral("WarpRemove"));
        WarpRemove->setGeometry(QRect(200, 20, 31, 21));
        DoorsToolbox->setWidget(dockWidgetContents);
        MainWindow->addDockWidget(static_cast<Qt::DockWidgetArea>(2), DoorsToolbox);
        LevelLayers = new QDockWidget(MainWindow);
        LevelLayers->setObjectName(QStringLiteral("LevelLayers"));
        LevelLayers->setMinimumSize(QSize(246, 200));
        LevelLayers->setAllowedAreas(Qt::LeftDockWidgetArea|Qt::RightDockWidgetArea);
        LevelLayersBox = new QWidget();
        LevelLayersBox->setObjectName(QStringLiteral("LevelLayersBox"));
        gridLayout = new QGridLayout(LevelLayersBox);
        gridLayout->setSpacing(0);
        gridLayout->setContentsMargins(11, 11, 11, 11);
        gridLayout->setObjectName(QStringLiteral("gridLayout"));
        gridLayout->setContentsMargins(0, 0, 0, 0);
        RemoveLayer = new QPushButton(LevelLayersBox);
        RemoveLayer->setObjectName(QStringLiteral("RemoveLayer"));
        RemoveLayer->setEnabled(false);

        gridLayout->addWidget(RemoveLayer, 1, 1, 1, 1);

        AddLayer = new QPushButton(LevelLayersBox);
        AddLayer->setObjectName(QStringLiteral("AddLayer"));
        AddLayer->setEnabled(false);

        gridLayout->addWidget(AddLayer, 1, 0, 1, 1);

        LockLayer = new QPushButton(LevelLayersBox);
        LockLayer->setObjectName(QStringLiteral("LockLayer"));
        LockLayer->setEnabled(false);
        LockLayer->setCheckable(true);

        gridLayout->addWidget(LockLayer, 1, 2, 1, 1);

        LvlLayerList = new QListWidget(LevelLayersBox);
        QListWidgetItem *__qlistwidgetitem2 = new QListWidgetItem(LvlLayerList);
        __qlistwidgetitem2->setCheckState(Qt::Checked);
        QListWidgetItem *__qlistwidgetitem3 = new QListWidgetItem(LvlLayerList);
        __qlistwidgetitem3->setCheckState(Qt::Unchecked);
        __qlistwidgetitem3->setFlags(Qt::ItemIsDragEnabled|Qt::ItemIsUserCheckable);
        QListWidgetItem *__qlistwidgetitem4 = new QListWidgetItem(LvlLayerList);
        __qlistwidgetitem4->setCheckState(Qt::Checked);
        __qlistwidgetitem4->setFlags(Qt::ItemIsDragEnabled|Qt::ItemIsUserCheckable);
        LvlLayerList->setObjectName(QStringLiteral("LvlLayerList"));
        LvlLayerList->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
        LvlLayerList->setDragDropMode(QAbstractItemView::InternalMove);
        LvlLayerList->setAlternatingRowColors(true);
        LvlLayerList->setSelectionBehavior(QAbstractItemView::SelectItems);
        LvlLayerList->setViewMode(QListView::ListMode);
        LvlLayerList->setUniformItemSizes(false);

        gridLayout->addWidget(LvlLayerList, 0, 0, 1, 3);

        LevelLayers->setWidget(LevelLayersBox);
        MainWindow->addDockWidget(static_cast<Qt::DockWidgetArea>(2), LevelLayers);

        menuBar->addAction(menu->menuAction());
        menuBar->addAction(menuEdit->menuAction());
        menuBar->addAction(menuLevel->menuAction());
        menuBar->addAction(menuWorld->menuAction());
        menuBar->addAction(menuView->menuAction());
        menuBar->addAction(menuTools->menuAction());
        menuBar->addAction(menuWindow->menuAction());
        menuBar->addAction(menuHelp->menuAction());
        menu->addAction(menuNew->menuAction());
        menu->addAction(OpenFile);
        menu->addAction(menuOpenRecent->menuAction());
        menu->addAction(actionClose);
        menu->addSeparator();
        menu->addAction(actionSave);
        menu->addAction(actionSave_as);
        menu->addAction(actionSave_all);
        menu->addSeparator();
        menu->addAction(actionExport_to_image);
        menu->addSeparator();
        menu->addAction(Exit);
        menuNew->addAction(actionNewLevel);
        menuNew->addAction(actionNewWorld_map);
        menuNew->addAction(actionNPC_config);
        menuOpenRecent->addAction(action_recent1);
        menuOpenRecent->addAction(action_recent2);
        menuOpenRecent->addAction(action_recent3);
        menuOpenRecent->addAction(action_recent4);
        menuOpenRecent->addAction(action_recent5);
        menuOpenRecent->addAction(action_recent6);
        menuOpenRecent->addAction(action_recent7);
        menuOpenRecent->addAction(action_recent8);
        menuOpenRecent->addAction(action_recent9);
        menuOpenRecent->addAction(action_recent10);
        menuHelp->addAction(actionContents);
        menuHelp->addAction(actionAbout);
        menuLevel->addAction(menuCurrent_section->menuAction());
        menuLevel->addSeparator();
        menuLevel->addAction(actionLevWarp);
        menuLevel->addAction(actionLevOffScr);
        menuLevel->addAction(actionLevNoBack);
        menuLevel->addAction(actionLevUnderW);
        menuLevel->addSeparator();
        menuLevel->addAction(actionLevelProp);
        menuCurrent_section->addAction(actionSection_1);
        menuCurrent_section->addAction(actionSection_2);
        menuCurrent_section->addAction(actionSection_3);
        menuCurrent_section->addAction(actionSection_4);
        menuCurrent_section->addAction(actionSection_5);
        menuCurrent_section->addAction(actionSection_6);
        menuCurrent_section->addAction(actionSection_7);
        menuCurrent_section->addAction(actionSection_8);
        menuCurrent_section->addAction(actionSection_9);
        menuCurrent_section->addAction(actionSection_10);
        menuCurrent_section->addAction(actionSection_11);
        menuCurrent_section->addAction(actionSection_12);
        menuCurrent_section->addAction(actionSection_13);
        menuCurrent_section->addAction(actionSection_14);
        menuCurrent_section->addAction(actionSection_15);
        menuCurrent_section->addAction(actionSection_16);
        menuCurrent_section->addAction(actionSection_17);
        menuCurrent_section->addAction(actionSection_18);
        menuCurrent_section->addAction(actionSection_19);
        menuCurrent_section->addAction(actionSection_20);
        menuCurrent_section->addAction(actionSection_21);
        menuWorld->addAction(actionWLDDisableMap);
        menuWorld->addAction(actionWLDFailRestart);
        menuWorld->addAction(menuDisable_characters->menuAction());
        menuWorld->addSeparator();
        menuWorld->addAction(actionWLDProperties);
        menuDisable_characters->addAction(actionWLDNoChar1);
        menuDisable_characters->addAction(actionWLDNoChar2);
        menuDisable_characters->addAction(actionWLDNoChar3);
        menuDisable_characters->addAction(actionWLDNoChar4);
        menuDisable_characters->addAction(actionWLDNoChar5);
        menuView->addAction(actionLVLToolBox);
        menuView->addAction(actionSection_Settings);
        menuView->addAction(actionWarpsAndDoors);
        menuView->addAction(actionLayersBox);
        menuView->addSeparator();
        menuView->addAction(actionWLDToolBox);
        menuView->addSeparator();
        menuView->addAction(actionAnimation);
        menuView->addAction(actionCollisions);
        menuWindow->addAction(actionEmpty);
        menuTools->addAction(actionLoad_configs);
        menuTools->addAction(actionReload);
        menuEdit->addAction(actionUndo);
        menuEdit->addAction(actionRedo);
        menuEdit->addSeparator();
        menuEdit->addAction(actionCopy);
        menuEdit->addAction(actionPaste);
        mainToolBar->addAction(OpenFile);
        mainToolBar->addAction(actionSave);
        mainToolBar->addAction(actionSave_as);
        EditionToolBar->addAction(actionHandScroll);
        EditionToolBar->addAction(actionSelect);
        EditionToolBar->addAction(actionEriser);
        EditionToolBar->addSeparator();
        EditionToolBar->addAction(actionPlayMusic);
        EditionToolBar->addSeparator();
        EditionToolBar->addAction(actionReset_position);
        EditionToolBar->addAction(actionGridEn);
        EditionToolBar->addSeparator();
        EditionToolBar->addAction(actionReload);
        EditionToolBar->addAction(actionWLDToolBox);
        LevelObjectToolbar->addAction(actionSetFirstPlayer);
        LevelObjectToolbar->addAction(actionSetSecondPlayer);
        LevelObjectToolbar->addAction(actionDrawWater);
        LevelObjectToolbar->addAction(actionDrawSand);
        LevelObjectToolbar->addSeparator();
        LevelObjectToolbar->addAction(actionLockBlocks);
        LevelObjectToolbar->addAction(actionLockBGO);
        LevelObjectToolbar->addAction(actionLockNPC);
        LevelObjectToolbar->addAction(actionLockDoors);
        LevelObjectToolbar->addAction(actionLockWaters);
        LevelObjectToolbar->addSeparator();
        LevelObjectToolbar->addAction(actionLVLToolBox);
        LevelObjectToolbar->addAction(actionSection_Settings);
        LevelObjectToolbar->addAction(actionWarpsAndDoors);
        LevelObjectToolbar->addAction(actionLayersBox);
        LevelSectionsToolBar->addAction(actionSection_1);
        LevelSectionsToolBar->addAction(actionSection_2);
        LevelSectionsToolBar->addAction(actionSection_3);
        LevelSectionsToolBar->addAction(actionSection_4);
        LevelSectionsToolBar->addAction(actionSection_5);
        LevelSectionsToolBar->addAction(actionSection_6);
        LevelSectionsToolBar->addAction(actionSection_7);
        LevelSectionsToolBar->addAction(actionSection_8);
        LevelSectionsToolBar->addAction(actionSection_9);
        LevelSectionsToolBar->addAction(actionSection_10);
        LevelSectionsToolBar->addAction(actionSection_11);
        LevelSectionsToolBar->addAction(actionSection_12);
        LevelSectionsToolBar->addAction(actionSection_13);
        LevelSectionsToolBar->addAction(actionSection_14);
        LevelSectionsToolBar->addAction(actionSection_15);
        LevelSectionsToolBar->addAction(actionSection_16);
        LevelSectionsToolBar->addAction(actionSection_17);
        LevelSectionsToolBar->addAction(actionSection_18);
        LevelSectionsToolBar->addAction(actionSection_19);
        LevelSectionsToolBar->addAction(actionSection_20);
        LevelSectionsToolBar->addAction(actionSection_21);

        retranslateUi(MainWindow);

        LevelToolBoxTabs->setCurrentIndex(0);
        WorldToolBoxTabs->setCurrentIndex(0);
        LvlLayerList->setCurrentRow(-1);


        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QApplication::translate("MainWindow", "Editor - PlatGEnWohl (Under Construction)", 0));
        OpenFile->setText(QApplication::translate("MainWindow", "Open...", 0));
#ifndef QT_NO_TOOLTIP
        OpenFile->setToolTip(QApplication::translate("MainWindow", "Open (Ctrl+O)", 0));
#endif // QT_NO_TOOLTIP
        OpenFile->setShortcut(QApplication::translate("MainWindow", "Ctrl+O", 0));
        Exit->setText(QApplication::translate("MainWindow", "Exit", 0));
        Exit->setShortcut(QApplication::translate("MainWindow", "Ctrl+Alt+Q", 0));
        actionNewLevel->setText(QApplication::translate("MainWindow", "Level...", 0));
        actionNewWorld_map->setText(QApplication::translate("MainWindow", "World map...", 0));
        actionNPC_config->setText(QApplication::translate("MainWindow", "NPC config...", 0));
        actionSave->setText(QApplication::translate("MainWindow", "Save", 0));
#ifndef QT_NO_TOOLTIP
        actionSave->setToolTip(QApplication::translate("MainWindow", "Save (Ctrl+S)", 0));
#endif // QT_NO_TOOLTIP
        actionSave->setShortcut(QApplication::translate("MainWindow", "Ctrl+S", 0));
        actionSave_as->setText(QApplication::translate("MainWindow", "Save as...", 0));
#ifndef QT_NO_TOOLTIP
        actionSave_as->setToolTip(QApplication::translate("MainWindow", "Save as (Ctrl+Shift+S)", 0));
#endif // QT_NO_TOOLTIP
        actionSave_as->setShortcut(QApplication::translate("MainWindow", "Ctrl+Shift+S", 0));
        actionClose->setText(QApplication::translate("MainWindow", "Close", 0));
        actionSave_all->setText(QApplication::translate("MainWindow", "Save all", 0));
#ifndef QT_NO_TOOLTIP
        actionSave_all->setToolTip(QApplication::translate("MainWindow", "Save all (Ctrl+Alt+S)", 0));
#endif // QT_NO_TOOLTIP
        actionSave_all->setShortcut(QApplication::translate("MainWindow", "Ctrl+Alt+S", 0));
        actionAbout->setText(QApplication::translate("MainWindow", "About", 0));
        actionContents->setText(QApplication::translate("MainWindow", "Contents", 0));
#ifndef QT_NO_TOOLTIP
        actionContents->setToolTip(QApplication::translate("MainWindow", "Contents (F1)", 0));
#endif // QT_NO_TOOLTIP
        actionContents->setShortcut(QApplication::translate("MainWindow", "F1", 0));
        actionSection_1->setText(QApplication::translate("MainWindow", "Section 1", 0));
#ifndef QT_NO_TOOLTIP
        actionSection_1->setToolTip(QApplication::translate("MainWindow", "Section 1 (Ctrl+1)", 0));
#endif // QT_NO_TOOLTIP
        actionSection_1->setShortcut(QApplication::translate("MainWindow", "Ctrl+1", 0));
        actionSection_2->setText(QApplication::translate("MainWindow", "Section 2", 0));
#ifndef QT_NO_TOOLTIP
        actionSection_2->setToolTip(QApplication::translate("MainWindow", "Section 2 (Ctrl+2)", 0));
#endif // QT_NO_TOOLTIP
        actionSection_2->setShortcut(QApplication::translate("MainWindow", "Ctrl+2", 0));
        actionSection_3->setText(QApplication::translate("MainWindow", "Section 3", 0));
#ifndef QT_NO_TOOLTIP
        actionSection_3->setToolTip(QApplication::translate("MainWindow", "Section 3 (Ctrl+3)", 0));
#endif // QT_NO_TOOLTIP
        actionSection_3->setShortcut(QApplication::translate("MainWindow", "Ctrl+3", 0));
        actionSection_4->setText(QApplication::translate("MainWindow", "Section 4", 0));
#ifndef QT_NO_TOOLTIP
        actionSection_4->setToolTip(QApplication::translate("MainWindow", "Section 4 (Ctrl+4)", 0));
#endif // QT_NO_TOOLTIP
        actionSection_4->setShortcut(QApplication::translate("MainWindow", "Ctrl+4", 0));
        actionSection_5->setText(QApplication::translate("MainWindow", "Section 5", 0));
#ifndef QT_NO_TOOLTIP
        actionSection_5->setToolTip(QApplication::translate("MainWindow", "Section 5 (Ctrl+5)", 0));
#endif // QT_NO_TOOLTIP
        actionSection_5->setShortcut(QApplication::translate("MainWindow", "Ctrl+5", 0));
        actionSection_6->setText(QApplication::translate("MainWindow", "Section 6", 0));
#ifndef QT_NO_TOOLTIP
        actionSection_6->setToolTip(QApplication::translate("MainWindow", "Section 6 (Ctrl+6)", 0));
#endif // QT_NO_TOOLTIP
        actionSection_6->setShortcut(QApplication::translate("MainWindow", "Ctrl+6", 0));
        actionSection_7->setText(QApplication::translate("MainWindow", "Section 7", 0));
#ifndef QT_NO_TOOLTIP
        actionSection_7->setToolTip(QApplication::translate("MainWindow", "Section 7 (Ctrl+7)", 0));
#endif // QT_NO_TOOLTIP
        actionSection_7->setShortcut(QApplication::translate("MainWindow", "Ctrl+7", 0));
        actionSection_8->setText(QApplication::translate("MainWindow", "Section 8", 0));
#ifndef QT_NO_TOOLTIP
        actionSection_8->setToolTip(QApplication::translate("MainWindow", "Section 8 (Ctrl+8)", 0));
#endif // QT_NO_TOOLTIP
        actionSection_8->setShortcut(QApplication::translate("MainWindow", "Ctrl+8", 0));
        actionSection_9->setText(QApplication::translate("MainWindow", "Section 9", 0));
#ifndef QT_NO_TOOLTIP
        actionSection_9->setToolTip(QApplication::translate("MainWindow", "Section 9 (Ctrl+9)", 0));
#endif // QT_NO_TOOLTIP
        actionSection_9->setShortcut(QApplication::translate("MainWindow", "Ctrl+9", 0));
        actionSection_10->setText(QApplication::translate("MainWindow", "Section 10", 0));
#ifndef QT_NO_TOOLTIP
        actionSection_10->setToolTip(QApplication::translate("MainWindow", "Section 10 (Ctrl+0)", 0));
#endif // QT_NO_TOOLTIP
        actionSection_10->setShortcut(QApplication::translate("MainWindow", "Ctrl+0", 0));
        actionSection_11->setText(QApplication::translate("MainWindow", "Section 11", 0));
#ifndef QT_NO_TOOLTIP
        actionSection_11->setToolTip(QApplication::translate("MainWindow", "Section 11 (Ctrl+Alt+1)", 0));
#endif // QT_NO_TOOLTIP
        actionSection_11->setShortcut(QApplication::translate("MainWindow", "Ctrl+Alt+1", 0));
        actionSection_12->setText(QApplication::translate("MainWindow", "Section 12", 0));
#ifndef QT_NO_TOOLTIP
        actionSection_12->setToolTip(QApplication::translate("MainWindow", "Section 12 (Ctrl+Alt+2)", 0));
#endif // QT_NO_TOOLTIP
        actionSection_12->setShortcut(QApplication::translate("MainWindow", "Ctrl+Alt+2", 0));
        actionSection_13->setText(QApplication::translate("MainWindow", "Section 13", 0));
#ifndef QT_NO_TOOLTIP
        actionSection_13->setToolTip(QApplication::translate("MainWindow", "Section 13 (Ctrl+Alt+3)", 0));
#endif // QT_NO_TOOLTIP
        actionSection_13->setShortcut(QApplication::translate("MainWindow", "Ctrl+Alt+3", 0));
        actionSection_14->setText(QApplication::translate("MainWindow", "Section 14", 0));
#ifndef QT_NO_TOOLTIP
        actionSection_14->setToolTip(QApplication::translate("MainWindow", "Section 14 (Ctrl+Alt+4)", 0));
#endif // QT_NO_TOOLTIP
        actionSection_14->setShortcut(QApplication::translate("MainWindow", "Ctrl+Alt+4", 0));
        actionSection_15->setText(QApplication::translate("MainWindow", "Section 15", 0));
#ifndef QT_NO_TOOLTIP
        actionSection_15->setToolTip(QApplication::translate("MainWindow", "Section 15 (Ctrl+Alt+5)", 0));
#endif // QT_NO_TOOLTIP
        actionSection_15->setShortcut(QApplication::translate("MainWindow", "Ctrl+Alt+5", 0));
        actionSection_16->setText(QApplication::translate("MainWindow", "Section 16", 0));
#ifndef QT_NO_TOOLTIP
        actionSection_16->setToolTip(QApplication::translate("MainWindow", "Section 16 (Ctrl+Alt+6)", 0));
#endif // QT_NO_TOOLTIP
        actionSection_16->setShortcut(QApplication::translate("MainWindow", "Ctrl+Alt+6", 0));
        actionSection_17->setText(QApplication::translate("MainWindow", "Section 17", 0));
#ifndef QT_NO_TOOLTIP
        actionSection_17->setToolTip(QApplication::translate("MainWindow", "Section 17 (Ctrl+Alt+7)", 0));
#endif // QT_NO_TOOLTIP
        actionSection_17->setShortcut(QApplication::translate("MainWindow", "Ctrl+Alt+7", 0));
        actionSection_18->setText(QApplication::translate("MainWindow", "Section 18", 0));
#ifndef QT_NO_TOOLTIP
        actionSection_18->setToolTip(QApplication::translate("MainWindow", "Section 18 (Ctrl+Alt+8)", 0));
#endif // QT_NO_TOOLTIP
        actionSection_18->setShortcut(QApplication::translate("MainWindow", "Ctrl+Alt+8", 0));
        actionSection_19->setText(QApplication::translate("MainWindow", "Section 19", 0));
#ifndef QT_NO_TOOLTIP
        actionSection_19->setToolTip(QApplication::translate("MainWindow", "Section 19 (Ctrl+Alt+9)", 0));
#endif // QT_NO_TOOLTIP
        actionSection_19->setShortcut(QApplication::translate("MainWindow", "Ctrl+Alt+9", 0));
        actionSection_20->setText(QApplication::translate("MainWindow", "Section 20", 0));
#ifndef QT_NO_TOOLTIP
        actionSection_20->setToolTip(QApplication::translate("MainWindow", "Section 20 (Ctrl+Alt+0)", 0));
#endif // QT_NO_TOOLTIP
        actionSection_20->setShortcut(QApplication::translate("MainWindow", "Ctrl+Alt+0", 0));
        actionSection_21->setText(QApplication::translate("MainWindow", "Section 21", 0));
#ifndef QT_NO_TOOLTIP
        actionSection_21->setToolTip(QApplication::translate("MainWindow", "Section 21 (Alt+1)", 0));
#endif // QT_NO_TOOLTIP
        actionSection_21->setShortcut(QApplication::translate("MainWindow", "Alt+1", 0));
        actionLevWarp->setText(QApplication::translate("MainWindow", "Warp section", 0));
        actionLevOffScr->setText(QApplication::translate("MainWindow", "Off screen exit", 0));
        actionLevNoBack->setText(QApplication::translate("MainWindow", "No turn back", 0));
        actionLevUnderW->setText(QApplication::translate("MainWindow", "Underwater", 0));
        actionExport_to_image->setText(QApplication::translate("MainWindow", "Export to image...", 0));
#ifndef QT_NO_TOOLTIP
        actionExport_to_image->setToolTip(QApplication::translate("MainWindow", "Export current section to image (F12)", 0));
#endif // QT_NO_TOOLTIP
        actionExport_to_image->setShortcut(QApplication::translate("MainWindow", "F12", 0));
        actionLevelProp->setText(QApplication::translate("MainWindow", "Properties...", 0));
        actionWLDDisableMap->setText(QApplication::translate("MainWindow", "Disable world map", 0));
        actionWLDFailRestart->setText(QApplication::translate("MainWindow", "Restart level after player's fail", 0));
        actionWLDNoChar1->setText(QApplication::translate("MainWindow", "Character 1", 0));
        actionWLDNoChar2->setText(QApplication::translate("MainWindow", "Character 2", 0));
        actionWLDNoChar3->setText(QApplication::translate("MainWindow", "Character 3", 0));
        actionWLDNoChar4->setText(QApplication::translate("MainWindow", "Character 4", 0));
        actionWLDNoChar5->setText(QApplication::translate("MainWindow", "Character 5", 0));
        actionWLDProperties->setText(QApplication::translate("MainWindow", "Properties...", 0));
        actionLVLToolBox->setText(QApplication::translate("MainWindow", "Level tool box", 0));
#ifndef QT_NO_TOOLTIP
        actionLVLToolBox->setToolTip(QApplication::translate("MainWindow", "Level objects tool box", 0));
#endif // QT_NO_TOOLTIP
        actionWLDToolBox->setText(QApplication::translate("MainWindow", "World tool box", 0));
        actionSelect->setText(QApplication::translate("MainWindow", "Select", 0));
#ifndef QT_NO_TOOLTIP
        actionSelect->setToolTip(QApplication::translate("MainWindow", "Select (S)", 0));
#endif // QT_NO_TOOLTIP
        actionSelect->setShortcut(QApplication::translate("MainWindow", "S", 0));
        actionEriser->setText(QApplication::translate("MainWindow", "Eriser", 0));
#ifndef QT_NO_TOOLTIP
        actionEriser->setToolTip(QApplication::translate("MainWindow", "Eriser (E)", 0));
#endif // QT_NO_TOOLTIP
        actionEriser->setShortcut(QApplication::translate("MainWindow", "E", 0));
        actionSection_Settings->setText(QApplication::translate("MainWindow", "Level Section Settings", 0));
        actionLoad_configs->setText(QApplication::translate("MainWindow", "Reload configurations", 0));
        actionReset_position->setText(QApplication::translate("MainWindow", "Reset position", 0));
        actionGridEn->setText(QApplication::translate("MainWindow", "Attach to grid", 0));
        actionLockBlocks->setText(QApplication::translate("MainWindow", "Lock all Blocks", 0));
        actionLockBGO->setText(QApplication::translate("MainWindow", "Lock all BGO", 0));
        actionLockNPC->setText(QApplication::translate("MainWindow", "Lock all NPC", 0));
        actionLockDoors->setText(QApplication::translate("MainWindow", "Lock all door objects", 0));
        actionLockWaters->setText(QApplication::translate("MainWindow", "Lock all water squares", 0));
        actionSetFirstPlayer->setText(QApplication::translate("MainWindow", "Set first player position", 0));
#ifndef QT_NO_TOOLTIP
        actionSetFirstPlayer->setToolTip(QApplication::translate("MainWindow", "Set first player start point", 0));
#endif // QT_NO_TOOLTIP
        actionSetSecondPlayer->setText(QApplication::translate("MainWindow", "Set second player position", 0));
#ifndef QT_NO_TOOLTIP
        actionSetSecondPlayer->setToolTip(QApplication::translate("MainWindow", "Set second player start point", 0));
#endif // QT_NO_TOOLTIP
        actionPlayMusic->setText(QApplication::translate("MainWindow", "Play music", 0));
#ifndef QT_NO_TOOLTIP
        actionPlayMusic->setToolTip(QApplication::translate("MainWindow", "Play music (F11)", 0));
#endif // QT_NO_TOOLTIP
        actionPlayMusic->setShortcut(QApplication::translate("MainWindow", "F11", 0));
        actionReload->setText(QApplication::translate("MainWindow", "Reload file data", 0));
#ifndef QT_NO_TOOLTIP
        actionReload->setToolTip(QApplication::translate("MainWindow", "Reload current file data", 0));
#endif // QT_NO_TOOLTIP
        actionReload->setShortcut(QApplication::translate("MainWindow", "F5", 0));
        actionHandScroll->setText(QApplication::translate("MainWindow", "Scroll hand", 0));
#ifndef QT_NO_TOOLTIP
        actionHandScroll->setToolTip(QApplication::translate("MainWindow", "Scrolling (D)", 0));
#endif // QT_NO_TOOLTIP
        actionHandScroll->setShortcut(QApplication::translate("MainWindow", "D", 0));
        actionUndo->setText(QApplication::translate("MainWindow", "Undo", 0));
        actionRedo->setText(QApplication::translate("MainWindow", "Redo", 0));
        actionWarpsAndDoors->setText(QApplication::translate("MainWindow", "Warps and doors", 0));
        actionAnimation->setText(QApplication::translate("MainWindow", "Animation", 0));
#ifndef QT_NO_TOOLTIP
        actionAnimation->setToolTip(QApplication::translate("MainWindow", "<html><head/><body><p>Enable animation on animated objects</p><p><span style=\" font-style:italic; color:#aa0000;\">If map have too many objects, recommends to </span><span style=\" font-weight:600; font-style:italic; color:#aa0000;\">diable</span><span style=\" font-style:italic; color:#aa0000;\"> this option</span></p></body></html>", 0));
#endif // QT_NO_TOOLTIP
        actionAnimation->setShortcut(QApplication::translate("MainWindow", "F10", 0));
        action_recent1->setText(QApplication::translate("MainWindow", "<empty>", 0));
        action_recent2->setText(QApplication::translate("MainWindow", "<empty>", 0));
        action_recent3->setText(QApplication::translate("MainWindow", "<empty>", 0));
        action_recent4->setText(QApplication::translate("MainWindow", "<empty>", 0));
        action_recent5->setText(QApplication::translate("MainWindow", "<empty>", 0));
        action_recent6->setText(QApplication::translate("MainWindow", "<empty>", 0));
        action_recent7->setText(QApplication::translate("MainWindow", "<empty>", 0));
        action_recent8->setText(QApplication::translate("MainWindow", "<empty>", 0));
        action_recent9->setText(QApplication::translate("MainWindow", "<empty>", 0));
        action_recent10->setText(QApplication::translate("MainWindow", "<empty>", 0));
        actionCopy->setText(QApplication::translate("MainWindow", "Copy", 0));
#ifndef QT_NO_TOOLTIP
        actionCopy->setToolTip(QApplication::translate("MainWindow", "Copy selected items", 0));
#endif // QT_NO_TOOLTIP
        actionCopy->setShortcut(QApplication::translate("MainWindow", "Ctrl+C", 0));
        actionCollisions->setText(QApplication::translate("MainWindow", "Collisions", 0));
#ifndef QT_NO_TOOLTIP
        actionCollisions->setToolTip(QApplication::translate("MainWindow", "<html><head/><body><p>Placing item to simular item Protection</p><p>(If enabeld, the movement operation will be slower)</p></body></html>", 0));
#endif // QT_NO_TOOLTIP
        actionCollisions->setShortcut(QApplication::translate("MainWindow", "F9", 0));
        actionDrawWater->setText(QApplication::translate("MainWindow", "Draw Water zone", 0));
#ifndef QT_NO_TOOLTIP
        actionDrawWater->setToolTip(QApplication::translate("MainWindow", "Hold mouse button on map and move mouse for draw water zone", 0));
#endif // QT_NO_TOOLTIP
        actionDrawSand->setText(QApplication::translate("MainWindow", "Draw QuickSand zone", 0));
#ifndef QT_NO_TOOLTIP
        actionDrawSand->setToolTip(QApplication::translate("MainWindow", "Hold mouse button on map and move mouse for draw quick sand zone", 0));
#endif // QT_NO_TOOLTIP
        actionPaste->setText(QApplication::translate("MainWindow", "Paste", 0));
        actionPaste->setShortcut(QApplication::translate("MainWindow", "Ctrl+V", 0));
        actionLayersBox->setText(QApplication::translate("MainWindow", "Layers", 0));
        actionEmpty->setText(QApplication::translate("MainWindow", "[No opened files]", 0));
        menu->setTitle(QApplication::translate("MainWindow", "File", 0));
        menuNew->setTitle(QApplication::translate("MainWindow", "New", 0));
        menuOpenRecent->setTitle(QApplication::translate("MainWindow", "Open Recent", 0));
        menuHelp->setTitle(QApplication::translate("MainWindow", "?", 0));
        menuLevel->setTitle(QApplication::translate("MainWindow", "Level", 0));
        menuCurrent_section->setTitle(QApplication::translate("MainWindow", "Current section", 0));
        menuWorld->setTitle(QApplication::translate("MainWindow", "World", 0));
        menuDisable_characters->setTitle(QApplication::translate("MainWindow", "Disable characters", 0));
        menuView->setTitle(QApplication::translate("MainWindow", "View", 0));
        menuWindow->setTitle(QApplication::translate("MainWindow", "Window", 0));
        menuTools->setTitle(QApplication::translate("MainWindow", "Tools", 0));
        menuEdit->setTitle(QApplication::translate("MainWindow", "Edit", 0));
        mainToolBar->setWindowTitle(QApplication::translate("MainWindow", "General", 0));
        LevelToolBox->setWindowTitle(QApplication::translate("MainWindow", "Level Tool box", 0));
        BlockCatLabel->setText(QApplication::translate("MainWindow", "Category:", 0));

        const bool __sortingEnabled = BlockItemsList->isSortingEnabled();
        BlockItemsList->setSortingEnabled(false);
        QListWidgetItem *___qlistwidgetitem = BlockItemsList->item(0);
        ___qlistwidgetitem->setText(QApplication::translate("MainWindow", "The test Icon for blocks", 0));
        BlockItemsList->setSortingEnabled(__sortingEnabled);

        BlockCatList->clear();
        BlockCatList->insertItems(0, QStringList()
         << QApplication::translate("MainWindow", "[all]", 0)
        );
        BlockUniform->setText(QString());
        LevelToolBoxTabs->setTabText(LevelToolBoxTabs->indexOf(Blocks), QApplication::translate("MainWindow", "Blocks", 0));
        BGOCatLabel->setText(QApplication::translate("MainWindow", "Category:", 0));

        const bool __sortingEnabled1 = BGOItemsList->isSortingEnabled();
        BGOItemsList->setSortingEnabled(false);
        QListWidgetItem *___qlistwidgetitem1 = BGOItemsList->item(0);
        ___qlistwidgetitem1->setText(QApplication::translate("MainWindow", "The test Icon for backgrounds", 0));
        BGOItemsList->setSortingEnabled(__sortingEnabled1);

        BGOCatList->clear();
        BGOCatList->insertItems(0, QStringList()
         << QApplication::translate("MainWindow", "[all]", 0)
        );
        BGOUniform->setText(QString());
        LevelToolBoxTabs->setTabText(LevelToolBoxTabs->indexOf(BGOs), QApplication::translate("MainWindow", "Backgrounds", 0));
        LevelToolBoxTabs->setTabText(LevelToolBoxTabs->indexOf(npc), QApplication::translate("MainWindow", "NPC", 0));
        EditionToolBar->setWindowTitle(QApplication::translate("MainWindow", "Editor", 0));
        LevelObjectToolbar->setWindowTitle(QApplication::translate("MainWindow", "toolBar", 0));
        LevelSectionsToolBar->setWindowTitle(QApplication::translate("MainWindow", "Level Sections", 0));
        WorldToolBox->setWindowTitle(QApplication::translate("MainWindow", "World map tool box", 0));
#ifndef QT_NO_TOOLTIP
        Tiles->setToolTip(QString());
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_ACCESSIBILITY
        Tiles->setAccessibleName(QString());
#endif // QT_NO_ACCESSIBILITY
        WorldToolBoxTabs->setTabText(WorldToolBoxTabs->indexOf(Tiles), QApplication::translate("MainWindow", "Tiles", 0));
        WorldToolBoxTabs->setTabText(WorldToolBoxTabs->indexOf(Scenery), QApplication::translate("MainWindow", "Scenery", 0));
        WorldToolBoxTabs->setTabText(WorldToolBoxTabs->indexOf(Level), QApplication::translate("MainWindow", "Levels", 0));
        groupBox_6->setTitle(QApplication::translate("MainWindow", "World music", 0));
        pushButton_2->setText(QApplication::translate("MainWindow", "Set point", 0));
        comboBox_3->clear();
        comboBox_3->insertItems(0, QStringList()
         << QApplication::translate("MainWindow", "0 - [silence]", 0)
        );
        label_12->setText(QApplication::translate("MainWindow", "Music:", 0));
        pushButton_3->setText(QApplication::translate("MainWindow", "Play", 0));
        WorldToolBoxTabs->setTabText(WorldToolBoxTabs->indexOf(WldMusicBox), QApplication::translate("MainWindow", "Music Box", 0));
        LevelSectionSettings->setWindowTitle(QApplication::translate("MainWindow", "Section Settings", 0));
        LVLPropMusicGr->setTitle(QApplication::translate("MainWindow", "Music", 0));
        LVLPropsMusicNumber->clear();
        LVLPropsMusicNumber->insertItems(0, QStringList()
         << QApplication::translate("MainWindow", "[Silence]", 0)
        );
        LVLPropCMusicLbl->setText(QApplication::translate("MainWindow", "Music file:", 0));
        LVLPropsMusicCustomEn->setText(QApplication::translate("MainWindow", "Custom", 0));
        LVLPropsMusicCustomBrowse->setText(QApplication::translate("MainWindow", "...", 0));
        LVLPropSettingsGrp->setTitle(QApplication::translate("MainWindow", "Section Settings", 0));
#ifndef QT_NO_WHATSTHIS
        LVLPropsLevelWarp->setWhatsThis(QApplication::translate("MainWindow", "Leaving for the screen, the player enters the screen on the other side", 0));
#endif // QT_NO_WHATSTHIS
        LVLPropsLevelWarp->setText(QApplication::translate("MainWindow", "Warp section", 0));
        LVLPropsOffScr->setText(QApplication::translate("MainWindow", "Off screen exit", 0));
        LVLPropsNoTBack->setText(QApplication::translate("MainWindow", "No turn back (disable moving to left)", 0));
        LVLPropsUnderWater->setText(QApplication::translate("MainWindow", "Underwater", 0));
        LVLPropStyleGr->setTitle(QApplication::translate("MainWindow", "Style", 0));
        LVLPropBckrImg->setText(QApplication::translate("MainWindow", "Background image", 0));
        LVLPropsBackImage->clear();
        LVLPropsBackImage->insertItems(0, QStringList()
         << QApplication::translate("MainWindow", "[No image]", 0)
        );
        groupBox_5->setTitle(QApplication::translate("MainWindow", "Current Section", 0));
        label_9->setText(QApplication::translate("MainWindow", "Section:", 0));
        LVLProp_CurSect->setText(QApplication::translate("MainWindow", "0", 0));
        ResizeSection->setText(QApplication::translate("MainWindow", "Resize section", 0));
        DoorsToolbox->setWindowTitle(QApplication::translate("MainWindow", "Warps and doors", 0));
        groupBox->setTitle(QApplication::translate("MainWindow", "Warps and doors", 0));
        groupBox_4->setTitle(QApplication::translate("MainWindow", "Level door", 0));
        WarpLevelEntrance->setText(QApplication::translate("MainWindow", "Entrance", 0));
        WarpLevelExit->setText(QApplication::translate("MainWindow", "Exit", 0));
        groupBox_7->setTitle(QApplication::translate("MainWindow", "Pipe direction", 0));
        WarpEntranceGrp->setTitle(QApplication::translate("MainWindow", "Entrance", 0));
#ifndef QT_NO_TOOLTIP
        Entr_Left->setToolTip(QApplication::translate("MainWindow", "Left", 0));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_TOOLTIP
        Entr_Right->setToolTip(QApplication::translate("MainWindow", "Right", 0));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_TOOLTIP
        Entr_Up->setToolTip(QApplication::translate("MainWindow", "Up", 0));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_TOOLTIP
        Entr_Down->setToolTip(QApplication::translate("MainWindow", "Down", 0));
#endif // QT_NO_TOOLTIP
        label_2->setText(QApplication::translate("MainWindow", "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">\n"
"<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">\n"
"p, li { white-space: pre-wrap; }\n"
"</style></head><body style=\" font-family:'Liberation Sans'; font-size:8pt; font-weight:400; font-style:normal;\">\n"
"<p align=\"center\" style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><img src=\":/images/warp_entrance.png\" /></p></body></html>", 0));
        WarpExitGrp->setTitle(QApplication::translate("MainWindow", "Exit", 0));
#ifndef QT_NO_TOOLTIP
        Exit_Left->setToolTip(QApplication::translate("MainWindow", "Left", 0));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_TOOLTIP
        Exit_Right->setToolTip(QApplication::translate("MainWindow", "Right", 0));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_TOOLTIP
        Exit_Up->setToolTip(QApplication::translate("MainWindow", "Up", 0));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_TOOLTIP
        Exit_Down->setToolTip(QApplication::translate("MainWindow", "Down", 0));
#endif // QT_NO_TOOLTIP
        label_7->setText(QApplication::translate("MainWindow", "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">\n"
"<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">\n"
"p, li { white-space: pre-wrap; }\n"
"</style></head><body style=\" font-family:'Liberation Sans'; font-size:8pt; font-weight:400; font-style:normal;\">\n"
"<p align=\"center\" style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><img src=\":/images/warp_exit.png\" /></p></body></html>", 0));
        groupBox_8->setTitle(QApplication::translate("MainWindow", "Warp to World map", 0));
        WarpGetXYFromWorldMap->setText(QApplication::translate("MainWindow", "Set", 0));
        label_6->setText(QApplication::translate("MainWindow", "Y:", 0));
        label_5->setText(QApplication::translate("MainWindow", "X:", 0));
        groupBox_9->setTitle(QApplication::translate("MainWindow", "Warp to other level", 0));
        label_8->setText(QApplication::translate("MainWindow", "Warp to", 0));
        label_11->setText(QApplication::translate("MainWindow", "Level file", 0));
        WarpBrowseLevels->setText(QApplication::translate("MainWindow", "...", 0));
        groupBox_10->setTitle(QApplication::translate("MainWindow", "Main", 0));
        WarpType->clear();
        WarpType->insertItems(0, QStringList()
         << QApplication::translate("MainWindow", "0 - Instant", 0)
         << QApplication::translate("MainWindow", "1 - Pipe", 0)
         << QApplication::translate("MainWindow", "2 - Door", 0)
        );
        label_3->setText(QApplication::translate("MainWindow", "Warp type", 0));
        WarpNoYoshi->setText(QApplication::translate("MainWindow", "No Yoshi", 0));
        label_4->setText(QApplication::translate("MainWindow", "Need stars", 0));
        WarpLock->setText(QApplication::translate("MainWindow", "Locked", 0));
        WarpAllowNPC->setText(QApplication::translate("MainWindow", "Allow NPC", 0));
        WarpSetEntrance->setText(QApplication::translate("MainWindow", "Set Entrance", 0));
        WarpSetExit->setText(QApplication::translate("MainWindow", "Set Exit", 0));
        WarpEntrancePlaced->setText(QString());
        WarpExitPlaced->setText(QString());
        WarpAdd->setText(QApplication::translate("MainWindow", "+", 0));
        WarpRemove->setText(QApplication::translate("MainWindow", "-", 0));
        LevelLayers->setWindowTitle(QApplication::translate("MainWindow", "Layers", 0));
        RemoveLayer->setText(QApplication::translate("MainWindow", "Remove", 0));
        AddLayer->setText(QApplication::translate("MainWindow", "Add", 0));
        LockLayer->setText(QApplication::translate("MainWindow", "Lock", 0));

        const bool __sortingEnabled2 = LvlLayerList->isSortingEnabled();
        LvlLayerList->setSortingEnabled(false);
        QListWidgetItem *___qlistwidgetitem2 = LvlLayerList->item(0);
        ___qlistwidgetitem2->setText(QApplication::translate("MainWindow", "Default", 0));
        QListWidgetItem *___qlistwidgetitem3 = LvlLayerList->item(1);
        ___qlistwidgetitem3->setText(QApplication::translate("MainWindow", "Destroyed Blocks", 0));
        QListWidgetItem *___qlistwidgetitem4 = LvlLayerList->item(2);
        ___qlistwidgetitem4->setText(QApplication::translate("MainWindow", "Spawned NPCs", 0));
        LvlLayerList->setSortingEnabled(__sortingEnabled2);

    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
