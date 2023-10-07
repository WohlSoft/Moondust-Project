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

#include <sstream>
#include <cstring>
#include <signal.h>

#if !defined(_WIN32) && !defined(__APPLE__)
#include <glob.h>
#endif

#ifdef _WIN32
#include <windows.h>
#include <psapi.h>
#include <stdio.h>
#endif

#include <QEventLoop>
#include <QTimer>
#include <QJsonObject>
#include <QMetaObject>
#include <QJsonDocument>
#include <QJsonArray>
#include <QMenu>
#include <QAction>
#include <QGridLayout>
#include <QRadioButton>
#include <QLineEdit>
#include <QPushButton>
#include <QFileDialog>
#include <QGroupBox>
#include <QSpacerItem>
#include <QInputDialog>

#if !defined(_WIN32)
#include <QStandardPaths>
#include "wine/wine_setup.h"
#endif
#include "luna_tester_engine.h"

#ifdef __APPLE__
#include <QRegExp>
#endif

#include <mainwindow.h>
#include <PGE_File_Formats/file_formats.h>
#include <data_functions/smbx64_validation_messages.h>
#include <common_features/app_path.h>
#include <common_features/logger.h>
#include <dev_console/devconsole.h>
#include <main_window/global_settings.h>
#include <networking/engine_intproc.h>

#include "qfile_dialogs_default_options.hpp"


#ifdef _WIN32
static BOOL  checkProc(DWORD procId, const wchar_t *proc_name_wanted);
static DWORD getPidsByPath(const std::wstring &process_path, DWORD *found_pids, DWORD found_pids_max_size);

/* Checks matching of single process with full path string by PID */
static BOOL checkProc(DWORD procId, const wchar_t *proc_name_wanted)
{
    wchar_t proc_name[MAX_PATH + 1] = L"<unknown>";
    HMODULE h_mod = nullptr;
    DWORD cbNeeded;
    DWORD proc_name_len;
    HANDLE h_process = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, procId);

    if(nullptr == h_process)
        return FALSE;

    if(!EnumProcessModules(h_process, &h_mod, sizeof(h_mod), &cbNeeded))
        return FALSE;

    proc_name_len = GetModuleFileNameExW(h_process, h_mod, proc_name, MAX_PATH);
    proc_name[proc_name_len] = '\0';

    if(proc_name_len == 0)
    {
        proc_name_len = GetModuleBaseNameW(h_process, h_mod, proc_name, MAX_PATH);
        proc_name[proc_name_len] = '\0';
    }

    CloseHandle(h_process);
    return (lstrcmpiW(proc_name, proc_name_wanted) == 0);
}

/* Get all PIDs are matching given full path */
static DWORD getPidsByPath(const std::wstring &process_path, DWORD *found_pids, DWORD found_pids_max_size)
{
    DWORD proc_id[1024];
    DWORD ret_bytes = 0;
    DWORD proc_count = 0;
    unsigned int i;
    DWORD found_pids_count = 0;

    if(!EnumProcesses(proc_id, sizeof(proc_id), &ret_bytes))
    {
        LogCritical("ERROR: Can not execute EnumProcesses()...");
        return 0;
    }

    proc_count = ret_bytes / sizeof(DWORD);

    for(i = 0; i < proc_count; i++)
    {
        if(proc_id[i] != 0)
        {
            if(checkProc(proc_id[i], process_path.c_str()))
            {
                found_pids[found_pids_count++] = proc_id[i];
                if(found_pids_count >= found_pids_max_size)
                    break;
            }
        }
    }

    return found_pids_count;
}
#endif // _WIN32

#if !defined(_WIN32) && !defined(__APPLE__)
static pid_t find_pid(const QString &proc_name)
{
    const std::string process_name = proc_name.toStdString();
    pid_t pid = -1;
    glob_t pglob;
    char real_path[PATH_MAX] = {0};
    char cmdline_buffer[PATH_MAX] = {0};

    /* Get a list of all comm files. man 5 proc */
    if (glob("/proc/*/exe", 0, nullptr, &pglob) != 0)
        return pid;

    for(size_t i = 0; i < pglob.gl_pathc; ++i)
    {
        std::memset(real_path, 0, PATH_MAX);
        std::memset(cmdline_buffer, 0, PATH_MAX);
        size_t len = std::strlen(pglob.gl_pathv[i]);

        if(nullptr == realpath(pglob.gl_pathv[i], real_path))
            continue;

        if(nullptr == std::strstr(real_path, "wine-preloader"))
            continue;

        std::string cmdline_path = std::string(pglob.gl_pathv[i], len - 3);
        cmdline_path += "cmdline";

        FILE *cline = std::fopen(cmdline_path.c_str(), "rb");
        if(!cline)
            continue;

        std::fread(cmdline_buffer, 1, PATH_MAX, cline);
        std::fclose(cline);
        for(size_t j = 0; j < PATH_MAX; j++)
        {
            char &c = cmdline_buffer[j];
            if(c == '\\')
                c = '/';
        }

        // If exe matches our process path, extract the process ID from the
        // path, convert it to a pid_t, and return it.
        if(nullptr != std::strstr(cmdline_buffer, process_name.c_str()))
        {
            pid = static_cast<pid_t>(std::atoi(pglob.gl_pathv[i] + strlen("/proc/")));
            break;
        }
    }

    /* Clean up. */
    globfree(&pglob);
    return pid;
}
#endif // !_WIN32 && !__APPLE__



LunaTesterEngine::LunaTesterEngine(QObject *parent) :
    AbstractRuntimeEngine(parent)
{
    std::memset(m_menuItems, 0, sizeof(m_menuItems));
}

LunaTesterEngine::~LunaTesterEngine()
{
    killEngine();
}


void LunaTesterEngine::init()
{
    MainWindow *w = qobject_cast<MainWindow*>(parent());

    Q_ASSERT(w);
    if(!w)
    {
        LogCritical("Can't run test: MainWindow parent is null!");
        return;
    }

    m_w = w;

#if QT_VERSION_CHECK(5, 6, 0)
    QObject::connect(&m_lunaGameIPC, &QProcess::errorOccurred,
                     this, &LunaTesterEngine::gameErrorOccurred);
#endif
    QObject::connect(&m_lunaGameIPC, &QProcess::started,
                     this, &LunaTesterEngine::gameStarted);
    QObject::connect(&m_lunaGameIPC,
                     static_cast<void(QProcess::*)(int, QProcess::ExitStatus)>(&QProcess::finished),
                     this, &LunaTesterEngine::gameFinished);
    QObject::connect(&m_lunaGameIPC, &QProcess::readyReadStandardError,
                     this, &LunaTesterEngine::gameReadyReadStandardError);
    QObject::connect(&m_lunaGameIPC, &QProcess::readyReadStandardOutput,
                     this, &LunaTesterEngine::gameReadyReadStandardOutput);
                     
    QObject::connect(&g_intEngine, &IntEngineSignals::sendPlacingBlock,
                     this, &LunaTesterEngine::sendPlacingBlock);
    QObject::connect(&g_intEngine, &IntEngineSignals::sendPlacingNPC,
                     this, &LunaTesterEngine::sendPlacingNPC);
    QObject::connect(&g_intEngine, &IntEngineSignals::sendPlacingBGO,
                     this, &LunaTesterEngine::sendPlacingBGO);

    QObject::connect(this, &LunaTesterEngine::testStarted,
                     m_w, &MainWindow::stopMusicForTesting);
    QObject::connect(this, &LunaTesterEngine::testFinished,
                     m_w, &MainWindow::testingFinished);

    QObject::connect(m_w, &MainWindow::configPackReloadBegin,
                     this, &LunaTesterEngine::saveSetup);
    QObject::connect(m_w, &MainWindow::configPackReloaded,
                     this, &LunaTesterEngine::loadSetup);

    loadSetup();
}

void LunaTesterEngine::unInit()
{
    killEngine();
    saveSetup();
}

