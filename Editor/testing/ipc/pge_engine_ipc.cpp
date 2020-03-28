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

#include "pge_engine_ipc.h"
#include <PGE_File_Formats/file_formats.h>
#include <common_features/main_window_ptr.h>
#include <common_features/app_path.h>
#include <common_features/logger.h>
#include <common_features/main_window_ptr.h>
#include <networking/engine_intproc.h>
#include <QSharedPointer>

/**
 * @brief Encodes string with a Base64
 * @param out output QByteArray container to write an output result
 * @param msg Input string message
 */
static void base64_encode(QByteArray &out, const char *msg)
{
    QByteArray ba(msg);
    out = ba.toBase64();
}

/**
 * @brief Decodes base64 string
 * @param out output QByteArray container to write an output result
 * @param string Input string message, encoded with a Base64
 */
static void base64_encode(QByteArray &out, const QString &string)
{
    QByteArray ba(string.toUtf8());
    out = ba.toBase64();
}


PgeEngineIpcClient::PgeEngineIpcClient(QObject *parent) :
    QObject(parent)
{}

void PgeEngineIpcClient::setMainWindow(QWidget *mw)
{
    m_mainWindow = mw;
}

void PgeEngineIpcClient::onInputData()
{
    if(!isWorking())
        return;

    QByteArray strData = m_engine->readAllStandardOutput();
    QByteArray msg = QByteArray::fromBase64(strData);

    if(msg.startsWith("CMD:"))
    {
        char *msgP = msg.data() + 4; //"CMD:"
        LogDebug("ENGINE COMMAND: >>");
        LogDebug(msgP);
        LogDebug("<<ENGINE COMMAND END");

        if(std::strcmp(msgP, "CONNECT_TO_ENGINE") == 0)
            sendLevelBuffer();
        else if(std::strcmp(msgP, "ENGINE_CLOSED") == 0)
        {
            if(m_mainWindow)
            {
                m_mainWindow->show();
                m_mainWindow->raise();
            }
        }
    }
    else
    {
        // Any unknown commands
        emit engineInputMsg(QString::fromUtf8(msg));
    }
}

void PgeEngineIpcClient::init(QProcess *engine)
{
    m_engine = engine;
    if(m_engine)
        QObject::connect(m_engine, &QProcess::readyReadStandardOutput,
                         this, &PgeEngineIpcClient::onInputData);

    QObject::connect(&g_intEngine, &IntEngineSignals::sendCheat,
                     this, &PgeEngineIpcClient::sendCheat);
    QObject::connect(&g_intEngine, &IntEngineSignals::sendMessageBox,
                     this, &PgeEngineIpcClient::sendMessageBox);
    QObject::connect(&g_intEngine, &IntEngineSignals::sendPlacingBlock,
                     this, &PgeEngineIpcClient::sendPlacingBlock);
    QObject::connect(&g_intEngine, &IntEngineSignals::sendPlacingBGO,
                     this, &PgeEngineIpcClient::sendPlacingBGO);
    QObject::connect(&g_intEngine, &IntEngineSignals::sendPlacingNPC,
                     this, &PgeEngineIpcClient::sendPlacingNPC);
}

void PgeEngineIpcClient::quit()
{
    FileFormats::CreateLevelData(m_levelTestBuffer);

    QObject::disconnect(&g_intEngine, &IntEngineSignals::sendCheat,
                        this, &PgeEngineIpcClient::sendCheat);
    QObject::disconnect(&g_intEngine, &IntEngineSignals::sendMessageBox,
                        this, &PgeEngineIpcClient::sendMessageBox);
    QObject::disconnect(&g_intEngine, &IntEngineSignals::sendPlacingBlock,
                        this, &PgeEngineIpcClient::sendPlacingBlock);
    QObject::disconnect(&g_intEngine, &IntEngineSignals::sendPlacingBGO,
                        this, &PgeEngineIpcClient::sendPlacingBGO);
    QObject::disconnect(&g_intEngine, &IntEngineSignals::sendPlacingNPC,
                        this, &PgeEngineIpcClient::sendPlacingNPC);
    if(m_engine)
        QObject::disconnect(m_engine, &QProcess::readyReadStandardOutput,
                            this, &PgeEngineIpcClient::onInputData);
    m_engine = nullptr;
}

bool PgeEngineIpcClient::isWorking()
{
    return ((m_engine != nullptr) && (m_engine->state() == QProcess::Running));
}

