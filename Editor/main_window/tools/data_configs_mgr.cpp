/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2014-2016 Vitaly Novichkov <admin@wohlnet.ru>
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

#include <QDesktopWidget>
#include <QtConcurrent>

#include <data_configs/configstatus.h>
#include <data_configs/config_manager.h>

#include <ui_mainwindow.h>
#include <mainwindow.h>
#include <main_window/dock/lvl_sctc_props.h>
#include <main_window/dock/lvl_events_box.h>

#include <main_window/dock/lvl_item_toolbox.h>
#include <main_window/dock/wld_item_toolbox.h>
#include <main_window/dock/tileset_item_box.h>

void MainWindow::on_actionLoad_configs_triggered()
{   
    if(ui->centralWidget->subWindowList().size()>0)
    {
        QMessageBox::warning(this,
                             tr("Configuration is busy"),
                             tr("To reload configuration you should close all opened files first."),
                             QMessageBox::Ok);
        return;
    }

//    //Disable all animations to take speed-up
//    foreach( QMdiSubWindow *window, ui->centralWidget->subWindowList() )
//    {
//        if(QString(window->widget()->metaObject()->className())==LEVEL_EDIT_CLASS)
//            qobject_cast<LevelEdit *>(window->widget())->scene->stopAnimation();
//        else if(QString(window->widget()->metaObject()->className())==WORLD_EDIT_CLASS)
//            qobject_cast<WorldEdit *>(window->widget())->scene->stopAnimation();
//    }

    QProgressDialog progress("Please wait...", tr("Abort"), 0,100, this);
    progress.setWindowTitle(tr("Reloading configurations"));
    //progress.setWindowModality(Qt::WindowModal);
    progress.setModal(true);
    progress.setWindowFlags(Qt::Window | Qt::WindowTitleHint | Qt::CustomizeWindowHint | Qt::WindowStaysOnTopHint);
    progress.setFixedSize(progress.size());
    progress.setGeometry(QStyle::alignedRect(Qt::LeftToRight, Qt::AlignCenter, progress.size(), qApp->desktop()->availableGeometry()));
    progress.setCancelButton(0);
    progress.setMinimumDuration(1);
    progress.setAutoClose(false);
    progress.show();

    if(!progress.wasCanceled()) progress.setValue(1);

    //Reload configs
    qApp->processEvents();

    progress.connect(&configs, SIGNAL(progressMax(int)), &progress, SLOT(setMaximum(int)));
    progress.connect(&configs, SIGNAL(progressTitle(QString)), &progress, SLOT(setLabelText(QString)));
    progress.connect(&configs, SIGNAL(progressValue(int)), &progress, SLOT(setValue(int)));

    dock_TilesetBox->lockTilesetBox=true;
    dock_TilesetBox->clearTilesetGroups();

    // Do the loading in a thread
    QFuture<bool> isOk = QtConcurrent::run(&this->configs, &dataconfigs::loadconfigs);
    while(!isOk.isFinished()) { qApp->processEvents(); QThread::msleep(1); }

    dock_TilesetBox->lockTilesetBox=false;

    dock_LvlItemBox->setLvlItemBoxes(false); //Apply item boxes from reloaded configs
    dock_WldItemBox->setWldItemBoxes(false);

    dock_LvlSectionProps->initDefaults();
    dock_LvlEvents->reloadSoundsList();
    dock_LvlItemBox->clearFilter();

    //Set tools from loaded configs
    //setLevelSectionData();

    if(!progress.wasCanceled())
        progress.close();

    progress.disconnect(&configs, SIGNAL(progressMax(int)), &progress, SLOT(setMaximum(int)));
    progress.disconnect(&configs, SIGNAL(progressTitle(QString)), &progress, SLOT(setLabelText(QString)));
    progress.disconnect(&configs, SIGNAL(progressValue(int)), &progress, SLOT(setValue(int)));


//    //Restore all animations states back
//    foreach (QMdiSubWindow *window, ui->centralWidget->subWindowList())
//    {
//        if(QString(window->widget()->metaObject()->className())==LEVEL_EDIT_CLASS)
//        {
//            if(qobject_cast<LevelEdit *>(window->widget())->scene->opts.animationEnabled)
//                qobject_cast<LevelEdit *>(window->widget())->scene->startAnimation();
//        }
//        else if(QString(window->widget()->metaObject()->className())==WORLD_EDIT_CLASS)
//        {
//            if(qobject_cast<WorldEdit *>(window->widget())->scene->opts.animationEnabled)
//                qobject_cast<WorldEdit *>(window->widget())->scene->startAnimation();
//        }
//    }

    if(isOk.result())
    {
        QMessageBox::information(this, tr("Reloading configuration"),
        tr("Configuration succesfully reloaded!"),
        QMessageBox::Ok);
    }
    else
    {
        if(configs.check())
        {
            QMessageBox::warning(this, tr("Configuration error"),
                tr("Configuration package is loaded with errors."), QMessageBox::Ok);
            on_actionCurConfig_triggered();
        }
    }
}

void MainWindow::on_actionReConfigure_triggered()
{
    //Config manager
    ConfigManager cmanager(this);
    cmanager.m_currentConfigPath = currentConfigDir;
    cmanager.runConfigureTool();
}

void MainWindow::on_actionCurConfig_triggered()
{
    ConfigStatus cnfWindow(configs, this);

    cnfWindow.exec();
}

void MainWindow::on_actionChangeConfig_triggered()
{
    // Config manager
    ConfigManager cmanager(this);
    QString configPath = cmanager.loadConfigs();
    cmanager.setAskAgain(askConfigAgain);
    if( cmanager.exec() == QDialog::Accepted )
    {
        configPath          =  cmanager.m_currentConfig;
        askConfigAgain      =  cmanager.m_doAskAgain;
        currentConfigDir    = (cmanager.m_doAskAgain)? "" : configPath;
        saveSettings();
        QMessageBox::information(this,
                                 tr("Configuration changed"),
                                 tr("The Configuration was switched!\n"
                                    "To start work with new configuration, please restart application."),
                                 QMessageBox::Ok);
    }
}

