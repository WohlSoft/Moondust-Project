#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "AssocFiles/assoc_files.h"
#include "Effects/reverb.h"
#include <math.h>

#include <QtDebug>
#include <QFileDialog>
#include <QMessageBox>
#include <QSettings>
#include <QMenu>
#include <QDesktopServices>
#include <QUrl>

#include "version.h"

#undef main
#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer_ext.h>
#undef main

#include "wave_writer.h"

/*!
 *  SDL Mixer wrapper
 */
namespace PGE_MusicPlayer
{
    Mix_Music *play_mus = NULL;
    Mix_MusicType type  = MUS_NONE;
    bool reverbEnabled = false;

    QString musicType()
    {
        return QString(
        type == MUS_NONE?"No Music":
        type == MUS_CMD?"CMD":
        type == MUS_WAV?"PCM Wave":
        type == MUS_MOD?"MidMod":
        type == MUS_MODPLUG?"ModPlug":
        type == MUS_MID?"MIDI/IMF":
        type == MUS_OGG?"OGG":
        type == MUS_MP3?"MP3 (SMPEG)":
        type == MUS_MP3_MAD?"MP3 (LibMAD)":
        type == MUS_FLAC?"FLAC":
        type == MUS_SPC?"Game Music Emulator": "<Unknown>" );
    }

    /*!
     * \brief Spawn warning message box with specific text
     * \param msg text to spawn in message box
     */
    static void error(QString msg)
    {
        QMessageBox::warning(nullptr, "SDL2 Mixer ext error", msg, QMessageBox::Ok);
    }

    /*!
     * \brief Stop music playing
     */
    void MUS_stopMusic()
    {
        Mix_HaltMusic();
    }

    /*!
     * \brief Get music title of current track
     * \return music title of current music file
     */
    QString MUS_getMusTitle()
    {
        if(play_mus)
            return QString(Mix_GetMusicTitle(play_mus));
        else
            return QString("[No music]");
    }

    /*!
     * \brief Get music artist tag text of current music track
     * \return music artist tag text of current music track
     */
    QString MUS_getMusArtist()
    {
        if(play_mus)
            return QString(Mix_GetMusicArtistTag(play_mus));
        else
            return QString("[Unknown Artist]");
    }

    /*!
     * \brief Get music album tag text of current music track
     * \return music ablum tag text of current music track
     */
    QString MUS_getMusAlbum()
    {
        if(play_mus)
            return QString(Mix_GetMusicAlbumTag(play_mus));
        else
            return QString("[Unknown Album]");
    }

    /*!
     * \brief Get music copyright tag text of current music track
     * \return music copyright tag text of current music track
     */
    QString MUS_getMusCopy()
    {
        if(play_mus)
            return QString(Mix_GetMusicCopyrightTag(play_mus));
        else
            return QString("");
    }

    /*!
     * \brief Start playing of currently opened music track
     */
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

    /*!
     * \brief Sets volume level of current music stream
     * \param volume level of volume from 0 tp 128
     */
    void MUS_changeVolume(int volume)
    {
        Mix_VolumeMusic(volume);
        qDebug() << QString("Mix_VolumeMusic: %1\n").arg(volume);
    }

    /*!
     * \brief Open music file
     * \param musFile Full path to music file
     * \return true if music file was successfully opened, false if loading was failed
     */
    bool MUS_openFile(QString musFile)
    {
        type = MUS_NONE;
        if(play_mus!=NULL) {Mix_FreeMusic(play_mus);play_mus=NULL;}
        play_mus = Mix_LoadMUS( musFile.toUtf8().data() );
        if(!play_mus) {
            error(QString("Mix_LoadMUS(\"%1\"): %2").arg(musFile).arg(Mix_GetError()));
            return false;
        }
        type = Mix_GetMusicType( play_mus );
        qDebug() << QString("Music type: %1").arg( musicType() );
        return true;
    }

    static bool wavOpened=false;

    // make a music play function
    // it expects udata to be a pointer to an int
    void myMusicPlayer(void */*udata*/, Uint8 *stream, int len)
    {
        wave_write( (short*)stream, len/2 );
    }

