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

#include "../ui_mainwindow.h"
#include "../mainwindow.h"


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
    //setLevelSectionData();

    if(!progress.wasCanceled())
        progress.close();

    QMessageBox::information(this, tr("Reload configuration"),
    tr("Configuration succesfully reloaded!"),
    QMessageBox::Ok);
}


