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
    PGE_MusicPlayer::setMainWindow(this);
#ifdef Q_OS_MAC
    this->setWindowIcon(QIcon(":/cat_musplay.icns"));
#endif
#ifdef Q_OS_WIN
    this->setWindowIcon(QIcon(":/cat_musplay.ico"));
#endif
    ui->fmbank->clear();
    int totalBakns = MIX_ADLMIDI_getTotalBanks();
    const char *const *names = MIX_ADLMIDI_getBankNames();

    for(int i = 0; i < totalBakns; i++)
        ui->fmbank->addItem(QString("%1 = %2").arg(i).arg(names[i]));

    ui->centralWidget->window()->setWindowFlags(
        Qt::WindowTitleHint |
        Qt::WindowSystemMenuHint |
        Qt::WindowCloseButtonHint |
        Qt::WindowMinimizeButtonHint |
        Qt::MSWindowsFixedSizeDialogHint);
    connect(&m_blinker, SIGNAL(timeout()), this, SLOT(_blink_red()));
    connect(this, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(contextMenu(QPoint)));
    connect(ui->volume, &QSlider::valueChanged, [this](int x)
    {
        on_volume_valueChanged(x);
    });
    connect(ui->fmbank, static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, [this](int x)
    {
        on_fmbank_currentIndexChanged(x);
    });
    connect(ui->volumeModel, static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, [this](int x)
    {
        on_volumeModel_currentIndexChanged(x);
    });
    connect(ui->tremolo, &QCheckBox::clicked, this, [this](int x)
    {
        on_tremolo_clicked(x);
    });
    connect(ui->vibrato, &QCheckBox::clicked, this, [this](int x)
    {
        on_vibrato_clicked(x);
    });
    connect(ui->modulation, &QCheckBox::clicked, this, [this](int x)
    {
        on_modulation_clicked(x);
    });
    connect(ui->adlibMode, &QCheckBox::clicked, this, [this](int x)
    {
        on_adlibMode_clicked(x);
    });
    connect(ui->logVolumes, &QCheckBox::clicked, this, [this](int x)
    {
        on_logVolumes_clicked(x);
    });

    connect(ui->playListPush, &QPushButton::clicked, this, &MusPlayer_Qt::playList_pushCurrent);
    connect(ui->playListPop, &QPushButton::clicked, this, &MusPlayer_Qt::playList_popCurrent);

    QApplication::setOrganizationName(_COMPANY);
    QApplication::setOrganizationDomain(_PGE_URL);
    QApplication::setApplicationName("PGE Music Player");
    ui->playList->setModel(&playList);

    ui->playList->setVisible(false);
    ui->playListPush->setVisible(false);
    ui->playListPop->setVisible(false);

    ui->sfx_testing->setVisible(false);

    QSettings setup;
    restoreGeometry(setup.value("Window-Geometry").toByteArray());
    ui->mididevice->setCurrentIndex(setup.value("MIDI-Device", 0).toInt());

    ui->opnmidi_extra->setVisible(ui->mididevice->currentIndex() == 3);
    ui->adlmidi_xtra->setVisible(ui->mididevice->currentIndex() == 0);

    switch(ui->mididevice->currentIndex())
    {
    case 0:
        MIX_SetMidiDevice(MIDI_ADLMIDI);
        break;

    case 1:
        MIX_SetMidiDevice(MIDI_Timidity);
        break;

    case 2:
        MIX_SetMidiDevice(MIDI_Native);
        break;

    case 3:
        MIX_SetMidiDevice(MIDI_OPNMIDI);
        break;

    case 4:
        MIX_SetMidiDevice(MIDI_Fluidsynth);
        break;

    default:
        MIX_SetMidiDevice(MIDI_ADLMIDI);
        break;
    }

    ui->fmbank->setCurrentIndex(setup.value("ADLMIDI-Bank-ID", 58).toInt());
    MIX_ADLMIDI_setBankID(ui->fmbank->currentIndex());
    ui->volumeModel->setCurrentIndex(setup.value("ADLMIDI-VolumeModel", 0).toInt());
    MIX_ADLMIDI_setVolumeModel(ui->volumeModel->currentIndex());
    ui->tremolo->setChecked(setup.value("ADLMIDI-Tremolo", true).toBool());
    MIX_ADLMIDI_setTremolo(static_cast<int>(ui->tremolo->isChecked()));
    ui->vibrato->setChecked(setup.value("ADLMIDI-Vibrato", true).toBool());
    MIX_ADLMIDI_setVibrato(static_cast<int>(ui->vibrato->isChecked()));
    ui->adlibMode->setChecked(setup.value("ADLMIDI-AdLib-Drums-Mode", false).toBool());
    MIX_ADLMIDI_setAdLibMode(static_cast<int>(ui->adlibMode->isChecked()));
    ui->modulation->setChecked(setup.value("ADLMIDI-Scalable-Modulation", false).toBool());
    MIX_ADLMIDI_setScaleMod(static_cast<int>(ui->modulation->isChecked()));
    ui->logVolumes->setChecked(setup.value("ADLMIDI-LogarithmicVolumes", false).toBool());
    MIX_ADLMIDI_setScaleMod(static_cast<int>(ui->logVolumes->isChecked()));
    ui->volume->setValue(setup.value("Volume", 128).toInt());
    m_prevTrackID = ui->trackID->value();
    ui->adlmidi_xtra->setVisible(false);
    ui->opnmidi_extra->setVisible(false);
    ui->midi_setup->setVisible(false);
    ui->gme_setup->setVisible(false);

    currentMusic = setup.value("RecentMusic", "").toString();
    m_testSfxDir = setup.value("RecentSfxDir", "").toString();

    adjustSize();
}

