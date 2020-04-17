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
#include <QThread>
#include <mainwindow.h>
#include <common_features/logger.h>
#include <networking/engine_intproc.h>
#include <cstring>
#include <signal.h>



SanBaEiIpcClient::SanBaEiIpcClient(QObject *parent) :
    QObject(parent)
{}

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
        msg += QString::number(block.npc_id);
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
    // N|layer[,name]|id[,dx,dy]|x|y|b1,b2,b3,b4,b5,b6|sp|[e1,e2,e3,e4,e5,e6,e7]|a1,a2|c1[,c2,c3,c4,c5,c6,c7]|msg|[wi,hi]
    /*
    N|layer[,name]|id[,dx,dy]|x|y|b1,b2,b3,b4,b5,b6|sp|[e1,e2,e3,e4,e5,e6,e7]|a1,a2|c1[,c2,c3,c4,c5,c6,c7]|msg|[wi,hi]
        layer=layer name["" == "Default"][***urlencode!***]
        only if name != ""
        name=npc's name
        id=npc id
        only if dx > 0 or dy > 0
        dx=graphics extend x
        dy=graphics extend y
        x=npc position x
        y=npc position y
        b1=[-1]left [0]random [1]right
        b2=friendly npc
        b3=don't move npc
        b4=[1=npc91][2=npc96][3=npc283][4=npc284][5=npc300][101~108=wing type]
        sp=special option
            []=optional
            [***urlencode!***]
            e1=death event
            e2=talk event
            e3=activate event
            e4=no more object in layer event
            e5=grabed event
            e6=next frame event
            e7=touch event
            a1=layer name to attach
            a2=variable name to send
        b5=autoscale
        b6=style of wings(0=wings 1=propeller)
        c1=generator enable
            [if c1!=0]
            c2=generator period[1 frame]
            c3=generator effect
                c3-1[1=warp][0=projective][4=no effect]
                c3-2[0=center][1=up][2=left][3=down][4=right][9=up+left][10=left+down][11=down+right][12=right+up]
                    if (c3-2)!=0
                    c3=4*(c3-1)+(c3-2)
                    else
                    c3=0
            c4=generator direction[angle][when c3=0]
            c5=batch[when c3=0][MAX=32]
            c6=angle range[when c3=0]
            c7=speed[when c3=0][float]
        msg=message by this npc talkative[***urlencode!***]
        wi=width of this npc
        hi=height of this npc
    */
    // TODO this!

    // sendMessage(msg);
}

bool SanBaEiIpcClient::setCursor()
{
    return sendMessage("SO|CURSOR");
}

bool SanBaEiIpcClient::setEraser()
{
    return sendMessage("SO|ERASER");
}

void SanBaEiIpcClient::onInputCommand(const QString &data)
{
    if(data.startsWith("WAITING"))
    {
        LogDebug("SMBX38A: Bridge waiting");
    }
    else if(data.startsWith("READY"))
    {
        LogDebug("SMBX38A: Bridge ready!");
        m_bridgeReady = true;
        QThread::msleep(200);
        sendMessage("GB");
    }
    else if(data.startsWith("GB|NULL"))
    {
        sendMessage("SO|CURSOR\n"
                    "GGI|NOPAUSE\n"
                    "GGI|GM");
    }
    else if(data.startsWith("TERMINATED"))
    {
        LogDebug("SMBX38A: Bridge got terminated!");
        m_bridgeReady = false;
    }
    else if(data.startsWith("GGI|GM"))
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
