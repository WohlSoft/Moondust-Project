/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2014-2015 Vitaly Novichkov <admin@wohlnet.ru>
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
#elif ((__linux__) && !(__ANDROID__) || __APPLE__)
    #include <execinfo.h>
    #include <unistd.h>
#endif
#include <signal.h>

#include <dev_console/devconsole.h>

#include "crashhandler.h"
#include <ui_crashhandler.h>

#include "common_features/mainwinconnect.h"
#include <common_features/app_path.h>
#include <common_features/logger.h>

QString CrashHandler::getStacktrace()
{
    #ifdef _WIN32
        StackTracer tracer;
        tracer.runStackTracerForAllThreads();
        return tracer.theOutput;
    #elif ((__linux__) && !(__ANDROID__) || __APPLE__)
        void *array[400];
        size_t size;

        char **strings;
        size = backtrace(array, 400);

        strings = backtrace_symbols(array, size);

        QString bkTrace("");
        for(unsigned int j = 0; j < (unsigned)size; j++)
             bkTrace += QString(strings[j])+"\n";
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
    doCrashScreenAndCleanup(QObject::tr("We're sorry, but PGE Editor has crashed. \nReason: Unhandled Exception\n\nPlease inform our forum staff so we can try to fix this problem, Thank you\n\nForum link: engine.wohlnet.ru/forum"));
}

void CrashHandler::crashBySIGNAL(int signalid)
{
    QString sigtype="Signal Segmentation Violation [SIGSEGV]";
    switch(signalid)
    {
        #ifndef _WIN32  //Unsupported signals by Windows
        case SIGHUP:   sigtype = QObject::tr("Terminal was closed [SIGHUP]"); break;
        case SIGQUIT:  sigtype = QObject::tr("Quit command [SIGQUIT]"); break;
        case SIGKILL:  sigtype = QObject::tr("Editor was killed by mad maniac :-P [SIGKILL]"); break;
        case SIGALRM:  sigtype = QObject::tr("Editor was abourted because alarm() time out! [SIGALRM]"); break;
        case SIGURG:
        case SIGUSR1:
        case SIGUSR2: return;
        case SIGILL:   sigtype = QObject::tr("Terminal was closed [SIGHUP]"); break;
        #endif
        case SIGFPE:  sigtype = QObject::tr("Wrong CPU Instruction [SIGFPE]"); break;
        case SIGABRT: sigtype = QObject::tr("Aborted! [SIGABRT]"); break;
        case SIGSEGV: sigtype = QObject::tr("Signal Segmentation Violation [SIGSEGV]"); break;
        case SIGINT:  sigtype = QObject::tr("Interrupted! [SIGINT]"); break;
        default: break;
    }
    doCrashScreenAndCleanup(QObject::tr("We're sorry, but PGE Editor has crashed. \nReason: %1\n\n").arg(sigtype));
}

void CrashHandler::doCrashScreenAndCleanup(QString crashMsg)
{
    //Write crash message into the log file first
    crashMsg += QString("\n\n") + getStacktrace();
    //Also save crash report into log file
    WriteToLog(QtFatalMsg, crashMsg, true);

    //Then, emergency save all opened files into reserve folder
    attemptCrashsave();

    //Close console box if possible
    if(DevConsole::isConsoleShown())
        DevConsole::closeIfPossible();

    MainWinConnect::pMainWin->hide();

    //And now, spawn dialog box with stack trance
    CrashHandler* crsh = new CrashHandler(crashMsg);
    crsh->exec();
    delete crsh;

    exit(EXIT_FAILURE);
}

