#ifndef TILESETGROUPEDITOR_H
#define TILESETGROUPEDITOR_H

#include <QDialog>

namespace Ui {
class TilesetGroupEditor;
}

class TilesetGroupEditor : public QDialog
{
    Q_OBJECT

public:
    explicit TilesetGroupEditor(QWidget *parent = 0);
    ~TilesetGroupEditor();

private:
    Ui::TilesetGroupEditor *ui;
};

#endif // TILESETGROUPEDITOR_H