    void startWavRecording(QString target)
    {
        if(wavOpened) return;
        if(!play_mus) return;

        /* Record 20 seconds to wave file */
        wave_open( 44100, target.toLocal8Bit().data() );
        wave_enable_stereo();
        Mix_SetPostMix(myMusicPlayer, NULL);
        wavOpened=true;
    }

    void stopWavRecording()
    {
        if(!wavOpened) return;
        wave_close();
        Mix_SetPostMix(NULL, NULL);
        wavOpened=false;
    }

}


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    #ifdef Q_OS_MAC
    this->setWindowIcon(QIcon(":/cat_musplay.icns"));
    #endif
    #ifdef Q_OS_WIN
    this->setWindowIcon(QIcon(":/cat_musplay.ico"));
    #endif
    ui->adlmidi_xtra->setVisible(false);
    ui->midi_setup->setVisible(false);
    ui->gme_setup->setVisible(false);
    ui->centralWidget->window()->setWindowFlags(
                Qt::WindowTitleHint|
                Qt::WindowSystemMenuHint|
                Qt::WindowCloseButtonHint|
                Qt::WindowMinimizeButtonHint);
    this->updateGeometry();
    this->window()->resize(100, 100);
    this->connect(&m_blinker, SIGNAL(timeout()), this, SLOT(_blink_red()));
    this->connect(this, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(contextMenu(QPoint)));
    m_blink_state=false;

    m_prevTrackID = ui->trackID->value();

    QApplication::setOrganizationName(_COMPANY);
    QApplication::setOrganizationDomain(_PGE_URL);
    QApplication::setApplicationName("PGE Music Player");
    QSettings setup;
    restoreGeometry(setup.value("Window-Geometry").toByteArray());
    ui->mididevice->setCurrentIndex(setup.value("MIDI-Device", 0).toInt());
    switch(ui->mididevice->currentIndex())
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
    ui->fmbank->setCurrentIndex(setup.value("ADLMIDI-Bank-ID", 58).toInt());
    MIX_ADLMIDI_setBankID( ui->fmbank->currentIndex() );
    ui->tremolo->setChecked(setup.value("ADLMIDI-Tremolo", true).toBool());
    MIX_ADLMIDI_setTremolo((int)ui->tremolo->isChecked());
    ui->vibrato->setChecked(setup.value("ADLMIDI-Vibrato", true).toBool());
    MIX_ADLMIDI_setVibrato((int)ui->vibrato->isChecked());
    ui->adlibMode->setChecked(setup.value("ADLMIDI-AdLib-Drums-Mode", false).toBool());
    MIX_ADLMIDI_setAdLibMode((int)ui->adlibMode->isChecked());
    ui->modulation->setChecked(setup.value("ADLMIDI-Scalable-Modulation", false).toBool());
    MIX_ADLMIDI_setScaleMod((int)ui->modulation->isChecked());
    ui->volume->setValue(setup.value("Volume", 128).toInt());
}

MainWindow::~MainWindow()
{
    on_stop_clicked();
    Mix_CloseAudio();

    QSettings setup;
    setup.setValue("Window-Geometry", saveGeometry());
    setup.setValue("MIDI-Device", ui->mididevice->currentIndex());
    setup.setValue("ADLMIDI-Bank-ID", ui->fmbank->currentIndex());
    setup.setValue("ADLMIDI-Tremolo", ui->tremolo->isChecked());
    setup.setValue("ADLMIDI-Vibrato", ui->vibrato->isChecked());
    setup.setValue("ADLMIDI-AdLib-Drums-Mode", ui->adlibMode->isChecked());
    setup.setValue("ADLMIDI-Scalable-Modulation", ui->modulation->isChecked());
    setup.setValue("Volume", ui->volume->value());

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
    if(ui->recordWav->isChecked()) return;

    currentMusic=musPath;
    ui->recordWav->setEnabled(!currentMusic.endsWith(".wav", Qt::CaseInsensitive));//Avoid self-trunkling!
    Mix_HaltMusic();
    on_play_clicked();
}

void MainWindow::dropEvent(QDropEvent *e)
{
    this->raise();
    this->setFocus(Qt::ActiveWindowFocusReason);

    if(ui->recordWav->isChecked())
    {
        return;
    }
    foreach (const QUrl &url, e->mimeData()->urls()) {
        const QString &fileName = url.toLocalFile();
        currentMusic=fileName;
    }
    ui->recordWav->setEnabled(!currentMusic.endsWith(".wav", Qt::CaseInsensitive));//Avoid self-trunkling!
    Mix_HaltMusic();
    on_play_clicked();
    this->raise();
    e->accept();
}

