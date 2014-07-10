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


//Edit NPC
npcedit *MainWindow::createNPCChild()
{
    npcedit *child = new npcedit(&configs);
    QMdiSubWindow *npcWindow = new QMdiSubWindow;
    npcWindow->setWidget(child);
    npcWindow->setAttribute(Qt::WA_DeleteOnClose);

    QMdiSubWindow * npcWindowP = ui->centralWidget->addSubWindow(npcWindow);
    npcWindowP->setAttribute(Qt::WA_DeleteOnClose);


    npcWindowP->setGeometry(
                (ui->centralWidget->subWindowList().size()*20)%(ui->centralWidget->size().width()/4),
                (ui->centralWidget->subWindowList().size()*20)%(ui->centralWidget->size().height()/4),
                 520,640);

 /*   connect(child, SIGNAL(copyAvailable(bool)),
            cutAct, SLOT(setEnabled(bool)));
    connect(child, SIGNAL(copyAvailable(bool)),
            copyAct, SLOT(setEnabled(bool)));
 */

    return child;
}



//Edit LEVEL
leveledit *MainWindow::createLvlChild()
{
    leveledit *child = new leveledit;
    QMdiSubWindow *levelWindow = new QMdiSubWindow;

    levelWindow->setWidget(child);
    levelWindow->setAttribute(Qt::WA_DeleteOnClose);

    QMdiSubWindow * levelWindowP = ui->centralWidget->addSubWindow(levelWindow);
    levelWindowP->setAttribute(Qt::WA_DeleteOnClose);

    levelWindowP->setGeometry(
                (ui->centralWidget->subWindowList().size()*20)%(ui->centralWidget->size().width()/4),
                (ui->centralWidget->subWindowList().size()*20)%(ui->centralWidget->size().height()/4),
                             800, 610);
    levelWindowP->setWindowIcon(QIcon(QPixmap(":/lvl16.png")));

    return child;
}



//Common functions

int MainWindow::activeChildWindow()
{
    if (QMdiSubWindow *activeSubWindow = ui->centralWidget->activeSubWindow())
    {
        if(QString(activeSubWindow->widget()->metaObject()->className())=="leveledit")
            return 1;
        if(QString(activeSubWindow->widget()->metaObject()->className())=="npcedit")
            return 2;
    }

    return 0;
}

npcedit *MainWindow::activeNpcEditWin()
{
    if (QMdiSubWindow *activeSubWindow = ui->centralWidget->activeSubWindow())
        return qobject_cast<npcedit *>(activeSubWindow->widget());
    return 0;
}

leveledit *MainWindow::activeLvlEditWin()
{
    if (QMdiSubWindow *activeSubWindow = ui->centralWidget->activeSubWindow())
        return qobject_cast<leveledit *>(activeSubWindow->widget());
    return 0;
}


QMdiSubWindow *MainWindow::findMdiChild(const QString &fileName)
{
    QString canonicalFilePath = QFileInfo(fileName).canonicalFilePath();
    leveledit *ChildWindow0;
    npcedit *ChildWindow2;

    foreach (QMdiSubWindow *window, ui->centralWidget->subWindowList())
    {
        if(QString(window->widget()->metaObject()->className())=="leveledit")
        {
        ChildWindow0 = qobject_cast<leveledit *>(window->widget());
        if (ChildWindow0->currentFile() == canonicalFilePath)
            return window;
        }
        else if(QString(window->widget()->metaObject()->className())=="npcedit")
        {
        ChildWindow2 = qobject_cast<npcedit *>(window->widget());
        if (ChildWindow2->currentFile() == canonicalFilePath)
            return window;
        }

    }
    return 0;
}


void MainWindow::setActiveSubWindow(QWidget *window)
{
    if (!window)
        return;
    ui->centralWidget->setActiveSubWindow(qobject_cast<QMdiSubWindow *>(window));
}

void MainWindow::SWCascade()
{
    if(GlobalSettings::MainWindowView!=QMdiArea::SubWindowView)
        setSubView(); // Switch into SubWindow mode on call this menuitem
    ui->centralWidget->cascadeSubWindows();
}

void MainWindow::SWTile()
{
    if(GlobalSettings::MainWindowView!=QMdiArea::SubWindowView)
        setSubView(); // Switch into SubWindow mode on call this menuitem
    ui->centralWidget->tileSubWindows();
}

void MainWindow::setSubView()
{
    GlobalSettings::MainWindowView = QMdiArea::SubWindowView;
    ui->centralWidget->setViewMode(GlobalSettings::MainWindowView);
    updateWindowMenu();
}

void MainWindow::setTabView()
{
    GlobalSettings::MainWindowView = QMdiArea::TabbedView;
    ui->centralWidget->setViewMode(GlobalSettings::MainWindowView);
    updateWindowMenu();
}
