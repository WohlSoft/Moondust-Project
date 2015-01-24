#include "mainwindow.h"

#ifdef Q_OS_WIN
#include <QtWinExtras>

void MainWindow::initWindowsThumbnail()
{
    QWinThumbnailToolBar *pge_thumbbar = new QWinThumbnailToolBar(this);
    pge_thumbbar->setWindow(this->windowHandle());


    QWinThumbnailToolButton *thumbbnt_save = new QWinThumbnailToolButton(pge_thumbbar);
    thumbbnt_save->setToolTip(tr("Save"));
    thumbbnt_save->setIcon(QIcon(":/images/save.png"));
    thumbbnt_save->setDismissOnClick(true);
    connect(thumbbnt_save, SIGNAL(clicked()), this, SLOT(raise()));
    connect(thumbbnt_save, SIGNAL(clicked()), this, SLOT(on_actionSave_triggered()));

    QWinThumbnailToolButton *thumbbnt_open = new QWinThumbnailToolButton(pge_thumbbar);
    thumbbnt_open->setToolTip(tr("Open"));
    thumbbnt_open->setIcon(QIcon(":/images/open.png"));
    thumbbnt_open->setDismissOnClick(true);
    connect(thumbbnt_open, SIGNAL(clicked()), this, SLOT(on_OpenFile_triggered()));

    pge_thumbbar->addButton(thumbbnt_save);
    pge_thumbbar->addButton(thumbbnt_open);



    pge_thumbbar->setIconicPixmapNotificationsEnabled(true);
    connect(pge_thumbbar, SIGNAL(iconicThumbnailPixmapRequested()), this, SLOT(updateWindowsThumbnailPixmap()));

}

void MainWindow::updateWindowsThumbnailPixmap()
{
    QRect viewPort;

    if(activeChildWindow(latest) == 1){
        LevelEdit* edit = qobject_cast<LevelEdit*>(latest->widget());
        viewPort = edit->scene->getViewportRect();
    }else if(activeChildWindow(latest) == 3){
        WorldEdit* edit = qobject_cast<WorldEdit*>(latest->widget());
        viewPort = edit->scene->getViewportRect();
    }

    QPixmap previewPixmap(viewPort.width(), viewPort.height());
    QPainter previewPainter(&previewPixmap);

    if(activeChildWindow(latest) == 1){
        LevelEdit* edit = qobject_cast<LevelEdit*>(latest->widget());
        edit->scene->render(previewPainter, QRectF(0,0,viewPort.width(), viewPort.height()), QRectF(viewPort));
    }else if(activeChildWindow(latest) == 3){
        WorldEdit* edit = qobject_cast<WorldEdit*>(latest->widget());
        edit->scene->render(previewPainter, QRectF(0,0,viewPort.width(), viewPort.height()), QRectF(viewPort));
    }

}


#endif
