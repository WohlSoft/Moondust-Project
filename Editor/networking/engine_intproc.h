#ifndef ENGINE_INTPROCINTPROC_H
#define ENGINE_INTPROCINTPROC_H

#include "engine_client.h"

class IntEngine
{
public:
    IntEngine();
    static void init();

    static void quit();

    static EngineClient * engineSocket;
    static bool isWorking();

};

#endif // ENGINE_INTPROCINTPROC_H
