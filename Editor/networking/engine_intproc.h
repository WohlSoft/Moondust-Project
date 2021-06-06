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
#ifndef ENGINE_INTPROCINTPROC_H
#define ENGINE_INTPROCINTPROC_H

#include <QObject>
#include <QProcess>
#include <PGE_File_Formats/lvl_filedata.h>

/**
 * @brief Global re-usable IPC interface
 */
class IntEngineSignals : public QObject
{
    Q_OBJECT
public:
    explicit IntEngineSignals(QObject *parent = nullptr);
    ~IntEngineSignals();

signals:
    /* Outcoming commands */
    bool sendCheat(QString _args);
    bool sendMessageBox(QString _args);
    void sendPlacingBlock(const LevelBlock &block);
    void sendPlacingBGO(const LevelBGO &bgo);
    void sendPlacingNPC(const LevelNPC &npc);
    void sendCurrentLayer(const QString &layerName);

    /* Incoming commands */
    void engineTakenBlock(const LevelBlock &block);
    void engineTakenBGO(const LevelBGO &bgo);
    void engineTakenNPC(const LevelNPC &npc);
    void engineNumStarsChanged(int numStars);
    void enginePlayerStateUpdated(int playerID, int character, int state, int vehicleID, int vehicleType);
    void engineCloseProperties();
};

extern IntEngineSignals g_intEngine;

#endif // ENGINE_INTPROCINTPROC_H
