/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2014-2023 Vitaly Novichkov <admin@wohlnet.ru>
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

#include <QtConcurrent>
#include <QMdiSubWindow>

#include <data_configs/config_status/config_status.h>
#include <data_configs/selection_dialog/config_manager.h>

#include <ui_mainwindow.h>
#include <mainwindow.h>
#include <main_window/dock/lvl_sctc_props.h>
#include <main_window/dock/lvl_events_box.h>

#include <main_window/dock/lvl_item_toolbox.h>
#include <main_window/dock/wld_item_toolbox.h>
#include <main_window/dock/tileset_item_box.h>

/**
 * @brief A child over QProgressDialog which forbids dialog closing on Esc key
 */
class PGEProgressDialog : public QProgressDialog
{
public:
    explicit PGEProgressDialog(QWidget *parent = Q_NULLPTR, Qt::WindowFlags flags = Qt::WindowFlags())
        : QProgressDialog(parent, flags) {}

    PGEProgressDialog(const QString &labelText, const QString &cancelButtonText,
                      int minimum, int maximum, QWidget *parent = Q_NULLPTR,
                      Qt::WindowFlags flags = Qt::WindowFlags()):
        QProgressDialog(labelText, cancelButtonText, minimum, maximum, parent, flags)
    {}
    virtual ~PGEProgressDialog() {}

    virtual void keyPressEvent(QKeyEvent *e)
    {
        if(e->key() != Qt::Key_Escape)
            QProgressDialog::keyPressEvent(e);
    }
    virtual void closeEvent(QCloseEvent *e)
    {
        //No way to close this dialog box!
        e->ignore();
    }
};

