/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2017 Vitaly Novichkov <admin@wohlnet.ru>
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

#include "pge_audio.h"
#include "play_music.h"

#include <gui/pge_msgbox.h>
#include <common_features/fmt_format_ne.h>

/***********************************PGE_MusPlayer********************************************/
static Mix_Music * p_playingMus = NULL;
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
		if (Mix_PlayingMusic() == 0)
		{
			// Reset music sample count
            PGE_Audio::resetMusicSampleCounter();
            Mix_PlayMusic(p_playingMus, -1);
		}
		else
        if(Mix_PausedMusic() == 1)
		{
			Mix_ResumeMusic();
		}
    }
}

std::string PGE_MusPlayer::getTitle()
{
    if(p_playingMus)
    {
        return Mix_GetMusicTitle(p_playingMus);
    }
    return std::string();
}

void  PGE_MusPlayer::fadeIn(int ms)
{
    if(!PGE_Audio::isLoaded())
        return;

    if(p_playingMus)
    {
		if(Mix_PausedMusic()==0)
		{
			// Reset music sample count
            PGE_Audio::resetMusicSampleCounter();

            if( (Mix_FadingMusic() != MIX_FADING_IN) &&
                (Mix_FadeInMusic(p_playingMus, -1, ms) == -1) )
            {
                PGE_MsgBox::warn( (std::string("Mix_FadeInMusic:") + Mix_GetError()).c_str());
            }
		}
		else
			Mix_ResumeMusic();

    }
    else
    {
        PGE_MsgBox::warn( (std::string("Play nothing:") + Mix_GetError()).c_str() );
    }
}

void PGE_MusPlayer::pause()
{
    if(!PGE_Audio::isLoaded()) return;
    Mix_PauseMusic();
}

void PGE_MusPlayer::stop()
{
    if(!PGE_Audio::isLoaded()) return;
    Mix_HaltMusic();
}

void PGE_MusPlayer::fadeOut(int ms)
{
    if(!PGE_Audio::isLoaded())
        return;

    if(Mix_FadingMusic() != MIX_FADING_OUT)
		Mix_FadeOutMusic(ms);
}


void PGE_MusPlayer::setVolume(int vlm)
{
    if(!PGE_Audio::isLoaded()) return;
    p_musVolume = vlm;
    Mix_VolumeMusic(p_musVolume);
}

bool PGE_MusPlayer::isPlaying()
{
    if(!PGE_Audio::isLoaded()) return false;
    return (Mix_PlayingMusic() == 1);
}
bool PGE_MusPlayer::isPaused()
{
    if(!PGE_Audio::isLoaded()) return false;
    return (Mix_PausedMusic() == 1);
}
bool PGE_MusPlayer::IsFading()
{
    if(!PGE_Audio::isLoaded()) return false;
    return (Mix_FadingMusic() == 1);
}

void PGE_MusPlayer::openFile(std::string musFile)
{
    if(!PGE_Audio::isLoaded())
        return;

    if( p_curMusicPath == musFile )
	{
        if(Mix_PlayingMusic() == 1)
			return;
	}

    if(p_playingMus != NULL)
    {
        Mix_HaltMusic();
        Mix_FreeMusic(p_playingMus);
        p_playingMus = NULL;
    }

    p_playingMus = Mix_LoadMUS( musFile.data() );

    if(!p_playingMus)
	{
        if( p_showMsgMus_for != musFile )
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
    p_playingMus = NULL;
}


