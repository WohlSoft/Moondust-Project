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

#include "38A_client.h"
#include <PGE_File_Formats/file_formats.h>
#include <PGE_File_Formats/pge_file_lib_private.h> // 38A specific URL Encode/Decode
#include <PGE_File_Formats/smbx38a_private.h> // 38A specific URL Encode/Decode
#include <QThread>
#include <mainwindow.h>
#include <common_features/logger.h>
#include <networking/engine_intproc.h>
#include <cstring>
#include <signal.h>



SanBaEiIpcClient::SanBaEiIpcClient(QObject *parent) :
    QObject(parent)
{
    QObject::connect(&m_outQueueTimer, &QTimer::timeout,
                     this, &SanBaEiIpcClient::processQueue);
    QObject::connect(&m_pinger, &QTimer::timeout,
                     this, &SanBaEiIpcClient::pingGame);
}

void SanBaEiIpcClient::setMainWindow(QWidget *mw)
{
    m_mainWindow = mw;
}

void SanBaEiIpcClient::init(QProcess *engine)
{
    m_game = engine;

    QObject::connect(&m_bridge, &QProcess::readyReadStandardOutput,
                     this, &SanBaEiIpcClient::onReadReady);

    QObject::connect(&m_bridge, &QProcess::started,
                     this, &SanBaEiIpcClient::onBridgeStart);
    QObject::connect(&m_bridge,
                     static_cast<void(QProcess::*)(int, QProcess::ExitStatus)>(&QProcess::finished),
                     this, &SanBaEiIpcClient::onBridgeFinish);

    m_bridge.setReadChannel(QProcess::StandardOutput);

    QObject::connect(&g_intEngine, &IntEngineSignals::sendCheat,
                     this, &SanBaEiIpcClient::sendCheat);
    QObject::connect(&g_intEngine, &IntEngineSignals::sendMessageBox,
                     this, &SanBaEiIpcClient::sendMessageBox);
    QObject::connect(&g_intEngine, &IntEngineSignals::sendPlacingBlock,
                     this, &SanBaEiIpcClient::sendPlacingBlock);
    QObject::connect(&g_intEngine, &IntEngineSignals::sendPlacingBGO,
                     this, &SanBaEiIpcClient::sendPlacingBGO);
    QObject::connect(&g_intEngine, &IntEngineSignals::sendPlacingNPC,
                     this, &SanBaEiIpcClient::sendPlacingNPC);
}

void SanBaEiIpcClient::quit()
{
    QObject::disconnect(&g_intEngine, &IntEngineSignals::sendCheat,
                        this, &SanBaEiIpcClient::sendCheat);
    QObject::disconnect(&g_intEngine, &IntEngineSignals::sendMessageBox,
                        this, &SanBaEiIpcClient::sendMessageBox);
    QObject::disconnect(&g_intEngine, &IntEngineSignals::sendPlacingBlock,
                        this, &SanBaEiIpcClient::sendPlacingBlock);
    QObject::disconnect(&g_intEngine, &IntEngineSignals::sendPlacingBGO,
                        this, &SanBaEiIpcClient::sendPlacingBGO);
    QObject::disconnect(&g_intEngine, &IntEngineSignals::sendPlacingNPC,
                        this, &SanBaEiIpcClient::sendPlacingNPC);

    QObject::disconnect(&m_bridge, &QProcess::readyReadStandardOutput,
                        this, &SanBaEiIpcClient::onReadReady);

    m_game = nullptr;
}

bool SanBaEiIpcClient::isWorking()
{
    return ((m_game != nullptr) && (m_game->state() == QProcess::Running));
}

bool SanBaEiIpcClient::isBridgeWorking()
{
    return (m_bridge.state() == QProcess::Running);
}

void SanBaEiIpcClient::terminateBridge()
{
    if(m_bridgeReady)
        sendMessage("TERMINATE");
    else
        m_bridge.terminate();
    m_bridge.waitForFinished(3000);
}

bool SanBaEiIpcClient::sendCheat(QString)
{
    return false; // Unsupported
}

bool SanBaEiIpcClient::sendMessageBox(QString _args)
{
    if(!isWorking() || _args.isEmpty() || !m_bridgeReady)
        return false;
    return sendMessage(QString("TMG|%1").arg(PGE_URLENC(_args)));
}

