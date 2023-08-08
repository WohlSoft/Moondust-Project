#pragma once
#ifndef WLD_ITEM_PROPS_H
#define WLD_ITEM_PROPS_H

#include <QDockWidget>
#include <QMenu>
#include "mwdock_base.h"

class MainWindow;
class QToolButton;
struct WorldLevelTile;

namespace Ui
{
class WLD_ItemProps;
}

class WLD_ItemProps : public QDockWidget, public MWDock_Base
{
    Q_OBJECT

    friend class MainWindow;
private:
    explicit WLD_ItemProps(QWidget *parent = nullptr);
    ~WLD_ItemProps();

public:
    void openPropertiesFor(int Type, WorldLevelTile level, bool newItem = false, bool dontShow = false);
    void hideToolbox();
    void resetExitTypesList();

public slots:
    void re_translate();

    // accept point from world map into a level properties
    void acceptWarpToCoordinates(QPoint p);

private slots:
    void on_WLD_PROPS_PathBG_clicked(bool checked);
    void on_WLD_PROPS_BigPathBG_clicked(bool checked);
    void on_WLD_PROPS_AlwaysVis_clicked(bool checked);
    void on_WLD_PROPS_GameStart_clicked(bool checked);
    void on_WLD_PROPS_LVLFile_editingFinished();
    void on_WLD_PROPS_LVLTitle_editingFinished();
    void on_WLD_PROPS_EnterTo_valueChanged(int arg1);
    void on_WLD_PROPS_LVLBrowse_clicked();

    void levelExitCodeSelected(QAction *exitCode);

    void on_WLD_PROPS_GotoX_editingFinished();
    void on_WLD_PROPS_GotoY_editingFinished();
    void on_WLD_PROPS_GetPoint_clicked();

private:
    Ui::WLD_ItemProps *ui;
    //! ArrayID of editing item
    long m_currentLevelArrayId = -1;
    //! Prevent writing of settings on slots execution
    bool m_lockSettings = false;
    //! Exit codes
    QMenu m_exitCodes[4];
    QToolButton *m_exitButtons[4];
    enum ExitCodesSides
    {
        EXITCODE_TOP = 0,
        EXITCODE_LEFT,
        EXITCODE_RIGHT,
        EXITCODE_BOTTOM,
        EXITCODE__INVALID
    };
    QIcon m_exitButtonIcons[4][3];
};

#endif // WLD_ITEM_PROPS_H
