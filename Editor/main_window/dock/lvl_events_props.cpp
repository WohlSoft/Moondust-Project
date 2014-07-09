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

#include <QMediaPlayer>

#include "../../ui_mainwindow.h"
#include "../../mainwindow.h"


#include "../../level_scene/item_bgo.h"
#include "../../level_scene/item_block.h"
#include "../../level_scene/item_npc.h"
#include "../../level_scene/item_water.h"

#include "../../level_scene/itemmsgbox.h"

#include "../../file_formats/file_formats.h"

#include "../../common_features/util.h"

static long currentEventArrayID=0;
static bool lockSetEventSettings=false;

static bool lockEventSectionDataList=false;
static long curSectionField=0;

static bool cloneEvent=false;
static long cloneEventId=0;

QMediaPlayer playSnd;

void MainWindow::setEventsBox()
{
    int WinType = activeChildWindow();
    QListWidgetItem * item;

    util::memclear(ui->LVLEvents_List);

    if (WinType==1)
    {
        foreach(LevelEvents event, activeLvlEditWin()->LvlData.events)
        {
            item = new QListWidgetItem;
            item->setText(event.name);
            //item->setFlags(Qt::ItemIsUserCheckable);

            item->setFlags(item->flags() | Qt::ItemIsEnabled | Qt::ItemIsSelectable);

            if((event.name!="Level - Start")&&(event.name!="P Switch - Start")&&(event.name!="P Switch - End"))
                item->setFlags(item->flags() | Qt::ItemIsEditable | Qt::ItemIsDragEnabled);

            item->setData(3, QString::number( event.array_id ) );
            ui->LVLEvents_List->addItem( item );
        }

        on_LVLEvents_List_itemSelectionChanged();
    }
}

void MainWindow::EventListsSync()
{
    ui->ItemProperties->hide();
    LvlItemPropsLock=true;
    lockSetEventSettings=true;

    QString curDestroyedBlock = ui->Find_Combo_EventDestoryedBlock->currentText();
    QString curHitedBlock = ui->Find_Combo_EventHitedBlock->currentText();
    QString curLayerEmptyBlock = ui->Find_Combo_EventLayerEmptyBlock->currentText();

    int WinType = activeChildWindow();

    ui->PROPS_BlkEventDestroy->clear();
    ui->PROPS_BlkEventHited->clear();
    ui->PROPS_BlkEventLayerEmpty->clear();

    ui->PROPS_NpcEventActivate->clear();
    ui->PROPS_NpcEventDeath->clear();
    ui->PROPS_NpcEventTalk->clear();
    ui->PROPS_NpcEventEmptyLayer->clear();
    ui->LVLEvent_TriggerEvent->clear();

    QString noEvent = tr("[None]");
    ui->PROPS_BlkEventDestroy->addItem(noEvent);
    ui->PROPS_BlkEventHited->addItem(noEvent);
    ui->PROPS_BlkEventLayerEmpty->addItem(noEvent);

    ui->PROPS_NpcEventActivate->addItem(noEvent);
    ui->PROPS_NpcEventDeath->addItem(noEvent);
    ui->PROPS_NpcEventTalk->addItem(noEvent);
    ui->PROPS_NpcEventEmptyLayer->addItem(noEvent);
    ui->LVLEvent_TriggerEvent->addItem(noEvent);

    ui->Find_Combo_EventDestoryedBlock->clear();
    ui->Find_Combo_EventHitedBlock->clear();
    ui->Find_Combo_EventLayerEmptyBlock->clear();

    if (WinType==1)
    {
        foreach(LevelEvents event, activeLvlEditWin()->LvlData.events)
        {
            ui->PROPS_BlkEventDestroy->addItem(event.name);
            ui->PROPS_BlkEventHited->addItem(event.name);
            ui->PROPS_BlkEventLayerEmpty->addItem(event.name);

            ui->PROPS_NpcEventActivate->addItem(event.name);
            ui->PROPS_NpcEventDeath->addItem(event.name);
            ui->PROPS_NpcEventTalk->addItem(event.name);
            ui->PROPS_NpcEventEmptyLayer->addItem(event.name);
            ui->LVLEvent_TriggerEvent->addItem(event.name);

            ui->Find_Combo_EventDestoryedBlock->addItem(event.name);
            ui->Find_Combo_EventHitedBlock->addItem(event.name);
            ui->Find_Combo_EventLayerEmptyBlock->addItem(event.name);
        }
    }
    //LvlItemPropsLock = false; - must be true always

    ui->Find_Combo_EventDestoryedBlock->setCurrentText(curDestroyedBlock);
    ui->Find_Combo_EventHitedBlock->setCurrentText(curHitedBlock);
    ui->Find_Combo_EventLayerEmptyBlock->setCurrentText(curLayerEmptyBlock);

    lockSetEventSettings=false;
}

void MainWindow::setSoundList()
{
    ui->LVLEvent_Cmn_PlaySnd->clear();
    ui->LVLEvent_Cmn_PlaySnd->addItem( tr("[Silence]"), "0" );

    if(configs.check()) return;

    foreach(obj_sound snd, configs.main_sound )
    {
        if(!snd.hidden)
            ui->LVLEvent_Cmn_PlaySnd->addItem(snd.name, QString::number(snd.id));
    }
}


