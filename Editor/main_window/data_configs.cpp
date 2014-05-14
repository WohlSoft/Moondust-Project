
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


dataconfigs * MainWindow::getConfigs()
{
    dataconfigs * pointer;
    pointer = &configs;
    return pointer;
}



void MainWindow::on_actionLoad_configs_triggered()
{
    thread1->start();
    moveToThread(thread1);

    QProgressDialog progress(tr("Reloading configurations"), tr("Abort"), 0,100, this);
    progress.setWindowTitle("Please, wait...");
    progress.setWindowModality(Qt::WindowModal);
    progress.setWindowFlags(Qt::Window | Qt::WindowTitleHint | Qt::CustomizeWindowHint | Qt::WindowStaysOnTopHint);
    progress.setFixedSize(progress.size());
    progress.setGeometry(QStyle::alignedRect(Qt::LeftToRight, Qt::AlignCenter, progress.size(), qApp->desktop()->availableGeometry()));
    progress.setCancelButton(0);
    progress.show();

    if(!progress.wasCanceled()) progress.setValue(1);

    //Reload configs
    configs.loadconfigs();

    if(!progress.wasCanceled())  progress.setValue(100);

    setItemBoxes(); //Apply item boxes from reloaded configs

    //Set tools from loaded configs
    //setTools();

    if(!progress.wasCanceled())
        progress.close();

    QMessageBox::information(this, tr("Reload configuration"),
    tr("Configuration succesfully reloaded!"),
    QMessageBox::Ok);
}




void MainWindow::setTools()
{
    int i;
        WriteToLog(QtDebugMsg, QString("Set toolboxes"));
    ui->LVLPropsBackImage->clear();
    ui->LVLPropsMusicNumber->clear();

    ui->LVLPropsBackImage->addItem( tr("[No image]") );
    ui->LVLPropsMusicNumber->addItem( tr("[Silence]") );

    for(i=0; i< configs.main_bg.size();i++)
        ui->LVLPropsBackImage->addItem(configs.main_bg[i].name);

    for(i=0; i< configs.main_music_lvl.size();i++)
        ui->LVLPropsMusicNumber->addItem(configs.main_music_lvl[i].name);

}

void MainWindow::setItemBoxes()
{
        WriteToLog(QtDebugMsg, "BGOTools -> Clear current (disabled)");
    ui->BGOItemsList->clear();
    ui->BlockItemsList->clear();

        WriteToLog(QtDebugMsg, "BGOTools -> Declare new");
    QListWidgetItem * item;
    QPixmap tmpI;

    QStringList tmpList;
    bool found = false;

    tmpList.clear();
    tmpList.push_back("[all]");

    //set Block item box
    foreach(obj_block blockItem, configs.main_block)
    {

        if(blockItem.animated)
            tmpI = blockItem.image.copy(0,0,
                        blockItem.image.width(),
                        (int)round(blockItem.image.height() / blockItem.frames));
        else
            tmpI = blockItem.image;

        item = new QListWidgetItem( blockItem.name );
        item->setIcon( QIcon( tmpI ) );
        item->setData(3, QString::number(blockItem.id) );
        item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled );

        ui->BlockItemsList->addItem( item );

        //Add category
        found = false;
        if(tmpList.size()!=0)
            foreach(QString cat, tmpList)
            {   if(blockItem.type==cat)
                {found =true; break;}  }
        if(!found) tmpList.push_back(blockItem.type);
    }

    //apply category list
    ui->BlockCatList->clear();
    ui->BlockCatList->addItems(tmpList);

    tmpList.clear();
    tmpList.push_back("[all]");

    //set BGO item box
    foreach(obj_bgo bgoItem, configs.main_bgo)
    {
        if(bgoItem.animated)
            tmpI = bgoItem.image.copy(0,0,
                        bgoItem.image.width(),
                        (int)round(bgoItem.image.height() / bgoItem.frames) );
        else
            tmpI = bgoItem.image;

        item = new QListWidgetItem( bgoItem.name );
        item->setIcon( QIcon( tmpI ) );
        item->setData(3, QString::number(bgoItem.id) );
        item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled );

        ui->BGOItemsList->addItem( item );

        //Add category
        found = false;
        if(tmpList.size()!=0)
            foreach(QString cat, tmpList)
            {   if(bgoItem.type==cat)
                {found =true; break;}  }
        if(!found) tmpList.push_back(bgoItem.type);
    }
    //apply category list
    ui->BGOCatList->clear();
    ui->BGOCatList->addItems(tmpList);

}
