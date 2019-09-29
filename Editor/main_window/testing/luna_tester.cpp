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

/*
    LunaTester provides ability to run level tests in the LunaLUA-SMBX.
    However it works on Windows OS only. To implement similar thing on other OS,
    communication with Wine is required.
*/

#include <mainwindow.h>
#include <ui_mainwindow.h>
#include "luna_tester.h"

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

#include <QThread>
#include <QEventLoop>
#include <QDesktopWidget>
#include <QDirIterator>
#include <QJsonObject>
#include <QMetaObject>
#include <QJsonDocument>
#include <QJsonArray>
#include <QIcon>
#include <QMenu>
#include <QAction>
#include <QtConcurrentRun>

#if !defined(_WIN32)
#include <QProcessEnvironment>
#endif

#ifdef __APPLE__
#include <QRegExp>
#endif

#include "luna_tester.h"

#include <PGE_File_Formats/file_formats.h>
#include <data_functions/smbx64_validation_messages.h>
#include <common_features/app_path.h>
#include <common_features/logger.h>
#include <dev_console/devconsole.h>
#include <main_window/global_settings.h>

class QThreadPointNuller
{
    QThread **pointer;
public:
    explicit QThreadPointNuller(QThread **ptr) : pointer(ptr) {}
    ~QThreadPointNuller()
    {
        *pointer = nullptr;
    }
};

static std::string readIPC(QProcess *input);

#ifdef _WIN32
static BOOL  checkProc(DWORD procId, const wchar_t *proc_name_wanted);
static DWORD getPidsByPath(const std::wstring &process_path, DWORD *found_pids, DWORD found_pids_max_size);

/* Checks matching of single process with full path string by PID */
static BOOL checkProc(DWORD procId, const wchar_t *proc_name_wanted)
{
    wchar_t proc_name[MAX_PATH + 1] = L"<unknown>";
    HMODULE h_mod = 0;
    DWORD cbNeeded;
    DWORD proc_name_len;
    HANDLE h_process = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, procId);

    if(NULL == h_process)
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
static pid_t find_pid(const char *process_name)
{
    pid_t pid = -1;
    glob_t pglob;
    char *procname, *readbuf;
    int buflen = strlen(process_name) + 2;
    unsigned i;
    /* Get a list of all comm files. man 5 proc */
    if (glob("/proc/*/comm", 0, nullptr, &pglob) != 0)
        return pid;
    /* The comm files include trailing newlines, so... */
    procname = (char*)malloc(buflen);
    strcpy(procname, process_name);
    procname[buflen - 2] = '\n';
    procname[buflen - 1] = 0;
    /* readbuff will hold the contents of the comm files. */
    readbuf = (char*)malloc(buflen);
    for (i = 0; i < pglob.gl_pathc; ++i)
    {
        FILE *comm;
        char *ret;
        /* Read the contents of the file. */
        if ((comm = fopen(pglob.gl_pathv[i], "r")) == nullptr)
            continue;
        ret = fgets(readbuf, buflen, comm);
        fclose(comm);
        if (ret == nullptr)
            continue;
        /*
        If comm matches our process name, extract the process ID from the
        path, convert it to a pid_t, and return it.
        */
        if (strcmp(readbuf, procname) == 0)
        {
            pid = (pid_t)std::atoi(pglob.gl_pathv[i] + strlen("/proc/"));
            break;
        }
    }
    /* Clean up. */
    free(procname);
    free(readbuf);
    globfree(&pglob);
    return pid;
}
#endif // !_WIN32 && !__APPLE__


void LunaWorker::init()
{
    if(!m_process)
    {
        m_process = new QProcess;
        QObject::connect(m_process, static_cast<void (QProcess::*)(int, QProcess::ExitStatus)>(&QProcess::finished),
                         this, &LunaWorker::processFinished);
        QObject::connect(m_process, &QProcess::errorOccurred, this, &LunaWorker::errorOccurred);
        m_lastStatus = m_process->state();
    }
}

void LunaWorker::unInit()
{
    QObject::disconnect(m_process, static_cast<void (QProcess::*)(int, QProcess::ExitStatus)>(&QProcess::finished),
                     this, &LunaWorker::processFinished);
    QObject::disconnect(m_process, &QProcess::errorOccurred, this, &LunaWorker::errorOccurred);
}

LunaWorker::LunaWorker(QObject *parent) : QObject(parent)
{
    m_lastStatus = QProcess::NotRunning;
}

LunaWorker::~LunaWorker()
{
    emit stopLoop();
    if(m_process)
    {
        unInit();
        if(m_process->state() == QProcess::Running)
            m_process->kill();
        m_process->waitForFinished(1000);
        delete m_process;
        m_process = nullptr;
    }
}

void LunaWorker::setEnv(const QHash<QString, QString> &env)
{
    if(!m_process)
        return;
    QProcessEnvironment e = QProcessEnvironment::systemEnvironment();
    for(auto it = env.begin(); it != env.end(); it++)
        e.insert(it.key(), it.value());
    m_process->setProcessEnvironment(e);
}

void LunaWorker::start(const QString &command, const QStringList &args, bool *ok, QString *errString)
{
    init();
    Q_ASSERT(m_process);
    LogDebug(QString("LunaTester: starting command: %1 %2").arg(command).arg(args.join(' ')));
    m_process->start(command, args);
    m_lastStatus = m_process->state();
    *ok = m_process->waitForStarted();
    *errString = m_process->errorString();
    if(!*ok)
        LogWarning(QString("LunaTester: startup error: %1").arg(*errString));
    m_lastStatus = m_process->state();
}

