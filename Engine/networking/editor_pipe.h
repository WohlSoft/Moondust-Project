#ifndef EDITOR_PIPE_H
#define EDITOR_PIPE_H

#include <QThread>
#include <QVector>
#include <mutex>
#include <PGE_File_Formats/file_formats.h>

class EditorPipe_std : public QThread
{
    Q_OBJECT
public:
    EditorPipe_std();
    void run();
signals:
    void msg(QString m);
public slots:
    void sendMessage(QString msg);
private:
    QTextStream m_input;
    QTextStream m_output;
};

class EditorPipe : public QObject
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
    bool        m_levelAccepted;
    std::mutex  m_levelAccepted_lock;

signals:
    void sendMessage(QString msg);
    void openFile(QString path);

private slots:
    void icomingData(QString in);

private:
    EditorPipe_std msgr;
};

#endif // EDITOR_PIPE_H
