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

#include <common_features/app_path.h>
#include <tools/smart_import/smartimporter.h>

#include <ui_mainwindow.h>
#include <mainwindow.h>

void MainWindow::on_actionCDATA_Import_triggered()
{

    QString sourceDir = ApplicationPath;
    if(activeChildWindow()==1)
    {
        sourceDir = QFileInfo(activeLvlEditWin()->curFile).absoluteDir().path();
    }
    else
    if(activeChildWindow()==3)
    {
        sourceDir = QFileInfo(activeWldEditWin()->curFile).absoluteDir().path();
    }
    else
    return;

    //QMessageBox::information(this, "Dummy", "This feature comming soon!", QMessageBox::Ok);
    QString fileName = QFileDialog::getExistingDirectory(this, tr("Select directory with custom data to import"),
                                                 sourceDir,
                                                 QFileDialog::DontResolveSymlinks);

    qDebug()<<"File path: "<< fileName;

    if(fileName.isEmpty()) return;

    qApp->setActiveWindow(this);
    this->setFocus();
    this->raise();

    if(activeChildWindow()==1)
    {
        LevelEdit * box = activeLvlEditWin();
        if(QFileInfo(fileName).isDir())
        {
            SmartImporter * importer = new SmartImporter((QWidget*)box, fileName, (QWidget*)box);
            if(importer->isValid())
            {
                if(importer->attemptFastImport())
                {
                    qDebug()<<"Imported!";
                    on_actionReload_triggered();
                }
                else qDebug()<<"Import fail";
            }
            delete importer;
        }
    }
    else if(activeChildWindow()==3)
    {
        WorldEdit * box = activeWldEditWin();
        if(QFileInfo(fileName).isDir())
        {
            qDebug()<<"build";
            SmartImporter * importer = new SmartImporter((QWidget*)box, fileName, (QWidget*)box);
            if(importer->isValid())
            {
                qDebug()<<"do Attempt to import!";
                if(importer->attemptFastImport())
                {
                    qDebug()<<"Imported!";
                    on_actionReload_triggered();
                }
                else qDebug()<<"Import fail";
            }
            delete importer;
        }
    }
    else
        qDebug() << "No active windows";
}
