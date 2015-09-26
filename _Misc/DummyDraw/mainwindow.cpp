#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QRgb>
#include <QPainter>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent), image(800, 600, QImage::Format_RGBA8888_Premultiplied), cv(&image),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    image.fill(Qt::white);
    ui->rasterImage->setScene(new QGraphicsScene(ui->rasterImage));
    ui->rasterImage->scene()->setBackgroundBrush(QBrush(Qt::gray));
    ui->rasterImage->scene()->addItem(&cv);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_actionExit_triggered()
{
    this->close();
}



