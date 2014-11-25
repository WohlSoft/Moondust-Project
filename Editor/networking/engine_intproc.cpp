#include "engine_intproc.h"

EngineClient * IntEngine::engineSocket=NULL;

IntEngine::IntEngine()
{}

void IntEngine::init()
{
    if(!isWorking())
    {
        if(engineSocket!=NULL) delete engineSocket;
        engineSocket = new EngineClient();
        engineSocket->start();
    }
}

void IntEngine::quit()
{
    if(isWorking())
    {
        engineSocket->exit();
        delete engineSocket;
        engineSocket = NULL;
    }
}

bool IntEngine::isWorking()
{
    bool isRuns=false;
    isRuns = (engineSocket!=NULL && !engineSocket->isConnected());
    return isRuns;
}
