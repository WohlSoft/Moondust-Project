/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2014-2026 Vitaly Novichkov <admin@wohlnet.ru>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef AUDIOCVT_SOX_GUI_H
#define AUDIOCVT_SOX_GUI_H

#include <QDialog>
#include <QQueue>
#include <QProcess>
#include <files/episode_box.h>
#include <QFuture>

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

signals:
    void doNextStep(int retStatus, int exitStatus);
    void gotMax(int max);

private slots:
    void consoleMessage();
    void nextStep_x(int retStatus, int exitStatus);
    void nextStep(int retStatus, QProcess::ExitStatus exitStatus);
    void on_browse_clicked();
    void on_add_clicked();
    void on_remove_clicked();
    void on_start_clicked();
    void on_closeME_clicked();
    void on_cvt_lvlfile_browse_clicked();
    void on_cvt_episode_browse_clicked();

    void on_browseFFMPEG_clicked();

protected:
    void closeEvent( QCloseEvent * e );

private:
    void CollectEpisodeData();
    void ProcessRenameMusic();
    QFuture<void> fetcher;

    bool isFileTypeSupported(QString file);
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

#endif // AUDIOCVT_SOX_GUI_H
