#ifndef DEBUGGER_H
#define DEBUGGER_H

#include <tools/debugger/custom_counter.h>
#include <QDockWidget>
#include "mwdock_base.h"

class MainWindow;
class QListWidgetItem;

namespace Ui {
class DebuggerBox;
}

class DebuggerBox : public QDockWidget, public MWDock_Base
{
    Q_OBJECT

    friend class MainWindow;
private:
    explicit DebuggerBox(QWidget *parent = 0);
    ~DebuggerBox();

public slots:
    void re_translate();
    void setMousePos(QPoint p, bool isOffScreen=false);
    void setItemStat(QString list);

    void Debugger_loadCustomCounters();
    void Debugger_saveCustomCounters();

private slots:
    void on_DebuggerBox_visibilityChanged(bool visible);
    void on_DEBUG_GotoPoint_clicked();

    void on_DEBUG_AddCustomCounter_clicked();
    void on_DEBUG_RefreshCoutners_clicked();

    void on_DEBUG_CustomCountersList_itemClicked(QListWidgetItem *item);
    void on_DEBUG_CustomCountersList_customContextMenuRequested(const QPoint &pos);

private:
    QVector<CustomCounter > customCounters;
    bool isLoaded;
    Ui::DebuggerBox *ui;
};

#endif // DEBUGGER_H
