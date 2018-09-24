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
#include <QMoveEvent>
#include <cmath>

#include "ui_mainwindow.h"
#include "musplayer_qt.h"
#include "../Player/mus_player.h"
#include "../AssocFiles/assoc_files.h"
#include "../Effects/reverb.h"
#include <math.h>
#include "../version.h"

#include "sfx_tester.h"
#include "setup_midi.h"
#include "seek_bar.h"

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

    QString title = windowTitle();
    /* Append library version to the title */
    const SDL_version *mixer_ver = Mix_Linked_Version();
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

    m_sfxTester = new SfxTester(this);
    m_sfxTester->setModal(false);

    m_setupMidi = new SetupMidi(this);
    m_setupMidi->setModal(false);

    connect(m_setupMidi, &SetupMidi::songRestartNeeded, this, &MusPlayer_Qt::restartMusic);

    m_seekBar = new SeekBar(this);
    ui->gridLayout->removeWidget(ui->musicPosition);
    ui->gridLayout->addWidget(m_seekBar, 5, 0, 1, 2);
    m_seekBar->setLength(100);
    m_seekBar->setVisible(true);
    ui->musicPosition->setVisible(false);

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

    connect(m_seekBar, &SeekBar::positionSeeked, this, &MusPlayer_Qt::musicPosition_seeked);


    //connect(ui->playListPush, &QPushButton::clicked, this, &MusPlayer_Qt::playList_pushCurrent);
    //connect(ui->playListPop, &QPushButton::clicked, this, &MusPlayer_Qt::playList_popCurrent);

    QApplication::setOrganizationName(V_COMPANY);
    QApplication::setOrganizationDomain(V_PGE_URL);
    QApplication::setApplicationName("PGE Music Player");
    //ui->playList->setModel(&playList);

    QSettings setup;
    m_seekBar->setEnabled(false);
    ui->isLooping->setVisible(false);
    //ui->playList->setVisible(false);
    //ui->playListPush->setVisible(false);
    //ui->playListPop->setVisible(false);

    ui->volume->setValue(setup.value("Volume", 128).toInt());
    m_prevTrackID = ui->trackID->value();    
    ui->gme_setup->setEnabled(false);

    currentMusic = setup.value("RecentMusic", "").toString();
    restoreGeometry(setup.value("Window-Geometry").toByteArray());
    layout()->activate();
    adjustSize();

    m_setupMidi->loadSetup();
}

MusPlayer_Qt::~MusPlayer_Qt()
{
    on_stop_clicked();

    m_sfxTester->close();

    m_setupMidi->close();
    m_setupMidi->saveSetup();

    Mix_CloseAudio();

    QSettings setup;
    setup.setValue("Window-Geometry", saveGeometry());
    setup.setValue("Volume", ui->volume->value());
    setup.setValue("RecentMusic", currentMusic);
    setup.sync();

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

void MusPlayer_Qt::moveEvent(QMoveEvent *event)
{
    if(m_oldWindowPos.isNull())
        m_oldWindowPos = event->oldPos();

    int deltaX = event->pos().x() - m_oldWindowPos.x();
    int deltaY = event->pos().y() - m_oldWindowPos.y();
    {
        QRect g = m_sfxTester->frameGeometry();
        m_sfxTester->move(g.x() + deltaX, g.y() + deltaY);
    }
    {
        QRect g = m_setupMidi->frameGeometry();
        m_setupMidi->move(g.x() + deltaX, g.y() + deltaY);
    }

    m_oldWindowPos = event->pos();
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

    QAction *sfx_testing_show = x.addAction("Show SFX testing");
    sfx_testing_show->setEnabled(!m_sfxTester->isVisible());

    QAction *midi_setup_show = x.addAction("Show MIDI setup");
    midi_setup_show->setEnabled(!m_setupMidi->isVisible());

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
        ui->actionEnableReverb->setChecked(reverb->isChecked());
        on_actionEnableReverb_triggered(reverb->isChecked());
    }
    else if(assoc_files == ret)
        on_actionFileAssoc_triggered();
    //    else if(ret == play_list)
    //    {
    //        setPlayListMode(!playListMode);
    //    }
    else if(ret == sfx_testing_show)
        on_actionSfxTesting_triggered();
    else if(ret == midi_setup_show)
        on_actionMidiSetup_triggered();
    else if(ret == license)
        on_actionHelpLicense_triggered();
    else if(ret == source)
        on_actionHelpGitHub_triggered();
}

