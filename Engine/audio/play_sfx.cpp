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
#include "play_sfx.h"

#include <gui/pge_msgbox.h>
#include <common_features/app_path.h>
#include <FileMapper/file_mapper.h>
#include <common_features/fmt_format_ne.h>

/***********************************PGE_Sounds********************************************/
std::unordered_map<std::string, Mix_Chunk*> PGE_SfxPlayer::chunksBuffer;

Mix_Chunk *PGE_SfxPlayer::openSFX(std::string sndFile)
{
    if(!PGE_Audio::isLoaded())
        return NULL;

    Mix_Chunk* tmpChunk = NULL;

    sfxHash::iterator snd = chunksBuffer.find(sndFile);
    if(snd == chunksBuffer.end())
    {
        #if defined(__unix__) || defined(__APPLE__) || defined(_WIN32)
        FileMapper fileMap;
        if( fileMap.open_file(sndFile) )
        {
            tmpChunk = Mix_LoadWAV_RW(SDL_RWFromMem(fileMap.data(), fileMap.size()), fileMap.size());
            fileMap.close_file();
        }
        #else
        tmpChunk = Mix_LoadWAV( sndFile.c_str() );
        #endif
        if(!tmpChunk)
        {
            PGE_MsgBox::warn(fmt::format_ne("OpenSFX: Mix_LoadWAV: {0}\n{1}", sndFile, Mix_GetError()));
        }
        chunksBuffer.insert({sndFile, tmpChunk});
    }
    else
    {
        tmpChunk = snd->second;
    }

    Mix_ReserveChannels(chunksBuffer.size()>4 ? 4: chunksBuffer.size());

    return tmpChunk;
}

void PGE_SfxPlayer::playFile(std::string sndFile)
{
    if(!PGE_Audio::isLoaded())
        return;

    Mix_Chunk* sound;
    sfxHash::iterator snd = chunksBuffer.find(sndFile);
    if(snd == chunksBuffer.end())
    {
        sound = Mix_LoadWAV( sndFile.c_str() );
        if(!sound)
        {
            PGE_MsgBox::warn(fmt::format_ne("PlaySND: Mix_LoadWAV: {0}\n{1}", sndFile, Mix_GetError()));
            return;
        }
        chunksBuffer.insert({sndFile, sound});
        if(Mix_PlayChannel( -1, sound, 0 )==-1)
        {
            const char* err = Mix_GetError();
            if (strcmp(err, "No free channels available") != 0)//Don't show overflow messagebox
                PGE_MsgBox::warn((std::string("Mix_PlayChannel: ")+std::string(Mix_GetError())).c_str());
            return;
        }
    }
    else
    {
        if(Mix_PlayChannel( -1, snd->second, 0 ) == -1)
        {
            const char* err = Mix_GetError();
            if(strcmp(err, "No free channels available") != 0)//Don't show overflow messagebox
                PGE_MsgBox::warn( (std::string("Mix_PlayChannel: ") + err).c_str());
        }
    }
}

void PGE_SfxPlayer::clearSoundBuffer()
{
    if(!PGE_Audio::isLoaded())
        return;

    Mix_HaltChannel(-1);
    for(sfxHash::iterator   it = chunksBuffer.begin();
                            it != chunksBuffer.end();
                            ++it)
    {
        Mix_FreeChunk((it->second));
    }

    chunksBuffer.clear();
    Mix_ReserveChannels(0);
}
