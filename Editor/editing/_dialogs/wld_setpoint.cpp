/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2014-2019 Vitaly Novichkov <admin@wohlnet.ru>
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

#include <common_features/app_path.h>
#include <common_features/util.h>

#include "wld_setpoint.h"
#include <ui_wld_setpoint.h>

WLD_SetPoint::WLD_SetPoint(QWidget *parent) :
    QDialog(parent)
{
    ui.reset(new Ui::WLD_SetPoint);
    ui->setupUi(this);

    m_mw = qobject_cast<MainWindow *>(parent);

    m_sceneCreated = false;
    m_mapPoint = QPoint(-1, -1);
    setWindowIcon(QIcon(QPixmap(":/images/world16.png")));

    ui->graphicsView->setOptimizationFlags(QGraphicsView::DontClipPainter);
    ui->graphicsView->setOptimizationFlags(QGraphicsView::DontSavePainterState);
    ui->graphicsView->setOptimizationFlags(QGraphicsView::DontAdjustForAntialiasing);

    ui->graphicsView->horizontalScrollBar()->setSingleStep(32);
    ui->graphicsView->horizontalScrollBar()->setTracking(true);
    ui->graphicsView->verticalScrollBar()->setSingleStep(32);
    ui->graphicsView->verticalScrollBar()->setTracking(true);

    ui->graphicsView->setFocus();
}

WLD_SetPoint::~WLD_SetPoint()
{
    m_scene.reset();
    ui.reset();
}


void WLD_SetPoint::updateScene()
{
    if(m_scene->m_opts.animationEnabled)
    {
        QRect viewport_rect(0, 0,
                            ui->graphicsView->viewport()->width(),
                            ui->graphicsView->viewport()->height());
        m_scene->update(ui->graphicsView->mapToScene(viewport_rect).boundingRect());
    }
}


void WLD_SetPoint::setAutoUpdateTimer(int ms)
{
    m_updateTimer.reset(new QTimer);
    QObject::connect(m_updateTimer.get(),
                     SIGNAL(timeout()),
                     this,
                     SLOT(updateScene()));
    m_updateTimer->start(ms);
}

void WLD_SetPoint::stopAutoUpdateTimer()
{
    if(m_updateTimer.get() != nullptr)
    {
        m_updateTimer->stop();
        m_updateTimer.reset();
    }
}


void WLD_SetPoint::resetPosition()
{
    goTo(0, 0, QPoint(-10, -10));
}

void WLD_SetPoint::goTo(long x, long y, QPoint offset)
{
    qreal zoom = 1.0;
    if(QString(ui->graphicsView->metaObject()->className()) == "GraphicsWorkspace")
        zoom = static_cast<GraphicsWorkspace *>(ui->graphicsView)->zoom();

    LogDebug(QString("Pos: %1, zoom %2, scenePos: %3")
             .arg(ui->graphicsView->horizontalScrollBar()->value())
             .arg(zoom).arg(x));

    ui->graphicsView->horizontalScrollBar()->setValue(qRound(qreal(x)*zoom) + offset.x());
    ui->graphicsView->verticalScrollBar()->setValue(qRound(qreal(y)*zoom) + offset.y());

    //scene->update();
    ui->graphicsView->update();

    update();

}