void MainWindow::on_open_clicked()
{
    QString file = QFileDialog::getOpenFileName(this, tr("Open music file"),
                                                 (currentMusic.isEmpty() ? QApplication::applicationDirPath() : currentMusic), "All (*.*)");
    if(file.isEmpty()) return;
    currentMusic=file;
    ui->recordWav->setEnabled(!currentMusic.endsWith(".wav", Qt::CaseInsensitive));//Avoid self-trunkling!
    Mix_HaltMusic();
    on_play_clicked();
}

void MainWindow::on_stop_clicked()
{
    PGE_MusicPlayer::MUS_stopMusic();
    ui->play->setText(tr("Play"));
    if(ui->recordWav->isChecked())
    {
        ui->recordWav->setChecked(false);
        PGE_MusicPlayer::stopWavRecording();
        ui->open->setEnabled(true);
        ui->play->setEnabled(true);
        ui->frame->setEnabled(true);
        m_blinker.stop();
        ui->recordWav->setStyleSheet("");
    }
}

void MainWindow::on_play_clicked()
{
    if(Mix_PlayingMusic())
    {
        if(Mix_PausedMusic())
        {
            Mix_ResumeMusic();
            ui->play->setText(tr("Pause"));
            return;
        }
        else
        {
            Mix_PauseMusic();
            ui->play->setText(tr("Resume"));
            return;
        }
    }

    ui->play->setText(tr("Play"));
    m_prevTrackID = ui->trackID->value();
    if(PGE_MusicPlayer::MUS_openFile(currentMusic+"|"+ui->trackID->text()))
    {
        PGE_MusicPlayer::MUS_changeVolume(ui->volume->value());
        PGE_MusicPlayer::MUS_playMusic();
        ui->play->setText(tr("Pause"));
    }
    ui->musTitle->setText(PGE_MusicPlayer::MUS_getMusTitle());
    ui->musArtist->setText(PGE_MusicPlayer::MUS_getMusArtist());
    ui->musAlbum->setText(PGE_MusicPlayer::MUS_getMusAlbum());
    ui->musCopyright->setText(PGE_MusicPlayer::MUS_getMusCopy());

    ui->gme_setup->setVisible(false);
    ui->adlmidi_xtra->setVisible(false);
    ui->midi_setup->setVisible(false);
    ui->frame->setVisible(false);
    this->window()->resize(100, 100);
    ui->frame->setVisible(true);
    ui->smallInfo->setText(PGE_MusicPlayer::musicType());

    ui->gridLayout->update();

    switch(PGE_MusicPlayer::type)
    {
        case MUS_MID:
            ui->adlmidi_xtra->setVisible(ui->mididevice->currentIndex()==0);
            ui->midi_setup->setVisible(true);
            ui->frame->setVisible(true);
            break;
        case MUS_SPC:
            ui->gme_setup->setVisible(true);
            ui->frame->setVisible(true);
            break;
        default:
            break;
    }
    this->window()->updateGeometry();
    this->window()->resize(100,100);
}

void MainWindow::on_mididevice_currentIndexChanged(int index)
{
    ui->midi_setup->setVisible(false);
    ui->adlmidi_xtra->setVisible(false);
    this->window()->resize(100,100);
    ui->midi_setup->setVisible(true);
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
    this->window()->resize(100,100);

    if(Mix_PlayingMusic())
    {
        if(PGE_MusicPlayer::type==MUS_MID)
        {
            Mix_HaltMusic();
            on_play_clicked();
        }
    }
}

