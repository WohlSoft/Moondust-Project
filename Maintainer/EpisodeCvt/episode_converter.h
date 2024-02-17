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

    bool                m_doBackup = true;
    int                 m_targetFormat = 0;
    int                 m_targetFormatVer = 64;
    int                 m_targetFormatWld = 0;
    int                 m_targetFormatVerWld = 64;
    bool                m_applyTheXTechSetup = false;

    QString             m_dstCpId;

    EpisodeBox          m_episodeBox;
    QAtomicInteger<bool>m_jobRunning;
public:
    QAtomicInteger<int> m_currentValue;
    bool                m_isFine;

    explicit EpisodeConverterWorker(QObject*parent = nullptr);
    ~EpisodeConverterWorker();

    void setBackup(bool enabled);

    struct JobSetup
    {
        QString path;
        bool recursive;
        int targetFormat;
        int fmtVer;
        int targetFormatWld;
        int fmtVerWld;
        QString cpId;
        bool applyTheXTechSetup;
    };

    bool initJob(const JobSetup &setup);
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

    struct EpisodeEnginePreset
    {
        QString engineName;
        QString cpIdTag;
        int comboBoxIdx;
        int levelFormat;
        int levelFormatVersion;
        int worldFormat;
        int worldFormatVersion;
        bool showTheXTechOptions;
    };

    QVector<EpisodeEnginePreset> m_enginesList;
    void buildEnginesList();

    struct EpisodeStats
    {
        bool mixed = false;
        bool allPGEX = false;
        bool all38A = false;
        bool allSMBX64 = false;
        unsigned int max38Aver = 0;
        unsigned int max64ver = 0;
    } m_episodeStats;

public:
    explicit EpisodeConverter(QWidget *parent = nullptr);
    ~EpisodeConverter();

private slots:
    void targetEngineSelected(int idx);
    void detectEpisodeFormat();

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