bool WLD_SetPoint::loadFile(const WorldData &fileData,
                            dataconfigs &configs,
                            EditingSettings options)
{
    m_worldData = fileData;
    setWindowModified(false);
    m_worldData.meta.modified = false;
    m_worldData.meta.untitled = false;

    ui->graphicsView->setBackgroundBrush(QBrush(Qt::black));

    ui->graphicsView->setInteractive(true);
    ui->graphicsView->setCursor(Qt::CrossCursor);
    ui->graphicsView->setDragMode(QGraphicsView::NoDrag);
    ui->graphicsView->setRenderHint(QPainter::Antialiasing, true);
    ui->graphicsView->viewport()->setMouseTracking(true);

    // Check is config pack valid
    if(configs.check())
    {
        LogCritical(QString("WLD_SetPoint: Error! config pack is invalid"));
        QMessageBox::warning(this,
                             tr("Configuration package has errors"),
                             tr("Cannot load the \"%1\" world map because of errors in a configuration package.")
                             .arg(fileData.meta.filename));
        LogCritical(QString("WLD_SetPoint: close subWindow"));
        this->close();

        LogCritical(QString("WLD_SetPoint: closed, return false"));
        return false;
    }

    LogDebug(QString(">>Starting to load file"));

    // Initializing the scene
    m_scene.reset(new WldScene(m_mw, ui->graphicsView, configs, m_worldData, this));
    Q_ASSERT(m_scene.get());

    m_scene->m_opts = options;
    m_scene->m_isSelectionDialog = true;

    ui->animation->setChecked(m_scene->m_opts.animationEnabled);

    // Preparing a point selection mode
    m_scene->SwitchEditingMode(WldScene::MODE_SetPoint);
    if(m_mapPointIsNull)
        m_scene->m_pointSelector.m_pointNotPlaced = true;
    else
    {
        m_scene->m_pointSelector.m_pointNotPlaced = false;
        m_scene->m_pointSelector.m_pointCoord = m_mapPoint;
    }
    m_scene->setItemPlacer(5);

    QObject::connect(&m_scene->m_pointSelector,
                     &WldPointSelector::pointSelected,
                     this,
                     &WLD_SetPoint::pointSelected);

    int dataSize = 3 + 6;
    QProgressDialog progress(tr("Loading World map data"), tr("Abort"), 0, dataSize, this);
    progress.setWindowTitle(tr("Loading World map data"));
    progress.setWindowModality(Qt::WindowModal);
    progress.setWindowFlags(Qt::Window | Qt::WindowTitleHint | Qt::CustomizeWindowHint | Qt::WindowStaysOnTopHint);
    progress.setFixedSize(progress.size());
    progress.setGeometry(util::alignToScreenCenter(progress.size()));
    progress.setMinimumDuration(0);

    if(!buildScene(progress))
    {
        m_worldData.meta.modified = false;
        this->close();
        return false;
    }

    QApplication::setOverrideCursor(Qt::WaitCursor);

    if(!progress.wasCanceled())
        progress.close();

    QApplication::restoreOverrideCursor();

    setAutoUpdateTimer(31);

    m_worldData.meta.modified = false;
    m_worldData.meta.untitled = false;

    progress.deleteLater();

    return true;
}


void WLD_SetPoint::pointSelected(QPoint p)
{
    m_mapPoint = p;
    m_mapPointIsNull = false;
    ui->x_point->setText(QString::number(p.x()));
    ui->y_point->setText(QString::number(p.y()));
}



bool WLD_SetPoint::buildScene(QProgressDialog &progress)
{
    //int DataSize = progress.maximum();
    int totalSteps = 5;

    if(!progress.wasCanceled())
        progress.setLabelText(tr("1/%1 Loading user data").arg(totalSteps));

    qApp->processEvents();
    m_scene->loadUserData(progress);

    if(progress.wasCanceled())
        return false;

    if(!progress.wasCanceled())
        progress.setLabelText(tr("1/%1 Applying Tiles").arg(totalSteps));

    progress.setValue(progress.value() + 1);
    qApp->processEvents();
    m_scene->setTiles(progress);

    if(progress.wasCanceled())
        return false;

    if(!progress.wasCanceled())
        progress.setLabelText(tr("2/%1 Applying Sceneries...").arg(totalSteps));

    progress.setValue(progress.value() + 1);
    qApp->processEvents();
    m_scene->setSceneries(progress);

    if(progress.wasCanceled())
        return false;

    if(!progress.wasCanceled())
        progress.setLabelText(tr("3/%1 Applying Paths...").arg(totalSteps));

    progress.setValue(progress.value() + 1);
    qApp->processEvents();
    m_scene->setPaths(progress);

    if(progress.wasCanceled())
        return false;

    if(!progress.wasCanceled())
        progress.setLabelText(tr("4/%1 Applying Levels...").arg(totalSteps));

    progress.setValue(progress.value() + 1);
    progress.setValue(progress.value() + 1);
    qApp->processEvents();
    m_scene->setLevels(progress);

    if(progress.wasCanceled())
        return false;

    if(!progress.wasCanceled())
        progress.setLabelText(tr("5/%1 Applying Musics...").arg(totalSteps));

    progress.setValue(progress.value() + 1);
    qApp->processEvents();

    m_scene->setMusicBoxes(progress);

    if(progress.wasCanceled())
        return false;

    if(m_scene->m_opts.animationEnabled)
        m_scene->startAnimation(); //Apply block animation

    if(!m_sceneCreated)
    {
        ui->graphicsView->setScene(m_scene.get());
        m_sceneCreated = true;
    }

    if(!progress.wasCanceled())
        progress.setValue(progress.maximum());

    return true;
}







