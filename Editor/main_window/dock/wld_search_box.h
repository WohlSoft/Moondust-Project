#ifndef WLD_SEARCH_BOX_H
#define WLD_SEARCH_BOX_H

#include <QDockWidget>
#include "mwdock_base.h"
#include <PGE_File_Formats/wld_filedata.h>

class MainWindow;
class QMdiSubWindow;
class WorldEdit;

namespace Ui {
class WldSearchBox;
}

class WldSearchBox : public QDockWidget, public MWDock_Base
{
    Q_OBJECT

    friend class MainWindow;
private:
    explicit WldSearchBox(QWidget *parent = 0);
    ~WldSearchBox();

public slots:
    void re_translate();

    void toggleNewWindowWLD(QMdiSubWindow *window);
    void resetAllSearchFieldsWLD();
    void resetAllSearchesWLD();
    void selectLevelForSearch();

    void resetTileSearch();
    void resetScenerySearch();
    void resetPathSearch();
    void resetLevelSearch();
    void resetMusicSearch();

private slots:
    void on_WldSearchBox_visibilityChanged(bool visible);
    void on_FindStartLevel_clicked();
    void on_FindStartTile_clicked();
    void on_FindStartScenery_clicked();
    void on_FindStartPath_clicked();
    void on_FindStartMusic_clicked();

    void on_Find_Button_TypeLevel_clicked();
    void on_Find_Button_TypeTile_clicked();
    void on_Find_Button_TypeScenery_clicked();
    void on_Find_Button_TypePath_clicked();
    void on_Find_Button_TypeMusic_clicked();
    void on_Find_Button_ResetLevel_clicked();
    void on_Find_Button_ResetMusic_clicked();
    void on_Find_Button_ResetPath_clicked();
    void on_Find_Button_ResetScenery_clicked();
    void on_Find_Button_ResetTile_clicked();

private:
    WorldTiles curSearchTile;
    WorldScenery curSearchScenery;
    WorldPaths curSearchPath;
    WorldLevels curSearchLevel;
    WorldMusic curSearchMusic;

    bool doSearchTile(WorldEdit *edit);
    bool doSearchScenery(WorldEdit *edit);
    bool doSearchPath(WorldEdit *edit);
    bool doSearchLevel(WorldEdit* edit);
    bool doSearchMusic(WorldEdit *edit);

    Ui::WldSearchBox *ui;
    bool lockResetWorld;

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

#endif // WLD_SEARCH_BOX_H
