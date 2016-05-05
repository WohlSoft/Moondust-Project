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
