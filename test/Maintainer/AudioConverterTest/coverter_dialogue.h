#ifndef COVERTER_DIALOGUE_H
#define COVERTER_DIALOGUE_H

#include <QDialog>
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

    MoondustAudioProcessor m_cvt;
    QFuture<void> m_runner;

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
