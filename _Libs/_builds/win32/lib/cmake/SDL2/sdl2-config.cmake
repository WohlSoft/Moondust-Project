# sdl2 cmake project-config input for ./configure scripts

set(prefix "/c/Repos/PGE-Project/_Libs/_sources/../_builds/win32") 
set(exec_prefix "${prefix}")
set(libdir "/c/Repos/PGE-Project/_Libs/_sources/../_builds/win32/lib")
set(SDL2_PREFIX "/c/Repos/PGE-Project/_Libs/_sources/../_builds/win32")
set(SDL2_EXEC_PREFIX "/c/Repos/PGE-Project/_Libs/_sources/../_builds/win32")
set(SDL2_LIBDIR "/c/Repos/PGE-Project/_Libs/_sources/../_builds/win32/lib")
set(SDL2_INCLUDE_DIRS "/c/Repos/PGE-Project/_Libs/_sources/../_builds/win32/include/SDL2")
set(SDL2_LIBRARIES "-L${SDL2_LIBDIR}  -lmingw32 -lSDL2main -lSDL2  -mwindows")
string(STRIP "${SDL2_LIBRARIES}" SDL2_LIBRARIES)