void LunaTesterEngine::initMenu(QMenu *lunaMenu)
{
    size_t menuItemId = 0;

    {
        m_menuRunLunaTest = lunaMenu->addAction("runTesting");
        QObject::connect(m_menuRunLunaTest,   &QAction::triggered,
                    this,               &LunaTesterEngine::startTestAction,
                    Qt::QueuedConnection);
        m_menuItems[menuItemId++] = m_menuRunLunaTest;
        QObject::connect(m_w, &MainWindow::windowActiveWorld, [this, menuItemId](bool wld)
        {
            auto *m = m_menuItems[menuItemId - 1];
            if(wld)
                m->setEnabled(hasCapability(AbstractRuntimeEngine::CAP_WORLD_IPC));
        });
        QObject::connect(m_w, &MainWindow::windowActiveLevel, [this, menuItemId](bool lvl)
        {
            auto *m = m_menuItems[menuItemId - 1];
            if(lvl)
                m->setEnabled(hasCapability(AbstractRuntimeEngine::CAP_LEVEL_IPC));
        });
    }

    {
        m_menuRunLevelSafeTest = lunaMenu->addAction("runSafeTesting");
        QObject::connect(m_menuRunLevelSafeTest,   &QAction::triggered,
                    this,               &LunaTesterEngine::startSafeTestAction,
                    Qt::QueuedConnection);
        m_menuItems[menuItemId++] = m_menuRunLevelSafeTest;
        QObject::connect(m_w, &MainWindow::windowActiveWorld, [this, menuItemId](bool wld)
        {
            auto *m = m_menuItems[menuItemId - 1];
            if(wld)
                m->setEnabled(hasCapability(AbstractRuntimeEngine::CAP_WORLD_FILE));
        });
        QObject::connect(m_w, &MainWindow::windowActiveLevel, [this, menuItemId](bool lvl)
        {
            auto *m = m_menuItems[menuItemId - 1];
            if(lvl)
                m->setEnabled(hasCapability(AbstractRuntimeEngine::CAP_LEVEL_FILE) && m_caps.args.contains("testLevel"));
        });
    }

    lunaMenu->addSeparator();

    QAction *resetCheckPoints = lunaMenu->addAction("resetCheckpoints");
    {
        QObject::connect(resetCheckPoints,   &QAction::triggered,
                    this,               &LunaTesterEngine::resetCheckPoints,
                    Qt::QueuedConnection);
        m_menuItems[menuItemId++] = resetCheckPoints;
    }
    {
        QAction *disableOpenGL = lunaMenu->addAction("Disable OpenGL");
        disableOpenGL->setCheckable(true);
        disableOpenGL->setChecked(m_noGL);
        QObject::connect(disableOpenGL,   &QAction::toggled,
                    [this](bool state)
        {
            m_noGL = state;
        });
        m_menuItems[menuItemId++] = disableOpenGL;
    }
    QAction *enableKeepingInBackground;
    {
        enableKeepingInBackground = lunaMenu->addAction("enableKeepingInBackground");
        enableKeepingInBackground->setCheckable(true);
        enableKeepingInBackground->setChecked(!m_killPreviousSession);
        QObject::connect(enableKeepingInBackground,   &QAction::toggled,
                    [this](bool state)
        {
            m_killPreviousSession = !state;
        });
        m_menuItems[menuItemId++] = enableKeepingInBackground;
    }
    {
        QAction *KillBackgroundInstance = lunaMenu->addAction("KillBackgroundInstance");
        QObject::connect(KillBackgroundInstance,   &QAction::triggered,
                    this,               &LunaTesterEngine::killBackgroundInstance,
                    Qt::QueuedConnection);
        m_menuItems[menuItemId++] = KillBackgroundInstance;
    }

    if(ConfStatus::defaultTestEngine != ConfStatus::ENGINE_LUNA)
        lunaMenu->addSeparator();

    QAction *choosEnginePath;
    {
        choosEnginePath = lunaMenu->addAction("changePath");
        QObject::connect(choosEnginePath,   &QAction::triggered,
                    this,                   &LunaTesterEngine::chooseEnginePath,
                    Qt::QueuedConnection);
        m_menuItems[menuItemId++] = choosEnginePath;
    }

    QAction *choosExeName;
    {
        choosExeName = lunaMenu->addAction("changeExeName");
        QObject::connect(choosExeName,   &QAction::triggered,
                    this,                &LunaTesterEngine::chooseExeName,
                    Qt::QueuedConnection);
        m_menuItems[menuItemId++] = choosExeName;
    }

#ifndef _WIN32
    lunaMenu->addSeparator();
    {
        QAction *wineSetup;
        wineSetup = lunaMenu->addAction("wineSetup");
        QObject::connect(wineSetup,   &QAction::triggered,
                    this,                   &LunaTesterEngine::runWineSetup,
                    Qt::QueuedConnection);
        m_menuItems[menuItemId++] = wineSetup;
    }
#endif

    lunaMenu->addSeparator();
    {
        QAction *runLegacyEngine = lunaMenu->addAction("startLegacyEngine");
        QObject::connect(runLegacyEngine,   &QAction::triggered,
                    this,              &LunaTesterEngine::lunaRunGame,
                    Qt::QueuedConnection);
        m_menuItems[menuItemId++] = runLegacyEngine;
    }

    Q_ASSERT(menuItemId < m_menuItemsSize);

    retranslateMenu();
    QObject::connect(m_w, &MainWindow::languageSwitched, this, &LunaTesterEngine::retranslateMenu);

    if(ConfStatus::defaultTestEngine == ConfStatus::ENGINE_LUNA)
    {
        choosEnginePath->setVisible(false); // Don't show this when LunaTester is a default engine
        resetCheckPoints->setShortcut(QStringLiteral("Ctrl+F5"));
        resetCheckPoints->setShortcutContext(Qt::WindowShortcut);
    }
}

void LunaTesterEngine::retranslateMenu()
{
    size_t menuItemId = 0;
    {
        QAction *runLunaTest = m_menuItems[menuItemId++];
        runLunaTest->setText(tr("Test level",
                                "Run the LunaTester based level testing."));
        runLunaTest->setToolTip(tr("Starts level testing in the legacy engine.\n"
                                   "To have this feature work, latest LunaLUA must be installed.\n"
                                   "Otherwise, it will be very limited."));
    }
    {
        QAction *runLunaTest = m_menuItems[menuItemId++];
        runLunaTest->setText(tr("Test saved level/world",
                                "Run the testing of current file in LunaTester from disk."));
    }
    {
        QAction *resetCheckPoints = m_menuItems[menuItemId++];
        resetCheckPoints->setText(tr("Reset checkpoints"));
        resetCheckPoints->setToolTip(tr("Reset all checkpoint states to initial state."));
    }
    {
        QAction *disableOpenGL = m_menuItems[menuItemId++];
        disableOpenGL->setText(tr("Disable OpenGL", "Disable OpenGL on LunaTester side"));
        disableOpenGL->setToolTip(tr("Disable OpenGL rendering engine and use the GDI. "
                                     "Useful if your video card does not support OpenGL or "
                                     "LunaLua is crashing on the attempt to use it."));
    }
    {
        QAction *enableKeepingInBackground = m_menuItems[menuItemId++];
        enableKeepingInBackground->setText(tr("Keep running in background",
                                              "Keep Legacy Engine be running in background to speed-up testing starts after first launch."));
        enableKeepingInBackground->setToolTip(tr("Allows to start level testing very fast after first launch.\n"
                                                 "Requires powerful computer, otherwise engine will freeze on next test launch.\n"
                                                 "Suggested to disable this feature on slow machines or if any troubles are happens while attempts to run a testing."));
    }
    {
        QAction *killBackgroundInstance = m_menuItems[menuItemId++];
        killBackgroundInstance->setText(tr("Terminate running process",
                                           "Ends the LunaTester process, regardless of whether it's in \n"
                                           "the background or foreground, so the engine can be loaded from scratch."));
        killBackgroundInstance->setToolTip(tr("Ends the LunaTester process so the engine can be loaded from scratch."));
    }
    {
        QAction *chooseEnginePath = m_menuItems[menuItemId++];
        chooseEnginePath->setText(tr("Change the path to LunaTester...",
                                     "Open a dialog to choose the location of LunaTester (aka SMBX2 data root directory)."));
        chooseEnginePath->setToolTip(tr("Select the location of LunaTester."));
    }
    {
        QAction *chooseExeName = m_menuItems[menuItemId++];
        chooseExeName->setText(tr("Change the executable name...",
                                     "Open a dialog to choose the name for executable LunaTester will use."));
        chooseExeName->setToolTip(tr("Select the name of executable for the LunaTester."));
    }
#ifndef _WIN32
    {
        QAction *runWineSetup = m_menuItems[menuItemId++];
        runWineSetup->setText(tr("Wine settings...",
                                 "Open Wine settings to choose which Wine toolchain use"));
        runWineSetup->setToolTip(tr("Select a Wine toolchain for use."));
    }
#endif
    {
        QAction *runLegacyEngine = m_menuItems[menuItemId++];
        runLegacyEngine->setText(tr("Start Game",
                                    "Launch LunaTester as a normal game."));
        runLegacyEngine->setToolTip(tr("Launch LunaTester as a normal game."));
    }
}

