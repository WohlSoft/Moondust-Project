#ifndef MAINSERVERWINDOW_H
#define MAINSERVERWINDOW_H

#include <QMainWindow>
#include <pgeserver.h>

namespace Ui {
class MainServerWindow;
}

class MainServerWindow : public QMainWindow
{
    Q_OBJECT

public:
    static MainServerWindow* INSTANCE;
    static void msgHandler(QtMsgType type, const QMessageLogContext & context, const QString & msg);

public:
    explicit MainServerWindow(QWidget *parent = 0);
    ~MainServerWindow();

    void start();
    void shutdown();


    void log(QtMsgType type, const QMessageLogContext & context, const QString &msg);
private:
    PGEServer* m_server;
    Ui::MainServerWindow *ui;
};

#endif // MAINSERVERWINDOW_H
