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

#include <QFileOpenEvent>
#include "editor_application.h"
#include <common_features/logger.h>

PGE_EditorApplication::PGE_EditorApplication(int &argc, char **argv)
    : QApplication(argc, argv)
{
    m_connected = false;
}

PGE_EditorApplication::~PGE_EditorApplication()
{}

void PGE_EditorApplication::setConnected()
{
    m_connected = true;
}

bool PGE_EditorApplication::event(QEvent *event)
{
    if(event->type() == QEvent::FileOpen)
    {
        QFileOpenEvent *openEvent = static_cast<QFileOpenEvent *>(event);
        if(openEvent)
        {
            if(m_connected)
            {
                QString file = openEvent->file();
                LogDebugQD("Opened file "+file + " (signal)");
                emit openFileRequested( file );
            }
            else
            {
                QString file = openEvent->file();
                LogDebugQD("Opened file "+file + " (queue)");
                m_openFileRequests.push_back(file);
            }
        } else {
            LogWarningQD("Failed to process openEvent: pointer is null!");
        }
    }
    return QApplication::event(event);
}

QStringList PGE_EditorApplication::getOpenFileChain()
{
    QStringList chain;
    while(!m_openFileRequests.isEmpty())
    {
        chain.append(m_openFileRequests.dequeue());
    }
    return chain;
}
