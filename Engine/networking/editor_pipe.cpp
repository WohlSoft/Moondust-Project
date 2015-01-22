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
        if(clients[i]) clients[i]->close();
    }
    server->close();
    this->wait(1000);
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
        socket.flush();
        //if(!socket.flush())
        //{
        //    qDebug() << "sendToEditor(QString command) fail to send: " << socket.errorString();
        //    return false;
        //}
        socket.waitForBytesWritten(10000);
        QThread::msleep(100);
        socket.close();
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
  server = new QLocalServer();

  QObject::connect(server, SIGNAL(newConnection()), this, SLOT(slotNewConnection()));
  QObject::connect(this, SIGNAL(privateDataReceived(QString)), this, SLOT(slotOnData(QString)));

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
            if(clients[i]->waitForReadyRead(1000))
            {
                QByteArray data = clients[i]->readAll();
                QString acceptedData = QString::fromUtf8(data);
                emit privateDataReceived(QString::fromUtf8(data));

                if(do_acceptLevelData)
                {
                    accepted_lvl_raw.append(acceptedData);
                    if(acceptedData.endsWith("\n\n"))
                        do_acceptLevelData=false;
                }
                else
                if(acceptedData.startsWith("SEND_LVLX: ", Qt::CaseSensitive))
                {
                    qDebug() << acceptedData;

                    acceptedData.remove("SEND_LVLX: ");
                    accepted_lvl_path = acceptedData;
                    do_acceptLevelData=true;

                    qDebug() << "Send 'Ready'";

                    QByteArray toClient = QString("READY\n\n").toUtf8();
                    clients[i]->write(toClient);
                    if(clients[i]->waitForBytesWritten(1000))
                        qDebug() << "'Ready' sent";
                    else
                        qDebug() << "Fail to send 'Ready': "
                                 << clients[i]->errorString();
                }
                else
                if(acceptedData=="PARSE_LVLX\n\n")
                {
                    do_parseLevelData=true;
                    accepted_lvl = FileFormats::ReadExtendedLvlFile(accepted_lvl_raw, accepted_lvl_path);
                    qDebug()<<"Level data parsed, Valid:" << accepted_lvl.ReadFileValid;
                    levelAccepted=true;
                }
                else
                if(acceptedData=="PING\n\n")
                {
                    QByteArray toClient = QString("PONG\n\n").toUtf8();
                    clients[i]->reset();
                    clients[i]->write(toClient);
                    if(clients[i]->waitForBytesWritten(1000))
                        qDebug() << "sent";
                    else
                        qDebug() << "error"
                                 << clients[i]->errorString();

                    qDebug()<< "Ping-Pong!";
                }
            }
        }
    }
    //qDebug() << "Listen " << server->isListening() << LOCAL_SERVER_NAME;
    //goto loop;
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
    qDebug() << clients.size();
}


void EditorPipe::slotOnData(QString data)
{

  if(do_acceptLevelData)
  {
      accepted_lvl = FileFormats::ReadExtendedLvlFile(data, accepted_lvl_path);
      qDebug() << "Accepted level data";
      return;
  }

  QStringList args = data.split('\n');
  foreach(QString c, args)
  {
     emit dataReceived(c);
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
