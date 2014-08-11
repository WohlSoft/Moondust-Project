#ifndef TILESETGROUPEDITOR_H
#define TILESETGROUPEDITOR_H

#include <QDialog>
#include "tileset.h"

#include "../common_features/flowlayout.h"

namespace Ui {
class TilesetGroupEditor;
}

class TilesetGroupEditor : public QDialog
{
    Q_OBJECT

public:
    explicit TilesetGroupEditor(QWidget *parent = 0);
    ~TilesetGroupEditor();

    struct SimpleTilesetGroup{
        QString groupName;
        QStringList tilesets;
    };

    SimpleTilesetGroup toSimpleTilesetGroup();
    static void SaveSimpleTilesetGroup(const QString &path, const SimpleTilesetGroup &tileset);
    static bool OpenSimpleTilesetGroup(const QString &path, SimpleTilesetGroup &tileset);

private slots:
    void on_addTileset_clicked();
    void on_RemoveTileset_clicked();
    void on_Close_clicked();
    void on_Open_clicked();
    void on_Save_clicked();

private:
    FlowLayout * layout;
    Ui::TilesetGroupEditor *ui;
    void redrawAll();
    QList<QPair<QString,tileset::SimpleTileset> > tilesets;
};

#endif // TILESETGROUPEDITOR_H
