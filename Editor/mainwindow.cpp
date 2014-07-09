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
#include <QDesktopServices>

MainWindow::MainWindow(QMdiArea *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    //thread1 = new QThread;

    setDefaults(); // Apply default common settings

    QPixmap splashimg(":/images/splash_editor.png");
    QSplashScreen splash(splashimg);
    splash.setCursor(Qt::ArrowCursor);
    splash.setDisabled(true);
    splash.setWindowFlags( splash.windowFlags() |  Qt::WindowStaysOnTopHint );
    splash.show();

    if(!configs.loadconfigs())
    {
        QMessageBox::critical(this, "Configuration error", "Configuration can't be loaded.\nSee in debug_log.txt for more information.", QMessageBox::Ok);
        splash.finish(this);
        WriteToLog(QtFatalMsg, "<Error, application closed>");
        exit(EXIT_FAILURE);
        return;
    }

    splash.finish(this);

    WriteToLog(QtDebugMsg, QString("Set UI..."));
    ui->setupUi(this);

    WriteToLog(QtDebugMsg, QString("Setting Lang..."));
    setDefLang();

    setUiDefults(); //Apply default UI settings
}

MainWindow::~MainWindow()
{
    //TickTackLock = false;
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
    qApp->quit();
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
    }
}

void MainWindow::on_actionContents_triggered()
{
    QDesktopServices::openUrl( QUrl::fromLocalFile( QApplication::applicationDirPath() + "/help/manual_editor.html" ) );
}

void MainWindow::on_actionNew_triggered()
{
    ui->menuNew->exec( this->cursor().pos() );
}


void MainWindow::on_actionRefresh_menu_and_toolboxes_triggered()
{
    updateMenus(true);
}
