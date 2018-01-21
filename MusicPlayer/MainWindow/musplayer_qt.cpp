#ifndef MUSPLAY_USE_WINAPI
#include <QtDebug>
#include <QFileDialog>
#include <QMessageBox>
#include <QSlider>
#include <QDateTime>
#include <QTime>
#include <QSettings>
#include <QMenu>
#include <QDesktopServices>
#include <QUrl>
#include <cmath>

#include "ui_mainwindow.h"
#include "musplayer_qt.h"
#include "../Player/mus_player.h"
#include "../AssocFiles/assoc_files.h"
#include "../Effects/reverb.h"
#include <math.h>
#include "../version.h"

static int tristateToInt(Qt::CheckState state)
{
    switch(state)
    {
    case Qt::Checked:
        return 1;
    case Qt::Unchecked:
        return 0;
    case Qt::PartiallyChecked:
        return -1;
    }
    return Qt::Unchecked;
}

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
    #ifdef SDL_MIXER_X
    int totalBakns = Mix_ADLMIDI_getTotalBanks();
    const char *const *names = Mix_ADLMIDI_getBankNames();
    #else
    int totalBakns = 0;
    const char *const *names = {0};
    #endif

    for(int i = 0; i < totalBakns; i++)
        ui->fmbank->addItem(QString("%1 = %2").arg(i).arg(names[i]));

    QString title = windowTitle();
    /* Append library version to the title */
    const SDL_version* mixer_ver = Mix_Linked_Version();
    #if defined(SDL_MIXER_X)
    title += QString(" (SDL Mixer X %1.%2.%3)")
            .arg(mixer_ver->major)
            .arg(mixer_ver->minor)
            .arg(mixer_ver->patch);
    #else
    title += QString(" (SDL Mixer %1.%2.%3)")
            .arg(mixer_ver->major)
            .arg(mixer_ver->minor)
            .arg(mixer_ver->patch);
    #endif
    setWindowTitle(title);

    ui->tremolo->setCheckState(Qt::PartiallyChecked);
    ui->vibrato->setCheckState(Qt::PartiallyChecked);
    ui->modulation->setCheckState(Qt::PartiallyChecked);
    ui->adlibMode->setCheckState(Qt::PartiallyChecked);
    ui->logVolumes->setCheckState(Qt::PartiallyChecked);

    ui->centralWidget->window()->setWindowFlags(
        Qt::WindowTitleHint |
        Qt::WindowSystemMenuHint |
        Qt::WindowCloseButtonHint |
        Qt::WindowMinimizeButtonHint |
        Qt::MSWindowsFixedSizeDialogHint);
    connect(&m_blinker, SIGNAL(timeout()), this, SLOT(_blink_red()));
    connect(&m_positionWatcher, SIGNAL(timeout()), this, SLOT(updatePositionSlider()));
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
    connect(ui->tremolo, &QCheckBox::clicked, this, [this](int)
    {
        #ifdef SDL_MIXER_X
        Mix_ADLMIDI_setTremolo(tristateToInt(ui->tremolo->checkState()));
        #endif
    });
    connect(ui->vibrato, &QCheckBox::clicked, this, [this](int)
    {
        #ifdef SDL_MIXER_X
        Mix_ADLMIDI_setVibrato(tristateToInt(ui->vibrato->checkState()));
        #endif
    });
    connect(ui->modulation, &QCheckBox::clicked, this, [this](int)
    {
        #ifdef SDL_MIXER_X
        Mix_ADLMIDI_setScaleMod(tristateToInt(ui->modulation->checkState()));
        #endif
    });
    connect(ui->adlibMode, &QCheckBox::clicked, this, [this](int)
    {
        #ifdef SDL_MIXER_X
        Mix_ADLMIDI_setAdLibMode(tristateToInt(ui->adlibMode->checkState()));
        #endif
    });
    connect(ui->logVolumes, &QCheckBox::clicked, this, [this](int)
    {
        #ifdef SDL_MIXER_X
        Mix_ADLMIDI_setLogarithmicVolumes(tristateToInt(ui->logVolumes->checkState()));
        #endif
    });

    connect(ui->opn_use_custom, &QCheckBox::clicked, this, &MusPlayer_Qt::on_opn_bank_editingFinished);
    connect(ui->adl_use_custom, &QCheckBox::clicked, this, &MusPlayer_Qt::on_adl_bank_editingFinished);

    //connect(ui->playListPush, &QPushButton::clicked, this, &MusPlayer_Qt::playList_pushCurrent);
    //connect(ui->playListPop, &QPushButton::clicked, this, &MusPlayer_Qt::playList_popCurrent);

    QApplication::setOrganizationName(V_COMPANY);
    QApplication::setOrganizationDomain(V_PGE_URL);
    QApplication::setApplicationName("PGE Music Player");
    //ui->playList->setModel(&playList);

    QSettings setup;
    ui->mididevice->setCurrentIndex(setup.value("MIDI-Device", 0).toInt());

    ui->musicPosition->setVisible(false);
    ui->isLooping->setVisible(false);
    //ui->playList->setVisible(false);
    //ui->playListPush->setVisible(false);
    //ui->playListPop->setVisible(false);

    ui->sfx_testing->setVisible(false);

    ui->opnmidi_extra->setVisible(ui->mididevice->currentIndex() == 3);
    ui->adlmidi_xtra->setVisible(ui->mididevice->currentIndex() == 0);

    #if defined(SDL_MIXER_X) || defined(SDL_MIXER_GE21)
    switch(ui->mididevice->currentIndex())
    {
    #ifdef SDL_MIXER_X
    case 0:
        Mix_SetMidiPlayer(MIDI_ADLMIDI);
        break;
    #endif
    case 1:
        Mix_SetMidiPlayer(MIDI_Timidity);
        break;
    case 2:
        Mix_SetMidiPlayer(MIDI_Native);
        break;
    #ifdef SDL_MIXER_X
    case 3:
        Mix_SetMidiPlayer(MIDI_OPNMIDI);
        break;
    #endif
    case 4:
        Mix_SetMidiPlayer(MIDI_Fluidsynth);
        break;
    default:
        #ifdef SDL_MIXER_X
        Mix_SetMidiPlayer(MIDI_ADLMIDI);
        #else
        Mix_SetMidiPlayer(MIDI_Timidity);
        #endif
        break;
    }
    #endif

    ui->fmbank->setCurrentIndex(setup.value("ADLMIDI-Bank-ID", 58).toInt());
    #ifdef SDL_MIXER_X
    Mix_ADLMIDI_setBankID(ui->fmbank->currentIndex());
    #endif

    ui->volumeModel->setCurrentIndex(setup.value("ADLMIDI-VolumeModel", 0).toInt());
    #ifdef SDL_MIXER_X
    Mix_ADLMIDI_setVolumeModel(ui->volumeModel->currentIndex());
    #endif

    ui->tremolo->setCheckState((Qt::CheckState)setup.value("ADLMIDI-Tremolo", Qt::PartiallyChecked).toInt());
    #ifdef SDL_MIXER_X
    Mix_ADLMIDI_setTremolo(tristateToInt(ui->tremolo->checkState()));
    #endif

    ui->vibrato->setCheckState((Qt::CheckState)setup.value("ADLMIDI-Vibrato", Qt::PartiallyChecked).toInt());
    #ifdef SDL_MIXER_X
    Mix_ADLMIDI_setVibrato(tristateToInt(ui->vibrato->checkState()));
    #endif

    ui->adlibMode->setCheckState((Qt::CheckState)setup.value("ADLMIDI-AdLib-Drums-Mode", Qt::Unchecked).toInt());
    #ifdef SDL_MIXER_X
    Mix_ADLMIDI_setAdLibMode(tristateToInt(ui->adlibMode->checkState()));
    #endif

    ui->modulation->setCheckState((Qt::CheckState)setup.value("ADLMIDI-Scalable-Modulation", Qt::Unchecked).toInt());
    #ifdef SDL_MIXER_X
    Mix_ADLMIDI_setScaleMod(tristateToInt(ui->modulation->checkState()));
    #endif

    ui->logVolumes->setCheckState((Qt::CheckState)setup.value("ADLMIDI-LogarithmicVolumes", Qt::Unchecked).toInt());
    #ifdef SDL_MIXER_X
    Mix_ADLMIDI_setLogarithmicVolumes(tristateToInt(ui->logVolumes->checkState()));
    #endif

    ui->adl_bank->setText(setup.value("ADLMIDI-Bank", "").toString());
    ui->adl_use_custom->setChecked(setup.value("ADLMIDI-Bank-UseCustom", true).toBool());
    on_adl_bank_editingFinished();

    ui->volume->setValue(setup.value("Volume", 128).toInt());
    m_prevTrackID = ui->trackID->value();
    ui->adlmidi_xtra->setVisible(false);
    ui->opnmidi_extra->setVisible(false);
    ui->midi_setup->setVisible(false);
    ui->gme_setup->setVisible(false);

    ui->opn_bank->setText(setup.value("OPNMIDI-Bank", "").toString());
    ui->opn_use_custom->setChecked(setup.value("OPNMIDI-Bank-UseCustom", true).toBool());
    on_opn_bank_editingFinished();

    currentMusic = setup.value("RecentMusic", "").toString();
    m_testSfxDir = setup.value("RecentSfxDir", "").toString();
    restoreGeometry(setup.value("Window-Geometry").toByteArray());
    layout()->activate();
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
    setup.setValue("ADLMIDI-Tremolo", ui->tremolo->checkState());
    setup.setValue("ADLMIDI-Vibrato", ui->vibrato->checkState());
    setup.setValue("ADLMIDI-AdLib-Drums-Mode", ui->adlibMode->checkState());
    setup.setValue("ADLMIDI-Scalable-Modulation", ui->modulation->checkState());
    setup.setValue("ADLMIDI-LogarithmicVolumes", ui->logVolumes->checkState());
    setup.setValue("ADLMIDI-Bank", ui->adl_bank->text());
    setup.setValue("ADLMIDI-Bank-UseCustom", ui->adl_use_custom->isChecked());

    setup.setValue("OPNMIDI-Bank", ui->opn_bank->text());
    setup.setValue("OPNMIDI-Bank-UseCustom", ui->opn_use_custom->isChecked());

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
    //QAction *play_list   = x.addAction("Play-list mode [WIP]");
    //play_list->setCheckable(true);
    //play_list->setChecked(playListMode);

    QAction *sfx_testing = x.addAction("SFX testing");
    sfx_testing->setCheckable(true);
    sfx_testing->setChecked(ui->sfx_testing->isVisible());

    x.addSeparator();
    QMenu   *about       = x.addMenu("About");
    QAction *version     = about->addAction("SDL Mixer X Music Player v." V_FILE_VERSION);
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
//    else if(ret == play_list)
//    {
//        setPlayListMode(!playListMode);
//    }
    else if(ret == sfx_testing)
    {
        ui->sfx_testing->setVisible(!ui->sfx_testing->isVisible());
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

//void MusPlayer_Qt::setPlayListMode(bool plMode)
//{
//    on_stop_clicked();
//    playListMode = plMode;
//    if(!plMode)
//    {
//        playList.clear();
//    } else {
//        playList_pushCurrent();
//    }

//    ui->playList->setVisible(plMode);
//    ui->playListPush->setVisible(plMode);
//    ui->playListPop->setVisible(plMode);
//    if(ui->recordWav->isChecked())
//        ui->recordWav->click();
//    ui->recordWav->setVisible(!plMode);
//    PGE_MusicPlayer::setPlayListMode(playListMode);

//    adjustSize();
//}

//void MusPlayer_Qt::playList_pushCurrent(bool)
//{
//    PlayListEntry e;

//    e.name = ui->musTitle->text();
//    e.fullPath = currentMusic;

//    e.gme_trackNum = ui->trackID->value();

//    e.midi_device = ui->mididevice->currentIndex();

//    e.adl_bankNo = ui->fmbank->currentIndex();
//    e.adl_cmfVolumes = ui->volumeModel->currentIndex();
//    e.adl_tremolo = ui->tremolo->isChecked();
//    e.adl_vibrato = ui->vibrato->isChecked();
//    e.adl_adlibDrums = ui->adlibMode->isChecked();
//    e.adl_modulation = ui->modulation->isChecked();
//    e.adl_cmfVolumes = ui->logVolumes->isChecked();

//    playList.insertEntry(e);
//}

//void MusPlayer_Qt::playList_popCurrent(bool)
//{
//    playList.removeEntry();
//}

//void MusPlayer_Qt::playListNext()
//{
//    PlayListEntry e = playList.nextEntry();
//    currentMusic = e.fullPath;
//    switchMidiDevice(e.midi_device);
//    ui->trackID->setValue(e.gme_trackNum);

//    ui->fmbank->setCurrentIndex(e.adl_bankNo);
//    ui->volumeModel->setCurrentIndex(e.adl_volumeModel);
//    ui->tremolo->setChecked(e.adl_tremolo);
//    ui->vibrato->setChecked(e.adl_vibrato);
//    ui->adlibMode->setChecked(e.adl_adlibDrums);
//    ui->modulation->setChecked(e.adl_modulation);
//    ui->logVolumes->setChecked(e.adl_cmfVolumes);

//    Mix_ADLMIDI_setBankID(e.adl_bankNo);
//    Mix_ADLMIDI_setVolumeModel(e.adl_volumeModel);

//    Mix_ADLMIDI_setTremolo(static_cast<int>(ui->tremolo->isChecked()));
//    Mix_ADLMIDI_setVibrato(static_cast<int>(ui->vibrato->isChecked()));
//    Mix_ADLMIDI_setAdLibMode(static_cast<int>(ui->adlibMode->isChecked()));
//    Mix_ADLMIDI_setScaleMod(static_cast<int>(ui->modulation->isChecked()));
//    Mix_ADLMIDI_setLogarithmicVolumes(static_cast<int>(ui->logVolumes->isChecked()));

//    PGE_MusicPlayer::MUS_stopMusic();
//    on_play_clicked();
//}

void MusPlayer_Qt::switchMidiDevice(int index)
{
    ui->midi_setup->setVisible(false);
    ui->adlmidi_xtra->setVisible(false);
    ui->opnmidi_extra->setVisible(false);
    ui->midi_setup->setVisible(true);

#if defined(SDL_MIXER_X) || defined(SDL_MIXER_GE21)
    switch(index)
    {
    #ifdef SDL_MIXER_X
    case 0:
        Mix_SetMidiPlayer(MIDI_ADLMIDI);
        ui->adlmidi_xtra->setVisible(true);
        break;
    #endif
    case 1:
        Mix_SetMidiPlayer(MIDI_Timidity);
        break;
    case 2:
        Mix_SetMidiPlayer(MIDI_Native);
        break;
    #ifdef SDL_MIXER_X
    case 3:
        Mix_SetMidiPlayer(MIDI_OPNMIDI);
        ui->opnmidi_extra->setVisible(true);
        break;
    #endif
    case 4:
        Mix_SetMidiPlayer(MIDI_Fluidsynth);
        break;
    default:
        #ifdef SDL_MIXER_X
        Mix_SetMidiPlayer(MIDI_ADLMIDI);
        ui->adlmidi_xtra->setVisible(true);
        #else
        Mix_SetMidiPlayer(MIDI_Timidity);
        #endif
        break;
    }
#else
    (void)index;
#endif
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
    m_positionWatcher.stop();
    ui->playingTimeLabel->setText("--:--:--");
    ui->musicPosition->setValue(0);
    ui->musicPosition->setEnabled(false);
    PGE_MusicPlayer::MUS_stopMusic();
    ui->play->setToolTip(tr("Play"));
    ui->play->setIcon(QIcon(":/buttons/play.png"));

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
            ui->play->setToolTip(tr("Pause"));
            ui->play->setIcon(QIcon(":/buttons/pause.png"));
            return;
        }
        else
        {
            Mix_PauseMusic();
            ui->play->setToolTip(tr("Resume"));
            ui->play->setIcon(QIcon(":/buttons/play.png"));
            return;
        }
    }

    ui->play->setToolTip(tr("Play"));
    ui->play->setIcon(QIcon(":/buttons/play.png"));
    ui->isLooping->setVisible(false);
    m_prevTrackID = ui->trackID->value();

    bool playSuccess = false;

    QString musicPath = currentMusic;
    #if defined(SDL_MIXER_X) || defined(SDL_MIXER_GE21)
    if(ui->gme_setup->isVisible())
        musicPath += "|" + ui->trackID->text();
    else if(ui->midi_setup->isVisible())
    {
        if(ui->midiRawArgs->text().isEmpty())
            Mix_SetLockMIDIArgs(1);
        else
        {
            Mix_SetLockMIDIArgs(0);
            musicPath += "|" + ui->midiRawArgs->text();
        }
    }
    #else
        currentMusic;
    #endif

    if(PGE_MusicPlayer::MUS_openFile(musicPath))
    {
        PGE_MusicPlayer::MUS_changeVolume(ui->volume->value());
        playSuccess = PGE_MusicPlayer::MUS_playMusic();
        ui->play->setToolTip(tr("Pause"));
        ui->play->setIcon(QIcon(":/buttons/pause.png"));
    }

    m_positionWatcher.stop();
    ui->musicPosition->setEnabled(false);
    ui->playingTimeLabel->setText("--:--:--");
    ui->playingTimeLenghtLabel->setText("/ --:--:--");

    if(playSuccess)
    {
        double total =
            #if defined(SDL_MIXER_X) || defined(SDL_MIXER_GE21)
                Mix_GetMusicTotalTime(PGE_MusicPlayer::play_mus);
            #else
                -1.0;
            #endif
        if(total > 0)
        {
            ui->musicPosition->setEnabled(true);
            ui->musicPosition->setRange(0, (int)std::ceil(total));
            ui->musicPosition->setValue(0);
            ui->playingTimeLenghtLabel->setText(QDateTime::fromTime_t((uint)std::floor(total)).toUTC().toString("/ hh:mm:ss"));
            m_positionWatcher.start(128);
        }
        ui->musicPosition->setVisible(ui->musicPosition->isEnabled());


        double loopStart =
            #if defined(SDL_MIXER_X) || defined(SDL_MIXER_GE21)
                Mix_GetMusicLoopStartTime(PGE_MusicPlayer::play_mus);
            #else
                -1.0;
            #endif
        if(loopStart >= 0.0)
            ui->isLooping->setVisible(true);

        ui->musTitle->setText(PGE_MusicPlayer::MUS_getMusTitle());
        ui->musArtist->setText(PGE_MusicPlayer::MUS_getMusArtist());
        ui->musAlbum->setText(PGE_MusicPlayer::MUS_getMusAlbum());
        ui->musCopyright->setText(PGE_MusicPlayer::MUS_getMusCopy());

        ui->gme_setup->setVisible(false);
        ui->adlmidi_xtra->setVisible(false);
        ui->opnmidi_extra->setVisible(false);
        ui->midi_setup->setVisible(false);
        ui->frame->setVisible(false);
        ui->frame->setVisible(true);//Workaround :-P
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
#ifdef SDL_MIXER_X
        case MUS_GME:
            ui->gme_setup->setVisible(true);
            ui->frame->setVisible(true);
            break;
#endif
        default:
            break;
        }
    }
    else
    {
        ui->musTitle->setText("[Unknown]");
        ui->musArtist->setText("[Unknown]");
        ui->musAlbum->setText("[Unknown]");
        ui->musCopyright->setText("[Unknown]");
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
    #ifdef SDL_MIXER_X
    if(Mix_PlayingMusic())
    {
        if((PGE_MusicPlayer::type == MUS_GME) && (m_prevTrackID != ui->trackID->value()))
        {
            PGE_MusicPlayer::MUS_stopMusic();
            on_play_clicked();
        }
    }
    #endif
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
#ifdef SDL_MIXER_X
    Mix_ADLMIDI_setTremolo(tristateToInt(ui->tremolo->checkState()));
    Mix_ADLMIDI_setVibrato(tristateToInt(ui->vibrato->checkState()));
    Mix_ADLMIDI_setAdLibMode(tristateToInt(ui->adlibMode->checkState()));
    Mix_ADLMIDI_setScaleMod(tristateToInt(ui->modulation->checkState()));
    Mix_ADLMIDI_setLogarithmicVolumes(tristateToInt(ui->logVolumes->checkState()));
#endif
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

void MusPlayer_Qt::updatePositionSlider()
{
    double pos =
#if defined(SDL_MIXER_X) || defined(SDL_MIXER_GE21)
    Mix_GetMusicPosition(PGE_MusicPlayer::play_mus);
#else
    -1.0;
#endif
    m_positionWatcherLock = true;
    ui->musicPosition->setValue((int)std::floor(pos));
    ui->playingTimeLabel->setText(QDateTime::fromTime_t((uint)std::floor(pos)).toUTC().toString("hh:mm:ss"));
    m_positionWatcherLock = false;
}

void MusPlayer_Qt::on_musicPosition_valueChanged(int value)
{
    if(m_positionWatcherLock)
        return;
    if(Mix_PlayingMusic())
    {
        Mix_SetMusicPosition((double)value);
        ui->playingTimeLabel->setText(QDateTime::fromTime_t((uint)value).toUTC().toString("hh:mm:ss"));
    }
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

#if defined(SDL_MIXER_X) || defined(SDL_MIXER_GE21)
    if(Mix_PlayChannelTimedVolume(0,
                                  m_testSfx,
                                  ui->sfx_loops->value(),
                                  ui->sfx_timed->value(),
                                  ui->sfx_volume->value()) == -1)
    {
        QMessageBox::warning(this, "SFX play error!", QString("Mix_PlayChannelTimedVolume: ") + Mix_GetError());
    }
#else
    int chan = Mix_PlayChannelTimed(0,
                                    m_testSfx,
                                    ui->sfx_loops->value(),
                                    ui->sfx_timed->value());
    if(chan == -1)
        QMessageBox::warning(this, "SFX play error!", QString("Mix_PlayChannelTimed: ") + Mix_GetError());
    else
        Mix_Volume(chan, ui->sfx_volume->value());
#endif
}

void MusPlayer_Qt::on_sfx_fadeIn_clicked()
{
    if(!m_testSfx)
        return;

#if defined(SDL_MIXER_X) || defined(SDL_MIXER_GE21)
    if(Mix_FadeInChannelTimedVolume(0,
                                    m_testSfx,
                                    ui->sfx_loops->value(),
                                    ui->sfx_fadems->value(),
                                    ui->sfx_timed->value(),
                                    ui->sfx_volume->value()) == -1)
    {
        QMessageBox::warning(this, "SFX play error!", QString("Mix_PlayChannelTimedVolume: ") + Mix_GetError());
    }
#else
    int chan = Mix_FadeInChannelTimed(0,
                                     m_testSfx,
                                     ui->sfx_loops->value(),
                                     ui->sfx_fadems->value(),
                                     ui->sfx_timed->value());
    if(chan == -1)
        QMessageBox::warning(this, "SFX play error!", QString("Mix_PlayChannelTimed: ") + Mix_GetError());
    else
        Mix_Volume(chan, ui->sfx_volume->value());
#endif
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

void MusPlayer_Qt::on_opn_bank_browse_clicked()
{
    QString path = QFileDialog::getOpenFileName(this,
                                                tr("Select WOPN bank file"),
                                                ui->opn_bank->text(),
                                                "OPN bank file by Wohlstand (*.wopn);;"
                                                "All Files (*.*)");
    if(!path.isEmpty())
    {
        ui->opn_bank->setText(path);
        on_opn_bank_editingFinished();
    }
}

void MusPlayer_Qt::on_opn_bank_editingFinished()
{
    #ifdef SDL_MIXER_X
    QString file = ui->opn_bank->text();
    if(!file.isEmpty() && QFile::exists(file) && ui->opn_use_custom->isChecked())
    {
        Mix_OPNMIDI_setCustomBankFile(file.toUtf8().data());
    } else {
        Mix_OPNMIDI_setCustomBankFile(NULL);
    }
    #endif
}

void MusPlayer_Qt::on_adl_bank_browse_clicked()
{
    #ifdef SDL_MIXER_X
    QString path = QFileDialog::getOpenFileName(this,
                                                tr("Select WOPL bank file"),
                                                ui->adl_bank->text(),
                                                "OPL3 bank file by Wohlstand (*.wopl);;"
                                                "All Files (*.*)");
    if(!path.isEmpty())
    {
        ui->adl_bank->setText(path);
        on_adl_bank_editingFinished();
    }
    #endif
}

void MusPlayer_Qt::on_adl_bank_editingFinished()
{
    #ifdef SDL_MIXER_X
    QString file = ui->adl_bank->text();
    if(!file.isEmpty() && QFile::exists(file) && ui->adl_use_custom->isChecked())
    {
        Mix_ADLMIDI_setCustomBankFile(file.toUtf8().data());
    } else {
        Mix_ADLMIDI_setCustomBankFile(NULL);
    }
    #endif
}

#endif


