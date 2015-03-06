#ifndef LVL_SEARCH_BOX_H
#define LVL_SEARCH_BOX_H

#include <QDockWidget>
#include "mwdock_base.h"
#include <PGE_File_Formats/lvl_filedata.h>

class MainWindow;
class QMdiSubWindow;
class LevelEdit;
class QComboBox;

namespace Ui {
class LvlSearchBox;
}

class LvlSearchBox : public QDockWidget, public MWDock_Base
{
    Q_OBJECT

    friend class MainWindow;
    explicit LvlSearchBox(QWidget *parent = 0);
    ~LvlSearchBox();
public:
    QComboBox *cbox_layer_block();
    QComboBox *cbox_layer_bgo();
    QComboBox *cbox_layer_npc();

    QComboBox *cbox_event_block_dest();
    QComboBox *cbox_event_block_hit();
    QComboBox *cbox_event_block_le();

public slots:
    void re_translate();
    void resetAllSearches();
    void toggleNewWindowLVL(QMdiSubWindow *window);
    void resetBlockSearch();
    void resetBGOSearch();
    void resetNPCSearch();

private slots:
    void on_LvlSearchBox_visibilityChanged(bool visible);

    void on_FindStartNPC_clicked();
    void on_Find_Button_TypeBlock_clicked();
    void on_Find_Button_TypeBGO_clicked();
    void on_Find_Button_TypeNPC_clicked();
    void on_Find_Button_ResetBlock_clicked();
    void on_Find_Button_ResetBGO_clicked();
    void on_Find_Button_ResetNPC_clicked();
    void on_FindStartBlock_clicked();
    void on_FindStartBGO_clicked();
    void on_Find_Button_ContainsNPCBlock_clicked();

private:
    Ui::LvlSearchBox *ui;
    bool lockReset;

    void resetAllSearchFields();
    LevelBlock curSearchBlock;
    LevelBGO curSearchBGO;
    LevelNPC curSearchNPC;

    bool doSearchBlock(LevelEdit* edit);
    bool doSearchBGO(LevelEdit* edit);
    bool doSearchNPC(LevelEdit* edit);

    enum currentSearch{
        SEARCH_BLOCK = 1 << 0,
        SEARCH_BGO = 1 << 1,
        SEARCH_NPC = 1 << 2,
        SEARCH_TILE = 1 << 3,
        SEARCH_SCENERY = 1 << 4,
        SEARCH_PATH = 1 << 5,
        SEARCH_LEVEL = 1 << 6,
        SEARCH_MUSICBOX = 1 << 7
    };
    int currentSearches;
};

#endif // LVL_SEARCH_BOX_H
