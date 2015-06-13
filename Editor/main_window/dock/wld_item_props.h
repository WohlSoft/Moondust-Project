#ifndef WLD_ITEM_PROPS_H
#define WLD_ITEM_PROPS_H

#include <QDockWidget>
#include "mwdock_base.h"

class MainWindow;
struct WorldLevels;

namespace Ui {
class WLD_ItemProps;
}

class WLD_ItemProps : public QDockWidget, public MWDock_Base
{
    Q_OBJECT

    friend class MainWindow;
private:
    explicit WLD_ItemProps(QWidget *parent = 0);
    ~WLD_ItemProps();

public:
    void WldItemProps(int Type, WorldLevels level, bool newItem=false);
    void WldItemProps_hide();
    long wlvlPtr;   //!< ArrayID of editing item

public slots:
    void re_translate();

    void WldLvlExitTypeListReset();
    // accept point from world map into a level properties
    void WLD_returnPointToLevelProperties(QPoint p);

private slots:
    void on_WLD_ItemProps_visibilityChanged(bool visible);

    void on_WLD_PROPS_PathBG_clicked(bool checked);
    void on_WLD_PROPS_BigPathBG_clicked(bool checked);
    void on_WLD_PROPS_AlwaysVis_clicked(bool checked);
    void on_WLD_PROPS_GameStart_clicked(bool checked);
    void on_WLD_PROPS_LVLFile_editingFinished();
    void on_WLD_PROPS_LVLTitle_editingFinished();
    void on_WLD_PROPS_EnterTo_valueChanged(int arg1);
    void on_WLD_PROPS_LVLBrowse_clicked();
    void on_WLD_PROPS_ExitTop_currentIndexChanged(int index);
    void on_WLD_PROPS_ExitLeft_currentIndexChanged(int index);
    void on_WLD_PROPS_ExitRight_currentIndexChanged(int index);
    void on_WLD_PROPS_ExitBottom_currentIndexChanged(int index);
    void on_WLD_PROPS_GotoX_editingFinished();
    void on_WLD_PROPS_GotoY_editingFinished();
    void on_WLD_PROPS_GetPoint_clicked();

private:
    Ui::WLD_ItemProps *ui;
    bool wld_tools_lock;
};

#endif // WLD_ITEM_PROPS_H