void CrashHandler::attemptCrashsave()
{
    QDir crashSave;
    crashSave.setCurrent(AppPathManager::userAppDir());
    crashSave.mkdir("__crashsave");
    crashSave.cd("__crashsave");

    int untitledCounter = 0;


    QList<QMdiSubWindow*> listOfAllSubWindows = MainWinConnect::pMainWin->allEditWins();
    foreach (QMdiSubWindow* subWin, listOfAllSubWindows) {
        if(MainWinConnect::pMainWin->activeChildWindow(subWin) == 1){
            LevelEdit* lvledit = MainWinConnect::pMainWin->activeLvlEditWin(subWin);

            QString fName = lvledit->currentFile();
            if(lvledit->isUntitled){
                fName = QString("Untitled_Crash") + QString::number(untitledCounter++) + QString(".lvlx");
            }else{
                fName = fName.section("/", -1)+QString(".lvlx");
            }

            lvledit->LvlData.metaData.crash.used=true;
            lvledit->LvlData.metaData.crash.untitled = lvledit->LvlData.untitled;
            lvledit->LvlData.metaData.crash.modifyed = lvledit->LvlData.modified;
            lvledit->LvlData.metaData.crash.filename = lvledit->LvlData.filename;
            lvledit->LvlData.metaData.crash.path     = lvledit->LvlData.path;
            lvledit->LvlData.metaData.crash.fullPath = lvledit->curFile;

            lvledit->saveFile(crashSave.absoluteFilePath(fName), false);
        }else if(MainWinConnect::pMainWin->activeChildWindow(subWin) == 2){
            NpcEdit* npcedit = MainWinConnect::pMainWin->activeNpcEditWin();

            QString fName = npcedit->currentFile();
            if(npcedit->isUntitled){
                fName = QString("Untitled_Crash") + QString::number(untitledCounter++) + QString(".txt");
            }else{
                fName = fName = fName.section("/", -1);
            }

            npcedit->saveFile(crashSave.absoluteFilePath(fName), false);
        }else if(MainWinConnect::pMainWin->activeChildWindow(subWin) == 3){
            WorldEdit* worldedit = MainWinConnect::pMainWin->activeWldEditWin();

            QString fName = worldedit->currentFile();
            if(worldedit->isUntitled){
                fName = QString("Untitled_Crash") + QString::number(untitledCounter++) + QString(".wldx");
            }else{
                fName = fName = fName.section("/", -1)+QString(".wldx");
            }

            worldedit->WldData.metaData.crash.used=true;
            worldedit->WldData.metaData.crash.untitled = worldedit->WldData.untitled;
            worldedit->WldData.metaData.crash.modifyed = worldedit->WldData.modified;
            worldedit->WldData.metaData.crash.filename = worldedit->WldData.filename;
            worldedit->WldData.metaData.crash.path = worldedit->WldData.path;
            worldedit->WldData.metaData.crash.fullPath = worldedit->curFile;

            worldedit->saveFile(crashSave.absoluteFilePath(fName), false);
        }
    }
}

void CrashHandler::checkCrashsaves()
{
    QDir crashSave;
    crashSave.setCurrent(AppPathManager::userAppDir());
    if(crashSave.exists("__crashsave")){
        crashSave.cd("__crashsave");
        QStringList allCrashFiles = crashSave.entryList(QDir::Files | QDir::NoDotAndDotDot);
        foreach(QString file, allCrashFiles){
            QString fPath = crashSave.absoluteFilePath(file);
            MainWinConnect::pMainWin->OpenFile(fPath, false);
        }
        QList<QMdiSubWindow*> listOfAllSubWindows = MainWinConnect::pMainWin->allEditWins();
        foreach (QMdiSubWindow* subWin, listOfAllSubWindows) {
            if(MainWinConnect::pMainWin->activeChildWindow(subWin) == 1){
                MainWinConnect::pMainWin->activeLvlEditWin()->makeCrashState();
            }else if(MainWinConnect::pMainWin->activeChildWindow(subWin) == 2){
                MainWinConnect::pMainWin->activeNpcEditWin()->makeCrashState();
            }else if(MainWinConnect::pMainWin->activeChildWindow(subWin) == 3){
                MainWinConnect::pMainWin->activeWldEditWin()->makeCrashState();
            }
        }


        crashSave.removeRecursively();
        QMessageBox::information(MainWinConnect::pMainWin, tr("Crashsave"), tr("Since the last crash, the editor recorved some files.\nPlease save them first before doing anything else."), QMessageBox::Ok, QMessageBox::Ok);
    }
}

void CrashHandler::initCrashHandlers()
{
    std::set_new_handler(&crashByFlood);
    std::set_terminate(&crashByUnhandledException);
    #ifndef _WIN32 //Unsupported signals by Windows
    signal(SIGHUP, &crashBySIGNAL);
    signal(SIGQUIT, &crashBySIGNAL);
    signal(SIGKILL, &crashBySIGNAL);
    signal(SIGALRM, &crashBySIGNAL);
    signal(SIGURG, &crashBySIGNAL);
    signal(SIGUSR1, &crashBySIGNAL);
    signal(SIGUSR2, &crashBySIGNAL);
    #endif
    signal(SIGILL, &crashBySIGNAL);
    signal(SIGFPE, &crashBySIGNAL);
    signal(SIGSEGV, &crashBySIGNAL);
    signal(SIGINT, &crashBySIGNAL);
    signal(SIGABRT, &crashBySIGNAL);
}

void CrashHandler::on_pgeForumButton_clicked()
{
    QDesktopServices::openUrl(QUrl("http://engine.wohlnet.ru/forum/"));
}

void CrashHandler::on_exitButton_clicked()
{
    this->close();
}
