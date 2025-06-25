#ifndef COVERTER_DIALOGUE_H
#define COVERTER_DIALOGUE_H

#include <QDialog>
#include <QSettings>
#include <QFuture>

#include "audio_processor.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class CoverterDialogue;
}
QT_END_NAMESPACE

class CoverterDialogue : public QDialog
{
    Q_OBJECT

    QSettings m_setup;
    MoondustAudioProcessor m_cvt;
    MDAudioFileSpec m_dstSpec;
    QFuture<void> m_runner;

    enum Phase
    {
        PHASE_IDLE = 0,
        PHASE_LENGHT_MEASURE,
        PHASE_CONVERSION
    };

    int m_phase = PHASE_IDLE;

public:
    CoverterDialogue(QWidget *parent = nullptr);
    ~CoverterDialogue();

signals:
    void setProgressSize(int size);
    void updateProgress(int value);
    void workStarted();
    void workFinished();

private slots:
    void on_runCvt_clicked();
    void workStartedRun();
    void workFinishedRun();

private:
    Ui::CoverterDialogue *ui;
    void runner();
};
#endif // COVERTER_DIALOGUE_H