#if QT_VERSION_CHECK(5, 6, 0)
void LunaTesterEngine::gameErrorOccurred(QProcess::ProcessError error)
{
    QString title = tr("LunaTester error");
    QString msg;
    switch(error)
    {
    case QProcess::FailedToStart:
        msg = tr("Failed to start: %1").arg(m_lunaGame.errorString());
        break;
    case QProcess::Crashed:
        msg = tr("Crashed: %1").arg(m_lunaGame.errorString());
        break;
    case QProcess::Timedout:
        msg = tr("Timed out: %1").arg(m_lunaGame.errorString());
        break;
    case QProcess::WriteError:
        msg = tr("Write error: %1").arg(m_lunaGame.errorString());
        break;
    case QProcess::ReadError:
        msg = tr("Read error: %1").arg(m_lunaGame.errorString());
        break;
    default:
    case QProcess::UnknownError:
        msg = tr("Unknown error: %1").arg(m_lunaGame.errorString());
        break;
    }

    if(!m_pendingCommands.contains(PendC_Kill))
        QMessageBox::critical(m_w, title, msg, QMessageBox::Ok);
    else
    {
        LogDebug("LunaTester: killed: " + msg);
    }
    m_pendingCommands.clear();
}
#endif

void LunaTesterEngine::gameStarted()
{
    if(m_caps.args.contains("sendIPCReady"))
        return; // Unneeded

    if(!sendLevelData(m_levelTestBuffer))
    {
        QMessageBox::warning(m_w,
                    LunaTesterEngine::tr("LunaTester error"),
                    LunaTesterEngine::tr("Failed to send level into LunaLUA-SMBX!"),
                    QMessageBox::Ok);
    }
}

void LunaTesterEngine::gameFinished(int exitCode, QProcess::ExitStatus exitStatus)
{
    emit testFinished();
    LogDebug(QString("LunaTester: finished with Exit Code %1 and status %2").arg(exitCode).arg(exitStatus));
}

void LunaTesterEngine::gameReadyReadStandardError()
{
    QString err = m_lunaGameIPC.readAllStandardError();
    DevConsole::log(err, "WineDebug");
}

void LunaTesterEngine::gameReadyReadStandardOutput()
{
    if(!isEngineActive())
        return;

    QByteArray blok = m_lunaGameIPC.readAllStandardOutput();
    m_ipcReadBuffer += blok;
    LogDebugQD(QString("LunaTester: Received a data block: %1").arg(QString::fromUtf8(blok)));

    while (m_ipcReadBuffer.size() > 0)
    {
        // Skip any non-digit characters
        int i;
        for(i = 0; i < m_ipcReadBuffer.size(); i++)
        {
            if((m_ipcReadBuffer[i] >= '0') && (m_ipcReadBuffer[i] <= '9'))
            {
                break;
            }
        }

        // If we skipped non-digits, clear that much from the start of the buffer
        if(i > 0)
        {
            m_ipcReadBuffer.remove(0, i);
        }

        // Check how many digits we've read
        for(i = 0; i < m_ipcReadBuffer.size(); i++)
        {
            if((m_ipcReadBuffer[i] < '0') || (m_ipcReadBuffer[i] > '9'))
            {
                break;
            }
        }

        // If we don't have a following byte, abort for now
        if(i >= m_ipcReadBuffer.size())
        {
            break;
        }

        // If the following byte is not a colon, skip past this
        if(m_ipcReadBuffer[i] != ':')
        {
            m_ipcReadBuffer.remove(0, i + 1);
            continue;
        }

        // Decode length string
        bool ok;
        int len = m_ipcReadBuffer.left(i).toInt(&ok);

        // If unable to decode length string, skip past this
        if(!ok)
        {
            m_ipcReadBuffer.remove(0, i + 1);
            continue;
        }

        // Check if we have enough data, otherwise we have to wait till we receive more
        // Note: +2 to account for colon and following comma
        if((i + 2 + len) > m_ipcReadBuffer.size())
        {
            break;
        }

        // If not followed by a comma, skip the message header and restart parsing
        if(m_ipcReadBuffer[i + 1 + len] != ',')
        {
            m_ipcReadBuffer.remove(0, i + 1);
            continue;
        }

        // We have enough data, let's get the packet as a string
        QByteArray pkt = m_ipcReadBuffer.mid(i + 1, len);

        // Discard from read buffer
        m_ipcReadBuffer.remove(0, i + 1 + len);

        LogDebugQD(QString("LunaTester: got a packet: %1").arg(QString::fromUtf8(pkt)));

        QJsonDocument data;
        QJsonParseError err;
        data = QJsonDocument::fromJson(pkt, &err);
        if(err.error != QJsonParseError::NoError)
            LogWarning("LunaTester: Fail to parse an incoming JSON: " + err.errorString());
        else
            onInputData(data);
    }
}

void LunaTesterEngine::sendPlacingBlock(const LevelBlock &block)
{
    if(!isEngineActive())
        return;

    if(!m_caps.ipcCommands.contains("sendItemPlacing"))
        return; // This command is not supported by this LunaLua build

    LevelData buffer;
    FileFormats::CreateLevelData(buffer);
    buffer.blocks.push_back(block);
    buffer.layers.clear();
    buffer.events.clear();
    QString encoded;

    if(FileFormats::WriteExtendedLvlFileRaw(buffer, encoded))
        sendItemPlacing(encoded, PendC_SendPlacingItem);
}

void LunaTesterEngine::sendPlacingNPC(const LevelNPC &npc)
{
    if(!isEngineActive())
        return;

    if(!m_caps.ipcCommands.contains("sendItemPlacing"))
        return; // This command is not supported by this LunaLua build

    LevelData buffer;
    FileFormats::CreateLevelData(buffer);
    buffer.npc.push_back(npc);
    buffer.layers.clear();
    buffer.events.clear();
    QString encoded;

    if(FileFormats::WriteExtendedLvlFileRaw(buffer, encoded))
        sendItemPlacing(encoded, PendC_SendPlacingItem);
}

void LunaTesterEngine::sendPlacingBGO(const LevelBGO &bgo)
{
    if(!isEngineActive())
        return;

    if(!m_caps.ipcCommands.contains("sendItemPlacing"))
        return; // This command is not supported by this LunaLua build

    LevelData buffer;
    FileFormats::CreateLevelData(buffer);
    buffer.bgo.push_back(bgo);
    buffer.layers.clear();
    buffer.events.clear();
    QString encoded;

    if(FileFormats::WriteExtendedLvlFileRaw(buffer, encoded))
        sendItemPlacing(encoded, PendC_SendPlacingItem);
}

bool LunaTesterEngine::sendItemPlacing(const QString &rawData, PendingCmd ipcPendCmd)
{
    //{"jsonrpc": "2.0", "method": "sendItemPlacing", "params":
    //   {"sendItemPlacing": <RAW ITEM DATA> }}
    
    if(!isEngineActive())
        return false;

    if(!m_caps.ipcCommands.contains("sendItemPlacing"))
        return false; // This command is not supported by this LunaLua build

    QJsonDocument jsonOut;
    QJsonObject jsonObj;
    jsonObj["jsonrpc"]  = "2.0";
    jsonObj["method"]   = "sendItemPlacing";
    QJsonObject JSONparams;
    JSONparams["sendItemPlacing"] = rawData;
    jsonObj["params"] = JSONparams;
    jsonObj["id"] = static_cast<int>(ipcPendCmd);
    jsonOut.setObject(jsonObj);
    
    LogDebug("ENGINE: Place item command: " + rawData);
    
    if(writeToIPC(jsonOut))
    {
        m_pendingCommands += ipcPendCmd;
        return true;
    }

    return false;
}

