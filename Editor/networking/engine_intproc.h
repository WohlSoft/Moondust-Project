/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2014-2020 Vitaly Novichkov <admin@wohlnet.ru>
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
#ifndef ENGINE_INTPROCINTPROC_H
#define ENGINE_INTPROCINTPROC_H

#include <QObject>
#include <QProcess>

struct LevelData;

class IntEngine: public QObject
{
    Q_OBJECT
public:
    IntEngine();
    ~IntEngine() override = default;
    static void init(QProcess *engine_proc);

    static void quit();
    static bool isWorking();

    static bool sendCheat(QString _args);
    static bool sendMessageBox(QString _args);
    static bool sendItemPlacing(QString _args);
    static void sendLevelBuffer();

    static void setTestLvlBuffer(LevelData &buffer);

    static bool sendMessage(const char *msg);
    static bool sendMessage(QString &msg);

    static LevelData testBuffer;
signals:
    void engineInputMsg(QString msg);

private slots:
    void onData();

private:
    static QProcess *engine;
};

#endif // ENGINE_INTPROCINTPROC_H
