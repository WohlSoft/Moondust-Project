#ifndef APPLICATION_H
#define APPLICATION_H

#include "localserver.h"

#include <QApplication>
#include <QLocalSocket>

/**
 * @brief The Application class handles trivial application initialization procedures
 */
class SingleApplication : public QApplication
{
  Q_OBJECT
public:
  explicit SingleApplication(int, char *[]);
  ~SingleApplication();
  bool shouldContinue();

public slots:

signals:
  void showUp();
  void openFile(QString path);

private slots:
  void slotShowUp();
  void slotOpenFile(QString path);
  
private:
  QLocalSocket* socket;
  LocalServer* server;
  bool _shouldContinue;

};

#endif // APPLICATION_H
