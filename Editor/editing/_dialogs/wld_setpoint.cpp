#include "wld_setpoint.h"
#include <ui_wld_setpoint.h>
#include "../../common_features/app_path.h"


WLD_SetPoint::WLD_SetPoint(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::WLD_SetPoint)
{
    ui->setupUi(this);

    scene = NULL;
    sceneCreated = false;
    FileType = 0;
    mapPoint = QPoint(-1,-1);
    mapPointIsNull=true;
    currentMusic = 0;
    isUntitled = true;
    latest_export_path = ApplicationPath;
    setWindowIcon(QIcon(QPixmap(":/images/world16.png")));

    updateTimer=NULL;

    ui->graphicsView->setOptimizationFlags(QGraphicsView::DontClipPainter);
    ui->graphicsView->setOptimizationFlags(QGraphicsView::DontSavePainterState);
    ui->graphicsView->setOptimizationFlags(QGraphicsView::DontAdjustForAntialiasing);

    ui->graphicsView->horizontalScrollBar()->setSingleStep(32);
    ui->graphicsView->horizontalScrollBar()->setTracking(true);
    ui->graphicsView->verticalScrollBar()->setSingleStep(32);
    ui->graphicsView->verticalScrollBar()->setTracking(true);

}

WLD_SetPoint::~WLD_SetPoint()
{
    delete ui;
}


void WLD_SetPoint::updateScene()
{
    if(scene->opts.animationEnabled)
    {
        QRect viewport_rect(0, 0, ui->graphicsView->viewport()->width(), ui->graphicsView->viewport()->height());
        scene->update( ui->graphicsView->mapToScene(viewport_rect).boundingRect() );
    }
}


void WLD_SetPoint::setAutoUpdateTimer(int ms)
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

void WLD_SetPoint::stopAutoUpdateTimer()
{
    if(updateTimer!=NULL)
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

    WriteToLog(QtDebugMsg, QString("Pos: %1, zoom %2, scenePos: %3")
               .arg(ui->graphicsView->horizontalScrollBar()->value())
               .arg(zoom).arg(x));

    ui->graphicsView->horizontalScrollBar()->setValue( qRound(qreal(x)*zoom)+offset.x() );
    ui->graphicsView->verticalScrollBar()->setValue( qRound(qreal(y)*zoom)+offset.y() );

    //scene->update();
    ui->graphicsView->update();

    update();

}













