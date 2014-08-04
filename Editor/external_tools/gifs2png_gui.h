#ifndef GIFS2PNG_GUI_H
#define GIFS2PNG_GUI_H

#include <QDialog>

namespace Ui {
class gifs2png_gui;
}

class gifs2png_gui : public QDialog
{
    Q_OBJECT

public:
    explicit gifs2png_gui(QWidget *parent = 0);
    ~gifs2png_gui();

private:
    Ui::gifs2png_gui *ui;
};

#endif // GIFS2PNG_GUI_H
