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

#include "pge_engine_ipc.h"
#include <PGE_File_Formats/file_formats.h>
#include <mainwindow.h>
#include <common_features/logger.h>
#include <common_features/app_path.h>
#include <networking/engine_intproc.h>
#include <cstring>

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
{
    QObject::connect(this, &PgeEngineIpcClient::signalEngineClosed,
                     this, &PgeEngineIpcClient::showMainWindow, Qt::QueuedConnection);
    QObject::connect(this, &PgeEngineIpcClient::signalSendLevelBuffer,
                     this, &PgeEngineIpcClient::sendLevelBuffer, Qt::QueuedConnection);
}

void PgeEngineIpcClient::setMainWindow(QWidget *mw)
{
    m_mainWindow = mw;
}

void PgeEngineIpcClient::readInputStream(QByteArray &out)
{
    char c = 0;

    out.clear();
    out.reserve(100);

    while(true)
    {
        qint64 bytesRead = m_engine->read(&c, 1);
        while(bytesRead == 0)
        {
            // No data? Block before trying again.
            m_engine->waitForReadyRead();
            bytesRead = m_engine->read(&c, 1);
        }

        if(bytesRead != 1)
            break; // error

        if(c == '\n')
            break; // complete
        out.push_back(c);
    }
}


void PgeEngineIpcClient::onInputData()
{
    if(!isWorking())
        return;

    QByteArray strData;
    readInputStream(strData);

    const QByteArray msg = QByteArray::fromBase64(strData);

    if(msg.startsWith("CMD:"))
    {
        const char *msgP = msg.data() + 4; //"CMD:"
        LogDebug("ENGINE COMMAND: >>");
        LogDebug(msgP);
        LogDebug("<<ENGINE COMMAND END");
        qApp->processEvents();

        if(std::strncmp(msgP, "NUM_STARS ", 10) == 0)
        {
            QString num = QString::fromUtf8(msgP + 10);
            int val;
            bool ok;
            val = num.toInt(&ok);
            if(ok)
                emit engineNumStarsChanged(val);
        }
        else if(std::strncmp(msgP, "TAKEN_BLOCK\n", 12) == 0)
        {
            LevelData got;
            QString raw = QString::fromUtf8(msgP);
            PGE_FileFormats_misc::RawTextInput raw_file(&raw);
            FileFormats::ReadExtendedLvlFile(raw_file, got);
            if(!got.meta.ReadFileValid)
                return;
            if(got.blocks.empty())
                return;
            emit engineTakenBlock(got.blocks[0]);
        }
        else if(std::strncmp(msgP, "TAKEN_BGO\n", 10) == 0)
        {
            LevelData got;
            QString raw = QString::fromUtf8(msgP);
            PGE_FileFormats_misc::RawTextInput raw_file(&raw);
            FileFormats::ReadExtendedLvlFile(raw_file, got);
            if(!got.meta.ReadFileValid)
                return;
            if(got.bgo.empty())
                return;
            emit engineTakenBGO(got.bgo[0]);
        }
        else if(std::strncmp(msgP, "TAKEN_NPC\n", 10) == 0)
        {
            LevelData got;
            QString raw = QString::fromUtf8(msgP);
            PGE_FileFormats_misc::RawTextInput raw_file(&raw);
            FileFormats::ReadExtendedLvlFile(raw_file, got);

            if(!got.meta.ReadFileValid)
                return;
            if(got.npc.empty())
                return;

            emit engineTakenNPC(got.npc[0]);
        }
        else if(std::strncmp(msgP, "PLAYER_SETUP_UPDATE ", 20) == 0)
        {
            qApp->processEvents();
            int val[5];
            bool ok;
            QString playernum = QString::fromUtf8(msgP + 20);
            QStringList p = playernum.split(' ');

            if(p.size() != 5)
                return;

            for(int i = 0; i < 5; i++)
            {
                val[i] = p[i].toInt(&ok);
                if(!ok)
                    break;
            }

            if(ok)
                emit enginePlayerStateUpdated(val[0], val[1], val[2], val[3], val[4]);
        }
        else if(std::strcmp(msgP, "CLOSE_PROPERTIES") == 0)
            emit engineCloseProperties();
        else if(std::strcmp(msgP, "CONNECT_TO_ENGINE") == 0)
            emit signalSendLevelBuffer();
        else if(std::strcmp(msgP, "ENGINE_CLOSED") == 0)
            emit signalEngineClosed();
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
                         this, &PgeEngineIpcClient::onInputData, Qt::QueuedConnection);

    m_engine->setReadChannel(QProcess::StandardOutput);
    m_engine->setProcessChannelMode(QProcess::SeparateChannels);

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
    QObject::connect(&g_intEngine, &IntEngineSignals::sendCurrentLayer,
                     this, &PgeEngineIpcClient::sendCurrentLayer);

    QObject::connect(this, &PgeEngineIpcClient::engineTakenBlock,
                     &g_intEngine, &IntEngineSignals::engineTakenBlock, Qt::QueuedConnection);
    QObject::connect(this, &PgeEngineIpcClient::engineTakenBGO,
                     &g_intEngine, &IntEngineSignals::engineTakenBGO, Qt::QueuedConnection);
    QObject::connect(this, &PgeEngineIpcClient::engineTakenNPC,
                     &g_intEngine, &IntEngineSignals::engineTakenNPC, Qt::QueuedConnection);
    QObject::connect(this, &PgeEngineIpcClient::engineNumStarsChanged,
                     &g_intEngine, &IntEngineSignals::engineNumStarsChanged, Qt::QueuedConnection);
    QObject::connect(this, &PgeEngineIpcClient::enginePlayerStateUpdated,
                     &g_intEngine, &IntEngineSignals::enginePlayerStateUpdated, Qt::QueuedConnection);
    QObject::connect(this, &PgeEngineIpcClient::engineCloseProperties,
                     &g_intEngine, &IntEngineSignals::engineCloseProperties, Qt::QueuedConnection);
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
    QObject::disconnect(&g_intEngine, &IntEngineSignals::sendCurrentLayer,
                        this, &PgeEngineIpcClient::sendCurrentLayer);

    QObject::disconnect(this, &PgeEngineIpcClient::engineTakenBlock,
                        &g_intEngine, &IntEngineSignals::engineTakenBlock);
    QObject::disconnect(this, &PgeEngineIpcClient::engineTakenBGO,
                        &g_intEngine, &IntEngineSignals::engineTakenBGO);
    QObject::disconnect(this, &PgeEngineIpcClient::engineTakenNPC,
                        &g_intEngine, &IntEngineSignals::engineTakenNPC);
    QObject::disconnect(this, &PgeEngineIpcClient::engineNumStarsChanged,
                        &g_intEngine, &IntEngineSignals::engineNumStarsChanged);
    QObject::disconnect(this, &PgeEngineIpcClient::enginePlayerStateUpdated,
                        &g_intEngine, &IntEngineSignals::enginePlayerStateUpdated);
    QObject::disconnect(this, &PgeEngineIpcClient::engineCloseProperties,
                        &g_intEngine, &IntEngineSignals::engineCloseProperties);

    if(m_engine)
        QObject::disconnect(m_engine, &QProcess::readyReadStandardOutput,
                            this, static_cast<void(PgeEngineIpcClient::*)(void)>(&PgeEngineIpcClient::onInputData));
    m_engine = nullptr;
}

