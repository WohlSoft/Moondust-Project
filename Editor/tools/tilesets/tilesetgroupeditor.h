#ifndef TILESETGROUPEDITOR_H
#define TILESETGROUPEDITOR_H

#include <QDialog>
#include "tileset.h"

#include "../../common_features/flowlayout.h"
#include "../../data_configs/obj_tilesets.h"

namespace Ui {
class TilesetGroupEditor;
}

class TilesetGroupEditor : public QDialog
{
    Q_OBJECT

public:
    explicit TilesetGroupEditor(QGraphicsScene *scene = 0, QWidget *parent = 0);
    ~TilesetGroupEditor();

    SimpleTilesetGroup toSimpleTilesetGroup();
    static void SaveSimpleTilesetGroup(const QString &path, const SimpleTilesetGroup &tileset);
    static bool OpenSimpleTilesetGroup(const QString &path, SimpleTilesetGroup &tileset);

private slots:
    void on_addTileset_clicked();
    void on_RemoveTileset_clicked();
    void on_Close_clicked();
    void on_Open_clicked();
    void on_Save_clicked();
    void on_tilesetUp_clicked();
    void on_tilesetDown_clicked();

    void movedTileset( const QModelIndex & sourceParent, int sourceStart, int sourceEnd, const QModelIndex & destinationParent, int destinationRow );

private:
    static QString lastFileName;
    FlowLayout * layout;
    Ui::TilesetGroupEditor *ui;
    void redrawAll();
    QGraphicsScene * scn;
    QList<QPair<QString,SimpleTileset> > tilesets;
};

#endif // TILESETGROUPEDITOR_H