void LunaWorker::terminate()
{
    if(m_process && (m_process->state() == QProcess::Running))
    {
        Q_PID pid = m_process->pid();
        LogDebug(QString("LunaWorker: Killing by QProcess::kill()..."));
        QMetaObject::invokeMethod(m_process, "kill", Qt::QueuedConnection);
#ifdef _WIN32
        if(pid)
        {
            DWORD lt_pid = static_cast<DWORD>(pid->dwProcessId);

            HANDLE h_process = OpenProcess(PROCESS_TERMINATE, FALSE, lt_pid);
            if(NULL != h_process)
            {
                LogDebug(QString("LunaWorker: Killing LunaLoader-exec with PID %1 by 'TerminateProcess()'...")
                         .arg(lt_pid));
                BOOL res = TerminateProcess(h_process, 0);
                if(!res)
                {
                    LogDebug(
                        QString("LunaWorker: Failed to kill LunaLoader-exec with PID %1 by 'TerminateProcess()' with error '%3', possibly it's already terminated")
                            .arg(lt_pid)
                            .arg(GetLastError())
                    );
                }
                CloseHandle(h_process);
            }
            else
            {
                LogDebug("LunaWorker: LunaLoader-exec is not run");
            }
        }

        // Kill everything that has "smbx.exe"
        DWORD proc_id[1024];
        DWORD proc_count = 0;
        QString smbxPath = ConfStatus::configDataPath + ConfStatus::SmbxEXE_Name;
        smbxPath.replace('/', '\\');
        proc_count = getPidsByPath(smbxPath.toStdWString(), proc_id, 1024);
        if(proc_count > 0)
        {
            LogDebug(QString("LunaWorker: Found matching PIDs for running %1, going to kill...")
                     .arg(ConfStatus::SmbxEXE_Name));
            for(DWORD i = 0; i < proc_count; i++)
            {
                DWORD f_pid = proc_id[i];
                HANDLE h_process = OpenProcess(PROCESS_TERMINATE, FALSE, f_pid);
                if(NULL != h_process)
                {
                    BOOL res = TerminateProcess(h_process, 0);
                    if(!res)
                    {
                        LogDebug(
                            QString("LunaWorker: Failed to kill %1 with PID %2 by 'TerminateProcess()' with error '%3', possibly it's already terminated.")
                                 .arg(ConfStatus::SmbxEXE_Name)
                                 .arg(f_pid)
                                 .arg(GetLastError())
                        );
                    }
                    CloseHandle(h_process);
                }
            }
        }
        else
            LogDebug(QString("LunaWorker: No matching PIDs found for %1:").arg(ConfStatus::SmbxEXE_Name));
#else // _WIN32
        if(pid)
            kill(static_cast<pid_t>(pid), SIGTERM);
#   ifdef __APPLE__
        LogDebug(QString("LunaWorker: Killing %1 by 'kill'...").arg(ConfStatus::SmbxEXE_Name));
        QProcess ps;
        ps.start("/bin/ps", {"-A"});
        ps.waitForFinished();
        QString psAll = ps.readAllStandardOutput();

        QStringList psAllList = psAll.split('\n');
        QString smbxExeName = ConfStatus::SmbxEXE_Name;

        QRegExp psReg(QString("(\\d+) .*(wine-preloader).*(%1)").arg(smbxExeName));
        for(QString &psOne : psAllList)
        {
            int pos = 0;
            while((pos = psReg.indexIn(psOne, pos)) != -1)
            {
                pid_t toKill = static_cast<pid_t>(psReg.cap(1).toUInt());
                LogDebug(QString("LunaWorker: kill -TERM %1").arg(toKill));
                kill(toKill, SIGTERM);
                pos += psReg.matchedLength();
            }
        }
#   else
        pid = find_pid(ConfStatus::SmbxEXE_Name.toUtf8().data());
        LogDebug(QString("LunaWorker: Killing %1 by pid %2...")
            .arg(ConfStatus::SmbxEXE_Name)
            .arg(pid));
        if(pid)
            kill(static_cast<pid_t>(pid), SIGKILL);
#   endif //__APPLE__
#endif // _WIN32
    }
}

void LunaWorker::write(const QString &out, bool *ok)
{
    if(!m_process)
    {
        *ok = false;
        return;
    }
    QByteArray o = out.toUtf8();
    *ok = m_process->write(o) == qint64(o.size());
    m_process->waitForBytesWritten();
}

void LunaWorker::read(QString *in, bool *ok)
{
    if(!m_process)
    {
        *ok = false;
        return;
    }
    m_process->waitForReadyRead();
    *in = QString::fromStdString(readIPC(m_process));
    *ok = !in->isEmpty();
}

void LunaWorker::writeStd(const std::string &out, bool *ok)
{
    if(!m_process)
    {
        *ok = false;
        return;
    }
    *ok = m_process->write(out.c_str(), qint64(out.size())) == qint64(out.size());
    m_process->waitForBytesWritten();
}

void LunaWorker::readStd(std::string *in, bool *ok)
{
    if(!m_process)
    {
        *ok = false;
        return;
    }
    m_process->waitForReadyRead();
    *in = readIPC(m_process);
    *ok = !in->empty();
}

void LunaWorker::processLoop()
{
    QEventLoop loop;
    QObject::connect(this, &LunaWorker::stopLoop,
                     &loop, &QEventLoop::quit,
                     Qt::BlockingQueuedConnection);
    loop.exec();
    emit loopFinished();
}

void LunaWorker::quitLoop()
{
    emit stopLoop();
}

bool LunaWorker::isActive()
{
    return m_lastStatus == QProcess::Running;
}

void LunaWorker::errorOccurred(QProcess::ProcessError err)
{
    QString errString;
    switch(err)
    {
    case QProcess::FailedToStart:
        errString = "Failed to start";
        break;
    case QProcess::Crashed:
        errString = "Crashed";
        break;
    case QProcess::Timedout:
        errString = "Timed out";
        break;
    case QProcess::ReadError:
        errString = "Read error";
        break;
    case QProcess::WriteError:
        errString = "Write error";
        break;
    case QProcess::UnknownError:
        errString = "Unknown error";
        break;
    }
    LogDebug(QString("LunaWorker: Process finished with error: %1").arg(errString));
    m_lastStatus = m_process->state();
}

void LunaWorker::processFinished(int exitCode, QProcess::ExitStatus exitStatus)
{
    LogDebug(QString("LunaWorker: Process finished with code %1 and %2 exit")
        .arg(exitCode)
        .arg(exitStatus == QProcess::NormalExit ? "normal" : "crash")
    );
    m_lastStatus = m_process->state();
}

#ifndef _WIN32
void LunaTester::useWine(QString &command, QStringList &args)
{
    args.push_front(command);
    command = m_wineBinDir + "wine";
    emit engineSetEnv(m_wineEnv);
}

QString LunaTester::pathUnixToWine(const QString &unixPath)
{
    QProcess winePath;
    QStringList args;
    // Ask for in-Wine Windows path from in-UNIX native path
    args << "--windows" << unixPath;
    // Use wine custom environment
    QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
    for(auto it = m_wineEnv.begin(); it != m_wineEnv.end(); it++)
        env.insert(it.key(), it.value());
    winePath.setProcessEnvironment(env);
    // Start winepath
    winePath.start(m_wineBinDir + "winepath", args);
    winePath.waitForFinished();
    // Retrieve converted path
    QString windowsPath = winePath.readAllStandardOutput();
    return windowsPath;
}
#else
void LunaTester::useWine(QString &, QStringList &) // Dummy
{}

QString LunaTester::pathUnixToWine(const QString &unixPath)
{
    // dummy, no need on real Windows
    return unixPath;
}
#endif

LunaTester::LunaTester() :
    QObject(nullptr),
    m_mw(nullptr),
    m_menuItems{nullptr},
    m_helperThread(nullptr),
    m_noGL(false)
{
#ifndef _WIN32
#   ifdef __APPLE__
#       define WINE_PREFIX_BIN "/usr/local/bin/"
#   else
#       define WINE_PREFIX_BIN ""
#   endif
    m_wineBinDir = WINE_PREFIX_BIN;
#endif
}

