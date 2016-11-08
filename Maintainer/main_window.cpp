#include "main_window.h"
#include "ui_main_window.h"

#include <Music/audiocvt_sox_gui.h>
#include <EpisodeCvt/episode_converter.h>
#include <QMessageBox>

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
    AudioCvt_Sox_gui mus(nullptr);
    mus.setWindowFlags(Qt::Window | Qt::WindowSystemMenuHint | Qt::WindowTitleHint | Qt::WindowCloseButtonHint | Qt::WindowMinimizeButtonHint);
    mus.setWindowModality(Qt::NonModal);
    this->hide();
    mus.exec();
    this->show();
}

void MaintainerMain::on_episode_converter_clicked()
{
    EpisodeConverter eps(nullptr);
    eps.setWindowFlags(Qt::Window | Qt::WindowSystemMenuHint | Qt::WindowTitleHint | Qt::WindowCloseButtonHint | Qt::WindowMinimizeButtonHint);
    eps.setWindowModality(Qt::NonModal);
    this->hide();
    eps.exec();
    this->show();
}

void MaintainerMain::on_gifs2png_converter_clicked()
{
    QMessageBox::information(this, "WIP", "This feature is in works!");
}

void MaintainerMain::on_png2gifs_converter_clicked()
{
    QMessageBox::information(this, "WIP", "This feature is in works!");
}

void MaintainerMain::on_pathcase_fixer_clicked()
{
    QMessageBox::information(this, "WIP", "This feature is in works!");
}