void SanBaEiIpcClient::sendPlacingBlock(const LevelBlock &block)
{   //    1     2      3   4  5  6 7  8      9  10   11   12   13 14 15 16 17 18
    // B|layer[,name]|id[,dx,dy]|x|y|contain,sp|b11[,b12]|b2|[e1,e2,e3,e4]|w|h

//Debug: SMBX-38A: Sent message:     SO|B||521|0      |0      ||0|0|,,|32|32
//Debug: SMBX38A: Input data: INPUT:GBA|B||1  |-199712|-200416||0|0||32|32

//    auto msg = QString("SO|B|%1,%2|%3,%4,%5|%6|%7|%8,%9|%10,%11|%12|%13,%14,%15,%16|%17|%18")
    QString msg = "SO|B|";
    if(block.layer != "Default")
        msg += PGE_URLENC(block.layer);
    if(!block.gfx_name.isEmpty())
        msg += "," + PGE_URLENC(block.gfx_name);
    msg += "|";

    msg += QString::number(block.id);
    if(block.gfx_dx != 0 || block.gfx_dy != 0)
        msg += QString(",%1,%2")
                .arg(block.gfx_dx)
                .arg(block.gfx_dy);
    msg += "|";

    msg += QString::number(block.x) + "|";
    msg += QString::number(block.y) + "|";

    if(block.npc_id != 0)
    {
        auto npcId = block.npc_id;
        if(npcId < 0)
            npcId *= -1;
        else
            npcId += 1000;
        msg += QString::number(npcId);
        if(block.npc_special_value > 0)
            msg += "," + QString::number(block.npc_special_value);
    }
    msg += "|";

    msg += block.slippery ? "1" : "0";
    if(block.motion_ai_id > 0)
        msg += "," + QString::number(block.motion_ai_id);
    msg += "|";

    msg += block.invisible ? "1" : "0";
    msg += "|";

    if(!block.event_destroy.isEmpty() || !block.event_hit.isEmpty() ||
       !block.event_emptylayer.isEmpty() || !block.event_on_screen.isEmpty())
    {
        msg += PGE_URLENC(block.event_destroy);
        msg += "," + PGE_URLENC(block.event_hit);
        msg += "," + PGE_URLENC(block.event_emptylayer);
        if(!block.event_on_screen.isEmpty())
            msg += "," + PGE_URLENC(block.event_on_screen);
    }
    msg += "|";

    msg += QString::number(block.w) + "|";
    msg += QString::number(block.h);

    sendMessage(msg);
    sendMessage("GB");
}

void SanBaEiIpcClient::sendPlacingBGO(const LevelBGO &bgo)
{
    // T|layer|id[,dx,dy]|x|y
    QString msg = "SO|T|";

    if(bgo.layer != "Default")
        msg += PGE_URLENC(bgo.layer);
    msg += "|";

    msg += QString::number(bgo.id);
    if(bgo.gfx_dx != 0 || bgo.gfx_dy != 0)
        msg += QString(",%1,%2")
                .arg(bgo.gfx_dx)
                .arg(bgo.gfx_dy);
    msg += "|";

    msg += QString::number(bgo.x) + "|";
    msg += QString::number(bgo.y);

    sendMessage(msg);
    sendMessage("GB");
}

