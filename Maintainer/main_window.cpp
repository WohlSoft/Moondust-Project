#include "main_window.h"
#include "ui_main_window.h"

#include <Music/audiocvt_sox_gui.h>

MaintainerMain::MaintainerMain(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::MaintainerMain)
{
    ui->setupUi(this);
}

MaintainerMain::~MaintainerMain()
{
    delete ui;
}

void MaintainerMain::on_quitOut_clicked()
{
    this->close();
}

void MaintainerMain::on_music_converter_batch_clicked()
{
    AudioCvt_Sox_gui mus(this);
    this->hide();
    mus.exec();
    this->show();
}
