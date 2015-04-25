#ifndef APPLICATION_H
#define APPLICATION_H

#include <QObject>
#include <QUdpSocket>

#include "localserver.h"

/**
 * @brief The Application class handles trivial application initialization procedures
 */
class SingleApplication : public QObject
{
  Q_OBJECT
public:
  explicit SingleApplication(int, char *[]);
  ~SingleApplication();
  bool shouldContinue();

public slots:

signals:
  void showUp();
  void stopServer();
  void openFile(QString path);
  void acceptedCommand(QString cmd);

private slots:
  void slotShowUp();
  void slotOpenFile(QString path);
  void slotAcceptedCommand(QString cmd);
  
private:
  QUdpSocket* socket;
  LocalServer* server;
  bool _shouldContinue;

};

#endif // APPLICATION_H
