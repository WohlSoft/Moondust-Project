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

#include <common_features/util.h>
#include <editing/_scenes/level/items/item_bgo.h>
#include <editing/_scenes/level/items/item_block.h>
#include <editing/_scenes/level/items/item_npc.h>
#include <editing/_scenes/level/items/item_water.h>
#include <editing/_scenes/level/itemmsgbox.h>
#include <audio/sdl_music_player.h>
#include <PGE_File_Formats/file_formats.h>
#include <tools/math/blocksperseconddialog.h>
#include <main_window/dock/lvl_item_properties.h>
#include <main_window/dock/lvl_search_box.h>

#include <ui_mainwindow.h>
#include <mainwindow.h>

#include "lvl_events_box.h"
#include "ui_lvl_events_box.h"

LvlEventsBox::LvlEventsBox(QWidget *parent) :
    QDockWidget(parent),
    MWDock_Base(parent),
    ui(new Ui::LvlEventsBox)
{
    setVisible(false);
    setAttribute(Qt::WA_ShowWithoutActivating);
    ui->setupUi(this);

    QRect mwg = mw()->geometry();
    int GOffset=240;
    mw()->addDockWidget(Qt::RightDockWidgetArea, this);
    connect(mw(), SIGNAL(languageSwitched()), this, SLOT(re_translate()));
    #ifdef Q_OS_WIN
    setFloating(true);
    #endif
    setGeometry(
                mwg.x()+mwg.width()-width()-GOffset,
                mwg.y()+120,
                width(),
                height()
                );

    connect(ui->LVLEvents_List->model(), SIGNAL(rowsMoved(QModelIndex,int,int,QModelIndex,int)),
            this, SLOT(DragAndDroppedEvent(QModelIndex,int,int,QModelIndex,int)));

    mw()->docks_level.
          addState(this, &GlobalSettings::LevelEventsBoxVis);

    LvlEventBoxLock=false;

    currentEventArrayID=0;
    lockSetEventSettings=false;

    lockEventSectionDataList=false;
    curSectionField=0;

    cloneEvent=false;
    cloneEventId=0;
    newEventCounter=1;

    /**Initializing spoilers**/
    refreshShownTabs(FileFormats::CreateLvlEvent(), true);
}

LvlEventsBox::~LvlEventsBox()
{
    delete ui;
}

QComboBox *LvlEventsBox::cbox_layer_to_move()
{
    return ui->LVLEvent_LayerMov_List;
}
QComboBox *LvlEventsBox::cbox_event_trigger()
{
    return ui->LVLEvent_TriggerEvent;
}

QComboBox *LvlEventsBox::cbox_sct_mus()
{
    return ui->LVLEvent_SctMus_List;
}

QComboBox *LvlEventsBox::cbox_sct_bg()
{
    return ui->LVLEvent_SctBg_List;
}

QPushButton *LvlEventsBox::button_event_dupe()
{
    return ui->LVLEvents_duplicate;
}

void LvlEventsBox::re_translate()
{
    LvlEventBoxLock=true;
    lockSetEventSettings=true;
    ui->retranslateUi(this);
    EventListsSync();
    setEventData(-1);
    reloadSoundsList();
    lockSetEventSettings=false;
    LvlEventBoxLock=false;
}


void LvlEventsBox::on_LvlEventsBox_visibilityChanged(bool visible)
{
    mw()->ui->actionLevelEvents->setChecked(visible);
}

void MainWindow::on_actionLevelEvents_triggered(bool checked)
{
    dock_LvlEvents->setVisible(checked);
    if(checked) dock_LvlEvents->raise();
}

void MainWindow::setEventsBox()
{
    dock_LvlEvents->setEventsBox();
}

void LvlEventsBox::setEventsBox()
{
    int WinType = mw()->activeChildWindow();
    QListWidgetItem * item;

    util::memclear(ui->LVLEvents_List);

    if (WinType==1)
    {
        LevelEdit * edit = mw()->activeLvlEditWin();
        if(!edit) return;
        foreach(LevelSMBX64Event event, edit->LvlData.events)
        {
            item = new QListWidgetItem;
            item->setText(event.name);
            //item->setFlags(Qt::ItemIsUserCheckable);

            item->setFlags(item->flags() | Qt::ItemIsEnabled | Qt::ItemIsSelectable);

            if((event.name!="Level - Start")&&(event.name!="P Switch - Start")&&(event.name!="P Switch - End"))
                item->setFlags(item->flags() | Qt::ItemIsEditable | Qt::ItemIsDragEnabled);

            item->setData(Qt::UserRole, QString::number( event.array_id ) );
            ui->LVLEvents_List->addItem( item );
        }
        on_LVLEvents_List_itemSelectionChanged();
    }
}

void LvlEventsBox::EventListsSync()
{
    mw()->EventListsSync();
}

void MainWindow::EventListsSync()
{
    dock_LvlItemProps->hide();
    dock_LvlItemProps->LvlItemPropsLock=true;
    dock_LvlEvents->lockSetEventSettings=true;

    QComboBox * _ip_block_d = dock_LvlItemProps->cbox_event_block_dest();
    QComboBox * _ip_block_h = dock_LvlItemProps->cbox_event_block_hit();
    QComboBox * _ip_block_l = dock_LvlItemProps->cbox_event_block_le();

    QComboBox * _ip_npc_a = dock_LvlItemProps->cbox_event_npc_act();
    QComboBox * _ip_npc_d = dock_LvlItemProps->cbox_event_npc_die();
    QComboBox * _ip_npc_t = dock_LvlItemProps->cbox_event_npc_talk();
    QComboBox * _ip_npc_l = dock_LvlItemProps->cbox_event_npc_le();

    QComboBox * _sb_block_d = dock_LvlSearchBox->cbox_event_block_dest();
    QComboBox * _sb_block_h = dock_LvlSearchBox->cbox_event_block_hit();
    QComboBox * _sb_block_l = dock_LvlSearchBox->cbox_event_block_le();

    QComboBox * _sb_npc_a = dock_LvlSearchBox->cbox_event_npc_activate();
    QComboBox * _sb_npc_d = dock_LvlSearchBox->cbox_event_npc_death();
    QComboBox * _sb_npc_t = dock_LvlSearchBox->cbox_event_npc_talk();
    QComboBox * _sb_npc_el = dock_LvlSearchBox->cbox_event_npc_empty_layer();

    QComboBox * _eb_trigger = dock_LvlEvents->cbox_event_trigger();

    QString curDestroyedBlock  = _sb_block_d->currentText();
    QString curHitedBlock      = _sb_block_h->currentText();
    QString curLayerEmptyBlock = _sb_block_l->currentText();

    QString curActivateNpc = _sb_npc_a->currentText();
    QString curDeathNpc = _sb_npc_d->currentText();
    QString curTalkNpc = _sb_npc_t->currentText();
    QString curEmptyLayerNpc = _sb_npc_el->currentText();

    int WinType = activeChildWindow();

    _ip_block_d->clear();
    _ip_block_h->clear();
    _ip_block_l->clear();

    _ip_npc_a->clear();
    _ip_npc_d->clear();
    _ip_npc_t->clear();
    _ip_npc_l->clear();
    _eb_trigger->clear();

    _sb_block_d->clear();
    _sb_block_h->clear();
    _sb_block_l->clear();

    _sb_npc_a->clear();
    _sb_npc_d->clear();
    _sb_npc_t->clear();
    _sb_npc_el->clear();

    QString noEvent = tr("[None]");
    _ip_block_d->addItem(noEvent);
    _ip_block_h->addItem(noEvent);
    _ip_block_l->addItem(noEvent);

    _ip_npc_a->addItem(noEvent);
    _ip_npc_d->addItem(noEvent);
    _ip_npc_t->addItem(noEvent);
    _ip_npc_l->addItem(noEvent);
    _eb_trigger->addItem(noEvent);

    _sb_npc_a->addItem(noEvent);
    _sb_npc_d->addItem(noEvent);
    _sb_npc_t->addItem(noEvent);
    _sb_npc_el->addItem(noEvent);

    if (WinType==1)
    {
        foreach(LevelSMBX64Event event, activeLvlEditWin()->LvlData.events)
        {
            _ip_block_d->addItem(event.name);
            _ip_block_h->addItem(event.name);
            _ip_block_l->addItem(event.name);

            _ip_npc_a->addItem(event.name);
            _ip_npc_d->addItem(event.name);
            _ip_npc_t->addItem(event.name);
            _ip_npc_l->addItem(event.name);
            _eb_trigger->addItem(event.name);

            _sb_block_d->addItem(event.name);
            _sb_block_h->addItem(event.name);
            _sb_block_l->addItem(event.name);

            _sb_npc_a->addItem(event.name);
            _sb_npc_d->addItem(event.name);
            _sb_npc_t->addItem(event.name);
            _sb_npc_el->addItem(event.name);
        }
    }

    _sb_block_d->setCurrentText(curDestroyedBlock);
    _sb_block_h->setCurrentText(curHitedBlock);
    _sb_block_l->setCurrentText(curLayerEmptyBlock);

    _sb_npc_a->setCurrentText(curActivateNpc);
    _sb_npc_d->setCurrentText(curDeathNpc);
    _sb_npc_t->setCurrentText(curTalkNpc);
    _sb_npc_el->setCurrentText(curEmptyLayerNpc);

    dock_LvlEvents->lockSetEventSettings=false;

    if(dock_LvlEvents->currentEventArrayID>0)
        dock_LvlEvents->setEventData(dock_LvlEvents->currentEventArrayID);
}

void LvlEventsBox::reloadSoundsList()
{
    lockSetEventSettings=true;
    ui->LVLEvent_Cmn_PlaySnd->clear();
    ui->LVLEvent_Cmn_PlaySnd->addItem( tr("[Silence]"), "0" );

    if(mw()->configs.check()) return;

    foreach(obj_sound snd, mw()->configs.main_sound )
    {
        if(!snd.hidden)
            ui->LVLEvent_Cmn_PlaySnd->addItem(snd.name, QString::number(snd.id));
    }

    lockSetEventSettings=false;
}


