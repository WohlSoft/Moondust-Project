#ifndef EDITOR_PIPE_H
#define EDITOR_PIPE_H

#include <QThread>
#include <QVector>
#include <QTcpServer>
#include <QTcpSocket>
#include <QLocalSocket>

#include <QAbstractSocket>
#include <PGE_File_Formats/file_formats.h>

#define LOCAL_SERVER_NAME "PGEEngine42e3j"

//Q_DECLARE_METATYPE(QAbstractSocket::SocketState)

class IntProcServer  : public QTcpServer
{
    Q_OBJECT
public:
    explicit IntProcServer();
    ~IntProcServer();
public slots:
    void writeMessage(QString msg);
    void stateChanged(QAbstractSocket::SocketState stat);
signals:
    void messageIn(QString msg);

protected slots:
    void readData();
    void handleNewConnection();
    void clientDisconnected();
    void displayError(QAbstractSocket::SocketError socketError);

private:
    QTcpSocket *clientConnection;
};



class EditorPipe : public QThread
{
    Q_OBJECT
public:
    EditorPipe();
    ~EditorPipe();
    void shut();
    bool isWorking;

                               // SEND_LVLX: /some/path/to/level file\n\n
    QString accepted_lvl_path; // Send to client the "READY\n\n" before accent raw data
    bool do_acceptLevelData;
    QString accepted_lvl_raw;  // accept any raw data before will be accepted '\n\n'
    bool do_parseLevelData;
    LevelData accepted_lvl;    // When accepted PARSE_LVLX\n\n, parse data and call signal
    bool levelIsLoad();
    bool sendToEditor(QString command);

private:
    bool levelAccepted;

protected:
    void run();
    void exec();

signals:
    void sendMessage(QString msg);
    //void dataReceived(QString data);
    //void privateDataReceived(QString data, QTcpSocket *client);
    //void showUp();
    void openFile(QString path);

private slots:
    void slotOnData(QString data);
    void icomingData(QString in);

private:
    bool lastMsgSuccess;
    QString buffer;
};

#endif // EDITOR_PIPE_H
