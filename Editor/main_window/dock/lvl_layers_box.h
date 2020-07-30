#pragma once
#ifndef LVL_LAYERS_BOX_H
#define LVL_LAYERS_BOX_H

#include <QDockWidget>
#include <QModelIndex>
#include "mwdock_base.h"

class MainWindow;
class QListWidgetItem;

namespace Ui {
class LvlLayersBox;
}

class LvlLayersBox : public QDockWidget, public MWDock_Base
{
    Q_OBJECT

    friend class MainWindow;
    explicit LvlLayersBox(QWidget *parent = nullptr);
    ~LvlLayersBox();

public:
    bool layerIsExist(QString lr, int *index = nullptr);

    void setLayersBox();

    void modifyLayer(QString layerName, QString newLayerName);
    void modifyLayer(QString layerName, bool visible);
    void setLayerToolsLocked(bool locked);

public slots:
    void re_translate();
    void dragAndDroppedLayer(QModelIndex sourceParent, int sourceStart, int sourceEnd, QModelIndex destinationParent, int destinationRow);

private slots:
    void on_AddLayer_clicked();
    void on_LvlLayerList_itemChanged(QListWidgetItem *item);

    void on_RemoveLayer_clicked();
    void on_LvlLayerList_customContextMenuRequested(const QPoint &pos);

    void on_LvlLayerList_itemClicked(QListWidgetItem *item);
    void on_LvlLayerList_itemSelectionChanged();

private:
    void removeCurrentLayer(bool moveToDefault);
    void removeLayerItems(QString layerName);
    void removeLayerFromListAndData(QListWidgetItem * layerItem);
    void modifyLayer(QString layerName, QString newLayerName, bool visible, int historyRecord = -1);

    //Direct List Functions
    void addNewLayer(QString layerName, bool setEdited);
    void modifyLayerItem(QListWidgetItem *item, QString oldLayerName, QString newLayerName, bool visible);

    Ui::LvlLayersBox *ui;
    //! Prevent writing of settings on slot executing
    bool m_lockSettings = false;
};

#endif // LVL_LAYERS_BOX_H
