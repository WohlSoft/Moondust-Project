/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2014-2021 Vitaly Novichkov <admin@wohlnet.ru>
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
#ifndef CRASHHANDLER_H
#define CRASHHANDLER_H

#include <QDialog>

namespace Ui
{
    class CrashHandler;
}

class CrashHandler : public QDialog
{
    Q_OBJECT

public:
    explicit CrashHandler(const QString &crashText, QWidget *parent = nullptr);
    ~CrashHandler();

    //Crash Handlers
    static void crashByFlood();
    static void crashByUnhandledException();
    static void crashBySIGNAL(int signalid);

    static void doCrashScreenAndCleanup(QString crashMsg);

    static void attemptCrashsave();
    static void checkCrashsaves();
    //Crash Handlers end

    static void initCrashHandlers();

    static QString getStacktrace();

private slots:
    void on_copyReport_clicked();
    void on_pgeForumButton_clicked();
    void on_pgeRepoButton_clicked();
    void on_exitButton_clicked();

private:
    Ui::CrashHandler *ui = nullptr;
};

#endif // CRASHHANDLER_H
