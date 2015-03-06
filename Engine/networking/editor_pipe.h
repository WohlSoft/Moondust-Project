#ifndef EDITOR_PIPE_H
#define EDITOR_PIPE_H

#include <QThread>
#include <QVector>
#include <QLocalServer>
#include <QLocalSocket>

#include <QAbstractSocket>
#include <PGE_File_Formats/file_formats.h>

#define LOCAL_SERVER_NAME "PGEEngine42e3j"

//Q_DECLARE_METATYPE(QAbstractSocket::SocketState)

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
    void dataReceived(QString data);
    void privateDataReceived(QString data, QLocalSocket *client);
    void showUp();
    void openFile(QString path);

private slots:
    void slotNewConnection();
    void slotReadClientData();
    void slotOnData(QString data, QLocalSocket *client);
    void icomingData(QString in, QLocalSocket *client);
    void displayError(QLocalSocket::LocalSocketError socketError);

private:
    QString buffer;
    QLocalServer* server;
    QVector<QLocalSocket*> clients;
};

#endif // EDITOR_PIPE_H
