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
#include <mainwindow.h>
#include <common_features/logger.h>
#include <networking/engine_intproc.h>
#include <cstring>



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
    if(m_game)
        QObject::connect(m_game, &QProcess::readyReadStandardOutput,
                         this, &SanBaEiIpcClient::onInputData);

    m_game->setReadChannel(QProcess::StandardOutput);

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
    if(m_game)
        QObject::disconnect(m_game, &QProcess::readyReadStandardOutput,
                            this, &SanBaEiIpcClient::onInputData);
    m_game = nullptr;
}

bool SanBaEiIpcClient::isWorking()
{
    return ((m_game != nullptr) && (m_game->state() == QProcess::Running));
}

bool SanBaEiIpcClient::sendCheat(QString)
{
    return false; // Unsupported
}

bool SanBaEiIpcClient::sendMessageBox(QString _args)
{
    if(!isWorking() || _args.isEmpty())
        return false;
    return sendMessage(QString("TMG|%1").arg(PGE_URLENC(_args)));
}

void SanBaEiIpcClient::sendPlacingBlock(const LevelBlock &block)
{
    // B|layer[,name]|id[,dx,dy]|x|y|contain,sp|b11[,b12]|b2|[e1,e2,e3,e4]|w|h
    auto msg = QString("SO|B|%1,%2|%3,%4,%5|%6,%7|%8,%9|%10|%11,%12,%13,%14|%15")
            .arg(block.layer == "Default" ? QString() : PGE_URLENC(block.layer))
            .arg(PGE_URLENC(block.gfx_name))

            .arg(block.id)
            .arg(block.gfx_dx)
            .arg(block.gfx_dy)

            .arg(block.x)
            .arg(block.y)

            .arg(block.npc_id)
            .arg(block.npc_special_value)

            .arg(block.slippery ? 1 : 0)
            .arg(block.motion_ai_id)

            .arg(block.invisible ? 1 : 0)

            .arg(PGE_URLENC(block.event_destroy))
            .arg(PGE_URLENC(block.event_hit))
            .arg(PGE_URLENC(block.event_emptylayer))
            .arg(PGE_URLENC(block.event_on_screen))

            .arg(block.w)
            .arg(block.h)
    ;
    sendMessage(msg);
}

void SanBaEiIpcClient::sendPlacingBGO(const LevelBGO &bgo)
{
    // T|layer|id[,dx,dy]|x|y
    auto msg = QString("SO|T|%1|%2,%3,%4|%5,%6")
            .arg(bgo.layer == "Default" ? QString() : PGE_URLENC(bgo.layer))

            .arg(bgo.id)
            .arg(bgo.gfx_dx)
            .arg(bgo.gfx_dy)

            .arg(bgo.x)
            .arg(bgo.y)
    ;
    sendMessage(msg);
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

void SanBaEiIpcClient::onInputData()
{

}


bool SanBaEiIpcClient::sendMessage(const char *msg)
{
    if(!isWorking())
        return false;

    QByteArray output_e(msg);
    output_e.append('\n');
    return (m_game->write(output_e) > 0);
}

bool SanBaEiIpcClient::sendMessage(const QString &msg)
{
    if(!isWorking())
        return false;

    QByteArray output_e = msg.toUtf8();
    output_e.append('\n');
    return (m_game->write(output_e) > 0);
}
