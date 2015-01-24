#include "editor_pipe.h"

#include <QFile>
#include <QStringList>
#include <QtDebug>
#include <QElapsedTimer>
#include <QApplication>

#include "../common_features/app_path.h"

/**
 * @brief EditorPipe::LocalServer
 *  Constructor
 */
EditorPipe::EditorPipe()
{
    this->moveToThread(this);
    qDebug() << "Construct interprocess pipe";
    do_acceptLevelData=false;
    do_parseLevelData=false;

    accepted_lvl_raw="";
    accepted_lvl.ReadFileValid = false;

    levelAccepted=false;
    qRegisterMetaType<QAbstractSocket::SocketState> ("QAbstractSocket::SocketState");
}


/**
 * @brief EditorPipe::~LocalServer
 *  Destructor
 */
EditorPipe::~EditorPipe()
{
    for(int i = 0; i < clients.size(); ++i)
    {
        if(clients[i])
        {
            clients[i]->write(QString("BYE\n\n").toUtf8());
            clients[i]->waitForBytesWritten(1000);
            clients[i]->disconnectFromServer();
            clients[i]->close();
            QLocalSocket *pop=clients[i];
            clients.remove(i);
            delete pop;
        }
    }
    server->close();
    delete server;
    this->wait(1000);
}

void EditorPipe::shut()
{
    for(int i = 0; i < clients.size(); ++i)
    {
        if(clients[i])
        {
            clients[i]->write(QString("BYE\n\n").toUtf8());
            clients[i]->waitForBytesWritten(1000);
            clients[i]->disconnectFromServer();
            clients[i]->close();
            QLocalSocket *pop=clients[i];
            clients.remove(i);
            delete pop;
        }
    }
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
    QLocalSocket socket;

    // Attempt to connect to the LocalServer
    socket.connectToServer("PGEEditor335jh3c3n8g7");

    if(socket.waitForConnected(100))
    {
        qDebug() << "Connected";
        QString str = QString(command);
        QByteArray bytes;
        bytes = str.toUtf8();
        socket.write(bytes);
        socket.waitForBytesWritten(10000);
        socket.flush();
        QThread::msleep(100);
        socket.disconnectFromServer();
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
    server = new QLocalServer;

    qDebug() << "set slots connections to QLocalServer server";
    QObject::connect(server, SIGNAL(newConnection()), this, SLOT(slotNewConnection()));
    QObject::connect(this, SIGNAL(privateDataReceived(QString, QLocalSocket*)), this, SLOT(slotOnData(QString, QLocalSocket*)));

#ifdef Q_OS_UNIX
  // Make sure the temp address file is deleted
  QFile address(QString("/tmp/" LOCAL_SERVER_NAME));
  if(address.exists()){
    address.remove();
  }
#endif

  QString serverName = QString(LOCAL_SERVER_NAME);
  server->listen(serverName);
  while(server->isListening() == false)
  {
    server->listen(serverName);
    msleep(100);
  }
  qDebug() << "Listen " << server->isListening() << serverName;

  exec();

  qDebug()<< "Server closed";
}



/**
 * @brief EditorPipe::exec
 *  Keeps the thread alive. Waits for incomming connections
 */
void EditorPipe::exec()
{
    //loop:
    while(server->isListening())
    {
        msleep(100);
        if(!server->waitForNewConnection(100))
        {
            QString msg = server->errorString();
            if(!msg.isEmpty())
            qDebug() << "exec()" << msg;
        }


        for(int i = 0; i < clients.size(); ++i)
        {
            clients[i]->waitForReadyRead(1000);
            if(!clients[i]->isOpen())
            {
                QLocalSocket *tmp=clients[i];
                clients.remove(i);
                tmp->disconnectFromServer();
                tmp->close();
                delete tmp;
                continue;
            }
        }
    }
}



/**
 * -------
 * SLOTS
 * -------
 */

void EditorPipe::slotNewConnection()
{
    qDebug() << "New connection!";
    clients.push_front(server->nextPendingConnection());

    QObject::connect(clients.last(), SIGNAL(readyRead()), this, SLOT(slotReadClientData()));

    qDebug() << clients.size();
}

void EditorPipe::slotReadClientData()
{
    QLocalSocket* client = (QLocalSocket*)sender();
    client->moveToThread(this);
    QByteArray data = client->readAll();
    QString acceptedData = QString::fromUtf8(data);

    qDebug() << "IN: >>"<< (acceptedData.size()>30 ? QString(acceptedData).remove(30, acceptedData.size()-31) : acceptedData);

    emit privateDataReceived(acceptedData, client);
}


void EditorPipe::slotOnData(QString data, QLocalSocket *client)
{
    unsigned int nc=0;
    foreach(QChar c, data)
    {
        buffer.append(c);

        if(c==QChar('\n'))
            nc++;
        else
            nc=0;

        if(nc==2)
        {
            buffer.resize(buffer.size()-2);
            qDebug() << "icomingData<-- ["<< (buffer.size()>30? QString(buffer).remove(30, buffer.size()-31) : buffer)<<"]";
            emit icomingData(buffer, client);
            buffer.clear();
        }
    }
}

void EditorPipe::icomingData(QString in, QLocalSocket *client)
{
    if(in=="PARSE_LVLX")
    {
        do_acceptLevelData=false;
    }

    if(do_acceptLevelData)
    {
        accepted_lvl_raw.append(in);
        if(in.endsWith("\n\n"))
            do_acceptLevelData=false;
    }
    else
    if(in.startsWith("SEND_LVLX: ", Qt::CaseSensitive))
    {
        qDebug() << "IN: >>"<< (in.size()>30? QString(in).remove(30, in.size()-31) : in);

        in.remove("SEND_LVLX: ");
        accepted_lvl_path = in;
        do_acceptLevelData=true;

        qDebug() << "Send 'Ready'";

        QByteArray toClient = QString("READY\n\n").toUtf8();
        qDebug() << "OUT: <<"<< "READY\n\n";
        client->moveToThread(this);
        client->write(toClient.data(), toClient.length());
        qDebug() << "'Ready' written";
        if(client->waitForBytesWritten(1000))
            qDebug() << "'Ready' sent";
        else
            qDebug() << "Fail to send 'Ready': "
                     << client->errorString();
    }
    else
    if(in=="PARSE_LVLX")
    {
        qDebug() << "do Parse LVLX: >>"<< in;
        do_parseLevelData=true;
        accepted_lvl = FileFormats::ReadExtendedLvlFile(accepted_lvl_raw, accepted_lvl_path);
        qDebug()<<"Level data parsed, Valid:" << accepted_lvl.ReadFileValid;
        levelAccepted=true;
    }
    else
    if(in=="PING")
    {
        qDebug() << "IN: >>"<< in;
        QByteArray toClient = QString("PONG\n\n").toUtf8();
        qDebug() << "OUT: <<"<< "PONG\n\n";
        client->write(toClient.data(), toClient.length());
        if(client->waitForBytesWritten(1000))
            qDebug() << "sent";
        else
            qDebug() << "error"
                     << client->errorString();

        qDebug()<< "Ping-Pong!";
    }
}


void EditorPipe::displayError(QLocalSocket::LocalSocketError socketError)
{
    switch (socketError)
    {
        default:
            qDebug() << QString("The following error occurred: %1.")
                                     .arg(server->errorString());
    }

}