void LvlEventsBox::setEventData(long index)
{
    lockSetEventSettings=true;
    long cIndex=index;
    bool found=false;

    if(index==-1) //Force reset current event data
        ui->LVLEvents_List->clearSelection();
    else
    if(index==-2) //Refresh current event data
        {
        if(!ui->LVLEvents_List->selectedItems().isEmpty())
            cIndex = ui->LVLEvents_List->currentItem()->data(Qt::UserRole).toInt();
        else
            {
                cIndex = currentEventArrayID;
                for(int q=0; q<ui->LVLEvents_List->count();q++) //Select if not selected
                {
                    if(ui->LVLEvents_List->item(q)->data(Qt::UserRole).toInt()==cIndex)
                    {
                        ui->LVLEvents_List->item(q)->setSelected(true);
                        break;
                    }
                }
            }
        }


    int WinType = mw()->activeChildWindow();
    if (WinType==1)
    {
        LevelEdit * edit = mw()->activeLvlEditWin();
        if(!edit) return;

        if( (edit->LvlData.events.size() > 0) && (cIndex >= 0))
        {
            currentEventArrayID=cIndex;
            foreach(LevelSMBX64Event event, edit->LvlData.events)
            {
                if(event.array_id == (unsigned int)cIndex)
                {
                    currentEventArrayID=event.array_id;

                    //Enable controls
                    ui->LVLEvents_Settings->setEnabled(true);
                    ui->LVLEvent_AutoStart->setEnabled(true);

                    //Set controls data
                    ui->LVLEvent_AutoStart->setChecked( event.autostart );

                    //Layers visibly - layerList
                    ui->LVLEvent_disableSmokeEffect->setChecked( event.nosmoke );
                    eventLayerVisiblySyncList();

                    // Layer Movement
                    ui->LVLEvent_LayerMov_List->setCurrentIndex(0);
                    for(int i=0; i< ui->LVLEvent_LayerMov_List->count(); i++)
                    {
                        if(ui->LVLEvent_LayerMov_List->itemText(i)==event.movelayer)
                        {
                            ui->LVLEvent_LayerMov_List->setCurrentIndex(i);
                            break;
                        }
                    }
                    ui->LVLEvent_LayerMov_spX->setValue(event.layer_speed_x);
                    ui->LVLEvent_LayerMov_spY->setValue(event.layer_speed_y);

                    //Scroll section / Move Camera
                    ui->LVLEvent_Scroll_Sct->setMaximum( edit->LvlData.sections.size() );
                    ui->LVLEvent_Scroll_Sct->setValue( event.scroll_section+1 );
                    ui->LVLEvent_Scroll_spX->setValue( event.move_camera_x );
                    ui->LVLEvent_Scroll_spY->setValue( event.move_camera_y );

                    //Section Settings
                    ui->LVLEvent_Sct_list->clear();
                    for(int i=0; i<edit->LvlData.sections.size(); i++)
                        ui->LVLEvent_Sct_list->addItem(tr("Section")+QString(" ")+QString::number(i+1), QString::number(i));

                    for(int i=0; i<ui->LVLEvent_Sct_list->count(); i++)
                    {
                        if(ui->LVLEvent_Sct_list->itemData(i).toInt()==edit->LvlData.CurSection)
                        {
                            ui->LVLEvent_Sct_list->setCurrentIndex(i);
                            curSectionField = i;
                            break;
                        }
                    }

                    eventSectionSettingsSync();


                    //Common settings
                    QString evnmsg = (event.msg.isEmpty() ? tr("[none]") : event.msg);
                    if(evnmsg.size()>20)
                    {
                        evnmsg.resize(18);
                        evnmsg.push_back("...");
                    }
                    ui->LVLEvent_Cmn_Msg->setText( evnmsg.replace("&", "&&&") );

                    ui->LVLEvent_Cmn_PlaySnd->setCurrentIndex(0);
                    for(int i=0; i<ui->LVLEvent_Cmn_PlaySnd->count(); i++)
                    {
                        if(ui->LVLEvent_Cmn_PlaySnd->itemData(i).toInt() == event.sound_id)
                        {
                            ui->LVLEvent_Cmn_PlaySnd->setCurrentIndex(i);
                            break;
                        }
                    }

                    if(event.end_game<ui->LVLEvent_Cmn_EndGame->count())
                        ui->LVLEvent_Cmn_EndGame->setCurrentIndex(event.end_game);

                    //Player Control key hold
                    ui->LVLEvent_Key_AltJump->setChecked(event.ctrl_altjump);
                    ui->LVLEvent_Key_AltRun->setChecked(event.ctrl_altrun);
                    ui->LVLEvent_Key_Jump->setChecked(event.ctrl_jump);
                    ui->LVLEvent_Key_Run->setChecked(event.ctrl_run);
                    ui->LVLEvent_Key_Start->setChecked(event.ctrl_start);
                    ui->LVLEvent_Key_Drop->setChecked(event.ctrl_drop);
                    ui->LVLEvent_Key_Left->setChecked(event.ctrl_left);
                    ui->LVLEvent_Key_Right->setChecked(event.ctrl_right);
                    ui->LVLEvent_Key_Up->setChecked(event.ctrl_up);
                    ui->LVLEvent_Key_Down->setChecked(event.ctrl_down);

                    //Trigger Event
                    ui->LVLEvent_TriggerEvent->setCurrentIndex(0);
                    for(int i=0; i<ui->LVLEvent_TriggerEvent->count(); i++)
                    {
                        if(ui->LVLEvent_TriggerEvent->itemText(i)== event.trigger)
                        {
                            ui->LVLEvent_TriggerEvent->setCurrentIndex(i);
                            break;
                        }
                    }
                    ui->LVLEvent_TriggerDelay->setValue( qreal(event.trigger_timer)/10 );

                    refreshShownTabs(event);

                    found=true;
                    break;
                }
            }
        }

        if(!found)
        {
            ui->LVLEvents_Settings->setEnabled(false);
            ui->LVLEvent_AutoStart->setEnabled(false);
        }

        currentEventArrayID=cIndex;
    }
    lockSetEventSettings=false;

}

void LvlEventsBox::refreshShownTabs(LevelSMBX64Event event, bool hideAll)
{
    ui->layerVisibility_toggleBox->setChecked(false);
    ui->layerVizible_group->setVisible(false);
    ui->LayerMovement_toggleBox->setChecked(false);
    ui->layerMovement_group->setVisible(false);
    ui->AutoscrollSection_toggleBox->setChecked(false);
    ui->Autoscroll_Area->setVisible(false);
    ui->SectionSettings_toggleBox->setChecked(false);
    ui->SectionSettings_area->setVisible(false);
    ui->Common_toggleBox->setChecked(false);
    ui->common_area->setVisible(false);
    ui->playerControl_toggleBox->setChecked(false);
    ui->playerControl_area->setVisible(false);
    ui->triggerEvent_toggleBox->setChecked(false);
    ui->triggerEvent_area->setVisible(false);

    if(!hideAll)
    {
        bool opened=false;
        if(!event.layers_hide.isEmpty() || !event.layers_show.isEmpty() || !event.layers_toggle.isEmpty())
        {
            ui->layerVisibility_toggleBox->setChecked(true);
            ui->layerVizible_group->setVisible(true);
            opened=true;
        }
        if(!event.movelayer.isEmpty())
        {
            ui->LayerMovement_toggleBox->setChecked(true);
            ui->layerMovement_group->setVisible(true);
            opened=true;
        }
        if( ( event.move_camera_x!=0.0f ) || (event.move_camera_y!=0.0f) )
        {
            ui->AutoscrollSection_toggleBox->setChecked(true);
            ui->Autoscroll_Area->setVisible(true);
            opened=true;
        }
        for(int i=0;i<event.sets.size();i++)
        {
            if( (event.sets[i].background_id!=-1) ||
               (event.sets[i].music_id!=-1) ||
               (event.sets[i].position_left!=-1) )
            {
                ui->SectionSettings_toggleBox->setChecked(true);
                ui->SectionSettings_area->setVisible(true);
                opened=true;
                break;
            }
        }
        if( (!event.msg.isEmpty()) || (event.sound_id>0) || (event.end_game>0) )
        {
            ui->Common_toggleBox->setChecked(true);
            ui->common_area->setVisible(true);
            opened=true;
        }
        if(event.ctrl_left||event.ctrl_right||event.ctrl_up||event.ctrl_down||
                event.ctrl_run||event.ctrl_altrun||event.ctrl_jump||event.ctrl_altjump||
                event.ctrl_drop||event.ctrl_start)
        {
            ui->playerControl_toggleBox->setChecked(true);
            ui->playerControl_area->setVisible(true);
            opened=true;
        }
        if(!event.trigger.isEmpty())
        {
            ui->triggerEvent_toggleBox->setChecked(true);
            ui->triggerEvent_area->setVisible(true);
            opened=true;
        }
        if(!opened)
        {
            ui->layerVisibility_toggleBox->setChecked(GlobalSettings::LvlItemDefaults.classicevents_tabs_layviz);
            ui->layerVizible_group->setVisible(GlobalSettings::LvlItemDefaults.classicevents_tabs_layviz);
            ui->LayerMovement_toggleBox->setChecked(GlobalSettings::LvlItemDefaults.classicevents_tabs_laymov);
            ui->layerMovement_group->setVisible(GlobalSettings::LvlItemDefaults.classicevents_tabs_laymov);
            ui->AutoscrollSection_toggleBox->setChecked(GlobalSettings::LvlItemDefaults.classicevents_tabs_autoscroll);
            ui->Autoscroll_Area->setVisible(GlobalSettings::LvlItemDefaults.classicevents_tabs_autoscroll);
            ui->SectionSettings_toggleBox->setChecked(GlobalSettings::LvlItemDefaults.classicevents_tabs_secset);
            ui->SectionSettings_area->setVisible(GlobalSettings::LvlItemDefaults.classicevents_tabs_secset);
            ui->Common_toggleBox->setChecked(GlobalSettings::LvlItemDefaults.classicevents_tabs_common);
            ui->common_area->setVisible(GlobalSettings::LvlItemDefaults.classicevents_tabs_common);
            ui->playerControl_toggleBox->setChecked(GlobalSettings::LvlItemDefaults.classicevents_tabs_buttons);
            ui->playerControl_area->setVisible(GlobalSettings::LvlItemDefaults.classicevents_tabs_buttons);
            ui->triggerEvent_toggleBox->setChecked(GlobalSettings::LvlItemDefaults.classicevents_tabs_trigger);
            ui->triggerEvent_area->setVisible(GlobalSettings::LvlItemDefaults.classicevents_tabs_trigger);
        }
    }
}

bool LvlEventsBox::eventIsExist(QString evt)
{
    LevelEdit * edit = mw()->activeLvlEditWin();
    if(!edit) return false;

    for(int i=0; i<edit->LvlData.events.size(); i++)
    {
        if( edit->LvlData.events[i].name == evt )
        {
            return true;
            break;
        }
    }
    return false;
}


