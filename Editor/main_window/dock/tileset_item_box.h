#ifndef TILESET_ITEM_BOX_H
#define TILESET_ITEM_BOX_H

#include <QDockWidget>
#include <QTabWidget>

#include <data_configs/obj_tilesets.h>

class MainWindow;
class QScrollArea;
class QComboBox;

namespace Ui {
class TilesetItemBox;
}

class TilesetItemBox : public QDockWidget
{
    Q_OBJECT

    friend class MainWindow;
public:
    explicit TilesetItemBox(QWidget *parent = 0);
    TilesetItemBox(MainWindow *ParentMW, QWidget *parent = 0);
    void setParentMW(MainWindow *ParentMW);
    ~TilesetItemBox();

public slots:
    void setTileSetBox(bool force=false);

    void on_TilesetItemBox_visibilityChanged(bool visible);
    void on_newTileset_clicked();
    void on_tilesetGroup_currentIndexChanged(int index);

    void setTabPosition(QTabWidget::TabPosition pos);

    void editSelectedTileset();
    QScrollArea *getFrameTilesetOfTab(QWidget *catTab);
    QComboBox *getGroupComboboxOfTab(QWidget *catTab);
    QWidget *findTabWidget(const QString &categoryItem);
    QWidget *makeCategory(const QString &categoryItem);
    void prepareTilesetGroup(const SimpleTilesetGroup &tilesetGroups);
    void clearTilesetGroups();
    void makeSelectedTileset(int tabIndex);
    QVector<SimpleTileset> loadCustomTilesets();
    void makeCurrentTileset();
    void makeAllTilesets();

private:
    void construct(MainWindow *ParentMW);
    Ui::TilesetItemBox *ui;
    MainWindow *mw;
};

#endif // TILESET_ITEM_BOX_H
