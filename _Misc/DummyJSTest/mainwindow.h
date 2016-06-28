#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QJSEngine>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    /**
     * @brief Our function which must be called from JS
     */
    void beep();

private slots:
    void on_pokeIt_clicked();

private:
    Ui::MainWindow *ui;

    //! Must be declared in SEPARATED from MainWindow class
    QJSEngine myEngine;

    /**
     * @brief Bind our classes to JavaScript (call once from constructor!)
     */
    void bindStuff();
};


/**
 * @brief A Proxy class for singletone global object (Don't bind controllable object directly, because QJSEngine destroys all binds!)
 */
class MWProxy : public QObject
{
    Q_OBJECT

public:
    MWProxy(MainWindow* mw): QObject(mw), p(mw) {}

    //! Pointer to controllable global object
    MainWindow* p;
    //! A function which a proxy to object's function
    Q_INVOKABLE void beep() { p->beep(); }
};

#endif // MAINWINDOW_H
