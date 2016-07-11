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

#ifndef PGE_EDITORAPPLICATION_H
#define PGE_EDITORAPPLICATION_H

#include <QApplication>
#include <QQueue>
#include <QStringList>

/*
Note: Class wasn't disabled completely to avoid "Note: No relevant classes found. No output generated." warning.
*/

/**
 * @brief Inhereted application which provides open file event, usually needed for OS X
 */
class PGE_OSXApplication : public QApplication
{
    Q_OBJECT
#ifdef Q_OS_MACX
    QQueue<QString> m_openFileRequests;
    bool            m_connected;
#endif
public:
    PGE_OSXApplication(int &argc, char **argv);
    virtual ~PGE_OSXApplication();
#ifdef Q_OS_MACX
    void    setConnected();
    bool    event(QEvent *event);
    QStringList getOpenFileChain();
signals:
    void openFileRequested(QString filePath);
#endif
};

#ifdef Q_OS_MACX
#define PGE_Application PGE_OSXApplication
#else
#define PGE_Application QApplication
#endif //Q_OS_MACX

#endif // PGE_EDITORAPPLICATION_H