void SanBaEiIpcClient::sendPlacingNPC(const LevelNPC &npc)
{
    QString msg = "SO|N|";
    // N|layer[,name]|id[,dx,dy]|x|y|b1,b2,b3,b4,b5,b6|sp|[e1,e2,e3,e4,e5,e6,e7]|a1,a2|c1[,c2,c3,c4,c5,c6,c7]|msg|[wi,hi]

    //Pre-convert some data into SMBX-38A compatible format
    long npcID = (long)npc.id;
    long containerType = 0;
    long specialData = npc.special_data;
    switch(npcID)//Convert npcID and contents ID into container type
    {
    case 91:
        containerType = 1;
        break;
    case 96:
        containerType = 2;
        break;
    case 283:
        containerType = 3;
        break;
    case 284:
        containerType = 4;
        break;
    case 300:
        containerType = 5;
        break;
    case 347:
        containerType = 6;
        break;
    default:
        containerType = 0;
        break;
    }
    if(containerType != 0)
    {
        //Set NPC-ID of contents as main NPC-ID for this NPC
        npcID = npc.contents;
    }

    //Convert "Is Boss" flag into special ID
    switch(npc.id)
    {
    case 15:
    case 39:
    case 86:
        if(npc.is_boss)
            specialData = (long)npc.is_boss;
        break;
    default:
        break;
    }

    //Convert generator type and direction into SMBX-38A Compatible format
    long genType_1 = npc.generator_type;

    //Swap "Appear" and "Projectile" types
    switch(genType_1)
    {
    case 0:
        genType_1 = 2;
        break;
    case 2:
        genType_1 = 0;
        break;
    }

    long genType_2 = npc.generator_direct;
    long genType = (genType_2 != 0) ? ((4 * genType_1) + genType_2) : 0 ;
    //    N|layer|id|x|y|b1,b2,b3,b4|sp|e1,e2,e3,e4,e5,e6,e7|a1,a2|c1[,c2,c3,c4,c5,c6,c7]|msg|
    //    layer=layer name["" == "Default"][***urlencode!***]
    if(npc.layer != "Default")
        msg += PGE_URLENC(npc.layer);
    msg += "|";
    //only if name != ""
    //name=npc's name
    if(!IsEmpty(npc.gfx_name))
        msg += "," + PGE_URLENC(npc.gfx_name);
    //    id=npc id
    msg += "|" + fromNum(npcID);
    if((npc.gfx_dx) > 0 || (npc.gfx_dy > 0))
    {
        //  dx=graphics extend x
        msg += "," + fromNum(npc.gfx_dx);
        //  dy=graphics extend y
        msg += "," + fromNum(npc.gfx_dy);
    }
    //    x=npc position x
    msg += "|" + fromNum(npc.x);
    //    y=npc position y
    msg += "|" + fromNum(npc.y);
    //    b1=[1]left [0]random [-1]right
    msg += "|" + fromNum(-1 * npc.direct);
    //    b2=friendly npc
    msg += "," + fromNum((int)npc.friendly);
    //    b3=don't move npc
    msg += "," + fromNum((int)npc.nomove);
    //    b4=[1=npc91][2=npc96][3=npc283][4=npc284][5=npc300]
    msg += "," + fromNum(containerType);
    //    sp=special option
    msg += "|" + fromNum(specialData);
    //        [***urlencode!***]
    //        e1=death event
    msg += "|" +PGE_URLENC(npc.event_die);
    //        e2=talk event
    msg += "," + PGE_URLENC(npc.event_talk);
    //        e3=activate event
    msg += "," + PGE_URLENC(npc.event_activate);
    //        e4=no more object in layer event
    msg += "," + PGE_URLENC(npc.event_emptylayer);
    //        e5=grabed event
    msg += "," + PGE_URLENC(npc.event_grab);
    //        e6=next frame event
    msg += "," + PGE_URLENC(npc.event_nextframe);
    //        e7=touch event
    msg += "," + PGE_URLENC(npc.event_touch);
    //        a1=layer name to attach
    msg += "|" + PGE_URLENC(npc.attach_layer);
    //        a2=variable name to send
    msg += "," + PGE_URLENC(npc.send_id_to_variable);
    //    c1=generator enable
    msg += "|" + fromNum((int)npc.generator);

    //        [if c1!=0]
    if(npc.generator)
    {
        //        c2=generator period[1 frame]
        //Convert deciseconds into frames with rounding
        long period = npc.generator_period_orig;
        SMBX38A_RestoreOrigTime(period, static_cast<long>(npc.generator_period), PGE_FileLibrary::TimeUnit::Decisecond);
        msg += "," + fromNum(period);
        //        c3=generator effect
        //            c3-1 [1=warp][0=projective][4=no effect]
        //            c3-2 [0=center][1=up][2=left][3=down][4=right][9=up+left][10=left+down][11=down+right][12=right+up]
        //                if (c3-2)!=0
        //                c3=4*(c3-1)+(c3-2)
        //                else
        //                c3=0
        msg += "," + fromNum(genType);
        //        c4=generator direction[angle][when c3=0]
        msg += "," + fromNum(npc.generator_custom_angle);
        //        c5=batch[when c3=0][MAX=32]
        msg += "," + fromNum(npc.generator_branches);
        //        c6=angle range[when c3=0]
        msg += "," + fromNum(npc.generator_angle_range);
        //        c7=speed[when c3=0][float]
        msg += "," + fromNum(npc.generator_initial_speed);
    }

    //    msg=message by this npc talkative[***urlencode!***]
    msg += "|" + PGE_URLENC(npc.msg);

    sendMessage(msg);
}

bool SanBaEiIpcClient::setCursor()
{
    return sendMessage("SO|CURSOR");
}

bool SanBaEiIpcClient::setEraser()
{
    return sendMessage("SO|ERASER");
}

void SanBaEiIpcClient::setNoSuspend(bool en)
{
    if(m_bridgeReady)
    {
        if(!m_noSuspend && en)
            sendMessage("GGI|NOPAUSE");
        // Can't be undone, need to restart a game
    }
    m_noSuspend = en;
}

void SanBaEiIpcClient::setMagicHand(bool en)
{
    if(m_bridgeReady)
    {
        if(!m_enableMagicHand && en)
            sendMessage("SO|CURSOR");
        else if(m_enableMagicHand && !en)
            sendMessage("SO");
    }
    m_enableMagicHand = en;
}