//////// Common
void WLD_SetPoint::closeEvent(QCloseEvent *event)
{
    if(!m_sceneCreated)
    {
        event->accept();
        return;
    }

    unloadData();
    event->accept();
}

void WLD_SetPoint::keyPressEvent(QKeyEvent *e)
{
    ui->graphicsView->setFocus(Qt::PopupFocusReason);
    QDialog::keyPressEvent(e);
}


void WLD_SetPoint::unloadData()
{
    if(!m_sceneCreated)
        return;

    LogDebug("WLD_SetPoint: Clean scene");
    stopAutoUpdateTimer();
    m_scene->setMessageBoxItem(false);
    m_scene->clear();

    LogDebug("WLD_SetPoint: Delete animators");
    while(! m_scene->m_animatorsTerrain.isEmpty())
    {
        SimpleAnimator *tmp = m_scene->m_animatorsTerrain.first();
        m_scene->m_animatorsTerrain.pop_front();
        if(tmp != nullptr) delete tmp;
    }
    while(! m_scene->m_animatorsScenery.isEmpty())
    {
        SimpleAnimator *tmp = m_scene->m_animatorsScenery.first();
        m_scene->m_animatorsScenery.pop_front();
        if(tmp != nullptr) delete tmp;
    }
    while(! m_scene->m_animatorsPaths.isEmpty())
    {
        SimpleAnimator *tmp = m_scene->m_animatorsPaths.first();
        m_scene->m_animatorsPaths.pop_front();
        if(tmp != nullptr) delete tmp;
    }
    while(! m_scene->m_animatorsLevels.isEmpty())
    {
        SimpleAnimator *tmp = m_scene->m_animatorsLevels.first();
        m_scene->m_animatorsLevels.pop_front();
        if(tmp != nullptr) delete tmp;
    }

    m_scene->m_localConfigTerrain.clear();
    m_scene->m_localConfigScenery.clear();
    m_scene->m_localConfigPaths.clear();
    m_scene->m_localConfigLevels.clear();

    LogDebug("WLD_SetPoint: Delete scene");
    m_scene.reset();
    m_sceneCreated = false;
    LogDebug("WLD_SetPoint: Scene clean up completed");
}

QWidget *WLD_SetPoint::gViewPort()
{
    return ui->graphicsView->viewport();
}



void WLD_SetPoint::on_buttonBox_clicked(QAbstractButton *button)
{
    if(ui->buttonBox->buttonRole(button) == QDialogButtonBox::AcceptRole)
    {
        if(m_mapPointIsNull)
        {
            QMessageBox::information(this,
                                     tr("Point is not selected"),
                                     tr("Select the point on the world map first."),
                                     QMessageBox::Ok);
            return;
        }

        unloadData();
        QDialog::accept();
    }
    else
    {
        unloadData();
        QDialog::reject();
    }
}

void WLD_SetPoint::on_ResetPosition_clicked()
{
    resetPosition();
}


void WLD_SetPoint::on_animation_clicked(bool checked)
{
    m_scene->m_opts.animationEnabled = checked;

    if(checked)
        m_scene->startAnimation();
    else
        m_scene->stopAnimation();

}

void WLD_SetPoint::on_GotoPoint_clicked()
{
    if(!m_mapPointIsNull)
    {
        goTo(m_mapPoint.x() + 16,
             m_mapPoint.y() + 16,
             QPoint(-qRound(qreal(ui->graphicsView->viewport()->width()) / 2),
                    -qRound(qreal(ui->graphicsView->viewport()->height()) / 2))
        );
    }
}
