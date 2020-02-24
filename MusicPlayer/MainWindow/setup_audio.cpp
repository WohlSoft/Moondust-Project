#include <QMessageBox>

#include "setup_audio.h"
#include "ui_setup_audio.h"

#include "../Player/mus_player.h"

SetupAudio::SetupAudio(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SetupAudio)
{
    ui->setupUi(this);
    ui->sampleFormat->addItem("U8", AUDIO_U8);
    ui->sampleFormat->addItem("S8", AUDIO_S8);
    ui->sampleFormat->addItem("U16MSB", AUDIO_U16MSB);
    ui->sampleFormat->addItem("S16MSB", AUDIO_S16MSB);
    ui->sampleFormat->addItem("U16LSB", AUDIO_U16LSB);
    ui->sampleFormat->addItem("S16LSB", AUDIO_S16LSB);
    ui->sampleFormat->addItem("S32MSB", AUDIO_S32MSB);
    ui->sampleFormat->addItem("S32LSB", AUDIO_S32LSB);
    ui->sampleFormat->addItem("F32MSB", AUDIO_F32MSB);
    ui->sampleFormat->addItem("F32LSB", AUDIO_F32LSB);

    ui->sampleRate->setValue(PGE_MusicPlayer::getSampleRate());
    int i = ui->sampleFormat->findData(PGE_MusicPlayer::getSampleFormat());
    ui->sampleFormat->setCurrentIndex(i);
    ui->channels->setValue(PGE_MusicPlayer::getChannels());
}

SetupAudio::~SetupAudio()
{
    delete ui;
}

void SetupAudio::on_confirm_accepted()
{
    QString error;
    int rate = ui->sampleRate->value();
    Uint16 format = static_cast<Uint16>(ui->sampleFormat->currentData().toUInt());
    int channels = ui->channels->value();

    PGE_MusicPlayer::closeAudio();

    if(!PGE_MusicPlayer::openAudioWithSpec(error, rate, format, channels))
    {
        QMessageBox::warning(this, tr("Audio loading error"), tr("Failed to load audio: %1").arg(error));
        if(!PGE_MusicPlayer::openAudio(error))
            QMessageBox::warning(this, tr("Audio loading error"), tr("Failed to re-load audio: %1").arg(error));
        return;
    }

    PGE_MusicPlayer::setSpec(rate, format, channels);
    PGE_MusicPlayer::saveAudioSettings();
    accept();
}

void SetupAudio::on_restoreDefaults_clicked()
{
    ui->sampleRate->setValue(MIX_DEFAULT_FREQUENCY);
    ui->channels->setValue(MIX_DEFAULT_CHANNELS);
    int i = ui->sampleFormat->findData(MIX_DEFAULT_FORMAT);
    ui->sampleFormat->setCurrentIndex(i);
}
