#include "serverapplication.h"
#include "PGEtcpserver.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    PgeTcpServer *pge = new PgeTcpServer(2222);
    ServerApplication server(pge);
    server.show();

    return a.exec();
}