LunaTester::~LunaTester()
{
    if(m_helper.isRunning())
    {
        killEngine();
        m_helper.waitForFinished();
    }
    if(!m_thread.isNull())
    {
        m_worker->unInit();
        m_worker->terminate();
        m_worker->quitLoop();
        m_thread->quit();
        m_thread->requestInterruption();
        m_thread->wait();
    }
    m_worker.reset();
    m_thread.reset();
}

void LunaTester::initRuntime()
{
    if(m_worker.isNull() && m_thread.isNull())
    {
        m_worker.reset(new LunaWorker());
        m_thread.reset(new QThread());
        m_worker->moveToThread(m_thread.data());
        auto *worker_ptr = m_worker.data();
        auto *thread_ptr = m_thread.data();
        QObject::connect(thread_ptr, SIGNAL(started()),
                         worker_ptr, SLOT(processLoop()));
        QObject::connect(worker_ptr, SIGNAL(loopFinished()),
                         thread_ptr, SLOT(quit()));
        QObject::connect(this, &LunaTester::engineSetEnv, worker_ptr,
                &LunaWorker::setEnv, Qt::BlockingQueuedConnection);
        QObject::connect(this, &LunaTester::engineStart, worker_ptr,
                &LunaWorker::start, Qt::BlockingQueuedConnection);
        QObject::connect(this, &LunaTester::engineWrite, worker_ptr,
                &LunaWorker::write, Qt::BlockingQueuedConnection);
        QObject::connect(this, &LunaTester::engineRead, worker_ptr,
                &LunaWorker::read, Qt::BlockingQueuedConnection);
        QObject::connect(this, &LunaTester::engineWriteStd, worker_ptr,
                &LunaWorker::writeStd, Qt::BlockingQueuedConnection);
        QObject::connect(this, &LunaTester::engineReadStd, worker_ptr,
                &LunaWorker::readStd, Qt::BlockingQueuedConnection);
        m_thread->start();
    }
}

void LunaTester::initLunaMenu(MainWindow *mw,
                              QMenu *mainMenu,
                              QAction *insert_before,
                              QAction *defaultTestAction,
                              QAction *secondaryTestAction,
                              QAction *startEngineAction)
{
    m_mw = mw;
    QIcon lunaIcon(":/lunalua.ico");
    QMenu *lunaMenu = mainMenu->addMenu(lunaIcon, "LunaTester");
    mainMenu->insertMenu(insert_before, lunaMenu);

    size_t menuItemId = 0;
    QAction *RunLunaTest;
    {
        RunLunaTest = lunaMenu->addAction("runTesting");
        QObject::connect(RunLunaTest,    &QAction::triggered,
                    this,           &LunaTester::startLunaTester,
                    Qt::QueuedConnection);
        m_menuItems[menuItemId++] = RunLunaTest;
    }
    QAction *ResetCheckPoints = lunaMenu->addAction("resetCheckpoints");
    {
        QObject::connect(ResetCheckPoints,   &QAction::triggered,
                    this,               &LunaTester::resetCheckPoints,
                    Qt::QueuedConnection);
        m_menuItems[menuItemId++] = ResetCheckPoints;
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
        QAction *KillFrozenThread = lunaMenu->addAction("Terminate frozen loader");
        QObject::connect(KillFrozenThread,   &QAction::triggered,
                    this,               &LunaTester::killFrozenThread,
                    Qt::QueuedConnection);
        m_menuItems[menuItemId++] = KillFrozenThread;
    }
    {
        QAction *KillBackgroundInstance = lunaMenu->addAction("KillBackgroundInstance");
        QObject::connect(KillBackgroundInstance,   &QAction::triggered,
                    this,               &LunaTester::killBackgroundInstance,
                    Qt::QueuedConnection);
        m_menuItems[menuItemId++] = KillBackgroundInstance;
    }
    {
        lunaMenu->addSeparator();
        QAction *runLegacyEngine = lunaMenu->addAction("startLegacyEngine");
        QObject::connect(runLegacyEngine,   &QAction::triggered,
                    this,              &LunaTester::lunaRunGame,
                    Qt::QueuedConnection);
        m_menuItems[menuItemId++] = runLegacyEngine;
    }

    QAction *sep = lunaMenu->addSeparator();
    mainMenu->insertAction(insert_before, sep);

    retranslateMenu();
    connect(mw, &MainWindow::languageSwitched, this, &LunaTester::retranslateMenu);

    if(ConfStatus::SmbxTest_By_Default)
    {
        defaultTestAction->setShortcut(QStringLiteral(""));
        defaultTestAction->setShortcutContext(Qt::WindowShortcut);
        QIcon pgeEngine;
        pgeEngine.addPixmap(QPixmap(":/images/cat/cat_16.png"));
        pgeEngine.addPixmap(QPixmap(":/images/cat/cat_32.png"));
        pgeEngine.addPixmap(QPixmap(":/images/cat/cat_48.png"));

        mainMenu->insertAction(defaultTestAction, RunLunaTest);

        mainMenu->removeAction(defaultTestAction);
        mainMenu->removeAction(secondaryTestAction);
        mainMenu->removeAction(startEngineAction);
        if(!ConfStatus::SmbxTest_HidePgeEngine)
        {
            QMenu *pgeEngineMenu = mainMenu->addMenu(pgeEngine, "PGE Engine");
            mainMenu->insertMenu(lunaMenu->menuAction(), pgeEngineMenu);
            pgeEngineMenu->insertAction(nullptr, defaultTestAction);
            pgeEngineMenu->insertAction(nullptr, secondaryTestAction);
            pgeEngineMenu->addSeparator();
            pgeEngineMenu->insertAction(nullptr, startEngineAction);
        }

        RunLunaTest->setShortcut(QStringLiteral("F5"));
        RunLunaTest->setShortcutContext(Qt::WindowShortcut);
        RunLunaTest->setIcon(lunaIcon);

        ResetCheckPoints->setShortcut(QStringLiteral("Ctrl+F5"));
        ResetCheckPoints->setShortcutContext(Qt::WindowShortcut);
    }
}

