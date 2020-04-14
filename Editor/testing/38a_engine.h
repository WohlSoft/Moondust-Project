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

#ifndef SANBAEIRUNTIMEENGINE_H
#define SANBAEIRUNTIMEENGINE_H

#include "abstract_engine.h"

#include <QProcess>
#include <QMutex>

#ifndef _WIN32
#include "wine/wine_setup_cfg.h"
#endif

class MainWindow;

class SanBaEiRuntimeEngine : public AbstractRuntimeEngine
{
    Q_OBJECT

    //! Game application handler
    QProcess m_testingProc;
    //! IPC Bridge handler
    QProcess m_bridgeProc;
    //! IPC Bridge handler
    QProcess m_gameProc;
    //! Mutex which helps to avoid multiple launches of engine
    QMutex   m_engineMutex;

    MainWindow *m_w = nullptr;
    //! List of registered menu items
    QAction *m_menuItems[7];

    /************** Settings **************/
    //! Path to custom SMBX-38A executable
    QString m_customEnginePath;
    /************** Settings **************/

    QString getEnginePath();

    void loadSetup();
    void saveSetup();

    void terminateAll();

    /**
     * @brief Converts a command line and sets an environment for a Wine runtime on non-Windows platforms
     * @param proc Process instance
     * @param command Program
     * @param args Arguments
     */
    void useWine(QProcess &proc, QString &command, QStringList &args);
    /**
     * @brief Pass a local file path into Wine environment (when running a non-Windows platforms)
     * @param unixPath Local path
     * @return Wine environment internal path
     */
    QString pathUnixToWine(const QString &unixPath);

private slots:
    void retranslateMenu();

    /********Menu items*******/
    void startTestAction();
    void startSafeTestAction();
    void startGameAction();
    void chooseEnginePath();
#ifndef _WIN32
private:
    void runWineSetup();
#endif

public:
    explicit SanBaEiRuntimeEngine(QObject *parent = nullptr);
    ~SanBaEiRuntimeEngine();

#ifndef _WIN32
    //! Wine capabilities to run game on non-Windows platforms
    WineSetupData       m_wineSetup;
#endif

    virtual void init();
    virtual void unInit();

    virtual void initMenu(QMenu *destmenu);

    virtual bool doTestLevelIPC(const LevelData &d);
    virtual bool doTestLevelFile(const QString &levelFile);

    virtual bool doTestWorldIPC(const WorldData &d);

    virtual bool doTestWorldFile(const QString &worldFile);

    virtual bool runNormalGame();

    virtual void terminate();

    virtual bool isRunning();

    virtual int capabilities();

private slots:
    void testStarted();
    void testFinished();
};

#endif // SANBAEIRUNTIMEENGINE_H