void MainWindow::on_fmbank_currentIndexChanged(int index)
{
    MIX_ADLMIDI_setBankID(index);
    if(Mix_PlayingMusic())
    {
        if(PGE_MusicPlayer::type==MUS_MID)
        {
            Mix_HaltMusic();
            on_play_clicked();
        }
    }
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

void MainWindow::on_adlibMode_clicked(bool checked)
{
    MIX_ADLMIDI_setAdLibMode((int)checked);
}

void MainWindow::on_trackID_editingFinished()
{
    if(Mix_PlayingMusic())
    {
        if( (PGE_MusicPlayer::type == MUS_SPC) && (m_prevTrackID != ui->trackID->value()) )
        {
            Mix_HaltMusic();
            on_play_clicked();
        }
    }
}

void MainWindow::on_recordWav_clicked(bool checked)
{
    if(checked)
    {
        PGE_MusicPlayer::MUS_stopMusic();
        ui->play->setText(tr("Play"));
        QFileInfo twav(currentMusic);
        PGE_MusicPlayer::stopWavRecording();
        PGE_MusicPlayer::startWavRecording(twav.absoluteDir().absolutePath()+"/"+twav.baseName()+".wav");
        on_play_clicked();
        ui->open->setEnabled(false);
        ui->play->setEnabled(false);
        ui->frame->setEnabled(false);
        m_blinker.start(500);
    } else {
        on_stop_clicked();
        PGE_MusicPlayer::stopWavRecording();
        ui->open->setEnabled(true);
        ui->play->setEnabled(true);
        ui->frame->setEnabled(true);
        m_blinker.stop();
        ui->recordWav->setStyleSheet("");
    }
}

void MainWindow::_blink_red()
{
    m_blink_state=!m_blink_state;
    if(m_blink_state)
        ui->recordWav->setStyleSheet("background-color : red; color : black;");
    else
        ui->recordWav->setStyleSheet("background-color : black; color : red;");
}

void MainWindow::on_resetDefaultADLMIDI_clicked()
{
    ui->fmbank->setCurrentIndex( 58 );
    ui->tremolo->setChecked(true);
    ui->vibrato->setChecked(true);
    ui->adlibMode->setChecked(false);
    ui->modulation->setChecked(false);

    MIX_ADLMIDI_setTremolo((int)ui->tremolo->isChecked());
    MIX_ADLMIDI_setVibrato((int)ui->vibrato->isChecked());
    MIX_ADLMIDI_setAdLibMode((int)ui->adlibMode->isChecked());
    MIX_ADLMIDI_setScaleMod((int)ui->modulation->isChecked());

    on_fmbank_currentIndexChanged( ui->fmbank->currentIndex() );
}

void MainWindow::on_volume_valueChanged(int value)
{
    PGE_MusicPlayer::MUS_changeVolume(value);
}

void MainWindow::contextMenu(const QPoint &pos)
{
    QMenu x;
    QAction* open        = x.addAction("Open");
    QAction* playpause   = x.addAction("Play/Pause");
    QAction* stop        = x.addAction("Stop");
                           x.addSeparator();
    QAction* reverb       = x.addAction("Reverb");
    reverb->setCheckable(true);
    reverb->setChecked(PGE_MusicPlayer::reverbEnabled);
    QAction* assoc_files = x.addAction("Associate files");
                           x.addSeparator();
    QMenu  * about       = x.addMenu("About");
    QAction* version     = about->addAction("SDL Mixer X Music Player v." _FILE_VERSION );
                           version->setEnabled(false);
    QAction* license     = about->addAction("Licensed under GNU GPLv3 license");
                           about->addSeparator();
    QAction* source      = about->addAction("Get source code");

    QAction *ret = x.exec(this->mapToGlobal(pos));
    if(open == ret)
    {
        on_open_clicked();
    }
    else if(playpause == ret)
    {
        on_play_clicked();
    }
    else if(stop == ret)
    {
        on_stop_clicked();
    }
    else if(reverb == ret)
    {
        PGE_MusicPlayer::reverbEnabled = reverb->isChecked();
        if(PGE_MusicPlayer::reverbEnabled)
            Mix_RegisterEffect(MIX_CHANNEL_POST, reverbEffect, NULL, NULL);
        else
            Mix_UnregisterEffect(MIX_CHANNEL_POST, reverbEffect);
    }
    else if(assoc_files == ret)
    {
        AssocFiles af(this);
        af.setWindowModality(Qt::WindowModal);
        af.exec();
    }
    else if(ret == license)
    {
        QDesktopServices::openUrl(QUrl("http://www.gnu.org/licenses/gpl"));
    }
    else if(ret == source)
    {
        QDesktopServices::openUrl(QUrl("https://github.com/Wohlhabend-Networks/PGE-Project"));
    }
}
