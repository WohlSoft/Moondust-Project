#ifndef DOCKVIZIBILITYMANAGER_H
#define DOCKVIZIBILITYMANAGER_H

#include <QList>
class QWidget;

struct VisiblyState
{
    bool *setting;             //Pointer to setting
    void (QWidget::*setVisState)(bool); //Pointer to set visible function
    bool (QWidget::*getVisState)() const;     //Pointer to get visible function
    QWidget* parent;
};

class DockVizibilityManager
{
public:
    DockVizibilityManager();
    ~DockVizibilityManager();
    void addState(QWidget* parent, bool *setting, void (QWidget::*setVizFunc)(bool), bool (QWidget::*getVisFunc)() const);
    QList<VisiblyState > list();
    void hideAll();
    void showAll();

private:
    QList<VisiblyState > stateList;
};

#endif // DOCKVIZIBILITYMANAGER_H
