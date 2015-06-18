#include "luafuncs_core_audio.h"

#include <audio/pge_audio.h>

void Binding_Core_GlobalFuncs_Audio::playSound(long soundID)
{
    PGE_Audio::playSound(soundID);
}

void Binding_Core_GlobalFuncs_Audio::playSoundByRole(int role)
{
    PGE_Audio::playSoundByRole(static_cast<obj_sound_role::roles>(role));
}

luabind::scope Binding_Core_GlobalFuncs_Audio::bindToLua()
{
    using namespace luabind;
    return
        namespace_("Audio")[
            def("playSound", &Binding_Core_GlobalFuncs_Audio::playSound),
            def("playSoundByRole", &Binding_Core_GlobalFuncs_Audio::playSoundByRole)
        ];
}