void LunaTester::retranslateMenu()
{
    size_t menuItemId = 0;
    {
        QAction *RunLunaTest = m_menuItems[menuItemId++];
        RunLunaTest->setText(tr("Test level",
                                "Run the LunaTester based level testing."));
        RunLunaTest->setToolTip(tr("Starts level testing in the legacy engine.\n"
                                   "To have this feature work, latest LunaLUA must be installed.\n"
                                   "Otherwise, it will be very limited."));
    }
    {
        QAction *ResetCheckPoints = m_menuItems[menuItemId++];
        ResetCheckPoints->setText(tr("Reset checkpoints"));
        ResetCheckPoints->setToolTip(tr("Reset all checkpoint states to initial state."));
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
        QAction *KillFrozenThread = m_menuItems[menuItemId++];
        KillFrozenThread->setText(tr("Terminate frozen loader",
                                     "Terminite frozen LunaTester on the attempt to send any command to LunaLua."));
        KillFrozenThread->setToolTip(tr("Termiates frozen thread to allow you to run a test again."));
    }
    {
        QAction *KillBackgroundInstance = m_menuItems[menuItemId++];
        KillBackgroundInstance->setText(tr("Terminate running process",
                                           "Ends the LunaTester process, regardless of whether it's in \n"
                                           "the background or foreground, so the engine can be loaded from scratch."));
        KillBackgroundInstance->setToolTip(tr("Ends the LunaTester process so the engine can be loaded from scratch."));
    }
    {
        QAction *runLegacyEngine = m_menuItems[menuItemId++];
        runLegacyEngine->setText(tr("Start Legacy Engine",
                                    "Launch legacy engine in game mode"));
        runLegacyEngine->setToolTip(tr("Launch legacy engine in game mode."));
    }
}

bool LunaTester::isEngineActive()
{
    if(m_worker.isNull())
        return false;
    return m_worker->isActive();
}

bool LunaTester::isInPipeOpen()
{
    if(m_worker.isNull())
        return false;
    return m_worker->isActive();
}

bool LunaTester::isOutPipeOpen()
{
    if(m_worker.isNull())
        return false;
    return m_worker->isActive();
}

bool LunaTester::writeToIPC(const std::string &out)
{
    bool ret = false;
    emit engineWriteStd(out, &ret);
    return ret;
}

bool LunaTester::writeToIPC(const QString &out)
{
    bool ret = false;
    emit engineWrite(out, &ret);
    return ret;
}

std::string LunaTester::readFromIPC()
{
    std::string out;
    bool ok = false;
    emit engineReadStd(&out, &ok);
    return out;
}

QString LunaTester::readFromIPCQ()
{
    QString out;
    bool ok = false;
    emit engineRead(&out, &ok);
    return out;
}


void LunaTester::killEngine()
{
    if(!m_thread.isNull())
        m_worker->terminate();
}

/*****************************************Menu slots*************************************************/

inline void busyThreadBox(MainWindow *mw)
{
    QMessageBox::warning(mw,
                         "LunaTester",
                         LunaTester::tr("LunaLUA test loader thread is busy, try again or try termiate frozen loader!"),
                         QMessageBox::Ok);
}

/*!
 * \brief Starts testing in the hacked with LunaLUA-SMBX Engine if possible (Only for Windows builds)
 */
void LunaTester::startLunaTester()
{
    initRuntime();
    if(m_helper.isRunning())
        busyThreadBox(m_mw);
    else
    {
        if(m_mw->activeChildWindow() == MainWindow::WND_Level)
        {
            LevelEdit *lvl = m_mw->activeLvlEditWin();
            if(lvl)
            {
                if(m_killPreviousSession)
                    killEngine();

                lvl->prepareLevelFile(lvl->LvlData);
                m_helper = QtConcurrent::run(this,
                                             &LunaTester::lunaRunnerThread,
                                             lvl->LvlData,
                                             lvl->curFile,
                                             lvl->isUntitled());
            }
        }
        else
        if(m_mw->activeChildWindow() == MainWindow::WND_World)
        {
            QMessageBox::information(m_mw,
                                     "LunaTester",
                                     LunaTester::tr("LunaTester can't be used on world map because the legacy engine "
                                                    "doesn't provide ability to run testing of world maps. "
                                                    "Instead, you can use PGE Engine (\"Test\" -> \"Test saved file\" "
                                                    "menu item) to test your world map file in action without episode running."),
                                     QMessageBox::Ok);
        }
    }
}

void LunaTester::resetCheckPoints()
{
    if(m_helper.isRunning())
        busyThreadBox(m_mw);
    else
        m_helper = QtConcurrent::run(this, &LunaTester::lunaChkResetThread);
}

void LunaTester::killFrozenThread()
{
    if(m_helper.isRunning())
    {
        QMessageBox::StandardButton reply = QMessageBox::warning(m_mw,
                                            "LunaTester",
                                            tr("Are you really want to terminate loader thread?"),
                                            QMessageBox::Yes | QMessageBox::No);
        if(reply == QMessageBox::Yes)
        {
            killEngine();
        }
    }
    else
    {
        QMessageBox::information(m_mw,
                                 "LunaTester",
                                 tr("Loader thread is not running.",
                                    "LunaTester thread now doing nothing.\n"
                                    "This message is shown on request to terminate "
                                    "frozen LunaTester while it is not "
                                    "actually frozen and responds."),
                                 QMessageBox::Ok);
    }
}

void LunaTester::killBackgroundInstance()
{
    if(isEngineActive())
    {
        QMessageBox::StandardButton reply = QMessageBox::warning(m_mw,
                                            "LunaTester",
                                            tr("Are you sure you want to close LunaTester? If you are testing a level, this will immediately end it!"),
                                            QMessageBox::Yes | QMessageBox::No);
        if(reply == QMessageBox::Yes)
        {
            killEngine();
            QMessageBox::information(m_mw,
                         "LunaTester",
                         tr("LunaTester has been successfully closed."),
                         QMessageBox::Ok);
        }
    }
    else
    {
        QMessageBox::information(m_mw,
                                 "LunaTester",
                                 tr("LunaTester is not running."),
                                 QMessageBox::Ok);
    }
}


/*****************************************Private functions*************************************************/

/**
 * @brief Converting JSON data into net string
 * @param jd Input JSON document
 * @param outString Output raw NET string
 */
static void jSonToNetString(const QJsonDocument &jd, std::string &outString)
{
    QByteArray outputJSON = jd.toJson();
    auto len = static_cast<size_t>(outputJSON.size());
    const char *dataToSend = outputJSON.data();
    std::string len_std = std::to_string(len) + ":";

    outString.clear();
    outString.append(len_std);
    outString.append(dataToSend, len);
    outString.push_back(',');
}

static bool stringToJson(const std::string &message, QJsonDocument &out, QJsonParseError &err)
{
    QByteArray jsonData(message.c_str(), static_cast<int>(message.size()));
    out = QJsonDocument::fromJson(jsonData, &err);
    return (err.error == QJsonParseError::NoError);
}