void LvlEventsBox::eventSectionSettingsSync()
{
    int WinType = mw()->activeChildWindow();

    if (WinType==1)
    {
        LevelEdit * edit = mw()->activeLvlEditWin();
        if(!edit) return;

        long i = getEventArrayIndex();
        if(i<0) return;

        int sectionID = ui->LVLEvent_Sct_list->currentData().toInt();

        if(sectionID<edit->LvlData.events[i].sets.size())
        {
            ui->LVLEvent_SctSize_left->setText("");
            ui->LVLEvent_SctSize_top->setText("");
            ui->LVLEvent_SctSize_bottom->setText("");
            ui->LVLEvent_SctSize_right->setText("");
            ui->LVLEvent_SctSize_left->setEnabled(false);
            ui->LVLEvent_SctSize_top->setEnabled(false);
            ui->LVLEvent_SctSize_bottom->setEnabled(false);
            ui->LVLEvent_SctSize_right->setEnabled(false);
            ui->LVLEvent_SctSize_Set->setEnabled(false);

        switch(edit->LvlData.events[i].sets[sectionID].position_left)
            {
            case -1:
                ui->LVLEvent_SctSize_none->setChecked(true);
                break;
            case -2:
                ui->LVLEvent_SctSize_reset->setChecked(true);
                break;
            default:
                ui->LVLEvent_SctSize_define->setChecked(true);
                lockEventSectionDataList=true;
                ui->LVLEvent_SctSize_left->setText(QString::number(edit->LvlData.events[i].sets[sectionID].position_left));
                ui->LVLEvent_SctSize_top->setText(QString::number(edit->LvlData.events[i].sets[sectionID].position_top));
                ui->LVLEvent_SctSize_bottom->setText(QString::number(edit->LvlData.events[i].sets[sectionID].position_bottom));
                ui->LVLEvent_SctSize_right->setText(QString::number(edit->LvlData.events[i].sets[sectionID].position_right));
                ui->LVLEvent_SctSize_left->setEnabled(true);
                ui->LVLEvent_SctSize_top->setEnabled(true);
                ui->LVLEvent_SctSize_bottom->setEnabled(true);
                ui->LVLEvent_SctSize_right->setEnabled(true);
                ui->LVLEvent_SctSize_Set->setEnabled(true);
                break;
            }

        ui->LVLEvent_SctMus_List->setEnabled(false);
        long musicID = edit->LvlData.events[i].sets[sectionID].music_id;
        switch(musicID)
            {
            case -1:
                ui->LVLEvent_SctMus_none->setChecked(true);
                break;
            case -2:
                ui->LVLEvent_SctMus_reset->setChecked(true);
                break;
            default:
                ui->LVLEvent_SctMus_define->setChecked(true);
                ui->LVLEvent_SctMus_List->setEnabled(true);

                lockEventSectionDataList=true;
                ui->LVLEvent_SctMus_List->setCurrentIndex(0);
                for(int q=0; q<ui->LVLEvent_SctMus_List->count(); q++)
                {
                    if(ui->LVLEvent_SctMus_List->itemData(q).toInt()==musicID)
                    {
                        ui->LVLEvent_SctMus_List->setCurrentIndex(q);
                        break;
                    }
                }
                break;
            }

        ui->LVLEvent_SctBg_List->setEnabled(false);
        long backgrndID = edit->LvlData.events[i].sets[sectionID].background_id;
        switch(backgrndID)
            {
            case -1:
                ui->LVLEvent_SctBg_none->setChecked(true);
                break;
            case -2:
                ui->LVLEvent_SctBg_reset->setChecked(true);
                break;
            default:
                ui->LVLEvent_SctBg_define->setChecked(true);
                ui->LVLEvent_SctBg_List->setEnabled(true);

                lockEventSectionDataList=true;
                ui->LVLEvent_SctBg_List->setCurrentIndex(0);
                for(int q=0; q<ui->LVLEvent_SctBg_List->count(); q++)
                {
                    if(ui->LVLEvent_SctBg_List->itemData(q).toInt()==backgrndID)
                    {
                        ui->LVLEvent_SctBg_List->setCurrentIndex(q);
                        break;
                    }
                }
                break;
            }
        }
    }

    lockEventSectionDataList=false;
}

void LvlEventsBox::eventLayerVisiblySyncList()
{
    int WinType = mw()->activeChildWindow();

    if (WinType==1)
    {
        LevelEdit * edit = mw()->activeLvlEditWin();
        if(!edit) return;

        long i = getEventArrayIndex();
        if(i<0) return;

        LevelSMBX64Event event = edit->LvlData.events[i];

        util::memclear(ui->LVLEvents_layerList);
        util::memclear(ui->LVLEvent_Layer_HideList);
        util::memclear(ui->LVLEvent_Layer_ShowList);
        util::memclear(ui->LVLEvent_Layer_ToggleList);

        QListWidgetItem * item;
        //Total layers list
        foreach(LevelLayer layer, edit->LvlData.layers)
        {
            item = new QListWidgetItem;
            item->setText(layer.name);
            item->setFlags(item->flags() | Qt::ItemIsEnabled | Qt::ItemIsSelectable);
            item->setData(Qt::UserRole, QString::number( layer.array_id ) );
            ui->LVLEvents_layerList->addItem( item );
        }

        //Hidden layers
        foreach(QString layer, event.layers_hide)
        {
            item = new QListWidgetItem;
            item->setText(layer);
            item->setFlags(item->flags() | Qt::ItemIsEnabled | Qt::ItemIsSelectable);
            QList<QListWidgetItem *> items =
                  ui->LVLEvents_layerList->findItems(QString("*"), Qt::MatchWrap | Qt::MatchWildcard);
            foreach(QListWidgetItem *item, items)
            {
                if(item->text()==layer)
                { delete item; break;}
            }
            ui->LVLEvent_Layer_HideList->addItem( item );
        }

        //Showed layers
        foreach(QString layer, event.layers_show)
        {
            item = new QListWidgetItem;
            item->setText(layer);
            item->setFlags(item->flags() | Qt::ItemIsEnabled | Qt::ItemIsSelectable);
            QList<QListWidgetItem *> items =
                  ui->LVLEvents_layerList->findItems(QString("*"), Qt::MatchWrap | Qt::MatchWildcard);
            foreach(QListWidgetItem *item, items)
            {
                if(item->text()==layer)
                { delete item; break;}
            }
            ui->LVLEvent_Layer_ShowList->addItem( item );
        }

        //Toggeled layers
        foreach(QString layer, event.layers_toggle)
        {
            item = new QListWidgetItem;
            item->setText(layer);
            item->setFlags(item->flags() | Qt::ItemIsEnabled | Qt::ItemIsSelectable);
            QList<QListWidgetItem *> items =
                  ui->LVLEvents_layerList->findItems(QString("*"), Qt::MatchWrap | Qt::MatchWildcard);
            foreach(QListWidgetItem *item, items)
            {
                if(item->text()==layer)
                { delete item; break;}
            }
            ui->LVLEvent_Layer_ToggleList->addItem( item );
        }
    }
}

void LvlEventsBox::on_LVLEvents_List_itemSelectionChanged()
{
    if(ui->LVLEvents_List->selectedItems().isEmpty())
    {
        setEventData(-1);
    }
    else
        setEventData(ui->LVLEvents_List->currentItem()->data(Qt::UserRole).toInt());

}

void LvlEventsBox::on_LVLEvents_List_itemChanged(QListWidgetItem *item)
{
    if(lockSetEventSettings || LvlEventBoxLock) return;
    lockSetEventSettings=true;
    int WinType = mw()->activeChildWindow();

    if (WinType==1)
    {
        LevelEdit * edit = mw()->activeLvlEditWin();
        if(!edit) return;

        QString eventName = item->text();
        QString oldEventName = item->text();

        ModifyEventItem(item, oldEventName, eventName);
        edit->LvlData.modified=true;
    }//if WinType==1
    EventListsSync();
    lockSetEventSettings=false;
}

void LvlEventsBox::DragAndDroppedEvent(QModelIndex /*sourceParent*/,int sourceStart, int sourceEnd,QModelIndex /*destinationParent*/,int destinationRow)
{
    lockSetEventSettings=true;
    WriteToLog(QtDebugMsg, "Row Change at " + QString::number(sourceStart) +
               " " + QString::number(sourceEnd) +
               " to " + QString::number(destinationRow));

    int WinType = mw()->activeChildWindow();
    if(WinType==1)
    {
        LevelEdit * edit = mw()->activeLvlEditWin();
        if(!edit) return;

        LevelSMBX64Event buffer;
        if(sourceStart < edit->LvlData.events.size())
        {
            buffer = edit->LvlData.events[sourceStart];
            edit->LvlData.events.removeAt(sourceStart);
            edit->LvlData.events.insert(((destinationRow>sourceStart)?destinationRow-1:destinationRow), buffer);
        }
    }
    lockSetEventSettings=false;
    EventListsSync();  //Sync comboboxes in properties
}

long LvlEventsBox::getEventArrayIndex()
{
    if(mw()->activeChildWindow()!=1) return -2;

    LevelEdit * edit = mw()->activeLvlEditWin();
    if(!edit) return -2;

    bool found=false;
    long i;
    for(i=0; i< edit->LvlData.events.size();i++)
    {
        if((unsigned long)currentEventArrayID==edit->LvlData.events[i].array_id)
        {
            found=true;
            break;
        }
    }
    if(!found)
        return -1;
    else
        return i;
}


void LvlEventsBox::AddNewEvent(QString eventName, bool setEdited)
{
    lockSetEventSettings=true;

    if(mw()->activeChildWindow()!=1) return;

    LevelEdit * edit = mw()->activeLvlEditWin();
    if(!edit) return;

    QListWidgetItem * item;
    item = new QListWidgetItem;
    item->setText(eventName);
    item->setFlags(Qt::ItemIsEditable);
    item->setFlags(item->flags() | Qt::ItemIsEnabled);
    item->setFlags(item->flags() | Qt::ItemIsEditable | Qt::ItemIsDragEnabled | Qt::ItemIsSelectable);
    //item->setData(Qt::UserRole, QString("NewEvent") );

    ui->LVLEvents_List->addItem( item );

    if(eventIsExist(item->text()))
    {
        delete item;
        lockSetEventSettings=false;
        cloneEvent=false;//Reset state
        return;
    }
    else
    {
        LevelSMBX64Event NewEvent = FileFormats::CreateLvlEvent();
        if(cloneEvent)
        {
            bool found=false;
            long i;
            for(i=0; i< edit->LvlData.events.size();i++)
            {
                if((unsigned long)cloneEventId==edit->LvlData.events[i].array_id)
                {
                    found=true;
                    break;
                }
            }
            if(found) NewEvent = edit->LvlData.events[i];
        }

        NewEvent.name = item->text();
        edit->LvlData.events_array_id++;
        NewEvent.array_id = edit->LvlData.events_array_id;
        item->setData(Qt::UserRole, QString::number(NewEvent.array_id));

        if(!cloneEvent)
            edit->scene->addAddEventHistory(NewEvent);
        else
            edit->scene->addDuplicateEventHistory(NewEvent);
        edit->LvlData.events.push_back(NewEvent);
        edit->LvlData.modified=true;
        cloneEvent=false;//Reset state

        if(setEdited)
        {
            ui->LVLEvents_List->setFocus();
            ui->LVLEvents_List->scrollToItem( item );
            ui->LVLEvents_List->editItem( item );
        }
    }
    lockSetEventSettings=false;
    EventListsSync();  //Sync comboboxes in properties
}

void LvlEventsBox::ModifyEventItem(QListWidgetItem *item, QString oldEventName, QString newEventName)
{
    lockSetEventSettings=true;
    //Find layer enrty in array and apply settings
    LevelEdit * edit = mw()->activeLvlEditWin();
    if(!edit) return;

    for(int i=0; i < edit->LvlData.events.size(); i++)
    {
        if( edit->LvlData.events[i].array_id==(unsigned int)item->data(Qt::UserRole).toInt() )
        {
            int l=0;
            bool exist=false;

            oldEventName = edit->LvlData.events[i].name;

            if(newEventName.isEmpty())
            {   //Discard change to empty
                lockSetEventSettings=true;
                   item->setText(oldEventName);
                lockSetEventSettings=false;
                return;
            }

            edit->scene->addRenameEventHistory(edit->LvlData.events[i].array_id, oldEventName, newEventName);

            if(oldEventName!=newEventName)
            {
                //Check for exists item equal to new item
                for(l=0;l<edit->LvlData.events.size();l++)
                {
                    if(edit->LvlData.events[l].name==newEventName)
                    {
                        exist=true;
                        break;
                    }
                }
            }
            if(exist)
            {   //Discard change to exist event
                lockSetEventSettings=true;
                   item->setText(oldEventName);
                lockSetEventSettings=false;
                return;
            }

            edit->LvlData.events[i].name = newEventName;
            break;
        }
    }

    lockSetEventSettings=false;
    //Update event name in items
    ModifyEvent(oldEventName, newEventName);
    EventListsSync();  //Sync comboboxes in properties
}

