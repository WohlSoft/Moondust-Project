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
#ifndef SANBAEIIPCCLIENT_H
#define SANBAEIIPCCLIENT_H

#include <QObject>
#include <QProcess>
#include <QTimer>
#include <PGE_File_Formats/lvl_filedata.h>

/**
 * @brief SMBX-38A compatible IPC client
 */
class SanBaEiIpcClient : public QObject
{
    Q_OBJECT
    friend class SanBaEiRuntimeEngine;

public:
    explicit SanBaEiIpcClient(QObject *parent = nullptr);
    ~SanBaEiIpcClient() override = default;

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
     * @brief Is a bridge running?
     * @return true if running
     */
    bool isBridgeWorking();

    void terminateBridge();

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
     * @brief Change a layer of placing element
     * @param layerName
     */
    void sendCurrentLayer(const QString &layerName);

    /**
     * @brief Change the level data buffer for testing
     * @param buffer Level data structure
     */
    void setTestLvlBuffer(const LevelData &buffer);

    bool setCursor();

    bool setEraser();

    void setNoSuspend(bool en);

    void setMagicHand(bool en);

private:
    /**
     * @brief Read all input data before last delimeter character
     * @param out A target buffer to store an output data
     */
    void readInputStream(QByteArray &out);

    void onInputCommand(const QString &data);

    void processQueue();
    void pingGame();

private slots:
    /**
     * @brief Event call, triggered when input data comes up
     */
    void onReadReady();

    void onBridgeStart();
    void onBridgeFinish(int exitCode, QProcess::ExitStatus exitStatus);

signals:
    /**
     * @brief A hook for any unrecognized engine commands
     * @param msg Raw engine message text
     */
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
     * @brief Send a raw message command to the Engine server with a delay
     * @param msg raw message string
     * @param ms Milliseconds delay
     * @return true on a success sending
     */
    bool sendMessageDelayed(const QString &msg, int ms);

    /**********SETUP***************/
    // Disable pausing when unfocused
    bool m_noSuspend = true;
    // Enable Magic Hand
    bool m_enableMagicHand = true;
    /**********SETUP***************/

    /**
     * @brief Keeps a state of game while game is off
     */
    struct GameState
    {
        //! 0 = 'X Button', 99 = player failed, else: same as world map's level exit code.
        int exitcode = 0;
        //! if the player arrived the checkpoint, this value will be set to the level's filename[***urlencode!***]
        QString levelName;
        //! if the player arrived the checkpoint, this value will be set to the checkpoint's perm ID.
        int cid = 0;
        //! if the player arrived the checkpoint, this value will be set to the checkpoint's advset value.
        int id = 0;
        //! current 1up number
        int hp = 20;
        //! the coins number
        int co = 0;
        //! current score
        int sr = 0;
        //! Reset game state
        void reset()
        {
            exitcode = 0;
            levelName.clear();
            cid = 0;
            id = 0;
            hp = 20;
            co = 0;
            sr = 0;
        }
    } m_lastGameState;

    struct MsgDelay
    {
        int timeLeft = 0;
        QString msg;
    };
    //! Queoe of delayed output messages
    QList<MsgDelay> m_outQueue;
    //! Timer to re-check delayed messages queue
    QTimer m_outQueueTimer;

    //! Timer to ping a game to detect it's readiness
    QTimer m_pinger;
    //! Input buffer
    QByteArray m_inBuffer;
    //! Readiness status of the bridge
    bool m_bridgeReady = false;
    //! Main window instance pointer
    QWidget *m_mainWindow = nullptr;
    //! Bridge process
    QProcess m_bridge;
    //! Game process (works independently from a bridge)
    QProcess *m_game = nullptr;
};

#endif // SANBAEIIPCCLIENT_H
