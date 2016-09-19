#ifndef ITEMSCENE_H
#define ITEMSCENE_H

#include <QMainWindow>

namespace Ui {
class ItemScene;
}

class ItemScene : public QMainWindow
{
    Q_OBJECT
    friend class KeyDropper;
public:
    explicit ItemScene(QWidget *parent = 0);
    ~ItemScene();

    void keyPressEvent(QKeyEvent *event);

private:
    Ui::ItemScene *ui;
};

#endif // ITEMSCENE_H
