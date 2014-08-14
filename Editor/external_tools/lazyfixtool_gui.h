#ifndef LAZYFIXTOOL_GUI_H
#define LAZYFIXTOOL_GUI_H

#include <QDialog>
#include <QProcess>

namespace Ui {
class LazyFixTool_gui;
}

class LazyFixTool_gui : public QDialog
{
    Q_OBJECT

public:
    explicit LazyFixTool_gui(QWidget *parent = 0);
    ~LazyFixTool_gui();

private slots:
    void on_BrowseInput_clicked();
    void on_BrowseOutput_clicked();
    void on_startTool_clicked();
    void on_close_clicked();

    void consoleMessage();

private:
    QProcess * proc;

    Ui::LazyFixTool_gui *ui;
};

#endif // LAZYFIXTOOL_GUI_H
