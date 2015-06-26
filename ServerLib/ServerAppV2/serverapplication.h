#ifndef SERVER_H
#define SERVER_H

#include <QWidget>
#include <QTcpServer>

#include "PGEtcpserver.h"

namespace Ui {
class ServerApplicationUi;
}

class ServerApplication : public QWidget
{
    Q_OBJECT

public:
    explicit ServerApplication(PgeTcpServer *aServer, QWidget *parent = 0);
    ~ServerApplication();

public slots:
    bool startServer();
    void changePort();

private:
    Ui::ServerApplicationUi *ui;

    PgeTcpServer *theServer;
};

#endif // SERVER_H