bool LunaTesterEngine::sendSimpleCommand(const QString &cmd, PendingCmd ipcPendCmd)
{
    QJsonDocument jsonOut;
    QJsonObject jsonObj;
    jsonObj["jsonrpc"]  = "2.0";
    jsonObj["method"]   = cmd;
    QJsonObject JSONparams;
    JSONparams["xxx"] = 0;
    jsonObj["params"] = JSONparams;
    jsonObj["id"] = static_cast<int>(ipcPendCmd);
    jsonOut.setObject(jsonObj);

    if(writeToIPC(jsonOut))
    {
        m_pendingCommands += ipcPendCmd;
        return true;
    }

    return false;
}

bool LunaTesterEngine::writeToIPC(const QJsonDocument &out)
{
    QByteArray outputJSON = out.toJson();
    auto len = static_cast<size_t>(outputJSON.size());

    QString outDataS;
    outDataS.append(QString::number(len)  + ":");
    outDataS.append(outputJSON);
    outDataS.push_back(',');

    QByteArray outData = outDataS.toUtf8();

    auto ret = m_lunaGameIPC.write(outData);
    return (ret == qint64(outData.size()));
}

static void lunaErrorMsg(QWidget *m_w, QJsonObject &obj)
{
    int errCode = -1;
    QString errMsg = "<unknown>";
    auto error = obj["error"].toObject();

    if(!error["code"].isNull())
        errCode = error["code"].toInt();

    if(!error["message"].isNull())
        errMsg = error["message"].toString();

    QMessageBox::warning(m_w,
                         "LunaTester",
                         LunaTesterEngine::tr("Error has occured: (Error %1) %2")
                             .arg(errCode)
                             .arg(errMsg),
                         QMessageBox::Ok);
}

void LunaTesterEngine::onInputData(const QJsonDocument &input)
{
    QJsonObject obj = input.object();

    if(!obj["method"].isNull())
    {
        QString call = obj["method"].toString();
        if(call == "startedNotification")
        {
            LogDebug("LunaTester: Got a ready confirmation!");
            if(sendLevelData(m_levelTestBuffer))
                LogDebug("LunaTester: Sent a level data");
            else
                LogWarning("LunaTester: Fail to send a level data.");
        }
        else if (call == "closedToBackgroundNotification")
        {
            // Engine window was closed, now hidden in the background
            emit testFinished();
        }
        else if (call == "showFromBackgroundNotification")
        {
            // Engine window is returning from being hidden in the background
            // Note: We don't call m_w->stopMusicForTesting() here because we do that after level
            //       data we send is acknowledged.
        }
        else if (call == "suspendWhileUnfocusedNotification")
        {
            // Engine window is still there but suspended due to being unfocused
        }
        else if (call == "resumeAfterUnfocusedNotification")
        {
            // Engine window is resuming after being unfocused
        }
        return;
    }

    // Get previous command from ID field, since we send this enum as the ID
    PendingCmd ipcPendCmd = static_cast<PendingCmd>(obj["id"].toInt(PendC_NONE));

    // If kill is pending or unexpected response, don't react to any message exept to log errors
    if(m_pendingCommands.contains(PendC_Kill) || !m_pendingCommands.contains(ipcPendCmd))
        ipcPendCmd = PendC_NONE;

    // Mark as no longer pending
    m_pendingCommands -= ipcPendCmd;

    switch(ipcPendCmd)
    {
    case PendC_CheckPoint:
        LogDebug("LunaTester: Got a checkpoint reset feedback");
        if(!obj["error"].isNull())
        {
            lunaErrorMsg(m_w, obj);
            break;
        }
        QMessageBox::information(m_w,
                                 "LunaTester",
                                 tr("Checkpoints successfully reseted!"),
                                 QMessageBox::Ok);
        break;

    case PendC_ShowWindow:
        LogDebug("LunaTester: Got a HWND feedback");
        if(!obj["error"].isNull())
        {
            lunaErrorMsg(m_w, obj);
            break;
        }
        else
        {
            uintptr_t smbxHwnd = uintptr_t(obj["result"].toVariant().toULongLong());
#ifdef _WIN32 // Windows-only trick
            HWND wSmbx = HWND(smbxHwnd);
            //msg.warning("Test2", QString("Received: %1, from: %2").arg(smbxHwnd).arg(QString::fromStdString(inMessage)), QMessageBox::Ok);
            //wchar_t title[MAX_PATH];
            //GetWindowTextW(wSmbx, title, MAX_PATH);
            //msg.warning("Test2", QString("Window title is: %1").arg(QString::fromWCharArray(title)), QMessageBox::Ok);
            ShowWindow(wSmbx, SW_SHOW);
            SetForegroundWindow(wSmbx);
            SetActiveWindow(wSmbx);
            SetFocus(wSmbx);
#else
            QStringList args;
            args << QString::number(smbxHwnd);
            QString cmd = getHwndShowBridgePath();
            if(QFile::exists(cmd))
            {
                qDebug() << "Starting HWND bring bridge: " << cmd << args;
                QProcess q;
                useWine(q, cmd, args);
                q.start(cmd, args);
                if(q.waitForFinished())
                    qDebug() << q.exitCode();
                else
                    qWarning() << q.exitCode();
            }
#endif // _WIN32
        }
        break;

    case PendC_SendLevel:
        LogDebug("LunaTester: <- Level data has been sent!");
        if(!obj["error"].isNull())
            lunaErrorMsg(m_w, obj);

        // Stop music playback in the PGE Editor
        // Note: Currently, use stopMusicForTesting if we get hide/show notifications
        //       but otherwise use LunaTesterEngine::stopEditorMusic()
        if(m_caps.features.contains("HideShowNotifications"))
            testStarted();
        else
            stopEditorMusic();

        break;

    case PendC_Quit:
        LogDebug("LunaTester: Got a Quit feedback");
        if(!obj["error"].isNull())
            lunaErrorMsg(m_w, obj);
        break;

    case PendC_SendPlacingItem:
        LogDebug("LunaTester: Sent editor item to game!");
        if(!obj["error"].isNull())
            lunaErrorMsg(m_w, obj);
        break;

    default:
    case PendC_Kill:
    case PendC_NONE:
        if(!obj["error"].isNull())
            lunaErrorMsg(m_w, obj);
        break;
    }
}


#ifndef _WIN32

void LunaTesterEngine::useWine(QProcess &proc, QString &command, QStringList &args)
{
    args.push_front(command);
    auto setup = m_wineSetup;
    WineSetup::prepareSetup(setup);
    command = setup.metaWineExec;
    auto env = WineSetup::buildEnv(setup);
    proc.setProcessEnvironment(env);
}

QString LunaTesterEngine::pathUnixToWine(const QString &unixPath)
{
    auto setup = m_wineSetup;
    WineSetup::prepareSetup(setup);
    auto env = WineSetup::buildEnv(setup);

    QStringList args;
    QString command = QStandardPaths::findExecutable("winepath", {setup.metaWineBinDir});

    QProcess winePathExec;
    // Ask for in-Wine Windows path from in-UNIX native path
    args << "--windows" << unixPath;
    // Use wine custom environment
    winePathExec.setProcessEnvironment(env);

    // Start winepath
    winePathExec.start(command, args);
    winePathExec.waitForFinished();
    // Retrieve converted path
    QString windowsPath = winePathExec.readAllStandardOutput();
    return windowsPath.trimmed();
}

#else // _WIN32

void LunaTesterEngine::useWine(QProcess &, QString &, QStringList &) // Dummy
{}

QString LunaTesterEngine::pathUnixToWine(const QString &unixPath)
{
    // dummy, no need on real Windows
    return unixPath;
}

#endif //_WIN32


QString LunaTesterEngine::getEnginePath()
{
    if(ConfStatus::defaultTestEngine == ConfStatus::ENGINE_LUNA)
        return ConfStatus::configDataPath; // Ignore custom path

    return m_customLunaPath.isEmpty() ?
                ConfStatus::configDataPath :
                m_customLunaPath + "/";
}

static QString s_extExe(const QString &in)
{
    return in + (!in.endsWith(".exe", Qt::CaseInsensitive) ? ".exe" : QString());
}

QString LunaTesterEngine::getExeName()
{
    if(ConfStatus::defaultTestEngine == ConfStatus::ENGINE_LUNA)
        return s_extExe(ConfStatus::SmbxEXE_Name); // Ignore custom name

    return s_extExe(m_customExeName.isEmpty() ?
                    ConfStatus::SmbxEXE_Name :
                    m_customExeName);
}

