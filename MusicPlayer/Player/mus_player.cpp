#include "mus_player.h"

#ifndef MUSPLAY_USE_WINAPI
#include <QMessageBox>
#include "../MainWindow/musplayer_qt.h"
#endif

#include "../wave_writer.h"

/*!
 *  SDL Mixer wrapper
 */
namespace PGE_MusicPlayer
{
    Mix_Music *play_mus = NULL;
    Mix_MusicType type  = MUS_NONE;
    bool reverbEnabled = false;

    bool g_playlistMode = false;
    bool g_skipPlayList = false;

    #ifndef MUSPLAY_USE_WINAPI
    MusPlayer_Qt* mw = NULL;
    void setMainWindow(void *mwp)
    {
        mw = reinterpret_cast<MusPlayer_Qt*>(mwp);
    }
    #endif

    const char *musicTypeC()
    {
        return (
                   type == MUS_NONE ? "No Music" :
                   type == MUS_CMD ? "CMD" :
                   type == MUS_WAV ? "PCM Wave" :
                   type == MUS_MOD ? "Tracker music" :
                   type == MUS_MID ? "MIDI" :
                   type == MUS_OGG ? "OGG" :
                   type == MUS_MP3 ? "MP3" :
                   type == MUS_FLAC ? "FLAC" :
#ifdef SDL_MIXER_X
#   if SDL_MIXER_MAJOR_VERSION > 2 || (SDL_MIXER_MAJOR_VERSION == 2 && SDL_MIXER_MINOR_VERSION >= 2)
                   type == MUS_OPUS ? "OPUS" :
#   endif
                   type == MUS_ADLMIDI ? "IMF/MUS/XMI" :
                   type == MUS_GME ? "Game Music Emulator" :
#endif
                   "<Unknown>");
    }
    QString musicType()
    {
        return QString(musicTypeC());
    }

    /*!
     * \brief Spawn warning message box with specific text
     * \param msg text to spawn in message box
     */
    static void error(QString msg)
    {
        #ifndef MUSPLAY_USE_WINAPI
        QMessageBox::warning(nullptr, "SDL2 Mixer ext error", msg, QMessageBox::Ok);
        #else
        MessageBoxA(NULL, msg.c_str(), "SDL2 Mixer ext error", MB_OK | MB_ICONWARNING);
        #endif
    }

    /*!
     * \brief Stop music playing
     */
    void MUS_stopMusic()
    {
        g_skipPlayList = true;
        Mix_HaltMusic();
        g_skipPlayList = false;
    }

    #ifndef MUSPLAY_USE_WINAPI
    /*!
     * \brief Get music title of current track
     * \return music title of current music file
     */
    QString MUS_getMusTitle()
    {
        #if defined(SDL_MIXER_X) || defined(SDL_MIXER_GE21)
        if(play_mus)
            return QString(Mix_GetMusicTitle(play_mus));
        else
        #endif
            return QString("[No music]");
    }

    /*!
     * \brief Get music artist tag text of current music track
     * \return music artist tag text of current music track
     */
    QString MUS_getMusArtist()
    {
        #if defined(SDL_MIXER_X) || defined(SDL_MIXER_GE21)
        if(play_mus)
            return QString(Mix_GetMusicArtistTag(play_mus));
        else
        #endif
            return QString("[Unknown Artist]");
    }

    /*!
     * \brief Get music album tag text of current music track
     * \return music ablum tag text of current music track
     */
    QString MUS_getMusAlbum()
    {
        #if defined(SDL_MIXER_X) || defined(SDL_MIXER_GE21)
        if(play_mus)
            return QString(Mix_GetMusicAlbumTag(play_mus));
        else
        #endif
            return QString("[Unknown Album]");
    }

    /*!
     * \brief Get music copyright tag text of current music track
     * \return music copyright tag text of current music track
     */
    QString MUS_getMusCopy()
    {
        #if defined(SDL_MIXER_X) || defined(SDL_MIXER_GE21)
        if(play_mus)
            return QString(Mix_GetMusicCopyrightTag(play_mus));
        else
        #endif
            return QString("");
    }
    #else
    /*!
     * \brief Get music title of current track
     * \return music title of current music file
     */
    const char *MUS_getMusTitle()
    {
        if(play_mus)
            return Mix_GetMusicTitle(play_mus);
        else
            return "[No music]";
    }