void LvlEventsBox::ModifyEvent(QString eventName, QString newEventName)
{
    //Apply layer's name to all items
    LevelEdit * edit = mw()->activeLvlEditWin();
    if(!edit) return;

    QList<QGraphicsItem*> ItemList = edit->scene->items();

    for (QList<QGraphicsItem*>::iterator it = ItemList.begin(); it != ItemList.end(); it++)
    {
        if((*it)->data(25).toString()=="CURSOR") continue; //skip cursor item

        if((*it)->data(0).toString()=="Block")
        {
            bool isMod=false;
            if( ((ItemBlock *)(*it))->blockData.event_destroy ==  eventName)
                {((ItemBlock *)(*it))->blockData.event_destroy = newEventName; isMod=true;}
            if( ((ItemBlock *)(*it))->blockData.event_hit ==  eventName)
                {((ItemBlock *)(*it))->blockData.event_hit = newEventName; isMod=true;}
            if( ((ItemBlock *)(*it))->blockData.event_emptylayer ==  eventName)
                {((ItemBlock *)(*it))->blockData.event_emptylayer = newEventName; isMod=true;}
            if(isMod){ ((ItemBlock *)(*it))->arrayApply(); }
        }
        else
        if((*it)->data(0).toString()=="NPC")
        {
            bool isMod=false;
            if( ((ItemNPC *)(*it))->npcData.event_activate ==  eventName)
                {((ItemNPC *)(*it))->npcData.event_activate = newEventName; isMod=true;}
            if( ((ItemNPC *)(*it))->npcData.event_die ==  eventName)
                {((ItemNPC *)(*it))->npcData.event_die = newEventName; isMod=true;}
            if( ((ItemNPC *)(*it))->npcData.event_talk ==  eventName)
                {((ItemNPC *)(*it))->npcData.event_talk = newEventName; isMod=true;}
            if( ((ItemNPC *)(*it))->npcData.event_emptylayer ==  eventName)
                {((ItemNPC *)(*it))->npcData.event_emptylayer = newEventName; isMod=true;}
            if(isMod) {((ItemNPC *)(*it))->arrayApply();}
        }
    }
    for(int i=0; i < edit->LvlData.events.size(); i++)
    {
        if( edit->LvlData.events[i].trigger == eventName)
            edit->LvlData.events[i].trigger = newEventName;
    }
}

QListWidget *LvlEventsBox::getEventList()
{
    return ui->LVLEvents_List;
}

void LvlEventsBox::setEventToolsLocked(bool locked)
{
    lockSetEventSettings = locked;
}

void LvlEventsBox::RemoveEvent(QString eventName)
{
    if(eventName.isEmpty()) return;
    //dummy
}


void LvlEventsBox::on_LVLEvents_add_clicked()
{
    newEventCounter=1;
    QString newEventName = tr("New Event %1");
    while(eventIsExist(newEventName.arg(newEventCounter)))
        newEventCounter++;
    AddNewEvent(newEventName.arg(newEventCounter), true);
}

void LvlEventsBox::on_LVLEvents_del_clicked()
{
    if(ui->LVLEvents_List->selectedItems().isEmpty()) return;

    if(ui->LVLEvents_List->selectedItems()[0]->text()=="Level - Start") return;
    if(ui->LVLEvents_List->selectedItems()[0]->text()=="P Switch - Start") return;
    if(ui->LVLEvents_List->selectedItems()[0]->text()=="P Switch - End") return;

    int WinType = mw()->activeChildWindow();
    if (WinType==1)
    {
        LevelEdit * edit = mw()->activeLvlEditWin();
        if(!edit) return;

        for(int i=0;i< edit->LvlData.events.size(); i++)
        {
            if( edit->LvlData.events[i].array_id==
                    (unsigned int)ui->LVLEvents_List->selectedItems()[0]->data(Qt::UserRole).toInt() )
            {
                edit->scene->addRemoveEventHistory(edit->LvlData.events[i]);
                ModifyEvent(edit->LvlData.events[i].name, "");
                edit->LvlData.events.removeAt(i);
                delete ui->LVLEvents_List->selectedItems()[0];
                break;
            }
        }
    }
    EventListsSync();
}


void LvlEventsBox::on_LVLEvents_duplicate_clicked()
{
    if(ui->LVLEvents_List->selectedItems().isEmpty()) return;

    cloneEvent=true;
    cloneEventId=ui->LVLEvents_List->selectedItems()[0]->data(Qt::UserRole).toInt();

    newEventCounter=1;
    QString copiedEventName = tr("Copyed Event %1");
    while(eventIsExist(copiedEventName.arg(newEventCounter)))
        newEventCounter++;

    AddNewEvent(copiedEventName.arg( ui->LVLEvents_List->count()+1 ), true);
}



void LvlEventsBox::on_LVLEvent_AutoStart_clicked(bool checked)
{
    if(lockSetEventSettings || LvlEventBoxLock) return;

    int WinType = mw()->activeChildWindow();

    if (WinType==1)
    {
        LevelEdit * edit = mw()->activeLvlEditWin();
        if(!edit) return;

        long i = getEventArrayIndex();
        if(i<0) return;

        edit->scene->addChangeEventSettingsHistory(edit->LvlData.events[i].array_id, HistorySettings::SETTING_EV_AUTOSTART, QVariant(checked));
        edit->LvlData.events[i].autostart = checked;
        edit->LvlData.modified=true;
    }
}







void LvlEventsBox::on_LVLEvent_disableSmokeEffect_clicked(bool checked)
{
    if(lockSetEventSettings || LvlEventBoxLock) return;

    int WinType = mw()->activeChildWindow();

    if (WinType==1)
    {
        LevelEdit * edit = mw()->activeLvlEditWin();
        if(!edit) return;

        long i = getEventArrayIndex();
        if(i<0) return;

        edit->scene->addChangeEventSettingsHistory(edit->LvlData.events[i].array_id, HistorySettings::SETTING_EV_SMOKE, QVariant(checked));
        edit->LvlData.events[i].nosmoke = checked;
        edit->LvlData.modified=true;
    }

}




void LvlEventsBox::on_LVLEvent_Layer_HideAdd_clicked()
{
    if(lockSetEventSettings || LvlEventBoxLock) return;

    int WinType = mw()->activeChildWindow();

    if (WinType==1)
    {
        LevelEdit * edit = mw()->activeLvlEditWin();
        if(!edit) return;

        long i = getEventArrayIndex();
        if(i<0) return;

        if(!ui->LVLEvents_layerList->selectedItems().isEmpty())
        {
            edit->scene->addChangeEventSettingsHistory(edit->LvlData.events[i].array_id, HistorySettings::SETTING_EV_LHIDEADD, QVariant(ui->LVLEvents_layerList->currentItem()->text()));
            edit->LvlData.events[i].layers_hide.push_back(ui->LVLEvents_layerList->currentItem()->text());
            edit->LvlData.modified=true;
            eventLayerVisiblySyncList();
        }
    }

}

void LvlEventsBox::on_LVLEvent_Layer_HideDel_clicked()
{
    if(lockSetEventSettings || LvlEventBoxLock) return;

    int WinType = mw()->activeChildWindow();

    if (WinType==1)
    {
        LevelEdit * edit = mw()->activeLvlEditWin();
        if(!edit) return;

        long i = getEventArrayIndex();
        if(i<0) return;

        if(!ui->LVLEvent_Layer_HideList->selectedItems().isEmpty())
        {
            for(int j=0; j<edit->LvlData.events[i].layers_hide.size(); j++)
            {
            if(edit->LvlData.events[i].layers_hide[j]==ui->LVLEvent_Layer_HideList->currentItem()->text())
                {
                    edit->scene->addChangeEventSettingsHistory(edit->LvlData.events[i].array_id, HistorySettings::SETTING_EV_LHIDEDEL, QVariant(edit->LvlData.events[i].layers_hide[j]));
                    edit->LvlData.events[i].layers_hide.removeAt(j);
                    edit->LvlData.modified=true;
                    break;
                }
            }
            eventLayerVisiblySyncList();
        }
    }
}

void LvlEventsBox::on_LVLEvent_Layer_ShowAdd_clicked()
{
    if(lockSetEventSettings || LvlEventBoxLock) return;

    int WinType = mw()->activeChildWindow();

    if (WinType==1)
    {
        LevelEdit *edit = mw()->activeLvlEditWin();
        if(!edit) return;

        long i = getEventArrayIndex();
        if(i<0) return;

        if(!ui->LVLEvents_layerList->selectedItems().isEmpty())
        {
            edit->scene->addChangeEventSettingsHistory(edit->LvlData.events[i].array_id, HistorySettings::SETTING_EV_LSHOWADD, QVariant(ui->LVLEvents_layerList->currentItem()->text()));
            edit->LvlData.events[i].layers_show.push_back(ui->LVLEvents_layerList->currentItem()->text());
            edit->LvlData.modified=true;
            eventLayerVisiblySyncList();
        }
    }
}

void LvlEventsBox::on_LVLEvent_Layer_ShowDel_clicked()
{
    if(lockSetEventSettings || LvlEventBoxLock) return;

    int WinType = mw()->activeChildWindow();

    if (WinType==1)
    {
        LevelEdit * edit = mw()->activeLvlEditWin();
        if(!edit) return;

        long i = getEventArrayIndex();
        if(i<0) return;

        if(!ui->LVLEvent_Layer_ShowList->selectedItems().isEmpty())
        {
            for(int j=0; j<edit->LvlData.events[i].layers_show.size(); j++)
            {
            if(edit->LvlData.events[i].layers_show[j]==ui->LVLEvent_Layer_ShowList->currentItem()->text())
                {
                edit->scene->addChangeEventSettingsHistory(edit->LvlData.events[i].array_id, HistorySettings::SETTING_EV_LSHOWDEL, QVariant(edit->LvlData.events[i].layers_show[j]));
                edit->LvlData.events[i].layers_show.removeAt(j);
                edit->LvlData.modified=true;
                break;
                }
            }
            eventLayerVisiblySyncList();
        }
    }

}

void LvlEventsBox::on_LVLEvent_Layer_TogAdd_clicked()
{
    if(lockSetEventSettings || LvlEventBoxLock) return;

    int WinType = mw()->activeChildWindow();

    if (WinType==1)
    {
        LevelEdit * edit = mw()->activeLvlEditWin();
        if(!edit) return;

        long i = getEventArrayIndex();
        if(i<0) return;

        if(!ui->LVLEvents_layerList->selectedItems().isEmpty())
        {
            edit->scene->addChangeEventSettingsHistory(edit->LvlData.events[i].array_id, HistorySettings::SETTING_EV_LTOGADD, QVariant(ui->LVLEvents_layerList->currentItem()->text()));
            edit->LvlData.events[i].layers_toggle.push_back(ui->LVLEvents_layerList->currentItem()->text());
            edit->LvlData.modified=true;
            eventLayerVisiblySyncList();
        }
    }
}

