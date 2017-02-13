=================================================================
     Wrapper of SDL Mixer Ext API for Visual Basic 6
=====        21 January 2017 by Vitaliy Novichkov           =====
=================================================================
= You are allowed to use and modify this wrapper without limits =
= Everything is presented as-is, and I don't take resonse for   = 
= any damages caused by usage of this wrapper.                  =
=================================================================

1) SDL2MixerVB.dll in the "dlls" directory is required
2) Working pre-built example in the "build" folder
3) It's going as non-COM!
4) To have your project working, you should include modSDL2_mixer_ext_vb6.bas
5) Specification of official SDL Mixer is here: https://www.libsdl.org/projects/SDL_mixer/docs/index.html

Functions are has similar name and arguments. BUT, some exceptions:
- Pointers to Mix_Music* and Mix_Chunk* are presented as Long variable
- Some functions are not binded into this wrapper because there are useless or incompatibility
- There is no binded SDL functions except some necessary functions, required for SDL2 Mixer Ext usage

Note about example project:
- to make it working, you shuld put all files from example_media and dlls folders
  into working directory of built application.

