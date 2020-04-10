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

#ifndef LUNATESTERENGINE_H
#define LUNATESTERENGINE_H

#include "abstract_engine.h"

#include <QFuture>
#include <QMutex>
#include <QProcess>
#include <QSharedPointer>
#include <QThread>
#include <QByteArray>
#include <QQueue>
#ifndef _WIN32
#   include <QHash>
#endif
#include <common_features/safe_msg_box.h>

#include "ipc/luna_capabilities.h"

#ifndef _WIN32
#include "wine/wine_setup_cfg.h"
#endif

class QMenu;
class QAction;
class MainWindow;

class LunaEngineWorker : public QObject
{
    Q_OBJECT
    QString m_workingPath;
    QString m_lunaExecPath;
    QProcess *m_process = nullptr;
    QProcess::ProcessState m_lastStatus = QProcess::NotRunning;
    QByteArray m_readBuffer;

    // Note: The following is only required to handle the synchronous read/readStd
    //       and in the future gotIPCPacket should probably become a signal?
    QQueue<std::string> m_readPktQueue;

    bool m_isRunning = false;
    void init();
public:
    explicit LunaEngineWorker(QObject *parent = nullptr);
    ~LunaEngineWorker() override;
public slots:
    void setEnv(const QProcessEnvironment &env);
    void setExecPath(const QString &path);
    void setWorkPath(const QString &wDir);
    void start(const QString &command, const QStringList &args, bool *ok, QString *errString);

    void write(const QString &out, bool *ok);
    void read(QString *in, bool *ok);

    void writeStd(const std::string &out, bool *ok);
    void readStd(std::string *in, bool *ok);

    void processLoop();
    void quitLoop();

public:
    void terminate();
    void unInit();
    bool isActive();

private:
    // Note: In the future this should maybe become a signal, and maybe should be passed the decoded JSON instead of just a string
    void gotIPCPacket(const std::string& str);

private slots:
    void errorOccurred(QProcess::ProcessError error);
    void processFinished(int exitCode, QProcess::ExitStatus exitStatus);
    void gotReadReady();

signals:
    void loopFinished();
    void stopLoop();
};

class LunaTesterEngine : public AbstractRuntimeEngine
{
    Q_OBJECT

    /**
     * @brief LunaLoader result code
     */
    enum LunaLoaderResult
    {
        LUNALOADER_OK = 0,
        LUNALOADER_CREATEPROCESS_FAIL,
        LUNALOADER_PATCH_FAIL
    };

    /**
     * \brief Initialize LunaTester's runtime
     */
    void initRuntime();
    /**
     * @brief De-Initialize LunaTester's runtime and prelare to destruction
     */
    void unInitRuntime();

    //! Pointer to main window
    MainWindow *m_w = nullptr;
    //! List of registered menu items
    QAction *m_menuItems[10];

    //! LunaTester process handler
    QSharedPointer<LunaEngineWorker> m_worker;
    //! LunaTester process handler's thread
    QSharedPointer<QThread> m_thread;
    //! Helper which protects from editor freezing
    QFuture<void>       m_helper;
    //! Ranner thread
    QThread            *m_helperThread = nullptr;
    //! Don't run same function multiple times
    QMutex              m_engine_mutex;
    //! Disable OpenGL on LunaLua side
    bool                m_noGL = false;
    //! Don't re-use running instance, always start testing from scratch
    bool                m_killPreviousSession = false;
    //! Custom path to LunaTester
    QString             m_customLunaPath;
    //! Capabilities of given LunaLua build
    LunaLuaCapabilities m_caps;

    //! LunaLua process
    QProcess            m_lunaGame;
#ifndef _WIN32
    //! Wine capabilities to run game on non-Windows platforms
    WineSetupData       m_wineSetup;
#endif

    bool updateLunaCaps();

    void useWine(QString &command, QStringList &args);
    QString pathUnixToWine(const QString &unixPath);

