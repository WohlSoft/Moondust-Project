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

#ifndef CRASHHANDLER_H
#define CRASHHANDLER_H

#include <QDialog>

namespace Ui {
class CrashHandler;
}

class CrashHandler : public QDialog
{
    Q_OBJECT

public:
    explicit CrashHandler(QString& crashText, QWidget *parent = 0);
    ~CrashHandler();

    //Crash Handlers
    static void crashByFlood();
    static void crashByUnhandledException();
    static void crashBySIGSERV(int signalid);

    static void doCrashScreenAndCleanup(QString crashMsg);

    static void attemptCrashsave();
    static void checkCrashsaves();

    //Crash Handlers end

    static void initCrashHandlers();


private slots:
    void on_pgeForumButton_clicked();
    void on_exitButton_clicked();

private:
    static QString getStacktrace();
    Ui::CrashHandler *ui;
};

#ifdef _WIN32
#include <stackwalker/StackWalker.h>
#include <tlhelp32.h>

class StackTracer : public StackWalker
{
public:
    StackTracer() : StackWalker(), theOutput(""){}

    void runStackTracerForAllThreads(){
        HANDLE h = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
        if (h != INVALID_HANDLE_VALUE) {
            THREADENTRY32 te;
            te.dwSize = sizeof(te);
            if (Thread32First(h, &te)) {
                do {
                    if (te.dwSize >= FIELD_OFFSET(THREADENTRY32, th32OwnerProcessID) +
                                sizeof(te.th32OwnerProcessID)) {
                        if(GetCurrentProcessId() == te.th32OwnerProcessID){
                            theOutput += QString("\n\nThread #%1:\n").arg(te.th32ThreadID);
                            HANDLE theThread = OpenThread(THREAD_ALL_ACCESS, false, te.th32ThreadID);
                            ShowCallstack(theThread);
                            CloseHandle(theThread);
                        }
                    }
                    te.dwSize = sizeof(te);
                } while (Thread32Next(h, &te));
            }
            CloseHandle(h);
        }else{
            theOutput = "Failed to create stacktrace!";
        }
    }

    QString theOutput;

protected:
    virtual void OnOutput(LPCSTR szText)
    {
        theOutput += szText;
    }
};

#endif

#endif // CRASHHANDLER_H
