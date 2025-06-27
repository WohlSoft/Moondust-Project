/*
 * Moondust, a free game engine for platform game making
 * Copyright (c) 2014-2025 Vitaly Novichkov <admin@wohlnet.ru>
 *
 * This software is licensed under a dual license system (MIT or GPL version 3 or later).
 * This means you are free to choose with which of both licenses (MIT or GPL version 3 or later)
 * you want to use this software.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *
 * You can see text of MIT license in the LICENSE.mit file you can see in Engine folder,
 * or see https://mit-license.org/.
 *
 * You can see text of GPLv3 license in the LICENSE.gpl3 file you can see in Engine folder,
 * or see <http://www.gnu.org/licenses/>.
 */

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
