# sdl2 cmake project-config input for ./configure scripts

set(prefix "C:/_Repos/PGE-Project/_Libs/_builds/win32") 
set(exec_prefix "${prefix}")
set(libdir "${exec_prefix}/lib")
set(SDL2_PREFIX "C:/_Repos/PGE-Project/_Libs/_builds/win32")
set(SDL2_EXEC_PREFIX "C:/_Repos/PGE-Project/_Libs/_builds/win32")
set(SDL2_LIBDIR "${exec_prefix}/lib")
set(SDL2_INCLUDE_DIRS "${prefix}/include/SDL2")
set(SDL2_LIBRARIES "-L${SDL2_LIBDIR}  -lmingw32 -lSDL2main -lSDL2  -mwindows")
