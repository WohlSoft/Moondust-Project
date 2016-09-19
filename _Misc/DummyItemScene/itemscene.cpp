#include "itemscene.h"
#include "item_scene/pge_edit_scene.h"
#include "ui_itemscene.h"

#include <QMdiSubWindow>

ItemScene::ItemScene(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::ItemScene)
{
    ui->setupUi(this);

    #ifdef TESTMILLION
    bool offset=false;
    for(int y= -1024; y<32000; y+=32)
        for(int x= -1024; x<32000; x+=32)
        {

            ui->centralWidget->addRect(x,  y + (offset ? 16 : 0));
            offset=!offset;
        }
    #else

    ui->centralWidget->addSubWindow( new PGE_EditScene(ui->centralWidget) );
    ui->centralWidget->addSubWindow( new PGE_EditScene(ui->centralWidget) );
    ui->centralWidget->addSubWindow( new PGE_EditScene(ui->centralWidget) );
    ui->dockWidget->setFocusPolicy(Qt::NoFocus);
    ui->dockWidget->setAttribute(Qt::WA_X11DoNotAcceptFocus);
    QList<QMdiSubWindow*> w = ui->centralWidget->subWindowList();
    for(int i=0; i<w.size(); i++)
    {
        w[i]->resize(500, 500);
        PGE_EditScene* e = qobject_cast<PGE_EditScene*>(w[i]->widget());
        bool offset = false;
        for(int y= -32; y<256; y+=32)
            for(int x= -32; x<256; x+=32)
            {
                e->addRect(x,  y + (offset ? 16 : 0));
                offset=!offset;
            }
        e->setWindowTitle( windowTitle() + QString(" (totally items on this map: %1)").arg(e->m_items.count()) );
    }

    #endif

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

