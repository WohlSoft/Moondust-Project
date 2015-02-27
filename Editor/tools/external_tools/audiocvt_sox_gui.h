#ifndef AUDIOCVT_SOX_GUI_H
#define AUDIOCVT_SOX_GUI_H

#include <QDialog>

namespace Ui {
class AudioCvt_Sox_gui;
}

class AudioCvt_Sox_gui : public QDialog
{
    Q_OBJECT

public:
    explicit AudioCvt_Sox_gui(QWidget *parent = 0);
    ~AudioCvt_Sox_gui();

private:
    Ui::AudioCvt_Sox_gui *ui;
};

#endif // AUDIOCVT_SOX_GUI_H