bool WLD_SetPoint::loadFile(const QString &fileName, WorldData FileData, dataconfigs &configs, LevelEditingSettings options)
{
    QFile file(fileName);
    WldData = FileData;
    setCurrentFile(fileName);
    WldData.modified = false;
    WldData.untitled = false;
    if (!file.open(QFile::ReadOnly | QFile::Text)) {
        QMessageBox::warning(this, tr("Read file error"),
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
        WriteToLog(QtCriticalMsg, QString("Error! *.INI configs not loaded"));

        QMessageBox::warning(this, tr("Configurations not loaded"),
                             tr("Cannot open level world %1:\nbecause object configurations are not loaded\n."
                                "Please check that the ""config/SMBX"" directory exists and contains the *.INI files with object settings.")
                             .arg(fileName));

        WriteToLog(QtCriticalMsg, QString(" << close subWindow"));

        this->close();

        WriteToLog(QtCriticalMsg, QString(" << closed, return false"));
        return false;
    }

    WriteToLog(QtDebugMsg, QString(">>Starting to load file"));

    //Declaring of the scene
    scene = new WldScene(ui->graphicsView, configs, WldData);

    scene->opts = options;
    scene->isSelectionDialog = true;

    ui->animation->setChecked(scene->opts.animationEnabled);

    //Preparing point selection mode
    scene->SwitchEditingMode(WldScene::MODE_SetPoint);
    if(mapPointIsNull)
    {
        scene->selectedPointNotUsed = true;
    }
    else
    {
        scene->selectedPointNotUsed = false;
        scene->selectedPoint = mapPoint;
    }
    scene->setItemPlacer(5);

    connect(scene, SIGNAL(pointSelected(QPoint)), this, SLOT(pointSelected(QPoint)));

    int DataSize=0;

    DataSize += 3;
    DataSize += 6;

    QProgressDialog progress(tr("Loading World map data"), tr("Abort"), 0, DataSize, this);
         progress.setWindowTitle(tr("Loading World data"));
         progress.setWindowModality(Qt::WindowModal);
         progress.setWindowFlags(Qt::Window | Qt::WindowTitleHint | Qt::CustomizeWindowHint | Qt::WindowStaysOnTopHint);
         progress.setFixedSize(progress.size());
         progress.setGeometry(QStyle::alignedRect(Qt::LeftToRight, Qt::AlignCenter, progress.size(), qApp->desktop()->availableGeometry()));
         progress.setMinimumDuration(0);

    if(! DrawObjects(progress) )
    {
        WldData.modified = false;
        this->close();
        return false;
    }

    QApplication::setOverrideCursor(Qt::WaitCursor);

    if( !progress.wasCanceled() )
        progress.close();

    QApplication::restoreOverrideCursor();

    setAutoUpdateTimer(31);

    WldData.modified = false;
    WldData.untitled = false;

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
    scene->loadUserData(progress);

        if(progress.wasCanceled()) return false;

        if(!progress.wasCanceled())
            progress.setLabelText(tr("1/%1 Applying Tiles").arg(TotalSteps));

    progress.setValue(progress.value()+1);
    qApp->processEvents();
    scene->setTiles(progress);

        if(progress.wasCanceled()) return false;

        if(!progress.wasCanceled())
            progress.setLabelText(tr("2/%1 Applying Sceneries...").arg(TotalSteps));

    progress.setValue(progress.value()+1);
    qApp->processEvents();
    scene->setSceneries(progress);

        if(progress.wasCanceled()) return false;

        if(!progress.wasCanceled())
            progress.setLabelText(tr("3/%1 Applying Paths...").arg(TotalSteps));

    progress.setValue(progress.value()+1);
    qApp->processEvents();
    scene->setPaths(progress);

        if(progress.wasCanceled()) return false;

        if(!progress.wasCanceled())
            progress.setLabelText(tr("4/%1 Applying Levels...").arg(TotalSteps));

    progress.setValue(progress.value()+1);
    progress.setValue(progress.value()+1);
    qApp->processEvents();
    scene->setLevels(progress);

        if(progress.wasCanceled()) return false;

        if(!progress.wasCanceled())
            progress.setLabelText(tr("5/%1 Applying Musics...").arg(TotalSteps));

    progress.setValue(progress.value()+1);
    qApp->processEvents();

    scene->setMusicBoxes(progress);

        if(progress.wasCanceled()) return false;

    if(scene->opts.animationEnabled)
        scene->startAnimation(); //Apply block animation

    if(!sceneCreated)
    {
        ui->graphicsView->setScene(scene);
        sceneCreated = true;
    }
    if(!progress.wasCanceled())
        progress.setValue(progress.maximum());
    return true;
}



void WLD_SetPoint::documentWasModified()
{
    WldData.modified = true;
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


void WLD_SetPoint::unloadData()
{
    if(!sceneCreated) return;

    stopAutoUpdateTimer();
    //LvlMusPlay::musicForceReset = true;
    //MainWinConnect::pMainWin->setMusicButton(false);
    //MainWinConnect::pMainWin->setMusic(false);

    scene->setMessageBoxItem(false);

    scene->clear();
    WriteToLog(QtDebugMsg, "!<-Cleared->!");
    scene->uTiles.clear();
    scene->uScenes.clear();
    scene->uPaths.clear();
    scene->uLevels.clear();

    WriteToLog(QtDebugMsg, "!<-Delete animators->!");
    while(! scene->animates_Tiles.isEmpty() )
    {
        SimpleAnimator* tmp = scene->animates_Tiles.first();
        scene->animates_Tiles.pop_front();
        if(tmp!=NULL) delete tmp;
    }
    while(! scene->animates_Scenery.isEmpty() )
    {
        SimpleAnimator* tmp = scene->animates_Scenery.first();
        scene->animates_Scenery.pop_front();
        if(tmp!=NULL) delete tmp;
    }
    while(! scene->animates_Paths.isEmpty() )
    {
        SimpleAnimator* tmp = scene->animates_Paths.first();
        scene->animates_Paths.pop_front();
        if(tmp!=NULL) delete tmp;
    }
    while(! scene->animates_Levels.isEmpty() )
    {
        SimpleAnimator* tmp = scene->animates_Levels.first();
        scene->animates_Levels.pop_front();
        if(tmp!=NULL) delete tmp;
    }

    WriteToLog(QtDebugMsg, "!<-Delete scene->!");
    delete scene;
    sceneCreated=false;
    WriteToLog(QtDebugMsg, "!<-Deleted->!");
}

QWidget *WLD_SetPoint::gViewPort()
{
    return ui->graphicsView->viewport();
}

void WLD_SetPoint::setCurrentFile(const QString &fileName)
{
    curFile = QFileInfo(fileName).canonicalFilePath();
    isUntitled = false;
    WldData.path = QFileInfo(fileName).absoluteDir().absolutePath();
    WldData.filename = QFileInfo(fileName).baseName();
    WldData.untitled = false;
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
        QDialog::reject();
    }
}

void WLD_SetPoint::on_ResetPosition_clicked()
{
    ResetPosition();
}


void WLD_SetPoint::on_animation_clicked(bool checked)
{
    scene->opts.animationEnabled = checked;

    if(checked)
        scene->startAnimation();
    else
        scene->stopAnimation();

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
