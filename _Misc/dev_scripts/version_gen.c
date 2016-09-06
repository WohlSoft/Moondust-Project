#include <stdio.h>

#ifdef EDITOR
#include "../../Editor/version.h"
#endif
#ifdef ENGINE
#include "../../Engine/version.h"
#endif

int main(int arvc, char**argv)
{

    #ifdef VER_FULL
    printf( _INTERNAL_NAME " " _FILE_VERSION  _FILE_RELEASE "-" _BUILD_VER );
    #endif
    #ifdef VER_SHORT
    printf( _FILE_VERSION  _FILE_RELEASE );
    #endif
    return 0;
}

