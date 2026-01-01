/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2014-2026 Vitaly Novichkov <admin@wohlnet.ru>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifdef Q_OS_MACX
#undef Q_OS_MACX //Workaroud for an AUTOMOC
#endif

#include <ui_mainwindow.h>
#include <mainwindow.h>

#include "sdl_music_player.h"
#include "music_player.h"
#include <main_window/dock/lvl_sctc_props.h>
#include <Utils/dir_list_ci_qt.h>

QString LvlMusPlay::currentCustomMusic;


QString LvlMusPlay::currentMusicPath;
QString LvlMusPlay::lastMusicPath;


long LvlMusPlay::currentMusicId = 0;
long LvlMusPlay::currentWldMusicId = 0;
long LvlMusPlay::currentSpcMusicId = 0;
bool LvlMusPlay::musicButtonChecked;
bool LvlMusPlay::musicForceReset = false;
int LvlMusPlay::musicType = LvlMusPlay::LevelMusic;
static bool s_previewActive = false;


void MainWindow::on_actionPlayMusic_triggered(bool checked)
{
    LogDebug("Clicked play music button");
    setMusic(checked);
}

static void getRootAndData(MainWindow *mw, QString &root, QString &data)
{
    if(mw->activeChildWindow() == MainWindow::WND_Level)
    {
        auto *w = mw->activeLvlEditWin();
        if(w != nullptr)
        {
            root = w->LvlData.meta.path + "/";
            data = w->LvlData.meta.filename + "/";
        }
    }
    else if(mw->activeChildWindow() == MainWindow::WND_World)
    {
        auto *w = mw->activeWldEditWin();
        if(w != nullptr)
        {
            root = w->WldData.meta.path + "/";
            data = w->WldData.meta.filename + "/";
        }
    }
}

static void processPathArguments(const QString &root, const QString &data, const QString &cpMusic, QString &inoutPath, QString &outArgs)
{
    outArgs.clear();

    if(inoutPath.contains('|'))
    {
        QStringList x = inoutPath.split("|");
        inoutPath = x[0];
        outArgs = x[1];
        // Replace macros with the absoulte paths:
        outArgs.replace("{e}", root); // Episode root
        outArgs.replace("{d}", root + data); // Level/World data directory
        outArgs.replace("{r}", cpMusic); // Config pack music root
    }
}

// TODO: Refactor this
void LvlMusPlay::setMusic(MainWindow *mw, LvlMusPlay::MusicType mt, unsigned long id, QString cmus)
{
    QString root = "./";
    QString data;
    if(!mw)
        return;

    if(id == 0)
    {
        setNoMusic();
        return;
    }

    getRootAndData(mw, root, data);

    DirListCIQt ci(root);

    //Force correction of Windows paths into UNIX style
    cmus.replace('\\', '/');

    switch(mt)
    {
    case LevelMusic:
        if(id == mw->configs.music_custom_id)
        {
            LogDebug(QString("get Custom music path"));
            currentMusicPath = root + ci.resolveFileCase(cmus);
        }
        else
        {
            LogDebug(QString("get standart music path (level)"));
            QString musicFile;
            long j = mw->configs.getMusLvlI(id);
            if(j >= 0)
            {
                if(id == mw->configs.main_music_lvl[static_cast<int>(j)].id)
                    musicFile = mw->configs.main_music_lvl[static_cast<int>(j)].file;
            }
            currentMusicPath = mw->configs.dirs.music + musicFile;
        }
        break;
    case WorldMusic:
        if(id == mw->configs.music_w_custom_id)
        {
            LogDebug(QString("get Custom music path"));
            currentMusicPath = root + ci.resolveFileCase(cmus);
        }
        else
        {
            LogDebug(QString("get standart music path (world)"));
            QString musicFile;
            long j = mw->configs.getMusWldI(id);
            if(j >= 0)
            {
                if(id == mw->configs.main_music_wld[static_cast<int>(j)].id)
                    musicFile = mw->configs.main_music_wld[static_cast<int>(j)].file;
            }
            currentMusicPath = mw->configs.dirs.music + musicFile;
        }
        break;
    case SpecialMusic:
    {
        LogDebug(QString("get standart music path (special)"));
        QString musicFile;
        long j = mw->configs.getMusSpcI(id);
        if(j >= 0)
        {
            if(id == mw->configs.main_music_spc[static_cast<int>(j)].id)
                musicFile = mw->configs.main_music_spc[static_cast<int>(j)].file;
        }
        currentMusicPath = mw->configs.dirs.music + musicFile;
    }
    break;
    }

    LogDebug(QString("path is %1").arg(currentMusicPath));

    QString pathArguments;
    processPathArguments(root, data, mw->configs.dirs.music, currentMusicPath, pathArguments);

    QFileInfo mus(currentMusicPath);

    if((!mus.exists()) || (!mus.isFile()))
        currentMusicPath.clear();
    else
    {
        if(!pathArguments.isEmpty())
            currentMusicPath = currentMusicPath + "|" + pathArguments;
    }
}

