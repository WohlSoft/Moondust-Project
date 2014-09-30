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

#include "ui_mainwindow.h"
#include "mainwindow.h"

#include "npc_dialog/npcdialog.h"
#include "data_configs/config_manager.h"
#include "common_features/app_path.h"
#include "common_features/themes.h"

#include <QDesktopServices>

MainWindow::MainWindow(QMdiArea *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    //thread1 = new QThread;
    this->setAttribute(Qt::WA_QuitOnClose, true);
    this->setAttribute(Qt::WA_DeleteOnClose, true);

    setDefaults(); // Apply default common settings

    //Create empty config directory if not exists
    if(!QDir(ApplicationPath + "/" +  "configs").exists())
        QDir().mkdir(ApplicationPath + "/" +  "configs");

    // Config manager
    ConfigManager *cmanager = new ConfigManager();
    cmanager->setWindowFlags (Qt::Window | Qt::WindowTitleHint | Qt::WindowCloseButtonHint);
    cmanager->setGeometry(QStyle::alignedRect(Qt::LeftToRight, Qt::AlignCenter, cmanager->size(), qApp->desktop()->availableGeometry()));
    QString configPath = cmanager->isPreLoaded();
    QString tPack = cmanager->themePack;
    //If application runned first time or target configuration is not exist
    if(configPath.isEmpty())
    {
        //Ask for configuration
        if(cmanager->exec()==QDialog::Accepted)
        {
            configPath = cmanager->currentConfig;
        }
        else
        {
            delete cmanager;
            ui->setupUi(this);
            setDefLang();
            setUiDefults(); //Apply default UI settings
            WriteToLog(QtWarningMsg, "<Configuration is not selected>");
            this->close();
            return;
        }
    }

    currentConfigDir = configPath;

    delete cmanager;

    configs.setConfigPath(configPath);

    configs.loadBasics();
    Themes::loadTheme(tPack);

    QPixmap splashimg(configs.splash_logo.isEmpty()?
                      Themes::Image(Themes::splash) :
                      configs.splash_logo);

    QSplashScreen splash(splashimg);
    splash.setCursor(Qt::ArrowCursor);
    splash.setDisabled(true);
    splash.setWindowFlags( splash.windowFlags() |  Qt::WindowStaysOnTopHint );
    splash.show();

    bool ok=configs.loadconfigs();

    splash.finish(this);

    WriteToLog(QtDebugMsg, QString("Set UI..."));
    ui->setupUi(this);

    WriteToLog(QtDebugMsg, QString("Setting Lang..."));
    setDefLang();
    setUiDefults(); //Apply default UI settings

    if(!ok)
    {
        QMessageBox::critical(this, "Configuration error", "Configuration can't be loaded.\nSee in debug_log.txt for more information.", QMessageBox::Ok);
        WriteToLog(QtFatalMsg, "<Error, application closed>");
        this->close();
        return;
    }
}

MainWindow::~MainWindow()
{
    delete ui;
    WriteToLog(QtDebugMsg, "--> Application closed <--");
}



//////////////////SLOTS///////////////////////////

//Exit from application
void MainWindow::on_Exit_triggered()
{
    //ui->centralWidget->closeAllSubWindows();
    if(!MainWindow::close())
        return;
    //qApp->quit();
    //exit(0);
}

//Open About box
void MainWindow::on_actionAbout_triggered()
{
    aboutDialog about;
    about.setWindowFlags (Qt::Window | Qt::WindowTitleHint | Qt::WindowCloseButtonHint);
    about.setGeometry(QStyle::alignedRect(Qt::LeftToRight, Qt::AlignCenter, about.size(), qApp->desktop()->availableGeometry()));
    about.exec();
}


//Toolbar context menu
void MainWindow::on_MainWindow_customContextMenuRequested(const QPoint &pos)
{
    WriteToLog(QtDebugMsg, QString("Main Menu's context menu called! %1 %2").arg(pos.x()).arg(pos.y()));

    QMenu *cu = new QMenu(this);
    QAction *test= cu->addAction("Nothing");
    test->setEnabled(false);
    QAction *test2= cu->addAction("Nothing");
    test2->setEnabled(false);
    QAction *test3= cu->addAction("Nothing");
    test3->setEnabled(false);

    cu->exec(pos);

}


void MainWindow::showStatusMsg(QString msg, int time)
{
    statusBar()->showMessage(msg, time);
}


void MainWindow::refreshHistoryButtons()
{
    if(activeChildWindow()==1)
    {
        if(activeLvlEditWin()->sceneCreated)
        {
            ui->actionUndo->setEnabled( activeLvlEditWin()->scene->canUndo() );
            ui->actionRedo->setEnabled( activeLvlEditWin()->scene->canRedo() );
        }
    }else if(activeChildWindow()==3){
        if(activeWldEditWin()->sceneCreated)
        {
            ui->actionUndo->setEnabled( activeWldEditWin()->scene->canUndo() );
            ui->actionRedo->setEnabled( activeWldEditWin()->scene->canRedo() );
        }
    }

}

void MainWindow::on_actionContents_triggered()
{
    QDesktopServices::openUrl( QUrl::fromLocalFile( ApplicationPath + "/help/manual_editor.html" ) );
}

void MainWindow::on_actionNew_triggered()
{
    ui->menuNew->exec( this->cursor().pos() );
}


void MainWindow::on_actionRefresh_menu_and_toolboxes_triggered()
{
    updateMenus(true);
}



void MainWindow::on_actionSwitch_to_Fullscreen_triggered(bool checked)
{
    if(checked){
        //this->hide();
        this->showFullScreen();
    }else{
        //this->hide();
        this->showNormal();
    }
}


