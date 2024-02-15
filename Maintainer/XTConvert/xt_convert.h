#ifndef xt_convert_H
#define xt_convert_H

#include <QDialog>
#include <QQueue>
#include <QProcess>
#include <files/episode_box.h>
#include <QFuture>

namespace Ui {
class XTConvert;
}

class XTConvert : public QDialog
{
    Q_OBJECT

public:
    explicit XTConvert(QWidget *parent = 0);
    ~XTConvert();
    void setEnableControls();
    void start();
    void stop(bool do_abort=false);

signals:
    void doNextStep(int retStatus, int exitStatus);
    void gotMax(int max);

private slots:
    void consoleMessage();
    void nextStep_x(int retStatus, int exitStatus);
    void nextStep(int retStatus, QProcess::ExitStatus exitStatus);
    void on_start_clicked();
    void on_browse_asset_pack_clicked();
    void on_browse_episode_clicked();

protected:
    void closeEvent( QCloseEvent * e );

private:
    QFuture<void> fetcher;

    bool inWork;
    int madeJob;
    QProcess converter;
    QQueue<QString > filesToConvert;
    QString current_musFileOld;
    QString current_musFileNew;
    Ui::XTConvert *ui;
    bool isBackUp;

    enum CVTJOBS{
        CVT_LVL=0,
        CVT_EPS,
        CVT_CUSTOM
    };

    CVTJOBS job;

    QString     curSectionMusic;
    QString     lastOutput;
    QStringList curLevelMusic;

    EpisodeBox        episodeBox;
    EpisodeBox_level  episodeBoxLvl;
    EpisodeBox_world  episodeBoxWld;

    //statistics
    void resetStat();
    long stat_converted_files;
    long stat_overwritten_levels;
    long stat_overwritten_worlds;
    long stat_failed_files;
    long stat_failed_levels;
    QStringList fails;
};

#endif // xt_convert_H
