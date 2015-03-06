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
    explicit LvlLayersBox(QWidget *parent = 0);
    ~LvlLayersBox();

public:
    bool layerIsExist(QString lr, int *index=0);

public slots:
    void re_translate();
    void setLayersBox();

    void ModifyLayer(QString layerName, QString newLayerName);
    void setLayerToolsLocked(bool locked);

    void DragAndDroppedLayer(QModelIndex sourceParent, int sourceStart, int sourceEnd, QModelIndex destinationParent, int destinationRow);

private slots:
    void on_LvlLayersBox_visibilityChanged(bool visible);

    void on_AddLayer_clicked();
    void on_LvlLayerList_itemChanged(QListWidgetItem *item);

    void on_RemoveLayer_clicked();
    void on_LvlLayerList_customContextMenuRequested(const QPoint &pos);

    void on_LvlLayerList_itemClicked(QListWidgetItem *item);
    void on_LvlLayerList_itemSelectionChanged();

private:
    void RemoveCurrentLayer(bool moveToDefault);
    void RemoveLayerItems(QString layerName);
    void RemoveLayerFromListAndData(QListWidgetItem * layerItem);
    void ModifyLayer(QString layerName, bool visible);
    void ModifyLayer(QString layerName, QString newLayerName, bool visible, int historyRecord = -1);

    //Direct List Functions
    void AddNewLayer(QString layerName, bool setEdited);
    void ModifyLayerItem(QListWidgetItem *item, QString oldLayerName, QString newLayerName, bool visible);

    Ui::LvlLayersBox *ui;
    bool lockLayerEdit;
};

#endif // LVL_LAYERS_BOX_H
