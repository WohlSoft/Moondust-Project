#ifndef PNG2GIFS_GUI_H
#define PNG2GIFS_GUI_H

#include <QDialog>
#include <QProcess>

namespace Ui {
class png2gifs_gui;
}

class png2gifs_gui : public QDialog
{
    Q_OBJECT

public:
    explicit png2gifs_gui(QWidget *parent = 0);
    ~png2gifs_gui();

private slots:
    void on_BrowseInput_clicked();
    void on_BrowseOutput_clicked();
    void on_startTool_clicked();

    void on_close_clicked();

    void consoleMessage();

private:
    QProcess * proc;
    Ui::png2gifs_gui *ui;
};

#endif // PNG2GIFS_GUI_H