void SanBaEiIpcClient::onInputCommand(const QString &data)
{
    if(data.startsWith("WAITING")) // Bridge waits for a game start
    {
        LogDebug("SMBX38A: Bridge waiting");
    }
    else if(data.startsWith("READY")) // Bridge confirms it's readiness
    {
        LogDebug("SMBX38A: Bridge ready!");
        m_bridgeReady = true;
        m_pinger.start(100);// Start pinging of a game
    }
    else if(data.startsWith("GB|NULL")) // Response to a ping with using of a cursor state request command
    {
        if(m_pinger.isActive()) // Response to the pinger
        {
            m_pinger.stop();
            QString msg;
            if(m_noSuspend)
                msg += "GGI|NOPAUSE\n";
            if(m_enableMagicHand)
                msg += "SO|CURSOR\n";
            msg += "GGI|GM";
            sendMessage(msg);
        }
    }
    else if(data.startsWith("TERMINATED")) // Bridge has been terminated
    {
        LogDebug("SMBX38A: Bridge got terminated!");
        m_bridgeReady = false;
        m_pinger.stop();
    }
    else if(data.startsWith("GGI|GM")) // Game end with a state report
    {
        QStringList d = data.split("|");
        while(d.size() < 5)
            d.push_back(QString());

        // GGI|GM|0|,0,0|20,0,0
        m_lastGameState.exitcode = d[2].toInt();
        auto lData = d[3].split(",");
        auto pData = d[4].split(",");

        while(lData.size() < 3)
            lData.push_back(QString());
        while(pData.size() < 3)
            pData.push_back(QString());

        m_lastGameState.levelName = PGE_URLDEC(lData[0]);
        m_lastGameState.cid = lData[1].toInt();
        m_lastGameState.id = lData[2].toInt();

        m_lastGameState.hp = pData[0].toInt();
        m_lastGameState.co = pData[1].toInt();
        m_lastGameState.sr = pData[2].toInt();

        switch(m_lastGameState.exitcode)
        {
        case 0:
            LogDebug("SMBX38A: Exit by X button");
            break;
        case 99:
            LogDebug("SMBX38A: Exit by player's failure.");
            break;
        default:
            LogDebug(QString("SMBX38A: Exit with code %1").arg(m_lastGameState.exitcode));
            break;
        }
    }
}

void SanBaEiIpcClient::processQueue()
{
    if(m_outQueue.isEmpty())
        return;
    for(auto &e : m_outQueue)
    {
        if(e.timeLeft <= 0)
            continue; // skip sent messages
        e.timeLeft -= 100;
        if(e.timeLeft <= 0)
            sendMessage(e.msg);
    }

    for(auto it = m_outQueue.begin(); it != m_outQueue.end();)
    {
        if(it->timeLeft < 0)
            it = m_outQueue.erase(it);
        else
            it++;
    }

    if(m_outQueue.isEmpty())
        m_outQueueTimer.stop();
}

void SanBaEiIpcClient::pingGame()
{
    // Ping game to recognize it's readiness
    sendMessage("GB");
}

void SanBaEiIpcClient::onReadReady()
{
    if(!isBridgeWorking())
        return;

    m_inBuffer += m_bridge.readAllStandardOutput();
    int ln = m_inBuffer.indexOf('\n');
    if(ln >= 0)
    {
        QString inMsg = QString::fromUtf8(m_inBuffer.mid(0, ln)).trimmed();
        LogDebug(QString("SMBX38A: Input data: %1").arg(inMsg));
        onInputCommand(inMsg);
        m_inBuffer.remove(0, ln + 1);
    }
}

void SanBaEiIpcClient::onBridgeStart()
{
    m_inBuffer.clear();
    m_bridgeReady = false; // Wait until bridge will confirm a readiness
}

void SanBaEiIpcClient::onBridgeFinish(int exitCode, QProcess::ExitStatus exitStatus)
{
    LogDebug(QString("SMBX-38A: Bridge got finished with Exit Code %1 and status %2").arg(exitCode).arg(exitStatus));
    m_bridgeReady = false; // Wait until bridge will confirm a readiness
    m_pinger.stop();
}

bool SanBaEiIpcClient::sendMessage(const char *msg)
{
    if(!m_bridgeReady)
        return false;

    LogDebug(QString("SMBX-38A: Sent message: %1").arg(msg));

    QByteArray output_e(msg);
    output_e.append("\n");
    return (m_bridge.write(output_e) > 0);
}

bool SanBaEiIpcClient::sendMessage(const QString &msg)
{
    if(!m_bridgeReady)
        return false;

    LogDebug(QString("SMBX-38A: Sent message: %1").arg(msg));

    QByteArray output_e = msg.toUtf8();
    output_e.append("\n");
    return (m_bridge.write(output_e) > 0);
}

bool SanBaEiIpcClient::sendMessageDelayed(const QString &msg, int ms)
{
    MsgDelay d = {ms, msg};
    m_outQueue.push_back(d);
    if(!m_outQueueTimer.isActive())
        m_outQueueTimer.start(100);
    return true;
}
