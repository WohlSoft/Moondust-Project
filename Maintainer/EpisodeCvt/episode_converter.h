#ifndef EPISODE_CONVERTER_H
#define EPISODE_CONVERTER_H

#include <QDialog>
#include <QDir>
#include <QQueue>
#include <QFuture>
#include <QAtomicInteger>
#include <QTimer>

#include <files/episode_box.h>

namespace Ui {
class EpisodeConverter;
}

class EpisodeConverter;
class EpisodeConverterWorker : public QObject
{
    Q_OBJECT
    QDir                m_episode;
    QString             m_errorString;
    QQueue<QString >    m_filesToConvert;
    bool                m_doBackup;
    int                 m_targetFormat;
    int                 m_targetFormatVer;
    EpisodeBox          m_episodeBox;
    QAtomicInteger<bool>m_jobRunning;
public:
    QAtomicInteger<int> m_currentValue;
    bool                m_isFine;

    explicit EpisodeConverterWorker(QObject*parent = nullptr);
    ~EpisodeConverterWorker();

    void setBackup(bool enabled);
    bool initJob(QString m_path, bool recursive, int targetFormat, int fmtVer);
    bool runJob();

    QString errorString();
signals:
    void workFinished(bool isFine);
    void totalElements(int max);
};

class EpisodeConverter : public QDialog
{
    Q_OBJECT
    friend class EpisodeConverterWorker;
public:
    explicit EpisodeConverter(QWidget *parent = nullptr);
    ~EpisodeConverter();

private slots:
    void on_closeBox_clicked();
    void on_DoMadJob_clicked();
    void on_browse_clicked();
    void workFinished(bool isFine);
    void refreshProgressBar();

protected:
    void closeEvent(QCloseEvent *cl);

signals:
    void setLocked(bool);

private:
    Ui::EpisodeConverter  *ui;
    EpisodeConverterWorker m_worker;
    QFuture<bool>          m_process;
    QTimer                 m_progressWatcher;
};

#endif // EPISODE_CONVERTER_H
