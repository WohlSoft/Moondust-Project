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

#include <QDesktopWidget>

#include <common_features/app_path.h>
#include <common_features/util.h>

#include "wld_setpoint.h"
#include <ui_wld_setpoint.h>

WLD_SetPoint::WLD_SetPoint(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::WLD_SetPoint)
{
    ui->setupUi(this);

    m_mw = qobject_cast<MainWindow*>(parent);

    m_scene = nullptr;
    sceneCreated = false;
    FileType = 0;
    mapPoint = QPoint(-1,-1);
    mapPointIsNull=true;
    currentMusic = 0;
    isUntitled = true;
    latest_export_path = AppPathManager::userAppDir();
    setWindowIcon(QIcon(QPixmap(":/images/world16.png")));

    updateTimer = nullptr;

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
    if(m_scene)
        delete m_scene;
    delete ui;
}


void WLD_SetPoint::updateScene()
{
    if(m_scene->m_opts.animationEnabled)
    {
        QRect viewport_rect(0, 0, ui->graphicsView->viewport()->width(), ui->graphicsView->viewport()->height());
        m_scene->update( ui->graphicsView->mapToScene(viewport_rect).boundingRect() );
    }
}


void WLD_SetPoint::setAutoUpdateTimer(int ms)
{
    if(updateTimer != nullptr)
        delete updateTimer;
    updateTimer = new QTimer;
    connect(
                updateTimer, SIGNAL(timeout()),
                this,
                SLOT( updateScene()) );
    updateTimer->start(ms);
}

void WLD_SetPoint::stopAutoUpdateTimer()
{
    if(updateTimer != nullptr)
    {
        updateTimer->stop();
        delete updateTimer;
    }
}


void WLD_SetPoint::ResetPosition()
{
    //    LvlData.sections[LvlData.CurSection].PositionX =
    //            LvlData.sections[LvlData.CurSection].size_left;
    //    LvlData.sections[LvlData.CurSection].PositionY =
    //            LvlData.sections[LvlData.CurSection].size_bottom-602;

    //goTo(LvlData.sections[LvlData.CurSection].size_left, LvlData.sections[LvlData.CurSection].size_bottom-602, false, QPoint(-10,10));
    goTo(0, 0, false, QPoint(-10,-10));
}

void WLD_SetPoint::goTo(long x, long y, bool SwitchToSection, QPoint offset)
{

    if(SwitchToSection)
    {
//        for(int i=0; i<LvlData.sections.size(); i++)
//        {
//            if( (x >= LvlData.sections[i].size_left) &&
//                (x <= LvlData.sections[i].size_right) &&
//                (y >= LvlData.sections[i].size_top) &&
//                (y <= LvlData.sections[i].size_bottom) )
//            {
//                    MainWinConnect::pMainWin->SetCurrentLevelSection(i);
//                    break;
//            }
//        }
    }

    qreal zoom=1.0;
    if(QString(ui->graphicsView->metaObject()->className())=="GraphicsWorkspace")
    {
        zoom = static_cast<GraphicsWorkspace *>(ui->graphicsView)->zoom();
    }

    LogDebug(QString("Pos: %1, zoom %2, scenePos: %3")
               .arg(ui->graphicsView->horizontalScrollBar()->value())
               .arg(zoom).arg(x));

    ui->graphicsView->horizontalScrollBar()->setValue( qRound(qreal(x)*zoom)+offset.x() );
    ui->graphicsView->verticalScrollBar()->setValue( qRound(qreal(y)*zoom)+offset.y() );

    //scene->update();
    ui->graphicsView->update();

    update();

}