MusPlayer_Qt::~MusPlayer_Qt()
{
    on_stop_clicked();
    if(m_testSfx)
        Mix_FreeChunk(m_testSfx);
    m_testSfx = nullptr;
    Mix_CloseAudio();
    QSettings setup;
    setup.setValue("Window-Geometry", saveGeometry());
    setup.setValue("MIDI-Device", ui->mididevice->currentIndex());
    setup.setValue("ADLMIDI-Bank-ID", ui->fmbank->currentIndex());
    setup.setValue("ADLMIDI-VolumeModel", ui->volumeModel->currentIndex());
    setup.setValue("ADLMIDI-Tremolo", ui->tremolo->isChecked());
    setup.setValue("ADLMIDI-Vibrato", ui->vibrato->isChecked());
    setup.setValue("ADLMIDI-AdLib-Drums-Mode", ui->adlibMode->isChecked());
    setup.setValue("ADLMIDI-Scalable-Modulation", ui->modulation->isChecked());
    setup.setValue("ADLMIDI-LogarithmicVolumes", ui->logVolumes->isChecked());
    setup.setValue("Volume", ui->volume->value());
    setup.setValue("RecentMusic", currentMusic);
    setup.setValue("RecentSfxDir", m_testSfxDir);
    delete ui;
}

void MusPlayer_Qt::dropEvent(QDropEvent *e)
{
    this->raise();
    this->setFocus(Qt::ActiveWindowFocusReason);

    if(ui->recordWav->isChecked())
        return;

    for(const QUrl &url : e->mimeData()->urls())
    {
        const QString &fileName = url.toLocalFile();
        currentMusic = fileName;
    }

    ui->recordWav->setEnabled(!currentMusic.endsWith(".wav", Qt::CaseInsensitive));//Avoid self-trunkling!
    PGE_MusicPlayer::MUS_stopMusic();
    on_play_clicked();
    this->raise();
    e->accept();
}

void MusPlayer_Qt::dragEnterEvent(QDragEnterEvent *e)
{
    if(e->mimeData()->hasUrls())
        e->acceptProposedAction();
}

