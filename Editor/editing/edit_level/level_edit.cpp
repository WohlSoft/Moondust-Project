/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2014 Vitaly Novichkov <admin@wohlnet.ru>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <QtWidgets>
#include <QGraphicsItem>
#include <QPixmap>
#include <QGraphicsScene>
#include <QProgressDialog>

#include "level_edit.h"
#include <ui_leveledit.h>

#include "./file_formats/file_formats.h"
#include "../_scenes/level/lvl_scene.h"
#include <main_window/file/lvl_export_image.h>
#include "./common_features/app_path.h"
#include "./common_features/logger.h"
#include "./tools/smart_import/smartimporter.h"

//#include <QGLWidget>


#include <QDebug>

LevelEdit::LevelEdit(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::LevelEdit)
{
    sceneCreated = false;
    FileType = 0;
    setAttribute(Qt::WA_DeleteOnClose);
    isUntitled = true;
    isModified = false;
    latest_export_path = ApplicationPath;
    setWindowIcon(QIcon(QPixmap(":/lvl16.png")));
    ui->setupUi(this);
    updateTimer=NULL;

    ui->graphicsView->setOptimizationFlags(QGraphicsView::DontClipPainter);
    ui->graphicsView->setOptimizationFlags(QGraphicsView::DontSavePainterState);
    ui->graphicsView->setOptimizationFlags(QGraphicsView::DontAdjustForAntialiasing);

    ui->graphicsView->horizontalScrollBar()->setSingleStep(32);
    ui->graphicsView->horizontalScrollBar()->setTracking(true);
    ui->graphicsView->verticalScrollBar()->setSingleStep(32);
    ui->graphicsView->verticalScrollBar()->setTracking(true);

    //ui->graphicsView->setViewport(new QGLWidget(QGLFormat(QGL::SampleBuffers | QGL::DirectRendering)));
    //ui->graphicsView->setViewportUpdateMode(QGraphicsView::SmartViewportUpdate);

            /*
             * 	setOptimizationFlags(QGraphicsView::DontClipPainter);
        setOptimizationFlags(QGraphicsView::DontSavePainterState);
        setOptimizationFlags(QGraphicsView::DontAdjustForAntialiasing);*/
    //connect(ui->graphicsView, SIGNAL(keyPressEvent(QKeyEvent *)), this, SLOT(gViewKeyPress(QKeyEvent *)));
}


void LevelEdit::focusInEvent(QFocusEvent *event)
{
    ui->graphicsView->setFocus();
    QWidget::focusInEvent(event);
}

void LevelEdit::dragEnterEvent(QDragEnterEvent *e)
{
    qDebug() << "Entered data into LevelEdit";
    if (e->mimeData()->hasUrls()) {
        e->acceptProposedAction();
    }
}

void LevelEdit::dropEvent(QDropEvent *e)
{
    this->raise();
    this->setFocus(Qt::ActiveWindowFocusReason);
    qApp->setActiveWindow(this);

    bool requestReload = false;

    foreach (const QUrl &url, e->mimeData()->urls()) {
        const QString &fileName = url.toLocalFile();
        if(QFileInfo(fileName).isDir())
        {
            SmartImporter * importer = new SmartImporter((QWidget*)this, fileName, (QWidget*)this);
            if(importer->isValid()){
                if(importer->attemptFastImport()){
                    requestReload = true;
                    delete importer;
                    break;
                }
            }
            delete importer;
        }
    }
    if(requestReload)
        emit forceReload();
}


void LevelEdit::updateScene()
{
    if(scene->opts.animationEnabled)
    {
        QRect viewport_rect(0, 0, ui->graphicsView->viewport()->width(), ui->graphicsView->viewport()->height());
        scene->update( ui->graphicsView->mapToScene(viewport_rect).boundingRect() );
    }
}

void LevelEdit::setAutoUpdateTimer(int ms)
{
    if(updateTimer!=NULL)
        delete updateTimer;
    updateTimer = new QTimer;
    connect(
                updateTimer, SIGNAL(timeout()),
                this,
                SLOT( updateScene()) );
    updateTimer->start(ms);
}

void LevelEdit::stopAutoUpdateTimer()
{
    if(updateTimer!=NULL)
    {
        updateTimer->stop();
        delete updateTimer;
    }
}