void LvlMusPlay::setNoMusic()
{
    currentMusicPath.clear();
}

void LvlMusPlay::setTempo(double tempo)
{
    PGE_MusPlayer::setTempo(tempo);
}

void LvlMusPlay::setGain(double gain)
{
    PGE_MusPlayer::setGain(gain);
}

// TODO: Refactor this
void LvlMusPlay::updateMusic(MainWindow *mw)
{
    Q_ASSERT(mw);
    int w_id = mw->activeChildWindow();

    switch(w_id)
    {
    case MainWindow::WND_NoWindow:
        setNoMusic();
        updatePlayerState(mw, false);
        break;
    case MainWindow::WND_Level:
        mw->dock_LvlSectionProps->loadMusic();
        break;
    case MainWindow::WND_NpcTxt:
        setNoMusic();
        updatePlayerState(mw, false);
        break;
    case MainWindow::WND_World:
    {
        WorldEdit *w = mw->activeWldEditWin();
        if(!w)
            return;
        setMusic(mw, LvlMusPlay::WorldMusic, static_cast<unsigned long>(w->currentMusic), w->currentCustomMusic);
        mw->setMusic();
    }
    break;
    default:
        break;
    }
}

void LvlMusPlay::updatePlayerState(MainWindow *mw, bool playing, bool force)
{
    if(s_previewActive)
    {
        force = true;
        s_previewActive = false;
    }

    if(!LvlMusPlay::musicForceReset && !force)
    {
        if(
            (!lastMusicPath.isNull()) &&
            (lastMusicPath == currentMusicPath) &&
            (musicButtonChecked == playing)
        )
            return;

    }
    else
        musicForceReset = false;

    if(!playing || (currentMusicPath.isNull()))
    {
        PGE_MusPlayer::stop();
        PGE_MusPlayer::closeFile();
    }
    else
    {
        PGE_MusPlayer::stop();
        PGE_MusPlayer::openFile(currentMusicPath);
        PGE_MusPlayer::changeVolume(mw->musicVolume());
        PGE_MusPlayer::play();
    }

    lastMusicPath = currentMusicPath;
    musicButtonChecked  = playing;
}

void LvlMusPlay::stopMusic(MainWindow *mw)
{
    setNoMusic();
    updatePlayerState(mw, false);
}

void LvlMusPlay::previewCustomMusic(MainWindow *mw, QString cmus)
{
    QString root = "./";
    QString data;
    QString previewCurrentMusicPath;

    if(!mw)
        return;

    getRootAndData(mw, root, data);

    DirListCIQt ci(root);

    //Force correction of Windows paths into UNIX style
    cmus.replace('\\', '/');

    LogDebug(QString("get Custom music path"));
    previewCurrentMusicPath = root + ci.resolveFileCase(cmus);

    LogDebug(QString("path is %1").arg(previewCurrentMusicPath));

    QString pathArguments;
    processPathArguments(root, data, mw->configs.dirs.music, previewCurrentMusicPath, pathArguments);

    QFileInfo mus(previewCurrentMusicPath);

    if((!mus.exists()) || (!mus.isFile()))
        return;

    if(!pathArguments.isEmpty())
        previewCurrentMusicPath = previewCurrentMusicPath + "|" + pathArguments;

    previewCustomMusicAbs(previewCurrentMusicPath);
    s_previewActive = true;
}

void LvlMusPlay::previewCustomMusicAbs(QString absMusicPath)
{
    PGE_MusPlayer::stop();
    PGE_MusPlayer::openFile(absMusicPath);
    PGE_MusPlayer::sendVolume();
    PGE_MusPlayer::play();
    s_previewActive = true;
}

void LvlMusPlay::previewSilence()
{
    s_previewActive = true;
    PGE_MusPlayer::stop();
}

void LvlMusPlay::previewReset(MainWindow *mw)
{
    if(s_previewActive)
    {
        updatePlayerState(mw, musicButtonChecked, true);
        s_previewActive = false;
    }
}

/*******************************************************************************/
void MainWindow::setMusic(bool checked)
{
    checked = ui->actionPlayMusic->isChecked();

    if(configs.check())
    {
        LogCritical(QString("Error! *.INI Configs for music not loaded"));
        return;
    }

    LvlMusPlay::updatePlayerState(this, checked);
}

void MainWindow::setMusicButton(bool checked)
{
    ui->actionPlayMusic->setChecked(checked);
}

int MainWindow::musicVolume()
{
    if(m_ui_musicVolume)
        return m_ui_musicVolume->value();
    else
        return 128;
}