void LvlEventsBox::on_LVLEvent_Layer_TogDel_clicked()
{
    if(lockSetEventSettings || LvlEventBoxLock) return;

    int WinType = mw()->activeChildWindow();

    if (WinType==1)
    {
        LevelEdit * edit = mw()->activeLvlEditWin();
        if(!edit) return;

        long i = getEventArrayIndex();
        if(i<0) return;

        if(!ui->LVLEvent_Layer_ToggleList->selectedItems().isEmpty())
        {
            for(int j=0; j<edit->LvlData.events[i].layers_toggle.size(); j++)
            {
            if(edit->LvlData.events[i].layers_toggle[j]==ui->LVLEvent_Layer_ToggleList->currentItem()->text())
                {
                    edit->scene->addChangeEventSettingsHistory(edit->LvlData.events[i].array_id, HistorySettings::SETTING_EV_LTOGDEL, QVariant(edit->LvlData.events[i].layers_toggle[j]));
                    edit->LvlData.events[i].layers_toggle.removeAt(j);
                    edit->LvlData.modified=true;
                    break;
                }
            }
            eventLayerVisiblySyncList();
        }
    }
}






void LvlEventsBox::on_LVLEvent_LayerMov_List_currentIndexChanged(int index)
{
    if(lockSetEventSettings || LvlEventBoxLock) return;

    if(index<0) return;

    int WinType = mw()->activeChildWindow();

    if (WinType==1)
    {
        LevelEdit * edit = mw()->activeLvlEditWin();
        if(!edit) return;

        long i = getEventArrayIndex();
        if(i<0) return;

        QList<QVariant> movLayerData;
        movLayerData.push_back(edit->LvlData.events[i].movelayer);
        movLayerData.push_back(((index<=0)?"":ui->LVLEvent_LayerMov_List->currentText()));
        edit->scene->addChangeEventSettingsHistory(edit->LvlData.events[i].array_id, HistorySettings::SETTING_EV_MOVELAYER, QVariant(movLayerData));
        edit->LvlData.events[i].movelayer = ((index<=0)?"":ui->LVLEvent_LayerMov_List->currentText());
        edit->LvlData.modified=true;
    }
}

void LvlEventsBox::on_LVLEvent_LayerMov_spX_valueChanged(double arg1)
{
    if(lockSetEventSettings || LvlEventBoxLock) return;

    int WinType = mw()->activeChildWindow();

    if (WinType==1)
    {
        LevelEdit * edit = mw()->activeLvlEditWin();
        if(!edit) return;

        long i = getEventArrayIndex();
        if(i<0) return;

        QList<QVariant> speedData;
        speedData.push_back(edit->LvlData.events[i].layer_speed_x);
        speedData.push_back(arg1);
        edit->scene->addChangeEventSettingsHistory(edit->LvlData.events[i].array_id, HistorySettings::SETTING_EV_SPEEDLAYERX, QVariant(speedData));
        edit->LvlData.events[i].layer_speed_x = arg1;
        edit->LvlData.modified=true;
    }

}

void LvlEventsBox::on_LVLEvent_LayerMov_spY_valueChanged(double arg1)
{
    if(lockSetEventSettings || LvlEventBoxLock) return;

    int WinType = mw()->activeChildWindow();

    if (WinType==1)
    {
        LevelEdit * edit = mw()->activeLvlEditWin();
        if(!edit) return;

        long i = getEventArrayIndex();
        if(i<0) return;

        QList<QVariant> speedData;
        speedData.push_back(edit->LvlData.events[i].layer_speed_y);
        speedData.push_back(arg1);
        edit->scene->addChangeEventSettingsHistory(edit->LvlData.events[i].array_id, HistorySettings::SETTING_EV_SPEEDLAYERY, QVariant(speedData));
        edit->LvlData.events[i].layer_speed_y = arg1;
        edit->LvlData.modified=true;
    }

}







void LvlEventsBox::on_LVLEvent_Scroll_Sct_valueChanged(int arg1)
{
    if(lockSetEventSettings || LvlEventBoxLock) return;

    int WinType = mw()->activeChildWindow();

    if (WinType==1)
    {
        LevelEdit * edit = mw()->activeLvlEditWin();
        if(!edit) return;
        long i = getEventArrayIndex();
        if(i<0) return;

        QList<QVariant> secData;
        secData.push_back((qlonglong)edit->LvlData.events[i].scroll_section);
        secData.push_back(arg1-1);
        edit->scene->addChangeEventSettingsHistory(edit->LvlData.events[i].array_id, HistorySettings::SETTING_EV_AUTOSCRSEC, QVariant(secData));
        edit->LvlData.events[i].scroll_section = arg1-1;
        edit->LvlData.modified=true;
    }
}

void LvlEventsBox::on_LVLEvent_Scroll_spX_valueChanged(double arg1)
{
    if(lockSetEventSettings || LvlEventBoxLock) return;

    int WinType = mw()->activeChildWindow();

    if (WinType==1)
    {
        LevelEdit * edit = mw()->activeLvlEditWin();
        if(!edit) return;

        long i = getEventArrayIndex();
        if(i<0) return;

        QList<QVariant> scrollXData;
        scrollXData.push_back(edit->LvlData.events[i].move_camera_x);
        scrollXData.push_back(arg1);
        edit->scene->addChangeEventSettingsHistory(edit->LvlData.events[i].array_id, HistorySettings::SETTING_EV_AUTOSCRX, QVariant(scrollXData));
        edit->LvlData.events[i].move_camera_x = arg1;
        edit->LvlData.modified=true;
    }

}

void LvlEventsBox::on_LVLEvent_Scroll_spY_valueChanged(double arg1)
{
    if(lockSetEventSettings || LvlEventBoxLock) return;

    int WinType = mw()->activeChildWindow();

    if (WinType==1)
    {
        LevelEdit * edit = mw()->activeLvlEditWin();
        if(!edit) return;

        long i = getEventArrayIndex();
        if(i<0) return;

        QList<QVariant> scrollYData;
        scrollYData.push_back(edit->LvlData.events[i].move_camera_y);
        scrollYData.push_back(arg1);
        edit->scene->addChangeEventSettingsHistory(edit->LvlData.events[i].array_id, HistorySettings::SETTING_EV_AUTOSCRY, QVariant(scrollYData));
        edit->LvlData.events[i].move_camera_y = arg1;
        edit->LvlData.modified=true;
    }
}




void LvlEventsBox::on_LVLEvent_Sct_list_currentIndexChanged(int index)
{
    if(lockSetEventSettings || LvlEventBoxLock) return;
    if(index<0) return;
    curSectionField = index;
    eventSectionSettingsSync();
}

void LvlEventsBox::on_LVLEvent_SctSize_none_clicked()
{
    if(lockSetEventSettings || LvlEventBoxLock) return;

    int WinType = mw()->activeChildWindow();

    if (WinType==1)
    {
        LevelEdit * edit = mw()->activeLvlEditWin();
        if(!edit) return;
        lockEventSectionDataList=true;
        long i = getEventArrayIndex();
        if(i<0) return;

        QList<QVariant> sizeData;
        sizeData.push_back((qlonglong)curSectionField);
        sizeData.push_back((qlonglong)edit->LvlData.events[i].sets[curSectionField].position_top);
        sizeData.push_back((qlonglong)edit->LvlData.events[i].sets[curSectionField].position_right);
        sizeData.push_back((qlonglong)edit->LvlData.events[i].sets[curSectionField].position_bottom);
        sizeData.push_back((qlonglong)edit->LvlData.events[i].sets[curSectionField].position_left);
        sizeData.push_back((qlonglong)0);
        sizeData.push_back((qlonglong)0);
        sizeData.push_back((qlonglong)0);
        sizeData.push_back((qlonglong)-1);
        edit->scene->addChangeEventSettingsHistory(edit->LvlData.events[i].array_id, HistorySettings::SETTING_EV_SECSIZE, QVariant(sizeData));

        ui->LVLEvent_SctSize_left->setText("");
        ui->LVLEvent_SctSize_top->setText("");
        ui->LVLEvent_SctSize_bottom->setText("");
        ui->LVLEvent_SctSize_right->setText("");
        ui->LVLEvent_SctSize_left->setEnabled(false);
        ui->LVLEvent_SctSize_top->setEnabled(false);
        ui->LVLEvent_SctSize_bottom->setEnabled(false);
        ui->LVLEvent_SctSize_right->setEnabled(false);
        ui->LVLEvent_SctSize_Set->setEnabled(false);

        edit->LvlData.events[i].sets[curSectionField].position_left = -1;
        edit->LvlData.events[i].sets[curSectionField].position_right = 0;
        edit->LvlData.events[i].sets[curSectionField].position_bottom = 0;
        edit->LvlData.events[i].sets[curSectionField].position_top = 0;
        edit->LvlData.modified=true;
    }
    lockEventSectionDataList=false;

}

void LvlEventsBox::on_LVLEvent_SctSize_reset_clicked()
{
    if(lockSetEventSettings || LvlEventBoxLock) return;

    int WinType = mw()->activeChildWindow();

    if (WinType==1)
    {
        LevelEdit * edit = mw()->activeLvlEditWin();
        if(!edit) return;

        lockEventSectionDataList=true;
        long i = getEventArrayIndex();
        if(i<0) return;

        QList<QVariant> sizeData;
        sizeData.push_back((qlonglong)curSectionField);
        sizeData.push_back((qlonglong)edit->LvlData.events[i].sets[curSectionField].position_top);
        sizeData.push_back((qlonglong)edit->LvlData.events[i].sets[curSectionField].position_right);
        sizeData.push_back((qlonglong)edit->LvlData.events[i].sets[curSectionField].position_bottom);
        sizeData.push_back((qlonglong)edit->LvlData.events[i].sets[curSectionField].position_left);
        sizeData.push_back((qlonglong)0);
        sizeData.push_back((qlonglong)0);
        sizeData.push_back((qlonglong)0);
        sizeData.push_back((qlonglong)-2);
        edit->scene->addChangeEventSettingsHistory(edit->LvlData.events[i].array_id, HistorySettings::SETTING_EV_SECSIZE, QVariant(sizeData));

        ui->LVLEvent_SctSize_left->setText("");
        ui->LVLEvent_SctSize_top->setText("");
        ui->LVLEvent_SctSize_bottom->setText("");
        ui->LVLEvent_SctSize_right->setText("");
        ui->LVLEvent_SctSize_left->setEnabled(false);
        ui->LVLEvent_SctSize_top->setEnabled(false);
        ui->LVLEvent_SctSize_bottom->setEnabled(false);
        ui->LVLEvent_SctSize_right->setEnabled(false);
        ui->LVLEvent_SctSize_Set->setEnabled(false);

        edit->LvlData.events[i].sets[curSectionField].position_left = -2;
        edit->LvlData.events[i].sets[curSectionField].position_right = 0;
        edit->LvlData.events[i].sets[curSectionField].position_bottom = 0;
        edit->LvlData.events[i].sets[curSectionField].position_top = 0;
        edit->LvlData.modified=true;
    }
    lockEventSectionDataList=false;

}

