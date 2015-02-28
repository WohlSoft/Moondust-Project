#ifndef LVL_ITEM_TOOLBOX_H
#define LVL_ITEM_TOOLBOX_H

#include <QDockWidget>
#include "mwdock_base.h"

class QTabWidget;
class QListWidgetItem;

class MainWindow;

namespace Ui {
class LevelItemBox;
}

class LevelItemBox : public QDockWidget, public MWDock_Base
{
    Q_OBJECT

    friend class MainWindow;
private:
    explicit LevelItemBox(QWidget *parent = 0);
    ~LevelItemBox();
public:
    QTabWidget *tabWidget();

    // update data of the toolboxes
public slots:
    void re_translate();

    void setLvlItemBoxes(bool setGrp=false, bool setCat=false);

    void updateFilters();
    void clearFilter();

private slots:
    void on_LevelItemBox_visibilityChanged(bool visible);

    void on_BlockUniform_clicked(bool checked);
    void on_BGOUniform_clicked(bool checked);
    void on_NPCUniform_clicked(bool checked);

    void on_BlockGroupList_currentIndexChanged(const QString &arg1);
    void on_BGOGroupList_currentIndexChanged(const QString &arg1);
    void on_NPCGroupList_currentIndexChanged(const QString &arg1);

    void on_BlockCatList_currentIndexChanged(const QString &arg1);
    void on_BGOCatList_currentIndexChanged(const QString &arg1);
    void on_NPCCatList_currentIndexChanged(const QString &arg1);

    void on_BlockFilterField_textChanged(const QString &arg1);
    void on_BGOFilterField_textChanged(const QString &arg1);
    void on_NPCFilterField_textChanged(const QString &arg1);

    void on_BlockFilterType_currentIndexChanged(int index);
    void on_BGOFilterType_currentIndexChanged(int index);
    void on_NPCFilterType_currentIndexChanged(int index);

    //Item was clicked
    void on_BlockItemsList_itemClicked(QListWidgetItem *item);
    void on_BGOItemsList_itemClicked(QListWidgetItem *item);
    void on_NPCItemsList_itemClicked(QListWidgetItem *item);

private:
    QString cat_blocks; //!< Category
    QString cat_bgos;
    QString cat_npcs;

private:
    Ui::LevelItemBox *ui;

    QString allLabel;
    QString customLabel;

    bool lock_grp;
    bool lock_cat;

    QString grp_blocks;
    QString grp_bgo;
    QString grp_npc;
};

#endif // LVL_ITEM_TOOLBOX_H
