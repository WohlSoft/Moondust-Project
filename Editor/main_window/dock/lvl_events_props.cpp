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

#include "../../ui_mainwindow.h"
#include "../../mainwindow.h"


#include "../../level_scene/item_bgo.h"
#include "../../level_scene/item_block.h"
#include "../../level_scene/item_npc.h"
#include "../../level_scene/item_water.h"

#include "../../file_formats/file_formats.h"

static long currentEventArrayID=0;
static bool lockSetEventSettings=false;

void MainWindow::setEventsBox()
{
    int WinType = activeChildWindow();
    QListWidgetItem * item;

    ui->LVLEvents_List->clear();

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

            //item->setCheckState( (layer.hidden) ? Qt::Unchecked: Qt::Checked );
            item->setData(3, QString::number( event.array_id ) );
            ui->LVLEvents_List->addItem( item );
        }

        on_LVLEvents_List_itemSelectionChanged();
    }
}

void MainWindow::EventListsSync()
{
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
        }
    }

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
    long cIndex;
    bool found=false;
    if(index==-2)
        {
        if(!ui->LVLEvents_List->selectedItems().isEmpty())
            cIndex = ui->LVLEvents_List->currentItem()->data(3).toInt();
        else
            cIndex = currentEventArrayID;
        }
    else
        cIndex = index;

    int WinType = activeChildWindow();
    if (WinType==1)
    {

        if( (activeLvlEditWin()->LvlData.events.size() > 0) && (cIndex >= 0))
        {
            leveledit * edit = activeLvlEditWin();
            currentEventArrayID=cIndex;
            foreach(LevelEvents event, edit->LvlData.events)
            {
                if(event.array_id == (unsigned int)index)
                {
                    currentEventArrayID=event.array_id;

                    //Enable controls
                    ui->LVLEvents_Settings->setEnabled(true);
                    ui->LVLEvent_AutoStart->setEnabled(true);

                    //Set controls data
                    ui->LVLEvent_AutoStart->setChecked( event.autostart );

                    //Layers visibly - layerList
                    ui->LVLEvents_layerList->clear();
                    ui->LVLEvent_Layer_HideList->clear();
                    ui->LVLEvent_Layer_ShowList->clear();
                    ui->LVLEvent_Layer_ToggleList->clear();

                    QListWidgetItem * item;
                    //Total layers list
                    foreach(LevelLayers layer, activeLvlEditWin()->LvlData.layers)
                    {
                        item = new QListWidgetItem;
                        item->setText(layer.name);
                        item->setFlags(item->flags() | Qt::ItemIsEnabled | Qt::ItemIsSelectable);
                        item->setData(3, QString::number( layer.array_id ) );
                        ui->LVLEvents_layerList->addItem( item );
                    }

                    ui->LVLEvent_disableSmokeEffect->setChecked( event.nosmoke );

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
                    ui->LVLEvent_Scroll_Sct->setValue( event.scroll_section );
                    ui->LVLEvent_Scroll_spX->setValue( event.move_camera_x );
                    ui->LVLEvent_Scroll_spY->setValue( event.move_camera_y );

                    //Section Settings
                    ui->LVLEvent_Sct_list->clear();
                    for(int i=0; i<edit->LvlData.sections.size(); i++)
                        ui->LVLEvent_Sct_list->addItem(tr("Section")+QString(" ")+QString::number(i+1), QString::number(i));

                    if(ui->LVLEvent_Sct_list->currentData().toInt()<event.sets.size())
                    {
                        ui->LVLEvent_SctSize_left->setText("");
                        ui->LVLEvent_SctSize_top->setText("");
                        ui->LVLEvent_SctSize_bottom->setText("");
                        ui->LVLEvent_SctSize_right->setText("");
                        ui->LVLEvent_SctSize_left->setEnabled(false);
                        ui->LVLEvent_SctSize_top->setEnabled(false);
                        ui->LVLEvent_SctSize_bottom->setEnabled(false);
                        ui->LVLEvent_SctSize_right->setEnabled(false);
                    switch(event.sets[ui->LVLEvent_Sct_list->currentData().toInt()].position_left)
                        {
                        case -1:
                            ui->LVLEvent_SctSize_none->setChecked(true);
                            break;
                        case -2:
                            ui->LVLEvent_SctSize_reset->setChecked(true);
                            break;
                        default:
                            ui->LVLEvent_SctSize_define->setChecked(true);
                            ui->LVLEvent_SctSize_left->setText(QString::number(event.sets[ui->LVLEvent_Sct_list->currentData().toInt()].position_left));
                            ui->LVLEvent_SctSize_top->setText(QString::number(event.sets[ui->LVLEvent_Sct_list->currentData().toInt()].position_top));
                            ui->LVLEvent_SctSize_bottom->setText(QString::number(event.sets[ui->LVLEvent_Sct_list->currentData().toInt()].position_bottom));
                            ui->LVLEvent_SctSize_right->setText(QString::number(event.sets[ui->LVLEvent_Sct_list->currentData().toInt()].position_right));
                            ui->LVLEvent_SctSize_left->setEnabled(true);
                            ui->LVLEvent_SctSize_top->setEnabled(true);
                            ui->LVLEvent_SctSize_bottom->setEnabled(true);
                            ui->LVLEvent_SctSize_right->setEnabled(true);
                            break;
                        }

                    ui->LVLEvent_SctMus_List->setEnabled(false);
                    switch(event.sets[ui->LVLEvent_Sct_list->currentData().toInt()].music_id)
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
                            break;
                        }

                    ui->LVLEvent_SctBg_List->setEnabled(false);
                    switch(event.sets[ui->LVLEvent_Sct_list->currentData().toInt()].background_id)
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
                            break;
                        }


                    }

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
    int WinType = activeChildWindow();

    if (WinType==1)
    {
        if(item->data(3).toString()=="NewEvent")
        {
            bool AlreadyExist=false;
            foreach(LevelEvents event, activeLvlEditWin()->LvlData.events)
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
                return;
            }
            else
            {
                LevelEvents NewEvent = FileFormats::dummyLvlEvent();
                NewEvent.name = item->text();
                activeLvlEditWin()->LvlData.events_array_id++;
                NewEvent.array_id = activeLvlEditWin()->LvlData.events_array_id;

                item->setData(3, QString::number(NewEvent.array_id));

                activeLvlEditWin()->LvlData.events.push_back(NewEvent);
                activeLvlEditWin()->LvlData.modified=true;
            }

        }//if(item->data(3).toString()=="NewEvent")
        else
        {
            //QString eventName = item->text();
            //QString oldEventName = item->text();

            //ModifyLayerItem(item, oldLayerName, layerName, layerVisible);
            activeLvlEditWin()->LvlData.modified=true;
        }

    }//if WinType==1
}