static std::string readIPC(QProcess *input)
{
    // Note: This is not written to be particularly efficient right now. Just
    //       readable enough and safe.
    if(!input->isOpen())
        return std::string();
    char c;
    std::vector<char> data;
    while(true)
    {
        data.clear();
        // Read until : delimiter
        bool err = false;
        while(true)
        {
            qint64 bytesRead = input->read(&c, 1);
            if(bytesRead != 1)
                return "";
            if(c == ':')
                break;
            if((c > '9') || (c < '0'))
            {
                err = true;
                break;
            }

            data.push_back(c);
        }

        if(err)
            continue;

        std::string byteCountStr = std::string(&data[0], data.size());
        data.clear();
        try
        {
            // Interpret as number
            int byteCount = byteCountStr.empty() ? 0 : std::stoi(byteCountStr);
            if(byteCount <= 0)
                continue;
            int byteCursor = 0;
            data.resize(static_cast<size_t>(byteCount));
            while(byteCursor < byteCount)
            {
                qint64 bytesRead = input->read(&data[static_cast<size_t>(byteCursor)], 1);
                if(bytesRead == 0)
                    return "";
                byteCursor += bytesRead;
            }
            // Get following comma
            {
                qint64 bytesRead = input->read(&c, 1);
                if(bytesRead != 1)
                    return "";
                if(c != ',')
                    continue;
            }
        }
        catch(const std::exception &)
        {
            return "";
        }
        catch(...)
        {
            return "";
        }
        return std::string(&data[0], data.size());
    }
}

bool LunaTester::sendLevelData(LevelData &lvl, QString levelPath, bool isUntitled)
{
    //{"jsonrpc": "2.0", "method": "testLevel", "params":
    //   {"filename": "myEpisode/test.lvl", "leveldata": "<RAW SMBX64 LEVEL DATA>", "players": [{"character": 1}], "godMode": false, "showFPS": false },
    //"id": 3}

    //"players": [ { "character": 1, "powerup": 1, "mountType": 1, "mountColor": 1 }, { "character": 2, "powerup": 1, "mountType": 1, "mountColor": 1 } ]

    if(!isOutPipeOpen())
        return false;

    // Ask "Is LVLX"
    bool hasLvlxSupport = false;
    {
        QJsonDocument jsonOut;
        QJsonObject jsonObj;
        jsonObj["jsonrpc"] = "2.0";
        jsonObj["method"] = "getSupportedFeatures";
        jsonObj["id"] = 3;
        jsonOut.setObject(jsonObj);
        std::string dataToSend;
        LogDebug("LunaTester: -> Checking for LVLX support on LunaLua side...");

        jSonToNetString(jsonOut, dataToSend);
        LogDebugQD(QString("Sending message to SMBX %1").arg(QString::fromStdString(dataToSend)));

        if(writeToIPC(dataToSend))
        {
            std::string resultMsg = readFromIPC();
            LogDebugQD(QString("LunaTester: Received from SMBX JSON message: %1").arg(QString::fromStdString(resultMsg)));
            QJsonParseError errData;
            QJsonDocument jsonOutData;
            if(stringToJson(resultMsg, jsonOutData, errData))
            {
                QJsonObject obj = jsonOutData.object();
                QJsonObject result = obj["result"].toObject();
                if(!result["LVLX"].isNull())
                {
                    hasLvlxSupport = result["LVLX"].toBool();
                    LogDebug("LunaTester: <- Yes! LVLX is supported!");
                }
            }
            if(errData.error != QJsonParseError::NoError)
            {
                LogDebug("LunaTester: <- Oops, fail to parse: " + errData.errorString());
            }
        }

        if(!hasLvlxSupport)
        {
            LogDebug("LunaTester: <- No! LVLX is NOT supported by this LunaLua build!");
        }
    }

    QJsonDocument jsonOut;
    QJsonObject jsonObj;
    jsonObj["jsonrpc"] = "2.0";
    jsonObj["method"] = "testLevel";

    QJsonObject JSONparams;
    QString levelPathOut;
    QString smbxPath = ConfStatus::configDataPath;

    if(!isUntitled)
    {
        if(!hasLvlxSupport && levelPath.endsWith(".lvlx", Qt::CaseInsensitive))
            levelPath.remove(levelPath.size() - 1, 1);
        levelPathOut = levelPath;
    }
    else
        levelPathOut = smbxPath + "/worlds/untitled.lvl" + (hasLvlxSupport ? "x" : "");

    JSONparams["filename"] = pathUnixToWine(levelPathOut);

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
    //Generate actual SMBX64 Level file data
    if(hasLvlxSupport)
    {
        FileFormats::WriteExtendedLvlFileRaw(lvl, LVLRawData);
    }
    else
    {
        FileFormats::WriteSMBX64LvlFileRaw(lvl, LVLRawData, 64);
        //Set CRLF
        LVLRawData.replace("\n", "\r\n");
    }
    //Store data into JSON
    JSONparams["leveldata"] = LVLRawData;

    jsonObj["params"] = JSONparams;
    jsonObj["id"] = 3;
    jsonOut.setObject(jsonObj);

    //Converting JSON data into net string
    std::string dataToSend;
    jSonToNetString(jsonOut, dataToSend);

    LogDebug("LunaTester: -> Sending level data and testing request...");
    //Send data to SMBX
    if(writeToIPC(dataToSend))
    {
        //Read result from level testing run
        std::string resultMsg = readFromIPC();
        LogDebugQD(QString("LunaTester: Received from SMBX JSON message: %1").arg(QString::fromStdString(resultMsg)));
        LogDebug("LunaTester: <- Command has been sent!");
        return true;
    }
    LogWarning("<- Fail to send level data and testing request into LunaTester!");
    return false;
}

void LunaTester::lunaChkResetThread()
{
    QMutexLocker mlocker(&this->m_engine_mutex);
    Q_UNUSED(mlocker);
    QThreadPointNuller tlocker(&this->m_helperThread);
    Q_UNUSED(tlocker);
    SafeMsgBoxInterface msg(&m_mw->m_messageBoxer);
    this->m_helperThread = QThread::currentThread();

    if(isEngineActive())
    {
        if(!isOutPipeOpen() || !isInPipeOpen())
            goto failed;
        QJsonDocument jsonOut;
        QJsonObject jsonObj;
        jsonObj["jsonrpc"]  = "2.0";
        jsonObj["method"]   = "resetCheckPoints";
        QJsonObject JSONparams;
        JSONparams["xxx"] = 0;
        jsonObj["params"] = JSONparams;
        jsonObj["id"] = 3;
        jsonOut.setObject(jsonObj);

        //Converting JSON data into net string
        std::string dataToSend;
        jSonToNetString(jsonOut, dataToSend);

        //Send data to SMBX
        if(writeToIPC(dataToSend))
        {
            std::string inMessage = readFromIPC();
            LogDebugQD(QString("Received from SMBX JSON message: %1").arg(QString::fromStdString(inMessage)));
            QJsonParseError err;
            QJsonDocument jsonOut;
            if(stringToJson(inMessage, jsonOut, err))
            {
                QJsonObject obj = jsonOut.object();
                if(obj["error"].isNull())
                {
                    msg.info("LunaTester",
                             tr("Checkpoints successfully reseted!"),
                             QMessageBox::Ok);
                }
                else
                {
                    msg.warning("LunaTester",
                                QString::fromStdString(inMessage),
                                QMessageBox::Ok);
                }
            }
            else
            {
                msg.warning(tr("LunaTester error!"),
                            err.errorString() +
                            "\n\nData:\n" +
                            QString::fromStdString(inMessage),
                            QMessageBox::Warning);
            }
        }
    }
    else
        goto failed;
    return;
failed:
    msg.warning("LunaTester", tr("LunaLUA tester is not started!"), QMessageBox::Ok);
}

