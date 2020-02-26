#include "window.h"
#include "ui_window.h"

ImageSelectTest::ImageSelectTest(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::ImageSelectTest)
{
    ui->setupUi(this);
}

ImageSelectTest::~ImageSelectTest()
{
    delete ui;
}

