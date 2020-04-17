/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2014-2020 Vitaly Novichkov <admin@wohlnet.ru>
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

#include <QGraphicsItem>
#include <QPixmap>
#include <QGraphicsScene>
#include <QProgressDialog>
#include <QMimeData>
#include <QMimeType>
#include <QFileInfo>
#include <QElapsedTimer>
//#include <QGLWidget>
#include <QDebug>

#include <common_features/app_path.h>
#include <common_features/logger.h>
#include <editing/_scenes/level/lvl_scene.h>
#include <main_window/file/lvl_export_image.h>
#include <tools/smart_import/smartimporter.h>
#include <PGE_File_Formats/file_formats.h>

#include "level_edit.h"
#include <ui_leveledit.h>

LevelEdit::LevelEdit(MainWindow *mw, QWidget *parent) :
    EditBase(mw, parent),
    scene(nullptr),
    sceneCreated(false),
    m_isUntitled(true),
    updateTimer(nullptr),
    ui(new Ui::LevelEdit),
    m_fileType(FileFormats::LVL_PGEX)
{
    ui->setupUi(this);
    setAttribute(Qt::WA_DeleteOnClose);
    m_recentExportPath = AppPathManager::userAppDir();
    setWindowIcon(QIcon(QPixmap(":/lvl16.png")));

    ui->graphicsView->setOptimizationFlags(QGraphicsView::DontClipPainter);
    ui->graphicsView->setOptimizationFlags(QGraphicsView::DontSavePainterState);
    ui->graphicsView->setOptimizationFlags(QGraphicsView::DontAdjustForAntialiasing);
    ui->graphicsView->setRenderHint(QPainter::Antialiasing, false);

    ui->graphicsView->horizontalScrollBar()->setSingleStep(32);
    ui->graphicsView->horizontalScrollBar()->setTracking(true);
    ui->graphicsView->verticalScrollBar()->setSingleStep(32);
    ui->graphicsView->verticalScrollBar()->setTracking(true);

    if(GlobalSettings::LvlOpts.default_zoom != 100)
        setZoom(GlobalSettings::LvlOpts.default_zoom);

#ifdef Q_OS_ANDROID
    ui->graphicsView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->graphicsView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
#endif
}

void LevelEdit::reTranslate()
{
    QString backup = windowTitle();
    ui->retranslateUi(this);
    setWindowTitle(backup);
}

void LevelEdit::prepareLevelFile(LevelData &data)
{
    if(!sceneCreated || !scene)
        return;

    DataConfig *config = scene->m_configs;

    FileFormats::arrayIdLevelSortBGOs(data);

    // TODO: make here the synchronizarion with the main scene
    // Instead of real-time in-scene synchronization, total performance
    // and safety of workflow turning into one huge hell.
    // Instead of per-action synchronization, make the data collecting
    // from off the scene through this function.

    for(LevelNPC &npc : data.npc)
    {
        if(npc.id == 0)
            continue;

        const obj_npc &npc_config = config->main_npc[npc.id];

        if(!npc_config.isValid)
            continue;

        // Mark stars (for SMBX64 standard)
        npc.is_star = npc_config.setup.is_star;
        if((npc.is_star) && (npc.friendly)) // Non-takable stars? Nevermind!
            npc.is_star = false;
    }
}

void LevelEdit::focusInEvent(QFocusEvent *event)
{
    ui->graphicsView->setFocus();
    QWidget::focusInEvent(event);
}

void LevelEdit::dragEnterEvent(QDragEnterEvent *e)
{
    qDebug() << "Entered data into LevelEdit";
    if(e->mimeData()->hasUrls())
        e->acceptProposedAction();
}

void LevelEdit::dropEvent(QDropEvent *e)
{
    this->raise();
    this->setFocus(Qt::ActiveWindowFocusReason);
    qApp->setActiveWindow(this);

    bool requestReload = false;

    foreach(const QUrl &url, e->mimeData()->urls())
    {
        const QString &fileName = url.toLocalFile();
        if(QFileInfo(fileName).isDir())
        {
            SmartImporter *importer = new SmartImporter((QWidget *)this, fileName, (QWidget *)this);
            if(importer->isValid())
            {
                if(importer->attemptFastImport())
                {
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

static int renderTime = 0;
void LevelEdit::updateScene()
{
    if(renderTime > 32)
    {
        renderTime -= 32;
        return;
    }

    QElapsedTimer t;
    t.start();

    if(scene->m_opts.animationEnabled)
    {
        QRect viewport_rect(0, 0, ui->graphicsView->viewport()->width(), ui->graphicsView->viewport()->height());
        scene->update(ui->graphicsView->mapToScene(viewport_rect).boundingRect());
        ui->graphicsView->viewport()->update();
        update();
    }

    renderTime += t.elapsed();
}

void LevelEdit::setAutoUpdateTimer(int ms)
{
    if(updateTimer != nullptr)
        delete updateTimer;
    updateTimer = new QTimer;
    updateTimer->connect(
        updateTimer, SIGNAL(timeout()),
        this,
        SLOT(updateScene()));
    ui->graphicsView->setViewportUpdateMode(QGraphicsView::NoViewportUpdate);
    renderTime = 0;
    updateTimer->start(ms);
}

void LevelEdit::stopAutoUpdateTimer()
{
    if(updateTimer != nullptr)
    {
        updateTimer->stop();
        delete updateTimer;
        updateTimer = nullptr;
    }
    ui->graphicsView->setViewportUpdateMode(QGraphicsView::SmartViewportUpdate);
    renderTime = 0;
}
