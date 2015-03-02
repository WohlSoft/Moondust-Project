#include "_dock_vizman.h"
#include <QDockWidget>

DockVizibilityManager::DockVizibilityManager()
{}

DockVizibilityManager::~DockVizibilityManager()
{
    stateList.clear();
}

void DockVizibilityManager::addState(QDockWidget* _widget, bool *setting)
{
    VisiblyState _vState;
    _vState.setting = setting;
    _vState.widget = _widget;
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
        *(s.setting) = s.widget->isVisible();
        s.widget->setVisible(false);
    }
}

void DockVizibilityManager::showAll()
{
    foreach(VisiblyState s, stateList)
    {
        s.widget->setVisible( *(s.setting) );
    }
}

