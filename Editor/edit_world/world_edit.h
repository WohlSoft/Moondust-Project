#ifndef WORLD_EDIT_H
#define WORLD_EDIT_H

#include <QWidget>

namespace Ui {
class WorldEdit;
}

class WorldEdit : public QWidget
{
    Q_OBJECT

public:
    explicit WorldEdit(QWidget *parent = 0);
    ~WorldEdit();

private:
    Ui::WorldEdit *ui;
};

#endif // WORLD_EDIT_H