void MainWindow::on_actionLoad_configs_triggered()
{
    QStringList openedFilesBackup;

    if(ui->centralWidget->subWindowList().size() > 0)
    {
        QMessageBox::StandardButton answer =
            QMessageBox::warning(this,
                                 tr("Configuration is busy"),
                                 tr("When reloading the configuration, all opened files will be closed and restored after reloading. Do you want to continue?"),
                                 QMessageBox::Yes | QMessageBox::No);
        if(answer != QMessageBox::Yes)
            return; // Do Nothing

        {
            // Collect list of all opened files
            QList<QMdiSubWindow *> subWindows = ui->centralWidget->subWindowList();
            for(QMdiSubWindow *w : subWindows)
            {
                EditBase *wnd = activeBaseEditWin(w);
                if(wnd)
                {
                    // Switch the subwindow to show wat to save or discard
                    setActiveSubWindow(w);
                    if((wnd->isUntitled() || wnd->isModified()) && !wnd->trySave())
                        return;
                    if(!wnd->isUntitled())
                        openedFilesBackup.push_back(wnd->currentFile());
                    wnd->markForForceClose();
                    w->close();
                }
            }
        }

        ui->centralWidget->closeAllSubWindows();
        //When some of subwindows are still be opened (for example, by user cancel)
        if(ui->centralWidget->subWindowList().size() > 0)
            return; // Do Nothing
    }

    emit configPackReloadBegin();

    //    //Disable all animations to take speed-up
    //    foreach( QMdiSubWindow *window, ui->centralWidget->subWindowList() )
    //    {
    //        if(QString(window->widget()->metaObject()->className())==LEVEL_EDIT_CLASS)
    //            qobject_cast<LevelEdit *>(window->widget())->scene->stopAnimation();
    //        else if(QString(window->widget()->metaObject()->className())==WORLD_EDIT_CLASS)
    //            qobject_cast<WorldEdit *>(window->widget())->scene->stopAnimation();
    //    }
    PGEProgressDialog progress("Please wait...", tr("Abort"),
                               0, 100,//Value, Maximum
                               this,//Parent
                               Qt::Window | Qt::WindowTitleHint | Qt::CustomizeWindowHint);
    progress.setWindowTitle(tr("Reloading configuration pack"));
    //progress.setWindowModality(Qt::WindowModal);
    progress.setModal(true);
    progress.setFixedSize(progress.size());
    progress.setGeometry(util::alignToScreenCenter(progress.size()));
    progress.setCancelButton(0);
    progress.setMinimumDuration(1);
    progress.setAutoClose(false);
    progress.show();

    if(!progress.wasCanceled())
        progress.setValue(1);

    //Reload configs
    qApp->processEvents();
    progress.connect(&configs, SIGNAL(progressPartsTotal(int)),
                     &progress, SLOT(setMaximum(int)), Qt::BlockingQueuedConnection);
    progress.connect(&configs, SIGNAL(progressTitle(QString)),
                     &progress, SLOT(setLabelText(QString)), Qt::BlockingQueuedConnection);
    progress.connect(&configs, SIGNAL(progressPartNumber(int)),
                     &progress, SLOT(setValue(int)), Qt::BlockingQueuedConnection);
    LogDebug("Lock tile item box...");
    dock_TilesetBox->m_lockSettings = true;
    dock_TilesetBox->clearTilesetGroups();
    // Do the loading in a thread
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    QFuture<bool> isOk = QtConcurrent::run(&this->configs, &DataConfig::loadFullConfig);
#else
    QFuture<bool> isOk = QtConcurrent::run(&DataConfig::loadFullConfig, &this->configs);
#endif

    while(!isOk.isFinished())
    {
        qApp->processEvents();
        QThread::msleep(1);
    }

    LogDebug("Configuration feloading is finished, re-initializing toolboxes...");
    dock_TilesetBox->m_lockSettings = false;
    dock_LvlItemBox->initItemLists(); //Apply item boxes from reloaded configs
    dock_WldItemBox->initItemLists();
    dock_LvlSectionProps->initDefaults();
    dock_LvlEvents->reloadSoundsList();
    dock_LvlItemBox->clearFilter();
    //Set tools from loaded configs
    //setLevelSectionData();
    LogDebug("Closing progress dialog...");
    progress.hide();
    LogDebug("Disconnecting slots...");
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
    LogDebug("Checking result...");

    // Restore previously closed files
    for(const QString &file : openedFilesBackup)
        OpenFile(file);

    if(isOk.result())
    {
        emit configPackReloaded();
        QMessageBox::information(this, tr("Reloading configuration"),
                                 tr("Configuration successfully reloaded!"),
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

    LogDebug("Completed!");
}

void MainWindow::on_actionReConfigure_triggered()
{
    //Config manager
    ConfigManager cmanager(this);
    cmanager.m_currentConfigPath = currentConfigDir;

    if(cmanager.runConfigureTool())
    {
        int ret = QMessageBox::question(this,
                                        tr("Configuration changed"),
                                        tr("The current configuration package needs to reload to apply recent changes. Do you want to proceed with it?"),
                                        QMessageBox::Yes|QMessageBox::No);
        if(ret == QMessageBox::Yes)
            on_actionLoad_configs_triggered();
    }
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

    if(cmanager.exec() == QDialog::Accepted)
    {
        configPath          =  cmanager.m_currentConfig;
        currentConfigDir    = (cmanager.m_doAskAgain) ? "" : configPath;
        saveSettings();
        QMessageBox::StandardButton answer = QMessageBox::question(this,
                                             tr("Configuration changed"),
                                             tr("The configuration pack has changed!\n"
                                                "To start using the new configuration pack, you need to restart the Editor. Do you want to continue?"),
                                             QMessageBox::Yes | QMessageBox::No);
        if(answer == QMessageBox::Yes)
        {
            ui->centralWidget->closeAllSubWindows();
            //When some of subwindows are still be opened (for example, by user cancel)
            if(ui->centralWidget->subWindowList().size() == 0)
            {
                m_isAppRestartRequested = true;
                this->close(); // Close main window
            }
        }
    }
}
