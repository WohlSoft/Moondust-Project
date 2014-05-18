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


#include "../ui_mainwindow.h"
#include "../mainwindow.h"



void MainWindow::on_LevelLayers_visibilityChanged(bool visible)
{
    ui->actionLayersBox->setChecked(visible);
}
void MainWindow::on_actionLayersBox_triggered(bool checked)
{
    ui->LevelLayers->setVisible(checked);
    if(checked) ui->LevelLayers->raise();
}




void MainWindow::setLayersBox()
{
    int WinType = activeChildWindow();
    QListWidgetItem * item;

    ui->LvlLayerList->clear();

    if (WinType==1)
    {
        foreach(LevelLayers layer, activeLvlEditWin()->LvlData.layers)
        {
            item = new QListWidgetItem;
            item->setText(layer.name);
            item->setFlags(Qt::ItemIsUserCheckable);

            if((layer.name!="Destroyed Blocks")&&(layer.name!="Spawned NPCs"))
                item->setFlags(item->flags() | Qt::ItemIsEnabled);

            if(layer.name!="Default")
                item->setFlags(item->flags() | Qt::ItemIsEditable | Qt::ItemIsDragEnabled | Qt::ItemIsSelectable);

            item->setCheckState( (layer.hidden) ? Qt::Unchecked: Qt::Checked );
            ui->LvlLayerList->addItem( item );
        }

    }
}



void MainWindow::on_AddLayer_clicked()
{
    QListWidgetItem * item;
    item = new QListWidgetItem;
    item->setText(tr("New Layer %1").arg( ui->LvlLayerList->count()+1 ) );
    item->setFlags(Qt::ItemIsUserCheckable | Qt::ItemIsEditable);
    item->setFlags(item->flags() | Qt::ItemIsEnabled);
    item->setFlags(item->flags() | Qt::ItemIsEditable | Qt::ItemIsDragEnabled | Qt::ItemIsSelectable);
    item->setCheckState( Qt::Checked );
    item->setData(3, QString("NewLayer") );
    ui->LvlLayerList->addItem( item );

    ui->LvlLayerList->setFocus();
    ui->LvlLayerList->editItem( item );

}
