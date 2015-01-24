#include "mainwindow.h"
#include <ui_mainwindow.h>

#ifdef Q_OS_WIN
#include <QtWinExtras>

void MainWindow::initWindowsThumbnail()
{
    pge_thumbbar = new QWinThumbnailToolBar(this);
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
    connect(ui->centralWidget, SIGNAL(subWindowActivated(QMdiSubWindow*)), this, SLOT(updateWindowsThumbnailPixmap()));
}

void MainWindow::updateWindowsThumbnailPixmap()
{
    QRect viewPort;

    if(!LastActiveSubWindow){
        drawDefaultThumb();
        return;
    }

    if(activeChildWindow(LastActiveSubWindow) == 1){
        LevelEdit* edit = qobject_cast<LevelEdit*>(LastActiveSubWindow->widget());
        viewPort = edit->scene->getViewportRect();
    }else if(activeChildWindow(LastActiveSubWindow) == 3){
        WorldEdit* edit = qobject_cast<WorldEdit*>(LastActiveSubWindow->widget());
        viewPort = edit->scene->getViewportRect();
    }else{
        drawDefaultThumb();
        return;
    }

    QPixmap previewPixmap(viewPort.width(), viewPort.height());
    QPainter previewPainter(&previewPixmap);

    if(activeChildWindow(LastActiveSubWindow) == 1){
        LevelEdit* edit = qobject_cast<LevelEdit*>(LastActiveSubWindow->widget());
        edit->scene->render(&previewPainter, QRectF(0,0,viewPort.width(), viewPort.height()), QRectF(viewPort));
    }else if(activeChildWindow(LastActiveSubWindow) == 3){
        WorldEdit* edit = qobject_cast<WorldEdit*>(LastActiveSubWindow->widget());
        edit->scene->render(&previewPainter, QRectF(0,0,viewPort.width(), viewPort.height()), QRectF(viewPort));
    }else{
        drawDefaultThumb();
        return;
    }



    if(pge_thumbbar){
        pge_thumbbar->setIconicThumbnailPixmap(previewPixmap);
    }
}

void MainWindow::drawDefaultThumb()
{
    QPixmap defPixmap(400,300);
    QPainter defThumbPainter(&defPixmap);
    defThumbPainter.setFont(QFont("Monospace", 30));
    defThumbPainter.fillRect(QRectF(defPixmap.rect()), QBrush(QColor(128,128,128), Qt::SolidPattern));
    defThumbPainter.drawText(defPixmap.rect(), Qt::AlignCenter, tr("No file loaded!"));
    pge_thumbbar->setIconicThumbnailPixmap(defPixmap);
}


#endif
