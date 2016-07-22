#pragma once
#ifndef LVL_WARP_PROPS_H
#define LVL_WARP_PROPS_H

#include <QDockWidget>
#include "mwdock_base.h"

namespace Ui {
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

public:
    QComboBox *cbox_layer();
    QComboBox *cbox_event_enter();

public slots:
    void setSMBX64Strict(bool en);
    void re_translate();
    void init();//void setDoorsToolbox();

public slots:
    // Warps and doors
    void setDoorData(long index=-1);
    void SwitchToDoor(long arrayID);
    QComboBox* getWarpList();
    void setWarpRemoveButtonEnabled(bool isEnabled);
    void removeItemFromWarpList(int index);

private slots:
    void on_WarpList_currentIndexChanged(int index); //Door list
    void on_WarpLayer_currentIndexChanged(const QString &arg1); //Door's layers list
    void on_WarpEnterEvent_currentIndexChanged(const QString &arg1);
    void on_WarpAdd_clicked();
    void on_WarpRemove_clicked();

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

    void on_Entr_Down_clicked();
    void on_Entr_Right_clicked();
    void on_Entr_Up_clicked();
    void on_Entr_Left_clicked();
    void on_Exit_Up_clicked();
    void on_Exit_Left_clicked();
    void on_Exit_Right_clicked();
    void on_Exit_Down_clicked();

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
    QString doorTitle(LevelDoor &door);
    bool lockWarpSetSettings;

    Ui::LvlWarpBox *ui;
};

#endif // LVL_WARP_PROPS_H
