#ifndef AUDIOCVT_SOX_GUI_H
#define AUDIOCVT_SOX_GUI_H

#include <QDialog>
#include <QQueue>
#include <QProcess>
#include <files/episode_box.h>

namespace Ui {
class AudioCvt_Sox_gui;
}

class AudioCvt_Sox_gui : public QDialog
{
    Q_OBJECT

public:
    explicit AudioCvt_Sox_gui(QWidget *parent = 0);
    ~AudioCvt_Sox_gui();
    void setEnableControls(bool en);
    void start();
    void stop(bool do_abort=false);

private slots:
    void consoleMessage();
    void consoleMessageErr();
    void nextStep(int retStatus, QProcess::ExitStatus exitStatus);
    void on_browse_clicked();
    void on_add_clicked();
    void on_remove_clicked();
    void on_start_clicked();
    void on_closeME_clicked();
    void on_cvt_lvlfile_browse_clicked();
    void on_cvt_episode_browse_clicked();

protected:
    void closeEvent( QCloseEvent * e );

private:
    bool formatSupports(QString file);
    bool inWork;
    int madeJob;
    QProcess converter;
    QQueue<QString > filesToConvert;
    QString current_musFileOld;
    QString current_musFileNew;
    Ui::AudioCvt_Sox_gui *ui;
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
    EpisodeBox_level  episodeBoxWld;

    //statistics
    void resetStat();
    long stat_converted_files;
    long stat_overwritten_levels;
    long stat_overwritten_worlds;
    long stat_failed_levels;
};

#endif // AUDIOCVT_SOX_GUI_H
