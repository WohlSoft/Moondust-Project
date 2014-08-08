#ifndef TILESETGROUPEDITOR_H
#define TILESETGROUPEDITOR_H

#include <QDialog>
#include "tileset.h"

namespace Ui {
class TilesetGroupEditor;
}

class TilesetGroupEditor : public QDialog
{
    Q_OBJECT

public:
    explicit TilesetGroupEditor(QWidget *parent = 0);
    ~TilesetGroupEditor();

private slots:


    void on_addTileset_clicked();

    void on_RemoveTileset_clicked();

    void on_Close_clicked();

    void on_Open_clicked();

    void on_Save_clicked();

private:
    Ui::TilesetGroupEditor *ui;
    void redrawAll();
    QList<QPair<QString,tileset::SimpleTileset> > tilesets;
};

#endif // TILESETGROUPEDITOR_H
