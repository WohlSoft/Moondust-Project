/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2014-2025 Vitaly Novichkov <admin@wohlnet.ru>
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

#include "qfile_dialogs_default_options.hpp"


void MainWindow::on_actionCDATA_Import_triggered()
{
    QString sourceDir = AppPathManager::userAppDir();

    if(activeChildWindow() == WND_Level)
    {
        if(activeLvlEditWin()->isUntitled())
        {
            QMessageBox::warning(this, tr("File not saved"),
                                 tr("File doesn't saved on disk."), QMessageBox::Ok);
            return;
        }
        sourceDir = QFileInfo(activeLvlEditWin()->curFile).absoluteDir().path();
    }
    else if(activeChildWindow() == WND_World)
    {
        if(activeWldEditWin()->isUntitled())
        {
            QMessageBox::warning(this, tr("File not saved"),
                                 tr("File doesn't saved on disk."), QMessageBox::Ok);
            return;
        }
        sourceDir = QFileInfo(activeWldEditWin()->curFile).absoluteDir().path();
    }
    else
        return;

    //QMessageBox::information(this, "Dummy", "This feature comming soon!", QMessageBox::Ok);
    QString fileName = QFileDialog::getExistingDirectory(this, tr("Select directory with custom data to import."),
                       sourceDir,
                       c_dirDialogOptions | QFileDialog::DontResolveSymlinks);

    qDebug() << "File path: " << fileName;

    if(fileName.isEmpty())
        return;

    qApp->setActiveWindow(this);
    this->setFocus();
    this->raise();

    if(activeChildWindow() == WND_Level)
    {
        LevelEdit *box = activeLvlEditWin();
        if(QFileInfo(fileName).isDir())
        {
            SmartImporter *importer = new SmartImporter((QWidget *)box, fileName, (QWidget *)box);
            if(importer->isValid())
            {
                if(importer->attemptFastImport())
                {
                    qDebug() << "Imported!";
                    on_actionReload_triggered();
                }
                else qDebug() << "Import fail";
            }
            delete importer;
        }
    }
    else if(activeChildWindow() == WND_World)
    {
        WorldEdit *box = activeWldEditWin();
        if(QFileInfo(fileName).isDir())
        {
            qDebug() << "build";
            SmartImporter *importer = new SmartImporter((QWidget *)box, fileName, (QWidget *)box);
            if(importer->isValid())
            {
                qDebug() << "do Attempt to import!";
                if(importer->attemptFastImport())
                {
                    qDebug() << "Imported!";
                    on_actionReload_triggered();
                }
                else qDebug() << "Import fail";
            }
            delete importer;
        }
    }
    else
        qDebug() << "No active windows";
}
