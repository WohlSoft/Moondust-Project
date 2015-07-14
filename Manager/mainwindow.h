#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_actionRepositories_triggered();
    void on_refresh_clicked();
    void updateConfigPacksList();
    void on_actionExit_triggered();

protected:
    void closeEvent(QCloseEvent* e);

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
