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

#include "crash_handler.h"

#include "logger.h"

#include <QMessageBox>

#include <cstdlib>
#include <signal.h>

CrashHandler::CrashHandler()
{}

void CrashHandler::crashByUnhandledException()
{
    WriteToLog(QtWarningMsg, "<Unhandled exception!>");
    QMessageBox::critical(NULL, QObject::tr("Unhandled exception!"),
                 QObject::tr("Engine was crashed because accepted unhandled exception!"));
    exit(-1);
}

void CrashHandler::crashByFlood()
{
    WriteToLog(QtWarningMsg, "<Out of memory!>");
    QMessageBox::critical(NULL, QObject::tr("Out of memory!"),
                 QObject::tr("Engine was crashed because out of memory! Try to close other applications and restart game."));
    exit(-1);
}


void CrashHandler::handle_signal(int signal)
{
    // Find out which signal we're handling
    switch (signal) {
        #ifndef _WIN32  //Unsupported signals by Windows
        case SIGHUP:
            WriteToLog(QtWarningMsg, "Terminal was closed");
            exit(signal);
            break;
        case SIGQUIT:
            WriteToLog(QtWarningMsg, "<Quit command>");
            exit(signal);
        case SIGKILL:
            WriteToLog(QtWarningMsg, "<killed>");
            QMessageBox::critical(NULL, QObject::tr("Killed!"),
                         QObject::tr("Engine was killed by mad maniac :-P"));
            exit(signal);
            break;
        case SIGALRM:
            WriteToLog(QtWarningMsg, "<alarm() time out!>");
            QMessageBox::critical(NULL, QObject::tr("Time out!"),
                          QObject::tr("Engine was abourted because alarm() time out!"));
            exit(signal);
            break;
        case SIGURG:
        case SIGUSR1:
        case SIGUSR2:
            break;
        case SIGILL:
            WriteToLog(QtWarningMsg, "<Wrong CPU Instruction>");
            QMessageBox::critical(NULL, QObject::tr("Wrong CPU Instruction!"),
                         QObject::tr("Engine was crashed because a wrong CPU instruction"));
            exit(signal);
        #endif
        case SIGFPE:
            WriteToLog(QtWarningMsg, "<wrong arithmetical operation>");
            QMessageBox::critical(NULL, QObject::tr("Wrong arithmetical operation"),
                          QObject::tr("Engine was crashed because wrong arithmetical opreation!"));
            exit(signal);
            break;
        case SIGABRT:
            WriteToLog(QtWarningMsg, "<Aborted!>");
            QMessageBox::critical(NULL, QObject::tr("Aborted"),
                          QObject::tr("Engine was been aborted because critical error was occouped."));
            exit(signal);
        case SIGSEGV:
            WriteToLog(QtWarningMsg, "<Segmentation fault crash!>");
            QMessageBox::critical(NULL, QObject::tr("Segmentation fault"),
                          QObject::tr("Engine was crashed because Segmentation fault. Run debug with built in debug mode "
                                      "and retry your recent action to take more detail info."));
            exit(signal);
            break;
        case SIGINT:
            WriteToLog(QtWarningMsg, "<Interrupted!>");
            QMessageBox::critical(NULL, QObject::tr("Interrupt"),
                          QObject::tr("Engine was interrupted"));
            exit(0);
            break;
        default:
            return;
    }
}

void CrashHandler::initSigs()
{
    std::set_new_handler(&crashByFlood);
    std::set_terminate(&crashByUnhandledException);
    #ifndef _WIN32 //Unsupported signals by Windows
    signal(SIGHUP, &handle_signal);
    signal(SIGQUIT, &handle_signal);
    signal(SIGKILL, &handle_signal);
    signal(SIGALRM, &handle_signal);
    signal(SIGURG, &handle_signal);
    signal(SIGUSR1, &handle_signal);
    signal(SIGUSR2, &handle_signal);
    #endif
    signal(SIGILL, &handle_signal);
    signal(SIGFPE, &handle_signal);
    signal(SIGSEGV, &handle_signal);
    signal(SIGINT, &handle_signal);
    signal(SIGABRT, &handle_signal);
}
/* Signals End */
