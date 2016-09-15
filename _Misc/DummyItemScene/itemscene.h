#ifndef ITEMSCENE_H
#define ITEMSCENE_H

#include <QMainWindow>

namespace Ui {
class ItemScene;
}

class ItemScene : public QMainWindow
{
    Q_OBJECT

public:
    explicit ItemScene(QWidget *parent = 0);
    ~ItemScene();

private:
    Ui::ItemScene *ui;
};

#endif // ITEMSCENE_H
