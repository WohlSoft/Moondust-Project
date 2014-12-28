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

#include <QFileInfo>
#include <QDir>
#include <QMessageBox>
#include <QProcess>

#include <common_features/app_path.h>
#include <common_features/logger_sets.h>
#include <main_window/global_settings.h>

#include <ui_mainwindow.h>
#include <mainwindow.h>
#include <networking/engine_intproc.h>

void MainWindow::on_action_doTest_triggered()
{

    QString command;

    #ifdef _WIN32
    command = ApplicationPath+"/pge_engine.exe";
    #elif __APPLE__
    command = ApplicationPath+"/pge_engine.app/Contents/MacOS/pge_engine";
    #else
    command = ApplicationPath+"/pge_engine";
    #endif

    if(!QFileInfo(command).exists())
    {
        QMessageBox::warning(this, tr("Engine is not found"),
                             tr("Can't start testing, engine is not found: \n%1\nPlease, check the application directory.")
                             .arg(command),
                             QMessageBox::Ok);
        return;
    }

    if(IntEngine::isWorking())
    {
        if(QMessageBox::warning(this, tr("Engine already runned"),
                             tr("Engine is already testing another level.\n"
                                "Do you want to abort current testing process?"),
                             QMessageBox::Abort|QMessageBox::Cancel)==QMessageBox::Abort)
        {
            IntEngine::quit();
        }
        return;
    }

    if(activeChildWindow()==1)
    {
        //if(activeLvlEditWin()->isUntitled) return;

        QStringList args;
        args << "--debug";
        args << "--config=\""+QDir(configs.config_dir).dirName()+"\"";
        args << "--interprocessing";//activeLvlEditWin()->curFile;

        QProcess::startDetached(command, args);
    }
    else
        return;

    if(!IntEngine::isWorking()) IntEngine::init();
}


void MainWindow::on_action_doSafeTest_triggered()
{
    QString command;

    #ifdef _WIN32
    command = ApplicationPath+"/pge_engine.exe";
    #elif __APPLE__
    command = ApplicationPath+"/pge_engine.app/Contents/MacOS/pge_engine";
    #else
    command = ApplicationPath+"/pge_engine";
    #endif


    if(!QFileInfo(command).exists())
    {
        QMessageBox::warning(this, tr("Engine is not found"),
                             tr("Can't start testing, engine is not found: \n%1\nPlease, check the application directory.")
                             .arg(command),
                             QMessageBox::Ok);
        return;
    }

    if(activeChildWindow()==1)
    {
        if(activeLvlEditWin()->isUntitled)
        {
            QMessageBox::warning(this, tr("Save file first"),
            tr("To run testing of saved file, please save them into disk first!\n"
               "You can run testing without saving of file if you will use \"Run testing\" menu item."),
            QMessageBox::Ok);
            return;
        }

        QStringList args;
        args << "--debug";
        args << "--config=\""+QDir(configs.config_dir).dirName()+"\"";
        args << activeLvlEditWin()->curFile;

        QProcess::startDetached(command, args);
    }
}



void MainWindow::on_action_testSettings_triggered()
{

    QMessageBox::information(this, tr("Dummy"),
    tr("Sorry, the testing feature is under construction.\nSettings will available soon."),
    QMessageBox::Ok);

}

