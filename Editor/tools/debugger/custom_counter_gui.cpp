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

#include "custom_counter_gui.h"
#include "ui_custom_counter_gui.h"

CustomCounterGUI::CustomCounterGUI(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CustomCounterGUI)
{
    ui->setupUi(this);
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
        ui->ItemList->addItem(QString("Item-%1").arg(x));
    }

}

void CustomCounterGUI::on_ItemType_currentIndexChanged(int index)
{

}

void CustomCounterGUI::on_ItemList_customContextMenuRequested(const QPoint &pos)
{
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
