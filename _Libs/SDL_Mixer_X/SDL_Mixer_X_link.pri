SDL_MIXER_X_LIBS_DYNAMIC = \
    -lSDL2_mixer_ext

SDL_MIXER_X_LIBS_STATIC = \
    -lSDL2_mixer_ext \
    -lFLAC -lvorbisfile -lvorbis -logg \
    -lmad -lid3tag \
    -lmodplug \
    -lADLMIDI \
    -lOPNMIDI \
    -ltimidity \
    -lgme -lzlib