void LvlEventsBox::on_LVLEvent_SctSize_define_clicked()
{
    if(lockSetEventSettings || LvlEventBoxLock) return;

    int WinType = mw()->activeChildWindow();

    if (WinType==1)
    {
        LevelEdit * edit = mw()->activeLvlEditWin();
        if(!edit) return;
        lockEventSectionDataList=true;
        long i = getEventArrayIndex();
        if(i<0) return;

        QList<QVariant> sizeData;
        sizeData.push_back((qlonglong)curSectionField);
        sizeData.push_back((qlonglong)edit->LvlData.events[i].sets[curSectionField].position_top);
        sizeData.push_back((qlonglong)edit->LvlData.events[i].sets[curSectionField].position_right);
        sizeData.push_back((qlonglong)edit->LvlData.events[i].sets[curSectionField].position_bottom);
        sizeData.push_back((qlonglong)edit->LvlData.events[i].sets[curSectionField].position_left);
        sizeData.push_back((qlonglong)edit->LvlData.sections[curSectionField].size_top);
        sizeData.push_back((qlonglong)edit->LvlData.sections[curSectionField].size_right);
        sizeData.push_back((qlonglong)edit->LvlData.sections[curSectionField].size_bottom);
        sizeData.push_back((qlonglong)edit->LvlData.sections[curSectionField].size_left);
        edit->scene->addChangeEventSettingsHistory(edit->LvlData.events[i].array_id, HistorySettings::SETTING_EV_SECSIZE, QVariant(sizeData));

        ui->LVLEvent_SctSize_left->setEnabled(true);
        ui->LVLEvent_SctSize_top->setEnabled(true);
        ui->LVLEvent_SctSize_bottom->setEnabled(true);
        ui->LVLEvent_SctSize_right->setEnabled(true);
        ui->LVLEvent_SctSize_Set->setEnabled(true);

        ui->LVLEvent_SctSize_left->setText(QString::number(edit->LvlData.sections[curSectionField].size_left) );
        ui->LVLEvent_SctSize_top->setText(QString::number(edit->LvlData.sections[curSectionField].size_top) );
        ui->LVLEvent_SctSize_bottom->setText(QString::number(edit->LvlData.sections[curSectionField].size_bottom) );
        ui->LVLEvent_SctSize_right->setText(QString::number(edit->LvlData.sections[curSectionField].size_right) );

        edit->LvlData.events[i].sets[curSectionField].position_left = edit->LvlData.sections[curSectionField].size_left;
        edit->LvlData.events[i].sets[curSectionField].position_right = edit->LvlData.sections[curSectionField].size_right;
        edit->LvlData.events[i].sets[curSectionField].position_bottom = edit->LvlData.sections[curSectionField].size_bottom;
        edit->LvlData.events[i].sets[curSectionField].position_top = edit->LvlData.sections[curSectionField].size_top;
        edit->LvlData.modified=true;
    }
    lockEventSectionDataList=false;
}

void LvlEventsBox::on_LVLEvent_SctSize_left_textEdited(const QString &arg1)
{
    if(lockSetEventSettings || LvlEventBoxLock) return;
    if(lockEventSectionDataList) return;

    int WinType = mw()->activeChildWindow();

    if (WinType==1)
    {
        LevelEdit * edit = mw()->activeLvlEditWin();
        if(!edit) return;
        lockEventSectionDataList=true;
        long i = getEventArrayIndex();
        if(i<0) return;

        QList<QVariant> sizeData;
        sizeData.push_back((qlonglong)curSectionField);
        sizeData.push_back((qlonglong)edit->LvlData.events[i].sets[curSectionField].position_top);
        sizeData.push_back((qlonglong)edit->LvlData.events[i].sets[curSectionField].position_right);
        sizeData.push_back((qlonglong)edit->LvlData.events[i].sets[curSectionField].position_bottom);
        sizeData.push_back((qlonglong)edit->LvlData.events[i].sets[curSectionField].position_left);
        sizeData.push_back((qlonglong)edit->LvlData.events[i].sets[curSectionField].position_top);
        sizeData.push_back((qlonglong)edit->LvlData.events[i].sets[curSectionField].position_right);
        sizeData.push_back((qlonglong)edit->LvlData.events[i].sets[curSectionField].position_bottom);
        sizeData.push_back(arg1.toInt());
        edit->scene->addChangeEventSettingsHistory(edit->LvlData.events[i].array_id, HistorySettings::SETTING_EV_SECSIZE, QVariant(sizeData));

        edit->LvlData.events[i].sets[curSectionField].position_left = arg1.toInt();
    }
    lockEventSectionDataList=false;
}

void LvlEventsBox::on_LVLEvent_SctSize_top_textEdited(const QString &arg1)
{
    if(lockSetEventSettings || LvlEventBoxLock) return;
    if(lockEventSectionDataList) return;


    int WinType = mw()->activeChildWindow();

    if (WinType==1)
    {
        LevelEdit * edit = mw()->activeLvlEditWin();
        if(!edit) return;
        lockEventSectionDataList=true;
        long i = getEventArrayIndex();
        if(i<0) return;

        QList<QVariant> sizeData;
        sizeData.push_back((qlonglong)curSectionField);
        sizeData.push_back((qlonglong)edit->LvlData.events[i].sets[curSectionField].position_top);
        sizeData.push_back((qlonglong)edit->LvlData.events[i].sets[curSectionField].position_right);
        sizeData.push_back((qlonglong)edit->LvlData.events[i].sets[curSectionField].position_bottom);
        sizeData.push_back((qlonglong)edit->LvlData.events[i].sets[curSectionField].position_left);
        sizeData.push_back((qlonglong)arg1.toInt());
        sizeData.push_back((qlonglong)edit->LvlData.events[i].sets[curSectionField].position_right);
        sizeData.push_back((qlonglong)edit->LvlData.events[i].sets[curSectionField].position_bottom);
        sizeData.push_back((qlonglong)edit->LvlData.events[i].sets[curSectionField].position_left);
        edit->scene->addChangeEventSettingsHistory(edit->LvlData.events[i].array_id, HistorySettings::SETTING_EV_SECSIZE, QVariant(sizeData));

        edit->LvlData.events[i].sets[curSectionField].position_top = arg1.toInt();
    }
    lockEventSectionDataList=false;

}

void LvlEventsBox::on_LVLEvent_SctSize_bottom_textEdited(const QString &arg1)
{
    if(lockSetEventSettings || LvlEventBoxLock) return;
    if(lockEventSectionDataList) return;

    int WinType = mw()->activeChildWindow();

    if (WinType==1)
    {
        LevelEdit * edit = mw()->activeLvlEditWin();
        if(!edit) return;

        lockEventSectionDataList=true;
        long i = getEventArrayIndex();
        if(i<0) return;

        QList<QVariant> sizeData;
        sizeData.push_back((qlonglong)curSectionField);
        sizeData.push_back((qlonglong)edit->LvlData.events[i].sets[curSectionField].position_top);
        sizeData.push_back((qlonglong)edit->LvlData.events[i].sets[curSectionField].position_right);
        sizeData.push_back((qlonglong)edit->LvlData.events[i].sets[curSectionField].position_bottom);
        sizeData.push_back((qlonglong)edit->LvlData.events[i].sets[curSectionField].position_left);
        sizeData.push_back((qlonglong)edit->LvlData.events[i].sets[curSectionField].position_top);
        sizeData.push_back((qlonglong)edit->LvlData.events[i].sets[curSectionField].position_right);
        sizeData.push_back((qlonglong)arg1.toInt());
        sizeData.push_back((qlonglong)edit->LvlData.events[i].sets[curSectionField].position_left);
        edit->scene->addChangeEventSettingsHistory(edit->LvlData.events[i].array_id, HistorySettings::SETTING_EV_SECSIZE, QVariant(sizeData));

        edit->LvlData.events[i].sets[curSectionField].position_bottom = arg1.toInt();
    }
    lockEventSectionDataList=false;
}

void LvlEventsBox::on_LVLEvent_SctSize_right_textEdited(const QString &arg1)
{
    if(lockSetEventSettings || LvlEventBoxLock) return;
    if(lockEventSectionDataList) return;

    int WinType = mw()->activeChildWindow();

    if (WinType==1)
    {
        LevelEdit * edit = mw()->activeLvlEditWin();
        if(!edit) return;
        lockEventSectionDataList=true;
        long i = getEventArrayIndex();
        if(i<0) return;

        QList<QVariant> sizeData;
        sizeData.push_back((qlonglong)curSectionField);
        sizeData.push_back((qlonglong)edit->LvlData.events[i].sets[curSectionField].position_top);
        sizeData.push_back((qlonglong)edit->LvlData.events[i].sets[curSectionField].position_right);
        sizeData.push_back((qlonglong)edit->LvlData.events[i].sets[curSectionField].position_bottom);
        sizeData.push_back((qlonglong)edit->LvlData.events[i].sets[curSectionField].position_left);
        sizeData.push_back((qlonglong)edit->LvlData.events[i].sets[curSectionField].position_top);
        sizeData.push_back((qlonglong)arg1.toInt());
        sizeData.push_back((qlonglong)edit->LvlData.events[i].sets[curSectionField].position_bottom);
        sizeData.push_back((qlonglong)edit->LvlData.events[i].sets[curSectionField].position_left);
        edit->scene->addChangeEventSettingsHistory(edit->LvlData.events[i].array_id, HistorySettings::SETTING_EV_SECSIZE, QVariant(sizeData));

        edit->LvlData.events[i].sets[curSectionField].position_right = arg1.toInt();
    }
    lockEventSectionDataList=false;

}

void LvlEventsBox::on_LVLEvent_SctSize_Set_clicked()
{
    if(lockSetEventSettings || LvlEventBoxLock) return;
    if(lockEventSectionDataList) return;

    int WinType = mw()->activeChildWindow();

    if (WinType==1)
    {
        LevelEdit * edit = mw()->activeLvlEditWin();
        if(!edit) return;
        if(curSectionField!=edit->LvlData.CurSection)
        {
            QMessageBox::information(this, tr("Get section size"),
             tr("Please, set current section to %1 for capture data for this event").arg(curSectionField+1),
             QMessageBox::Ok);
            return;
        }

        long i = getEventArrayIndex();
        if(i<0) return;

        edit->setFocus();
        if(edit->scene->pResizer==NULL)
        {
            edit->goTo(edit->LvlData.events[i].sets[curSectionField].position_left,
                                     edit->LvlData.events[i].sets[curSectionField].position_top,
                                     false,
                                     QPoint(-10,-10));
            edit->scene->setEventSctSizeResizer(i, true);
        }
    }

}