bool PgeEngineIpcClient::isWorking()
{
    return ((m_engine != nullptr) && (m_engine->state() == QProcess::Running));
}

bool PgeEngineIpcClient::sendCheat(QString _args)
{
    if(!isWorking() || _args.isEmpty())
        return false;

    _args.replace('\n', "\\n");
    QString out = QString("CHEAT: %1").arg(_args);
    return sendMessage(out);
}

bool PgeEngineIpcClient::sendMessageBox(QString _args)
{
    if(!isWorking() || _args.isEmpty())
        return false;

    _args.replace('\n', "\\n");
    QString out = QString("MSGBOX: %1").arg(_args);
    return sendMessage(out);
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

void PgeEngineIpcClient::sendCurrentLayer(const QString &layerName)
{
    if(!isWorking() || layerName.isEmpty())
        return;

    QString out = QString("SET_LAYER: %1").arg(layerName);
    sendMessage(out);
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

    sendNumStars(GlobalSettings::testing.xtra_starsNum);

    LogDebug("Attempt to send LVLX buffer");
    QString output;
    FileFormats::WriteExtendedLvlFileRaw(m_levelTestBuffer, output);
    QString sendLvlx;
    QString x = (!m_levelTestBuffer.meta.smbx64strict ? "x" : "");

    if(!m_levelTestBuffer.meta.path.isEmpty())
        sendLvlx = QString("SEND_LVLX: %1/%2\n")
                   .arg(m_levelTestBuffer.meta.path)
                   .arg(m_levelTestBuffer.meta.filename + ".lvl" + x);
    else
        sendLvlx = QString("SEND_LVLX: %1/%2%3\n")
                   .arg(ApplicationPath)
                   .arg("_untitled.lvl").arg(x);

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

void PgeEngineIpcClient::showMainWindow()
{
    if(!m_mainWindow)
        return;
    m_mainWindow->show();
    m_mainWindow->raise();
}

void PgeEngineIpcClient::setTestLvlBuffer(const LevelData &buffer)
{
    m_levelTestBuffer = buffer;
}

void PgeEngineIpcClient::sendNumStars(int numStars)
{
    if(!isWorking())
        return;
    QString out = QString("SET_NUMSTARS: %1").arg(numStars);
    sendMessage(out);
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
