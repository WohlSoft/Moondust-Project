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
    printf( V_INTERNAL_NAME " " V_FILE_VERSION  V_FILE_RELEASE "-" V_BUILD_VER "-" GIT_BRANCH );
    #endif
    #ifdef VER_SHORT
    printf( V_FILE_VERSION  V_FILE_RELEASE );
    #endif
    return 0;
}

