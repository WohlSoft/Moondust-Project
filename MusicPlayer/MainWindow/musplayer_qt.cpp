#ifndef MUSPLAY_USE_WINAPI
#include <QtDebug>
#include <QFileDialog>
#include <QMessageBox>
#include <QSlider>
#include <QSettings>
#include <QMenu>
#include <QDesktopServices>
#include <QUrl>

#include "ui_mainwindow.h"
#include "musplayer_qt.h"
#include "../Player/mus_player.h"
#include "../AssocFiles/assoc_files.h"
#include "../Effects/reverb.h"
#include <math.h>
#include "../version.h"

MusPlayer_Qt::MusPlayer_Qt(QWidget *parent) : QMainWindow(parent),
    MusPlayerBase(),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    #ifdef Q_OS_MAC
    this->setWindowIcon(QIcon(":/cat_musplay.icns"));
    #endif
    #ifdef Q_OS_WIN
    this->setWindowIcon(QIcon(":/cat_musplay.ico"));
    #endif

    ui->fmbank->clear();
    int totalBakns = MIX_ADLMIDI_getTotalBanks();
    const char*const* names = MIX_ADLMIDI_getBankNames();
    for(int i=0; i<totalBakns; i++)
    {
        ui->fmbank->addItem(QString("%1 = %2").arg(i).arg(names[i]));
    }

    ui->adlmidi_xtra->setVisible(false);
    ui->midi_setup->setVisible(false);
    ui->gme_setup->setVisible(false);
    ui->centralWidget->window()->setWindowFlags(
                Qt::WindowTitleHint|
                Qt::WindowSystemMenuHint|
                Qt::WindowCloseButtonHint|
                Qt::WindowMinimizeButtonHint);
    updateGeometry();
    window()->resize(100, 100);
    connect(&m_blinker, SIGNAL(timeout()), this, SLOT(_blink_red()));
    connect(this, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(contextMenu(QPoint)));

    connect(ui->volume, &QSlider::valueChanged, [this](int x){ on_volume_valueChanged(x); });
    connect(ui->fmbank, static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, [this](int x){ on_fmbank_currentIndexChanged(x); } );
    connect(ui->tremolo, &QCheckBox::clicked, this, [this](int x){ on_tremolo_clicked(x); });
    connect(ui->vibrato, &QCheckBox::clicked, this, [this](int x){ on_vibrato_clicked(x); });
    connect(ui->modulation, &QCheckBox::clicked, this, [this](int x){ on_modulation_clicked(x); });
    connect(ui->adlibMode, &QCheckBox::clicked, this, [this](int x){ on_adlibMode_clicked(x); });

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
        case 3: MIX_SetMidiDevice(MIDI_Fluidsynth);
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

    m_prevTrackID = ui->trackID->value();
}

MusPlayer_Qt::~MusPlayer_Qt()
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

void MusPlayer_Qt::dropEvent(QDropEvent *e)
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

void MusPlayer_Qt::dragEnterEvent(QDragEnterEvent *e)
{
    if (e->mimeData()->hasUrls()) {
        e->acceptProposedAction();
    }
}

void MusPlayer_Qt::contextMenu(const QPoint &pos)
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
            Mix_RegisterEffect(MIX_CHANNEL_POST, reverbEffect, reverbEffectDone, NULL);
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
        QDesktopServices::openUrl(QUrl("https://github.com/WohlSoft/PGE-Project"));
    }
}

void MusPlayer_Qt::openMusicByArg(QString musPath)
{
    if(ui->recordWav->isChecked()) return;
    currentMusic=musPath;
    ui->recordWav->setEnabled(!currentMusic.endsWith(".wav", Qt::CaseInsensitive));//Avoid self-trunkling!
    Mix_HaltMusic();
    on_play_clicked();
}

void MusPlayer_Qt::on_open_clicked()
{
    QString file = QFileDialog::getOpenFileName(this, tr("Open music file"),
                                                 (currentMusic.isEmpty() ? QApplication::applicationDirPath() : currentMusic), "All (*.*)");
    if(file.isEmpty())
        return;

    currentMusic=file;
    ui->recordWav->setEnabled(!currentMusic.endsWith(".wav", Qt::CaseInsensitive));//Avoid self-trunkling!
    Mix_HaltMusic();
    on_play_clicked();
}

void MusPlayer_Qt::on_stop_clicked()
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

void MusPlayer_Qt::on_play_clicked()
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
    if(PGE_MusicPlayer::MUS_openFile( currentMusic +"|" + ui->trackID->text() ) )
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

void MusPlayer_Qt::on_mididevice_currentIndexChanged(int index)
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
        case 3: MIX_SetMidiDevice(MIDI_Fluidsynth);
        ui->adlmidi_xtra->setVisible(false);
        break;
        default: MIX_SetMidiDevice(MIDI_ADLMIDI);
        ui->adlmidi_xtra->setVisible(true);
        break;
    }
    this->window()->resize(100,100);
    if(Mix_PlayingMusic())
    {
        if(PGE_MusicPlayer::type == MUS_MID)
        {
            Mix_HaltMusic();
            on_play_clicked();
        }
    }
}

void MusPlayer_Qt::on_trackID_editingFinished()
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

void MusPlayer_Qt::on_recordWav_clicked(bool checked)
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

void MusPlayer_Qt::on_resetDefaultADLMIDI_clicked()
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

void MusPlayer_Qt::_blink_red()
{
    m_blink_state=!m_blink_state;
    if(m_blink_state)
        ui->recordWav->setStyleSheet("background-color : red; color : black;");
    else
        ui->recordWav->setStyleSheet("background-color : black; color : red;");
}

#endif