QString LunaTesterEngine::getBridgePath()
{
    const QString execProxy = getEnginePath() + "LunaLoader-exec.exe";

    if(QFile::exists(execProxy))
        return execProxy;
    else
    {
#   ifdef Q_OS_MAC
        // on macOS an IPC bridge is shipped inside of a bundle
        QDir exePath(QApplication::applicationDirPath());
        return exePath.absoluteFilePath("../Resources/ipc/LunaLoader-exec.exe");
#   else
        return ApplicationPath + "/ipc/LunaLoader-exec.exe";
#   endif
    }
}

#ifndef _WIN32
QString LunaTesterEngine::getHwndShowBridgePath()
{
#   ifdef Q_OS_MAC
    // on macOS an IPC bridge is shipped inside of a bundle
    QDir exePath(QApplication::applicationDirPath());
    return exePath.absoluteFilePath("../Resources/ipc/luna_hwnd_show.exe");
#   else
    return ApplicationPath + "/ipc/luna_hwnd_show.exe";
#   endif
}
#endif // _WIN32

bool LunaTesterEngine::isEngineActive()
{
    return (m_lunaGameIPC.state() == QProcess::Running);
}

void LunaTesterEngine::killProcess()
{
    // Mark kill as pending for purposes of command processing
    m_pendingCommands += PendC_Kill;

    const QString smbxExeName = getExeName();

    QString lunaExecPath = getEnginePath();
    if(isEngineActive())
    {
#ifdef _WIN32
        // Kill everything that has "smbx.exe"
        DWORD proc_id[1024];
        DWORD proc_count = 0;
        QString smbxPath = lunaExecPath + smbxExeName;
        smbxPath.replace('/', '\\');
        proc_count = getPidsByPath(smbxPath.toStdWString(), proc_id, 1024);
        if(proc_count > 0)
        {
            LogDebugNC(QString("LunaEngineWorker: Found matching PIDs for running %1, going to kill...")
                     .arg(smbxExeName));
            for(DWORD i = 0; i < proc_count; i++)
            {
                DWORD f_pid = proc_id[i];
                HANDLE h_process = OpenProcess(PROCESS_TERMINATE, FALSE, f_pid);
                if(nullptr != h_process)
                {
                    BOOL res = TerminateProcess(h_process, 0);
                    if(!res)
                    {
                        LogDebugNC(
                            QString("LunaEngineWorker: Failed to kill %1 with PID %2 by 'TerminateProcess()' with error '%3', possibly it's already terminated.")
                                 .arg(smbxExeName)
                                 .arg(f_pid)
                                 .arg(GetLastError())
                        );
                    }
                    CloseHandle(h_process);
                }
            }
        }
        else
            LogDebugNC(QString("LunaEngineWorker: No matching PIDs found for %1:").arg(smbxExeName));
#else // anything but _WIN32

#   ifdef __APPLE__
        LogDebugNC(QString("LunaEngineWorker: Killing %1 by 'kill'...").arg(smbxExeName));
        QProcess ps;
        ps.start("/bin/ps", {"-A"});
        ps.waitForFinished();
        QString psAll = ps.readAllStandardOutput();

        QStringList psAllList = psAll.split('\n');
        QString smbxExeName = smbxExeName;

        QRegExp psReg(QString("(\\d+) .*(wine-preloader).*(%1)").arg(smbxExeName));
        for(QString &psOne : psAllList)
        {
            int pos = 0;
            while((pos = psReg.indexIn(psOne, pos)) != -1)
            {
                pid_t toKill = static_cast<pid_t>(psReg.cap(1).toUInt());
                LogDebugNC(QString("LunaEngineWorker: kill -TERM %1").arg(toKill));
                ::kill(toKill, SIGTERM);
                pos += psReg.matchedLength();
            }
        }

#   else // anything but __APPLE__
        pid_t pid = find_pid(lunaExecPath + smbxExeName);
        LogDebugNC(QString("LunaEngineWorker: Killing %1 by pid %2...")
            .arg(smbxExeName)
            .arg(pid));

        if(pid > 0)
            ::kill(pid, SIGKILL);
#   endif // __APPLE__ else

#endif // _WIN32 else

        LogDebugNC(QString("LunaEngineWorker: Killing by QProcess::kill()..."));
        m_lunaGameIPC.kill();
    }
}



void LunaTesterEngine::killEngine()
{
    if(!isEngineActive())
        return;

    if(m_caps.ipcCommands.contains("quit"))
    {
        sendSimpleCommand("quit", PendC_Quit);

        QEventLoop loop;
        QTimer waiter;
        QObject::connect(&m_lunaGameIPC, static_cast<void (QProcess::*)(int, QProcess::ExitStatus)>(&QProcess::finished),
                         &loop, &QEventLoop::quit);
        QObject::connect(&waiter, &QTimer::timeout,
                         &loop, &QEventLoop::quit);

        waiter.setInterval(2000);
        waiter.setSingleShot(true);
        waiter.start();
        loop.exec();

        if(isEngineActive())
            killProcess(); // if engine stuck, kill it
    }
    else
    {
        killProcess();
    }
}

/*****************************************Menu slots*************************************************/

/*!
 * \brief Starts testing in the hacked with LunaLUA-SMBX Engine if possible (Only for Windows builds)
 */
void LunaTesterEngine::startTestAction()
{
    if(m_w->activeChildWindow() == MainWindow::WND_Level)
    {
        LevelEdit *edit = m_w->activeLvlEditWin();
        if(!edit)
            return;

        if(m_killPreviousSession)
            killEngine();

        LevelData l = edit->LvlData;
        edit->prepareLevelFile(l);
        doTestLevelIPC(l);
    }
}

void LunaTesterEngine::startSafeTestAction()
{
    if(!AbstractRuntimeEngine::checkIsEngineRunning(this, m_w))
        return;

    if(m_w->activeChildWindow() == MainWindow::WND_Level)
    {
        LevelEdit *edit = m_w->activeLvlEditWin();
        if(!edit)
            return;

        if(edit->isUntitled())
        {
            AbstractRuntimeEngine::rejectUntitled(m_w);
            return;
        }

        doTestLevelFile(edit->curFile);
    }
    else if(m_w->activeChildWindow() == MainWindow::WND_World)
    {
        WorldEdit *edit = m_w->activeWldEditWin();
        if(!edit)
            return;

        if(edit->isUntitled())
        {
            AbstractRuntimeEngine::rejectUntitled(m_w);
            return;
        }

        doTestWorldFile(edit->currentFile());
    }
}

void LunaTesterEngine::lunaRunGame()
{
    runNormalGame();
}

void LunaTesterEngine::resetCheckPoints()
{
    QMutexLocker mlocker(&this->m_engine_mutex);

    if(!isEngineActive())
    {
        QMessageBox::warning(m_w, "LunaTester", tr("LunaLUA tester is not started!"), QMessageBox::Ok);
        return;
    }

    sendSimpleCommand("resetCheckPoints", PendC_CheckPoint);
}

void LunaTesterEngine::killBackgroundInstance()
{
    if(isEngineActive())
    {
        QMessageBox::StandardButton reply = QMessageBox::warning(m_w,
                                            "LunaTester",
                                            tr("Are you sure you want to close LunaTester? If you are testing a level, this will immediately end it!"),
                                            QMessageBox::Yes | QMessageBox::No);
        if(reply == QMessageBox::Yes)
        {
            emit testFinished();
            killEngine();
            QMessageBox::information(m_w,
                         "LunaTester",
                         tr("LunaTester has been successfully closed."),
                         QMessageBox::Ok);
        }
    }
    else
    {
        QMessageBox::information(m_w,
                                 "LunaTester",
                                 tr("LunaTester is not running."),
                                 QMessageBox::Ok);
    }
}

