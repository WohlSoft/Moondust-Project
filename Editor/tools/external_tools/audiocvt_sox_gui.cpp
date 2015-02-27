#include "audiocvt_sox_gui.h"
#include "ui_audiocvt_sox_gui.h"

AudioCvt_Sox_gui::AudioCvt_Sox_gui(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AudioCvt_Sox_gui)
{
    ui->setupUi(this);
}

AudioCvt_Sox_gui::~AudioCvt_Sox_gui()
{
    delete ui;
}
