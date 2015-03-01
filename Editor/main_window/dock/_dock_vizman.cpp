#include "_dock_vizman.h"
#include <QWidget>

DockVizibilityManager::DockVizibilityManager()
{}

DockVizibilityManager::~DockVizibilityManager()
{
    stateList.clear();
}

void DockVizibilityManager::addState(QWidget* parent, bool *setting, void (QWidget::*setVizFunc)(bool), bool (QWidget::*getVisFunc)() const)
{
    VisiblyState _vState;
    _vState.setting = setting;
    _vState.setVisState = setVizFunc;
    _vState.getVisState = getVisFunc;
    _vState.parent = parent;
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
        *(s.setting) = (s.parent->*(s.getVisState))();
        (s.parent->*(s.setVisState))(false);
    }
}

void DockVizibilityManager::showAll()
{
    foreach(VisiblyState s, stateList)
    {
        (s.parent->*(s.setVisState))( *(s.setting) );
    }
}

