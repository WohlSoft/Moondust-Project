#pragma once
#ifndef LVL_ITEM_TOOLBOX_H
#define LVL_ITEM_TOOLBOX_H

#include <QDockWidget>
#include <QMenu>
#include "mwdock_base.h"

class QTabWidget;
class QListWidgetItem;
class ItemBoxListModel;

class MainWindow;

namespace Ui {
class LevelItemBox;
}

class LevelItemBox : public QDockWidget, public MWDock_Base
{
    Q_OBJECT

    friend class MainWindow;
private:
    explicit LevelItemBox(QWidget *parent = nullptr);
    ~LevelItemBox();

public:
    QTabWidget *tabWidget();

    // update data of the toolboxes
public slots:
    void re_translate();

    void initItemLists();

    void clearFilter();

private slots:
    void on_BlockItemsList_customContextMenuRequested(const QPoint &pos);
    void on_BGOItemsList_customContextMenuRequested(const QPoint &pos);
    void on_NPCItemsList_customContextMenuRequested(const QPoint &pos);

    void update_block_group_filter(int arg1);
    void update_bgo_group_filter(int arg1);
    void update_npc_group_filter(int arg1);

    void update_block_category_filter(int arg1);
    void update_bgo_category_filter(int arg1);
    void update_npc_category_filter(int arg1);

    //Item was clicked
    void BlockList_itemClicked(const QModelIndex &item);
    void BGOList_itemClicked(const QModelIndex &item);
    void NPCList_itemClicked(const QModelIndex &item);


private:
    Ui::LevelItemBox *ui = nullptr;

    ItemBoxListModel *m_blockModel = nullptr;
    ItemBoxListModel *m_bgoModel = nullptr;
    ItemBoxListModel *m_npcModel = nullptr;

    QMenu   m_blockFilterSetup;
    QMenu   m_bgoFilterSetup;
    QMenu   m_npcFilterSetup;

    QString m_allLabelText = "[all]";

    bool m_lockGroup = false;
    bool m_lockCategory = false;
};

#endif // LVL_ITEM_TOOLBOX_H
