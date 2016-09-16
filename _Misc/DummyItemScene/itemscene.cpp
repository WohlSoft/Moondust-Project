#include "itemscene.h"
#include "ui_itemscene.h"

ItemScene::ItemScene(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::ItemScene)
{
    ui->setupUi(this);
    ui->centralWidget->addRect(20,  20);
    ui->centralWidget->addRect(60,  20);
    ui->centralWidget->addRect(100, 20);
    ui->centralWidget->addRect(140, 20);
    ui->centralWidget->addRect(180, 20);
    ui->centralWidget->addRect(20,  60);
    ui->centralWidget->addRect(60,  60);
    ui->centralWidget->addRect(100, 60);
    ui->centralWidget->addRect(140, 60);
    ui->centralWidget->addRect(180, 60);
    update();
}

ItemScene::~ItemScene()
{
    delete ui;
}

