#ifndef PGE_AUDIO_H
#define PGE_AUDIO_H

#include <data_configs/obj_sound_roles.h>

class PGE_Audio
{
public:
    PGE_Audio();
    ~PGE_Audio();
    static void playSound(long soundID);
    static void playSoundByRole(obj_sound_role::roles role);
};

#endif // PGE_AUDIO_H
