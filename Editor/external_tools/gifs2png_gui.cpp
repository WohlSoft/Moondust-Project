#include "gifs2png_gui.h"
#include "ui_gifs2png_gui.h"

gifs2png_gui::gifs2png_gui(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::gifs2png_gui)
{
    ui->setupUi(this);
}

gifs2png_gui::~gifs2png_gui()
{
    delete ui;
}
