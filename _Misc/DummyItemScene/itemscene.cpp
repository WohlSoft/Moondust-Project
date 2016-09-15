#include "itemscene.h"
#include "ui_itemscene.h"

ItemScene::ItemScene(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::ItemScene)
{
    ui->setupUi(this);
    ui->centralWidget->addRect(20, 20);
    ui->centralWidget->addRect(60, 60);
    ui->centralWidget->addRect(50, 60);
    update();
}

ItemScene::~ItemScene()
{
    delete ui;
}

