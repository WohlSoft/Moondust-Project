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

private slots:
    void on_actionAdd80_triggered();
    void on_actionAdd1000000_triggered();

    void on_actionPoke_triggered();

private:
    Ui::ItemScene *ui;
};

#endif // ITEMSCENE_H
