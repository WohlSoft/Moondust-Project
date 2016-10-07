/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2014-2016 Vitaly Novichkov <admin@wohlnet.ru>
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

#define SDL_MAIN_HANDLED
#include <SDL2/SDL_messagebox.h>
#include <cstdlib>
#include <signal.h>

#include <graphics/window.h>

CrashHandler::CrashHandler()
{}

static void msgBox(QString title, QString text)
{
    std::string ttl = title.toUtf8().data();
    std::string msg = text.toUtf8().data();
    SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR,
                             ttl.c_str(), msg.c_str(), PGE_Window::window);
}


void CrashHandler::crashByUnhandledException()
{
    LogWarning("<Unhandled exception!>");
    msgBox(
          //% "Unhandled exception!"
          qtTrId("CRASH_UNHEXC_TITLE"),
          //% "Engine has crashed because accepted unhandled exception!"
          qtTrId("CRASH_UNHEXC_MSG"));
    exit(-1);
}

void CrashHandler::crashByFlood()
{
    LogWarning("<Out of memory!>");
    msgBox(
          //% "Out of memory!"
          qtTrId("CRASH_OUT_OF_MEM_TITLE"),
          //% "Engine has crashed because out of memory! Try to close other applications and restart game."
          qtTrId("CRASH_OUT_OF_MEM_MSG"));
    exit(-1);
}


void CrashHandler::handle_signal(int signal)
{
    // Find out which signal we're handling
    switch (signal) {
        #ifndef _WIN32  //Unsupported signals by Windows
        case SIGHUP:
            LogWarning("Terminal was closed");
            exit(signal);
            break;
        case SIGQUIT:
            LogWarning("<Quit command>");
            exit(signal);
        case SIGKILL:
            LogWarning("<killed>");
            msgBox(
                    //% "Killed!"
                    qtTrId("CRASH_KILLED_TITLE"),
                    //% "Engine has been killed by a mad maniac :-P"
                    qtTrId("CRASH_KILLED_MSG"));
            exit(signal);
            break;
        case SIGALRM:
            LogWarning("<alarm() time out!>");
            msgBox(
                  //% "Time out!"
                  qtTrId("CRASH_TIMEOUT_TITLE"),
                  //% "Engine has abourted because alarm() time out!"
                  qtTrId("CRASH_TIMEOUT_MSG"));
            exit(signal);
            break;
        case SIGURG:
        case SIGUSR1:
        case SIGUSR2:
            break;
        case SIGILL:
            LogWarning("<Wrong CPU Instruction>");
            msgBox(
                  //% "Wrong CPU Instruction!"
                  qtTrId("CRASH_ILL_TITLE"),
                  //% "Engine has crashed because a wrong CPU instruction"
                  qtTrId("CRASH_ILL_MSG"));
            exit(signal);
        #endif
        case SIGFPE:
            LogWarning("<wrong arithmetical operation>");
            msgBox(
                  //% "Wrong arithmetical operation"
                  qtTrId("CRASH_FPE_TITLE"),
                  //% "Engine has crashed because of a wrong arithmetical operation!"
                  qtTrId("CRASH_FPE_MSG"));
            exit(signal);
            break;
        case SIGABRT:
            LogWarning("<Aborted!>");
            msgBox(
                  //% "Aborted"
                  qtTrId("CRASH_ABORT_TITLE"),
                  //% "Engine has been aborted because critical error was occouped."
                  qtTrId("CRASH_ABORT_TITLE."));
            exit(signal);
        case SIGSEGV:
            LogWarning("<Segmentation fault crash!>");
            msgBox(
                  //% "Segmentation fault"
                  qtTrId("CRASH_SIGSEGV_TITLE"),

                  /*% "Engine has crashed because of a Segmentation fault.\n"
                      "Run debugging with a built in debug mode application\n"
                      "and retry your recent actions to get more detailed information." */
                  qtTrId("CRASH_SIGSEGV_MSG."));
            exit(signal);
            break;
        case SIGINT:
            LogWarning("<Interrupted!>");
            msgBox(
                  //% "Interrupt"
                  qtTrId("CRASH_INT_TITLE"),
                  //% "Engine has been interrupted"
                  qtTrId("CRASH_INT_MSG"));
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
