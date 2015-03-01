#ifndef DOCKVIZIBILITYMANAGER_H
#define DOCKVIZIBILITYMANAGER_H

#include <QList>

struct VisiblyState
{
    bool *setting;             //Pointer to setting
    void (*setVisState)(bool); //Pointer to set visible function
    bool (*getVisState)();     //Pointer to get visible function
};

class DockVizibilityManager
{
public:
    DockVizibilityManager();
    ~DockVizibilityManager();
    void addState(bool *setting, void (*setVizFunc)(bool), bool (*getVisFunc)());
    QList<VisiblyState > list();
    void hideAll();
    void showAll();

private:
    QList<VisiblyState > stateList;
};

#endif // DOCKVIZIBILITYMANAGER_H
