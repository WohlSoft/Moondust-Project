/*
 * Moondust, a free game engine for platform game making
 * Copyright (c) 2014-2021 Vitaly Novichkov <admin@wohlnet.ru>
 *
 * This software is licensed under a dual license system (MIT or GPL version 3 or later).
 * This means you are free to choose with which of both licenses (MIT or GPL version 3 or later)
 * you want to use this software.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *
 * You can see text of MIT license in the LICENSE.mit file you can see in Engine folder,
 * or see https://mit-license.org/.
 *
 * You can see text of GPLv3 license in the LICENSE.gpl3 file you can see in Engine folder,
 * or see <http://www.gnu.org/licenses/>.
 */

#include "pge_audio.h"
#include "play_music.h"

#include <gui/pge_msgbox.h>
#include <common_features/fmt_format_ne.h>
#include <data_configs/config_manager.h>
#include <Utils/strings.h>

/***********************************PGE_MusPlayer********************************************/
static Mix_Music *p_playingMus = nullptr;
static std::string p_curMusicPath = "";
static int         p_musVolume = 127;
static bool        p_showMsgMus = true;
static std::string p_showMsgMus_for = "";
/********************************************************************************************/

void PGE_MusPlayer::play()
{
    if(!PGE_Audio::isLoaded())
        return;

    if(p_playingMus)
    {
        if(Mix_PlayingMusicStream(p_playingMus) == 0)
        {
            // Reset music sample count
            PGE_Audio::resetMusicSampleCounter();
            Mix_PlayMusic(p_playingMus, -1);
        }
        else if(Mix_PausedMusicStream(p_playingMus) == 1)
            Mix_ResumeMusicStream(p_playingMus);
    }
}

std::string PGE_MusPlayer::getTitle()
{
    if(p_playingMus)
        return Mix_GetMusicTitle(p_playingMus);
    return std::string();
}

void PGE_MusPlayer::processPathArgs(std::string &path,
                                    const std::string &episodeRoot,
                                    const std::string &dataDirName)
{
    if(path.find('|') == std::string::npos)
        return; // Nothing to do
    Strings::List p;
    Strings::split(p, path, '|');
    Strings::replaceInAll(p[1], "{e}", episodeRoot);
    Strings::replaceInAll(p[1], "{d}", episodeRoot + dataDirName);
    Strings::replaceInAll(p[1], "{r}", ConfigManager::dirs.music);
    path = p[0] + "|" + p[1];
}

void  PGE_MusPlayer::fadeIn(int ms)
{
    if(!PGE_Audio::isLoaded())
        return;

    if(p_playingMus)
    {
        if(Mix_PausedMusicStream(p_playingMus) == 0)
        {
            // Reset music sample count
            PGE_Audio::resetMusicSampleCounter();

            if((Mix_FadingMusicStream(p_playingMus) != MIX_FADING_IN) &&
               (Mix_FadeInMusic(p_playingMus, -1, ms) == -1))
                PGE_MsgBox::warn(std::string("Mix_FadeInMusic:") + Mix_GetError());
        }
        else
            Mix_ResumeMusicStream(p_playingMus);

    }
    else
        PGE_MsgBox::warn(std::string("Play nothing:") + Mix_GetError());
}

void PGE_MusPlayer::pause()
{
    if(!PGE_Audio::isLoaded()) return;
    Mix_PauseMusicStream(p_playingMus);
}

void PGE_MusPlayer::stop()
{
    if(!PGE_Audio::isLoaded()) return;
    Mix_HaltMusicStream(p_playingMus);
}

void PGE_MusPlayer::fadeOut(int ms)
{
    if(!PGE_Audio::isLoaded())
        return;

    if(Mix_FadingMusicStream(p_playingMus) != MIX_FADING_OUT)
        Mix_FadeOutMusicStream(p_playingMus, ms);
}


void PGE_MusPlayer::setVolume(int vlm)
{
    if(!PGE_Audio::isLoaded()) return;
    p_musVolume = vlm;
    Mix_VolumeMusicStream(p_playingMus, p_musVolume);
}

bool PGE_MusPlayer::isPlaying()
{
    if(!PGE_Audio::isLoaded()) return false;
    return (Mix_PlayingMusicStream(p_playingMus) == 1);
}
bool PGE_MusPlayer::isPaused()
{
    if(!PGE_Audio::isLoaded()) return false;
    return (Mix_PausedMusicStream(p_playingMus) == 1);
}
bool PGE_MusPlayer::isFading()
{
    if(!PGE_Audio::isLoaded()) return false;
    return (Mix_FadingMusicStream(p_playingMus) == 1);
}

void PGE_MusPlayer::openFile(std::string musFile)
{
    if(!PGE_Audio::isLoaded())
        return;

    if(p_curMusicPath == musFile)
    {
        if(Mix_PlayingMusicStream(p_playingMus) == 1)
            return;
    }

    if(p_playingMus != nullptr)
    {
        Mix_HaltMusicStream(p_playingMus);
        Mix_FreeMusic(p_playingMus);
        p_playingMus = nullptr;
    }

    p_playingMus = Mix_LoadMUS(musFile.data());

    if(!p_playingMus)
    {
        if(p_showMsgMus_for != musFile)
            p_showMsgMus = true;

        if(p_showMsgMus)
        {
            PGE_MsgBox::warn(
                fmt::format_ne("Mix_LoadMUS: {0}\n{1}",
                               musFile, Mix_GetError())
            );
            p_showMsgMus_for = std::string(musFile);
            p_showMsgMus = false;
        }
    }
    else
    {
        p_curMusicPath = musFile;
        p_showMsgMus = true;
    }
}


void PGE_MusPlayer::freeStream()
{
    if(p_playingMus)
        Mix_FreeMusic(p_playingMus);
    p_playingMus = nullptr;
}
