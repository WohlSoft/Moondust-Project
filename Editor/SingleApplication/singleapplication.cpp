#include "singleapplication.h"
#include <QtDebug>

/**
 * @brief SingleApplication::SingleApplication
 *  Constructor. Checks and fires up LocalServer or closes the program
 *  if another instance already exists
 * @param argc
 * @param argv
 */
SingleApplication::SingleApplication(int argc, char *argv[])
{
  _shouldContinue = false; // By default this is not the main process

  socket = new QLocalSocket();

  // Attempt to connect to the LocalServer
  socket->connectToServer(LOCAL_SERVER_NAME);
  if(socket->waitForConnected(100))
  {

    QString str = QString("CMD:showUp");
    QByteArray bytes;

    for(int i=1;i<argc; i++)
    {
       str.append(QString("\n%1").arg(QString::fromLocal8Bit(argv[i])));
    }
    bytes = str.toUtf8();
    socket->write(bytes);
    socket->flush();
    QThread::msleep(100);
    socket->close();
  }
  else
  {
    // The attempt was insuccessful, so we continue the program
    _shouldContinue = true;
    server = new LocalServer();
    server->start();
    QObject::connect(server, SIGNAL(showUp()), this, SLOT(slotShowUp()));
    QObject::connect(server, SIGNAL(dataReceived(QString)), this, SLOT(slotOpenFile(QString)));
    QObject::connect(server, SIGNAL(acceptedCommand(QString)), this, SLOT(slotAcceptedCommand(QString)));
  }
}

/**
 * @brief SingleApplication::~SingleApplication
 *  Destructor
 */
SingleApplication::~SingleApplication()
{
  if(_shouldContinue){
    server->terminate();
  }
}

/**
 * @brief SingleApplication::shouldContinue
 *  Weather the program should be terminated
 * @return bool
 */
bool SingleApplication::shouldContinue()
{
  return _shouldContinue;
}

/**
 * @brief SingleApplication::slotShowUp
 *  Executed when the showUp command is sent to LocalServer
 */
void SingleApplication::slotShowUp()
{
  emit showUp();
}

void SingleApplication::slotOpenFile(QString path)
{
    emit openFile(path);
}

void SingleApplication::slotAcceptedCommand(QString cmd)
{
    emit acceptedCommand(cmd);
}

