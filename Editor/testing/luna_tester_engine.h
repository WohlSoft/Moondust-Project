/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2014-2023 Vitaly Novichkov <admin@wohlnet.ru>
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

#include <QMutex>
#include <QProcess>
#include <QByteArray>

#include "ipc/luna_capabilities.h"

#ifndef _WIN32
#include "wine/wine_setup_cfg.h"
#endif

class QMenu;
class QAction;
class MainWindow;

class LunaTesterEngine : public AbstractRuntimeEngine
{
    Q_OBJECT

    //! Pointer to main window
    MainWindow *m_w = nullptr;
    //! Size of the menu items array
    static const size_t m_menuItemsSize = 11;
    //! List of registered menu items
    QAction *m_menuItems[m_menuItemsSize];

    QAction *m_menuRunLunaTest = nullptr;
    QAction *m_menuRunLevelSafeTest = nullptr;

    //! Disable OpenGL on LunaLua side
    bool                m_noGL = false;
    //! Don't re-use running instance, always start testing from scratch
    bool                m_killPreviousSession = false;
    //! Custom path to LunaTester
    QString             m_customLunaPath;
    //! Custom luna executable name
    QString             m_customExeName;
    //! Capabilities of given LunaLua build
    LunaLuaCapabilities m_caps;
    bool                m_capsNeedReload = false;

    //! Cached level data buffer
    LevelData           m_levelTestBuffer;

    //! Input buffer
    QByteArray          m_ipcReadBuffer;

    enum PendingCmd
    {
        PendC_NONE = 0,
        PendC_SendLevel,
        PendC_CheckPoint,
        PendC_ShowWindow,
        PendC_Quit,
        PendC_Kill,
        PendC_SendPlacingItem
    };
    QSet<PendingCmd>    m_pendingCommands;

    //! Don't run same function multiple times
    QMutex              m_engine_mutex;
    //! LunaLua process with IPC
    QProcess            m_lunaGameIPC;
    //! LunaLua process without IPC
    QProcess            m_lunaGame;
#ifndef _WIN32
    //! Wine capabilities to run game on non-Windows platforms
    WineSetupData       m_wineSetup;
#endif

private slots:
#if QT_VERSION_CHECK(5, 6, 0)
    void gameErrorOccurred(QProcess::ProcessError error);
#endif
    void gameStarted();
    void gameFinished(int exitCode, QProcess::ExitStatus exitStatus);
    void gameReadyReadStandardError();
    void gameReadyReadStandardOutput();

private:
    bool sendSimpleCommand(const QString &cmd, PendingCmd ipcPrevCmd = PendC_NONE);
    bool writeToIPC(const QJsonDocument &out);
    void onInputData(const QJsonDocument &input);

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

    /**
     * @brief Returns a path to a directory where LunaTester is installed
     * @return path to LunaTester directory
     */
    QString getEnginePath();
    /**
     * @brief Returns a name of executable that LunaTester will run
     * @return name of executable file to run
     */
    QString getExeName();

    /**
     * @brief Returns a path to a bridge executable to run a LunaTester
     * @return path to an executable
     */
    QString getBridgePath();

#ifndef _WIN32
    /**
     * @brief Returns a path to an executable of bridge to show a Wine window by HWND
     * @return path to an executable
     */
    QString getHwndShowBridgePath();
#endif

    bool isEngineActive();
    void killProcess();
    void killEngine();

public slots:
    /********Menu items*******/
    /**
     * @brief Start a level testing through IPC
     */
    void startTestAction();
    /**
     * @brief Start a level testing from a disk
     */
    void startSafeTestAction();
    /**
     * @brief Start legacy engine in game mode
     */
    void lunaRunGame();
    /**
     * @brief Reset all check point states
     */
    void resetCheckPoints();
    /**
     * @brief Kill running background instance
     */
    void killBackgroundInstance();
    /**
     * @brief Change path to LunaTester
     */
    void chooseEnginePath();
    /**
     * @brief Change the executable name (apply into config pack specific settings)
     */
    void chooseExeName();
    
    /**
     * @brief Turn on the block placing mode (or change properties)
     * @param block Block data structure
     */
    void sendPlacingBlock(const LevelBlock &block);

    /**
     * @brief Turn on the BGO placing mode (or change properties)
     * @param bgo BGO data structure
     */
    void sendPlacingBGO(const LevelBGO &bgo);

    /**
     * @brief Turn on the NPC placing mode (or change properties)
     * @param npc NPC data structure
     */
    void sendPlacingNPC(const LevelNPC &npc);

#ifndef _WIN32
private:
    /**
     * @brief Change Wine setup
     */
    void runWineSetup();
#endif

private:
    /********Internal private functions*******/
    /**
     * @brief Switch to active LunaLUA testing window
     * @param msg Safe message box interface (to spawn message boxes at main window in main thread)
     * @return true if window successfully switched, false on failure
     */
    bool switchToSmbxWindow();

    /**
     * @brief Sends level data to LunaLUA
     * @param lvl Level data to send
     * @return true if data successfully sent, false on error
     */
    bool sendLevelData(LevelData &lvl);

    void stopEditorMusic();

/********Settings functions*******/
    bool reloadLunaCapabilities();
    bool verifyCompatibility();

    void loadSetup();
    void saveSetup();
    
    /**
     * @brief Send a raw data of a placing item
     * @param raw Encoded setting of an object to place
     * @return true on a succes sending
     */
    bool sendItemPlacing(const QString &rawData, PendingCmd ipcPendCmd);

public:
    explicit LunaTesterEngine(QObject *parent = nullptr);
    ~LunaTesterEngine();

    virtual void init();
    virtual void unInit();

    virtual void initMenu(QMenu *destmenu);

    virtual bool doTestLevelIPC(const LevelData &d);
    virtual bool doTestLevelFile(const QString &levelFile);

//    virtual bool doTestWorldIPC(const WorldData &d);

    virtual bool doTestWorldFile(const QString &worldFile);

    virtual bool runNormalGame();

    virtual void terminate();

    virtual bool isRunning();

    virtual int capabilities();

private slots:
    void retranslateMenu();

signals:
    void testStarted();
    void testFinished();
};

#endif // LUNATESTERENGINE_H
