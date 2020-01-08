#pragma once
#ifndef LVL_WARP_PROPS_H
#define LVL_WARP_PROPS_H

#include <QDockWidget>
#include "mwdock_base.h"

namespace Ui
{
class LvlWarpBox;
}

class MainWindow;
class QComboBox;
struct LevelDoor;

class LvlWarpBox : public QDockWidget, public MWDock_Base
{
    Q_OBJECT

    friend class MainWindow;
private:
    explicit LvlWarpBox(QWidget *parent);
    ~LvlWarpBox();

    void re_translate_widgets();

public:
    QComboBox *cbox_layer();
    QComboBox *cbox_event_enter();

    void setSettingsLock(bool locked);

public slots:
    void setSMBX64Strict(bool en);
    void re_translate();
    void init();//void setDoorsToolbox();

public slots:
    // Warps and doors
    void setDoorData(long index = -1);
    void SwitchToDoor(long arrayID);
    QComboBox *getWarpList();
    void setWarpRemoveButtonEnabled(bool isEnabled);
    void removeItemFromWarpList(int index);

private slots:
    void on_warpsList_currentIndexChanged(int index); //Door list
    void on_WarpLayer_currentIndexChanged(const QString &arg1); //Door's layers list
    void on_WarpEnterEvent_currentIndexChanged(const QString &arg1);
    void addWarpEntry();
    void removeWarpEntry();

    void on_WarpSetEntrance_clicked();
    void on_WarpSetExit_clicked();

    void on_WarpTwoWay_clicked(bool checked);

    void on_WarpNoVehicles_clicked(bool checked);
    void on_WarpAllowNPC_clicked(bool checked);
    void on_WarpLock_clicked(bool checked);
    void on_WarpBombNeed_clicked(bool checked);
    void on_WarpSpecialStateOnly_clicked(bool checked);

    void on_WarpType_currentIndexChanged(int index);
    void on_WarpNeedAStars_valueChanged(int arg1);
    void on_WarpNeedAStarsMsg_editingFinished();
    void on_WarpHideStars_clicked(bool checked);

    void entrance_clicked(int direction);
    void exit_clicked(int direction);

    void on_WarpEnableCannon_clicked(bool checked);
    void on_WarpCannonSpeed_valueChanged(double arg1);

    void on_WarpToMapX_editingFinished();
    void on_WarpToMapY_editingFinished();
    void on_WarpGetXYFromWorldMap_clicked();

    void on_WarpLevelExit_clicked(bool checked);
    void on_WarpLevelEntrance_clicked(bool checked);

    void on_WarpLevelFile_editingFinished();
    void on_WarpToExitNu_valueChanged(int arg1);
    void on_WarpBrowseLevels_clicked();

    void on_WarpHideLevelEnterScreen_clicked(bool checked);
    void on_WarpAllowNPC_IL_clicked(bool checked);

private:
    unsigned int getWarpId();
    QString doorTitle(LevelDoor &door);
    //! Prevent any settings changed slots to write changed settings values
    bool m_lockSettings = false;

    Ui::LvlWarpBox *ui;
};

#endif // LVL_WARP_PROPS_H
