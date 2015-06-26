#ifndef CLIENTCONNECTION_H
#define CLIENTCONNECTION_H

#include <QObject>
#include <QTcpSocket>

class ClientConnection : public QObject
{
    Q_OBJECT
public:
    explicit ClientConnection(QTcpSocket *socket, QObject *parent = 0);

    void connectToServer(QString hostname, int port);
    void disconnectFromServer();

    void sendData(QByteArray bytes);

signals:
    void messageReceived(QByteArray bytes);

private:
    QTcpSocket *connectionToServer;

private slots:
    void messageReceived();
};

#endif // CLIENTCONNECTION_H