bool WLD_SetPoint::loadFile(const QString &fileName, WorldData FileData, dataconfigs &configs, EditingSettings options)
{
    QFile file(fileName);
    WldData = FileData;
    setCurrentFile(fileName);
    WldData.meta.modified = false;
    WldData.meta.untitled = false;
    if (!file.open(QFile::ReadOnly | QFile::Text)) {
        QMessageBox::warning(this, tr("File read error"),
                             tr("Cannot read file %1:\n%2.")
                             .arg(fileName)
                             .arg(file.errorString()));
        return false;
    }
    StartWldData = WldData; //Save current history for made reset

    ui->graphicsView->setBackgroundBrush(QBrush(Qt::black));

    ui->graphicsView->setInteractive(true);
    ui->graphicsView->setCursor(Qt::CrossCursor);
    ui->graphicsView->setDragMode(QGraphicsView::NoDrag);
    ui->graphicsView->setRenderHint(QPainter::Antialiasing, true);
    ui->graphicsView->viewport()->setMouseTracking(true);

    //Check if data configs exists
    if( configs.check() )
    {
        LogCritical(QString("Error! *.INI configs not loaded"));

        QMessageBox::warning(this, tr("Configurations not loaded"),
                             tr("Cannot open level world %1:\nbecause object configurations are not loaded\n."
                                "Please check that the ""config/SMBX"" directory exists and contains the *.INI files with object settings.")
                             .arg(fileName));

        LogCritical(QString(" << close subWindow"));

        this->close();

        LogCritical(QString(" << closed, return false"));
        return false;
    }

    LogDebug(QString(">>Starting to load file"));

    //Declaring of the scene
    m_scene = new WldScene(m_mw, ui->graphicsView, configs, WldData, this);

    m_scene->m_opts = options;
    m_scene->m_isSelectionDialog = true;

    ui->animation->setChecked(m_scene->m_opts.animationEnabled);

    //Preparing point selection mode
    m_scene->SwitchEditingMode(WldScene::MODE_SetPoint);
    if(mapPointIsNull)
    {
        m_scene->m_pointSelector.m_pointNotPlaced = true;
    }
    else
    {
        m_scene->m_pointSelector.m_pointNotPlaced = false;
        m_scene->m_pointSelector.m_pointCoord = mapPoint;
    }
    m_scene->setItemPlacer(5);

    connect(&m_scene->m_pointSelector, &WldPointSelector::pointSelected, this, &WLD_SetPoint::pointSelected);

    int DataSize=0;

    DataSize += 3;
    DataSize += 6;

    QProgressDialog progress(tr("Loading World map data"), tr("Abort"), 0, DataSize, this);
         progress.setWindowTitle(tr("Loading World map data"));
         progress.setWindowModality(Qt::WindowModal);
         progress.setWindowFlags(Qt::Window | Qt::WindowTitleHint | Qt::CustomizeWindowHint | Qt::WindowStaysOnTopHint);
         progress.setFixedSize(progress.size());
         progress.setGeometry(QStyle::alignedRect(Qt::LeftToRight, Qt::AlignCenter, progress.size(), qApp->desktop()->availableGeometry()));
         progress.setMinimumDuration(0);

    if(! DrawObjects(progress) )
    {
        WldData.meta.modified = false;
        this->close();
        return false;
    }

    QApplication::setOverrideCursor(Qt::WaitCursor);

    if( !progress.wasCanceled() )
        progress.close();

    QApplication::restoreOverrideCursor();

    setAutoUpdateTimer(31);

    WldData.meta.modified = false;
    WldData.meta.untitled = false;

    progress.deleteLater();

    return true;
}


void WLD_SetPoint::pointSelected(QPoint p)
{
    mapPoint = p;
    mapPointIsNull=false;
    ui->x_point->setText(QString::number(p.x()));
    ui->y_point->setText(QString::number(p.y()));
}



bool WLD_SetPoint::DrawObjects(QProgressDialog &progress)
{
    //int DataSize = progress.maximum();
    int TotalSteps = 5;

        if(!progress.wasCanceled())

            progress.setLabelText(tr("1/%1 Loading user data").arg(TotalSteps));

    qApp->processEvents();
    m_scene->loadUserData(progress);

        if(progress.wasCanceled()) return false;

        if(!progress.wasCanceled())
            progress.setLabelText(tr("1/%1 Applying Tiles").arg(TotalSteps));

    progress.setValue(progress.value()+1);
    qApp->processEvents();
    m_scene->setTiles(progress);

        if(progress.wasCanceled()) return false;

        if(!progress.wasCanceled())
            progress.setLabelText(tr("2/%1 Applying Sceneries...").arg(TotalSteps));

    progress.setValue(progress.value()+1);
    qApp->processEvents();
    m_scene->setSceneries(progress);

        if(progress.wasCanceled()) return false;

        if(!progress.wasCanceled())
            progress.setLabelText(tr("3/%1 Applying Paths...").arg(TotalSteps));

    progress.setValue(progress.value()+1);
    qApp->processEvents();
    m_scene->setPaths(progress);

        if(progress.wasCanceled()) return false;

        if(!progress.wasCanceled())
            progress.setLabelText(tr("4/%1 Applying Levels...").arg(TotalSteps));

    progress.setValue(progress.value()+1);
    progress.setValue(progress.value()+1);
    qApp->processEvents();
    m_scene->setLevels(progress);

        if(progress.wasCanceled()) return false;

        if(!progress.wasCanceled())
            progress.setLabelText(tr("5/%1 Applying Musics...").arg(TotalSteps));

    progress.setValue(progress.value()+1);
    qApp->processEvents();

    m_scene->setMusicBoxes(progress);

        if(progress.wasCanceled()) return false;

    if(m_scene->m_opts.animationEnabled)
        m_scene->startAnimation(); //Apply block animation

    if(!sceneCreated)
    {
        ui->graphicsView->setScene(m_scene);
        sceneCreated = true;
    }
    if(!progress.wasCanceled())
        progress.setValue(progress.maximum());
    return true;
}



