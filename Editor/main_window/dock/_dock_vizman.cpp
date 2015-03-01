#include "_dock_vizman.h"

DockVizibilityManager::DockVizibilityManager()
{}

DockVizibilityManager::~DockVizibilityManager()
{
    stateList.clear();
}

void DockVizibilityManager::addState(bool *setting, void (*setVizFunc)(bool), bool (*getVisFunc)())
{
    VisiblyState _vState;
    _vState.setting = setting;
    _vState.setVisState = setVizFunc;
    _vState.getVisState = getVisFunc;
    stateList.push_back(_vState);
}

QList<VisiblyState> DockVizibilityManager::list()
{
    return stateList;
}

void DockVizibilityManager::hideAll()
{
    foreach(VisiblyState s, stateList)
    {
        *(s.setting) = s.getVisState();
        s.setVisState(false);
    }
}

void DockVizibilityManager::showAll()
{
    foreach(VisiblyState s, stateList)
    {
        s.setVisState( *(s.setting) );
    }
}