void MusPlayer_Qt::openMusicByArg(QString musPath)
{
    if(ui->recordWav->isChecked()) return;

    currentMusic = musPath;
    //ui->recordWav->setEnabled(!currentMusic.endsWith(".wav", Qt::CaseInsensitive));//Avoid self-trunkling!
    PGE_MusicPlayer::MUS_stopMusic();
    on_play_clicked();
}

void MusPlayer_Qt::restartMusic()
{
    if(Mix_PlayingMusicStream(PGE_MusicPlayer::play_mus))
    {
        PGE_MusicPlayer::MUS_stopMusic();
        on_play_clicked();
    }
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
    m_seekBar->setPosition(0.0);
    m_seekBar->setEnabled(false);
    PGE_MusicPlayer::MUS_stopMusic();
    ui->play->setToolTip(tr("Play"));
    ui->play->setIcon(QIcon(":/buttons/play.png"));

    if(ui->recordWav->isChecked())
    {
        ui->recordWav->setChecked(false);
        PGE_MusicPlayer::stopWavRecording();
        ui->open->setEnabled(true);
        ui->play->setEnabled(true);
        m_blinker.stop();
        ui->recordWav->setStyleSheet("");
    }
}

void MusPlayer_Qt::on_play_clicked()
{
    if(Mix_PlayingMusicStream(PGE_MusicPlayer::play_mus))
    {
        if(Mix_PausedMusicStream(PGE_MusicPlayer::play_mus))
        {
            Mix_ResumeMusicStream(PGE_MusicPlayer::play_mus);
            ui->play->setToolTip(tr("Pause"));
            ui->play->setIcon(QIcon(":/buttons/pause.png"));
            return;
        }
        else
        {
            Mix_PauseMusicStream(PGE_MusicPlayer::play_mus);
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
    QString midiRawArgs = m_setupMidi->getRawMidiArgs();
    if(ui->gme_setup->isEnabled())
        musicPath += "|" + ui->trackID->text();
    else if((PGE_MusicPlayer::type == MUS_MID || PGE_MusicPlayer::type == MUS_ADLMIDI))
    {
        if(midiRawArgs.isEmpty())
            Mix_SetLockMIDIArgs(1);
        else
        {
            Mix_SetLockMIDIArgs(0);
            musicPath += "|" + midiRawArgs;
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
    m_seekBar->setEnabled(false);
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
        double loopStart =
#if defined(SDL_MIXER_X) || defined(SDL_MIXER_GE21)
            Mix_GetMusicLoopStartTime(PGE_MusicPlayer::play_mus);
#else
            -1.0;
#endif
        double loopEnd =
#if defined(SDL_MIXER_X) || defined(SDL_MIXER_GE21)
            Mix_GetMusicLoopEndTime(PGE_MusicPlayer::play_mus);
#else
            -1.0;
#endif
        m_seekBar->clearLoopPoints();
        m_seekBar->setEnabled(false);

        if(total > 0)
        {
            m_seekBar->setEnabled(true);
            m_seekBar->setLength(total);
            m_seekBar->setPosition(0.0);
            m_seekBar->setLoopPoints(loopStart, loopEnd);
            ui->playingTimeLenghtLabel->setText(QDateTime::fromTime_t((uint)std::floor(total)).toUTC().toString("/ hh:mm:ss"));
            m_positionWatcher.start(128);
        }
        // ui->musicPosition->setVisible(ui->musicPosition->isEnabled());

        if(loopStart >= 0.0)
            ui->isLooping->setVisible(true);

        ui->musTitle->setText(PGE_MusicPlayer::MUS_getMusTitle());
        ui->musArtist->setText(PGE_MusicPlayer::MUS_getMusArtist());
        ui->musAlbum->setText(PGE_MusicPlayer::MUS_getMusAlbum());
        ui->musCopyright->setText(PGE_MusicPlayer::MUS_getMusCopy());

        ui->gme_setup->setEnabled(false);

        ui->smallInfo->setText(PGE_MusicPlayer::musicType());
        ui->gridLayout->update();

        switch(PGE_MusicPlayer::type)
        {
#ifdef SDL_MIXER_X
        case MUS_GME:
            ui->gme_setup->setEnabled(true);
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
}

void MusPlayer_Qt::on_trackID_editingFinished()
{
#ifdef SDL_MIXER_X
    if(Mix_PlayingMusicStream(PGE_MusicPlayer::play_mus))
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
        m_blinker.start(500);
    }
    else
    {
        on_stop_clicked();
        PGE_MusicPlayer::stopWavRecording();
        ui->open->setEnabled(true);
        ui->play->setEnabled(true);
        m_blinker.stop();
        ui->recordWav->setStyleSheet("");
    }
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
    m_seekBar->setPosition(pos);
    ui->playingTimeLabel->setText(QDateTime::fromTime_t((uint)std::floor(pos)).toUTC().toString("hh:mm:ss"));
    m_positionWatcherLock = false;
}

void MusPlayer_Qt::musicPosition_seeked(double value)
{
    if(m_positionWatcherLock)
        return;

    qDebug() << "Seek to: " << value;

    if(Mix_PlayingMusicStream(PGE_MusicPlayer::play_mus))
    {
        Mix_SetMusicStreamPosition(PGE_MusicPlayer::play_mus, value);
        ui->playingTimeLabel->setText(QDateTime::fromTime_t((uint)value).toUTC().toString("hh:mm:ss"));
    }
}

#endif



void MusPlayer_Qt::on_actionOpen_triggered()
{
    on_open_clicked();
}

void MusPlayer_Qt::on_actionQuit_triggered()
{
    this->close();
}

void MusPlayer_Qt::on_actionHelpLicense_triggered()
{
    QDesktopServices::openUrl(QUrl("http://www.gnu.org/licenses/gpl"));
}

void MusPlayer_Qt::on_actionHelpAbout_triggered()
{
    QString library;
    /* Append library version to the title */
    const SDL_version *mixer_ver = Mix_Linked_Version();
#if defined(SDL_MIXER_X)
    library += QString("SDL Mixer X %1.%2.%3")
             .arg(mixer_ver->major)
             .arg(mixer_ver->minor)
             .arg(mixer_ver->patch);
#else
    library += QString("SDL Mixer %1.%2.%3")
             .arg(mixer_ver->major)
             .arg(mixer_ver->minor)
             .arg(mixer_ver->patch);
#endif

    QMessageBox::about(this, tr("SDL Mixer X Music Player"),
        tr("SDL Mixer X Music Player\n\n"
           "Version %1\n\n"
           "Linked library: %2")
            .arg(V_FILE_VERSION)
            .arg(library)
    );
}

void MusPlayer_Qt::on_actionHelpGitHub_triggered()
{
    QDesktopServices::openUrl(QUrl("https://github.com/WohlSoft/PGE-Project"));
}

void MusPlayer_Qt::on_actionMidiSetup_triggered()
{
    m_setupMidi->show();
    QRect g = this->frameGeometry();
    m_setupMidi->move(g.right(), g.top());
    m_setupMidi->update();
    m_setupMidi->repaint();
}

void MusPlayer_Qt::on_actionSfxTesting_triggered()
{
    m_sfxTester->show();
    QRect g = this->frameGeometry();
    m_sfxTester->move(g.left(), g.bottom());
    m_sfxTester->update();
    m_sfxTester->repaint();
}

void MusPlayer_Qt::on_actionEnableReverb_triggered(bool checked)
{
    PGE_MusicPlayer::reverbEnabled = checked;
    if(PGE_MusicPlayer::reverbEnabled)
        Mix_RegisterEffect(MIX_CHANNEL_POST, reverbEffect, reverbEffectDone, NULL);
    else
        Mix_UnregisterEffect(MIX_CHANNEL_POST, reverbEffect);
}

void MusPlayer_Qt::on_actionFileAssoc_triggered()
{
    AssocFiles af(this);
    af.setWindowModality(Qt::WindowModal);
    af.exec();
}
