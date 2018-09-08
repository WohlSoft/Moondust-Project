/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2014-2018 Vitaly Novichkov <admin@wohlnet.ru>
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

#include <QDesktopServices>
#include <QMessageBox>
#ifdef _WIN32
#include <windows.h>
#include <dbghelp.h>
#define USE_STACK_WALKER
#ifdef USE_STACK_WALKER
#include <StackWalker/StackWalker.h>
#endif
#elif (defined(__linux__) && !defined(__ANDROID__)) || defined(__APPLE__)
#include <execinfo.h>
#include <unistd.h>
#endif
#include <signal.h>

#include <dev_console/devconsole.h>
#include <version.h>

#include "crashhandler.h"
#include <ui_crashhandler.h>

#include "common_features/main_window_ptr.h"
#include <common_features/app_path.h>
#include <common_features/logger.h>
#include <common_features/logger_sets.h>

static const char *g_messageToUser =
    "\n"
    "================================================\n"
    "            Additional information:\n"
    "================================================\n"
    V_FILE_DESC " version: " V_FILE_VERSION V_FILE_RELEASE "\n"
    "Architecture: " FILE_CPU "\n"
    "GIT Revision code: " GIT_VERSION "\n"
    "Build date: " V_DATE_OF_BUILD "\n"
    "================================================\n"
    " Please send this log file to the developers by one of ways:\n"
    " - Via contact form:          http://wohlsoft.ru/forum/memberlist.php?mode=contactadmin\n"
    " - Official forums:           http://wohlsoft.ru/forum/\n"
    " - Make issue at GitHub repo: https://github.com/WohlSoft/PGE-Project\n\n"
    "================================================\n";

#ifdef _WIN32
static bool GetStackWalk(std::string &outWalk)
{
    #ifdef USE_STACK_WALKER
    StackWalkerToString x(outWalk);
    x.ShowCallstack();
    #else
    //
    // http://blog.aaronballman.com/2011/04/generating-a-stack-crawl/
    //

    // Set up the symbol options so that we can gather information from the current
    // executable's PDB files, as well as the Microsoft symbol servers.  We also want
    // to undecorate the symbol names we're returned.  If you want, you can add other
    // symbol servers or paths via a semi-colon separated list in SymInitialized.
    ::SymSetOptions(SYMOPT_DEFERRED_LOADS | SYMOPT_INCLUDE_32BIT_MODULES | SYMOPT_UNDNAME);

    if(!::SymInitialize(::GetCurrentProcess(), "http://msdl.microsoft.com/download/symbols", TRUE)) return false;

    // Capture up to 25 stack frames from the current call stack.  We're going to
    // skip the first stack frame returned because that's the GetStackWalk function
    // itself, which we don't care about.
    PVOID addrs[ 400 ] = { 0 };
    USHORT frames = CaptureStackBackTrace(1, 400, addrs, nullptr);

    for(USHORT i = 0; i < frames; i++)
    {
        // Allocate a buffer large enough to hold the symbol information on the stack and get
        // a pointer to the buffer.  We also have to set the size of the symbol structure itself
        // and the number of bytes reserved for the name.
        ULONG64 buffer[(sizeof(SYMBOL_INFO) + 1024 + sizeof(ULONG64) - 1) / sizeof(ULONG64) ] = { 0 };
        SYMBOL_INFO *info = (SYMBOL_INFO *)buffer;
        info->SizeOfStruct = sizeof(SYMBOL_INFO);
        info->MaxNameLen = 1024;
        // Attempt to get information about the symbol and add it to our output parameter.
        DWORD64 displacement = 0;

        if(::SymFromAddr(::GetCurrentProcess(), (DWORD64)addrs[ i ], &displacement, info))
        {
            outWalk.append(info->Name, info->NameLen);
            outWalk.append("\n");
        }
    }
    ::SymCleanup(::GetCurrentProcess());
    #endif
    return true;
}
#endif

QString CrashHandler::getStacktrace()
{
    #ifdef _WIN32
    //StackTracer tracer;
    //tracer.runStackTracerForAllThreads();
    //return tracer.theOutput();
    //dbg::stack s;
    //std::stringstream out;
    //std::copy(s.begin(), s.end(), std::ostream_iterator<dbg::stack_frame>(out, "\n"));
    std::string stack;
    GetStackWalk(stack);
    return QString::fromStdString(stack);
    #elif (defined(__linux__) && !defined(__ANDROID__)) || defined(__APPLE__)
    void *array[401];
    int size;
    char **strings;
    size = backtrace(array, 400);
    strings = backtrace_symbols(array, size);
    QString bkTrace("");

    for(int j = 0; j < size; j++)
        bkTrace += QString(strings[j]) + "\n";

    return bkTrace;
    #else
    return QString("");
    #endif
}

CrashHandler::CrashHandler(QString &crashText, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CrashHandler)
{
    ui->setupUi(this);
    ui->crashData->setText(crashText);
}


CrashHandler::~CrashHandler()
{
    delete ui;
}