void WLD_SetPoint::documentWasModified()
{
    WldData.meta.modified = true;
}









//////// Common
QString WLD_SetPoint::userFriendlyCurrentFile()
{
    return strippedName(curFile);
}

void WLD_SetPoint::closeEvent(QCloseEvent *event)
{
    if(!sceneCreated)
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
    if(!sceneCreated) return;

    stopAutoUpdateTimer();
    //LvlMusPlay::musicForceReset = true;
    //MainWinConnect::pMainWin->setMusicButton(false);
    //MainWinConnect::pMainWin->setMusic(false);

    m_scene->setMessageBoxItem(false);

    m_scene->clear();
    LogDebug("!<-Cleared->!");

    LogDebug("!<-Delete animators->!");
    while(! m_scene->m_animatorsTerrain.isEmpty() )
    {
        SimpleAnimator* tmp = m_scene->m_animatorsTerrain.first();
        m_scene->m_animatorsTerrain.pop_front();
        if(tmp!=nullptr) delete tmp;
    }
    while(! m_scene->m_animatorsScenery.isEmpty() )
    {
        SimpleAnimator* tmp = m_scene->m_animatorsScenery.first();
        m_scene->m_animatorsScenery.pop_front();
        if(tmp!=nullptr) delete tmp;
    }
    while(! m_scene->m_animatorsPaths.isEmpty() )
    {
        SimpleAnimator* tmp = m_scene->m_animatorsPaths.first();
        m_scene->m_animatorsPaths.pop_front();
        if(tmp!=nullptr) delete tmp;
    }
    while(! m_scene->m_animatorsLevels.isEmpty() )
    {
        SimpleAnimator* tmp = m_scene->m_animatorsLevels.first();
        m_scene->m_animatorsLevels.pop_front();
        if(tmp!=nullptr) delete tmp;
    }

    m_scene->m_localConfigTerrain.clear();
    m_scene->m_localConfigScenery.clear();
    m_scene->m_localConfigPaths.clear();
    m_scene->m_localConfigLevels.clear();

    LogDebug("!<-Delete scene->!");
    delete m_scene;
    m_scene = nullptr;
    sceneCreated=false;
    LogDebug("!<-Deleted->!");
}

QWidget *WLD_SetPoint::gViewPort()
{
    return ui->graphicsView->viewport();
}

void WLD_SetPoint::setCurrentFile(const QString &fileName)
{
    QFileInfo info(fileName);
    curFile = info.canonicalFilePath();
    isUntitled = false;
    WldData.meta.path = info.absoluteDir().absolutePath();
    WldData.meta.filename = util::getBaseFilename(info.fileName());
    WldData.meta.untitled = false;
    //document()->setModified(false);
    setWindowModified(false);
    //setWindowTitle(WldData.EpisodeTitle =="" ? userFriendlyCurrentFile() : WldData.EpisodeTitle );
}

QString WLD_SetPoint::strippedName(const QString &fullFileName)
{
    return QFileInfo(fullFileName).fileName();
}


void WLD_SetPoint::on_buttonBox_clicked(QAbstractButton *button)
{
    if(ui->buttonBox->buttonRole(button)==QDialogButtonBox::AcceptRole)
    {
        if(mapPointIsNull)
        {
            QMessageBox::information(this, tr("Point is not selected"), tr("Select the point on the world map first."), QMessageBox::Ok);
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
    ResetPosition();
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
    if(!mapPointIsNull)
    {
        goTo(mapPoint.x()+16, mapPoint.y()+16,
                                          false,
                                            QPoint(-qRound(qreal(ui->graphicsView->viewport()->width())/2),
                                                   -qRound(qreal(ui->graphicsView->viewport()->height())/2))
                                          );
    }
}
