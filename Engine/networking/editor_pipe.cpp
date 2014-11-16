#include "editor_pipe.h"

#include <QFile>
#include <QStringList>
#include <QtDebug>

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
}

/**
 * @brief EditorPipe::~LocalServer
 *  Destructor
 */
EditorPipe::~EditorPipe()
{
  server->close();
  for(int i = 0; i < clients.size(); ++i)
  {
    clients[i]->close();
  }
}

bool EditorPipe::levelIsLoad()
{
    bool state = levelAccepted;
    levelAccepted=false;
    return state;
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
  while(server->isListening() == false){
    server->listen(serverName);
    msleep(100);
  }
  exec();
}

/**
 * @brief EditorPipe::exec
 *  Keeps the thread alive. Waits for incomming connections
 */
void EditorPipe::exec()
{
  while(server->isListening())
  {
    msleep(100);
    server->waitForNewConnection(100);
    for(int i = 0; i < clients.size(); ++i)
    {
      if(clients[i]->waitForReadyRead(100))
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
        if(acceptedData.startsWith("SEND_LVLX:", Qt::CaseSensitive))
        {
            acceptedData.remove("SEND_LVLX: ");
            accepted_lvl_path = acceptedData;
            do_acceptLevelData=true;
            QByteArray toClient = QString("READY\n\n").toUtf8();
            clients[i]->write(toClient);
        }
        else
        if(acceptedData=="PARSE_LVLX\n\n")
        {
            do_parseLevelData=true;
            accepted_lvl = FileFormats::ReadExtendedLvlFile(data, accepted_lvl_path);
            levelAccepted=true;
        }

      }
      else
      {
          QLocalSocket* tmp = clients[i];
          clients.removeAt(i);
          delete tmp;
          i--;
      }
    }
  }
}

/**
 * -------
 * SLOTS
 * -------
 */

/**
 * @brief EditorPipe::slotNewConnection
 *  Executed when a new connection is available
 */
void EditorPipe::slotNewConnection()
{
  clients.push_front(server->nextPendingConnection());
}

/**
 * @brief EditorPipe::slotOnData
 *  Executed when data is received
 * @param data
 */
void EditorPipe::slotOnData(QString data)
{

  if(do_acceptLevelData)
  {
      accepted_lvl = FileFormats::ReadExtendedLvlFile(data, accepted_lvl_path);
      return;
  }

  QStringList args = data.split('\n');
  foreach(QString c, args)
  {
     emit dataReceived(c);
  }
}