void CrashHandler::crashByFlood()
{
    doCrashScreenAndCleanup(QObject::tr("We're sorry, but PGE Editor has crashed. \nReason: Out of memory!\n\n"
                                        "To prevent this, try closing other uneccessary programs to free up more memory."));
}

void CrashHandler::crashByUnhandledException()
{
    doCrashScreenAndCleanup(QObject::tr("We're sorry, but PGE Editor has crashed. \nReason: Unhandled Exception\n\nPlease inform our forum staff so we can try to fix this problem, Thank you\n\nForum link: wohlsoft.ru/forum"));
}

void CrashHandler::crashBySIGNAL(int signalid)
{
    QString sigtype = "Signal Segmentation Violation [SIGSEGV]";

    switch(signalid)
    {
        #ifndef _WIN32  //Unsupported signals by Windows

    case SIGHUP:
        sigtype = QObject::tr("Terminal was closed [SIGHUP]");
        break;

    case SIGQUIT:
        sigtype = QObject::tr("Quit command [SIGQUIT]");
        break;

    case SIGALRM:
        sigtype = QObject::tr("Editor was abourted because alarm() time out! [SIGALRM]");
        break;

    case SIGBUS:
        sigtype = QObject::tr("Editor was abourted because physical memory error! [SIGBUS]");
        break;

    case SIGURG:
    case SIGUSR1:
    case SIGUSR2:
        return;
        #endif

    case SIGILL:
        sigtype = QObject::tr("Wrong CPU Instruction [SIGILL]");
        break;

    case SIGFPE:
        sigtype = QObject::tr("Floating-point exception [SIGFPE]");
        break;

    case SIGABRT:
        sigtype = QObject::tr("Aborted! [SIGABRT]");
        break;

    case SIGSEGV:
        sigtype = QObject::tr("Signal Segmentation Violation [SIGSEGV]");
        break;

    case SIGINT:
        sigtype = QObject::tr("Interrupted! [SIGINT]");
        break;

    default:
        break;
    }

    doCrashScreenAndCleanup(QObject::tr("We're sorry, but PGE Editor has crashed. \nReason: %1\n\n").arg(sigtype));
    std::exit(signalid);
}

void CrashHandler::doCrashScreenAndCleanup(QString crashMsg)
{
    //Force debug log enabling
    LogWriter::logLevel = PGE_LogLevel::Debug;
    //Append extra explanation to user how to report the crash
    crashMsg += g_messageToUser;
    //Write crash message into the log file first
    crashMsg += QString("\n\n") + getStacktrace();
    //Also save crash report into log file
    LogFatalNC(crashMsg);
    //Then, emergency save all opened files into reserve folder
    attemptCrashsave();

    //Close console box if possible
    if(DevConsole::isConsoleShown())
        DevConsole::closeIfPossible();

    MainWinConnect::pMainWin->hide();
    //And now, spawn dialog box with stack trance
    CrashHandler *crsh = new CrashHandler(crashMsg);
    crsh->exec();
    delete crsh;
}

void CrashHandler::attemptCrashsave()
{
    QDir crashSave;
    crashSave.setCurrent(AppPathManager::userAppDir());

    /*
     * For case if crash happened while ... attempt to load crashed files!
     * This avoids looping crashes because backup files are causing crash too!
     */
    if(crashSave.exists("__crashsave"))
    {
        LogCriticalNC("We are detected that crash has been occouped on attempt to load or initialize backup files. Backup directory has been renamed into \"__crashsave_danger\".");
        LogCriticalNC("Please attach all files in that directory and, if possible, additional contents (custom images, sounds, musics, configs and scripts) while reporting this crash.");
        QDir    dupeDir(AppPathManager::userAppDir() + "/__crashsave_danger");

        if(dupeDir.exists())
            dupeDir.removeRecursively();

        crashSave.rename("__crashsave", "__crashsave_danger");
        return;
    }

    crashSave.mkdir("__crashsave");
    crashSave.cd("__crashsave");
    int untitledCounter = 0;
    MainWindow *mw = MainWinConnect::pMainWin;

    if(!mw) return;

    QList<QMdiSubWindow *> listOfAllSubWindows = mw->allEditWins();

    for(QMdiSubWindow *subWin : listOfAllSubWindows)
    {
        if(mw->activeChildWindow(subWin) == 1)
        {
            LevelEdit *lvledit = mw->activeLvlEditWin(subWin);

            if(!lvledit)
                continue;

            QString fName = lvledit->currentFile();

            if(lvledit->isUtitled())
                fName = QString("Untitled_Crash") + QString::number(untitledCounter++) + QString(".lvlx");
            else
                fName = fName.section("/", -1) + QString(".lvlx");

            LevelData &lvl = lvledit->LvlData;
            lvl.metaData.crash.used = true;
            lvl.metaData.crash.untitled = lvl.meta.untitled;
            lvl.metaData.crash.modifyed = lvl.meta.modified;
            lvl.metaData.crash.strictModeSMBX64 = lvl.meta.smbx64strict;
            lvl.metaData.crash.fmtID    = lvl.meta.RecentFormat;
            lvl.metaData.crash.fmtVer   = lvl.meta.RecentFormatVersion;
            lvl.metaData.crash.filename = lvl.meta.filename;
            lvl.metaData.crash.path     = lvl.meta.path;
            lvl.metaData.crash.fullPath = lvledit->curFile;
            //Forcely save data into PGE-X format
            lvl.meta.RecentFormat = LevelData::PGEX;
            lvledit->saveFile(crashSave.absoluteFilePath(fName), false);
        }
        else if(mw->activeChildWindow(subWin) == 2)
        {
            NpcEdit *npcedit = mw->activeNpcEditWin(subWin);

            if(!npcedit)
                continue;

            QString fName = npcedit->currentFile();

            if(npcedit->isUtitled())
                fName = QString("Untitled_Crash") + QString::number(untitledCounter++) + QString(".txt");
            else
                fName = fName.section("/", -1);

            npcedit->saveFile(crashSave.absoluteFilePath(fName), false);
        }
        else if(mw->activeChildWindow(subWin) == 3)
        {
            WorldEdit *worldedit = mw->activeWldEditWin(subWin);

            if(!worldedit)
                continue;

            QString fName = worldedit->currentFile();

            if(worldedit->isUtitled())
                fName = QString("Untitled_Crash") + QString::number(untitledCounter++) + QString(".wldx");
            else
                fName = fName.section("/", -1) + QString(".wldx");

            WorldData &wld = worldedit->WldData;
            wld.metaData.crash.used = true;
            wld.metaData.crash.untitled = wld.meta.untitled;
            wld.metaData.crash.modifyed = wld.meta.modified;
            wld.metaData.crash.strictModeSMBX64 = wld.meta.smbx64strict;
            wld.metaData.crash.fmtID    = wld.meta.RecentFormat;
            wld.metaData.crash.fmtVer   = wld.meta.RecentFormatVersion;
            wld.metaData.crash.filename = wld.meta.filename;
            wld.metaData.crash.path     = wld.meta.path;
            wld.metaData.crash.fullPath = worldedit->curFile;
            //Forcely save data into PGE-X format
            wld.meta.RecentFormat = WorldData::PGEX;
            worldedit->saveFile(crashSave.absoluteFilePath(fName), false);
        }
    }
}

