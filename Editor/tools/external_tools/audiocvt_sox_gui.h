#ifndef AUDIOCVT_SOX_GUI_H
#define AUDIOCVT_SOX_GUI_H

#include <QDialog>
#include <QQueue>
#include <QProcess>

class LevelEdit;
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

    void nexStep(int retStatus, QProcess::ExitStatus exitStatus);
    void on_browse_clicked();

    void on_add_clicked();

    void on_remove_clicked();

    void on_start_clicked();
protected:
    void closeEvent( QCloseEvent * e );

private:
    bool inWork;
    int madeJob;
    QProcess converter;
    QQueue<QString > filesToConvert;
    QString current_musFileOld;
    QString current_musFileNew;
    Ui::AudioCvt_Sox_gui *ui;
    bool isBackUp;

    QString     curSectionMusic;
    QString     lastOutput;
    QStringList curLevelMusic;

    bool isLevel;
    LevelEdit *ledit;
};

#endif // AUDIOCVT_SOX_GUI_H