    /*!
     * \brief Get music artist tag text of current music track
     * \return music artist tag text of current music track
     */
    const char *MUS_getMusArtist()
    {
        if(play_mus)
            return Mix_GetMusicArtistTag(play_mus);
        else
            return "[Unknown Artist]";
    }

    /*!
     * \brief Get music album tag text of current music track
     * \return music ablum tag text of current music track
     */
    const char *MUS_getMusAlbum()
    {
        if(play_mus)
            return Mix_GetMusicAlbumTag(play_mus);
        else
            return "[Unknown Album]";
    }

    /*!
     * \brief Get music copyright tag text of current music track
     * \return music copyright tag text of current music track
     */
    const char *MUS_getMusCopy()
    {
        if(play_mus)
            return Mix_GetMusicCopyrightTag(play_mus);
        else
            return "";
    }
    #endif

    void playListProcess()
    {
        if(g_skipPlayList)
            return;
        #ifndef MUSPLAY_USE_WINAPI
        if(mw)
            QMetaObject::invokeMethod(mw, "playListNext", Qt::QueuedConnection);
        #endif
    }

    void setPlayListMode(bool playList)
    {
        g_playlistMode = playList;
        if(playList)
            Mix_HookMusicFinished(&playListProcess);
        else
            Mix_HookMusicFinished(NULL);
    }

    /*!
     * \brief Start playing of currently opened music track
     */
    bool MUS_playMusic()
    {
        if(play_mus)
        {
            if(Mix_PlayMusic(play_mus, g_playlistMode ? 0 : -1) == -1)
            {
                error(QString("Mix_PlayMusic: ") + Mix_GetError());
                return false;
                // well, there's no music, but most games don't break without music...
            }
            //QString("Music is %1\n").arg(Mix_PlayingMusic()==1?"Playing":"Silence");
        }
        else
        {
            error(QString("Play nothing: Mix_PlayMusic: ") + Mix_GetError());
            return false;
        }
        //qDebug() << QString("Check Error of SDL: %1\n").arg(Mix_GetError());
        return true;
    }

    /*!
     * \brief Sets volume level of current music stream
     * \param volume level of volume from 0 tp 128
     */
    void MUS_changeVolume(int volume)
    {
        Mix_VolumeMusic(volume);
        DebugLog(QString("Mix_VolumeMusic: %1\n").arg(volume));
    }

    /*!
     * \brief Open music file
     * \param musFile Full path to music file
     * \return true if music file was successfully opened, false if loading was failed
     */
    bool MUS_openFile(QString musFile)
    {
        type = MUS_NONE;
        if(play_mus != NULL)
        {
            Mix_FreeMusic(play_mus);
            play_mus = NULL;
        }

        #ifndef MUSPLAY_USE_WINAPI
        QByteArray p = musFile.toUtf8();
        play_mus = Mix_LoadMUS(p.data());
        #else
        play_mus = Mix_LoadMUS(musFile.c_str());
        #endif
        if(!play_mus)
        {
            error(QString("Mix_LoadMUS(\"" + QString(musFile) + "\"): ") + Mix_GetError());
            return false;
        }
        type = Mix_GetMusicType(play_mus);
        DebugLog(QString("Music type: %1").arg(musicType()));
        return true;
    }

    static bool wavOpened = false;

    // make a music play function
    // it expects udata to be a pointer to an int
    static void myMusicPlayer(void * /*udata*/, Uint8 *stream, int len)
    {
        wave_write((short *)stream, len / 2);
    }

    void startWavRecording(QString target)
    {
        if(wavOpened) return;
        if(!play_mus) return;

        /* Record 20 seconds to wave file */
        #ifndef MUSPLAY_USE_WINAPI
        wave_open(44100, target.toLocal8Bit().data());
        #else
        wave_open(44100, target.c_str());
        #endif
        wave_enable_stereo();
        Mix_SetPostMix(myMusicPlayer, NULL);
        wavOpened = true;
    }

    void stopWavRecording()
    {
        if(!wavOpened) return;
        wave_close();
        Mix_SetPostMix(NULL, NULL);
        wavOpened = false;
    }
}
