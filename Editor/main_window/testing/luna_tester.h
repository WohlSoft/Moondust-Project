/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2014-2019 Vitaly Novichkov <admin@wohlnet.ru>
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

#pragma once
#ifndef LUNA_TESTER_H
#define LUNA_TESTER_H

#include <QFuture>
#include <QMutex>
#include <QProcess>
#include <QSharedPointer>
#include <QThread>
#ifndef _WIN32
#   include <QHash>
#endif
#include <PGE_File_Formats/lvl_filedata.h>
#include <common_features/safe_msg_box.h>
class QMenu;
class QAction;
class MainWindow;


class LunaWorker : public QObject
{
    Q_OBJECT
    QProcess *m_process = nullptr;
    QProcess::ProcessState m_lastStatus = QProcess::NotRunning;
    bool m_isRunning = false;
    void init();
public:
    explicit LunaWorker(QObject *parent = nullptr);
    ~LunaWorker() override;

public slots:
    void setEnv(const QHash<QString, QString> &env);
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

private slots:
    void errorOccurred(QProcess::ProcessError error);
    void processFinished(int exitCode, QProcess::ExitStatus exitStatus);

signals:
    void loopFinished();
    void stopLoop();
};


/**
 * @brief Provides IPC layer with LunaLUA to manipulate legacy engine.
 */
class LunaTester : public QObject
{
    Q_OBJECT

public:
    /**
     * @brief LunaLoader result code
     */
    enum LunaLoaderResult
    {
        LUNALOADER_OK = 0,
        LUNALOADER_CREATEPROCESS_FAIL,
        LUNALOADER_PATCH_FAIL
    };

    LunaTester();
    ~LunaTester() override;
    /**
     * \brief Initialize LunaTester's runtime
     */
    void initRuntime();
    //! Pointer to main window
    MainWindow *m_mw = nullptr;
    //! List of registered menu items
    QAction *m_menuItems[7];
    /**
     * @brief Initialize menu of the LunaTester
     * @param mw pointer to the Main Window
     * @param mainMenu Menu where insert LunaTester
     * @param insert_before Action where is need to insert LunaTester menu
     * @param defaultTestAction pointer to default test launcher (run interprocessing test)
     * @param secondaryTestAction pointer to second test launcher (run test of saved file)
     * @param startEngineAction pointer to engine launcher
     */
    void initLunaMenu(MainWindow   *mw,
                      QMenu        *mainMenu,
                      QAction      *insert_before,
                      QAction      *defaultTestAction,
                      QAction      *secondaryTestAction,
                      QAction      *startEngineAction);
    /**
     * @brief Refresh menu text
     */
    void retranslateMenu();
    //! LunaTester process handler
    QSharedPointer<LunaWorker> m_worker;
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
    bool                m_killPreviousSession = false;

#ifndef _WIN32
    QString                 m_wineBinDir;
    QHash<QString, QString> m_wineEnv;
#endif
    void useWine(QString &command, QStringList &args);
    QString pathUnixToWine(const QString &unixPath);

    bool isEngineActive();
    bool isInPipeOpen();
    bool isOutPipeOpen();

    bool writeToIPC(const std::string &out);
    bool writeToIPC(const QString &out);
    std::string readFromIPC();
    QString readFromIPCQ();

public slots:
    void killEngine();
    /********Menu items*******/
    /**
     * @brief Start testing of currently opened level
     */
    void startLunaTester();
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
    /**
     * @brief Try to close SMBX's window
     * @return true if window successfully switched, false on failure
     */
    bool closeSmbxWindow();
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

signals:
    void engineSetEnv(const QHash<QString, QString> &env);
    void engineStart(const QString &command, const QStringList &args, bool *ok, QString *errString);
    void engineWrite(const QString &out, bool *ok);
    void engineRead(QString *in, bool *ok);
    void engineWriteStd(const std::string &out, bool *ok);
    void engineReadStd(std::string *in, bool *ok);
};
#endif // LUNA_TESTER_H