void CrashHandler::checkCrashsaves()
{
    MainWindow *mw = MainWinConnect::pMainWin;
    QDir crashSave;
    crashSave.setCurrent(AppPathManager::userAppDir());

    if(crashSave.exists("__crashsave"))
    {
        crashSave.cd("__crashsave");
        QStringList allCrashFiles = crashSave.entryList(QDir::Files | QDir::NoDotAndDotDot);

        for(QString &file : allCrashFiles)
        {
            QString fPath = crashSave.absoluteFilePath(file);
            mw->OpenFile(fPath, false);
        }

        QList<QMdiSubWindow *> listOfAllSubWindows = mw->allEditWins();

        for(QMdiSubWindow *subWin : listOfAllSubWindows)
        {
            /*if(MainWinConnect::pMainWin->activeChildWindow(subWin) == 1){
                MainWinConnect::pMainWin->activeLvlEditWin()->makeCrashState();
            }else */
            if(mw->activeChildWindow(subWin) == 2)
                mw->activeNpcEditWin()->makeCrashState();

            /*else if(MainWinConnect::pMainWin->activeChildWindow(subWin) == 3){
                MainWinConnect::pMainWin->activeWldEditWin()->makeCrashState();
            }*/
        }

        //Clean up all files from crash-save folder after restoring
        crashSave.removeRecursively();
        QMessageBox::information(mw,
                                 tr("Crash recovery", "Crash recovery - emergency file saving after crash. A title of message box."),
                                 tr("Since the last crash, the editor recovered some files.\n"
                                    "Please save them before doing anything else."),
                                 QMessageBox::Ok);
    }
}

void CrashHandler::initCrashHandlers()
{
    #if !defined(DEBUG_BUILD) && !defined(__APPLE__)
    std::set_terminate(&crashByUnhandledException);
    #endif
    #ifndef DEBUG_BUILD
    std::set_new_handler(&crashByFlood);
    #ifndef _WIN32 //Unsupported signals by Windows
    signal(SIGHUP,  &crashBySIGNAL);
    signal(SIGQUIT, &crashBySIGNAL);
    signal(SIGALRM, &crashBySIGNAL);
    signal(SIGBUS,  &crashBySIGNAL);
    signal(SIGURG,  &crashBySIGNAL);
    signal(SIGUSR1, &crashBySIGNAL);
    signal(SIGUSR2, &crashBySIGNAL);
    #endif
    signal(SIGILL,  &crashBySIGNAL);
    signal(SIGFPE,  &crashBySIGNAL);
    signal(SIGSEGV, &crashBySIGNAL);
    signal(SIGINT,  &crashBySIGNAL);
    signal(SIGABRT, &crashBySIGNAL);
    #endif
}

void CrashHandler::on_pgeForumButton_clicked()
{
    QDesktopServices::openUrl(QUrl("http://wohlsoft.ru/forum/"));
}

void CrashHandler::on_exitButton_clicked()
{
    this->close();
}