void LunaTesterEngine::chooseEnginePath()
{
    if(isEngineActive())
    {
        int ret = QMessageBox::question(m_w,
                                        tr("LunaTester is still active"),
                                        tr("To change a path to LunaTester, you will need to shut down a currently running game. "
                                           "Do you want to shut down LunaTester now?"),
                                        QMessageBox::Yes|QMessageBox::No);
        if(ret != QMessageBox::Yes)
            return; // Do nothing
        killEngine();
    }

    QDialog d(m_w);
    d.setWindowTitle(tr("Path to LunaTester", "Title of dialog"));
    d.setModal(true);

    QGridLayout *g = new QGridLayout(&d);
    d.setLayout(g);

    QGroupBox *gr = new QGroupBox(&d);
    gr->setTitle(tr("Please select a path to LunaTester:"));
    g->addWidget(gr, 0, 0, 1, 2);
    QGridLayout *gd = new QGridLayout(gr);
    gr->setLayout(gd);

    g->setColumnStretch(0, 1000);
    g->setColumnStretch(1, 0);

    QRadioButton *useDefault = new QRadioButton(gr);
    useDefault->setText(tr("Use default", "Using default LunaTester path, specified by a config pack"));

    QRadioButton *useCustom = new QRadioButton(gr);
    useCustom->setText(tr("Custom", "Using a user selected LunaTester path"));
    QLineEdit *c = new QLineEdit(gr);
    c->setMinimumWidth(400);
    QPushButton *br = new QPushButton(gr);
    br->setText(tr("Browse..."));

    gd->addWidget(useDefault, 0, 0, 1, 3);
    gd->addWidget(useCustom, 1, 0);
    gd->addWidget(c, 1, 1);
    gd->addWidget(br, 1, 2);

    gd->setColumnStretch(0, 0);
    gd->setColumnStretch(1, 1000);
    gd->setColumnStretch(2, 0);

    if(m_customLunaPath.isEmpty())
    {
        useDefault->setChecked(true);
        c->setText(ConfStatus::configDataPath);
        c->setEnabled(false);
    }
    else
    {
        useCustom->setChecked(true);
        c->setText(m_customLunaPath);
    }

    QPushButton *save = new QPushButton(&d);
    save->setText(tr("Save"));
    g->addItem(new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding), 1, 0);
    g->addWidget(save, 1, 1);

    save->setFocus(Qt::TabFocusReason);

    QObject::connect(useCustom, &QRadioButton::toggled, c, &QLineEdit::setEnabled);
    QObject::connect(useCustom, &QRadioButton::toggled, br, &QPushButton::setEnabled);

    QObject::connect(save, &QPushButton::clicked, &d, &QDialog::accept);
    QObject::connect(br, &QPushButton::clicked,
                     [&d, c](bool)->void
    {
        QString p = QFileDialog::getExistingDirectory(&d, tr("Select a location of LunaTester", "Directory select dialog title"), c->text(), c_dirDialogOptions);
        if(!p.isEmpty())
            c->setText(p);
    });

    int ret = d.exec();

    if(ret == QDialog::Accepted)
    {
        if(useCustom->isChecked())
            m_customLunaPath = c->text();
        else
            m_customLunaPath.clear();

        if(!reloadLunaCapabilities())
        {
            QMessageBox::critical(m_w,
                                  tr("LunaTester error"),
                                  tr("Unable to recognize capabilities of selected LunaLua path, game may not work. Please select a different path."),
                                  QMessageBox::Ok);
        }
    }
}

void LunaTesterEngine::chooseExeName()
{
    if(isEngineActive())
    {
        int ret = QMessageBox::question(m_w,
                                        tr("LunaTester is still active"),
                                        tr("To change an executable name, you will need to shut down a currently running game. "
                                           "Do you want to shut down LunaTester now?"),
                                        QMessageBox::Yes|QMessageBox::No);
        if(ret != QMessageBox::Yes)
            return; // Do nothing
        killEngine();
    }

    QString cur = m_customExeName.isEmpty() ? ConfStatus::SmbxEXE_Name : m_customExeName;

    bool ok;
    QString en = QInputDialog::getText(m_w,
                                       tr("Change the executable name"),
                                       tr("Please type the PE executable name to be used with LunaTester (for example, smbx.exe)"),
                                       QLineEdit::Normal, cur, &ok);

    if(ok && !en.isEmpty())
    {
        if(en != ConfStatus::SmbxEXE_Name)
            m_customExeName = en;
        else
            m_customExeName.clear();
    }
}

#ifndef _WIN32
void LunaTesterEngine::runWineSetup()
{
    if(isEngineActive())
    {
        int ret = QMessageBox::question(m_w,
                                        tr("LunaTester is still active"),
                                        tr("To change a setup of Wine, you will need to shut down a currently working LunaTester. "
                                           "Do you want to shut down the LunaTester now?"),
                                        QMessageBox::Yes|QMessageBox::No);
        if(ret != QMessageBox::Yes)
            return; // Do nothing
        killEngine();
    }

    WineSetup d(m_w);
    d.setModal(true);
    d.setSetup(m_wineSetup);
    int ret = d.exec();

    if(ret == QDialog::Accepted)
        m_wineSetup = d.getSetup();
}
#endif // _WIN32


/*****************************************Private functions*************************************************/

bool LunaTesterEngine::switchToSmbxWindow()
{
    if(!isEngineActive())
        return false;

    if(m_caps.features.contains("SelfForegrounding"))
        return true; // this workaround is no more needed

    if(sendSimpleCommand("getWindowHandle", PendC_ShowWindow))
        return true;

    return false;
}

bool LunaTesterEngine::sendLevelData(LevelData &lvl)
{
    //{"jsonrpc": "2.0", "method": "testLevel", "params":
    //   {"filename": "myEpisode/test.lvl", "leveldata": "<RAW SMBX64 LEVEL DATA>", "players": [{"character": 1}], "godMode": false, "showFPS": false },
    //"id": 3}

    //"players": [ { "character": 1, "powerup": 1, "mountType": 1, "mountColor": 1 }, { "character": 2, "powerup": 1, "mountType": 1, "mountColor": 1 } ]

    if(!isEngineActive())
        return false;

    bool hasLvlxSupport = m_caps.features.contains("LVLX");

    if(hasLvlxSupport)
        LogDebug("LunaTester: <- Yes! LVLX is supported!");
    else
        LogDebug("LunaTester: <- No! LVLX is NOT supported by this LunaLua build!");

    QJsonDocument jsonOut;
    QJsonObject jsonObj;
    jsonObj["jsonrpc"] = "2.0";
    jsonObj["method"] = "testLevel";

    bool isUntitled = lvl.meta.path.isEmpty();

    QJsonObject JSONparams;
    QString levelPathOut;
    QString x = (hasLvlxSupport && !lvl.meta.smbx64strict ? "x" : "");

    if(isUntitled)
    {
        levelPathOut = QStringLiteral("worlds\\untitled.lvl") + x;
        JSONparams["filename"] = levelPathOut;
    }
    else
    {
        QString levelPath = QString("%1/%2.lvl%3").arg(lvl.meta.path).arg(lvl.meta.filename).arg(x);
        levelPathOut = pathUnixToWine(levelPath);
        JSONparams["filename"] = levelPathOut;
    }

    QJsonArray JSONPlayers;
    QJsonObject JSONPlayer1, JSONPlayer2;
    SETTINGS_TestSettings  t = GlobalSettings::testing;
    JSONPlayer1["character"]  = t.p1_char;
    JSONPlayer1["powerup"]    = t.p1_state;
    JSONPlayer1["mountType"]  = t.p1_vehicleID;
    JSONPlayer1["mountColor"] = t.p1_vehicleType;
    JSONPlayer2["character"]  = t.p2_char;
    JSONPlayer2["powerup"]    = t.p2_state;
    JSONPlayer2["mountType"]  = t.p2_vehicleID;
    JSONPlayer2["mountColor"] = t.p2_vehicleType;
    //mountType

    JSONPlayers.push_back(JSONPlayer1);

    if(t.numOfPlayers > 1)
        JSONPlayers.push_back(JSONPlayer2);

    JSONparams["players"] = JSONPlayers;

    // Extra flags
    JSONparams["godMode"] = t.xtra_god;
    JSONparams["showFPS"] = t.xtra_showFPS;

    QString LVLRawData;
    if(hasLvlxSupport && !lvl.meta.smbx64strict)
    {
        //Generate the PGE-X Level file data
        FileFormats::WriteExtendedLvlFileRaw(lvl, LVLRawData);
    }
    else
    {
        //Generate actual SMBX64 Level file data
        FileFormats::WriteSMBX64LvlFileRaw(lvl, LVLRawData, 64);
        //Set CRLF
        LVLRawData.replace("\n", "\r\n");
    }
    //Store data into JSON
    JSONparams["leveldata"] = LVLRawData;

    jsonObj["params"] = JSONparams;
    jsonObj["id"] = static_cast<int>(PendC_SendLevel);
    jsonOut.setObject(jsonObj);

    LogDebug("LunaTester: -> Sending level data and testing request...");
    //Send data to SMBX
    if(writeToIPC(jsonOut))
    {
        m_pendingCommands += PendC_SendLevel;
        return true;
    }

    LogWarning("<- Fail to send level data and testing request into LunaTester!");
    return false;
}

