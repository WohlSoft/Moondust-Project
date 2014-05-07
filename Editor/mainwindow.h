/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2014 Vitaly Novichkov <admin@wohlnet.ru>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA
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
#include <QtMultimedia/QMediaPlayer>
#include <QMediaPlaylist>
#include <QFileDialog>
#include <QFile>
#include <QSettings>
#include <QTranslator>
#include <QLocale>
#include <QSplashScreen>

#include "lvl_filedata.h"
#include "wld_filedata.h"
#include "npc_filedata.h"

#include "leveledit.h"
#include "npcedit.h"

#include "npcedit.h"

#include "aboutdialog.h"
#include "levelprops.h"

#include "dataconfigs.h"

#include "musicfilelist.h"

#include "logger.h"


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
    dataconfigs *getConfigs();
    
protected:
    void closeEvent(QCloseEvent *event);
    void dragEnterEvent(QDragEnterEvent *e);
    void dropEvent(QDropEvent *e);

private slots:

    void TickTack();

    //Common functions
    void save();
    void save_as();
    void close_sw();
    void save_all();

    void OpenFile(QString FilePath);
    void SyncRecentFiles();
    void AddToRecentFiles(QString FilePath);

    void updateMenus();
    void setTools();
    void setMusic(bool checked);

    void setItemBoxes();


    //SubWindow functions
    npcedit *createNPCChild();
    leveledit *createChild();
    void setActiveSubWindow(QWidget *window);


    //File format read functions
    LevelData ReadLevelFile(QFile &inf); // SMBX LVL File
    NPCConfigFile ReadNpcTXTFile(QFile &inf); // SMBX WLD File
    WorldData ReadWorldFile(QFile &inf); //SMBX NPC.TXT File


    //LevelEdit functions
    void SetCurrentLevelSection(int SctId, int open=0);
    void setDoorsToolbox();
    void setDoorData(long index);
    void setLayersBox();


    //Actions
    void on_LevelSectionSettings_visibilityChanged(bool visible);
    void on_LevelToolBox_visibilityChanged(bool visible);
	void on_WorldToolBox_visibilityChanged(bool visible);
    //void on_pushButton_4_clicked();

    void on_OpenFile_triggered();
    void on_Exit_triggered();
    void on_actionAbout_triggered();
    void on_actionLVLToolBox_triggered();
    void on_actionWLDToolBox_triggered();
    void on_actionLevelProp_triggered();
    void on_actionSection_Settings_triggered();
    void on_actionSave_triggered();
    void on_actionSave_as_triggered();
    void on_actionClose_triggered();
    void on_actionSave_all_triggered();
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
    void on_actionLoad_configs_triggered();
    void on_actionExport_to_image_triggered();


    void on_LVLPropsMusicNumber_currentIndexChanged(int index);
    void on_LVLPropsMusicCustomEn_toggled(bool checked);
    void on_LVLPropsMusicCustomBrowse_clicked();

    //void on_LVLPropsMusicPlay_toggled(bool checked);
    void on_actionPlayMusic_triggered(bool checked);

    void on_LVLPropsMusicCustom_textChanged(const QString &arg1);

    void on_actionReset_position_triggered();

    void on_actionGridEn_triggered(bool checked);

    void on_actionSelect_triggered();
    void on_actionEriser_triggered();
    void on_actionHandScroll_triggered();

    void on_LVLPropsBackImage_currentIndexChanged(int index);

    void on_actionReload_triggered();

    //Locks
    void on_actionLockBlocks_triggered(bool checked);
    void on_actionLockBGO_triggered(bool checked);
    void on_actionLockNPC_triggered(bool checked);
    void on_actionLockWaters_triggered(bool checked);
    void on_actionLockDoors_triggered(bool checked);

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

    //RecentFiles Items
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

    void on_actionCopy_triggered();



    void on_actionPaste_triggered();

    void on_actionWarpsAndDoors_triggered(bool checked);

    void on_DoorsToolbox_visibilityChanged(bool visible);

    void on_WarpList_currentIndexChanged(int index);

    void on_LevelLayers_visibilityChanged(bool visible);

    void on_actionLayersBox_triggered(bool checked);

    void on_BGOUniform_clicked(bool checked);

    void on_BlockUniform_clicked(bool checked);

private:
    dataconfigs configs; // Global objects configucrations

    LevelEditingSettings LvlOpts;
    bool AnimationEnabled; //Animator switch

    LevelData LvlBuffer; // The Clipboarc for Level objects


    QString LastOpenDir;
    int lastWinType;
    bool LevelToolBoxVis; //Level toolbox
    bool SectionToolBoxVis; //Section Settings
    bool LevelDoorsBoxVis; //Doors box
    bool LevelLayersBoxVis; //Layers box

    bool WorldToolBoxVis;
    bool autoPlayMusic;

    QMediaPlayer * MusicPlayer;

    QString currentCustomMusic;
    long currentMusicId;
    bool musicButtonChecked;


    Ui::MainWindow *ui;
    QMdiSubWindow *findMdiChild(const QString &fileName);
    QSignalMapper *windowMapper;

    int activeChildWindow();
    npcedit *activeNpcEditWin();
    leveledit *activeLvlEditWin();

    QTimer * TickTackTimer; //Scene Events detector
    bool TickTackLock;      //Scene Events detector locker

    QList<QString> recentOpen; //Recent files list

    //Application Settings
    void setDefaults();
    void setUiDefults();

    void loadSettings();
    void saveSettings();

};



void BadFileMsg(MainWindow *window, QString fileName_DATA, int str_count, QString line);

////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////

#endif // MAINWINDOW_H