bool LunaTester::closeSmbxWindow()
{
    if(m_killPreviousSession || (!isOutPipeOpen()) || (!isInPipeOpen()))
        return false;

    QJsonDocument jsonOut;
    QJsonObject jsonObj;
    jsonObj["jsonrpc"]  = "2.0";
    jsonObj["method"]   = "getWindowHandle";
    QJsonObject JSONparams;
    JSONparams["xxx"] = 0;
    jsonObj["params"] = JSONparams;
    jsonObj["id"] = 3;
    jsonOut.setObject(jsonObj);

    //Converting JSON data into net string
    std::string dataToSend;
    jSonToNetString(jsonOut, dataToSend);

    //Send data to SMBX
    if(writeToIPC(dataToSend))
    {
        std::string inMessage = readFromIPC();
        LogDebugQD(QString("Received from SMBX JSON message: %1").arg(QString::fromStdString(inMessage)));
        QJsonParseError err;
        QJsonDocument jsonOut;
        if(stringToJson(inMessage, jsonOut, err))
        {
            QJsonObject obj = jsonOut.object();
            if(obj["error"].isNull())
            {
#ifdef _WIN32 // Windows-only trick
                uintptr_t smbxHwnd = uintptr_t(obj["result"].toVariant().toULongLong());
                HWND wSmbx = HWND(smbxHwnd);
                //Close SMBX's window
                PostMessage(wSmbx, WM_CLOSE, 0, 0);
                //Wait half of second to avoid racings
                Sleep(500);
#endif
            }
            else
                LogWarningQD("LunaTester (closeSmbxWindow, obj[\"error\"]): "
                             "LunaLua returned error message:\n" +
                             QString::fromStdString(inMessage));
        }
        else
        {
            LogWarningQD("LunaTester (closeSmbxWindow): Error of parsing "
                         "input data from LunaLua:\n" + err.errorString() +
                         "\n\nData:\n" + QString::fromStdString(inMessage));
        }
        return true;
    }
    return false;
}

bool LunaTester::switchToSmbxWindow(SafeMsgBoxInterface &msg)
{
    if(!isOutPipeOpen() || !isInPipeOpen())
        return false;

    QJsonDocument jsonOut;
    QJsonObject jsonObj;
    jsonObj["jsonrpc"]  = "2.0";
    jsonObj["method"]   = "getWindowHandle";
    QJsonObject JSONparams;
    JSONparams["xxx"] = 0;
    jsonObj["params"] = JSONparams;
    jsonObj["id"] = 3;
    jsonOut.setObject(jsonObj);

    //Converting JSON data into net string
    std::string dataToSend;
    jSonToNetString(jsonOut, dataToSend);

    //Send data to SMBX
    if(writeToIPC(dataToSend))
    {
        std::string inMessage = readFromIPC();
        LogDebugQD(QString("Received from SMBX JSON message: %1").arg(QString::fromStdString(inMessage)));
        QJsonParseError err;
        QJsonDocument jsonOut;
        if(stringToJson(inMessage, jsonOut, err))
        {
            QJsonObject obj = jsonOut.object();
            if(obj["error"].isNull())
            {
#ifdef _WIN32 // Windows-only trick
                uintptr_t smbxHwnd = uintptr_t(obj["result"].toVariant().toULongLong());
                HWND wSmbx = HWND(smbxHwnd);
                //msg.warning("Test2", QString("Received: %1, from: %2").arg(smbxHwnd).arg(QString::fromStdString(inMessage)), QMessageBox::Ok);
                //wchar_t title[MAX_PATH];
                //GetWindowTextW(wSmbx, title, MAX_PATH);
                //msg.warning("Test2", QString("Window title is: %1").arg(QString::fromWCharArray(title)), QMessageBox::Ok);
                ShowWindow(wSmbx, SW_SHOW);
                SetForegroundWindow(wSmbx);
                SetActiveWindow(wSmbx);
                SetFocus(wSmbx);
#endif
            }
            else
                msg.warning("LunaTester (switchToSmbxWindow, obj[\"error\"])",
                            "LunaLua returned error message:\n" +
                            QString::fromStdString(inMessage),
                            QMessageBox::Ok);
        }
        else
        {
            msg.warning("LunaTester (switchToSmbxWindow)",
                        "Error of parsing input data from LunaLua:\n" +
                        err.errorString() +
                        "\n\nData:\n" + QString::fromStdString(inMessage),
                        QMessageBox::Warning);
        }
        return true;
    }
    return false;
}

