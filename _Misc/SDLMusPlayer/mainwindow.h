#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include <QUrl>
#include <QDragEnterEvent>
#include <QMimeData>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    QString currentMusic;

public slots:
    void dropEvent(QDropEvent *e);
    void dragEnterEvent(QDragEnterEvent *e);
    void openMusicByArg(QString musPath);
private slots:

    void on_open_clicked();

    void on_stop_clicked();

    void on_play_clicked();

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
