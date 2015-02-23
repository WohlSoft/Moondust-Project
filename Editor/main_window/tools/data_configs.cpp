/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2014-2015 Vitaly Novichkov <admin@wohlnet.ru>
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

#include <data_configs/configstatus.h>
#include <data_configs/config_manager.h>

#include <ui_mainwindow.h>
#include <mainwindow.h>
#include <main_window/dock/lvl_sctc_props.h>

#include <main_window/dock/lvl_item_toolbox.h>

void MainWindow::on_actionLoad_configs_triggered()
{
    //Disable all animations to take speed-up
    foreach (QMdiSubWindow *window, ui->centralWidget->subWindowList())
    {
        if(QString(window->widget()->metaObject()->className())==LEVEL_EDIT_CLASS)
            qobject_cast<LevelEdit *>(window->widget())->scene->stopAnimation();
        else if(QString(window->widget()->metaObject()->className())==WORLD_EDIT_CLASS)
            qobject_cast<WorldEdit *>(window->widget())->scene->stopAnimation();
    }

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
    configs.loadconfigs(&progress);

    if(!progress.wasCanceled())  progress.setValue(100);

    dock_LvlItemBox->setLvlItemBoxes(false); //Apply item boxes from reloaded configs
    setWldItemBoxes(false);

    dock_LvlSectionProps->setLevelSectionData();
    setSoundList();
    dock_LvlItemBox->clearFilter();

    //Set tools from loaded configs
    //setLevelSectionData();

    if(!progress.wasCanceled())
        progress.close();

    //Restore all animations states back
    foreach (QMdiSubWindow *window, ui->centralWidget->subWindowList())
    {
        if(QString(window->widget()->metaObject()->className())==LEVEL_EDIT_CLASS)
        {
            if(qobject_cast<LevelEdit *>(window->widget())->scene->opts.animationEnabled)
                qobject_cast<LevelEdit *>(window->widget())->scene->startAnimation();
        }
        else if(QString(window->widget()->metaObject()->className())==WORLD_EDIT_CLASS)
        {
            if(qobject_cast<WorldEdit *>(window->widget())->scene->opts.animationEnabled)
                qobject_cast<WorldEdit *>(window->widget())->scene->startAnimation();
        }
    }

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




void MainWindow::on_actionChangeConfig_triggered()
{
    // Config manager
    ConfigManager * cmanager;
    cmanager = new ConfigManager();
    cmanager->setWindowFlags (Qt::Window | Qt::WindowTitleHint | Qt::WindowCloseButtonHint);
    cmanager->setGeometry(QStyle::alignedRect(Qt::LeftToRight, Qt::AlignCenter, cmanager->size(), qApp->desktop()->availableGeometry()));
    QString configPath;
    configPath = cmanager->isPreLoaded();
    cmanager->setAskAgain(askConfigAgain);
    if(cmanager->exec()==QDialog::Accepted)
    {
        configPath = cmanager->currentConfig;
        askConfigAgain = cmanager->askAgain;
        currentConfigDir = (cmanager->askAgain)?"":configPath;
        saveSettings();
        QMessageBox::information(this, tr("Configuration changed"), tr("The Configuration was switched!\nTo start work with new configuration, please restart application."), QMessageBox::Ok);
    }
    delete cmanager;
}

