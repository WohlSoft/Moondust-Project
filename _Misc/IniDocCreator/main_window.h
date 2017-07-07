#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include <QMainWindow>
#include "db_manager.h"

namespace Ui {
class MainWindow;
}

class IniSpec;

class MainWindow : public QMainWindow
{
    Q_OBJECT

    DB_Manager  m_cache;
    IniSpec     *m_ini_spec = nullptr;

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

protected:
    void changeEvent(QEvent *e);

private slots:
    void on_actionQuit_triggered();

    void on_actionAbout_triggered();

private:
    Ui::MainWindow *ui;
};

#endif // MAIN_WINDOW_H