    /**
     * @brief Returns a path to a directory where LunaTester is installed
     * @return path to LunaTester directory
     */
    QString getEnginePath();

    bool isEngineActive();
    bool isInPipeOpen();
    bool isOutPipeOpen();

    bool writeToIPC(const std::string &out);
    bool writeToIPC(const QString &out);
    std::string readFromIPC();
    QString readFromIPCQ();

private slots:
#if QT_VERSION_CHECK(5, 6, 0)
    void gameErrorOccurred(QProcess::ProcessError error);
#endif
    void gameStarted();
    void gameFinished(int exitCode, QProcess::ExitStatus exitStatus);
    void gameStateChanged(QProcess::ProcessState newState);
    void gameReadyReadStandardError();
    void gameReadyReadStandardOutput();

public slots:
    void killEngine();
    /********Menu items*******/
    void startTestAction();
    /**
     * @brief Start testing of currently opened level
     */
    bool startLunaTester(const LevelData &d);
    /**
     * @brief Reset all check point states
     */
    void resetCheckPoints();
    /**
     * @brief Kill frozen runner thread
     */
    void killFrozenThread();
    /**
     * @brief Kill running background instance
     */
    void killBackgroundInstance();

    void chooseEnginePath();

#ifndef _WIN32
    void runWineSetup();
#endif

private:
    /********Internal private functions*******/
    /**
     * @brief Starts testing of the level. Must be started in another thread via QtConcurrent
     * @param mw Pointer to main window
     * @param in_levelData Input level data
     * @param levelPath Full path to the level file
     * @param isUntitled Is untitled level (just created but not saved)
     */
    void lunaRunnerThread(LevelData in_levelData, const QString &levelPath, bool isUntitled);

    /**
     * @brief Start legacy engine in game mode
     */
    void lunaRunGame();

    /**
     * @brief Process checkpoints resetting
     */
    void lunaChkResetThread();

#if 0 // Unused
    /**
     * @brief Try to close SMBX's window
     * @return true if window successfully switched, false on failure
     */
    bool closeSmbxWindow();
#endif

    /**
     * @brief Switch to active LunaLUA testing window
     * @param msg Safe message box interface (to spawn message boxes at main window in main thread)
     * @return true if window successfully switched, false on failure
     */
    bool switchToSmbxWindow(SafeMsgBoxInterface &msg);

    /**
     * @brief Sends level data to LunaLUA
     * @param lvl Level data to send
     * @param levelPath Full file path to level file (is non-untitled)
     * @param isUntitled Is untitled level (just created but not saved)
     * @return true if data successfully sent, false on error
     */
    bool sendLevelData(LevelData &lvl, QString levelPath, bool isUntitled);

    void loadSetup();
    void saveSetup();

public:
    explicit LunaTesterEngine(QObject *parent = nullptr);
    ~LunaTesterEngine();

    virtual void init();
    virtual void unInit();

    virtual void initMenu(QMenu *destmenu);

    virtual bool doTestLevelIPC(const LevelData &d);
//    virtual bool doTestLevelFile(const QString &levelFile);

//    virtual bool doTestWorldIPC(const WorldData &d);
//    virtual bool doTestWorldFile(const QString &worldFile);

    virtual bool runNormalGame();

    virtual void terminate();

    virtual bool isRunning();

    virtual int capabilities();

private slots:
    void retranslateMenu();

signals:
    void engineSetExecPath(const QString &path);
    void engineSetEnv(const QProcessEnvironment &env);
    void engineSetWorkPath(const QString &wPath);
    void engineStart(const QString &command, const QStringList &args, bool *ok, QString *errString);
    void engineWrite(const QString &out, bool *ok);
    void engineRead(QString *in, bool *ok);
    void engineWriteStd(const std::string &out, bool *ok);
    void engineReadStd(std::string *in, bool *ok);
};

#endif // LUNATESTERENGINE_H
