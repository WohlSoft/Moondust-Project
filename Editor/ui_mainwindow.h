/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created: Mon 24. Mar 22:02:53 2014
**      by: Qt User Interface Compiler version 4.8.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QLocale>
#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QCheckBox>
#include <QtGui/QComboBox>
#include <QtGui/QDockWidget>
#include <QtGui/QGraphicsView>
#include <QtGui/QGroupBox>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QListView>
#include <QtGui/QMainWindow>
#include <QtGui/QMdiArea>
#include <QtGui/QMenu>
#include <QtGui/QMenuBar>
#include <QtGui/QPushButton>
#include <QtGui/QRadioButton>
#include <QtGui/QScrollArea>
#include <QtGui/QSpinBox>
#include <QtGui/QStatusBar>
#include <QtGui/QTabWidget>
#include <QtGui/QToolBar>
#include <QtGui/QVBoxLayout>
#include <QtGui/QWidget>

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
    QAction *action_No_files;
    QAction *actionSelect;
    QAction *actionEriser;
    QAction *actionSection_Settings;
    QMdiArea *centralWidget;
    QMenuBar *menuBar;
    QMenu *menu;
    QMenu *menuNew;
    QMenu *menu_2;
    QMenu *menuLevel;
    QMenu *menuCurrent_section;
    QMenu *menuWorld;
    QMenu *menuDisable_characters;
    QMenu *menuWindows;
    QToolBar *mainToolBar;
    QStatusBar *statusBar;
    QDockWidget *LevelToolBox;
    QTabWidget *LevelToolBoxTabs;
    QScrollArea *Blocks;
    QWidget *blocksscroll;
    QScrollArea *backs;
    QWidget *backsscroll;
    QScrollArea *npc;
    QWidget *npcscroll;
    QScrollArea *special;
    QWidget *specialscroll;
    QWidget *layoutWidget;
    QVBoxLayout *verticalLayout_2;
    QPushButton *BTNWarpsAndDoors;
    QGroupBox *groupBox;
    QGroupBox *groupBox_4;
    QCheckBox *WarpLevelEntrance;
    QCheckBox *WarpLevelExit;
    QGroupBox *groupBox_7;
    QGroupBox *groupBox_2;
    QRadioButton *Entr_Left;
    QRadioButton *Entr_Right;
    QRadioButton *Entr_Up;
    QRadioButton *Entr_Down;
    QLabel *label_2;
    QGroupBox *groupBox_3;
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
    QSpinBox *WarpToExitNo;
    QLineEdit *lineEdit;
    QLabel *label_11;
    QPushButton *pushButton;
    QPushButton *pushButton_4;
    QGroupBox *groupBox_10;
    QComboBox *comboBox_2;
    QComboBox *comboBox;
    QLabel *label_3;
    QLabel *label;
    QCheckBox *WarpNoYoshi;
    QLabel *label_4;
    QSpinBox *WarpNeedAStars;
    QCheckBox *checkBox_2;
    QCheckBox *checkBox;
    QPushButton *BTNWaterRanges;
    QPushButton *BTNStartPoints;
    QToolBar *EditionToolBar;
    QToolBar *LevelSectionsToolBar;
    QDockWidget *WorldToolBox;
    QTabWidget *WorldToolBoxTabs;
    QScrollArea *Tiles;
    QListView *TilesItemBox;
    QScrollArea *Scenery;
    QWidget *SceneryScroll;
    QScrollArea *Level;
    QWidget *WLDLevelScroll;
    QScrollArea *MusicSet;
    QWidget *WLDMusicScroll;
    QGroupBox *groupBox_6;
    QPushButton *pushButton_2;
    QComboBox *comboBox_3;
    QLabel *label_12;
    QPushButton *pushButton_3;
    QDockWidget *LevelSectionSettings;
    QWidget *dockWidgetContents_2;
    QGroupBox *LVLPropMusicGr;
    QComboBox *LVLPropsMusicNumver_2;
    QLabel *LVLPropCMusicLbl_2;
    QLineEdit *LVLPropsMusicCustom_2;
    QCheckBox *LVLPropsMusicCustonEn_2;
    QPushButton *LVLPropsMusicCustomBrowse_2;
    QPushButton *LVLPropsMusicPlay_2;
    QGroupBox *LVLPropSettingsGrp;
    QVBoxLayout *verticalLayout_3;
    QCheckBox *LVLPropsLevelWarp_2;
    QCheckBox *LVLPropsOffScr_2;
    QCheckBox *LVLPropsNoTBack_2;
    QCheckBox *LVLPropsUnderWater_2;
    QGroupBox *LVLPropStyleGr;
    QVBoxLayout *verticalLayout;
    QLabel *LVLPropBckgrClrLbl;
    QComboBox *LVLPropsBackColor;
    QLabel *LVLPropBckrImg;
    QComboBox *LVLPropsBackImage;
    QGraphicsView *LVLPropsBackImgPreview;
    QGroupBox *groupBox_5;
    QLabel *label_9;
    QLabel *label_10;
    QPushButton *BTNResize;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QString::fromUtf8("MainWindow"));
        MainWindow->setWindowModality(Qt::NonModal);
        MainWindow->resize(1124, 910);
        QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
        sizePolicy.setHorizontalStretch(4);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(MainWindow->sizePolicy().hasHeightForWidth());
        MainWindow->setSizePolicy(sizePolicy);
        MainWindow->setFocusPolicy(Qt::NoFocus);
        MainWindow->setContextMenuPolicy(Qt::DefaultContextMenu);
        MainWindow->setAcceptDrops(true);
        MainWindow->setLocale(QLocale(QLocale::English, QLocale::UnitedStates));
        MainWindow->setDockOptions(QMainWindow::AllowNestedDocks|QMainWindow::AllowTabbedDocks|QMainWindow::AnimatedDocks);
        OpenFile = new QAction(MainWindow);
        OpenFile->setObjectName(QString::fromUtf8("OpenFile"));
        OpenFile->setCheckable(false);
        QIcon icon;
        icon.addFile(QString::fromUtf8(":/images/open.png"), QSize(), QIcon::Normal, QIcon::Off);
        OpenFile->setIcon(icon);
        Exit = new QAction(MainWindow);
        Exit->setObjectName(QString::fromUtf8("Exit"));
        actionNewLevel = new QAction(MainWindow);
        actionNewLevel->setObjectName(QString::fromUtf8("actionNewLevel"));
        actionNewWorld_map = new QAction(MainWindow);
        actionNewWorld_map->setObjectName(QString::fromUtf8("actionNewWorld_map"));
        actionNPC_config = new QAction(MainWindow);
        actionNPC_config->setObjectName(QString::fromUtf8("actionNPC_config"));
        actionSave = new QAction(MainWindow);
        actionSave->setObjectName(QString::fromUtf8("actionSave"));
        actionSave->setEnabled(false);
        QIcon icon1;
        icon1.addFile(QString::fromUtf8(":/images/save.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionSave->setIcon(icon1);
        actionSave_as = new QAction(MainWindow);
        actionSave_as->setObjectName(QString::fromUtf8("actionSave_as"));
        actionSave_as->setEnabled(false);
        QIcon icon2;
        icon2.addFile(QString::fromUtf8(":/images/saveas.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionSave_as->setIcon(icon2);
        actionClose = new QAction(MainWindow);
        actionClose->setObjectName(QString::fromUtf8("actionClose"));
        actionSave_all = new QAction(MainWindow);
        actionSave_all->setObjectName(QString::fromUtf8("actionSave_all"));
        actionSave_all->setEnabled(false);
        actionAbout = new QAction(MainWindow);
        actionAbout->setObjectName(QString::fromUtf8("actionAbout"));
        actionAbout->setShortcutContext(Qt::ApplicationShortcut);
        actionAbout->setMenuRole(QAction::NoRole);
        actionContents = new QAction(MainWindow);
        actionContents->setObjectName(QString::fromUtf8("actionContents"));
        actionContents->setEnabled(false);
        actionSection_1 = new QAction(MainWindow);
        actionSection_1->setObjectName(QString::fromUtf8("actionSection_1"));
        actionSection_1->setCheckable(true);
        actionSection_1->setEnabled(false);
        QIcon icon3;
        icon3.addFile(QString::fromUtf8(":/images/01.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionSection_1->setIcon(icon3);
        actionSection_2 = new QAction(MainWindow);
        actionSection_2->setObjectName(QString::fromUtf8("actionSection_2"));
        actionSection_2->setCheckable(true);
        actionSection_2->setEnabled(false);
        QIcon icon4;
        icon4.addFile(QString::fromUtf8(":/images/02.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionSection_2->setIcon(icon4);
        actionSection_3 = new QAction(MainWindow);
        actionSection_3->setObjectName(QString::fromUtf8("actionSection_3"));
        actionSection_3->setCheckable(true);
        actionSection_3->setEnabled(false);
        QIcon icon5;
        icon5.addFile(QString::fromUtf8(":/images/03.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionSection_3->setIcon(icon5);
        actionSection_4 = new QAction(MainWindow);
        actionSection_4->setObjectName(QString::fromUtf8("actionSection_4"));
        actionSection_4->setCheckable(true);
        actionSection_4->setEnabled(false);
        QIcon icon6;
        icon6.addFile(QString::fromUtf8(":/images/04.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionSection_4->setIcon(icon6);
        actionSection_5 = new QAction(MainWindow);
        actionSection_5->setObjectName(QString::fromUtf8("actionSection_5"));
        actionSection_5->setCheckable(true);
        actionSection_5->setEnabled(false);
        QIcon icon7;
        icon7.addFile(QString::fromUtf8(":/images/05.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionSection_5->setIcon(icon7);
        actionSection_6 = new QAction(MainWindow);
        actionSection_6->setObjectName(QString::fromUtf8("actionSection_6"));
        actionSection_6->setCheckable(true);
        actionSection_6->setEnabled(false);
        QIcon icon8;
        icon8.addFile(QString::fromUtf8(":/images/06.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionSection_6->setIcon(icon8);
        actionSection_7 = new QAction(MainWindow);
        actionSection_7->setObjectName(QString::fromUtf8("actionSection_7"));
        actionSection_7->setCheckable(true);
        actionSection_7->setEnabled(false);
        QIcon icon9;
        icon9.addFile(QString::fromUtf8(":/images/07.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionSection_7->setIcon(icon9);
        actionSection_8 = new QAction(MainWindow);
        actionSection_8->setObjectName(QString::fromUtf8("actionSection_8"));
        actionSection_8->setCheckable(true);
        actionSection_8->setEnabled(false);
        QIcon icon10;
        icon10.addFile(QString::fromUtf8(":/images/08.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionSection_8->setIcon(icon10);
        actionSection_9 = new QAction(MainWindow);
        actionSection_9->setObjectName(QString::fromUtf8("actionSection_9"));
        actionSection_9->setCheckable(true);
        actionSection_9->setEnabled(false);
        QIcon icon11;
        icon11.addFile(QString::fromUtf8(":/images/09.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionSection_9->setIcon(icon11);
        actionSection_10 = new QAction(MainWindow);
        actionSection_10->setObjectName(QString::fromUtf8("actionSection_10"));
        actionSection_10->setCheckable(true);
        actionSection_10->setEnabled(false);
        QIcon icon12;
        icon12.addFile(QString::fromUtf8(":/images/10.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionSection_10->setIcon(icon12);
        actionSection_11 = new QAction(MainWindow);
        actionSection_11->setObjectName(QString::fromUtf8("actionSection_11"));
        actionSection_11->setCheckable(true);
        actionSection_11->setEnabled(false);
        QIcon icon13;
        icon13.addFile(QString::fromUtf8(":/images/11.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionSection_11->setIcon(icon13);
        actionSection_12 = new QAction(MainWindow);
        actionSection_12->setObjectName(QString::fromUtf8("actionSection_12"));
        actionSection_12->setCheckable(true);
        actionSection_12->setEnabled(false);
        QIcon icon14;
        icon14.addFile(QString::fromUtf8(":/images/12.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionSection_12->setIcon(icon14);
        actionSection_13 = new QAction(MainWindow);
        actionSection_13->setObjectName(QString::fromUtf8("actionSection_13"));
        actionSection_13->setCheckable(true);
        actionSection_13->setEnabled(false);
        QIcon icon15;
        icon15.addFile(QString::fromUtf8(":/images/13.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionSection_13->setIcon(icon15);
        actionSection_14 = new QAction(MainWindow);
        actionSection_14->setObjectName(QString::fromUtf8("actionSection_14"));
        actionSection_14->setCheckable(true);
        actionSection_14->setEnabled(false);
        QIcon icon16;
        icon16.addFile(QString::fromUtf8(":/images/14.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionSection_14->setIcon(icon16);
        actionSection_15 = new QAction(MainWindow);
        actionSection_15->setObjectName(QString::fromUtf8("actionSection_15"));
        actionSection_15->setCheckable(true);
        actionSection_15->setEnabled(false);
        QIcon icon17;
        icon17.addFile(QString::fromUtf8(":/images/15.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionSection_15->setIcon(icon17);
        actionSection_16 = new QAction(MainWindow);
        actionSection_16->setObjectName(QString::fromUtf8("actionSection_16"));
        actionSection_16->setCheckable(true);
        actionSection_16->setEnabled(false);
        QIcon icon18;
        icon18.addFile(QString::fromUtf8(":/images/16.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionSection_16->setIcon(icon18);
        actionSection_17 = new QAction(MainWindow);
        actionSection_17->setObjectName(QString::fromUtf8("actionSection_17"));
        actionSection_17->setCheckable(true);
        actionSection_17->setEnabled(false);
        QIcon icon19;
        icon19.addFile(QString::fromUtf8(":/images/17.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionSection_17->setIcon(icon19);
        actionSection_18 = new QAction(MainWindow);
        actionSection_18->setObjectName(QString::fromUtf8("actionSection_18"));
        actionSection_18->setCheckable(true);
        actionSection_18->setEnabled(false);
        QIcon icon20;
        icon20.addFile(QString::fromUtf8(":/images/18.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionSection_18->setIcon(icon20);
        actionSection_19 = new QAction(MainWindow);
        actionSection_19->setObjectName(QString::fromUtf8("actionSection_19"));
        actionSection_19->setCheckable(true);
        actionSection_19->setEnabled(false);
        QIcon icon21;
        icon21.addFile(QString::fromUtf8(":/images/19.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionSection_19->setIcon(icon21);
        actionSection_20 = new QAction(MainWindow);
        actionSection_20->setObjectName(QString::fromUtf8("actionSection_20"));
        actionSection_20->setCheckable(true);
        actionSection_20->setEnabled(false);
        QIcon icon22;
        icon22.addFile(QString::fromUtf8(":/images/20.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionSection_20->setIcon(icon22);
        actionSection_21 = new QAction(MainWindow);
        actionSection_21->setObjectName(QString::fromUtf8("actionSection_21"));
        actionSection_21->setCheckable(true);
        actionSection_21->setEnabled(false);
        QIcon icon23;
        icon23.addFile(QString::fromUtf8(":/images/21.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionSection_21->setIcon(icon23);
        actionLevWarp = new QAction(MainWindow);
        actionLevWarp->setObjectName(QString::fromUtf8("actionLevWarp"));
        actionLevWarp->setCheckable(true);
        actionLevWarp->setEnabled(false);
        actionLevOffScr = new QAction(MainWindow);
        actionLevOffScr->setObjectName(QString::fromUtf8("actionLevOffScr"));
        actionLevOffScr->setCheckable(true);
        actionLevOffScr->setEnabled(false);
        actionLevNoBack = new QAction(MainWindow);
        actionLevNoBack->setObjectName(QString::fromUtf8("actionLevNoBack"));
        actionLevNoBack->setCheckable(true);
        actionLevNoBack->setEnabled(false);
        actionLevUnderW = new QAction(MainWindow);
        actionLevUnderW->setObjectName(QString::fromUtf8("actionLevUnderW"));
        actionLevUnderW->setCheckable(true);
        actionLevUnderW->setEnabled(false);
        actionExport_to_image = new QAction(MainWindow);
        actionExport_to_image->setObjectName(QString::fromUtf8("actionExport_to_image"));
        actionExport_to_image->setEnabled(false);
        actionLevelProp = new QAction(MainWindow);
        actionLevelProp->setObjectName(QString::fromUtf8("actionLevelProp"));
        actionLevelProp->setEnabled(false);
        actionWLDDisableMap = new QAction(MainWindow);
        actionWLDDisableMap->setObjectName(QString::fromUtf8("actionWLDDisableMap"));
        actionWLDDisableMap->setCheckable(true);
        actionWLDFailRestart = new QAction(MainWindow);
        actionWLDFailRestart->setObjectName(QString::fromUtf8("actionWLDFailRestart"));
        actionWLDFailRestart->setCheckable(true);
        actionWLDNoChar1 = new QAction(MainWindow);
        actionWLDNoChar1->setObjectName(QString::fromUtf8("actionWLDNoChar1"));
        actionWLDNoChar1->setCheckable(true);
        actionWLDNoChar2 = new QAction(MainWindow);
        actionWLDNoChar2->setObjectName(QString::fromUtf8("actionWLDNoChar2"));
        actionWLDNoChar2->setCheckable(true);
        actionWLDNoChar3 = new QAction(MainWindow);
        actionWLDNoChar3->setObjectName(QString::fromUtf8("actionWLDNoChar3"));
        actionWLDNoChar3->setCheckable(true);
        actionWLDNoChar4 = new QAction(MainWindow);
        actionWLDNoChar4->setObjectName(QString::fromUtf8("actionWLDNoChar4"));
        actionWLDNoChar4->setCheckable(true);
        actionWLDNoChar5 = new QAction(MainWindow);
        actionWLDNoChar5->setObjectName(QString::fromUtf8("actionWLDNoChar5"));
        actionWLDNoChar5->setCheckable(true);
        actionWLDProperties = new QAction(MainWindow);
        actionWLDProperties->setObjectName(QString::fromUtf8("actionWLDProperties"));
        actionLVLToolBox = new QAction(MainWindow);
        actionLVLToolBox->setObjectName(QString::fromUtf8("actionLVLToolBox"));
        actionLVLToolBox->setCheckable(true);
        QIcon icon24;
        icon24.addFile(QString::fromUtf8(":/images/level.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionLVLToolBox->setIcon(icon24);
        actionLVLToolBox->setIconVisibleInMenu(false);
        actionWLDToolBox = new QAction(MainWindow);
        actionWLDToolBox->setObjectName(QString::fromUtf8("actionWLDToolBox"));
        actionWLDToolBox->setCheckable(true);
        QIcon icon25;
        icon25.addFile(QString::fromUtf8(":/images/world.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionWLDToolBox->setIcon(icon25);
        actionWLDToolBox->setIconVisibleInMenu(false);
        action_No_files = new QAction(MainWindow);
        action_No_files->setObjectName(QString::fromUtf8("action_No_files"));
        action_No_files->setEnabled(false);
        actionSelect = new QAction(MainWindow);
        actionSelect->setObjectName(QString::fromUtf8("actionSelect"));
        actionSelect->setCheckable(true);
        actionSelect->setChecked(true);
        actionSelect->setEnabled(false);
        QIcon icon26;
        icon26.addFile(QString::fromUtf8(":/images/arrow.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionSelect->setIcon(icon26);
        actionEriser = new QAction(MainWindow);
        actionEriser->setObjectName(QString::fromUtf8("actionEriser"));
        actionEriser->setCheckable(true);
        actionEriser->setEnabled(false);
        QIcon icon27;
        icon27.addFile(QString::fromUtf8(":/images/rubber.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionEriser->setIcon(icon27);
        actionSection_Settings = new QAction(MainWindow);
        actionSection_Settings->setObjectName(QString::fromUtf8("actionSection_Settings"));
        actionSection_Settings->setCheckable(true);
        QIcon icon28;
        icon28.addFile(QString::fromUtf8(":/images/section.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionSection_Settings->setIcon(icon28);
        actionSection_Settings->setVisible(true);
        actionSection_Settings->setIconVisibleInMenu(false);
        centralWidget = new QMdiArea(MainWindow);
        centralWidget->setObjectName(QString::fromUtf8("centralWidget"));
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
        menuBar->setObjectName(QString::fromUtf8("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 1124, 21));
        menuBar->setContextMenuPolicy(Qt::NoContextMenu);
        menu = new QMenu(menuBar);
        menu->setObjectName(QString::fromUtf8("menu"));
        menuNew = new QMenu(menu);
        menuNew->setObjectName(QString::fromUtf8("menuNew"));
        menu_2 = new QMenu(menuBar);
        menu_2->setObjectName(QString::fromUtf8("menu_2"));
        menuLevel = new QMenu(menuBar);
        menuLevel->setObjectName(QString::fromUtf8("menuLevel"));
        menuLevel->setEnabled(true);
        menuCurrent_section = new QMenu(menuLevel);
        menuCurrent_section->setObjectName(QString::fromUtf8("menuCurrent_section"));
        menuCurrent_section->setEnabled(true);
        menuWorld = new QMenu(menuBar);
        menuWorld->setObjectName(QString::fromUtf8("menuWorld"));
        menuWorld->setEnabled(true);
        menuDisable_characters = new QMenu(menuWorld);
        menuDisable_characters->setObjectName(QString::fromUtf8("menuDisable_characters"));
        menuWindows = new QMenu(menuBar);
        menuWindows->setObjectName(QString::fromUtf8("menuWindows"));
        MainWindow->setMenuBar(menuBar);
        mainToolBar = new QToolBar(MainWindow);
        mainToolBar->setObjectName(QString::fromUtf8("mainToolBar"));
        mainToolBar->setEnabled(true);
        mainToolBar->setMinimumSize(QSize(0, 0));
        mainToolBar->setAllowedAreas(Qt::TopToolBarArea);
        mainToolBar->setIconSize(QSize(24, 24));
        MainWindow->addToolBar(Qt::TopToolBarArea, mainToolBar);
        statusBar = new QStatusBar(MainWindow);
        statusBar->setObjectName(QString::fromUtf8("statusBar"));
        MainWindow->setStatusBar(statusBar);
        LevelToolBox = new QDockWidget(MainWindow);
        LevelToolBox->setObjectName(QString::fromUtf8("LevelToolBox"));
        LevelToolBox->setEnabled(true);
        QSizePolicy sizePolicy1(QSizePolicy::Preferred, QSizePolicy::Preferred);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(LevelToolBox->sizePolicy().hasHeightForWidth());
        LevelToolBox->setSizePolicy(sizePolicy1);
        LevelToolBox->setMinimumSize(QSize(255, 200));
        LevelToolBox->setFocusPolicy(Qt::NoFocus);
        LevelToolBox->setWindowIcon(icon24);
        LevelToolBox->setStyleSheet(QString::fromUtf8("font: 8pt \"Liberation Sans\";"));
        LevelToolBox->setAllowedAreas(Qt::BottomDockWidgetArea|Qt::LeftDockWidgetArea|Qt::RightDockWidgetArea);
        LevelToolBoxTabs = new QTabWidget();
        LevelToolBoxTabs->setObjectName(QString::fromUtf8("LevelToolBoxTabs"));
        Blocks = new QScrollArea();
        Blocks->setObjectName(QString::fromUtf8("Blocks"));
        Blocks->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
        Blocks->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        Blocks->setWidgetResizable(true);
        blocksscroll = new QWidget();
        blocksscroll->setObjectName(QString::fromUtf8("blocksscroll"));
        blocksscroll->setGeometry(QRect(0, 0, 239, 313));
        blocksscroll->setStyleSheet(QString::fromUtf8("background-color: rgb(255, 255, 255);"));
        Blocks->setWidget(blocksscroll);
        LevelToolBoxTabs->addTab(Blocks, QString());
        backs = new QScrollArea();
        backs->setObjectName(QString::fromUtf8("backs"));
        backs->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
        backs->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        backs->setWidgetResizable(true);
        backsscroll = new QWidget();
        backsscroll->setObjectName(QString::fromUtf8("backsscroll"));
        backsscroll->setGeometry(QRect(0, 0, 239, 313));
        backsscroll->setStyleSheet(QString::fromUtf8("background-color: rgb(255, 255, 255);"));
        backs->setWidget(backsscroll);
        LevelToolBoxTabs->addTab(backs, QString());
        npc = new QScrollArea();
        npc->setObjectName(QString::fromUtf8("npc"));
        npc->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
        npc->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        npc->setWidgetResizable(true);
        npcscroll = new QWidget();
        npcscroll->setObjectName(QString::fromUtf8("npcscroll"));
        npcscroll->setGeometry(QRect(0, 0, 239, 313));
        npcscroll->setStyleSheet(QString::fromUtf8("background-color: rgb(255, 255, 255);"));
        npc->setWidget(npcscroll);
        LevelToolBoxTabs->addTab(npc, QString());
        special = new QScrollArea();
        special->setObjectName(QString::fromUtf8("special"));
        special->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
        special->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        special->setWidgetResizable(true);
        specialscroll = new QWidget();
        specialscroll->setObjectName(QString::fromUtf8("specialscroll"));
        specialscroll->setGeometry(QRect(0, 0, 239, 313));
        specialscroll->setAutoFillBackground(true);
        specialscroll->setStyleSheet(QString::fromUtf8(""));
        layoutWidget = new QWidget(specialscroll);
        layoutWidget->setObjectName(QString::fromUtf8("layoutWidget"));
        layoutWidget->setGeometry(QRect(0, 10, 241, 571));
        verticalLayout_2 = new QVBoxLayout(layoutWidget);
        verticalLayout_2->setSpacing(6);
        verticalLayout_2->setContentsMargins(11, 11, 11, 11);
        verticalLayout_2->setObjectName(QString::fromUtf8("verticalLayout_2"));
        verticalLayout_2->setContentsMargins(0, 0, 0, 0);
        BTNWarpsAndDoors = new QPushButton(layoutWidget);
        BTNWarpsAndDoors->setObjectName(QString::fromUtf8("BTNWarpsAndDoors"));
        QSizePolicy sizePolicy2(QSizePolicy::Expanding, QSizePolicy::Fixed);
        sizePolicy2.setHorizontalStretch(0);
        sizePolicy2.setVerticalStretch(0);
        sizePolicy2.setHeightForWidth(BTNWarpsAndDoors->sizePolicy().hasHeightForWidth());
        BTNWarpsAndDoors->setSizePolicy(sizePolicy2);
        BTNWarpsAndDoors->setCheckable(true);

        verticalLayout_2->addWidget(BTNWarpsAndDoors);

        groupBox = new QGroupBox(layoutWidget);
        groupBox->setObjectName(QString::fromUtf8("groupBox"));
        groupBox->setAutoFillBackground(true);
        groupBox->setStyleSheet(QString::fromUtf8("border-color: rgb(0, 0, 0);"));
        groupBox->setFlat(false);
        groupBox_4 = new QGroupBox(groupBox);
        groupBox_4->setObjectName(QString::fromUtf8("groupBox_4"));
        groupBox_4->setGeometry(QRect(10, 330, 111, 61));
        WarpLevelEntrance = new QCheckBox(groupBox_4);
        WarpLevelEntrance->setObjectName(QString::fromUtf8("WarpLevelEntrance"));
        WarpLevelEntrance->setGeometry(QRect(10, 40, 91, 17));
        WarpLevelExit = new QCheckBox(groupBox_4);
        WarpLevelExit->setObjectName(QString::fromUtf8("WarpLevelExit"));
        WarpLevelExit->setGeometry(QRect(10, 20, 91, 17));
        groupBox_7 = new QGroupBox(groupBox);
        groupBox_7->setObjectName(QString::fromUtf8("groupBox_7"));
        groupBox_7->setGeometry(QRect(10, 150, 211, 121));
        groupBox_2 = new QGroupBox(groupBox_7);
        groupBox_2->setObjectName(QString::fromUtf8("groupBox_2"));
        groupBox_2->setGeometry(QRect(10, 10, 91, 101));
        groupBox_2->setAlignment(Qt::AlignCenter);
        groupBox_2->setFlat(false);
        Entr_Left = new QRadioButton(groupBox_2);
        Entr_Left->setObjectName(QString::fromUtf8("Entr_Left"));
        Entr_Left->setGeometry(QRect(70, 50, 16, 16));
        Entr_Left->setChecked(true);
        Entr_Right = new QRadioButton(groupBox_2);
        Entr_Right->setObjectName(QString::fromUtf8("Entr_Right"));
        Entr_Right->setGeometry(QRect(10, 50, 16, 16));
        Entr_Up = new QRadioButton(groupBox_2);
        Entr_Up->setObjectName(QString::fromUtf8("Entr_Up"));
        Entr_Up->setGeometry(QRect(40, 80, 16, 16));
        Entr_Up->setChecked(false);
        Entr_Down = new QRadioButton(groupBox_2);
        Entr_Down->setObjectName(QString::fromUtf8("Entr_Down"));
        Entr_Down->setGeometry(QRect(40, 20, 16, 16));
        label_2 = new QLabel(groupBox_2);
        label_2->setObjectName(QString::fromUtf8("label_2"));
        label_2->setGeometry(QRect(25, 38, 41, 41));
        groupBox_3 = new QGroupBox(groupBox_7);
        groupBox_3->setObjectName(QString::fromUtf8("groupBox_3"));
        groupBox_3->setGeometry(QRect(110, 10, 91, 101));
        groupBox_3->setAlignment(Qt::AlignCenter);
        Exit_Left = new QRadioButton(groupBox_3);
        Exit_Left->setObjectName(QString::fromUtf8("Exit_Left"));
        Exit_Left->setGeometry(QRect(10, 50, 16, 16));
        Exit_Left->setChecked(true);
        Exit_Right = new QRadioButton(groupBox_3);
        Exit_Right->setObjectName(QString::fromUtf8("Exit_Right"));
        Exit_Right->setGeometry(QRect(70, 50, 16, 16));
        Exit_Up = new QRadioButton(groupBox_3);
        Exit_Up->setObjectName(QString::fromUtf8("Exit_Up"));
        Exit_Up->setGeometry(QRect(40, 20, 16, 16));
        Exit_Up->setChecked(false);
        Exit_Down = new QRadioButton(groupBox_3);
        Exit_Down->setObjectName(QString::fromUtf8("Exit_Down"));
        Exit_Down->setGeometry(QRect(40, 80, 16, 16));
        label_7 = new QLabel(groupBox_3);
        label_7->setObjectName(QString::fromUtf8("label_7"));
        label_7->setGeometry(QRect(20, 37, 51, 41));
        groupBox_8 = new QGroupBox(groupBox);
        groupBox_8->setObjectName(QString::fromUtf8("groupBox_8"));
        groupBox_8->setGeometry(QRect(10, 280, 211, 51));
        WarpGetXYFromWorldMap = new QPushButton(groupBox_8);
        WarpGetXYFromWorldMap->setObjectName(QString::fromUtf8("WarpGetXYFromWorldMap"));
        WarpGetXYFromWorldMap->setGeometry(QRect(150, 20, 41, 21));
        label_6 = new QLabel(groupBox_8);
        label_6->setObjectName(QString::fromUtf8("label_6"));
        label_6->setGeometry(QRect(90, 20, 16, 16));
        WarpToMapX = new QLineEdit(groupBox_8);
        WarpToMapX->setObjectName(QString::fromUtf8("WarpToMapX"));
        WarpToMapX->setGeometry(QRect(30, 20, 51, 20));
        label_5 = new QLabel(groupBox_8);
        label_5->setObjectName(QString::fromUtf8("label_5"));
        label_5->setGeometry(QRect(10, 20, 16, 16));
        WarpToMapY = new QLineEdit(groupBox_8);
        WarpToMapY->setObjectName(QString::fromUtf8("WarpToMapY"));
        WarpToMapY->setGeometry(QRect(100, 20, 51, 20));
        groupBox_9 = new QGroupBox(groupBox);
        groupBox_9->setObjectName(QString::fromUtf8("groupBox_9"));
        groupBox_9->setGeometry(QRect(10, 400, 211, 71));
        label_8 = new QLabel(groupBox_9);
        label_8->setObjectName(QString::fromUtf8("label_8"));
        label_8->setGeometry(QRect(140, 20, 61, 16));
        WarpToExitNo = new QSpinBox(groupBox_9);
        WarpToExitNo->setObjectName(QString::fromUtf8("WarpToExitNo"));
        WarpToExitNo->setGeometry(QRect(140, 40, 61, 22));
        lineEdit = new QLineEdit(groupBox_9);
        lineEdit->setObjectName(QString::fromUtf8("lineEdit"));
        lineEdit->setGeometry(QRect(10, 40, 101, 20));
        label_11 = new QLabel(groupBox_9);
        label_11->setObjectName(QString::fromUtf8("label_11"));
        label_11->setGeometry(QRect(10, 20, 111, 16));
        pushButton = new QPushButton(groupBox_9);
        pushButton->setObjectName(QString::fromUtf8("pushButton"));
        pushButton->setGeometry(QRect(110, 40, 21, 20));
        pushButton_4 = new QPushButton(groupBox_9);
        pushButton_4->setObjectName(QString::fromUtf8("pushButton_4"));
        pushButton_4->setGeometry(QRect(60, 10, 61, 23));
        groupBox_10 = new QGroupBox(groupBox);
        groupBox_10->setObjectName(QString::fromUtf8("groupBox_10"));
        groupBox_10->setGeometry(QRect(10, 20, 211, 121));
        comboBox_2 = new QComboBox(groupBox_10);
        comboBox_2->setObjectName(QString::fromUtf8("comboBox_2"));
        comboBox_2->setGeometry(QRect(10, 90, 101, 22));
        comboBox = new QComboBox(groupBox_10);
        comboBox->setObjectName(QString::fromUtf8("comboBox"));
        comboBox->setGeometry(QRect(10, 40, 101, 22));
        label_3 = new QLabel(groupBox_10);
        label_3->setObjectName(QString::fromUtf8("label_3"));
        label_3->setGeometry(QRect(10, 70, 101, 16));
        label = new QLabel(groupBox_10);
        label->setObjectName(QString::fromUtf8("label"));
        label->setGeometry(QRect(10, 20, 101, 16));
        WarpNoYoshi = new QCheckBox(groupBox_10);
        WarpNoYoshi->setObjectName(QString::fromUtf8("WarpNoYoshi"));
        WarpNoYoshi->setGeometry(QRect(120, 10, 81, 17));
        label_4 = new QLabel(groupBox_10);
        label_4->setObjectName(QString::fromUtf8("label_4"));
        label_4->setGeometry(QRect(130, 70, 61, 16));
        WarpNeedAStars = new QSpinBox(groupBox_10);
        WarpNeedAStars->setObjectName(QString::fromUtf8("WarpNeedAStars"));
        WarpNeedAStars->setGeometry(QRect(130, 90, 61, 22));
        checkBox_2 = new QCheckBox(groupBox_10);
        checkBox_2->setObjectName(QString::fromUtf8("checkBox_2"));
        checkBox_2->setGeometry(QRect(120, 50, 81, 17));
        checkBox = new QCheckBox(groupBox_10);
        checkBox->setObjectName(QString::fromUtf8("checkBox"));
        checkBox->setGeometry(QRect(120, 30, 81, 17));

        verticalLayout_2->addWidget(groupBox);

        BTNWaterRanges = new QPushButton(layoutWidget);
        BTNWaterRanges->setObjectName(QString::fromUtf8("BTNWaterRanges"));

        verticalLayout_2->addWidget(BTNWaterRanges);

        BTNStartPoints = new QPushButton(layoutWidget);
        BTNStartPoints->setObjectName(QString::fromUtf8("BTNStartPoints"));

        verticalLayout_2->addWidget(BTNStartPoints);

        special->setWidget(specialscroll);
        LevelToolBoxTabs->addTab(special, QString());
        LevelToolBox->setWidget(LevelToolBoxTabs);
        MainWindow->addDockWidget(static_cast<Qt::DockWidgetArea>(2), LevelToolBox);
        EditionToolBar = new QToolBar(MainWindow);
        EditionToolBar->setObjectName(QString::fromUtf8("EditionToolBar"));
        EditionToolBar->setAllowedAreas(Qt::TopToolBarArea);
        MainWindow->addToolBar(Qt::TopToolBarArea, EditionToolBar);
        LevelSectionsToolBar = new QToolBar(MainWindow);
        LevelSectionsToolBar->setObjectName(QString::fromUtf8("LevelSectionsToolBar"));
        LevelSectionsToolBar->setAcceptDrops(false);
        LevelSectionsToolBar->setAllowedAreas(Qt::BottomToolBarArea|Qt::TopToolBarArea);
        MainWindow->addToolBar(Qt::TopToolBarArea, LevelSectionsToolBar);
        MainWindow->insertToolBarBreak(LevelSectionsToolBar);
        WorldToolBox = new QDockWidget(MainWindow);
        WorldToolBox->setObjectName(QString::fromUtf8("WorldToolBox"));
        WorldToolBox->setEnabled(true);
        sizePolicy1.setHeightForWidth(WorldToolBox->sizePolicy().hasHeightForWidth());
        WorldToolBox->setSizePolicy(sizePolicy1);
        WorldToolBox->setWindowIcon(icon25);
        WorldToolBox->setStyleSheet(QString::fromUtf8("font: 8pt \"Liberation Sans\";"));
        WorldToolBox->setAllowedAreas(Qt::BottomDockWidgetArea|Qt::LeftDockWidgetArea|Qt::RightDockWidgetArea);
        WorldToolBoxTabs = new QTabWidget();
        WorldToolBoxTabs->setObjectName(QString::fromUtf8("WorldToolBoxTabs"));
        Tiles = new QScrollArea();
        Tiles->setObjectName(QString::fromUtf8("Tiles"));
        Tiles->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
        Tiles->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
        Tiles->setWidgetResizable(true);
        TilesItemBox = new QListView();
        TilesItemBox->setObjectName(QString::fromUtf8("TilesItemBox"));
        TilesItemBox->setGeometry(QRect(0, 0, 256, 389));
        TilesItemBox->setStyleSheet(QString::fromUtf8(" Item {\n"
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
        Scenery->setObjectName(QString::fromUtf8("Scenery"));
        Scenery->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
        Scenery->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        Scenery->setWidgetResizable(true);
        SceneryScroll = new QWidget();
        SceneryScroll->setObjectName(QString::fromUtf8("SceneryScroll"));
        SceneryScroll->setGeometry(QRect(0, 0, 239, 389));
        Scenery->setWidget(SceneryScroll);
        WorldToolBoxTabs->addTab(Scenery, QString());
        Level = new QScrollArea();
        Level->setObjectName(QString::fromUtf8("Level"));
        Level->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
        Level->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        Level->setWidgetResizable(true);
        WLDLevelScroll = new QWidget();
        WLDLevelScroll->setObjectName(QString::fromUtf8("WLDLevelScroll"));
        WLDLevelScroll->setGeometry(QRect(0, 0, 239, 389));
        Level->setWidget(WLDLevelScroll);
        WorldToolBoxTabs->addTab(Level, QString());
        MusicSet = new QScrollArea();
        MusicSet->setObjectName(QString::fromUtf8("MusicSet"));
        MusicSet->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
        MusicSet->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        MusicSet->setWidgetResizable(true);
        WLDMusicScroll = new QWidget();
        WLDMusicScroll->setObjectName(QString::fromUtf8("WLDMusicScroll"));
        WLDMusicScroll->setGeometry(QRect(0, 0, 239, 389));
        groupBox_6 = new QGroupBox(WLDMusicScroll);
        groupBox_6->setObjectName(QString::fromUtf8("groupBox_6"));
        groupBox_6->setGeometry(QRect(10, 10, 141, 131));
        pushButton_2 = new QPushButton(groupBox_6);
        pushButton_2->setObjectName(QString::fromUtf8("pushButton_2"));
        pushButton_2->setGeometry(QRect(10, 20, 121, 23));
        pushButton_2->setCheckable(true);
        comboBox_3 = new QComboBox(groupBox_6);
        comboBox_3->setObjectName(QString::fromUtf8("comboBox_3"));
        comboBox_3->setGeometry(QRect(10, 70, 121, 22));
        label_12 = new QLabel(groupBox_6);
        label_12->setObjectName(QString::fromUtf8("label_12"));
        label_12->setGeometry(QRect(10, 50, 121, 16));
        pushButton_3 = new QPushButton(groupBox_6);
        pushButton_3->setObjectName(QString::fromUtf8("pushButton_3"));
        pushButton_3->setGeometry(QRect(10, 100, 121, 23));
        pushButton_3->setCheckable(true);
        MusicSet->setWidget(WLDMusicScroll);
        WorldToolBoxTabs->addTab(MusicSet, QString());
        WorldToolBox->setWidget(WorldToolBoxTabs);
        MainWindow->addDockWidget(static_cast<Qt::DockWidgetArea>(2), WorldToolBox);
        LevelSectionSettings = new QDockWidget(MainWindow);
        LevelSectionSettings->setObjectName(QString::fromUtf8("LevelSectionSettings"));
        QSizePolicy sizePolicy3(QSizePolicy::Minimum, QSizePolicy::Preferred);
        sizePolicy3.setHorizontalStretch(0);
        sizePolicy3.setVerticalStretch(0);
        sizePolicy3.setHeightForWidth(LevelSectionSettings->sizePolicy().hasHeightForWidth());
        LevelSectionSettings->setSizePolicy(sizePolicy3);
        LevelSectionSettings->setMinimumSize(QSize(230, 500));
        LevelSectionSettings->setMaximumSize(QSize(230, 524287));
        LevelSectionSettings->setWindowIcon(icon28);
        LevelSectionSettings->setStyleSheet(QString::fromUtf8("font: 8pt \"Liberation Sans\";"));
        LevelSectionSettings->setFloating(false);
        LevelSectionSettings->setFeatures(QDockWidget::AllDockWidgetFeatures);
        LevelSectionSettings->setAllowedAreas(Qt::LeftDockWidgetArea|Qt::RightDockWidgetArea);
        dockWidgetContents_2 = new QWidget();
        dockWidgetContents_2->setObjectName(QString::fromUtf8("dockWidgetContents_2"));
        LVLPropMusicGr = new QGroupBox(dockWidgetContents_2);
        LVLPropMusicGr->setObjectName(QString::fromUtf8("LVLPropMusicGr"));
        LVLPropMusicGr->setGeometry(QRect(0, 160, 221, 131));
        LVLPropsMusicNumver_2 = new QComboBox(LVLPropMusicGr);
        LVLPropsMusicNumver_2->setObjectName(QString::fromUtf8("LVLPropsMusicNumver_2"));
        LVLPropsMusicNumver_2->setGeometry(QRect(10, 20, 101, 20));
        LVLPropCMusicLbl_2 = new QLabel(LVLPropMusicGr);
        LVLPropCMusicLbl_2->setObjectName(QString::fromUtf8("LVLPropCMusicLbl_2"));
        LVLPropCMusicLbl_2->setGeometry(QRect(10, 50, 47, 16));
        LVLPropCMusicLbl_2->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
        LVLPropsMusicCustom_2 = new QLineEdit(LVLPropMusicGr);
        LVLPropsMusicCustom_2->setObjectName(QString::fromUtf8("LVLPropsMusicCustom_2"));
        LVLPropsMusicCustom_2->setGeometry(QRect(10, 70, 133, 21));
        LVLPropsMusicCustonEn_2 = new QCheckBox(LVLPropMusicGr);
        LVLPropsMusicCustonEn_2->setObjectName(QString::fromUtf8("LVLPropsMusicCustonEn_2"));
        LVLPropsMusicCustonEn_2->setGeometry(QRect(120, 20, 88, 17));
        LVLPropsMusicCustonEn_2->setTristate(false);
        LVLPropsMusicCustomBrowse_2 = new QPushButton(LVLPropMusicGr);
        LVLPropsMusicCustomBrowse_2->setObjectName(QString::fromUtf8("LVLPropsMusicCustomBrowse_2"));
        LVLPropsMusicCustomBrowse_2->setGeometry(QRect(140, 70, 31, 21));
        LVLPropsMusicPlay_2 = new QPushButton(LVLPropMusicGr);
        LVLPropsMusicPlay_2->setObjectName(QString::fromUtf8("LVLPropsMusicPlay_2"));
        LVLPropsMusicPlay_2->setGeometry(QRect(10, 100, 131, 23));
        LVLPropsMusicPlay_2->setCheckable(true);
        LVLPropSettingsGrp = new QGroupBox(dockWidgetContents_2);
        LVLPropSettingsGrp->setObjectName(QString::fromUtf8("LVLPropSettingsGrp"));
        LVLPropSettingsGrp->setGeometry(QRect(0, 40, 221, 121));
        verticalLayout_3 = new QVBoxLayout(LVLPropSettingsGrp);
        verticalLayout_3->setSpacing(6);
        verticalLayout_3->setContentsMargins(11, 11, 11, 11);
        verticalLayout_3->setObjectName(QString::fromUtf8("verticalLayout_3"));
        LVLPropsLevelWarp_2 = new QCheckBox(LVLPropSettingsGrp);
        LVLPropsLevelWarp_2->setObjectName(QString::fromUtf8("LVLPropsLevelWarp_2"));
        LVLPropsLevelWarp_2->setTristate(false);

        verticalLayout_3->addWidget(LVLPropsLevelWarp_2);

        LVLPropsOffScr_2 = new QCheckBox(LVLPropSettingsGrp);
        LVLPropsOffScr_2->setObjectName(QString::fromUtf8("LVLPropsOffScr_2"));
        LVLPropsOffScr_2->setTristate(false);

        verticalLayout_3->addWidget(LVLPropsOffScr_2);

        LVLPropsNoTBack_2 = new QCheckBox(LVLPropSettingsGrp);
        LVLPropsNoTBack_2->setObjectName(QString::fromUtf8("LVLPropsNoTBack_2"));
        LVLPropsNoTBack_2->setTristate(false);

        verticalLayout_3->addWidget(LVLPropsNoTBack_2);

        LVLPropsUnderWater_2 = new QCheckBox(LVLPropSettingsGrp);
        LVLPropsUnderWater_2->setObjectName(QString::fromUtf8("LVLPropsUnderWater_2"));
        LVLPropsUnderWater_2->setTristate(false);

        verticalLayout_3->addWidget(LVLPropsUnderWater_2);

        LVLPropStyleGr = new QGroupBox(dockWidgetContents_2);
        LVLPropStyleGr->setObjectName(QString::fromUtf8("LVLPropStyleGr"));
        LVLPropStyleGr->setGeometry(QRect(0, 290, 221, 201));
        verticalLayout = new QVBoxLayout(LVLPropStyleGr);
        verticalLayout->setSpacing(6);
        verticalLayout->setContentsMargins(11, 11, 11, 11);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        LVLPropBckgrClrLbl = new QLabel(LVLPropStyleGr);
        LVLPropBckgrClrLbl->setObjectName(QString::fromUtf8("LVLPropBckgrClrLbl"));
        LVLPropBckgrClrLbl->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignVCenter);

        verticalLayout->addWidget(LVLPropBckgrClrLbl);

        LVLPropsBackColor = new QComboBox(LVLPropStyleGr);
        LVLPropsBackColor->setObjectName(QString::fromUtf8("LVLPropsBackColor"));

        verticalLayout->addWidget(LVLPropsBackColor);

        LVLPropBckrImg = new QLabel(LVLPropStyleGr);
        LVLPropBckrImg->setObjectName(QString::fromUtf8("LVLPropBckrImg"));

        verticalLayout->addWidget(LVLPropBckrImg);

        LVLPropsBackImage = new QComboBox(LVLPropStyleGr);
        LVLPropsBackImage->setObjectName(QString::fromUtf8("LVLPropsBackImage"));

        verticalLayout->addWidget(LVLPropsBackImage);

        LVLPropsBackImgPreview = new QGraphicsView(LVLPropStyleGr);
        LVLPropsBackImgPreview->setObjectName(QString::fromUtf8("LVLPropsBackImgPreview"));

        verticalLayout->addWidget(LVLPropsBackImgPreview);

        groupBox_5 = new QGroupBox(dockWidgetContents_2);
        groupBox_5->setObjectName(QString::fromUtf8("groupBox_5"));
        groupBox_5->setGeometry(QRect(0, 0, 91, 41));
        label_9 = new QLabel(groupBox_5);
        label_9->setObjectName(QString::fromUtf8("label_9"));
        label_9->setGeometry(QRect(10, 20, 51, 16));
        label_9->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
        label_10 = new QLabel(groupBox_5);
        label_10->setObjectName(QString::fromUtf8("label_10"));
        label_10->setGeometry(QRect(70, 20, 16, 16));
        BTNResize = new QPushButton(dockWidgetContents_2);
        BTNResize->setObjectName(QString::fromUtf8("BTNResize"));
        BTNResize->setGeometry(QRect(100, 10, 111, 23));
        BTNResize->setCheckable(true);
        LevelSectionSettings->setWidget(dockWidgetContents_2);
        MainWindow->addDockWidget(static_cast<Qt::DockWidgetArea>(1), LevelSectionSettings);
        QWidget::setTabOrder(BTNWarpsAndDoors, BTNWaterRanges);
        QWidget::setTabOrder(BTNWaterRanges, BTNStartPoints);

        menuBar->addAction(menu->menuAction());
        menuBar->addAction(menuLevel->menuAction());
        menuBar->addAction(menuWorld->menuAction());
        menuBar->addAction(menuWindows->menuAction());
        menuBar->addAction(menu_2->menuAction());
        menu->addAction(menuNew->menuAction());
        menu->addAction(OpenFile);
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
        menu_2->addAction(actionContents);
        menu_2->addAction(actionAbout);
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
        menuWindows->addAction(actionLVLToolBox);
        menuWindows->addAction(actionSection_Settings);
        menuWindows->addAction(actionWLDToolBox);
        menuWindows->addSeparator();
        menuWindows->addAction(action_No_files);
        mainToolBar->addAction(OpenFile);
        mainToolBar->addAction(actionSave);
        mainToolBar->addAction(actionSave_as);
        EditionToolBar->addAction(actionSelect);
        EditionToolBar->addAction(actionEriser);
        EditionToolBar->addSeparator();
        EditionToolBar->addAction(actionLVLToolBox);
        EditionToolBar->addAction(actionSection_Settings);
        EditionToolBar->addAction(actionWLDToolBox);
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

        LevelToolBoxTabs->setCurrentIndex(3);
        WorldToolBoxTabs->setCurrentIndex(3);


        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QApplication::translate("MainWindow", "Editor [Skeleton] - PlatGEnWohl (Under Construction)", 0, QApplication::UnicodeUTF8));
        OpenFile->setText(QApplication::translate("MainWindow", "Open...", 0, QApplication::UnicodeUTF8));
        OpenFile->setShortcut(QApplication::translate("MainWindow", "Ctrl+O", 0, QApplication::UnicodeUTF8));
        Exit->setText(QApplication::translate("MainWindow", "Exit", 0, QApplication::UnicodeUTF8));
        Exit->setShortcut(QApplication::translate("MainWindow", "Ctrl+Alt+Q", 0, QApplication::UnicodeUTF8));
        actionNewLevel->setText(QApplication::translate("MainWindow", "Level...", 0, QApplication::UnicodeUTF8));
        actionNewWorld_map->setText(QApplication::translate("MainWindow", "World map...", 0, QApplication::UnicodeUTF8));
        actionNPC_config->setText(QApplication::translate("MainWindow", "NPC config...", 0, QApplication::UnicodeUTF8));
        actionSave->setText(QApplication::translate("MainWindow", "Save", 0, QApplication::UnicodeUTF8));
        actionSave->setShortcut(QApplication::translate("MainWindow", "Ctrl+S", 0, QApplication::UnicodeUTF8));
        actionSave_as->setText(QApplication::translate("MainWindow", "Save as...", 0, QApplication::UnicodeUTF8));
        actionSave_as->setShortcut(QApplication::translate("MainWindow", "Ctrl+Shift+S", 0, QApplication::UnicodeUTF8));
        actionClose->setText(QApplication::translate("MainWindow", "Close", 0, QApplication::UnicodeUTF8));
        actionSave_all->setText(QApplication::translate("MainWindow", "Save all", 0, QApplication::UnicodeUTF8));
        actionSave_all->setShortcut(QApplication::translate("MainWindow", "Ctrl+Alt+S", 0, QApplication::UnicodeUTF8));
        actionAbout->setText(QApplication::translate("MainWindow", "About", 0, QApplication::UnicodeUTF8));
        actionContents->setText(QApplication::translate("MainWindow", "Contents", 0, QApplication::UnicodeUTF8));
        actionContents->setShortcut(QApplication::translate("MainWindow", "F1", 0, QApplication::UnicodeUTF8));
        actionSection_1->setText(QApplication::translate("MainWindow", "Section 1", 0, QApplication::UnicodeUTF8));
        actionSection_1->setShortcut(QApplication::translate("MainWindow", "Ctrl+1", 0, QApplication::UnicodeUTF8));
        actionSection_2->setText(QApplication::translate("MainWindow", "Section 2", 0, QApplication::UnicodeUTF8));
        actionSection_2->setShortcut(QApplication::translate("MainWindow", "Ctrl+2", 0, QApplication::UnicodeUTF8));
        actionSection_3->setText(QApplication::translate("MainWindow", "Section 3", 0, QApplication::UnicodeUTF8));
        actionSection_3->setShortcut(QApplication::translate("MainWindow", "Ctrl+3", 0, QApplication::UnicodeUTF8));
        actionSection_4->setText(QApplication::translate("MainWindow", "Section 4", 0, QApplication::UnicodeUTF8));
        actionSection_4->setShortcut(QApplication::translate("MainWindow", "Ctrl+4", 0, QApplication::UnicodeUTF8));
        actionSection_5->setText(QApplication::translate("MainWindow", "Section 5", 0, QApplication::UnicodeUTF8));
        actionSection_5->setShortcut(QApplication::translate("MainWindow", "Ctrl+5", 0, QApplication::UnicodeUTF8));
        actionSection_6->setText(QApplication::translate("MainWindow", "Section 6", 0, QApplication::UnicodeUTF8));
        actionSection_6->setShortcut(QApplication::translate("MainWindow", "Ctrl+6", 0, QApplication::UnicodeUTF8));
        actionSection_7->setText(QApplication::translate("MainWindow", "Section 7", 0, QApplication::UnicodeUTF8));
        actionSection_7->setShortcut(QApplication::translate("MainWindow", "Ctrl+7", 0, QApplication::UnicodeUTF8));
        actionSection_8->setText(QApplication::translate("MainWindow", "Section 8", 0, QApplication::UnicodeUTF8));
        actionSection_8->setShortcut(QApplication::translate("MainWindow", "Ctrl+8", 0, QApplication::UnicodeUTF8));
        actionSection_9->setText(QApplication::translate("MainWindow", "Section 9", 0, QApplication::UnicodeUTF8));
        actionSection_9->setShortcut(QApplication::translate("MainWindow", "Ctrl+9", 0, QApplication::UnicodeUTF8));
        actionSection_10->setText(QApplication::translate("MainWindow", "Section 10", 0, QApplication::UnicodeUTF8));
        actionSection_10->setShortcut(QApplication::translate("MainWindow", "Ctrl+0", 0, QApplication::UnicodeUTF8));
        actionSection_11->setText(QApplication::translate("MainWindow", "Section 11", 0, QApplication::UnicodeUTF8));
        actionSection_11->setShortcut(QApplication::translate("MainWindow", "Ctrl+Alt+1", 0, QApplication::UnicodeUTF8));
        actionSection_12->setText(QApplication::translate("MainWindow", "Section 12", 0, QApplication::UnicodeUTF8));
        actionSection_12->setShortcut(QApplication::translate("MainWindow", "Ctrl+Alt+2", 0, QApplication::UnicodeUTF8));
        actionSection_13->setText(QApplication::translate("MainWindow", "Section 13", 0, QApplication::UnicodeUTF8));
        actionSection_13->setShortcut(QApplication::translate("MainWindow", "Ctrl+Alt+3", 0, QApplication::UnicodeUTF8));
        actionSection_14->setText(QApplication::translate("MainWindow", "Section 14", 0, QApplication::UnicodeUTF8));
        actionSection_14->setShortcut(QApplication::translate("MainWindow", "Ctrl+Alt+4", 0, QApplication::UnicodeUTF8));
        actionSection_15->setText(QApplication::translate("MainWindow", "Section 15", 0, QApplication::UnicodeUTF8));
        actionSection_15->setShortcut(QApplication::translate("MainWindow", "Ctrl+Alt+5", 0, QApplication::UnicodeUTF8));
        actionSection_16->setText(QApplication::translate("MainWindow", "Section 16", 0, QApplication::UnicodeUTF8));
        actionSection_16->setShortcut(QApplication::translate("MainWindow", "Ctrl+Alt+6", 0, QApplication::UnicodeUTF8));
        actionSection_17->setText(QApplication::translate("MainWindow", "Section 17", 0, QApplication::UnicodeUTF8));
        actionSection_17->setShortcut(QApplication::translate("MainWindow", "Ctrl+Alt+7", 0, QApplication::UnicodeUTF8));
        actionSection_18->setText(QApplication::translate("MainWindow", "Section 18", 0, QApplication::UnicodeUTF8));
        actionSection_18->setShortcut(QApplication::translate("MainWindow", "Ctrl+Alt+8", 0, QApplication::UnicodeUTF8));
        actionSection_19->setText(QApplication::translate("MainWindow", "Section 19", 0, QApplication::UnicodeUTF8));
        actionSection_19->setShortcut(QApplication::translate("MainWindow", "Ctrl+Alt+9", 0, QApplication::UnicodeUTF8));
        actionSection_20->setText(QApplication::translate("MainWindow", "Section 20", 0, QApplication::UnicodeUTF8));
        actionSection_20->setShortcut(QApplication::translate("MainWindow", "Ctrl+Alt+0", 0, QApplication::UnicodeUTF8));
        actionSection_21->setText(QApplication::translate("MainWindow", "Section 21", 0, QApplication::UnicodeUTF8));
        actionSection_21->setShortcut(QApplication::translate("MainWindow", "Alt+1", 0, QApplication::UnicodeUTF8));
        actionLevWarp->setText(QApplication::translate("MainWindow", "Warp section", 0, QApplication::UnicodeUTF8));
        actionLevOffScr->setText(QApplication::translate("MainWindow", "Off screen exit", 0, QApplication::UnicodeUTF8));
        actionLevNoBack->setText(QApplication::translate("MainWindow", "No turn back", 0, QApplication::UnicodeUTF8));
        actionLevUnderW->setText(QApplication::translate("MainWindow", "Underwater", 0, QApplication::UnicodeUTF8));
        actionExport_to_image->setText(QApplication::translate("MainWindow", "Export to image...", 0, QApplication::UnicodeUTF8));
        actionLevelProp->setText(QApplication::translate("MainWindow", "Properties...", 0, QApplication::UnicodeUTF8));
        actionWLDDisableMap->setText(QApplication::translate("MainWindow", "Disable world map", 0, QApplication::UnicodeUTF8));
        actionWLDFailRestart->setText(QApplication::translate("MainWindow", "Restart level after player's fail", 0, QApplication::UnicodeUTF8));
        actionWLDNoChar1->setText(QApplication::translate("MainWindow", "Anthony", 0, QApplication::UnicodeUTF8));
        actionWLDNoChar2->setText(QApplication::translate("MainWindow", "Stuwart", 0, QApplication::UnicodeUTF8));
        actionWLDNoChar3->setText(QApplication::translate("MainWindow", "Susan", 0, QApplication::UnicodeUTF8));
        actionWLDNoChar4->setText(QApplication::translate("MainWindow", "Jose", 0, QApplication::UnicodeUTF8));
        actionWLDNoChar5->setText(QApplication::translate("MainWindow", "Yudziro", 0, QApplication::UnicodeUTF8));
        actionWLDProperties->setText(QApplication::translate("MainWindow", "Properties...", 0, QApplication::UnicodeUTF8));
        actionLVLToolBox->setText(QApplication::translate("MainWindow", "Level tool box", 0, QApplication::UnicodeUTF8));
        actionWLDToolBox->setText(QApplication::translate("MainWindow", "World tool box", 0, QApplication::UnicodeUTF8));
        action_No_files->setText(QApplication::translate("MainWindow", "<No files>", 0, QApplication::UnicodeUTF8));
        actionSelect->setText(QApplication::translate("MainWindow", "Select", 0, QApplication::UnicodeUTF8));
        actionSelect->setShortcut(QApplication::translate("MainWindow", "S", 0, QApplication::UnicodeUTF8));
        actionEriser->setText(QApplication::translate("MainWindow", "Eriser", 0, QApplication::UnicodeUTF8));
        actionEriser->setShortcut(QApplication::translate("MainWindow", "E", 0, QApplication::UnicodeUTF8));
        actionSection_Settings->setText(QApplication::translate("MainWindow", "Level Section Settings", 0, QApplication::UnicodeUTF8));
        menu->setTitle(QApplication::translate("MainWindow", "File", 0, QApplication::UnicodeUTF8));
        menuNew->setTitle(QApplication::translate("MainWindow", "New", 0, QApplication::UnicodeUTF8));
        menu_2->setTitle(QApplication::translate("MainWindow", "?", 0, QApplication::UnicodeUTF8));
        menuLevel->setTitle(QApplication::translate("MainWindow", "Level", 0, QApplication::UnicodeUTF8));
        menuCurrent_section->setTitle(QApplication::translate("MainWindow", "Current section", 0, QApplication::UnicodeUTF8));
        menuWorld->setTitle(QApplication::translate("MainWindow", "World", 0, QApplication::UnicodeUTF8));
        menuDisable_characters->setTitle(QApplication::translate("MainWindow", "Disable characters", 0, QApplication::UnicodeUTF8));
        menuWindows->setTitle(QApplication::translate("MainWindow", "View", 0, QApplication::UnicodeUTF8));
        mainToolBar->setWindowTitle(QApplication::translate("MainWindow", "General", 0, QApplication::UnicodeUTF8));
        LevelToolBox->setWindowTitle(QApplication::translate("MainWindow", "Level Tool box", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        Blocks->setToolTip(QString());
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_ACCESSIBILITY
        Blocks->setAccessibleName(QString());
#endif // QT_NO_ACCESSIBILITY
        LevelToolBoxTabs->setTabText(LevelToolBoxTabs->indexOf(Blocks), QApplication::translate("MainWindow", "Blocks", 0, QApplication::UnicodeUTF8));
        LevelToolBoxTabs->setTabText(LevelToolBoxTabs->indexOf(backs), QApplication::translate("MainWindow", "Backgrounds", 0, QApplication::UnicodeUTF8));
        LevelToolBoxTabs->setTabText(LevelToolBoxTabs->indexOf(npc), QApplication::translate("MainWindow", "NPC", 0, QApplication::UnicodeUTF8));
        BTNWarpsAndDoors->setText(QApplication::translate("MainWindow", "Warps and Doors", 0, QApplication::UnicodeUTF8));
        groupBox->setTitle(QApplication::translate("MainWindow", "Warps and doors", 0, QApplication::UnicodeUTF8));
        groupBox_4->setTitle(QApplication::translate("MainWindow", "Level", 0, QApplication::UnicodeUTF8));
        WarpLevelEntrance->setText(QApplication::translate("MainWindow", "Level entrance", 0, QApplication::UnicodeUTF8));
        WarpLevelExit->setText(QApplication::translate("MainWindow", "Level exit", 0, QApplication::UnicodeUTF8));
        groupBox_7->setTitle(QApplication::translate("MainWindow", "Direction", 0, QApplication::UnicodeUTF8));
        groupBox_2->setTitle(QApplication::translate("MainWindow", "Entrance", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        Entr_Left->setToolTip(QApplication::translate("MainWindow", "Left", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_TOOLTIP
        Entr_Right->setToolTip(QApplication::translate("MainWindow", "Right", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_TOOLTIP
        Entr_Up->setToolTip(QApplication::translate("MainWindow", "Up", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_TOOLTIP
        Entr_Down->setToolTip(QApplication::translate("MainWindow", "Down", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        label_2->setText(QApplication::translate("MainWindow", "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">\n"
"<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">\n"
"p, li { white-space: pre-wrap; }\n"
"</style></head><body style=\" font-family:'Liberation Sans'; font-size:8pt; font-weight:400; font-style:normal;\">\n"
"<p align=\"center\" style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><img src=\":/images/warp_entrance.png\" /></p></body></html>", 0, QApplication::UnicodeUTF8));
        groupBox_3->setTitle(QApplication::translate("MainWindow", "Exit", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        Exit_Left->setToolTip(QApplication::translate("MainWindow", "Left", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_TOOLTIP
        Exit_Right->setToolTip(QApplication::translate("MainWindow", "Right", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_TOOLTIP
        Exit_Up->setToolTip(QApplication::translate("MainWindow", "Up", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_TOOLTIP
        Exit_Down->setToolTip(QApplication::translate("MainWindow", "Down", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        label_7->setText(QApplication::translate("MainWindow", "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">\n"
"<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">\n"
"p, li { white-space: pre-wrap; }\n"
"</style></head><body style=\" font-family:'Liberation Sans'; font-size:8pt; font-weight:400; font-style:normal;\">\n"
"<p align=\"center\" style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><img src=\":/images/warp_exit.png\" /></p></body></html>", 0, QApplication::UnicodeUTF8));
        groupBox_8->setTitle(QApplication::translate("MainWindow", "Warp to World map", 0, QApplication::UnicodeUTF8));
        WarpGetXYFromWorldMap->setText(QApplication::translate("MainWindow", "Set", 0, QApplication::UnicodeUTF8));
        label_6->setText(QApplication::translate("MainWindow", "Y:", 0, QApplication::UnicodeUTF8));
        label_5->setText(QApplication::translate("MainWindow", "X:", 0, QApplication::UnicodeUTF8));
        groupBox_9->setTitle(QApplication::translate("MainWindow", "Warp to level", 0, QApplication::UnicodeUTF8));
        label_8->setText(QApplication::translate("MainWindow", "Warp to", 0, QApplication::UnicodeUTF8));
        label_11->setText(QApplication::translate("MainWindow", "Level file", 0, QApplication::UnicodeUTF8));
        pushButton->setText(QApplication::translate("MainWindow", "...", 0, QApplication::UnicodeUTF8));
        pushButton_4->setText(QApplication::translate("MainWindow", "[some test]", 0, QApplication::UnicodeUTF8));
        groupBox_10->setTitle(QApplication::translate("MainWindow", "Main", 0, QApplication::UnicodeUTF8));
        comboBox_2->clear();
        comboBox_2->insertItems(0, QStringList()
         << QApplication::translate("MainWindow", "0 - Instant", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("MainWindow", "1 - Pipe", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("MainWindow", "2 - Door", 0, QApplication::UnicodeUTF8)
        );
        comboBox->clear();
        comboBox->insertItems(0, QStringList()
         << QApplication::translate("MainWindow", "Entrance", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("MainWindow", "Exit", 0, QApplication::UnicodeUTF8)
        );
        label_3->setText(QApplication::translate("MainWindow", "Warp type", 0, QApplication::UnicodeUTF8));
        label->setText(QApplication::translate("MainWindow", "Placement", 0, QApplication::UnicodeUTF8));
        WarpNoYoshi->setText(QApplication::translate("MainWindow", "No Yoshi", 0, QApplication::UnicodeUTF8));
        label_4->setText(QApplication::translate("MainWindow", "Need stars", 0, QApplication::UnicodeUTF8));
        checkBox_2->setText(QApplication::translate("MainWindow", "Locked", 0, QApplication::UnicodeUTF8));
        checkBox->setText(QApplication::translate("MainWindow", "Allow NPC", 0, QApplication::UnicodeUTF8));
        BTNWaterRanges->setText(QApplication::translate("MainWindow", "Water Ranges", 0, QApplication::UnicodeUTF8));
        BTNStartPoints->setText(QApplication::translate("MainWindow", "Player start points", 0, QApplication::UnicodeUTF8));
        LevelToolBoxTabs->setTabText(LevelToolBoxTabs->indexOf(special), QApplication::translate("MainWindow", "Special", 0, QApplication::UnicodeUTF8));
        EditionToolBar->setWindowTitle(QApplication::translate("MainWindow", "Editor", 0, QApplication::UnicodeUTF8));
        LevelSectionsToolBar->setWindowTitle(QApplication::translate("MainWindow", "Level Sections", 0, QApplication::UnicodeUTF8));
        WorldToolBox->setWindowTitle(QApplication::translate("MainWindow", "World map tool box", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        Tiles->setToolTip(QString());
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_ACCESSIBILITY
        Tiles->setAccessibleName(QString());
#endif // QT_NO_ACCESSIBILITY
        WorldToolBoxTabs->setTabText(WorldToolBoxTabs->indexOf(Tiles), QApplication::translate("MainWindow", "Tiles", 0, QApplication::UnicodeUTF8));
        WorldToolBoxTabs->setTabText(WorldToolBoxTabs->indexOf(Scenery), QApplication::translate("MainWindow", "Scenery", 0, QApplication::UnicodeUTF8));
        WorldToolBoxTabs->setTabText(WorldToolBoxTabs->indexOf(Level), QApplication::translate("MainWindow", "Levels", 0, QApplication::UnicodeUTF8));
        groupBox_6->setTitle(QApplication::translate("MainWindow", "World music", 0, QApplication::UnicodeUTF8));
        pushButton_2->setText(QApplication::translate("MainWindow", "Set point", 0, QApplication::UnicodeUTF8));
        comboBox_3->clear();
        comboBox_3->insertItems(0, QStringList()
         << QApplication::translate("MainWindow", "0 - [silence]", 0, QApplication::UnicodeUTF8)
        );
        label_12->setText(QApplication::translate("MainWindow", "Music:", 0, QApplication::UnicodeUTF8));
        pushButton_3->setText(QApplication::translate("MainWindow", "Play", 0, QApplication::UnicodeUTF8));
        WorldToolBoxTabs->setTabText(WorldToolBoxTabs->indexOf(MusicSet), QApplication::translate("MainWindow", "Music Set", 0, QApplication::UnicodeUTF8));
        LevelSectionSettings->setWindowTitle(QApplication::translate("MainWindow", "Section Settings", 0, QApplication::UnicodeUTF8));
        LVLPropMusicGr->setTitle(QApplication::translate("MainWindow", "Music", 0, QApplication::UnicodeUTF8));
        LVLPropsMusicNumver_2->clear();
        LVLPropsMusicNumver_2->insertItems(0, QStringList()
         << QApplication::translate("MainWindow", "[Silence]", 0, QApplication::UnicodeUTF8)
        );
        LVLPropCMusicLbl_2->setText(QApplication::translate("MainWindow", "Music file:", 0, QApplication::UnicodeUTF8));
        LVLPropsMusicCustonEn_2->setText(QApplication::translate("MainWindow", "Custom music", 0, QApplication::UnicodeUTF8));
        LVLPropsMusicCustomBrowse_2->setText(QApplication::translate("MainWindow", "...", 0, QApplication::UnicodeUTF8));
        LVLPropsMusicPlay_2->setText(QApplication::translate("MainWindow", "Play", 0, QApplication::UnicodeUTF8));
        LVLPropSettingsGrp->setTitle(QApplication::translate("MainWindow", "Section Settings", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_WHATSTHIS
        LVLPropsLevelWarp_2->setWhatsThis(QApplication::translate("MainWindow", "Leaving for the screen, the player enters the screen on the other side", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_WHATSTHIS
        LVLPropsLevelWarp_2->setText(QApplication::translate("MainWindow", "Level warp", 0, QApplication::UnicodeUTF8));
        LVLPropsOffScr_2->setText(QApplication::translate("MainWindow", "Off screen exit", 0, QApplication::UnicodeUTF8));
        LVLPropsNoTBack_2->setText(QApplication::translate("MainWindow", "No turn back (disable moving to left)", 0, QApplication::UnicodeUTF8));
        LVLPropsUnderWater_2->setText(QApplication::translate("MainWindow", "Underwater", 0, QApplication::UnicodeUTF8));
        LVLPropStyleGr->setTitle(QApplication::translate("MainWindow", "Style", 0, QApplication::UnicodeUTF8));
        LVLPropBckgrClrLbl->setText(QApplication::translate("MainWindow", "Background color:", 0, QApplication::UnicodeUTF8));
        LVLPropsBackColor->clear();
        LVLPropsBackColor->insertItems(0, QStringList()
         << QApplication::translate("MainWindow", "Black", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("MainWindow", "Blue", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("MainWindow", "Turquoise", 0, QApplication::UnicodeUTF8)
        );
        LVLPropBckrImg->setText(QApplication::translate("MainWindow", "Background image", 0, QApplication::UnicodeUTF8));
        LVLPropsBackImage->clear();
        LVLPropsBackImage->insertItems(0, QStringList()
         << QApplication::translate("MainWindow", "[No image]", 0, QApplication::UnicodeUTF8)
        );
        groupBox_5->setTitle(QApplication::translate("MainWindow", "Current Section", 0, QApplication::UnicodeUTF8));
        label_9->setText(QApplication::translate("MainWindow", "Section:", 0, QApplication::UnicodeUTF8));
        label_10->setText(QApplication::translate("MainWindow", "0", 0, QApplication::UnicodeUTF8));
        BTNResize->setText(QApplication::translate("MainWindow", "Resize section", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