bool PgeEngineIpcClient::sendCheat(QString _args)
{
    if(isWorking())
    {
        if(_args.isEmpty())
            return false;

        _args.replace('\n', "\\n");
        QString out = QString("CHEAT: %1").arg(_args);
        return sendMessage(out);
    }
    else
        return false;
}

bool PgeEngineIpcClient::sendMessageBox(QString _args)
{
    if(isWorking())
    {
        if(_args.isEmpty())
            return false;

        _args.replace('\n', "\\n");
        QString out = QString("MSGBOX: %1").arg(_args);
        return sendMessage(out);
    }
    else
        return false;
}

void PgeEngineIpcClient::sendPlacingBlock(const LevelBlock &block)
{
    if(!isWorking())
        return;
    LevelData buffer;
    FileFormats::CreateLevelData(buffer);
    buffer.blocks.push_back(block);
    buffer.layers.clear();
    buffer.events.clear();
    QString encoded;
    if(FileFormats::WriteExtendedLvlFileRaw(buffer, encoded))
        sendItemPlacing("BLOCK_PLACE\nBLOCK_PLACE_END\n" + encoded);
}

void PgeEngineIpcClient::sendPlacingBGO(const LevelBGO &bgo)
{
    if(!isWorking())
        return;
    LevelData buffer;
    FileFormats::CreateLevelData(buffer);
    buffer.bgo.push_back(bgo);
    buffer.layers.clear();
    buffer.events.clear();
    QString encoded;
    if(FileFormats::WriteExtendedLvlFileRaw(buffer, encoded))
        sendItemPlacing("BGO_PLACE\nBGO_PLACE_END\n" + encoded);
}

void PgeEngineIpcClient::sendPlacingNPC(const LevelNPC &npc)
{
    if(!isWorking())
        return;
    LevelData buffer;
    FileFormats::CreateLevelData(buffer);
    buffer.npc.push_back(npc);
    buffer.layers.clear();
    buffer.events.clear();
    QString encoded;
    if(FileFormats::WriteExtendedLvlFileRaw(buffer, encoded))
        sendItemPlacing("NPC_PLACE\nNPC_PLACE_END\n" + encoded);
}

bool PgeEngineIpcClient::sendItemPlacing(const QString &rawData)
{
    if(!isWorking())
        return false;
    LogDebug("ENGINE: Place item command: " + rawData);
    QString out = QString("PLACEITEM: %1").arg(rawData);
    bool ret = sendMessage(out);
    return ret;
}

void PgeEngineIpcClient::sendLevelBuffer()
{
    if(!isWorking())
        return;

    LogDebug("Attempt to send LVLX buffer");
    QString output;
    FileFormats::WriteExtendedLvlFileRaw(m_levelTestBuffer, output);
    QString sendLvlx;

    if(!m_levelTestBuffer.meta.path.isEmpty())
        sendLvlx = QString("SEND_LVLX: %1/%2\n")
                   .arg(m_levelTestBuffer.meta.path)
                   .arg(m_levelTestBuffer.meta.filename + ".lvlx");
    else
        sendLvlx = QString("SEND_LVLX: %1/%2\n")
                   .arg(ApplicationPath)
                   .arg("_untitled.lvlx");

    if(output.size() <= 0)
        output = "HEAD\nEMPTY:1\nHEAD_END\n";

//#ifdef DEBUG_BUILD
//        qDebug() << "Sent File data BEGIN >>>>>>>>>>>\n" << output << "\n<<<<<<<<<<<<Sent File data END";
//#endif
    sendMessage(sendLvlx);
    QByteArray output_e;
    base64_encode(output_e, output);
    output_e.append('\n');
    m_engine->write(output_e);
    sendMessage("PARSE_LVLX");
    LogDebug("LVLX buffer sent");
}

void PgeEngineIpcClient::setTestLvlBuffer(const LevelData &buffer)
{
    m_levelTestBuffer = buffer;
}

bool PgeEngineIpcClient::sendMessage(const char *msg)
{
    if(!isWorking())
        return false;

    QByteArray output_e;
    base64_encode(output_e, msg);
    output_e.append('\n');
    return (m_engine->write(output_e) > 0);
}

bool PgeEngineIpcClient::sendMessage(const QString &msg)
{
    if(!isWorking())
        return false;

    QByteArray output_e;
    base64_encode(output_e, msg);
    output_e.append('\n');
    return (m_engine->write(output_e) > 0);
}
