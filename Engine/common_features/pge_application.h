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

#ifdef __APPLE__
#include <QApplication>
#include <QQueue>
#include <QStringList>

/**
 * @brief Inhereted application which provides open file event, usually needed for OS X
 */
class PGE_Application : public QApplication
{
    Q_OBJECT

    QQueue<QString> m_openFileRequests;
    bool            m_connected;
public:
    PGE_Application(int &argc, char **argv);
    virtual ~PGE_Application();
    void    setConnected();
    bool    event(QEvent *event);
    QStringList getOpenFileChain();
signals:
    void openFileRequested(QString filePath);
};
#else
#define PGE_Application QApplication
#endif //Q_OS_MACX

#endif // PGE_EDITORAPPLICATION_H
