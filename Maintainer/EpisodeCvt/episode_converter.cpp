#include "episode_converter.h"
#include "ui_episode_converter.h"

EpisodeConverter::EpisodeConverter(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::EpisodeConverter)
{
    ui->setupUi(this);
}

EpisodeConverter::~EpisodeConverter()
{
    delete ui;
}

void EpisodeConverter::on_closeBox_clicked()
{
    this->close();
}

void EpisodeConverter::on_pushButton_clicked()
{

}

void EpisodeConverter::on_DoMadJob_clicked()
{

}
