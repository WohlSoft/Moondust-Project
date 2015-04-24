#ifndef NO_SDL

#include "SdlMusPlayer.h"
#include <gui/pge_msgbox.h>

/***********************************PGE_MusPlayer********************************************/
bool PGE_MusPlayer::isLoaded=false;
Mix_Music *PGE_MusPlayer::play_mus = NULL;
QString PGE_MusPlayer::currentTrack="";
int PGE_MusPlayer::volume=100;
int PGE_MusPlayer::sRate=44100;
bool PGE_MusPlayer::showMsg=true;
QString PGE_MusPlayer::showMsg_for="";
Uint64 PGE_MusPlayer::sCount = 0;
Uint64 PGE_MusPlayer::musSCount = 0;
SDL_mutex* PGE_MusPlayer::sampleCountMutex = NULL;




int PGE_MusPlayer::initAudio(int sampleRate, int allocateChannels, int bufferSize)
{
    int ret=0;
    sRate=sampleRate;
    if(isLoaded) Mix_CloseAudio();
    ret = Mix_OpenAudio(sRate, AUDIO_S16, 2, bufferSize);
    if(ret==-1) return ret;
    Mix_AllocateChannels(allocateChannels);

    // Reset the audio sample count and set the post mix callback
    if (sampleCountMutex == NULL)
    {
        sampleCountMutex = SDL_CreateMutex();
    }
    // Reset music sample count
    if (SDL_LockMutex(sampleCountMutex) == 0)
    {
        sCount = 0;
        musSCount = 0;
        Mix_SetPostMix(postMixCallback, NULL);
        SDL_UnlockMutex(sampleCountMutex);
    }
    isLoaded = true;
    return ret;
}




void PGE_MusPlayer::MUS_playMusic()
{
    if(!isLoaded) return;

	if(play_mus)
	{
		if (Mix_PlayingMusic() == 0)
		{
			// Reset music sample count
			if (SDL_LockMutex(sampleCountMutex) == 0)
			{
				musSCount = 0;
				SDL_UnlockMutex(sampleCountMutex);
			}

			Mix_PlayMusic(play_mus, -1);
		}
		else
		if(Mix_PausedMusic()==1)
		{
			Mix_ResumeMusic();
		}
	}
	else
	{
		//MessageBoxA(0, std::string(std::string("Play nothing:")+std::string(Mix_GetError())).c_str(), "Error", 0);
	}
}

void  PGE_MusPlayer::MUS_playMusicFadeIn(int ms)
{
    if(!isLoaded) return;
	if(play_mus)
    {
		if(Mix_PausedMusic()==0)
		{
			// Reset music sample count
			if (SDL_LockMutex(sampleCountMutex) == 0)
			{
				musSCount = 0;
				SDL_UnlockMutex(sampleCountMutex);
			}

			if(Mix_FadingMusic()!=MIX_FADING_IN)
				if(Mix_FadeInMusic(play_mus, -1, ms)==-1)
				{
                    PGE_MsgBox::warn(std::string(std::string("Mix_FadeInMusic:")+std::string(Mix_GetError())).c_str());
				}
		}
		else
			Mix_ResumeMusic();

    }
    else
    {
        PGE_MsgBox::warn(std::string(std::string("Play nothing:")+std::string(Mix_GetError())).c_str());
    }
}

void PGE_MusPlayer::MUS_pauseMusic()
{
    if(!isLoaded) return;
    Mix_PauseMusic();
}

void PGE_MusPlayer::MUS_stopMusic()
{
    if(!isLoaded) return;
    Mix_HaltMusic();
}

void PGE_MusPlayer::MUS_stopMusicFadeOut(int ms)
{
    if(!isLoaded) return;
	if(Mix_FadingMusic()!=MIX_FADING_OUT)
		Mix_FadeOutMusic(ms);
}


void PGE_MusPlayer::MUS_changeVolume(int vlm)
{
    if(!isLoaded) return;
    volume = vlm;
    Mix_VolumeMusic(volume);
}

bool PGE_MusPlayer::MUS_IsPlaying()
{
    if(!isLoaded) return false;
    return (Mix_PlayingMusic()==1);
}
bool PGE_MusPlayer::MUS_IsPaused()
{
    if(!isLoaded) return false;
    return (Mix_PausedMusic()==1);
}
bool PGE_MusPlayer::MUS_IsFading()
{
    if(!isLoaded) return false;
    return (Mix_FadingMusic()==1);
}

int PGE_MusPlayer::sampleRate()
{
    return sRate;
}

int PGE_MusPlayer::currentVolume()
{
    return volume;
}


