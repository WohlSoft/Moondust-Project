#pragma once
#ifndef LVL_EVENTS_BOX_H
#define LVL_EVENTS_BOX_H

#include <QDockWidget>
#include <QModelIndex>
#include "mwdock_base.h"
#include <PGE_File_Formats/lvl_filedata.h>

class MainWindow;
class QListWidget;
class QListWidgetItem;
class QComboBox;
class QPushButton;
class QToolButton;
class ImageSelector;

namespace Ui {
class LvlEventsBox;
}

class LvlEventsBox : public QDockWidget, public MWDock_Base
{
    Q_OBJECT

    friend class MainWindow;

    explicit LvlEventsBox(QWidget *parent = nullptr);
    ~LvlEventsBox();

    /**
     * @brief Automatically adds extra section setup entries if added more sections
     * @param setsList Reference to list of section setup entries
     * @param sectionID ID (from 0 to n-1) of current section
     */
    void checkSectionSet(QList<LevelEvent_Sets> &setsList, int sectionID);
public:
    bool m_externalLock = false;
    QComboBox* cbox_layer_to_move();
    QComboBox* cbox_event_trigger();
    QComboBox* cbox_sct_mus();
    ImageSelector* cbox_sct_bg();
    QToolButton *button_event_dupe();

    bool eventIsExist(QString evt);

public slots:
    void setSMBX64Strict(bool en);
    void re_translate();

    void eventSectionSettingsSync();
    void reloadSoundsList();

    void dragAndDroppedEvent(QModelIndex sourceParent,
                             int sourceStart, int sourceEnd,
                             QModelIndex destinationParent,
                             int destinationRow);

    void eventListsSync();
    void setEventsBox();
    void setEventData(long index = -1);

public slots:
    void ModifyEvent(QString eventName, QString newEventName);

    QListWidget *getEventList();
    void setEventToolsLocked(bool locked);
    long getEventArrayIndex();

private slots:
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

    void on_LVLEvents_List_customContextMenuRequested(const QPoint &pos);

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
    void sctBackgroundImageChanged(int index);

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

    void on_autoscrollType_currentIndexChanged(int index);

    void on_simpleAutoscrollH_valueChanged(double arg1);

    void on_simpleAutoscrollV_valueChanged(double arg1);

    void on_simpleAutoscrollH_BPS_clicked();

    void on_simpleAutoscrollV_BPS_clicked();

    void on_sectionAutoscroll_clicked(bool checked);

private:
    void addNewEvent(QString eventName, bool setEdited);
    void modifyEventItem(QListWidgetItem *item, QString oldEventName, QString newEventName);

    void removeEvent(QString eventName);

    void refreshShownTabs(LevelSMBX64Event event, bool hideAll=false);

    Ui::LvlEventsBox *ui;

    uint32_t m_currentEventArrayID = 0;
    bool m_internalLock = false;

    bool m_lockEventSectionDataList = false;
    long m_curSectionField = 0;

    bool m_cloneEvent = false;
    long m_cloneEventId = 0;

    int m_newEventCounter = 1;
};

#endif // LVL_EVENTS_BOX_H
