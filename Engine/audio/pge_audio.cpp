#include "pge_audio.h"
#include <data_configs/config_manager.h>
#include <audio/SdlMusPlayer.h>

PGE_Audio::PGE_Audio() {}

PGE_Audio::~PGE_Audio() {}

void PGE_Audio::playSound(long soundID)
{
    if(soundID<=0) return;
    if(soundID>=ConfigManager::main_sfx_index.size()) return;
    ConfigManager::main_sfx_index[soundID-1].play();
}

void PGE_Audio::playSoundByRole(obj_sound_role::roles role)
{
    long id = ConfigManager::getSoundByRole(role);
    playSound(id);
}

