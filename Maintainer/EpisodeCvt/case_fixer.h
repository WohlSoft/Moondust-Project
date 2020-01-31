#ifndef CASE_FIXER_H
#define CASE_FIXER_H

#include <QDialog>
#include <QTimer>
#include <QFuture>
#include <QDir>
#include <QQueue>
#include <QAtomicInteger>

#include <files/episode_box.h>

namespace Ui {
class CaseFixer;
}

class CaseFixer;
class CaseFixerWorker : public QObject
{
    Q_OBJECT
    QDir                m_episode;
    QString             m_errorString;
    QQueue<QString>     m_filesToConvert;
    int                 m_mode;
    EpisodeBox          m_episodeBox;
    QAtomicInteger<bool>m_jobRunning;

public:
    QAtomicInteger<int> m_currentValue;
    bool                m_isFine;

    enum Mode
    {
        MODE_TOLOWER = 0,
        MODE_MATCH_FS,
        MODE_MATCH_SETUP
    };

    explicit CaseFixerWorker(QObject*parent = nullptr);
    ~CaseFixerWorker();

    bool initJob(QString configPack,
                 QString episodePath,
                 bool recursive,
                 int targetMode);
    bool runJob();

    QString errorString();

signals:
    void statusMessage(const QString &msg);
    void workFinished(bool isFine);
    void totalElements(int max);
};

class CaseFixer : public QDialog
{
    Q_OBJECT

public:
    explicit CaseFixer(QWidget *parent = nullptr);
    ~CaseFixer();

private slots:
    void on_start_clicked();

private slots:
    void on_episodeBrowse_clicked();
    void on_configPackBrowse_clicked();
    void workFinished(bool isFine);
    void refreshProgressBar();

signals:
    void setLocked(bool);

private:
    Ui::CaseFixer *ui;
    QFuture<bool>   m_process;
    CaseFixerWorker m_worker;
    QTimer          m_progressWatcher;
    void loadSetup();
    void saveSetup();
};

#endif // CASE_FIXER_H
