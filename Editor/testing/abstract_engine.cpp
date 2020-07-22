/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2014-2020 Vitaly Novichkov <admin@wohlnet.ru>
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

#include <QMessageBox>
#include <mainwindow.h>

#include "abstract_engine.h"


bool AbstractRuntimeEngine::checkIsEngineRunning(AbstractRuntimeEngine *e, QWidget *p)
{
    if(e->hasCapability(AbstractRuntimeEngine::CAP_DAEMON))
        return true; // Daemons allowed to be always-run

    if(!e->isRunning())
        return true; // Engine is not run

    if(QMessageBox::warning(p, MainWindow::tr("Engine already runned"),
                            MainWindow::tr("Engine is already testing another level.\n"
                               "Do you want to abort current testing process?"),
                            QMessageBox::Abort | QMessageBox::Cancel) == QMessageBox::Abort)
    {
        e->terminate();
        return true;
    }
    return false;
}

void AbstractRuntimeEngine::rejectUntitled(QWidget *p)
{
    QMessageBox::warning(p, MainWindow::tr("Save file first"),
                         MainWindow::tr("To run testing of saved file, please save them into disk first!\n"
                            "You can run testing without saving of file if you will use \"Run testing\" menu item."),
                         QMessageBox::Ok);
}



AbstractRuntimeEngine::AbstractRuntimeEngine(QObject *parent) :
    QObject(parent)
{}

AbstractRuntimeEngine::~AbstractRuntimeEngine()
{}

void AbstractRuntimeEngine::init()
{}

void AbstractRuntimeEngine::unInit()
{}

void AbstractRuntimeEngine::initMenu(QMenu *destmenu)
{
    Q_UNUSED(destmenu)
}

bool AbstractRuntimeEngine::doTestLevelIPC(const LevelData &d)
{
    Q_UNUSED(d)
    return false;
}

bool AbstractRuntimeEngine::doTestLevelFile(const QString &levelFile)
{
    Q_UNUSED(levelFile)
    return false;
}

bool AbstractRuntimeEngine::doTestWorldIPC(const WorldData &d)
{
    Q_UNUSED(d)
    m_errorString = tr("World map testing via IPC is not supported");
    return false;
}

bool AbstractRuntimeEngine::doTestWorldFile(const QString &worldFile)
{
    Q_UNUSED(worldFile)
    m_errorString = tr("World map testing is not supported");
    return false;
}

bool AbstractRuntimeEngine::runNormalGame()
{
    m_errorString = tr("This feature is not implemented");
    return false;
}

void AbstractRuntimeEngine::terminate()
{}

bool AbstractRuntimeEngine::isRunning()
{
    return false;
}

int AbstractRuntimeEngine::capabilities()
{
    return 0;
}

bool AbstractRuntimeEngine::hasCapability(int cap)
{
    int caps = capabilities();
    return (caps & cap) != 0;
}
