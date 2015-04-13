#include "pge_audio.h"
#include <data_configs/config_manager.h>
#include <audio/SdlMusPlayer.h>

PGE_Audio::PGE_Audio()
{

}

PGE_Audio::~PGE_Audio()
{

}

void PGE_Audio::playSound(long soundID)
{
    if(soundID<=0) return;
    QString soundFile = ConfigManager::getSound(soundID);
    if(soundFile.isEmpty()) return;
    PGE_Sounds::SND_PlaySnd(soundFile);
}

void PGE_Audio::playSoundByRole(obj_sound_role::roles role)
{
    long id = ConfigManager::getSoundByRole(role);
    if(id<=0) return;
    QString soundFile = ConfigManager::getSound(id);
    if(soundFile.isEmpty()) return;
    PGE_Sounds::SND_PlaySnd(soundFile);
}

