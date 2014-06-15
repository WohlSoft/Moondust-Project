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
            ui->LVLEvent_Cmn_PlaySnd->addItem(snd.name, QString::number(snd.id) );
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


                    //Scroll section / Move Camera
                    ui->LVLEvent_Scroll_Sct->setMaximum( edit->LvlData.sections.size() );
                    ui->LVLEvent_Scroll_Sct->setValue( event.scroll_section );
                    ui->LVLEvent_Scroll_spX->setValue( event.move_camera_x );
                    ui->LVLEvent_Scroll_spY->setValue( event.move_camera_y );

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