void PGE_MusPlayer::MUS_openFile(QString musFile)
{
    if(!isLoaded) return;

    if(currentTrack==musFile)
	{
		if(Mix_PlayingMusic()==1)
			return;
	}

    if(play_mus!=NULL)
    {
        Mix_HaltMusic();
		Mix_FreeMusic(play_mus);
		play_mus=NULL;
    }

    play_mus = Mix_LoadMUS( musFile.toUtf8() );

    if(!play_mus)
	{
		if(showMsg_for!=musFile)
			showMsg=true;
		if(showMsg)
		{
            PGE_MsgBox::warn(QString("Mix_LoadMUS: %1\n%2").arg(musFile).arg(Mix_GetError()));
            showMsg_for = QString(musFile);
			showMsg=false;
		}
    }
	else
	{
        currentTrack = musFile;
		showMsg=true;
	}
}

void PGE_MusPlayer::postMixCallback(void *udata, Uint8 *stream, int len)
{
    if(!isLoaded) return;

    Q_UNUSED(udata);
    Q_UNUSED(stream);
	if (SDL_LockMutex(sampleCountMutex) == 0)
	{
		// This post mix callback has a simple purpose: count audio samples.
		sCount += len/4;

		// (Approximate) sample count for only when music is playing
		if ((Mix_PlayingMusic() == 1) && (Mix_PausedMusic() == 0))
		{
			musSCount += len/4;
		}
		SDL_UnlockMutex(sampleCountMutex);
	}
}

Uint64 PGE_MusPlayer::sampleCount()
{
    if(!isLoaded) return 0;
    Uint64 ret = 0;

	// Make sure we don't have a race condition with the callback
	if (SDL_LockMutex(sampleCountMutex) == 0)
	{
		ret = sCount;
		SDL_UnlockMutex(sampleCountMutex);
	}

	return ret;
}

Uint64 PGE_MusPlayer::MUS_sampleCount()
{
    if(!isLoaded) return 0;
    Uint64 ret = 0;

	// Make sure we don't have a race condition with the callback
	if (SDL_LockMutex(sampleCountMutex) == 0)
	{
		ret = musSCount;
		SDL_UnlockMutex(sampleCountMutex);
	}

    return ret;
}

void PGE_MusPlayer::freeStream()
{
    if(play_mus)
        Mix_FreeMusic(play_mus);
    play_mus=NULL;
}


/***********************************PGE_Sounds********************************************/
Mix_Chunk *PGE_Sounds::sound = NULL;
QString PGE_Sounds::current = "";

QHash<QString, Mix_Chunk* > PGE_Sounds::chunksBuffer;

Mix_Chunk *PGE_Sounds::SND_OpenSnd(QString sndFile)
{
    QString filePath = sndFile;
    Mix_Chunk* tmpChunk = NULL;
    if(!chunksBuffer.contains(filePath))
    {
        tmpChunk = Mix_LoadWAV( sndFile.toUtf8() );
        if(!tmpChunk) {
            PGE_MsgBox::warn(QString("OpenSFX: Mix_LoadWAV: %1\n%2").arg(sndFile).arg(Mix_GetError()));
        }
        chunksBuffer[filePath] = tmpChunk;
    }
    else
    {
        tmpChunk = chunksBuffer[filePath];
    }

    Mix_ReserveChannels(chunksBuffer.size()>4 ? 4: chunksBuffer.size());

    return tmpChunk;
}

void PGE_Sounds::SND_PlaySnd(QString sndFile)
{
    QString filePath = sndFile;
    if(!chunksBuffer.contains(filePath))
    {
        sound = Mix_LoadWAV( sndFile.toUtf8() );
        if(!sound) {
            PGE_MsgBox::warn(QString("PlaySND: Mix_LoadWAV: %1\n%2").arg(sndFile).arg(Mix_GetError()));
        }
        chunksBuffer[filePath] = sound;
        if(Mix_PlayChannel( -1, chunksBuffer[filePath], 0 )==-1)
        {
			if (std::string(Mix_GetError()) != "No free channels available")//Don't show overflow messagebox
            PGE_MsgBox::warn(std::string(std::string("Mix_PlayChannel: ")+std::string(Mix_GetError())).c_str());
        }
    }
    else
    {
        if(Mix_PlayChannel( -1, chunksBuffer[filePath], 0 )==-1)
        {
			if (std::string(Mix_GetError()) != "No free channels available")//Don't show overflow messagebox
            PGE_MsgBox::warn(std::string(std::string("Mix_PlayChannel: ")+std::string(Mix_GetError())).c_str());
        }
    }
}

void PGE_Sounds::clearSoundBuffer()
{
    Mix_HaltChannel(-1);
    for (QHash<QString, Mix_Chunk* >::iterator it=chunksBuffer.begin(); it!=chunksBuffer.end(); ++it)
	{
        Mix_FreeChunk((*it));
	}
	chunksBuffer.clear();
    Mix_ReserveChannels(0);
}

#endif
