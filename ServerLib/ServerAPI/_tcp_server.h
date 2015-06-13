#ifndef _TCP_SERVER_H
#define _TCP_SERVER_H

#include <QTcpServer>
#include <QTcpSocket>
#include <QHash>
#include <QPair>

typedef QPair<QTcpSocket *, qint64 > ClientPing; //!< Client pointer, timeout

class PGE_TcpServer : public QTcpServer
{
    Q_OBJECT
public:
    explicit PGE_TcpServer();
    ~PGE_TcpServer();
public slots:
    void writeMessage(QString msg, QString clientID);
    void stateChanged(QAbstractSocket::SocketState stat);
    void sendMessageToAllClients(QString msg);
    void stop();
signals:
    void messageIn(QString msg);

protected slots:
    void readData();
    void handleNewConnection();
    void clientDisconnected();
    void displayError(QAbstractSocket::SocketError socketError);
    void doPingPong();

private:
    QList<QTcpSocket *>          clientConnections;     //!< Total list of clients

    QList<QTcpSocket *>          unregistredClients;    //!< Anonymouse guys, hasn't passport!
    QHash<QString, QTcpSocket *> registredClients;      //!< our citizens, have passport!
    QList<ClientPing >          waitsPong;             /*< Let's play ping-pong! Who will miss ball,
                                                             will be kicked out from client list!*/
};

#endif // _TCP_SERVER_H
