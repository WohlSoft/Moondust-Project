#ifndef ENGINE_CLIENT_H
#define ENGINE_CLIENT_H

#include <QThread>
#include <QVector>
#include <QLocalServer>
#include <QLocalSocket>

#include "../file_formats/file_formats.h"

#define ENGINE_SERVER_NAME "PGEEngine42e3j"

class EngineClient : public QThread
{
    Q_OBJECT
public:
    EngineClient();
    ~EngineClient();

    void sendLevelData(LevelData _data);
    void sendCommand(QString command);
    void OpenConnection();
    bool isConnected();

protected:
    void run();
    void exec();

signals:
    void dataReceived(QString data);
    void privateDataReceived(QString data);
    void showUp();
    void openFile(QString path);

private slots:
    void slotOnData(QString data);
    void displayError(QLocalSocket::LocalSocketError socketError);

private:
    QLocalSocket* engine;
    bool _connected;
    bool readyToSendLvlx;
};

#endif // ENGINE_CLIENT_H
