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
#ifndef PGE_ENGINE_IPC_H
#define PGE_ENGINE_IPC_H

#include <QObject>
#include <QProcess>
#include <PGE_File_Formats/lvl_filedata.h>

/**
 * @brief PGE-Engine compatible IPC client
 */
class PgeEngineIpcClient: public QObject
{
    Q_OBJECT
public:
    explicit PgeEngineIpcClient(QObject *parent);
    ~PgeEngineIpcClient() override = default;

    /**
     * @brief Set the main window pointer
     * @param mw Pointer to a main window instance
     */
    void setMainWindow(QWidget *mw);

    /**
     * @brief Initialize IPC module and register it in the global interface
     * @param engine pointer to server engine QProcess
     */
    void init(QProcess *engine);
    /**
     * @brief De-initializee IPC module and unregister it from the global interface
     */
    void quit();

    /**
     * @brief Is a server engine running?
     * @return true if running
     */
    bool isWorking();

    /**
     * @brief Send a cheat-code command
     * @param _args string contains a raw cheat command
     * @return true on a success sending
     */
    bool sendCheat(QString _args);
    /**
     * @brief Send a message box tech
     * @param _args string contains a text to show in the message box
     * @return true on a success sending
     */
    bool sendMessageBox(QString _args);

    /**
     * @brief Turn on the block placing mode (or change properties)
     * @param block Block data structure
     */
    void sendPlacingBlock(const LevelBlock &block);

    /**
     * @brief Turn on the BGO placing mode (or change properties)
     * @param bgo BGO data structure
     */
    void sendPlacingBGO(const LevelBGO &bgo);

    /**
     * @brief Turn on the NPC placing mode (or change properties)
     * @param npc NPC data structure
     */
    void sendPlacingNPC(const LevelNPC &npc);

    /**
     * @brief Change the level data buffer for testing
     * @param buffer Level data structure
     */
    void setTestLvlBuffer(const LevelData &buffer);

private slots:
    void onInputData();

signals:
    void engineInputMsg(const QString &msg);

private:
    /**
     * @brief Send a raw command to the Engine server
     * @param msg raw message string
     * @return true on a success sending
     */
    bool sendMessage(const char *msg);
    /**
     * @brief Send a raw command to the Engine server
     * @param msg raw message string
     * @return true on a success sending
     */
    bool sendMessage(const QString &msg);

    /**
     * @brief When engine is ready, submit a level data from a cache
     */
    void sendLevelBuffer();

    /**
     * @brief Send a raw data of a placing item
     * @param raw Encoded setting of an object to place
     * @return true on a succes sending
     */
    bool sendItemPlacing(const QString &rawData);

    //! Cached level data buffer
    LevelData m_levelTestBuffer;
    //! Main window instance pointer
    QWidget *m_mainWindow = nullptr;
    //! Engine server QProcess instance
    QProcess *m_engine = nullptr;
};

#endif // PGE_ENGINE_IPC_H