void MusPlayer_Qt::contextMenu(const QPoint &pos)
{
    QMenu x;
    QAction *open        = x.addAction("Open");
    QAction *playpause   = x.addAction("Play/Pause");
    QAction *stop        = x.addAction("Stop");
    x.addSeparator();
    QAction *reverb       = x.addAction("Reverb");
    reverb->setCheckable(true);
    reverb->setChecked(PGE_MusicPlayer::reverbEnabled);
    QAction *assoc_files = x.addAction("Associate files");
    QAction *play_list   = x.addAction("Play-list mode [WIP]");
    play_list->setCheckable(true);
    play_list->setChecked(playListMode);

    QAction *sfx_testing = x.addAction("SFX testing");
    sfx_testing->setCheckable(true);
    sfx_testing->setChecked(ui->sfx_testing->isVisible());

    x.addSeparator();
    QMenu   *about       = x.addMenu("About");
    QAction *version     = about->addAction("SDL Mixer X Music Player v." _FILE_VERSION);
    version->setEnabled(false);
    QAction *license     = about->addAction("Licensed under GNU GPLv3 license");
    about->addSeparator();
    QAction *source      = about->addAction("Get source code");
    QAction *ret = x.exec(this->mapToGlobal(pos));

    if(open == ret)
        on_open_clicked();
    else if(playpause == ret)
        on_play_clicked();
    else if(stop == ret)
        on_stop_clicked();
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
    else if(ret == play_list)
    {
        setPlayListMode(!playListMode);
    }
    else if(ret == sfx_testing)
    {
        ui->sfx_testing->setVisible(!ui->sfx_testing->isVisible());
        updateGeometry();
        adjustSize();
    }
    else if(ret == license)
        QDesktopServices::openUrl(QUrl("http://www.gnu.org/licenses/gpl"));
    else if(ret == source)
        QDesktopServices::openUrl(QUrl("https://github.com/WohlSoft/PGE-Project"));
}

void MusPlayer_Qt::openMusicByArg(QString musPath)
{
    if(ui->recordWav->isChecked()) return;

    currentMusic = musPath;
    //ui->recordWav->setEnabled(!currentMusic.endsWith(".wav", Qt::CaseInsensitive));//Avoid self-trunkling!
    PGE_MusicPlayer::MUS_stopMusic();
    on_play_clicked();
}

void MusPlayer_Qt::setPlayListMode(bool plMode)
{
    on_stop_clicked();
    playListMode = plMode;
    if(!plMode)
    {
        playList.clear();
    } else {
        playList_pushCurrent();
    }

    ui->playList->setVisible(plMode);
    ui->playListPush->setVisible(plMode);
    ui->playListPop->setVisible(plMode);
    if(ui->recordWav->isChecked())
        ui->recordWav->click();
    ui->recordWav->setVisible(!plMode);
    PGE_MusicPlayer::setPlayListMode(playListMode);

    adjustSize();
}

void MusPlayer_Qt::playList_pushCurrent(bool)
{
    PlayListEntry e;

    e.name = ui->musTitle->text();
    e.fullPath = currentMusic;

    e.gme_trackNum = ui->trackID->value();

    e.midi_device = ui->mididevice->currentIndex();

    e.adl_bankNo = ui->fmbank->currentIndex();
    e.adl_cmfVolumes = ui->volumeModel->currentIndex();
    e.adl_tremolo = ui->tremolo->isChecked();
    e.adl_vibrato = ui->vibrato->isChecked();
    e.adl_adlibDrums = ui->adlibMode->isChecked();
    e.adl_modulation = ui->modulation->isChecked();
    e.adl_cmfVolumes = ui->logVolumes->isChecked();

    playList.insertEntry(e);
}

void MusPlayer_Qt::playList_popCurrent(bool)
{
    playList.removeEntry();
}

