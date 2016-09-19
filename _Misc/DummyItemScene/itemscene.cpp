#include "itemscene.h"
#include "item_scene/pge_edit_scene.h"
#include "ui_itemscene.h"

#include <QMdiSubWindow>

ItemScene::ItemScene(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::ItemScene)
{
    ui->setupUi(this);

    ui->dockWidget->setFocusPolicy(Qt::NoFocus);
    ui->dockWidget->setAttribute(Qt::WA_X11DoNotAcceptFocus);
    update();
}

ItemScene::~ItemScene()
{
    delete ui;
}

void ItemScene::keyPressEvent(QKeyEvent *event)
{
    QMdiSubWindow* w = ui->centralWidget->activeSubWindow();
    if(w)
    {
        PGE_EditScene* e = qobject_cast<PGE_EditScene*>(w->widget());
        if(e)
        {
            e->setFocus();
            e->keyPressEvent(event);
            return;
        }
    }
    QMainWindow::keyPressEvent(event);
}


void ItemScene::on_actionAdd80_triggered()
{
    QMdiSubWindow *w = ui->centralWidget->addSubWindow( new PGE_EditScene(ui->centralWidget) );
    w->resize(500, 500);
    PGE_EditScene* e = qobject_cast<PGE_EditScene*>(w->widget());
    bool offset = false;
    for(int y= -32; y<480; y+=32)
        for(int x= -32; x<480; x+=32)
        {
            e->addRect(x,  y + (offset ? 16 : 0));
            offset=!offset;
        }
    w->setWindowTitle( windowTitle() + QString(" (totally items on this map: %1)").arg(e->m_items.count()) );
    w->show();
}

void ItemScene::on_actionAdd1000000_triggered()
{
    QMdiSubWindow *w = ui->centralWidget->addSubWindow( new PGE_EditScene(ui->centralWidget) );
    w->resize(500, 500);
    PGE_EditScene* e = qobject_cast<PGE_EditScene*>(w->widget());
    w->setWindowTitle( QString("Million items") );
    w->show();
    e->startInitAsync();
}

void ItemScene::on_actionPoke_triggered()
{
    ui->dockWidget->show();
    ui->dockWidget->setFloating(true);
    ui->dockWidget->move(this->pos());
    ui->dockWidget->resize(150, 300);
    qApp->setActiveWindow(this);
    this->setFocus();
}
