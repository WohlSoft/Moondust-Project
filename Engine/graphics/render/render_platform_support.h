#ifndef RENDER_PLATFORM_SUPPORT_H
#define RENDER_PLATFORM_SUPPORT_H

#if !defined(__ANDROID__) && !defined(__EMSCRIPTEN__)
#define RENDER_SUPORT_OPENGL2
#endif

#if !defined(__APPLE__) && !defined(__EMSCRIPTEN__)
#define RENDER_SUPORT_OPENGL3
#endif

#endif//RENDER_PLATFORM_SUPPORT_H
