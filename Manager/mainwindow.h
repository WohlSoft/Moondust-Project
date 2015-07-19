#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <http_downloader/http_downloader.h>
#include <QQueue>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    HttpDownloader downloader;
    QString tempDir;

    enum ActionQueueEnum
    {
        ACT_DOWNLOAD_FILE=0,
        ACT_PARSE_XML,
        ACT_UNPACK_ZIP,
        ACT_LOCK_CONFIG_PAGE,
        ACT_UNLOCK_CONFIG_PAGE,
        ACT_SHOWMSG
    };

    struct ActionQueueItem
    {
        ActionQueueEnum type;
        QString param1;
        QString param2;
        QString param3;
        QString param4;
    };

    QQueue<ActionQueueItem > queue;
    ActionQueueItem          currentAct;
    int curstep;
    int totalSteps;

private slots:
    void on_actionRepositories_triggered();
    void on_refresh_clicked();
    void updateConfigPacksList();
    void on_actionExit_triggered();

    void queueStepBegin();
    void downloadSuccess();
    void downloadAborted();
    void downloadFailed(QString reason);
    void setProgress(qint64 bytesRead, qint64 totalBytes);

protected:
    void closeEvent(QCloseEvent* e);

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
