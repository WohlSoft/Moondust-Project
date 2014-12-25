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

#include <QMenu>

#include <common_features/util.h>
#include <common_features/mainwinconnect.h>

#include "custom_counter_gui.h"
#include "ui_custom_counter_gui.h"

CustomCounterGUI::CustomCounterGUI(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CustomCounterGUI)
{
    ui->setupUi(this);
    if(MainWinConnect::pMainWin->activeChildWindow()==1)
    {
        ui->ItemType->addItem(tr("Block"), QVariant(int(ItemTypes::LVL_Block)));
        ui->ItemType->addItem(tr("BGO"), QVariant(int(ItemTypes::LVL_BGO)));
        ui->ItemType->addItem(tr("NPC"), QVariant(int(ItemTypes::LVL_NPC)));
    }
    else
    if(MainWinConnect::pMainWin->activeChildWindow()==3)
    {
        ui->ItemType->addItem(tr("Tile"), QVariant(int(ItemTypes::WLD_Tile)));
        ui->ItemType->addItem(tr("Scenery"), QVariant(int(ItemTypes::WLD_Scenery)));
        ui->ItemType->addItem(tr("Path"), QVariant(int(ItemTypes::WLD_Path)));
        ui->ItemType->addItem(tr("Level"), QVariant(int(ItemTypes::WLD_Level)));
        ui->ItemType->addItem(tr("Music"), QVariant(int(ItemTypes::WLD_MusicBox)));
    }
}


CustomCounterGUI::~CustomCounterGUI()
{
    delete ui;
}

void CustomCounterGUI::setCounterData(CustomCounter &data)
{
    counterData=data;
    util::memclear(ui->ItemList);
    foreach(long x, counterData.items)
    {
        QListWidgetItem *item;
        item = new QListWidgetItem(ui->ItemList);
        item->setData(Qt::UserRole, QVariant((int)x));
        item->setText(QString("Item-%1").arg(x));
        ui->ItemList->addItem(item);
    }

}

void CustomCounterGUI::on_ItemType_currentIndexChanged(int)
{

}

void CustomCounterGUI::on_ItemList_customContextMenuRequested(const QPoint &pos)
{
    if(ui->ItemList->selectedItems().isEmpty()) return;

    QMenu menu;
    QAction* replace = menu.addAction("Set another item");
    QAction* remove = menu.addAction("Remove");
    menu.exec(pos);

}

void CustomCounterGUI::on_buttonBox_accepted()
{

}

void CustomCounterGUI::on_buttonBox_rejected()
{

}
