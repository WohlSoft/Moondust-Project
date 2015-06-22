#include "editor_pipe.h"

#include <QFile>
#include <QStringList>
#include <QtDebug>
#include <QElapsedTimer>
#include <QApplication>

#include <QUdpSocket>

#include "../common_features/app_path.h"
#include <networking/intproc.h>


IntProcServer::IntProcServer()
{
    clientConnection=NULL;
    connect(this, SIGNAL(newConnection()), this, SLOT(handleNewConnection()));
}

IntProcServer::~IntProcServer()
{}

void IntProcServer::writeMessage(QString msg)
{
    if (!clientConnection)
        return;
    clientConnection->write(msg.toUtf8());
    clientConnection->waitForBytesWritten(1000);
}

void IntProcServer::stateChanged(QAbstractSocket::SocketState stat)
{
    switch(stat)
    {
        case QAbstractSocket::UnconnectedState: qDebug()<<"The socket is not connected.";break;
        case QAbstractSocket::HostLookupState: qDebug()<<"The socket is performing a host name lookup.";break;
        case QAbstractSocket::ConnectingState: qDebug()<<"The socket has started establishing a connection.";break;
        case QAbstractSocket::ConnectedState: qDebug()<<"A connection is established.";break;
        case QAbstractSocket::BoundState: qDebug()<<"The socket is bound to an address and port.";break;
        case QAbstractSocket::ClosingState: qDebug()<<"The socket is about to close (data may still be waiting to be written).";break;
        case QAbstractSocket::ListeningState: qDebug()<<"[For internal]";break;
    }
}

void IntProcServer::readData()
{
    QObject * object = QObject::sender(); // далее и ниже до цикла идет преобразования "отправителя сигнала" в сокет, дабы извлечь данные
    if (!object)
        return;
    QTcpSocket * socket = static_cast<QTcpSocket *>(object);
    QByteArray arr =  socket->readAll();
    emit messageIn(QString::fromUtf8(arr));
}

void IntProcServer::handleNewConnection()
{
    if(clientConnection) return;
    clientConnection = nextPendingConnection();
    connect(clientConnection, SIGNAL(disconnected()), this, SLOT(clientDisconnected()));
    connect(clientConnection, SIGNAL(readyRead()), this, SLOT(readData()));
    connect(clientConnection, SIGNAL(stateChanged(QAbstractSocket::SocketState)), this, SLOT(stateChanged(QAbstractSocket::SocketState)));
}

void IntProcServer::clientDisconnected()
{
    clientConnection->deleteLater();
}

void IntProcServer::displayError(QTcpSocket::SocketError socketError)
{
    switch (socketError)
    {
        default:
            qDebug() << QString("ENGINE: The following error occurred: %1.")
                                     .arg(errorString());
    }
}

IntProcServer *ipServer=NULL;


/**
 * @brief EditorPipe::LocalServer
 *  Constructor
 */

EditorPipe::EditorPipe() : QThread(NULL)
{
    if(!ipServer)
        ipServer = new IntProcServer();
    ipServer->listen(QHostAddress::LocalHost, 58486);

    while(ipServer->isListening() == false)
    {
      ipServer->listen(QHostAddress::LocalHost);
      msleep(100);
    }
    qDebug() << "Listen " << ipServer->isListening() << "localhost:58486";

    qDebug() << "Construct interprocess pipe";
    do_acceptLevelData=false;
    do_parseLevelData=false;

    accepted_lvl_raw="";
    accepted_lvl.ReadFileValid = false;
    isWorking = false;
    lastMsgSuccess=true;

    levelAccepted=false;

    connect(ipServer, SIGNAL(messageIn(QString)), this, SLOT(slotOnData(QString)));
    connect(this, SIGNAL(sendMessage(QString)), ipServer, SLOT(writeMessage(QString)));
}


/**
 * @brief EditorPipe::~LocalServer
 *  Destructor
 */
EditorPipe::~EditorPipe()
{
    if(ipServer)
        delete ipServer;
}

void EditorPipe::shut()
{
    sendToEditor("CMD:ENGINE_CLOSED");
}

bool EditorPipe::levelIsLoad()
{
    bool state = levelAccepted;
    levelAccepted=false;
    return state;
}

bool EditorPipe::sendToEditor(QString command)
{
    QUdpSocket socket;
    // Attempt to connect to the LocalServer
    socket.connectToHost(QHostAddress::LocalHost, 58487);

    if(socket.waitForConnected(100))
    {
        qDebug() << "Connected";
        QString str = QString(command);
        QByteArray bytes;
        bytes = str.toUtf8();
        socket.write(bytes);
        socket.waitForBytesWritten(10000);
        socket.flush();
        socket.disconnectFromHost();
        qDebug() << "Bytes sent: " <<command;
        return true;
    }
    else
    {
        qDebug() << "sendToEditor(QString command) fail to connect: " << socket.errorString();
        return false;
    }
}

/**
 * -----------------------
 * QThread requred methods
 * -----------------------
 */

/**
 * @brief run
 *  Initiate the thread.
 */
void EditorPipe::run()
{
    qDebug() << "Start server thread";
  isWorking=true;
  exec();
  isWorking=false;

  qDebug()<< "Server closed";
}



/**
 * @brief EditorPipe::exec
 *  Keeps the thread alive. Waits for incomming connections
 */
void EditorPipe::exec()
{
    IntProc::state="Waiting of commands...";
    while(isWorking)
    {
        msleep(100);
    }
}



//void EditorPipe::slotSendMessage(QString msg)
//{
//    qDebug() << "OUT: <<"<< msg+"\\n\\n";
//    QTcpSocket *client = clients.first();
//    QByteArray toClient = QString(msg+"\n\n").toUtf8();
//    client->write(toClient.data(), toClient.length());
//    lastMsgSuccess=client->waitForBytesWritten(1000);
//}