void MainWindow::setEventData(long index)
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
            cIndex = ui->LVLEvents_List->currentItem()->data(3).toInt();
        else
            {
                cIndex = currentEventArrayID;
                for(int q=0; q<ui->LVLEvents_List->count();q++) //Select if not selected
                {
                    if(ui->LVLEvents_List->item(q)->data(3).toInt()==cIndex)
                    {
                        ui->LVLEvents_List->item(q)->setSelected(true);
                        break;
                    }
                }
            }
        }


    int WinType = activeChildWindow();
    if (WinType==1)
    {

        if( (activeLvlEditWin()->LvlData.events.size() > 0) && (cIndex >= 0))
        {
            leveledit * edit = activeLvlEditWin();
            currentEventArrayID=cIndex;
            foreach(LevelEvents event, edit->LvlData.events)
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
                    ui->LVLEvent_Cmn_Msg->setText( evnmsg );

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
                    ui->LVLEvent_Key_AltJump->setChecked(event.altjump);
                    ui->LVLEvent_Key_AltRun->setChecked(event.altrun);
                    ui->LVLEvent_Key_Jump->setChecked(event.jump);
                    ui->LVLEvent_Key_Run->setChecked(event.run);
                    ui->LVLEvent_Key_Start->setChecked(event.start);
                    ui->LVLEvent_Key_Drop->setChecked(event.drop);
                    ui->LVLEvent_Key_Left->setChecked(event.left);
                    ui->LVLEvent_Key_Right->setChecked(event.right);
                    ui->LVLEvent_Key_Up->setChecked(event.up);
                    ui->LVLEvent_Key_Down->setChecked(event.down);

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


void MainWindow::eventSectionSettingsSync()
{
    int WinType = activeChildWindow();

    if (WinType==1)
    {
        leveledit * edit = activeLvlEditWin();
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

void MainWindow::eventLayerVisiblySyncList()
{
    int WinType = activeChildWindow();

    if (WinType==1)
    {
        leveledit * edit = activeLvlEditWin();
        long i = getEventArrayIndex();
        if(i<0) return;

        LevelEvents event = edit->LvlData.events[i];

        util::memclear(ui->LVLEvents_layerList);
        util::memclear(ui->LVLEvent_Layer_HideList);
        util::memclear(ui->LVLEvent_Layer_ShowList);
        util::memclear(ui->LVLEvent_Layer_ToggleList);

        QListWidgetItem * item;
        //Total layers list
        foreach(LevelLayers layer, edit->LvlData.layers)
        {
            item = new QListWidgetItem;
            item->setText(layer.name);
            item->setFlags(item->flags() | Qt::ItemIsEnabled | Qt::ItemIsSelectable);
            item->setData(3, QString::number( layer.array_id ) );
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

void MainWindow::on_LVLEvents_List_itemSelectionChanged()
{
    if(ui->LVLEvents_List->selectedItems().isEmpty())
    {
        setEventData(-1);
    }
    else
        setEventData(ui->LVLEvents_List->currentItem()->data(3).toInt());

}

void MainWindow::on_LVLEvents_List_itemChanged(QListWidgetItem *item)
{
    if(lockSetEventSettings) return;
    lockSetEventSettings=true;
    int WinType = activeChildWindow();

    if (WinType==1)
    {
        leveledit * edit = activeLvlEditWin();
        if(item->data(3).toString()=="NewEvent")
        {
            bool AlreadyExist=false;
            foreach(LevelEvents event, edit->LvlData.events)
            {
                if( event.name==item->text() )
                {
                    AlreadyExist=true;
                    break;
                }
            }

            if(AlreadyExist)
            {
                delete item;
                cloneEvent=false;//Reset state
                return;
            }
            else
            {
                LevelEvents NewEvent = FileFormats::dummyLvlEvent();

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

                item->setData(3, QString::number(NewEvent.array_id));

                if(!cloneEvent)
                    edit->scene->addAddEventHistory(NewEvent.array_id, NewEvent.name);
                else
                    edit->scene->addDuplicateEventHistory(NewEvent);
                edit->LvlData.events.push_back(NewEvent);
                edit->LvlData.modified=true;
                cloneEvent=false;//Reset state
            }

        }//if(item->data(3).toString()=="NewEvent")
        else
        {
            QString eventName = item->text();
            QString oldEventName = item->text();

            ModifyEventItem(item, oldEventName, eventName);
            activeLvlEditWin()->LvlData.modified=true;
        }

    }//if WinType==1
    EventListsSync();
    lockSetEventSettings=false;
}

void MainWindow::DragAndDroppedEvent(QModelIndex /*sourceParent*/,int sourceStart,int sourceEnd,QModelIndex /*destinationParent*/,int destinationRow)
{
    lockSetEventSettings=true;
    WriteToLog(QtDebugMsg, "Row Change at " + QString::number(sourceStart) +
               " " + QString::number(sourceEnd) +
               " to " + QString::number(destinationRow));

    int WinType = activeChildWindow();
    if (WinType==1)
    {
        LevelEvents buffer;
        if(sourceStart<activeLvlEditWin()->LvlData.events.size())
        {
            buffer = activeLvlEditWin()->LvlData.events[sourceStart];
            activeLvlEditWin()->LvlData.events.remove(sourceStart);
            activeLvlEditWin()->LvlData.events.insert(((destinationRow>sourceStart)?destinationRow-1:destinationRow), buffer);
        }
    }
    lockSetEventSettings=false;
    EventListsSync();  //Sync comboboxes in properties
}


long MainWindow::getEventArrayIndex()
{
    if(activeChildWindow()!=1) return -2;

    leveledit * edit = activeLvlEditWin();
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


void MainWindow::AddNewEvent(QString eventName, bool setEdited)
{
    lockSetEventSettings=true;

    if(activeChildWindow()!=1) return;

    QListWidgetItem * item;
    item = new QListWidgetItem;
    item->setText(eventName);
    item->setFlags(Qt::ItemIsEditable);
    item->setFlags(item->flags() | Qt::ItemIsEnabled);
    item->setFlags(item->flags() | Qt::ItemIsEditable | Qt::ItemIsDragEnabled | Qt::ItemIsSelectable);
    item->setData(3, QString("NewEvent") );

    ui->LVLEvents_List->addItem( item );

    leveledit * edit = activeLvlEditWin();

    if(setEdited)
    {
        ui->LVLEvents_List->setFocus();
        ui->LVLEvents_List->scrollToItem( item );
        ui->LVLEvents_List->editItem( item );
    }
    else
    {
        bool AlreadyExist=false;
        foreach(LevelEvents event, edit->LvlData.events)
        {
            if( event.name==item->text() )
            {
                AlreadyExist=true;
                break;
            }
        }

        if(AlreadyExist)
        {
            delete item;
            lockSetEventSettings=false;
            cloneEvent=false;//Reset state
            return;
        }
        else
        {
            LevelEvents NewEvent = FileFormats::dummyLvlEvent();
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
            item->setData(3, QString::number(NewEvent.array_id));

            if(!cloneEvent)
                edit->scene->addAddEventHistory(NewEvent.array_id, NewEvent.name);
            else
                edit->scene->addDuplicateEventHistory(NewEvent);
            edit->LvlData.events.push_back(NewEvent);
            edit->LvlData.modified=true;
            cloneEvent=false;//Reset state
        }
    }
    lockSetEventSettings=false;
    EventListsSync();  //Sync comboboxes in properties
}

void MainWindow::ModifyEventItem(QListWidgetItem *item, QString oldEventName, QString newEventName)
{
    lockSetEventSettings=true;
    //Find layer enrty in array and apply settings
    leveledit * edit = activeLvlEditWin();
    for(int i=0; i < edit->LvlData.events.size(); i++)
    {
        if( edit->LvlData.events[i].array_id==(unsigned int)item->data(3).toInt() )
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

            activeLvlEditWin()->scene->addRenameEventHistory(edit->LvlData.events[i].array_id, oldEventName, newEventName);

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

void MainWindow::ModifyEvent(QString eventName, QString newEventName)
{
    //Apply layer's name to all items
    leveledit * edit = activeLvlEditWin();
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
            if( ((ItemBlock *)(*it))->blockData.event_no_more ==  eventName)
                {((ItemBlock *)(*it))->blockData.event_no_more = newEventName; isMod=true;}
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
            if( ((ItemNPC *)(*it))->npcData.event_nomore ==  eventName)
                {((ItemNPC *)(*it))->npcData.event_nomore = newEventName; isMod=true;}
            if(isMod) {((ItemNPC *)(*it))->arrayApply();}
        }
    }
    for(int i=0; i < edit->LvlData.events.size(); i++)
    {
        if( edit->LvlData.events[i].trigger == eventName)
            edit->LvlData.events[i].trigger = newEventName;
    }
}

QListWidget *MainWindow::getEventList()
{
    return ui->LVLEvents_List;
}

void MainWindow::setEventToolsLocked(bool locked)
{
    lockSetEventSettings = locked;
}

void MainWindow::RemoveEvent(QString eventName)
{
    if(eventName.isEmpty()) return;
    //dummy
}

void MainWindow::on_LVLEvents_add_clicked()
{
    AddNewEvent(tr("New Event %1").arg( ui->LVLEvents_List->count()+1 ), true);
}

void MainWindow::on_LVLEvents_del_clicked()
{
    if(ui->LVLEvents_List->selectedItems().isEmpty()) return;

    if(ui->LVLEvents_List->selectedItems()[0]->text()=="Level - Start") return;
    if(ui->LVLEvents_List->selectedItems()[0]->text()=="P Switch - Start") return;
    if(ui->LVLEvents_List->selectedItems()[0]->text()=="P Switch - End") return;

    int WinType = activeChildWindow();

    if (WinType==1)
    {
        for(int i=0;i< activeLvlEditWin()->LvlData.events.size(); i++)
        {
            if( activeLvlEditWin()->LvlData.events[i].array_id==
                    (unsigned int)ui->LVLEvents_List->selectedItems()[0]->data(3).toInt() )
            {
                activeLvlEditWin()->scene->addRemoveEventHistory(activeLvlEditWin()->LvlData.events[i]);
                ModifyEvent(activeLvlEditWin()->LvlData.events[i].name, "");
                activeLvlEditWin()->LvlData.events.remove(i);
                delete ui->LVLEvents_List->selectedItems()[0];
                break;
            }
        }
    }
    EventListsSync();
}

void MainWindow::on_LVLEvents_duplicate_clicked()
{
    if(ui->LVLEvents_List->selectedItems().isEmpty()) return;

    cloneEvent=true;
    cloneEventId=ui->LVLEvents_List->selectedItems()[0]->data(3).toInt();

    AddNewEvent(tr("Copyed Event %1").arg( ui->LVLEvents_List->count()+1 ), true);

}


void MainWindow::on_LVLEvent_AutoStart_clicked(bool checked)
{
    if(lockSetEventSettings) return;

    int WinType = activeChildWindow();

    if (WinType==1)
    {
        leveledit * edit = activeLvlEditWin();
        long i = getEventArrayIndex();
        if(i<0) return;

        edit->scene->addChangeEventSettingsHistory(edit->LvlData.events[i].array_id, LvlScene::SETTING_EV_AUTOSTART, QVariant(checked));
        edit->LvlData.events[i].autostart = checked;
        edit->LvlData.modified=true;
    }
}







void MainWindow::on_LVLEvent_disableSmokeEffect_clicked(bool checked)
{
    if(lockSetEventSettings) return;

    int WinType = activeChildWindow();

    if (WinType==1)
    {
        leveledit * edit = activeLvlEditWin();
        long i = getEventArrayIndex();
        if(i<0) return;

        edit->scene->addChangeEventSettingsHistory(edit->LvlData.events[i].array_id, LvlScene::SETTING_EV_SMOKE, QVariant(checked));
        edit->LvlData.events[i].nosmoke = checked;
        edit->LvlData.modified=true;
    }

}




void MainWindow::on_LVLEvent_Layer_HideAdd_clicked()
{
    if(lockSetEventSettings) return;

    int WinType = activeChildWindow();

    if (WinType==1)
    {
        leveledit * edit = activeLvlEditWin();
        long i = getEventArrayIndex();
        if(i<0) return;

        if(!ui->LVLEvents_layerList->selectedItems().isEmpty())
        {
            edit->scene->addChangeEventSettingsHistory(edit->LvlData.events[i].array_id, LvlScene::SETTING_EV_LHIDEADD, QVariant(ui->LVLEvents_layerList->currentItem()->text()));
            edit->LvlData.events[i].layers_hide.push_back(ui->LVLEvents_layerList->currentItem()->text());
            edit->LvlData.modified=true;
            eventLayerVisiblySyncList();
        }
    }

}

void MainWindow::on_LVLEvent_Layer_HideDel_clicked()
{
    if(lockSetEventSettings) return;

    int WinType = activeChildWindow();

    if (WinType==1)
    {
        leveledit * edit = activeLvlEditWin();
        long i = getEventArrayIndex();
        if(i<0) return;

        if(!ui->LVLEvent_Layer_HideList->selectedItems().isEmpty())
        {
            for(int j=0; j<edit->LvlData.events[i].layers_hide.size(); j++)
            {
            if(edit->LvlData.events[i].layers_hide[j]==ui->LVLEvent_Layer_HideList->currentItem()->text())
                {
                    edit->scene->addChangeEventSettingsHistory(edit->LvlData.events[i].array_id, LvlScene::SETTING_EV_LHIDEDEL, QVariant(edit->LvlData.events[i].layers_hide[j]));
                    edit->LvlData.events[i].layers_hide.removeAt(j);
                    edit->LvlData.modified=true;
                    break;
                }
            }
            eventLayerVisiblySyncList();
        }
    }
}

void MainWindow::on_LVLEvent_Layer_ShowAdd_clicked()
{
    if(lockSetEventSettings) return;

    int WinType = activeChildWindow();

    if (WinType==1)
    {
        leveledit * edit = activeLvlEditWin();
        long i = getEventArrayIndex();
        if(i<0) return;

        if(!ui->LVLEvents_layerList->selectedItems().isEmpty())
        {
            edit->scene->addChangeEventSettingsHistory(edit->LvlData.events[i].array_id, LvlScene::SETTING_EV_LSHOWADD, QVariant(ui->LVLEvents_layerList->currentItem()->text()));
            edit->LvlData.events[i].layers_show.push_back(ui->LVLEvents_layerList->currentItem()->text());
            edit->LvlData.modified=true;
            eventLayerVisiblySyncList();
        }
    }
}

void MainWindow::on_LVLEvent_Layer_ShowDel_clicked()
{
    if(lockSetEventSettings) return;

    int WinType = activeChildWindow();

    if (WinType==1)
    {
        leveledit * edit = activeLvlEditWin();
        long i = getEventArrayIndex();
        if(i<0) return;

        if(!ui->LVLEvent_Layer_ShowList->selectedItems().isEmpty())
        {
            for(int j=0; j<edit->LvlData.events[i].layers_show.size(); j++)
            {
            if(edit->LvlData.events[i].layers_show[j]==ui->LVLEvent_Layer_ShowList->currentItem()->text())
                {
                edit->scene->addChangeEventSettingsHistory(edit->LvlData.events[i].array_id, LvlScene::SETTING_EV_LSHOWDEL, QVariant(edit->LvlData.events[i].layers_show[j]));
                edit->LvlData.events[i].layers_show.removeAt(j);
                edit->LvlData.modified=true;
                break;
                }
            }
            eventLayerVisiblySyncList();
        }
    }

}

void MainWindow::on_LVLEvent_Layer_TogAdd_clicked()
{
    if(lockSetEventSettings) return;

    int WinType = activeChildWindow();

    if (WinType==1)
    {
        leveledit * edit = activeLvlEditWin();
        long i = getEventArrayIndex();
        if(i<0) return;

        if(!ui->LVLEvents_layerList->selectedItems().isEmpty())
        {
            edit->scene->addChangeEventSettingsHistory(edit->LvlData.events[i].array_id, LvlScene::SETTING_EV_LTOGADD, QVariant(ui->LVLEvents_layerList->currentItem()->text()));
            edit->LvlData.events[i].layers_toggle.push_back(ui->LVLEvents_layerList->currentItem()->text());
            edit->LvlData.modified=true;
            eventLayerVisiblySyncList();
        }
    }
}

void MainWindow::on_LVLEvent_Layer_TogDel_clicked()
{
    if(lockSetEventSettings) return;

    int WinType = activeChildWindow();

    if (WinType==1)
    {
        leveledit * edit = activeLvlEditWin();
        long i = getEventArrayIndex();
        if(i<0) return;

        if(!ui->LVLEvent_Layer_ToggleList->selectedItems().isEmpty())
        {
            for(int j=0; j<edit->LvlData.events[i].layers_toggle.size(); j++)
            {
            if(edit->LvlData.events[i].layers_toggle[j]==ui->LVLEvent_Layer_ToggleList->currentItem()->text())
                {
                    edit->scene->addChangeEventSettingsHistory(edit->LvlData.events[i].array_id, LvlScene::SETTING_EV_LTOGDEL, QVariant(edit->LvlData.events[i].layers_toggle[j]));
                    edit->LvlData.events[i].layers_toggle.removeAt(j);
                    edit->LvlData.modified=true;
                    break;
                }
            }
            eventLayerVisiblySyncList();
        }
    }
}






void MainWindow::on_LVLEvent_LayerMov_List_currentIndexChanged(int index)
{
    if(lockSetEventSettings) return;

    if(index<0) return;

    int WinType = activeChildWindow();

    if (WinType==1)
    {
        leveledit * edit = activeLvlEditWin();
        long i = getEventArrayIndex();
        if(i<0) return;

        QList<QVariant> movLayerData;
        movLayerData.push_back(edit->LvlData.events[i].movelayer);
        movLayerData.push_back(((index<=0)?"":ui->LVLEvent_LayerMov_List->currentText()));
        edit->scene->addChangeEventSettingsHistory(edit->LvlData.events[i].array_id, LvlScene::SETTING_EV_MOVELAYER, QVariant(movLayerData));
        edit->LvlData.events[i].movelayer = ((index<=0)?"":ui->LVLEvent_LayerMov_List->currentText());
        edit->LvlData.modified=true;
    }
}

void MainWindow::on_LVLEvent_LayerMov_spX_valueChanged(double arg1)
{
    if(lockSetEventSettings) return;

    int WinType = activeChildWindow();

    if (WinType==1)
    {
        leveledit * edit = activeLvlEditWin();
        long i = getEventArrayIndex();
        if(i<0) return;

        QList<QVariant> speedData;
        speedData.push_back(edit->LvlData.events[i].layer_speed_x);
        speedData.push_back(arg1);
        edit->scene->addChangeEventSettingsHistory(edit->LvlData.events[i].array_id, LvlScene::SETTING_EV_SPEEDLAYERX, QVariant(speedData));
        edit->LvlData.events[i].layer_speed_x = arg1;
        edit->LvlData.modified=true;
    }

}

void MainWindow::on_LVLEvent_LayerMov_spY_valueChanged(double arg1)
{
    if(lockSetEventSettings) return;

    int WinType = activeChildWindow();

    if (WinType==1)
    {
        leveledit * edit = activeLvlEditWin();
        long i = getEventArrayIndex();
        if(i<0) return;

        QList<QVariant> speedData;
        speedData.push_back(edit->LvlData.events[i].layer_speed_y);
        speedData.push_back(arg1);
        edit->scene->addChangeEventSettingsHistory(edit->LvlData.events[i].array_id, LvlScene::SETTING_EV_SPEEDLAYERY, QVariant(speedData));
        edit->LvlData.events[i].layer_speed_y = arg1;
        edit->LvlData.modified=true;
    }

}







void MainWindow::on_LVLEvent_Scroll_Sct_valueChanged(int arg1)
{
    if(lockSetEventSettings) return;

    int WinType = activeChildWindow();

    if (WinType==1)
    {
        leveledit * edit = activeLvlEditWin();
        long i = getEventArrayIndex();
        if(i<0) return;

        QList<QVariant> secData;
        secData.push_back((qlonglong)edit->LvlData.events[i].scroll_section);
        secData.push_back(arg1-1);
        edit->scene->addChangeEventSettingsHistory(edit->LvlData.events[i].array_id, LvlScene::SETTING_EV_AUTOSCRSEC, QVariant(secData));
        edit->LvlData.events[i].scroll_section = arg1-1;
        edit->LvlData.modified=true;
    }
}

void MainWindow::on_LVLEvent_Scroll_spX_valueChanged(double arg1)
{
    if(lockSetEventSettings) return;

    int WinType = activeChildWindow();

    if (WinType==1)
    {
        leveledit * edit = activeLvlEditWin();
        long i = getEventArrayIndex();
        if(i<0) return;

        QList<QVariant> scrollXData;
        scrollXData.push_back(edit->LvlData.events[i].move_camera_x);
        scrollXData.push_back(arg1);
        edit->scene->addChangeEventSettingsHistory(edit->LvlData.events[i].array_id, LvlScene::SETTING_EV_AUTOSCRX, QVariant(scrollXData));
        edit->LvlData.events[i].move_camera_x = arg1;
        edit->LvlData.modified=true;
    }

}

void MainWindow::on_LVLEvent_Scroll_spY_valueChanged(double arg1)
{
    if(lockSetEventSettings) return;

    int WinType = activeChildWindow();

    if (WinType==1)
    {
        leveledit * edit = activeLvlEditWin();
        long i = getEventArrayIndex();
        if(i<0) return;

        QList<QVariant> scrollYData;
        scrollYData.push_back(edit->LvlData.events[i].move_camera_y);
        scrollYData.push_back(arg1);
        edit->scene->addChangeEventSettingsHistory(edit->LvlData.events[i].array_id, LvlScene::SETTING_EV_AUTOSCRY, QVariant(scrollYData));
        edit->LvlData.events[i].move_camera_y = arg1;
        edit->LvlData.modified=true;
    }
}




void MainWindow::on_LVLEvent_Sct_list_currentIndexChanged(int index)
{
    if(lockSetEventSettings) return;
    if(index<0) return;
    curSectionField = index;
    eventSectionSettingsSync();
}

void MainWindow::on_LVLEvent_SctSize_none_clicked()
{
    if(lockSetEventSettings) return;

    int WinType = activeChildWindow();

    if (WinType==1)
    {
        lockEventSectionDataList=true;
        leveledit * edit = activeLvlEditWin();
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
        edit->scene->addChangeEventSettingsHistory(edit->LvlData.events[i].array_id, LvlScene::SETTING_EV_SECSIZE, QVariant(sizeData));

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

void MainWindow::on_LVLEvent_SctSize_reset_clicked()
{
    if(lockSetEventSettings) return;

    int WinType = activeChildWindow();

    if (WinType==1)
    {
        lockEventSectionDataList=true;
        leveledit * edit = activeLvlEditWin();
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
        edit->scene->addChangeEventSettingsHistory(edit->LvlData.events[i].array_id, LvlScene::SETTING_EV_SECSIZE, QVariant(sizeData));

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

void MainWindow::on_LVLEvent_SctSize_define_clicked()
{
    if(lockSetEventSettings) return;

    int WinType = activeChildWindow();

    if (WinType==1)
    {
        lockEventSectionDataList=true;
        leveledit * edit = activeLvlEditWin();
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
        edit->scene->addChangeEventSettingsHistory(edit->LvlData.events[i].array_id, LvlScene::SETTING_EV_SECSIZE, QVariant(sizeData));

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

void MainWindow::on_LVLEvent_SctSize_left_textEdited(const QString &arg1)
{
    if(lockSetEventSettings) return;
    if(lockEventSectionDataList) return;

    int WinType = activeChildWindow();

    if (WinType==1)
    {
        lockEventSectionDataList=true;
        leveledit * edit = activeLvlEditWin();
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
        edit->scene->addChangeEventSettingsHistory(edit->LvlData.events[i].array_id, LvlScene::SETTING_EV_SECSIZE, QVariant(sizeData));

        edit->LvlData.events[i].sets[curSectionField].position_left = arg1.toInt();
    }
    lockEventSectionDataList=false;
}

void MainWindow::on_LVLEvent_SctSize_top_textEdited(const QString &arg1)
{
    if(lockSetEventSettings) return;
    if(lockEventSectionDataList) return;


    int WinType = activeChildWindow();

    if (WinType==1)
    {
        lockEventSectionDataList=true;
        leveledit * edit = activeLvlEditWin();
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
        edit->scene->addChangeEventSettingsHistory(edit->LvlData.events[i].array_id, LvlScene::SETTING_EV_SECSIZE, QVariant(sizeData));

        edit->LvlData.events[i].sets[curSectionField].position_top = arg1.toInt();
    }
    lockEventSectionDataList=false;

}

void MainWindow::on_LVLEvent_SctSize_bottom_textEdited(const QString &arg1)
{
    if(lockSetEventSettings) return;
    if(lockEventSectionDataList) return;

    int WinType = activeChildWindow();

    if (WinType==1)
    {
        lockEventSectionDataList=true;
        leveledit * edit = activeLvlEditWin();
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
        edit->scene->addChangeEventSettingsHistory(edit->LvlData.events[i].array_id, LvlScene::SETTING_EV_SECSIZE, QVariant(sizeData));

        edit->LvlData.events[i].sets[curSectionField].position_bottom = arg1.toInt();
    }
    lockEventSectionDataList=false;
}

void MainWindow::on_LVLEvent_SctSize_right_textEdited(const QString &arg1)
{
    if(lockSetEventSettings) return;
    if(lockEventSectionDataList) return;

    int WinType = activeChildWindow();

    if (WinType==1)
    {
        lockEventSectionDataList=true;
        leveledit * edit = activeLvlEditWin();
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
        edit->scene->addChangeEventSettingsHistory(edit->LvlData.events[i].array_id, LvlScene::SETTING_EV_SECSIZE, QVariant(sizeData));

        edit->LvlData.events[i].sets[curSectionField].position_right = arg1.toInt();
    }
    lockEventSectionDataList=false;

}

void MainWindow::on_LVLEvent_SctSize_Set_clicked()
{
    if(lockSetEventSettings) return;
    if(lockEventSectionDataList) return;

    int WinType = activeChildWindow();

    if (WinType==1)
    {
        leveledit * edit = activeLvlEditWin();
        if(curSectionField!=edit->LvlData.CurSection)
        {
            QMessageBox::information(this, tr("Get section size"),
             tr("Please, set current section to %1 for capture data for this event").arg(curSectionField+1),
             QMessageBox::Ok);
            return;
        }

        long i = getEventArrayIndex();
        if(i<0) return;

        activeLvlEditWin()->setFocus();
        if(activeLvlEditWin()->scene->pResizer==NULL)
        {
            activeLvlEditWin()->goTo(edit->LvlData.events[i].sets[curSectionField].position_left,
                                     edit->LvlData.events[i].sets[curSectionField].position_top,
                                     false,
                                     QPoint(-10,-10));
            activeLvlEditWin()->scene->setEventSctSizeResizer(i, true);
        }
    }

}

void MainWindow::on_LVLEvent_SctMus_none_clicked()
{
    if(lockSetEventSettings) return;
    if(lockEventSectionDataList) return;

    int WinType = activeChildWindow();

    if (WinType==1)
    {
        ui->LVLEvent_SctMus_List->setEnabled(false);
        lockEventSectionDataList=true;
        leveledit * edit = activeLvlEditWin();
        long i = getEventArrayIndex();
        if(i<0) return;

        QList<QVariant> musData;
        musData.push_back((qlonglong)curSectionField);
        musData.push_back((qlonglong)edit->LvlData.events[i].sets[curSectionField].music_id);
        musData.push_back((qlonglong)-1);
        edit->scene->addChangeEventSettingsHistory(edit->LvlData.events[i].array_id, LvlScene::SETTING_EV_SECMUS, QVariant(musData));
        edit->LvlData.events[i].sets[curSectionField].music_id = -1;
    }
    lockEventSectionDataList=false;

}

void MainWindow::on_LVLEvent_SctMus_reset_clicked()
{
    if(lockSetEventSettings) return;
    if(lockEventSectionDataList) return;

    int WinType = activeChildWindow();

    if (WinType==1)
    {
        ui->LVLEvent_SctMus_List->setEnabled(false);
        lockEventSectionDataList=true;
        leveledit * edit = activeLvlEditWin();
        long i = getEventArrayIndex();
        if(i<0) return;

        QList<QVariant> musData;
        musData.push_back((qlonglong)curSectionField);
        musData.push_back((qlonglong)edit->LvlData.events[i].sets[curSectionField].music_id);
        musData.push_back((qlonglong)-2);
        edit->scene->addChangeEventSettingsHistory(edit->LvlData.events[i].array_id, LvlScene::SETTING_EV_SECMUS, QVariant(musData));
        edit->LvlData.events[i].sets[curSectionField].music_id = -2;
    }
    lockEventSectionDataList=false;
}

void MainWindow::on_LVLEvent_SctMus_define_clicked()
{
    if(lockSetEventSettings) return;
    if(lockEventSectionDataList) return;

    int WinType = activeChildWindow();

    if (WinType==1)
    {
        ui->LVLEvent_SctMus_List->setEnabled(true);
        lockEventSectionDataList=true;
        leveledit * edit = activeLvlEditWin();
        long i = getEventArrayIndex();
        if(i<0) return;

        QList<QVariant> musData;
        musData.push_back((qlonglong)curSectionField);
        musData.push_back((qlonglong)edit->LvlData.events[i].sets[curSectionField].music_id);
        musData.push_back((qlonglong)ui->LVLEvent_SctMus_List->currentData().toInt());
        edit->scene->addChangeEventSettingsHistory(edit->LvlData.events[i].array_id, LvlScene::SETTING_EV_SECMUS, QVariant(musData));
        edit->LvlData.events[i].sets[curSectionField].music_id = ui->LVLEvent_SctMus_List->currentData().toInt();
    }
    lockEventSectionDataList=false;
}

void MainWindow::on_LVLEvent_SctMus_List_currentIndexChanged(int index)
{
    if(lockSetEventSettings) return;
    if(lockEventSectionDataList) return;
    if(index<0) return;

    int WinType = activeChildWindow();

    if (WinType==1)
    {
        lockEventSectionDataList=true;
        leveledit * edit = activeLvlEditWin();
        long i = getEventArrayIndex();
        if(i<0) return;

        QList<QVariant> musData;
        musData.push_back((qlonglong)curSectionField);
        musData.push_back((qlonglong)edit->LvlData.events[i].sets[curSectionField].music_id);
        musData.push_back((qlonglong)ui->LVLEvent_SctMus_List->itemData(index).toInt());
        edit->scene->addChangeEventSettingsHistory(edit->LvlData.events[i].array_id, LvlScene::SETTING_EV_SECMUS, QVariant(musData));
        edit->LvlData.events[i].sets[curSectionField].music_id = ui->LVLEvent_SctMus_List->itemData(index).toInt();
    }
    lockEventSectionDataList=false;
}

void MainWindow::on_LVLEvent_SctBg_none_clicked()
{
    if(lockSetEventSettings) return;
    if(lockEventSectionDataList) return;

    int WinType = activeChildWindow();

    if (WinType==1)
    {
        ui->LVLEvent_SctBg_List->setEnabled(false);
        lockEventSectionDataList=true;
        leveledit * edit = activeLvlEditWin();
        long i = getEventArrayIndex();
        if(i<0) return;

        QList<QVariant> bgData;
        bgData.push_back((qlonglong)curSectionField);
        bgData.push_back((qlonglong)edit->LvlData.events[i].sets[curSectionField].background_id);
        bgData.push_back((qlonglong)-1);
        edit->scene->addChangeEventSettingsHistory(edit->LvlData.events[i].array_id, LvlScene::SETTING_EV_SECBG, QVariant(bgData));
        edit->LvlData.events[i].sets[curSectionField].background_id = -1;
    }
    lockEventSectionDataList=false;

}

void MainWindow::on_LVLEvent_SctBg_reset_clicked()
{
    if(lockSetEventSettings) return;
    if(lockEventSectionDataList) return;

    int WinType = activeChildWindow();

    if (WinType==1)
    {
        ui->LVLEvent_SctBg_List->setEnabled(false);
        lockEventSectionDataList=true;
        leveledit * edit = activeLvlEditWin();
        long i = getEventArrayIndex();
        if(i<0) return;

        QList<QVariant> bgData;
        bgData.push_back((qlonglong)curSectionField);
        bgData.push_back((qlonglong)edit->LvlData.events[i].sets[curSectionField].background_id);
        bgData.push_back((qlonglong)-2);
        edit->scene->addChangeEventSettingsHistory(edit->LvlData.events[i].array_id, LvlScene::SETTING_EV_SECBG, QVariant(bgData));
        edit->LvlData.events[i].sets[curSectionField].background_id = -2;
    }
    lockEventSectionDataList=false;

}

void MainWindow::on_LVLEvent_SctBg_define_clicked()
{
    if(lockSetEventSettings) return;
    if(lockEventSectionDataList) return;

    int WinType = activeChildWindow();

    if (WinType==1)
    {
        ui->LVLEvent_SctBg_List->setEnabled(true);
        lockEventSectionDataList=true;
        leveledit * edit = activeLvlEditWin();
        long i = getEventArrayIndex();
        if(i<0) return;

        QList<QVariant> bgData;
        bgData.push_back((qlonglong)curSectionField);
        bgData.push_back((qlonglong)edit->LvlData.events[i].sets[curSectionField].background_id);
        bgData.push_back((qlonglong)ui->LVLEvent_SctBg_List->currentData().toInt());
        edit->scene->addChangeEventSettingsHistory(edit->LvlData.events[i].array_id, LvlScene::SETTING_EV_SECBG, QVariant(bgData));
        edit->LvlData.events[i].sets[curSectionField].background_id = ui->LVLEvent_SctBg_List->currentData().toInt();
    }
    lockEventSectionDataList=false;

}

void MainWindow::on_LVLEvent_SctBg_List_currentIndexChanged(int index)
{
    if(lockSetEventSettings) return;
    if(lockEventSectionDataList) return;
    if(index<0) return;


    int WinType = activeChildWindow();

    if (WinType==1)
    {
        lockEventSectionDataList=true;
        leveledit * edit = activeLvlEditWin();
        long i = getEventArrayIndex();
        if(i<0) return;

        QList<QVariant> bgData;
        bgData.push_back((qlonglong)curSectionField);
        bgData.push_back((qlonglong)edit->LvlData.events[i].sets[curSectionField].background_id);
        bgData.push_back((qlonglong)ui->LVLEvent_SctBg_List->itemData(index).toInt());
        edit->scene->addChangeEventSettingsHistory(edit->LvlData.events[i].array_id, LvlScene::SETTING_EV_SECBG, QVariant(bgData));
        edit->LvlData.events[i].sets[curSectionField].background_id = ui->LVLEvent_SctBg_List->itemData(index).toInt();
    }
    lockEventSectionDataList=false;
}





void MainWindow::on_LVLEvent_Cmn_Msg_clicked()
{
    if(currentEventArrayID<0) return;

    int WinType = activeChildWindow();

    if (WinType==1)
    {
        leveledit * edit = activeLvlEditWin();
        long i = getEventArrayIndex();
        if(i<0) return;

        ItemMsgBox * msgBox = new ItemMsgBox(edit->LvlData.events[i].msg,
                tr("Please, enter message\nMessage limits: max line lenth is 28 characters"));
        msgBox->setWindowFlags (Qt::Window | Qt::WindowTitleHint | Qt::WindowCloseButtonHint);
        msgBox->setGeometry(QStyle::alignedRect(Qt::LeftToRight, Qt::AlignCenter, msgBox->size(), qApp->desktop()->availableGeometry()));
        if(msgBox->exec()==QDialog::Accepted)
        {
            QList<QVariant> msgData;
            msgData.push_back(edit->LvlData.events[i].msg);
            msgData.push_back(msgBox->currentText);
            edit->scene->addChangeEventSettingsHistory(edit->LvlData.events[i].array_id, LvlScene::SETTING_EV_MSG, QVariant(msgData));

            edit->LvlData.events[i].msg = msgBox->currentText;
            QString evnmsg = (edit->LvlData.events[i].msg.isEmpty() ? tr("[none]") : edit->LvlData.events[i].msg);
            if(evnmsg.size()>20)
            {
                evnmsg.resize(18);
                evnmsg.push_back("...");
            }
            ui->LVLEvent_Cmn_Msg->setText( evnmsg );
            edit->LvlData.modified=true;
        }

    }
}


void MainWindow::on_LVLEvent_Cmn_PlaySnd_currentIndexChanged(int index)
{
    if(lockSetEventSettings) return;
    if(index<0) return;

    int WinType = activeChildWindow();

    if (WinType==1)
    {
        leveledit * edit = activeLvlEditWin();
        long i = getEventArrayIndex();
        if(i<0) return;
        QList<QVariant> soundData;
        soundData.push_back((qlonglong)edit->LvlData.events[i].sound_id);
        soundData.push_back((qlonglong)ui->LVLEvent_Cmn_PlaySnd->currentData().toInt());
        edit->scene->addChangeEventSettingsHistory(edit->LvlData.events[i].array_id, LvlScene::SETTING_EV_SOUND, QVariant(soundData));

        edit->LvlData.events[i].sound_id = ui->LVLEvent_Cmn_PlaySnd->currentData().toInt();
        edit->LvlData.modified=true;
    }
}

void MainWindow::on_LVLEvent_playSnd_clicked()
{
    if(ui->LVLEvent_Cmn_PlaySnd->currentData().toInt()==0) return;

    QString sndPath = configs.dirs.sounds;
    long i;
    bool found=false;
    for(i=0; i<configs.main_sound.size(); i++)
    {
        if((unsigned int)ui->LVLEvent_Cmn_PlaySnd->currentData().toInt()==configs.main_sound[i].id)
        {
            found=true;
            sndPath += configs.main_sound[i].file;
            break;
        }
    }

    WriteToLog(QtDebugMsg, QString("Test Sound -> path-1 %1").arg(sndPath));

    if(!found) return;
    if(!QFileInfo::exists(sndPath)) return;

    playSnd.setMedia(QMediaContent(QUrl(sndPath)));
    playSnd.setVolume(100);
    playSnd.play();
}



void MainWindow::on_LVLEvent_Cmn_EndGame_currentIndexChanged(int index)
{
    if(lockSetEventSettings) return;
    if(index<0) return;

    int WinType = activeChildWindow();

    if (WinType==1)
    {
        leveledit * edit = activeLvlEditWin();
        long i = getEventArrayIndex();
        if(i<0) return;
        QList<QVariant> endData;
        endData.push_back((qlonglong)edit->LvlData.events[i].end_game);
        endData.push_back((qlonglong)ui->LVLEvent_Cmn_EndGame->currentIndex());
        edit->scene->addChangeEventSettingsHistory(edit->LvlData.events[i].array_id, LvlScene::SETTING_EV_ENDGAME, QVariant(endData));

        edit->LvlData.events[i].end_game = ui->LVLEvent_Cmn_EndGame->currentIndex();
        edit->LvlData.modified=true;
    }

}







void MainWindow::on_LVLEvent_Key_Up_clicked(bool checked)
{

    if(lockSetEventSettings) return;

    int WinType = activeChildWindow();

    if (WinType==1)
    {
        leveledit * edit = activeLvlEditWin();
        long i = getEventArrayIndex();
        if(i<0) return;

        edit->scene->addChangeEventSettingsHistory(edit->LvlData.events[i].array_id, LvlScene::SETTING_EV_KUP, QVariant(checked));
        edit->LvlData.events[i].up = checked;
        edit->LvlData.modified=true;
    }

}

void MainWindow::on_LVLEvent_Key_Down_clicked(bool checked)
{
    if(lockSetEventSettings) return;

    int WinType = activeChildWindow();

    if (WinType==1)
    {
        leveledit * edit = activeLvlEditWin();
        long i = getEventArrayIndex();
        if(i<0) return;

        edit->scene->addChangeEventSettingsHistory(edit->LvlData.events[i].array_id, LvlScene::SETTING_EV_KDOWN, QVariant(checked));
        edit->LvlData.events[i].down = checked;
        edit->LvlData.modified=true;
    }
}

void MainWindow::on_LVLEvent_Key_Left_clicked(bool checked)
{
    if(lockSetEventSettings) return;

    int WinType = activeChildWindow();

    if (WinType==1)
    {
        leveledit * edit = activeLvlEditWin();
        long i = getEventArrayIndex();
        if(i<0) return;

        edit->scene->addChangeEventSettingsHistory(edit->LvlData.events[i].array_id, LvlScene::SETTING_EV_KLEFT, QVariant(checked));
        edit->LvlData.events[i].left = checked;
        edit->LvlData.modified=true;
    }
}

void MainWindow::on_LVLEvent_Key_Right_clicked(bool checked)
{
    if(lockSetEventSettings) return;

    int WinType = activeChildWindow();

    if (WinType==1)
    {
        leveledit * edit = activeLvlEditWin();
        long i = getEventArrayIndex();
        if(i<0) return;

        edit->scene->addChangeEventSettingsHistory(edit->LvlData.events[i].array_id, LvlScene::SETTING_EV_KRIGHT, QVariant(checked));
        edit->LvlData.events[i].right = checked;
        edit->LvlData.modified=true;
    }
}

void MainWindow::on_LVLEvent_Key_Run_clicked(bool checked)
{
    if(lockSetEventSettings) return;

    int WinType = activeChildWindow();

    if (WinType==1)
    {
        leveledit * edit = activeLvlEditWin();
        long i = getEventArrayIndex();
        if(i<0) return;

        edit->scene->addChangeEventSettingsHistory(edit->LvlData.events[i].array_id, LvlScene::SETTING_EV_KRUN, QVariant(checked));
        edit->LvlData.events[i].run = checked;
        edit->LvlData.modified=true;
    }
}

void MainWindow::on_LVLEvent_Key_AltRun_clicked(bool checked)
{
    if(lockSetEventSettings) return;

    int WinType = activeChildWindow();

    if (WinType==1)
    {
        leveledit * edit = activeLvlEditWin();
        long i = getEventArrayIndex();
        if(i<0) return;

        edit->scene->addChangeEventSettingsHistory(edit->LvlData.events[i].array_id, LvlScene::SETTING_EV_KALTRUN, QVariant(checked));
        edit->LvlData.events[i].altrun = checked;
        edit->LvlData.modified=true;
    }
}

void MainWindow::on_LVLEvent_Key_Jump_clicked(bool checked)
{
    if(lockSetEventSettings) return;

    int WinType = activeChildWindow();

    if (WinType==1)
    {
        leveledit * edit = activeLvlEditWin();
        long i = getEventArrayIndex();
        if(i<0) return;

        edit->scene->addChangeEventSettingsHistory(edit->LvlData.events[i].array_id, LvlScene::SETTING_EV_KJUMP, QVariant(checked));
        edit->LvlData.events[i].jump = checked;
        edit->LvlData.modified=true;
    }
}

void MainWindow::on_LVLEvent_Key_AltJump_clicked(bool checked)
{
    if(lockSetEventSettings) return;

    int WinType = activeChildWindow();

    if (WinType==1)
    {
        leveledit * edit = activeLvlEditWin();
        long i = getEventArrayIndex();
        if(i<0) return;

        edit->scene->addChangeEventSettingsHistory(edit->LvlData.events[i].array_id, LvlScene::SETTING_EV_KALTJUMP, QVariant(checked));
        edit->LvlData.events[i].altjump = checked;
        edit->LvlData.modified=true;
    }
}

void MainWindow::on_LVLEvent_Key_Drop_clicked(bool checked)
{
    if(lockSetEventSettings) return;

    int WinType = activeChildWindow();

    if (WinType==1)
    {
        leveledit * edit = activeLvlEditWin();
        long i = getEventArrayIndex();
        if(i<0) return;

        edit->scene->addChangeEventSettingsHistory(edit->LvlData.events[i].array_id, LvlScene::SETTING_EV_KDROP, QVariant(checked));
        edit->LvlData.events[i].drop = checked;
        edit->LvlData.modified=true;
    }
}

void MainWindow::on_LVLEvent_Key_Start_clicked(bool checked)
{
    if(lockSetEventSettings) return;

    int WinType = activeChildWindow();

    if (WinType==1)
    {
        leveledit * edit = activeLvlEditWin();
        long i = getEventArrayIndex();
        if(i<0) return;

        edit->scene->addChangeEventSettingsHistory(edit->LvlData.events[i].array_id, LvlScene::SETTING_EV_KSTART, QVariant(checked));
        edit->LvlData.events[i].start = checked;
        edit->LvlData.modified=true;
    }
}




void MainWindow::on_LVLEvent_TriggerEvent_currentIndexChanged(int index)
{
    if(lockSetEventSettings) return;
    if(index<0) return;

    int WinType = activeChildWindow();

    if (WinType==1)
    {
        leveledit * edit = activeLvlEditWin();
        long i = getEventArrayIndex();
        if(i<0) return;

        QList<QVariant> triggerData;
        triggerData.push_back(edit->LvlData.events[i].trigger);
        triggerData.push_back(ui->LVLEvent_TriggerEvent->currentText());
        edit->scene->addChangeEventSettingsHistory(edit->LvlData.events[i].array_id, LvlScene::SETTING_EV_TRIACTIVATE, QVariant(triggerData));

        edit->LvlData.events[i].trigger = ui->LVLEvent_TriggerEvent->currentText();
        edit->LvlData.modified=true;
    }

}

void MainWindow::on_LVLEvent_TriggerDelay_valueChanged(double arg1)
{
    if(lockSetEventSettings) return;

    int WinType = activeChildWindow();

    if (WinType==1)
    {
        leveledit * edit = activeLvlEditWin();
        long i = getEventArrayIndex();
        if(i<0) return;

        QList<QVariant> triggerData;
        triggerData.push_back((qlonglong)edit->LvlData.events[i].trigger_timer);
        triggerData.push_back((qlonglong)qRound(arg1*10));
        edit->scene->addChangeEventSettingsHistory(edit->LvlData.events[i].array_id, LvlScene::SETTING_EV_TRIDELAY, QVariant(triggerData));

        edit->LvlData.events[i].trigger_timer = qRound(arg1*10);
        edit->LvlData.modified=true;
    }

}
