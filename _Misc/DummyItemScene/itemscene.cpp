#include "itemscene.h"
#include "ui_itemscene.h"

ItemScene::ItemScene(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::ItemScene)
{
    ui->setupUi(this);
    bool offset=false;
    for(int y= -1024; y<32000; y+=32)
        for(int x= -1024; x<32000; x+=32)
        {

            ui->centralWidget->addRect(x,  y + (offset ? 16 : 0));
            offset=!offset;
        }
    update();

    setWindowTitle( windowTitle() + QString(" (totally items on this map: %1)").arg(ui->centralWidget->m_items.count()) );
}

ItemScene::~ItemScene()
{
    delete ui;
}