/**
 * -------
 * SLOTS
 * -------
 */

//void EditorPipe::slotNewConnection()
//{
//    qDebug() << "New connection!";
//    clients.push_front(server->nextPendingConnection());
//    clients.last()->moveToThread(this);

//    QObject::connect(clients.last(), SIGNAL(readyRead()), this, SLOT(slotReadClientData()));

//    qDebug() << clients.size();
//}

//void EditorPipe::slotReadClientData()
//{
//    QTcpSocket* client = (QTcpSocket*)sender();
//    client->moveToThread(this);
//    QByteArray data = client->readAll();
//    QString acceptedData = QString::fromUtf8(data);
//    qDebug() << "--1--IN: >>"<< (acceptedData.size()>30 ? QString(acceptedData).remove(30, acceptedData.size()-31) : acceptedData);

//    emit privateDataReceived(acceptedData, client);
//}


void EditorPipe::slotOnData(QString data)
{
    unsigned int nc=0;
    foreach(QChar c, data)
    {
        buffer.append(c);

        if(c==QChar(QChar::LineFeed))
            nc++;
        else
            nc=0;

        if(nc==2)
        {
            buffer.resize(buffer.size()-2);
            qDebug() << "--2--icomingData<-- ["<< (buffer.size()>30? QString(buffer).remove(30, buffer.size()-31) : buffer)<<"]";
            //qDebug() << "--2--icomingData<-- ["<< buffer<<"]";
            emit icomingData(buffer);
            buffer.clear();
        }
    }
}

void EditorPipe::icomingData(QString in)
{
    if((in=="PARSE_LVLX")||(in=="OPEN_TEMP_LVLX"))
    {
        do_acceptLevelData=false;
        qDebug() << "LVLX accepting is done";
        IntProc::state="LVLX Accepted, do parsing of LVLX";
    }

    if(do_acceptLevelData)
    {
        accepted_lvl_raw.append(in);
        qDebug() << "append LVLX data";
        if(in.endsWith("\n\n"))
        {
            do_acceptLevelData=false;
            qDebug() << "LVLX accepting is done";
        }
    }
    else
    if(in.startsWith("SEND_LVLX: ", Qt::CaseSensitive))
    {
        qDebug() << "IN: >>"<< (in.size()>30? QString(in).remove(30, in.size()-31) : in);

        in.remove("SEND_LVLX: ");
        accepted_lvl_path = in;
        do_acceptLevelData=true;
        IntProc::state="Accepted SEND_LVLX";
        qDebug() << "Send 'Ready'";
        sendMessage("READY\n\n");

        qDebug() << "'Ready' written";
        if(lastMsgSuccess)
        {
            qDebug() << "'Ready' sent";
            IntProc::state="Wait for LVLX data...";
        }
        else
        {
            qDebug() << "Fail to send 'Ready': "
                     << ipServer->errorString();
            IntProc::state="Fail to answer";
        }
    }
    else
    if(in=="PARSE_LVLX")
    {
        qDebug() << "do Parse LVLX: >>"<< in;
        do_parseLevelData=true;
        accepted_lvl = FileFormats::ReadExtendedLvlFile(accepted_lvl_raw, accepted_lvl_path);
        IntProc::state="LVLX is valid: %1";
        IntProc::state = IntProc::state.arg(accepted_lvl.ReadFileValid);
        qDebug()<<"Level data parsed, Valid:" << accepted_lvl.ReadFileValid;
        levelAccepted=true;
    }
    else
    if(in=="OPEN_TEMP_LVLX")
    {
        qDebug() << "do Parse LVLX: >>"<< in;
        do_parseLevelData=true;
        QFile temp(accepted_lvl_raw.remove("\n"));
        if(temp.open(QFile::ReadOnly|QFile::Text))
        {
            QTextStream x(&temp);
            accepted_lvl = FileFormats::ReadExtendedLvlFile(x.readAll(), accepted_lvl_path);
            IntProc::state="LVLX is valid: %1";
            IntProc::state = IntProc::state.arg(accepted_lvl.ReadFileValid);
            qDebug()<<"Level data parsed, Valid:" << accepted_lvl.ReadFileValid;
        }
        else
        {
            qDebug()<<"ERROR: Can't open temp file";
            accepted_lvl.ReadFileValid = false;
        }
        temp.remove();
        levelAccepted=true;
    }
    else
    if(in.startsWith("PLACEITEM: ", Qt::CaseSensitive))
    {
        qDebug() << "Accepted Placing item!";
        in.remove("PLACEITEM: ");
        IntProc::cmd = in;
        IntProc::command = IntProc::PlaceItem;
        IntProc::cmd_accepted=true;
    }
    else
    if(in.startsWith("MSGBOX: ", Qt::CaseSensitive))
    {
        qDebug() << "Accepted Message box";
        in.remove("MSGBOX: ");
        IntProc::cmd = in.replace("\\n", "\n");
        IntProc::command = IntProc::MessageBox;
        IntProc::cmd_accepted=true;
    }
    else
    if(in.startsWith("CHEAT: ", Qt::CaseSensitive))
    {
        qDebug() << "Accepted cheat code";
        in.remove("CHEAT: ");
        IntProc::cmd = in.replace("\\n", "\n");
        IntProc::command = IntProc::Cheat;
        IntProc::cmd_accepted=true;
    }
    else
    if(in=="PING")
    {
        qDebug() << "IN: >>"<< in;
        sendMessage("PONG\n\n");
        if(lastMsgSuccess)
            qDebug() << "sent";
        else
            qDebug() << "error"
                     << ipServer->errorString();

        qDebug()<< "Ping-Pong!";
    }
}