void LunaTesterEngine::stopEditorMusic()
{
    //Stop music playback in the PGE Editor!
    QMetaObject::invokeMethod(m_w, "setMusicButton", Qt::QueuedConnection, Q_ARG(bool, false));
    // not sure how efficient it is
    QMetaObject::invokeMethod(m_w, "on_actionPlayMusic_triggered", Qt::QueuedConnection, Q_ARG(bool, false));
}

/*****************************************Settings functions*************************************************/

bool LunaTesterEngine::reloadLunaCapabilities()
{
    //! Path to SMBX root
    const QString smbxPath = getEnginePath();
    //! Full path to LunaDLL library
    const QString lunaDllPath = smbxPath + "LunaDll.dll";

    bool ret = getLunaCapabilities(m_caps, lunaDllPath);
    if(ret)
    {
        if(m_menuRunLevelSafeTest)
        {
            m_menuRunLevelSafeTest->setEnabled((m_w->activeChildWindow() == MainWindow::WND_Level && m_caps.args.contains("testLevel")) ||
                                                m_w->activeChildWindow() == MainWindow::WND_World);
        }
    }

    m_capsNeedReload = false;

    return ret;
}

bool LunaTesterEngine::verifyCompatibility()
{
    if(m_capsNeedReload)
        reloadLunaCapabilities();

    //! Path to SMBX root
    const QString smbxPath = getEnginePath();
    //! Executable name
    const QString smbxExeName = getExeName();
    //! Full path to SMBX executable
    const QString smbxExePath = smbxPath + smbxExeName;
    //! Full path to LunaDLL library
    const QString lunaDllPath = smbxPath + "LunaDll.dll";
    //! Full path to LunaTester proxy executable
    const QString execProxy = getBridgePath();

    const QStringList smbxFiles =
    {
        smbxExePath,
        execProxy
    };
    //! Extra checks of SMBX path
    const QStringList smbxDirs =
    {
        smbxPath,
        smbxPath + "graphics",
        smbxPath + "graphics/background",
        smbxPath + "graphics/background2",
        smbxPath + "graphics/block",
        smbxPath + "graphics/npc",
        smbxPath + "graphics/tile",
        smbxPath + "graphics/scene",
        smbxPath + "graphics/path",
        smbxPath + "graphics/level",
        smbxPath + "graphics/player"
    };

    const QString notFoundErrorTitle = LunaTesterEngine::tr("LunaTester directory check failed",
                                                            "A title of a message box that shows when some of the files or directories not exist.");
    const QString notFoundErrorText = LunaTesterEngine::tr("Can't start LunaTester because \"%1\" is not found! That might happen due to any of the following reasons:",
                                                           "A text of a message box that shows when some of the files or directories not exist.");

    const QString explanationGeneric =
            LunaTesterEngine::tr("- Incorrect location of LunaTester (or SMBX2 data root) was specified, please check the LunaTester location setup.\n"
                                 "- Possible removal of files by your antivirus (false positive or an infection of the file), "
                                 "please check your antivirus' quarantine or report of recently removed threats.\n"
                                 "- Incorrect installation of SMBX2 has caused missing files, please reinstall SMBX2 to fix your problem.",
                                 "Description of a problem, showing when LunaTester is NOT a default engine in a current config pack.");

    const QString explanationSMBX2 =
            LunaTesterEngine::tr("- Possible removal of files by your antivirus (false positive or an infection of the file), "
                                 "please check your antivirus' quarantine or report of recently removed threats.\n"
                                 "- Incorrect installation of SMBX2 has caused missing files, please reinstall SMBX2 to fix your problem.",
                                 "Description of a problem, showing when LunaTester is a default engine in a current config pack.");

    for(auto &f : smbxFiles)
    {
        if(!QFile::exists(f))
        {
            QMessageBox::warning(m_w,
                        notFoundErrorTitle,
                        QString("%1\n%2")
                        .arg(notFoundErrorText.arg(f))
                        .arg((ConfStatus::defaultTestEngine == ConfStatus::ENGINE_LUNA) ? explanationSMBX2 : explanationGeneric),
                        QMessageBox::Ok);
            return false;
        }
    }

    for(auto &d : smbxDirs)
    {
        QFileInfo i(d);
        if(!i.exists() || !i.isDir())
        {
            QMessageBox::warning(m_w,
                        notFoundErrorTitle,
                        QString("%1\n%2")
                        .arg(notFoundErrorText.arg(d))
                        .arg((ConfStatus::defaultTestEngine == ConfStatus::ENGINE_LUNA) ? explanationSMBX2 : explanationGeneric),
                        QMessageBox::Ok);
            return false;
        }
    }

    if(!QFile::exists(lunaDllPath))
    {
        QMessageBox::warning(m_w,
                    LunaTesterEngine::tr("Vanilla SMBX detected!"),
                    LunaTesterEngine::tr("\"%1\" not found!\n"
                                         "You have a Vanilla SMBX!\n"
                                         "That means, impossible to launch level testing with a LunaTester. "
                                         "LunaLua is required to run level testing with SMBX Engine.")
                    .arg(lunaDllPath),
                    QMessageBox::Close);
        m_capsNeedReload = true;
        return false;
    }

    if(!m_caps.loaded)
    {
        QMessageBox::critical(m_w,
                              LunaTesterEngine::tr("Incompatible LunaDll found"),
                              LunaTesterEngine::tr("Impossible to start a LunaTester due to an incompatible LunaDll.dll module found in the path: %1").arg(smbxPath),
                              QMessageBox::Ok);
        m_capsNeedReload = true;
        return false;
    }

    if(!m_caps.isCompatible)
    {
        QMessageBox::critical(m_w,
                              LunaTesterEngine::tr("Incompatible LunaLua"),
                              LunaTesterEngine::tr("Impossible to start a LunaTester due to an incompatible LunaLua in the path: %1").arg(smbxPath),
                              QMessageBox::Ok);
        m_capsNeedReload = true;
        return false;
    }

    return true;
}

void LunaTesterEngine::loadSetup()
{
    QSettings settings(ConfStatus::configLocalSettingsFile, QSettings::IniFormat);
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    settings.setIniCodec("UTF-8");
#endif

    settings.beginGroup("LunaTester");
    {
        m_noGL = settings.value("nogl", false).toBool();
        m_killPreviousSession = settings.value("kill-engine-on-every-test", false).toBool();
        m_customLunaPath = settings.value("custom-runtime-path", QString()).toString();
        m_customExeName = settings.value("custom-exe-name", QString()).toString();
#ifndef _WIN32
        WineSetup::iniLoad(settings, m_wineSetup);
#endif
    }
    settings.endGroup();

    if(!reloadLunaCapabilities())
        qWarning() << "Failed to initialize LunaLua capabilities!";
}

void LunaTesterEngine::saveSetup()
{
    QSettings settings(ConfStatus::configLocalSettingsFile, QSettings::IniFormat);
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    settings.setIniCodec("UTF-8");
#endif

    settings.beginGroup("LunaTester");
    {
        settings.setValue("nogl", m_noGL);
        settings.setValue("kill-engine-on-every-test", m_killPreviousSession);
        settings.setValue("custom-runtime-path", m_customLunaPath);
        settings.setValue("custom-exe-name", m_customExeName);
#ifndef _WIN32
        WineSetup::iniSave(settings, m_wineSetup);
#endif
    }
    settings.endGroup();
}


/*****************************************Default actions*************************************************/

