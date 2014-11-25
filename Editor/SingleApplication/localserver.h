#ifndef LOCALSERVER_H
#define LOCALSERVER_H

#include <QThread>
#include <QVector>
#include <QLocalServer>
#include <QLocalSocket>

#define LOCAL_SERVER_NAME "PGEEditor335jh3c3n8g7"

class LocalServer : public QThread
{
  Q_OBJECT
public:
  LocalServer();
  ~LocalServer();
  void shut();

protected:
  void run();
  void exec();

signals:
  void dataReceived(QString data);
  void privateDataReceived(QString data);
  void showUp();
  void openFile(QString path);
  void acceptedCommand(QString cmd);

private slots:
  void slotNewConnection();
  void slotOnData(QString data);

private:
  QLocalServer* server;
  QVector<QLocalSocket*> clients;
  void onCMD(QString data);

};

#endif // LOCALSERVER_H
