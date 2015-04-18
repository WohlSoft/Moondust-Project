#ifndef MAINSERVERWINDOW_H
#define MAINSERVERWINDOW_H

#include <QMainWindow>

namespace Ui {
class MainServerWindow;
}

class MainServerWindow : public QMainWindow
{
    Q_OBJECT

public:
    static MainServerWindow* INSTANCE;
    explicit MainServerWindow(QWidget *parent = 0);
    ~MainServerWindow();

private:
    Ui::MainServerWindow *ui;
};

#endif // MAINSERVERWINDOW_H