void LvlEventsBox::on_LVLEvent_SctMus_none_clicked()
{
    if(lockSetEventSettings || LvlEventBoxLock) return;
    if(lockEventSectionDataList) return;

    int WinType = mw()->activeChildWindow();

    if (WinType==1)
    {
        LevelEdit * edit = mw()->activeLvlEditWin();
        if(!edit) return;
        ui->LVLEvent_SctMus_List->setEnabled(false);
        lockEventSectionDataList=true;
        long i = getEventArrayIndex();
        if(i<0) return;

        QList<QVariant> musData;
        musData.push_back((qlonglong)curSectionField);
        musData.push_back((qlonglong)edit->LvlData.events[i].sets[curSectionField].music_id);
        musData.push_back((qlonglong)-1);
        edit->scene->addChangeEventSettingsHistory(edit->LvlData.events[i].array_id, HistorySettings::SETTING_EV_SECMUS, QVariant(musData));
        edit->LvlData.events[i].sets[curSectionField].music_id = -1;
    }
    lockEventSectionDataList=false;

}

void LvlEventsBox::on_LVLEvent_SctMus_reset_clicked()
{
    if(lockSetEventSettings || LvlEventBoxLock) return;
    if(lockEventSectionDataList) return;

    int WinType = mw()->activeChildWindow();

    if (WinType==1)
    {
        LevelEdit * edit = mw()->activeLvlEditWin();
        if(!edit) return;

        ui->LVLEvent_SctMus_List->setEnabled(false);
        lockEventSectionDataList=true;
        long i = getEventArrayIndex();
        if(i<0) return;

        QList<QVariant> musData;
        musData.push_back((qlonglong)curSectionField);
        musData.push_back((qlonglong)edit->LvlData.events[i].sets[curSectionField].music_id);
        musData.push_back((qlonglong)-2);
        edit->scene->addChangeEventSettingsHistory(edit->LvlData.events[i].array_id, HistorySettings::SETTING_EV_SECMUS, QVariant(musData));
        edit->LvlData.events[i].sets[curSectionField].music_id = -2;
    }
    lockEventSectionDataList=false;
}

void LvlEventsBox::on_LVLEvent_SctMus_define_clicked()
{
    if(lockSetEventSettings || LvlEventBoxLock) return;
    if(lockEventSectionDataList) return;

    int WinType = mw()->activeChildWindow();

    if (WinType==1)
    {
        LevelEdit * edit = mw()->activeLvlEditWin();
        if(!edit) return;

        ui->LVLEvent_SctMus_List->setEnabled(true);
        lockEventSectionDataList=true;
        long i = getEventArrayIndex();
        if(i<0) return;

        QList<QVariant> musData;
        musData.push_back((qlonglong)curSectionField);
        musData.push_back((qlonglong)edit->LvlData.events[i].sets[curSectionField].music_id);
        musData.push_back((qlonglong)ui->LVLEvent_SctMus_List->currentData().toInt());
        edit->scene->addChangeEventSettingsHistory(edit->LvlData.events[i].array_id, HistorySettings::SETTING_EV_SECMUS, QVariant(musData));
        edit->LvlData.events[i].sets[curSectionField].music_id = ui->LVLEvent_SctMus_List->currentData().toInt();
    }
    lockEventSectionDataList=false;
}

void LvlEventsBox::on_LVLEvent_SctMus_List_currentIndexChanged(int index)
{
    if(lockSetEventSettings || LvlEventBoxLock) return;
    if(lockEventSectionDataList) return;
    if(index<0) return;

    int WinType = mw()->activeChildWindow();

    if (WinType==1)
    {
        LevelEdit * edit = mw()->activeLvlEditWin();
        if(!edit) return;
        lockEventSectionDataList=true;
        long i = getEventArrayIndex();
        if(i<0) return;

        QList<QVariant> musData;
        musData.push_back((qlonglong)curSectionField);
        musData.push_back((qlonglong)edit->LvlData.events[i].sets[curSectionField].music_id);
        musData.push_back((qlonglong)ui->LVLEvent_SctMus_List->itemData(index).toInt());
        edit->scene->addChangeEventSettingsHistory(edit->LvlData.events[i].array_id, HistorySettings::SETTING_EV_SECMUS, QVariant(musData));
        edit->LvlData.events[i].sets[curSectionField].music_id = ui->LVLEvent_SctMus_List->itemData(index).toInt();
    }
    lockEventSectionDataList=false;
}

void LvlEventsBox::on_LVLEvent_SctBg_none_clicked()
{
    if(lockSetEventSettings || LvlEventBoxLock) return;
    if(lockEventSectionDataList) return;

    int WinType = mw()->activeChildWindow();

    if (WinType==1)
    {
        LevelEdit * edit = mw()->activeLvlEditWin();
        if(!edit) return;

        ui->LVLEvent_SctBg_List->setEnabled(false);
        lockEventSectionDataList=true;
        long i = getEventArrayIndex();
        if(i<0) return;

        QList<QVariant> bgData;
        bgData.push_back((qlonglong)curSectionField);
        bgData.push_back((qlonglong)edit->LvlData.events[i].sets[curSectionField].background_id);
        bgData.push_back((qlonglong)-1);
        edit->scene->addChangeEventSettingsHistory(edit->LvlData.events[i].array_id, HistorySettings::SETTING_EV_SECBG, QVariant(bgData));
        edit->LvlData.events[i].sets[curSectionField].background_id = -1;
    }
    lockEventSectionDataList=false;

}

void LvlEventsBox::on_LVLEvent_SctBg_reset_clicked()
{
    if(lockSetEventSettings || LvlEventBoxLock) return;
    if(lockEventSectionDataList) return;

    int WinType = mw()->activeChildWindow();

    if (WinType==1)
    {
        LevelEdit * edit = mw()->activeLvlEditWin();
        if(!edit) return;

        ui->LVLEvent_SctBg_List->setEnabled(false);
        lockEventSectionDataList=true;
        long i = getEventArrayIndex();
        if(i<0) return;

        QList<QVariant> bgData;
        bgData.push_back((qlonglong)curSectionField);
        bgData.push_back((qlonglong)edit->LvlData.events[i].sets[curSectionField].background_id);
        bgData.push_back((qlonglong)-2);
        edit->scene->addChangeEventSettingsHistory(edit->LvlData.events[i].array_id, HistorySettings::SETTING_EV_SECBG, QVariant(bgData));
        edit->LvlData.events[i].sets[curSectionField].background_id = -2;
    }
    lockEventSectionDataList=false;

}

void LvlEventsBox::on_LVLEvent_SctBg_define_clicked()
{
    if(lockSetEventSettings || LvlEventBoxLock) return;
    if(lockEventSectionDataList) return;

    int WinType = mw()->activeChildWindow();

    if (WinType==1)
    {
        LevelEdit * edit = mw()->activeLvlEditWin();
        if(!edit) return;

        ui->LVLEvent_SctBg_List->setEnabled(true);
        lockEventSectionDataList=true;
        long i = getEventArrayIndex();
        if(i<0) return;

        QList<QVariant> bgData;
        bgData.push_back((qlonglong)curSectionField);
        bgData.push_back((qlonglong)edit->LvlData.events[i].sets[curSectionField].background_id);
        bgData.push_back((qlonglong)ui->LVLEvent_SctBg_List->currentData().toInt());
        edit->scene->addChangeEventSettingsHistory(edit->LvlData.events[i].array_id, HistorySettings::SETTING_EV_SECBG, QVariant(bgData));
        edit->LvlData.events[i].sets[curSectionField].background_id = ui->LVLEvent_SctBg_List->currentData().toInt();
    }
    lockEventSectionDataList=false;

}

void LvlEventsBox::on_LVLEvent_SctBg_List_currentIndexChanged(int index)
{
    if(lockSetEventSettings || LvlEventBoxLock) return;
    if(lockEventSectionDataList) return;
    if(index<0) return;


    int WinType = mw()->activeChildWindow();

    if (WinType==1)
    {
        LevelEdit * edit = mw()->activeLvlEditWin();
        if(!edit) return;

        lockEventSectionDataList=true;
        long i = getEventArrayIndex();
        if(i<0) return;

        QList<QVariant> bgData;
        bgData.push_back((qlonglong)curSectionField);
        bgData.push_back((qlonglong)edit->LvlData.events[i].sets[curSectionField].background_id);
        bgData.push_back((qlonglong)ui->LVLEvent_SctBg_List->itemData(index).toInt());
        edit->scene->addChangeEventSettingsHistory(edit->LvlData.events[i].array_id, HistorySettings::SETTING_EV_SECBG, QVariant(bgData));
        edit->LvlData.events[i].sets[curSectionField].background_id = ui->LVLEvent_SctBg_List->itemData(index).toInt();
    }
    lockEventSectionDataList=false;
}





void LvlEventsBox::on_LVLEvent_Cmn_Msg_clicked()
{
    if(currentEventArrayID<0) return;

    int WinType = mw()->activeChildWindow();

    if (WinType==1)
    {
        LevelEdit * edit = mw()->activeLvlEditWin();
        if(!edit) return;

        long i = getEventArrayIndex();
        if(i<0) return;

        ItemMsgBox * msgBox = new ItemMsgBox(Opened_By::EVENT, edit->LvlData.events[i].msg, false,
                tr("Please, enter message\nMessage limits: max line lenth is 28 characters"), "", this);
        util::DialogToCenter(msgBox);
        if(msgBox->exec()==QDialog::Accepted)
        {
            QList<QVariant> msgData;
            msgData.push_back(edit->LvlData.events[i].msg);
            msgData.push_back(msgBox->currentText);
            edit->scene->addChangeEventSettingsHistory(edit->LvlData.events[i].array_id, HistorySettings::SETTING_EV_MSG, QVariant(msgData));

            edit->LvlData.events[i].msg = msgBox->currentText;
            QString evnmsg = (edit->LvlData.events[i].msg.isEmpty() ? tr("[none]") : edit->LvlData.events[i].msg);
            if(evnmsg.size()>20)
            {
                evnmsg.resize(18);
                evnmsg.push_back("...");
            }
            ui->LVLEvent_Cmn_Msg->setText( evnmsg.replace("&", "&&&") );
            edit->LvlData.modified=true;
        }
        delete msgBox;
    }
}


void LvlEventsBox::on_LVLEvent_Cmn_PlaySnd_currentIndexChanged(int index)
{
    if(lockSetEventSettings || LvlEventBoxLock) return;
    if(index<0) return;

    int WinType = mw()->activeChildWindow();

    if(WinType==1)
    {
        LevelEdit * edit = mw()->activeLvlEditWin();
        if(!edit) return;

        long i = getEventArrayIndex();
        if(i<0) return;
        QList<QVariant> soundData;
        soundData.push_back((qlonglong)edit->LvlData.events[i].sound_id);
        soundData.push_back((qlonglong)ui->LVLEvent_Cmn_PlaySnd->currentData().toInt());
        edit->scene->addChangeEventSettingsHistory(edit->LvlData.events[i].array_id, HistorySettings::SETTING_EV_SOUND, QVariant(soundData));

        edit->LvlData.events[i].sound_id = ui->LVLEvent_Cmn_PlaySnd->currentData().toInt();
        edit->LvlData.modified=true;
    }
}

