#ifndef DOCKVIZIBILITYMANAGER_H
#define DOCKVIZIBILITYMANAGER_H

#include <QList>
class QDockWidget;

struct VisiblyState
{
    bool *setting;             //Pointer to setting
    QDockWidget* widget;
};

class DockVizibilityManager
{
public:
    DockVizibilityManager();
    ~DockVizibilityManager();
    void addState(QDockWidget* _widget, bool *setting);
    QList<VisiblyState > list();
    void hideAll();
    void showAll();

private:
    QList<VisiblyState > stateList;
};

#endif // DOCKVIZIBILITYMANAGER_H