void LunaTester::lunaRunnerThread(LevelData in_levelData, const QString &levelPath, bool isUntitled)
{
    QMutexLocker mlocker(&this->m_engine_mutex);
    Q_UNUSED(mlocker);
    QThreadPointNuller tlocker(&this->m_helperThread);
    Q_UNUSED(tlocker);

    SafeMsgBoxInterface msg(&m_mw->m_messageBoxer);

    this->m_helperThread = QThread::currentThread();

    // Prepare level file for SMBX-64 format
    FileFormats::smbx64LevelPrepare(in_levelData);

    int smbx64limits = FileFormats::smbx64LevelCheckLimits(in_levelData);
    if(smbx64limits != FileFormats::SMBX64_FINE)
    {
        int reply = msg.warning(tr("SMBX64 limits are excited!"),
                                tr("Violation of SMBX64 standard has beeen found!\n"
                                   "%1\n"
                                   ", legacy engine may crash!\n"
                                   "Suggested to remove all excess elements.\n"
                                   "Are you want continue process?")
                                .arg(smbx64ErrMsgs(in_levelData, smbx64limits)),
                                QMessageBox::Yes | QMessageBox::No);
        if(reply != QMessageBox::Yes)
            return;
    }

    // Launch LunaLoader
    {
        QString smbxPath = ConfStatus::configDataPath;
        if(!QFile(smbxPath + ConfStatus::SmbxEXE_Name).exists())
        {
            msg.warning(LunaTester::tr("SMBX Directory wasn't configured right"),
                        LunaTester::tr("%1 not found!\nTo run testing via SMBX you should have right SMBX Integration configuration package!")
                        .arg(smbxPath + ConfStatus::SmbxEXE_Name),
                        QMessageBox::Ok);
            return;
        }

        if(isEngineActive())
        {
            if(isOutPipeOpen())
            {
                //Workaround to avoid weird crash
                closeSmbxWindow();
                //Then send level data to SMBX Engine
                if(sendLevelData(in_levelData, levelPath, isUntitled))
                {
                    //Stop music playback in the PGE Editor!
                    QMetaObject::invokeMethod(m_mw, "setMusicButton", Qt::QueuedConnection, Q_ARG(bool, false));
                    // not sure how efficient it is
                    QMetaObject::invokeMethod(m_mw, "on_actionPlayMusic_triggered", Qt::QueuedConnection, Q_ARG(bool, false));
                    //Attempt to switch SMBX Window
                    switchToSmbxWindow(msg);
                }
            }
            else if(msg.warning(LunaTester::tr("SMBX Test is already runned"),
                                LunaTester::tr("SMBX Engine is already testing another level.\n"
                                               "Do you want to abort current testing process?"),
                                QMessageBox::Abort | QMessageBox::Cancel) == QMessageBox::Abort)
            {
                killEngine();
            }
            return;
        }

        QString command = smbxPath + "LunaLoader-exec.exe";
        QStringList params;

        if(!QFile(smbxPath + "LunaDll.dll").exists())
        {
#ifdef _WIN32
            /**********************************************
             *****Do Vanilla test with dummy episode!******
             **********************************************/
            {
                int msgRet = msg.richBox(LunaTester::tr("Vanilla SMBX detected!"),
                                         LunaTester::tr("%2 not found!\nYou have a Vanilla SMBX!<br>\n"
                                                        "That means, impossible to launch level testing automatically. "
                                                        "To launch a level testing, will be generated a dummy episode which you can "
                                                        "start and select manually.<br>\n<br>\n"
                                                        "Name of episode to generate: %1<br>\n<br>\n"
                                                        "Are you still want to launch a test?<br>\n<br>\n"
                                                        "If you want to have a full featured level testing, you need to get a LunaLUA here:<br>\n"
                                                        "%3")
                                         .arg("\"_temp_episode_pge\"")
                                         .arg(smbxPath + "LunaDll.dll")
                                         .arg("<a href=\"http://wohlsoft.ru/LunaLua/\">http://wohlsoft.ru/LunaLua/</a>"),
                                         QMessageBox::Yes | QMessageBox::No, QMessageBox::Warning);

                if(msgRet != QMessageBox::Yes)
                    return;
            }

            QString tempPath        = smbxPath + "/worlds/";
            QString tempName        = "tmp.PgeWorld.DeleteMe";
            QString dst_Episode     = tempPath + tempName + "/";
            QString src_episodePath = in_levelData.meta.path;
            QString src_customPath  = in_levelData.meta.path + "/" + in_levelData.meta.filename;

            QDir dummyWorld(dst_Episode);
            if(dummyWorld.exists(dst_Episode))
            {
                QString testFolder = dst_Episode + "templevel/";
                RemoveDirectoryW(testFolder.toStdWString().c_str());//Remove symblic link if possible
                //Clean-up old stuff
                dummyWorld.removeRecursively();
            }
            dummyWorld.mkpath(dst_Episode);

            FileFormats::WriteSMBX64LvlFileF(dst_Episode + "/templevel.lvl", in_levelData, 64);

            if(!isUntitled)
            {
                //Copy available custom stuff into temp directory
                QDir episodeDir(src_episodePath);
                QDir customDir(src_customPath);

                QStringList fileters;
                fileters << "*.txt" << "*.ini" << "*.lua" << "*.gif" << "*.png" << "*.bmp";
                QStringList fileters_cdir;
                fileters_cdir << "*.txt" << "*.ini" << "*.lua" << "*.gif" << "*.png" << "*.mp3"
                              //OGG Vorbis and FLAC (LibOGG, LibVorbis, LibFLAC)
                              << "*.ogg" << "*.flac"
                              //Uncompressed audio data
                              << "*.wav" << "*.voc" << "*.aiff"
                              //MIDI
                              << "*.mid" << "*.cmf"
                              //MikMod (Modules)
                              << "*.mod" << "*.it" << "*.s3m" << "*.669" << "*.med" << "*.xm" << "*.amf"
                              << "*.apun" << "*.dsm" << "*.far" << "*.gdm" << "*.imf" << "*.mtm"
                              << "*.okt" << "*.stm" << "*.stx" << "*.ult" << "*.uni"
                              //GAME EMU
                              << "*.ay" << "*.gbs" << "*.gym" << "*.hes" << "*.kss" << "*.nsf" << "*.nsfe" << "*.sap" << "*.spc" << "*.vgm" << "*.vgz"
                              //Rockythechao's extensions
                              << "*.anim";
                episodeDir.setNameFilters(fileters);
                customDir.setNameFilters(fileters_cdir);

                //***********************Attempt to make symbolic link*******************************/
                bool needToCopyEverything = true;
                typedef BOOL *(WINAPI * FUNK_OF_SYMLINKS)(TCHAR * linkFileName, TCHAR * existingFileName, DWORD flags);
                HMODULE hKernel32 = NULL;
                FUNK_OF_SYMLINKS fCreateSymbolicLink = NULL;
                hKernel32 = LoadLibraryW(L"KERNEL32.DLL");
                if(hKernel32)
                {
                    fCreateSymbolicLink = (FUNK_OF_SYMLINKS)(void*)GetProcAddress(hKernel32, "CreateSymbolicLinkW");
                    if(fCreateSymbolicLink) //Try to make a symblic link
                    {
                        QString newPath = dst_Episode + "templevel/";
                        if(fCreateSymbolicLink((TCHAR *)newPath.toStdWString().c_str(),
                                               (TCHAR *)src_customPath.toStdWString().c_str(), 0x1))
                            needToCopyEverything = false;
                    }
                }
                //************Copy images and scripts from episode folder**********************/
                QStringList files = episodeDir.entryList(QDir::Files);
                foreach(QString filex, files)
                    QFile::copy(src_episodePath + "/" + filex, dst_Episode + filex);

                //********************Copy images and scripts from custom folder*****************/
                //***************for case where impossible to make a symbolic link***************/
                if(needToCopyEverything)
                {
                    customDir.setSorting(QDir::NoSort);
                    QDirIterator dirsList(src_customPath, fileters_cdir,
                                          QDir::Files | QDir::NoSymLinks | QDir::NoDotAndDotDot,
                                          QDirIterator::Subdirectories);
                    while(dirsList.hasNext())
                    {
                        dirsList.next();
                        QString relativeDir = customDir.relativeFilePath(dirsList.fileInfo().absoluteDir().absolutePath());
                        QString filex = dirsList.fileName();//customDir.relativeFilePath(dirsList.filePath());
                        QString relNewPath = dst_Episode + "templevel/" + relativeDir;
                        QDir newRelDir(relNewPath);
                        if(!newRelDir.exists())
                            newRelDir.mkpath(relNewPath);
                        QFile::copy(src_customPath + "/" + relativeDir + "/" + filex, relNewPath + "/" + filex);
                    }
                }
                /*********************************************************************************/

                //Copy custom musics if possible
                foreach(LevelSection sec, in_levelData.sections)
                {
                    if(sec.music_file.isEmpty())
                        continue;

                    QString musFile = sec.music_file;
                    for(int i = 0; i < musFile.size(); i++)
                    {
                        if(musFile[i] == '|')
                            musFile.remove(i, musFile.size() - i);
                    }
                    QString MusicFileName = src_episodePath + "/" + musFile;
                    QString MusicNewPath = dst_Episode + musFile;

                    QFile mus(MusicFileName);
                    if(!mus.exists(MusicFileName))
                        continue;

                    QFileInfo inf(MusicNewPath);
                    if(!needToCopyEverything)
                    {
                        if(inf.absoluteDir().absolutePath().contains((dst_Episode + "templevel/"), Qt::CaseInsensitive))
                            continue; //Don't copy musics to the same directory!
                    }
                    if(!inf.absoluteDir().exists())
                        inf.absoluteDir().mkpath(inf.absoluteDir().absolutePath());
                    if(!needToCopyEverything)
                    {
                        if(fCreateSymbolicLink((TCHAR *)MusicNewPath.toStdWString().c_str(),
                                               (TCHAR *)MusicFileName.toStdWString().c_str(), 0x0) == 0)
                        {
                            mus.copy(MusicNewPath);//Copy file if impossible to make symbolic link to it
                        }
                    }
                    else
                        mus.copy(MusicNewPath);
                }
                if(hKernel32)
                    FreeLibrary(hKernel32);
            }

            WorldData worldmap;
            FileFormats::CreateWorldData(worldmap);
            worldmap.EpisodeTitle = "_temp_episode_pge";
            worldmap.IntroLevel_file = "templevel.lvl";
            worldmap.restartlevel = true;

            if(!FileFormats::WriteSMBX64WldFileF(dst_Episode + "/tempworld.wld", worldmap, 64))
            {
                msg.warning(LunaTester::tr("File save error"),
                            LunaTester::tr("Cannot save file %1:\n%2.")
                            .arg(dst_Episode + "/tempworld.wld")
                            .arg(worldmap.meta.ERROR_info),
                            QMessageBox::Ok);
                return;
            }

            QProcess::startDetached(smbxPath + ConfStatus::SmbxEXE_Name, params, smbxPath);
            //Stop music playback in the PGE Editor!
            QMetaObject::invokeMethod(m_mw, "setMusicButton", Qt::QueuedConnection, Q_ARG(bool, false));
            // not sure how efficient it is
            QMetaObject::invokeMethod(m_mw, "on_actionPlayMusic_triggered", Qt::QueuedConnection, Q_ARG(bool, false));
#else // _WIN32
            {
                int msgRet = msg.richBox(LunaTester::tr("Vanilla SMBX detected!"),
                                         LunaTester::tr("%2 not found!\nYou have a Vanilla SMBX!<br>\n"
                                                        "That means, impossible to launch level testing on your operating operating. "
                                                        "LunaLua is required to run level testing with SMBX on a non-Windows "
                                                        "operating systems."),
                                         QMessageBox::Close, QMessageBox::Warning);

                if(msgRet != QMessageBox::Yes)
                    return;
            }
#endif // _WIN32
            return;
        }
        else
        {
            /**********************************************
             **********Do LunaLUA testing launch!**********
             **********************************************/
            params << (m_killPreviousSession ? "--patch" : "--hideOnCloseIPC");

            if(m_noGL)
                params << "--nogl";

            QString argString;
            for(int i = 0; i < params.length(); i++)
            {
                if(i > 0)
                    argString += " ";
                argString += params[i];
            }

            bool engineStartedSuccess = true;
            QString engineStartupErrorString;
            useWine(command, params);
            emit engineStart(command, params, &engineStartedSuccess, &engineStartupErrorString);

            if(engineStartedSuccess)
            {
                if(sendLevelData(in_levelData, levelPath, isUntitled))
                {
                    //GlobalSettings::recentMusicPlayingState = ui->actionPlayMusic->isChecked();
                    //Stop music playback in the PGE Editor!
                    QMetaObject::invokeMethod(m_mw, "setMusicButton", Qt::QueuedConnection, Q_ARG(bool, false));
                    QMetaObject::invokeMethod(m_mw, "on_actionPlayMusic_triggered", Qt::QueuedConnection, Q_ARG(bool, false));
                }
                else
                {
                    msg.warning(LunaTester::tr("LunaTester error"),
                                LunaTester::tr("Failed to send level into LunaLUA-SMBX!"),
                                QMessageBox::Ok);
                }
            }
            else
            {
                QString luna_error = engineStartupErrorString.isEmpty() ? "Unknown error" : engineStartupErrorString;
                msg.warning(LunaTester::tr("LunaTester error"),
                            LunaTester::tr("Impossible to launch SMBX Engine, because %1").arg(luna_error),
                            QMessageBox::Ok);
            }
        }//Do LunaLUA direct testing launch
    }
}