void LvlEventsBox::on_LVLEvent_playSnd_clicked()
{
    if(ui->LVLEvent_Cmn_PlaySnd->currentData().toInt()==0) return;

    QString sndPath = mw()->configs.dirs.sounds;
    long i;
    bool found=false;
    i = mw()->configs.getSndI( ui->LVLEvent_Cmn_PlaySnd->currentData().toInt() );
    if(i>=0)
    {
        found=true;
        sndPath += mw()->configs.main_sound[i].file;
    }

    WriteToLog(QtDebugMsg, QString("Test Sound -> path-1 %1").arg(sndPath));

    if(!found) return;
    if(!QFileInfo::exists(sndPath)) return;

    PGE_Sounds::SND_PlaySnd(sndPath);
    WriteToLog(QtDebugMsg, QString("Test Sound -> done!"));
}



void LvlEventsBox::on_LVLEvent_Cmn_EndGame_currentIndexChanged(int index)
{
    if(lockSetEventSettings || LvlEventBoxLock) return;
    if(index<0) return;

    int WinType = mw()->activeChildWindow();

    if (WinType==1)
    {
        LevelEdit * edit = mw()->activeLvlEditWin();
        if(!edit) return;

        long i = getEventArrayIndex();
        if(i<0) return;
        QList<QVariant> endData;
        endData.push_back((qlonglong)edit->LvlData.events[i].end_game);
        endData.push_back((qlonglong)ui->LVLEvent_Cmn_EndGame->currentIndex());
        edit->scene->addChangeEventSettingsHistory(edit->LvlData.events[i].array_id, HistorySettings::SETTING_EV_ENDGAME, QVariant(endData));

        edit->LvlData.events[i].end_game = ui->LVLEvent_Cmn_EndGame->currentIndex();
        edit->LvlData.modified=true;
    }

}



void LvlEventsBox::on_LVLEvent_Key_Up_clicked(bool checked)
{

    if(lockSetEventSettings || LvlEventBoxLock) return;

    int WinType = mw()->activeChildWindow();

    if (WinType==1)
    {
        LevelEdit * edit = mw()->activeLvlEditWin();
        if(!edit) return;

        long i = getEventArrayIndex();
        if(i<0) return;

        edit->scene->addChangeEventSettingsHistory(edit->LvlData.events[i].array_id, HistorySettings::SETTING_EV_KUP, QVariant(checked));
        edit->LvlData.events[i].ctrl_up = checked;
        edit->LvlData.modified=true;
    }

}

void LvlEventsBox::on_LVLEvent_Key_Down_clicked(bool checked)
{
    if(lockSetEventSettings || LvlEventBoxLock) return;

    int WinType = mw()->activeChildWindow();

    if (WinType==1)
    {
        LevelEdit * edit = mw()->activeLvlEditWin();
        if(!edit) return;

        long i = getEventArrayIndex();
        if(i<0) return;

        edit->scene->addChangeEventSettingsHistory(edit->LvlData.events[i].array_id, HistorySettings::SETTING_EV_KDOWN, QVariant(checked));
        edit->LvlData.events[i].ctrl_down = checked;
        edit->LvlData.modified=true;
    }
}

void LvlEventsBox::on_LVLEvent_Key_Left_clicked(bool checked)
{
    if(lockSetEventSettings || LvlEventBoxLock) return;

    int WinType = mw()->activeChildWindow();

    if (WinType==1)
    {
        LevelEdit * edit = mw()->activeLvlEditWin();
        if(!edit) return;

        long i = getEventArrayIndex();
        if(i<0) return;

        edit->scene->addChangeEventSettingsHistory(edit->LvlData.events[i].array_id, HistorySettings::SETTING_EV_KLEFT, QVariant(checked));
        edit->LvlData.events[i].ctrl_left = checked;
        edit->LvlData.modified=true;
    }
}

void LvlEventsBox::on_LVLEvent_Key_Right_clicked(bool checked)
{
    if(lockSetEventSettings || LvlEventBoxLock) return;

    int WinType = mw()->activeChildWindow();

    if (WinType==1)
    {
        LevelEdit * edit = mw()->activeLvlEditWin();
        if(!edit) return;

        long i = getEventArrayIndex();
        if(i<0) return;

        edit->scene->addChangeEventSettingsHistory(edit->LvlData.events[i].array_id, HistorySettings::SETTING_EV_KRIGHT, QVariant(checked));
        edit->LvlData.events[i].ctrl_right = checked;
        edit->LvlData.modified=true;
    }
}

void LvlEventsBox::on_LVLEvent_Key_Run_clicked(bool checked)
{
    if(lockSetEventSettings || LvlEventBoxLock) return;

    int WinType = mw()->activeChildWindow();

    if (WinType==1)
    {
        LevelEdit * edit = mw()->activeLvlEditWin();
        if(!edit) return;

        long i = getEventArrayIndex();
        if(i<0) return;

        edit->scene->addChangeEventSettingsHistory(edit->LvlData.events[i].array_id, HistorySettings::SETTING_EV_KRUN, QVariant(checked));
        edit->LvlData.events[i].ctrl_run = checked;
        edit->LvlData.modified=true;
    }
}

void LvlEventsBox::on_LVLEvent_Key_AltRun_clicked(bool checked)
{
    if(lockSetEventSettings || LvlEventBoxLock) return;

    int WinType = mw()->activeChildWindow();

    if (WinType==1)
    {
        LevelEdit * edit = mw()->activeLvlEditWin();
        if(!edit) return;

        long i = getEventArrayIndex();
        if(i<0) return;

        edit->scene->addChangeEventSettingsHistory(edit->LvlData.events[i].array_id, HistorySettings::SETTING_EV_KALTRUN, QVariant(checked));
        edit->LvlData.events[i].ctrl_altrun = checked;
        edit->LvlData.modified=true;
    }
}

void LvlEventsBox::on_LVLEvent_Key_Jump_clicked(bool checked)
{
    if(lockSetEventSettings || LvlEventBoxLock) return;

    int WinType = mw()->activeChildWindow();

    if (WinType==1)
    {
        LevelEdit * edit = mw()->activeLvlEditWin();
        if(!edit) return;

        long i = getEventArrayIndex();
        if(i<0) return;

        edit->scene->addChangeEventSettingsHistory(edit->LvlData.events[i].array_id, HistorySettings::SETTING_EV_KJUMP, QVariant(checked));
        edit->LvlData.events[i].ctrl_jump = checked;
        edit->LvlData.modified=true;
    }
}

void LvlEventsBox::on_LVLEvent_Key_AltJump_clicked(bool checked)
{
    if(lockSetEventSettings || LvlEventBoxLock) return;

    int WinType = mw()->activeChildWindow();

    if (WinType==1)
    {
        LevelEdit * edit = mw()->activeLvlEditWin();
        if(!edit) return;

        long i = getEventArrayIndex();
        if(i<0) return;

        edit->scene->addChangeEventSettingsHistory(edit->LvlData.events[i].array_id, HistorySettings::SETTING_EV_KALTJUMP, QVariant(checked));
        edit->LvlData.events[i].ctrl_altjump = checked;
        edit->LvlData.modified=true;
    }
}

void LvlEventsBox::on_LVLEvent_Key_Drop_clicked(bool checked)
{
    if(lockSetEventSettings || LvlEventBoxLock) return;

    int WinType = mw()->activeChildWindow();

    if (WinType==1)
    {
        LevelEdit * edit = mw()->activeLvlEditWin();
        if(!edit) return;

        long i = getEventArrayIndex();
        if(i<0) return;

        edit->scene->addChangeEventSettingsHistory(edit->LvlData.events[i].array_id, HistorySettings::SETTING_EV_KDROP, QVariant(checked));
        edit->LvlData.events[i].ctrl_drop = checked;
        edit->LvlData.modified=true;
    }
}

void LvlEventsBox::on_LVLEvent_Key_Start_clicked(bool checked)
{
    if(lockSetEventSettings || LvlEventBoxLock) return;

    int WinType = mw()->activeChildWindow();

    if (WinType==1)
    {
        LevelEdit * edit = mw()->activeLvlEditWin();
        if(!edit) return;

        long i = getEventArrayIndex();
        if(i<0) return;

        edit->scene->addChangeEventSettingsHistory(edit->LvlData.events[i].array_id, HistorySettings::SETTING_EV_KSTART, QVariant(checked));
        edit->LvlData.events[i].ctrl_start = checked;
        edit->LvlData.modified=true;
    }
}




void LvlEventsBox::on_LVLEvent_TriggerEvent_currentIndexChanged(int index)
{
    if(lockSetEventSettings || LvlEventBoxLock) return;
    if(index<0) return;

    int WinType = mw()->activeChildWindow();

    if (WinType==1)
    {
        LevelEdit * edit = mw()->activeLvlEditWin();
        if(!edit) return;

        long i = getEventArrayIndex();
        if(i<0) return;

        QList<QVariant> triggerData;
        triggerData.push_back(edit->LvlData.events[i].trigger);
        triggerData.push_back(ui->LVLEvent_TriggerEvent->currentText());
        edit->scene->addChangeEventSettingsHistory(edit->LvlData.events[i].array_id, HistorySettings::SETTING_EV_TRIACTIVATE, QVariant(triggerData));

        if(index==0)
            edit->LvlData.events[i].trigger="";
        else
            edit->LvlData.events[i].trigger = ui->LVLEvent_TriggerEvent->currentText();
        edit->LvlData.modified=true;
    }

}

void LvlEventsBox::on_LVLEvent_TriggerDelay_valueChanged(double arg1)
{
    if(lockSetEventSettings || LvlEventBoxLock) return;

    int WinType = mw()->activeChildWindow();

    if (WinType==1)
    {
        LevelEdit * edit = mw()->activeLvlEditWin();
        if(!edit) return;

        long i = getEventArrayIndex();
        if(i<0) return;

        QList<QVariant> triggerData;
        triggerData.push_back((qlonglong)edit->LvlData.events[i].trigger_timer);
        triggerData.push_back((qlonglong)qRound(arg1*10));
        edit->scene->addChangeEventSettingsHistory(edit->LvlData.events[i].array_id, HistorySettings::SETTING_EV_TRIDELAY, QVariant(triggerData));

        edit->LvlData.events[i].trigger_timer = qRound(arg1*10);
        edit->LvlData.modified=true;
    }

}


void LvlEventsBox::on_bps_LayerMov_horSpeed_clicked()
{
    BlocksPerSecondDialog bps;
    if(!bps.exec())
        return;

    ui->LVLEvent_LayerMov_spX->setValue(bps.result());
}

void LvlEventsBox::on_bps_LayerMov_vertSpeed_clicked()
{
    BlocksPerSecondDialog bps;
    if(!bps.exec())
        return;

    ui->LVLEvent_LayerMov_spY->setValue(bps.result());
}

void LvlEventsBox::on_bps_Scroll_horSpeed_clicked()
{
    BlocksPerSecondDialog bps;
    if(!bps.exec())
        return;

    ui->LVLEvent_Scroll_spX->setValue(bps.result());
}

void LvlEventsBox::on_bps_Scroll_vertSpeed_clicked()
{
    BlocksPerSecondDialog bps;
    if(!bps.exec())
        return;

    ui->LVLEvent_Scroll_spY->setValue(bps.result());
}
