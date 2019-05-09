/*
 * Moondust, a free game engine for platform game making
 * Copyright (c) 2014-2019 Vitaly Novichkov <admin@wohlnet.ru>
 *
 * This software is licensed under a dual license system (MIT or GPL version 3 or later).
 * This means you are free to choose with which of both licenses (MIT or GPL version 3 or later)
 * you want to use this software.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *
 * You can see text of MIT license in the LICENSE.mit file you can see in Engine folder,
 * or see https://mit-license.org/.
 *
 * You can see text of GPLv3 license in the LICENSE.gpl3 file you can see in Engine folder,
 * or see <http://www.gnu.org/licenses/>.
 */

#include "pge_qt_application.h"
#ifdef Q_OS_MACX
#include <QFileOpenEvent>
#endif
#include <common_features/logger.h>

PGE_OSXApplication::PGE_OSXApplication(int &argc, char **argv)
    : QCoreApplication(argc, argv)
{
#ifdef Q_OS_MACX
    m_connected = false;
#endif
}

PGE_OSXApplication::~PGE_OSXApplication()
{}

#ifdef Q_OS_MACX
void PGE_OSXApplication::setConnected()
{
    m_connected = true;
}

bool PGE_OSXApplication::event(QEvent *event)
{
    if(event->type() == QEvent::FileOpen)
    {
        QFileOpenEvent *openEvent = static_cast<QFileOpenEvent *>(event);

        if(openEvent)
        {
            if(m_connected)
            {
                QString file = openEvent->file();
                std::string file_s = file.toStdString();
                pLogDebug("Opened file %s (signal)", file_s.c_str());
                emit openFileRequested(file);
            }
            else
            {
                QString file = openEvent->file();
                std::string file_s = file.toStdString();
                pLogDebug("Opened file %s (queue)", file_s.c_str());
                m_openFileRequests.enqueue(file);
            }
        }
        else
            pLogWarning("Failed to process openEvent: pointer is null!");
    }

    return QApplication::event(event);
}

QStringList PGE_OSXApplication::getOpenFileChain()
{
    QStringList chain;

    while(!m_openFileRequests.isEmpty())
    {
        QString file = m_openFileRequests.dequeue();
        chain.push_back(file);
    }

    return chain;
}
#endif