void MusPlayer_Qt::playListNext()
{
    PlayListEntry e = playList.nextEntry();
    currentMusic = e.fullPath;
    switchMidiDevice(e.midi_device);
    ui->trackID->setValue(e.gme_trackNum);

    ui->fmbank->setCurrentIndex(e.adl_bankNo);
    ui->volumeModel->setCurrentIndex(e.adl_volumeModel);
    ui->tremolo->setChecked(e.adl_tremolo);
    ui->vibrato->setChecked(e.adl_vibrato);
    ui->adlibMode->setChecked(e.adl_adlibDrums);
    ui->modulation->setChecked(e.adl_modulation);
    ui->logVolumes->setChecked(e.adl_cmfVolumes);

    MIX_ADLMIDI_setBankID(e.adl_bankNo);
    MIX_ADLMIDI_setVolumeModel(e.adl_volumeModel);

    MIX_ADLMIDI_setTremolo(static_cast<int>(ui->tremolo->isChecked()));
    MIX_ADLMIDI_setVibrato(static_cast<int>(ui->vibrato->isChecked()));
    MIX_ADLMIDI_setAdLibMode(static_cast<int>(ui->adlibMode->isChecked()));
    MIX_ADLMIDI_setScaleMod(static_cast<int>(ui->modulation->isChecked()));
    MIX_ADLMIDI_setLogarithmicVolumes(static_cast<int>(ui->logVolumes->isChecked()));

    PGE_MusicPlayer::MUS_stopMusic();
    on_play_clicked();
}

void MusPlayer_Qt::switchMidiDevice(int index)
{
    ui->midi_setup->setVisible(false);
    ui->adlmidi_xtra->setVisible(false);
    ui->opnmidi_extra->setVisible(false);
    ui->midi_setup->setVisible(true);

    switch(index)
    {
    case 0:
        MIX_SetMidiDevice(MIDI_ADLMIDI);
        ui->adlmidi_xtra->setVisible(true);
        break;

    case 1:
        MIX_SetMidiDevice(MIDI_Timidity);
        break;

    case 2:
        MIX_SetMidiDevice(MIDI_Native);
        break;

    case 3:
        MIX_SetMidiDevice(MIDI_OPNMIDI);
        ui->opnmidi_extra->setVisible(true);
        break;

    case 4:
        MIX_SetMidiDevice(MIDI_Fluidsynth);
        break;

    default:
        MIX_SetMidiDevice(MIDI_ADLMIDI);
        ui->adlmidi_xtra->setVisible(true);
        break;
    }
}

