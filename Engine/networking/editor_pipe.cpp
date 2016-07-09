#include "editor_pipe.h"
#include <common_features/logger.h>

#include <QFile>
#include <QStringList>
#include <QtDebug>
#include <QElapsedTimer>
#include <QApplication>

#include <QUdpSocket>

#include "../common_features/app_path.h"
#include <networking/intproc.h>

/**************************************************************************************************************/
static QString base64_encode(QString string){
    QByteArray ba;
    ba.append(string);
    return ba.toBase64();
}

static QString base64_decode(QString string){
    QByteArray ba;
    ba.append(string);
    return QByteArray::fromBase64(ba);
}

EditorPipe_std::EditorPipe_std() : m_input(stdin, QIODevice::ReadOnly), m_output(stdout, QIODevice::WriteOnly)
{
    m_input.setCodec("UTF-8");
}

void EditorPipe_std::run()
{
    forever
    {
        QString m = m_input.readLine();
        emit msg(base64_decode(m));
    }
}

void EditorPipe_std::sendMessage(QString msg)
{
    m_output << base64_encode(msg);
    m_output.flush();
}
/**************************************************************************************************************/


/**
 * @brief EditorPipe::LocalServer
 *  Constructor
 */

EditorPipe::EditorPipe() : QObject(NULL)
{
    //direct connection is required, because main thread is not processes signal-slots system events
    connect(&msgr, SIGNAL(msg(QString)),         this,  SLOT(icomingData(QString)), Qt::DirectConnection);
    connect(this,  SIGNAL(sendMessage(QString)), &msgr, SLOT(sendMessage(QString)));
    msgr.start();

    qDebug() << "Construct interprocess pipe";
    do_acceptLevelData=false;
    do_parseLevelData=false;

    accepted_lvl_raw="";
    accepted_lvl.ReadFileValid = false;
    isWorking = false;
    m_levelAccepted=false;
}

/**
 * @brief EditorPipe::~LocalServer
 *  Destructor
 */
EditorPipe::~EditorPipe()
{
    msgr.terminate();
}

void EditorPipe::shut()
{
    sendToEditor("CMD:ENGINE_CLOSED");
    msgr.terminate();
}

bool EditorPipe::levelIsLoad()
{
    m_levelAccepted_lock.lock();
    bool state = m_levelAccepted;
    m_levelAccepted = false;
    m_levelAccepted_lock.unlock();
    return state;
}

bool EditorPipe::sendToEditor(QString command)
{
    msgr.sendMessage(command);
    return true;
}

void EditorPipe::icomingData(QString in)
{
    if( ("PARSE_LVLX"==in) || ("OPEN_TEMP_LVLX"==in) )
    {
        do_acceptLevelData=false;
        qDebug() << "LVLX accepting is done";
        IntProc::setState("LVLX Accepted, do parsing of LVLX");
    }

    if(do_acceptLevelData)
    {
        accepted_lvl_raw.append(in);
        qDebug() << "append LVLX data";
    }
    else
    if(in.startsWith("SEND_LVLX: ", Qt::CaseSensitive))
    {
        qDebug() << "IN: >>"<< (in.size()>30 ? QString(in).remove(30, in.size()-31) : in);
        in.remove("SEND_LVLX: ");
        accepted_lvl_path   = in;
        do_acceptLevelData  = true;
        IntProc::setState("Accepted SEND_LVLX");
        sendMessage("READY\n\n");
    }
    else
    if(in=="PARSE_LVLX")
    {
        qDebug() << "do Parse LVLX: >>"<< in;
        do_parseLevelData=true;
        FileFormats::ReadExtendedLvlFileRaw(accepted_lvl_raw, accepted_lvl_path, accepted_lvl);
        IntProc::setState(QString("LVLX is valid: %1").arg(accepted_lvl.ReadFileValid));
        qDebug()<<"Level data parsed, Valid:" << accepted_lvl.ReadFileValid;
        if(!accepted_lvl.ReadFileValid)
        {
            qDebug()<<"Error reason: "  << accepted_lvl.ERROR_info;
            qDebug()<<"line number: "   << accepted_lvl.ERROR_linenum;
            qDebug()<<"line contents: " << accepted_lvl.ERROR_linedata;
            #ifdef DEBUG_BUILD
            qDebug()<<"Invalid File data BEGIN >>>>>>>>>>>\n" << accepted_lvl_raw << "\n<<<<<<<<<<<<INVALID File data END";
            #endif
        }
        m_levelAccepted_lock.lock();
        m_levelAccepted=true;
        m_levelAccepted_lock.unlock();
    }
    else
    if(in=="OPEN_TEMP_LVLX")
    {
        qDebug() << "do Parse LVLX: >>"<< in;
        do_parseLevelData=true;
        QFile temp(accepted_lvl_raw.remove("\n"));
        //if( FileFormats::ReadExtendedLvlFileF(accepted_lvl_raw.remove("\n"), accepted_lvl) )
        if(temp.open(QFile::ReadOnly|QFile::Text))
        {
            QTextStream x(&temp);
            QString raw=x.readAll();
            FileFormats::ReadExtendedLvlFileRaw(raw, accepted_lvl_path, accepted_lvl);
            IntProc::setState(QString("LVLX is valid: %1").arg(accepted_lvl.ReadFileValid));
            qDebug()<<"Level data parsed, Valid:" << accepted_lvl.ReadFileValid;
        }
        else
        {
            IntProc::setState(QString("LVLX is valid: %1").arg(accepted_lvl.ReadFileValid));
            qDebug()<<"Level data parsed, Valid:" << accepted_lvl.ReadFileValid;
            qDebug()<<"ERROR: Can't open temp file";
            accepted_lvl.ReadFileValid = false;
        }
        temp.remove();
        m_levelAccepted_lock.lock();
        m_levelAccepted=true;
        m_levelAccepted_lock.unlock();
    }
    else
    if(in.startsWith("PLACEITEM: ", Qt::CaseSensitive))
    {
        qDebug() << "Accepted Placing item!";
        in.remove("PLACEITEM: ");
        IntProc::storeCommand(in, IntProc::PlaceItem);
    }
    else
    if(in.startsWith("MSGBOX: ", Qt::CaseSensitive))
    {
        qDebug() << "Accepted Message box";
        in.remove("MSGBOX: ");
        IntProc::storeCommand(in, IntProc::MsgBox);
    }
    else
    if(in.startsWith("CHEAT: ", Qt::CaseSensitive))
    {
        qDebug() << "Accepted cheat code";
        in.remove("CHEAT: ");
        IntProc::storeCommand(in, IntProc::Cheat);
    }
    else
    if(in=="PING")
    {
        qDebug() << "IN: >>"<< in;
        sendMessage("PONG\n\n");
        qDebug()<< "Ping-Pong!";
    }
}
