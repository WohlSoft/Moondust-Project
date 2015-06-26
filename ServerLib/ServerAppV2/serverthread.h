#ifndef CLIENTTHREAD_H
#define CLIENTTHREAD_H

#include <QObject>
#include <QTcpSocket>
#include <QThread>
#include <QHostAddress>

class ServerThread : public QThread
{
    Q_OBJECT
public:
    ServerThread(QTcpSocket *clientSocket, QObject *parent = 0);
    virtual ~ServerThread();

    void writeToClient(QByteArray bytes);
    QByteArray getMessage();

signals:
    void outputSent();

protected:
    void run();

private:
    QTcpSocket *clientSocket;
    QByteArray message;

private slots:
    void displayMessage();
    void deleteThread();
};

#endif // CLIENTTHREAD_H
