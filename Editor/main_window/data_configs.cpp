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

#include "../data_configs/configstatus.h"


void MainWindow::on_actionLoad_configs_triggered()
{

    QProgressDialog progress("Please wait...", tr("Abort"), 0,100, this);
    progress.setWindowTitle(tr("Reloading configurations"));
    //progress.setWindowModality(Qt::WindowModal);
    progress.setModal(true);
    progress.setWindowFlags(Qt::Window | Qt::WindowTitleHint | Qt::CustomizeWindowHint | Qt::WindowStaysOnTopHint);
    progress.setFixedSize(progress.size());
    progress.setGeometry(QStyle::alignedRect(Qt::LeftToRight, Qt::AlignCenter, progress.size(), qApp->desktop()->availableGeometry()));
    progress.setCancelButton(0);
    progress.setMinimumDuration(0);
    progress.setAutoClose(false);
    //progress.show();

    if(!progress.wasCanceled()) progress.setValue(1);

    //Reload configs
    qApp->processEvents();
    configs.loadconfigs(&progress);

    if(!progress.wasCanceled())  progress.setValue(100);

    setItemBoxes(false); //Apply item boxes from reloaded configs
    setLevelSectionData();
    setSoundList();
    clearFilter();

    //Set tools from loaded configs
    //setLevelSectionData();

    if(!progress.wasCanceled())
        progress.close();

    QMessageBox::information(this, tr("Reloading configuration"),
    tr("Configuration succesfully reloaded!"),
    QMessageBox::Ok);
}



void MainWindow::on_actionCurConfig_triggered()
{
    ConfigStatus * cnfWindow = new ConfigStatus(configs);
    cnfWindow->setWindowFlags (Qt::Window | Qt::WindowTitleHint | Qt::WindowCloseButtonHint);
    cnfWindow->setGeometry(QStyle::alignedRect(Qt::LeftToRight, Qt::AlignCenter, cnfWindow->size(), qApp->desktop()->availableGeometry()));
    cnfWindow->exec();
    delete cnfWindow;
    //if(cnfWindow->exec()==QDialog::Accepted)
}