void MusPlayer_Qt::on_open_clicked()
{
    QString file = QFileDialog::getOpenFileName(this, tr("Open music file"),
                   (currentMusic.isEmpty() ? QApplication::applicationDirPath() : currentMusic), "All (*.*)");

    if(file.isEmpty())
        return;

    currentMusic = file;
    //ui->recordWav->setEnabled(!currentMusic.endsWith(".wav", Qt::CaseInsensitive));//Avoid self-trunkling!
    PGE_MusicPlayer::MUS_stopMusic();
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

    if(PGE_MusicPlayer::MUS_openFile(currentMusic + "|" + ui->trackID->text()))
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
    ui->opnmidi_extra->setVisible(false);
    ui->midi_setup->setVisible(false);
    ui->frame->setVisible(false);
    ui->frame->setVisible(true);
    ui->smallInfo->setText(PGE_MusicPlayer::musicType());
    ui->gridLayout->update();

    switch(PGE_MusicPlayer::type)
    {
    case MUS_MID:
        ui->adlmidi_xtra->setVisible(ui->mididevice->currentIndex() == 0);
        ui->opnmidi_extra->setVisible(ui->mididevice->currentIndex() == 3);
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

    adjustSize();
}

void MusPlayer_Qt::on_mididevice_currentIndexChanged(int index)
{
    switchMidiDevice(index);
    adjustSize();

    if(Mix_PlayingMusic())
    {
        if(PGE_MusicPlayer::type == MUS_MID)
        {
            PGE_MusicPlayer::MUS_stopMusic();
            on_play_clicked();
        }
    }
}

void MusPlayer_Qt::on_trackID_editingFinished()
{
    if(Mix_PlayingMusic())
    {
        if((PGE_MusicPlayer::type == MUS_SPC) && (m_prevTrackID != ui->trackID->value()))
        {
            PGE_MusicPlayer::MUS_stopMusic();
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
        QString wavPathBase = twav.absoluteDir().absolutePath() + "/" + twav.baseName();
        QString wavPath = wavPathBase + ".wav";
        int count = 1;
        while(QFile::exists(wavPath))
            wavPath = wavPathBase + QString("-%1.wav").arg(count++);
        PGE_MusicPlayer::startWavRecording(wavPath);
        on_play_clicked();
        ui->open->setEnabled(false);
        ui->play->setEnabled(false);
        ui->frame->setEnabled(false);
        m_blinker.start(500);
    }
    else
    {
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
    ui->fmbank->setCurrentIndex(58);
    ui->tremolo->setChecked(true);
    ui->vibrato->setChecked(true);
    ui->adlibMode->setChecked(false);
    ui->modulation->setChecked(false);
    ui->logVolumes->setChecked(false);
    MIX_ADLMIDI_setTremolo(static_cast<int>(ui->tremolo->isChecked()));
    MIX_ADLMIDI_setVibrato(static_cast<int>(ui->vibrato->isChecked()));
    MIX_ADLMIDI_setAdLibMode(static_cast<int>(ui->adlibMode->isChecked()));
    MIX_ADLMIDI_setScaleMod(static_cast<int>(ui->modulation->isChecked()));
    MIX_ADLMIDI_setLogarithmicVolumes(static_cast<int>(ui->logVolumes->isChecked()));
    on_volumeModel_currentIndexChanged(ui->volumeModel->currentIndex());
    on_fmbank_currentIndexChanged(ui->fmbank->currentIndex());
}

void MusPlayer_Qt::_blink_red()
{
    m_blink_state = !m_blink_state;

    if(m_blink_state)
        ui->recordWav->setStyleSheet("background-color : red; color : black;");
    else
        ui->recordWav->setStyleSheet("background-color : black; color : red;");
}

void MusPlayer_Qt::on_sfx_open_clicked()
{
    QString file = QFileDialog::getOpenFileName(this, tr("Open SFX file"),
                   (m_testSfxDir.isEmpty() ? QApplication::applicationDirPath() : m_testSfxDir), "All (*.*)");

    if(file.isEmpty())
        return;

    if(m_testSfx)
    {
        Mix_HaltChannel(0);
        Mix_FreeChunk(m_testSfx);
        m_testSfx = nullptr;
    }

    m_testSfx = Mix_LoadWAV(file.toUtf8().data());
    if(!m_testSfx)
        QMessageBox::warning(this, "SFX open error!", QString("Mix_LoadWAV: ") + Mix_GetError());
    else
    {
        QFileInfo f(file);
        m_testSfxDir = f.absoluteDir().absolutePath();
        ui->sfx_file->setText(f.fileName());
    }
}


void MusPlayer_Qt::on_sfx_play_clicked()
{
    if(!m_testSfx)
        return;

    if(Mix_PlayChannelTimedVolume(0,
                                  m_testSfx,
                                  ui->sfx_loops->value(),
                                  ui->sfx_timed->value(),
                                  ui->sfx_volume->value()) == -1)
    {
        QMessageBox::warning(this, "SFX play error!", QString("Mix_PlayChannelTimedVolume: ") + Mix_GetError());
    }
}

void MusPlayer_Qt::on_sfx_fadeIn_clicked()
{
    if(!m_testSfx)
        return;

    if(Mix_FadeInChannelTimedVolume(0,
                                    m_testSfx,
                                    ui->sfx_loops->value(),
                                    ui->sfx_fadems->value(),
                                    ui->sfx_timed->value(),
                                    ui->sfx_volume->value()) == -1)
    {
        QMessageBox::warning(this, "SFX play error!", QString("Mix_PlayChannelTimedVolume: ") + Mix_GetError());
    }
}

void MusPlayer_Qt::on_sfx_stop_clicked()
{
    if(!m_testSfx)
        return;
    Mix_HaltChannel(0);
}

void MusPlayer_Qt::on_sfx_fadeout_clicked()
{
    if(!m_testSfx)
        return;
    Mix_FadeOutChannel(0, ui->sfx_fadems->value());
}

#endif
