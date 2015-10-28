#include "mainwindow.h"
#include <ui_mainwindow.h>

#ifdef Q_OS_WIN
#include <QtWinExtras>
#include <QSysInfo>
#include <winbase.h>

void MainWindow::initWindowsThumbnail()
{
    if(QSysInfo::WindowsVersion<QSysInfo::WV_VISTA) return;

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
    connect(pge_thumbbar, SIGNAL(iconicThumbnailPixmapRequested()), this, SLOT(updateWindowsExtrasPixmap()));
    connect(pge_thumbbar, SIGNAL(iconicLivePreviewPixmapRequested()), this, SLOT(updateWindowsExtrasPixmap()));
    connect(ui->centralWidget, SIGNAL(subWindowActivated(QMdiSubWindow*)), this, SLOT(updateWindowsExtrasPixmap()));
}

void MainWindow::updateWindowsExtrasPixmap()
{
    if(QSysInfo::WindowsVersion<QSysInfo::WV_VISTA) return;
    QRect viewPort;

    if(!LastActiveSubWindow){
        drawWindowsDefaults();
        return;
    }

    if(activeChildWindow(LastActiveSubWindow) == 1){
        LevelEdit* edit = qobject_cast<LevelEdit*>(LastActiveSubWindow->widget());
        viewPort = edit->scene->getViewportRect();
    }else if(activeChildWindow(LastActiveSubWindow) == 3){
        WorldEdit* edit = qobject_cast<WorldEdit*>(LastActiveSubWindow->widget());
        viewPort = edit->scene->getViewportRect();
    }else{
        drawWindowsDefaults();
        return;
    }

    QPixmap thumbPixmap(viewPort.width(), viewPort.height());
    QPixmap livePreviewPixmap(size());
    QPainter thumbPainter(&thumbPixmap);
    QPainter livePreviewPainter(&livePreviewPixmap);

    if(activeChildWindow(LastActiveSubWindow) == 1){
        LevelEdit* edit = qobject_cast<LevelEdit*>(LastActiveSubWindow->widget());
        edit->scene->render(&thumbPainter, QRectF(0, 0, viewPort.width(), viewPort.height()), QRectF(viewPort));
        edit->scene->render(&livePreviewPainter, QRectF(0, 0, livePreviewPixmap.width(), livePreviewPixmap.height()), QRectF((qreal)viewPort.x(), (qreal)viewPort.y(), (qreal)livePreviewPixmap.width(), (qreal)livePreviewPixmap.height()));
    }else if(activeChildWindow(LastActiveSubWindow) == 3){
        WorldEdit* edit = qobject_cast<WorldEdit*>(LastActiveSubWindow->widget());
        edit->scene->render(&thumbPainter, QRectF(0, 0, viewPort.width(), viewPort.height()), QRectF(viewPort));
        edit->scene->render(&livePreviewPainter, QRectF(0, 0, livePreviewPixmap.width(), livePreviewPixmap.height()), QRectF((qreal)viewPort.x(), (qreal)viewPort.y(), (qreal)livePreviewPixmap.width(), (qreal)livePreviewPixmap.height()));
    }else{
        drawWindowsDefaults();
        return;
    }



    if(pge_thumbbar){
        pge_thumbbar->setIconicThumbnailPixmap(thumbPixmap);
        pge_thumbbar->setIconicLivePreviewPixmap(livePreviewPixmap);
    }
}

void MainWindow::drawWindowsDefaults()
{
    if(QSysInfo::WindowsVersion<QSysInfo::WV_VISTA) return;

    QPixmap defThumbPixmap(400,300);
    QPixmap defLivePreviewPixmap(size());
    QPainter defThumbPainter(&defThumbPixmap);
    QPainter defLivePreviewPainter(&defLivePreviewPixmap);

    defThumbPainter.setFont(QFont("Monospace", 30));
    defLivePreviewPainter.setFont(QFont("Monospace", 60));

    defThumbPainter.fillRect(QRectF(defThumbPixmap.rect()), QBrush(QColor(128,128,128), Qt::SolidPattern));
    defLivePreviewPainter.fillRect(QRectF(defLivePreviewPixmap.rect()), QBrush(QColor(128, 128, 128)));

    defThumbPainter.drawText(defThumbPixmap.rect(), Qt::AlignCenter, tr("No file loaded!"));
    defLivePreviewPainter.drawText(defLivePreviewPixmap.rect(), Qt::AlignCenter, tr("No file loaded!"));

    pge_thumbbar->setIconicThumbnailPixmap(defThumbPixmap);
    pge_thumbbar->setIconicLivePreviewPixmap(defLivePreviewPixmap);
}

#endif
