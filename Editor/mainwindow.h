#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtGui>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QMdiArea *parent = 0);
    ~MainWindow();
    
private slots:
    void on_OpenFile_activated();

    void on_Exit_activated();

    void on_actionAbout_activated();

    void on_LevelToolBox_visibilityChanged(bool visible);

    void on_actionLVLToolBox_activated();

    void on_actionWLDToolBox_activated();

    void on_WorldToolBox_visibilityChanged(bool visible);

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