bool LunaTesterEngine::doTestLevelIPC(const LevelData &d)
{
    QMutexLocker mlocker(&this->m_engine_mutex);
    Q_UNUSED(mlocker)

    if(!verifyCompatibility())
        return false;

    // Store level buffer
    m_levelTestBuffer = d;

    // Prepare level file for SMBX-64 format
    FileFormats::smbx64LevelPrepare(m_levelTestBuffer);

    if(m_caps.features.contains("SMBX64"))
    {
        int smbx64limits = FileFormats::smbx64LevelCheckLimits(m_levelTestBuffer);
        if(smbx64limits != FileFormats::SMBX64_FINE)
        {
            int reply = QMessageBox::warning(m_w,
                                    tr("SMBX64 limits are exceeded!"),
                                    tr("Violation of SMBX64 standard has been found!\n"
                                       "%1\n"
                                       ", legacy engine may crash!\n"
                                       "Suggested to remove all excess elements.\n"
                                       "Do you want to continue the process?")
                                    .arg(smbx64ErrMsgs(m_levelTestBuffer, smbx64limits)),
                                    QMessageBox::Yes | QMessageBox::No);
            if(reply != QMessageBox::Yes)
                return false;
        }
    }

    //-----------------------------------------------------------------

    //! Path to SMBX root
    const QString smbxPath = getEnginePath();
    //! Full path to LunaTester proxy executable
    const QString execProxy = getBridgePath();

    if(isEngineActive())
    {
        //Attempt to switch SMBX Window
        switchToSmbxWindow();
        //Then send level data to SMBX Engine
        return sendLevelData(m_levelTestBuffer);
    }

    QString command = execProxy;
    QStringList params;

    /**********************************************
     **********Do LunaLUA testing launch!**********
     **********************************************/

    // Reset flags
    m_pendingCommands.clear();

    params << (m_killPreviousSession ? "--patch" : "--hideOnCloseIPC");

    if(m_noGL)
    {
        if(m_caps.args.contains("softGL"))
            params << "--softGL";
        else
            params << "--nogl";
    }

    if(m_caps.args.contains("sendIPCReady"))
        params << "--sendIPCReady";

    useWine(m_lunaGameIPC, command, params);
    m_lunaGameIPC.setProgram(command);
    m_lunaGameIPC.setWorkingDirectory(smbxPath);
    m_lunaGameIPC.start(command, params);
    LogDebug(QString("LunaTester: starting command: %1 %2").arg(command).arg(params.join(' ')));
    return true;
}

bool LunaTesterEngine::doTestLevelFile(const QString &levelFile)
{
    if(!verifyCompatibility())
        return false;

    //! Path to SMBX root
    const QString smbxPath = getEnginePath();
    //! Full path to LunaTester proxy executable
    const QString execProxy = getBridgePath();

    if(!m_caps.args.contains("testLevel"))
        return false; // Unsupported by a current LunaLua build

    if(m_lunaGame.state() == QProcess::Running)
        m_lunaGame.kill();

    m_lunaGame.waitForFinished(2000);

    QString command = execProxy;
    QStringList params;

    if(m_caps.args.contains("patch"))
        params << "--patch";

    params << "--testLevel=" + pathUnixToWine(levelFile);

    if(m_noGL)
    {
        if(m_caps.args.contains("softGL"))
            params << "--softGL";
        else
            params << "--nogl";
    }

    useWine(m_lunaGame, command, params);
    m_lunaGame.setProgram(command);
    m_lunaGame.setArguments(params);
    m_lunaGame.setWorkingDirectory(smbxPath);
    m_lunaGame.start();
    LogDebug(QString("LunaTester: starting command: %1 %2").arg(command).arg(params.join(' ')));

    stopEditorMusic();
    return true;
}

bool LunaTesterEngine::doTestWorldFile(const QString &worldFile)
{
    if(!verifyCompatibility())
        return false;

    //! Path to SMBX root
    const QString smbxPath = getEnginePath();
    //! Full path to LunaTester proxy executable
    const QString execProxy = getBridgePath();
    //! Full path to an autostart.ini file for an episode starting
    const QString autoStart = smbxPath + "autostart.ini";

    bool argsSupported =
            m_caps.args.contains("loadWorld") &&
            m_caps.args.contains("num-players") &&
            m_caps.args.contains("p1c") &&
            m_caps.args.contains("p2c") &&
            m_caps.args.contains("saveslot");

    WorldData wld;

    if(!FileFormats::OpenWorldFileHeader(worldFile, wld))
    {
        QMessageBox::warning(m_w,
                             "LunaTester",
                             tr("Impossible to launch an episode because of an invalid world file."),
                             QMessageBox::Ok);
        return false;
    }

    if(!m_caps.features.contains("WLDX") && (wld.meta.RecentFormat != WorldData::SMBX64))
    {
        QMessageBox::warning(m_w,
                             "LunaTester",
                             tr("Cannot launch the episode because the world map file is saved in an unsupported format. "
                                "Please save the world map in the SMBX64-WLD format."),
                             QMessageBox::Ok);
        return false;
    }

    if(m_lunaGame.state() == QProcess::Running)
        m_lunaGame.kill();

    m_lunaGame.waitForFinished(2000);

    QString command = execProxy;
    QStringList params;

    params << "--patch";
    params << "--game";

    if(argsSupported)
    {
        // For the case were we specify episode launch via command line
        SETTINGS_TestSettings t = GlobalSettings::testing;
        params << ("--loadWorld=" + pathUnixToWine(worldFile));
        params << QString("--num-players=%1").arg((t.numOfPlayers >= 2) ? 2 : 1);
        params << QString("--p1c=%1").arg(t.p1_char);
        if(t.numOfPlayers >= 2)
            params << QString("--p2c=%1").arg(t.p2_char);
        params << QString("--saveslot=%1").arg(0); // Save slot 0 means no saving, test mode
    }
    else
    {
        // For the case were we use autostart.ini
        if(!worldFile.contains(smbxPath + "worlds/"))
        {
            QMessageBox::warning(m_w,
                                 "LunaTester",
                                 tr("Impossible to launch an episode out of LunaTester worlds root."),
                                 QMessageBox::Ok);
            return false;
        }

        if(QFile::exists(autoStart))
            QFile::remove(autoStart);

        SETTINGS_TestSettings  t = GlobalSettings::testing;
        QSettings autostartINI(autoStart, QSettings::IniFormat);
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
        autostartINI.setIniCodec("UTF-8");
#endif

        autostartINI.beginGroup("autostart");
        autostartINI.setValue("do-autostart", true);
        autostartINI.setValue("episode-name", wld.EpisodeTitle);
        autostartINI.setValue("singleplayer", t.numOfPlayers == 1);
        autostartINI.setValue("character-player1", t.p1_char);
        autostartINI.setValue("character-player2", t.p2_char);
        autostartINI.setValue("save-slot", 0); // Save slot 0 means no saving, test mode
        autostartINI.setValue("transient", true);
        autostartINI.endGroup();
        autostartINI.sync();
    }

    if(m_noGL)
    {
        if(m_caps.args.contains("softGL"))
            params << "--softGL";
        else
            params << "--nogl";
    }

    useWine(m_lunaGame, command, params);
    m_lunaGame.setProgram(command);
    m_lunaGame.setArguments(params);
    m_lunaGame.setWorkingDirectory(smbxPath);
    m_lunaGame.start();
    LogDebug(QString("LunaTester: starting command: %1 %2").arg(command).arg(params.join(' ')));

    stopEditorMusic();
    return true;
}

bool LunaTesterEngine::runNormalGame()
{
    if(!verifyCompatibility())
        return false;
    //! Path to SMBX root
    const QString smbxPath = getEnginePath();
    //! Full path to LunaTester proxy executable
    const QString execProxy = getBridgePath();

    if(m_lunaGame.state() == QProcess::Running)
        m_lunaGame.kill();

    m_lunaGame.waitForFinished(2000);

    QString command = execProxy;
    QStringList params;

    params << "--patch";
    params << "--game";

    if(m_noGL)
    {
        if(m_caps.args.contains("softGL"))
            params << "--softGL";
        else
            params << "--nogl";
    }

    useWine(m_lunaGame, command, params);
    m_lunaGame.setProgram(command);
    m_lunaGame.setArguments(params);
    m_lunaGame.setWorkingDirectory(smbxPath);
    m_lunaGame.start();
    LogDebug(QString("LunaTester: starting command: %1 %2").arg(command).arg(params.join(' ')));

    stopEditorMusic();
    return true;
}

void LunaTesterEngine::terminate()
{
    killEngine();
}

bool LunaTesterEngine::isRunning()
{
    return false;
}

int LunaTesterEngine::capabilities()
{
    int caps =
            CAP_LEVEL_IPC |
            CAP_RUN_GAME |
            CAP_DAEMON |
            CAP_HAS_MENU |
            CAP_WORLD_FILE;

    if(m_caps.args.contains("testLevel"))
        caps |= CAP_LEVEL_FILE;

    return caps;
}
