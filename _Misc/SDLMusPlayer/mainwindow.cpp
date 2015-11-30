#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <math.h>

#include <QtDebug>
#include <QFileDialog>
#include <QMessageBox>

#undef main
#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer_ext.h>
#undef main

namespace PGE_MusicPlayer
{

static void error(QString msg)
{
    QMessageBox::warning(nullptr, "SDL2 Mixer ext error", msg, QMessageBox::Ok);
}

Mix_Music *play_mus = NULL;
static Mix_MusicType type;

    void MUS_stopMusic()
    {
        Mix_HaltMusic();
    }
    QString MUS_getMusTitle()
    {
        if(play_mus)
            return QString(Mix_GetMusicTitle(play_mus));
        else
            return QString("[No music]");
    }
    QString MUS_getMusArtist()
    {
        if(play_mus)
            return QString(Mix_GetMusicArtistTag(play_mus));
        else
            return QString("[Unknown Artist]");
    }
    QString MUS_getMusAlbum()
    {
        if(play_mus)
            return QString(Mix_GetMusicAlbumTag(play_mus));
        else
            return QString("[Unknown Album]");
    }
    QString MUS_getMusCopy()
    {
        if(play_mus)
            return QString(Mix_GetMusicCopyrightTag(play_mus));
        else
            return QString("");
    }

    void MUS_playMusic()
    {
        if(play_mus)
        {
            if(Mix_PlayMusic(play_mus, -1)==-1)
            {
                error(QString("Mix_PlayMusic: %1").arg(Mix_GetError()));
                // well, there's no music, but most games don't break without music...
            }
            //QString("Music is %1\n").arg(Mix_PlayingMusic()==1?"Playing":"Silence");
        }
        else
        {
            error(QString("Play nothing: Mix_PlayMusic: %1").arg(Mix_GetError()));
        }
        //qDebug() << QString("Check Error of SDL: %1\n").arg(Mix_GetError());
    }

    void MUS_changeVolume(int volume)
    {
        Mix_VolumeMusic(volume);
        qDebug() << QString("Mix_VolumeMusic: %1\n").arg(volume);
    }

    bool MUS_openFile(QString musFile)
    {
        if(play_mus!=NULL) {Mix_FreeMusic(play_mus);play_mus=NULL;}
        play_mus = Mix_LoadMUS( musFile.toUtf8() );
        if(!play_mus) {
            error(QString("Mix_LoadMUS(\"%1\"): %2").arg(musFile).arg(Mix_GetError()));
            return false;
        }

                type=Mix_GetMusicType(play_mus);
        qDebug() << QString("Music type: %1").arg(
                type==MUS_NONE?"MUS_NONE":
                type==MUS_CMD?"MUS_CMD":
                type==MUS_WAV?"MUS_WAV":
                /*type==MUS_MOD_MODPLUG?"MUS_MOD_MODPLUG":*/
                type==MUS_MOD?"MUS_MOD":
                type==MUS_MID?"MUS_MID":
                type==MUS_OGG?"MUS_OGG":
                type==MUS_MP3?"MUS_MP3":
                type==MUS_MP3_MAD?"MUS_MP3_MAD":
                type==MUS_FLAC?"MUS_FLAC":
                type==MUS_SPC?"MUS_SPC":
                "Unknown");
        return true;
    }
}


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->adlmidi_xtra->setVisible(false);
    ui->midi_setup->setVisible(false);
    ui->gme_setup->setVisible(false);
    ui->gridLayout->setSizeConstraint(QLayout::SetFixedSize);
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::dragEnterEvent(QDragEnterEvent *e)
{
    if (e->mimeData()->hasUrls()) {
        e->acceptProposedAction();
    }
}

void MainWindow::openMusicByArg(QString musPath)
{
    currentMusic=musPath;
    on_play_clicked();
}

void MainWindow::dropEvent(QDropEvent *e)
{
    this->raise();
    this->setFocus(Qt::ActiveWindowFocusReason);

    foreach (const QUrl &url, e->mimeData()->urls()) {
        const QString &fileName = url.toLocalFile();
        currentMusic=fileName;
    }
    on_play_clicked();
    this->raise();
}

void MainWindow::on_open_clicked()
{
    QString file = QFileDialog::getOpenFileName(this, tr("Open music file"),
                                                 (currentMusic.isEmpty() ? QApplication::applicationDirPath() : currentMusic), "All (*.*)");
    if(file.isEmpty()) return;
    currentMusic=file;
    on_play_clicked();
}

void MainWindow::on_stop_clicked()
{
    PGE_MusicPlayer::MUS_stopMusic();
}

void MainWindow::on_play_clicked()
{
    if(PGE_MusicPlayer::MUS_openFile(currentMusic+"|"+ui->trackID->text()))
    {
        PGE_MusicPlayer::MUS_changeVolume(128);
        PGE_MusicPlayer::MUS_playMusic();
    }
    ui->musTitle->setText(PGE_MusicPlayer::MUS_getMusTitle());
    ui->musArtist->setText(PGE_MusicPlayer::MUS_getMusArtist());
    ui->musAlbum->setText(PGE_MusicPlayer::MUS_getMusAlbum());
    ui->musCopyright->setText(PGE_MusicPlayer::MUS_getMusCopy());
    switch(PGE_MusicPlayer::type)
    {
        case MUS_MID:
            ui->adlmidi_xtra->setVisible(ui->mididevice->currentIndex()==0);
            ui->midi_setup->setVisible(true);
            ui->gme_setup->setVisible(false);
            break;
        case MUS_SPC:
            ui->adlmidi_xtra->setVisible(false);
            ui->midi_setup->setVisible(false);
            ui->gme_setup->setVisible(true);
            break;
        default:
            ui->adlmidi_xtra->setVisible(false);
            ui->midi_setup->setVisible(false);
            ui->gme_setup->setVisible(false);
            break;
    }
    this->window()->resize(minimumWidth(), minimumHeight());
}

void MainWindow::on_mididevice_currentIndexChanged(int index)
{
    switch(index)
    {
        case 0: MIX_SetMidiDevice(MIDI_ADLMIDI);
        ui->adlmidi_xtra->setVisible(true);
        break;
        case 1: MIX_SetMidiDevice(MIDI_Timidity);
        ui->adlmidi_xtra->setVisible(false);
        break;
        case 2: MIX_SetMidiDevice(MIDI_Native);
        ui->adlmidi_xtra->setVisible(false);
        break;
        default: MIX_SetMidiDevice(MIDI_ADLMIDI);
        ui->adlmidi_xtra->setVisible(true);
        break;
    }
    this->window()->resize(minimumWidth(), minimumHeight());
}

void MainWindow::on_fmBank_valueChanged(int arg1)
{
    MIX_ADLMIDI_setBankID(arg1);
}

void MainWindow::on_tremolo_clicked(bool checked)
{
    MIX_ADLMIDI_setTremolo((int)checked);
}

void MainWindow::on_vibrato_clicked(bool checked)
{
    MIX_ADLMIDI_setVibrato((int)checked);
}

void MainWindow::on_modulation_clicked(bool checked)
{
    MIX_ADLMIDI_setScaleMod((int)checked);
}
