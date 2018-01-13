#include "musplayer_base.h"
#include "../Player/mus_player.h"

MusPlayerBase::MusPlayerBase() :
    m_blink_state(false),
    m_prevTrackID(0)
{}

MusPlayerBase::~MusPlayerBase()
{}

void MusPlayerBase::openMusicByArg(QString)
{}

void MusPlayerBase::on_open_clicked()
{}

void MusPlayerBase::on_stop_clicked()
{}

void MusPlayerBase::on_play_clicked()
{}

void MusPlayerBase::on_volume_valueChanged(int value)
{
    PGE_MusicPlayer::MUS_changeVolume(value);
}

void MusPlayerBase::on_mididevice_currentIndexChanged(int) {}

void MusPlayerBase::on_fmbank_currentIndexChanged(int index)
{
#ifdef SDL_MIXER_X
    Mix_ADLMIDI_setBankID(index);
#endif

    if(Mix_PlayingMusic())
    {
        if(PGE_MusicPlayer::type == MUS_MID)
        {
            PGE_MusicPlayer::MUS_stopMusic();
            on_play_clicked();
        }
    }
}

void MusPlayerBase::on_volumeModel_currentIndexChanged(int index)
{
#ifdef SDL_MIXER_X
    Mix_ADLMIDI_setVolumeModel(index);
#endif
}

void MusPlayerBase::on_tremolo_clicked(bool checked)
{
#ifdef SDL_MIXER_X
    Mix_ADLMIDI_setTremolo((int)checked);
#endif
}

void MusPlayerBase::on_vibrato_clicked(bool checked)
{
#ifdef SDL_MIXER_X
    Mix_ADLMIDI_setVibrato((int)checked);
#endif
}

void MusPlayerBase::on_modulation_clicked(bool checked)
{
#ifdef SDL_MIXER_X
    Mix_ADLMIDI_setScaleMod((int)checked);
#endif
}

void MusPlayerBase::on_adlibMode_clicked(bool checked)
{
#ifdef SDL_MIXER_X
    Mix_ADLMIDI_setAdLibMode((int)checked);
#endif
}

void MusPlayerBase::on_logVolumes_clicked(bool checked)
{
#ifdef SDL_MIXER_X
    Mix_ADLMIDI_setLogarithmicVolumes(checked);
#endif
}

void MusPlayerBase::on_trackID_editingFinished() {}

void MusPlayerBase::on_recordWav_clicked(bool) {}

void MusPlayerBase::_blink_red()
{}

void MusPlayerBase::on_resetDefaultADLMIDI_clicked()
{}
