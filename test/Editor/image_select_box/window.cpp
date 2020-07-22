#include "window.h"
#include "ui_window.h"

ImageSelectTest::ImageSelectTest(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::ImageSelectTest)
{
    ui->setupUi(this);

    QPixmap q = QPixmap(100, 80);

    q.fill(Qt::black);
    ui->kekWidrzet->addItem(q, "[None]", 0);

    for(int i = 0; i < 2; i++)
    {
        q.fill(Qt::blue);
        ui->kekWidrzet->addItem(q, "Blue", 1 + (i * 10));

        q.fill(Qt::green);
        ui->kekWidrzet->addItem(q, "Green Green Green Green Green", 2 + (i * 10));

        q.fill(Qt::red);
        ui->kekWidrzet->addItem(q, "Red", 3 + (i * 10));

        q.fill(Qt::yellow);
        ui->kekWidrzet->addItem(q, "yellow", 4 + (i * 10));

        q.fill(Qt::darkRed);
        ui->kekWidrzet->addItem(q, "darkRed", 5 + (i * 10));

        q.fill(Qt::darkGray);
        ui->kekWidrzet->addItem(q, "darkGray", 6 + (i * 10));

        q.fill(Qt::darkGreen);
        ui->kekWidrzet->addItem(q, "darkGreen", 7 + (i * 10));

        q.fill(Qt::lightGray);
        ui->kekWidrzet->addItem(q, "lightGray", 8 + (i * 10));

        q.fill(Qt::cyan);
        ui->kekWidrzet->addItem(q, "cyan", 9 + (i * 10));

        q.fill(Qt::magenta);
        ui->kekWidrzet->addItem(q, "magenta", 10 + (i * 10));
    }

    ui->kekWidrzet->setItem(3);
}

ImageSelectTest::~ImageSelectTest()
{
    delete ui;
}
