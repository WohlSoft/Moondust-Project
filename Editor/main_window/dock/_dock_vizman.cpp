#include "_dock_vizman.h"
#include <QDockWidget>

DockVizibilityManager::DockVizibilityManager()
{}

DockVizibilityManager::~DockVizibilityManager()
{
    m_stateList.clear();
}

void DockVizibilityManager::addState(QDockWidget* _widget, bool *setting)
{
    VisiblyState _vState;
    _vState.setting = setting;
    _vState.widget = _widget;
    m_stateList.push_back(_vState);
}

QList<VisiblyState> DockVizibilityManager::list()
{
    return m_stateList;
}

void DockVizibilityManager::hideAll()
{
    foreach(VisiblyState s, m_stateList)
    {
        *(s.setting) = s.widget->isVisible();
        s.widget->setVisible(false);
    }
}

void DockVizibilityManager::showAll()
{
    foreach(VisiblyState s, m_stateList)
    {
        s.widget->setVisible( *(s.setting) );
    }
}

