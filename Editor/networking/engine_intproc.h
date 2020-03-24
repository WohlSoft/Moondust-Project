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
#include <PGE_File_Formats/lvl_filedata.h>

struct LevelData;

class IntEngineSignals : public QObject
{
    Q_OBJECT
public:
    explicit IntEngineSignals(QObject *parent = nullptr);
    ~IntEngineSignals();

signals:
    bool sendCheat(QString _args);
    bool sendMessageBox(QString _args);
    void sendPlacingBlock(const LevelBlock &block);
    void sendPlacingBGO(const LevelBGO &bgo);
    void sendPlacingNPC(const LevelNPC &npc);
};

extern IntEngineSignals g_intEngine;


class IntEngine: public QObject
{
    Q_OBJECT
public:
    IntEngine();
    ~IntEngine() override = default;

    void init(QProcess *engine);
    void quit();

    bool isWorking();

    bool sendCheat(QString _args);
    bool sendMessageBox(QString _args);

    void sendPlacingBlock(const LevelBlock &block);
    void sendPlacingBGO(const LevelBGO &bgo);
    void sendPlacingNPC(const LevelNPC &npc);

    void sendLevelBuffer();

    void setTestLvlBuffer(LevelData &buffer);

    bool sendMessage(const char *msg);
    bool sendMessage(QString &msg);

    LevelData testBuffer;

signals:
    void engineInputMsg(QString msg);

private slots:
    void onData();

private:
    bool sendItemPlacing(QString _args);

    QProcess *m_engine = nullptr;
};

#endif // ENGINE_INTPROCINTPROC_H