void LunaTester::lunaRunGame()
{
    QString smbxPath = ConfStatus::configDataPath;
    QString command = smbxPath + "LunaLoader-exec.exe";

    if(!QFile(command).exists())
    {
        QMessageBox::warning(m_mw,
                             LunaTester::tr("Directory of Legacy Engine wasn't configured right"),
                             LunaTester::tr("%1 not found!\nTo use LunaTester you should have right "
                                            "Integration configuration package!")
                             .arg(command),
                             QMessageBox::Ok);
        return;
    }

    QStringList params;
    params << "--patch";
    params << "--game";

    if(m_noGL)
        params << "--nogl";

    if(!QFile(smbxPath + "LunaDll.dll").exists())
    {
        useWine(command, params);
        QProcess::startDetached(command, params, pathUnixToWine(smbxPath));
    }
    else
    {
        initRuntime();

        bool engineStartedSuccess = true;
        QString engineStartupErrorString;
        killEngine();// Kill previously running game
        useWine(command, params);
        emit engineStart(command, params, &engineStartedSuccess, &engineStartupErrorString);

        if(!engineStartedSuccess)
        {
            QString luna_error = engineStartupErrorString.isEmpty() ? "Unknown error" : engineStartupErrorString;
            QMessageBox::warning(m_mw,
                                 LunaTester::tr("LunaTester error"),
                                 LunaTester::tr("Impossible to launch Legacy Engine, because %1")
                                 .arg(luna_error),
                                 QMessageBox::Ok);
            return;
        }
    }

    //Stop music playback in the PGE Editor!
    QMetaObject::invokeMethod(m_mw,
                              "setMusicButton",
                              Qt::QueuedConnection,
                              Q_ARG(bool, false));

    // not sure how efficient it is
    QMetaObject::invokeMethod(m_mw,
                              "on_actionPlayMusic_triggered",
                              Qt::QueuedConnection,
                              Q_ARG(bool, false));
}
