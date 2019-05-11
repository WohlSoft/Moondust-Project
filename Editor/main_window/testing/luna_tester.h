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

#if defined(_WIN32) && !defined(_WIN64)
#define LUNA_TESTER_32
#endif

#include <QFuture>
#include <QMutex>
#include <PGE_File_Formats/lvl_filedata.h>
#include <common_features/safe_msg_box.h>
class QMenu;
class QAction;
class MainWindow;
#ifdef LUNA_TESTER_32
#include <windows.h>
#else
#include <QProcess>
#endif

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
    //! Pointer to main window
    MainWindow *m_mw = nullptr;
    //! List of registered menu items
    QAction *m_menuItems[7];
    /**
     * @brief Initialize menu of the LunaTester
     * @param mw pointer to the Main Window
     * @param mainmenu Menu where insert LunaTester
     * @param insert_before Action where is need to insert LunaTester menu
     * @param defaultTestAction pointer to default test launcher (run interprocessing test)
     * @param secondaryTestAction pointer to second test launcher (run test of saved file)
     * @param startEngineAction pointer to engine launcher
     */
    void initLunaMenu(MainWindow   *mw,
                      QMenu        *mainmenu,
                      QAction      *insert_before,
                      QAction      *defaultTestAction,
                      QAction      *secondaryTestAction,
                      QAction      *startEngineAction);
    /**
     * @brief Refresh menu text
     */
    void retranslateMenu();

#ifdef LUNA_TESTER_32
    //! LunaLoader process information
    PROCESS_INFORMATION m_pi;
    //! LunaLUA IPC Out pipe
    HANDLE              m_ipc_pipe_out = 0;
    //! LunaLUA IPC Out pipe backend
    HANDLE              m_ipc_pipe_out_i = 0;
    //! LunaLUA IPC In pipe
    HANDLE              m_ipc_pipe_in = 0;
    //! LunaLUA IPC In pipe backend
    HANDLE              m_ipc_pipe_in_o = 0;
#else
    QProcess            m_process;
#endif
    //! Helper which protects from editor freezing
    QFuture<void>       m_helper;
    //! Ranner thread
    QThread            *m_helperThread = nullptr;
    //! Don't run same function multiple times
    QMutex              m_engine_mutex;
    //! Disable OpenGL on LunaLua side
    bool                m_noGL = false;
    bool                m_killPreviousSession = false;

    bool isEngineActive();
    bool isInPipeOpen();
    bool isOutPipeOpen();

    bool writeToIPC(const std::string &out);
    bool writeToIPC(const QString &out);
    std::string readFromIPC();
    QString readFromIPCQ();
public slots:
    void start(const QString &program, const QStringList &arguments, bool *ok, QString *errorString = nullptr);
    void write(const QString &out, bool *ok);
    void write(const std::string &out, bool *ok);
    void read(std::string *in);

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
    void lunaRunnerThread(LevelData in_levelData, QString levelPath, bool isUntitled);

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
     * @param msg Safe message box interface (to spawn message boxes at main window in main thread)
     * @return true if window successfully switched, false on failure
     */
    bool closeSmbxWindow(SafeMsgBoxInterface &msg);
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

#ifdef USE_LUNAHEXER
    /**
     * @brief Starts legacy engine with attaching LunaLUA library by hexing way
     * @param pathToLegacyEngine full path to legacy engine executive
     * @param cmdLineArgs full list of arguments to start legacy engine
     * @param workingDir working directory (must be equal to legacy engine executable!)
     * @return Result code
     */
    LunaLoaderResult LunaHexerRun(const wchar_t *pathToLegacyEngine,
                                  const wchar_t *cmdLineArgs,
                                  const wchar_t *workingDir);
#endif

#ifdef LUNA_TESTER_32
    /**
     * @brief Starts legacy engine with attaching LunaLUA library by in-memory patching way
     * @param pathToLegacyEngine full path to legacy engine executive
     * @param cmdLineArgs full list of arguments to start legacy engine
     * @param workingDir working directory (must be equal to legacy engine executable!)
     * @return Result code
     */
    LunaLoaderResult LunaLoaderRun(const wchar_t *pathToLegacyEngine,
                                   const wchar_t *cmdLineArgs,
                                   const wchar_t *workingDir);
#endif

};
#endif // LUNA_TESTER_H
