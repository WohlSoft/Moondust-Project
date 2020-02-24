/*
 * Moondust, a free game engine for platform game making
 * Copyright (c) 2014-2020 Vitaly Novichkov <admin@wohlnet.ru>
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

#ifndef PGE_EDITORAPPLICATION_H
#define PGE_EDITORAPPLICATION_H

/*
    FIXME: Re-Implement ability to open files on OS X with using of the native API instead of this
*/

#include <QCoreApplication>
#include <QQueue>
#include <QStringList>

/**
 * @brief Inhereted application which provides open file event, usually needed for OS X
 */
class PGE_OSXApplication : public QCoreApplication
{
    Q_OBJECT
#ifdef Q_OS_MACX
    //! Queue used before slot will be connected to collect file paths received via QFileOpenEvent
    QQueue<QString> m_openFileRequests;
    //! Mark means to don't collect file paths and send them via signals
    bool            m_connected;
#endif
public:
    PGE_OSXApplication(int &argc, char **argv);
    virtual ~PGE_OSXApplication();
#ifdef Q_OS_MACX
    /**
     * @brief Disable collecting of the file paths via queue and send any new-received paths via signal
     */
    void    setConnected();
    /**
     * @brief Input event
     * @param event Event descriptor
     * @return is event successfully processed
     */
    bool    event(QEvent *event);
    /**
     * @brief Get all collected file paths and clear internal queue
     * @return String list of all collected file paths
     */
    QStringList getOpenFileChain();
signals:
    /**
     * @brief Signal emiting on receiving a file path via QFileOpenEvent
     * @param filePath full path to open
     */
    void openFileRequested(QString filePath);
#endif
};

#ifdef Q_OS_MACX
//! Allows processing file open and some other events on OS X. On non-Mac operating systems works as stub.
typedef PGE_OSXApplication PGE_Application;
#else
//! Allows processing file open and some other events on OS X. On non-Mac operating systems works as stub.
typedef QCoreApplication PGE_Application;
#endif //Q_OS_MACX


#endif // PGE_EDITORAPPLICATION_H
