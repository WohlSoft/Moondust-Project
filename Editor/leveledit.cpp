/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2014 Vitaly Novichkov <admin@wohlnet.ru>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA
 */

#include <QtWidgets>
#include <QGraphicsItem>
#include <QPixmap>
#include <QGraphicsScene>
#include <QProgressDialog>

#include "leveledit.h"
#include "ui_leveledit.h"
#include "lvl_filedata.h"
#include "lvlscene.h"
#include "dataconfigs.h"
#include "saveimage.h"
#include "logger.h"


#include <QDebug>

leveledit::leveledit(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::leveledit)
{
    sceneCreared = false;
    FileType = 0;
    setAttribute(Qt::WA_DeleteOnClose);
    isUntitled = true;
    isModified = false;
    latest_export = "*.png";
    latest_export_path = QApplication::applicationDirPath();
    setWindowIcon(QIcon(QPixmap(":/lvl16.png")));
    ui->setupUi(this);

    ui->graphicsView->setOptimizationFlags(QGraphicsView::DontClipPainter);
    ui->graphicsView->setOptimizationFlags(QGraphicsView::DontSavePainterState);
    ui->graphicsView->setOptimizationFlags(QGraphicsView::DontAdjustForAntialiasing);

            /*
             * 	setOptimizationFlags(QGraphicsView::DontClipPainter);
        setOptimizationFlags(QGraphicsView::DontSavePainterState);
        setOptimizationFlags(QGraphicsView::DontAdjustForAntialiasing);*/
}

void leveledit::mouseReleaseEvent( QMouseEvent * event )
{
    /*
    if(scene->PasteFromBuffer)
    {
       changeCursor(0);
       scene->PasteFromBuffer=false;
    }*/
    QWidget::mouseReleaseEvent( event );
}

leveledit::~leveledit()
{
    //free(scene);
    delete ui;
}


void leveledit::newFile()
{
    static int sequenceNumber = 1;

    isUntitled = true;
    curFile = tr("Untitled %1").arg(sequenceNumber++);
    setWindowTitle(curFile + "[*]");

    /*connect(document(), SIGNAL(contentsChanged()),
            this, SLOT(documentWasModified()));*/
}

void leveledit::ExportToImage_fn()
{
        long x, y, h, w, th, tw;

        bool proportion;
        QString inifile = QApplication::applicationDirPath() + "/" + "plweditor.ini";
        QSettings settings(inifile, QSettings::IniFormat);
        settings.beginGroup("Main");
        latest_export = settings.value("export-file", "*.png").toString();
        latest_export_path = settings.value("export-path", QApplication::applicationDirPath()).toString();
        proportion = settings.value("export-proportions", false).toBool();
        settings.endGroup();


        x=LvlData.sections[LvlData.CurSection].size_left;
        y=LvlData.sections[LvlData.CurSection].size_top;
        w=LvlData.sections[LvlData.CurSection].size_right;
        h=LvlData.sections[LvlData.CurSection].size_bottom;
        w=(long)fabs(x-w);
        h=(long)fabs(y-h);

        tw=w;
        th=h;
        QVector<long> imgSize;

        imgSize.push_back(th);
        imgSize.push_back(tw);
        imgSize.push_back((int)proportion);

        ExportToImage ExportImage(imgSize);
        ExportImage.setWindowFlags (Qt::Window | Qt::WindowTitleHint | Qt::WindowCloseButtonHint);
        ExportImage.setGeometry(QStyle::alignedRect(Qt::LeftToRight, Qt::AlignCenter, ExportImage.size(), qApp->desktop()->availableGeometry()));
        if(ExportImage.exec()!=QDialog::Rejected)
            imgSize = ExportImage.imageSize;
        else return;

        if(imgSize.size()>=3)
            if((imgSize[0]<0)||(imgSize[1]<0))
                return;

        QString fileName = QFileDialog::getSaveFileName(this, tr("Export current section to image"),
            latest_export_path + "/" + latest_export, tr("PNG Image (*.png)"));
        if (fileName.isEmpty())
            return;

        QFileInfo exported(fileName);
        latest_export = exported.fileName();
        latest_export_path = exported.absoluteDir().path();
        proportion = imgSize[2];

        th=imgSize[0];
        tw=imgSize[1];

        QImage img(tw,th,QImage::Format_ARGB32_Premultiplied);

        QPainter p(&img);
        scene->render(&p, QRectF(0,0,tw,th),QRectF(x,y,w,h));
        p.end();

        QApplication::setOverrideCursor(Qt::WaitCursor);
        img.save(fileName);
        QApplication::restoreOverrideCursor();

        settings.beginGroup("Main");
        settings.setValue("export-file", latest_export);
        settings.setValue("export-path", latest_export_path);
        settings.setValue("export-proportions", proportion);
        settings.endGroup();
}

void leveledit::ResetPosition()
{
    LvlData.sections[LvlData.CurSection].PositionX =
            LvlData.sections[LvlData.CurSection].size_left;
    LvlData.sections[LvlData.CurSection].PositionY =
            LvlData.sections[LvlData.CurSection].size_bottom-602;

    ui->graphicsView->verticalScrollBar()->setValue(LvlData.sections[LvlData.CurSection].size_bottom-602);
    ui->graphicsView->horizontalScrollBar()->setValue(LvlData.sections[LvlData.CurSection].size_left);
}

void leveledit::setCurrentSection(int scId)
{
    WriteToLog(QtDebugMsg, QString("Save current position %1 %2")
               .arg(ui->graphicsView->horizontalScrollBar()->value())
               .arg(ui->graphicsView->verticalScrollBar()->value())
               );

    //Save currentPosition on Section
    LvlData.sections[LvlData.CurSection].PositionX =
            ui->graphicsView->horizontalScrollBar()->value();
    LvlData.sections[LvlData.CurSection].PositionY =
            ui->graphicsView->verticalScrollBar()->value();

    //Change Current Section
    LvlData.CurSection = scId;

    WriteToLog(QtDebugMsg, QString("Move to current section position"));
    //Move to new section position
    ui->graphicsView->verticalScrollBar()->setValue(LvlData.sections[LvlData.CurSection].PositionY);
    ui->graphicsView->horizontalScrollBar()->setValue(LvlData.sections[LvlData.CurSection].PositionX);

    WriteToLog(QtDebugMsg, QString("Call to Draw intersection space"));
    scene->drawSpace(LvlData);
}



bool leveledit::loadFile(const QString &fileName, LevelData FileData, dataconfigs &configs, LevelEditingSettings options)
{
    QFile file(fileName);
    LvlData = FileData;
    LvlData.modified = false;

    if (!file.open(QFile::ReadOnly | QFile::Text)) {
        QMessageBox::warning(this, tr("Read file error"),
                             tr("Cannot read file %1:\n%2.")
                             .arg(fileName)
                             .arg(file.errorString()));
        return false;
    }
    StartLvlData = LvlData; //Save current history for made reset

    //Data configs exists
    if(
            (configs.main_bgo.size()<=0)||
            (configs.main_bg.size()<=0)||
            (configs.main_block.size()<=0)||
            (configs.main_music_lvl.size()<=0)||
            (configs.main_music_wld.size()<=0)||
            (configs.main_music_spc.size()<=0)
      )
    {
        WriteToLog(QtCriticalMsg, QString("Error! *.INI Configs not loaded"));
        QMessageBox::warning(this, tr("Configurations not loaded"),
                             tr("Cannot open level file %1:\nbecause object configurations not loaded\n."
                                "Please, check that the config/SMBX directory exists and contains the *.INI files with object settings.")
                             .arg(fileName));
        LvlData.modified = false;
        return false;
    }

    WriteToLog(QtDebugMsg, QString(">>Starting load file"));

    //Declaring of the scene
    scene = new LvlScene(configs, LvlData);

    scene->opts = options;

    int DataSize=0;

    DataSize += LvlData.sections.size()*2;
    DataSize += configs.main_bgo.size();
    DataSize += LvlData.bgo.size();
    DataSize += configs.main_block.size();
    DataSize += LvlData.blocks.size();
    DataSize += LvlData.npc.size();
    DataSize += LvlData.water.size();
    DataSize += LvlData.doors.size();

    QProgressDialog progress("Loading level data", "Abort", 0, DataSize, this);
         progress.setWindowTitle("Loading level data");
         progress.setWindowModality(Qt::WindowModal);
         progress.setWindowFlags(Qt::Window | Qt::WindowTitleHint | Qt::CustomizeWindowHint | Qt::WindowStaysOnTopHint);
         progress.setFixedSize(progress.size());
         progress.setGeometry(QStyle::alignedRect(Qt::LeftToRight, Qt::AlignCenter, progress.size(), qApp->desktop()->availableGeometry()));
         //progress.setCancelButton(0);

    if(! DrawObjects(progress) )
    {
        LvlData.modified = false;
        this->close();
        return false;
    }

    QApplication::setOverrideCursor(Qt::WaitCursor);

    if( !progress.wasCanceled() )
        progress.close();

    ui->graphicsView->verticalScrollBar()->setValue(LvlData.sections[0].size_bottom-602);
    ui->graphicsView->horizontalScrollBar()->setValue(LvlData.sections[0].size_left);

    ResetPosition();

    QApplication::restoreOverrideCursor();

    setCurrentFile(fileName);
    LvlData.modified = false;

    return true;
}

void leveledit::changeCursor(int mode)
{
    switch(mode)
    {
    case (-1): // Hand Dragger
        ui->graphicsView->setCursor(Qt::ArrowCursor);
        ui->graphicsView->setInteractive(false);
        ui->graphicsView->setDragMode(QGraphicsView::ScrollHandDrag);
        break;
    case 0:    // Selector
        ui->graphicsView->setInteractive(true);
        ui->graphicsView->setCursor(Qt::ArrowCursor);
        ui->graphicsView->setDragMode(QGraphicsView::RubberBandDrag);
        break;
    case 1:    // Eriser
        ui->graphicsView->setInteractive(true);
        ui->graphicsView->setCursor(QCursor(QPixmap(":/cur_rubber.png"), 0, 0));
        ui->graphicsView->setDragMode(QGraphicsView::RubberBandDrag);
        break;
    case 2:    // place New item
        ui->graphicsView->setInteractive(false);
        ui->graphicsView->setCursor(Qt::CrossCursor);
        ui->graphicsView->setDragMode(QGraphicsView::RubberBandDrag);
        break;
    case 3:    // Draw water zones
        ui->graphicsView->setInteractive(false);
        ui->graphicsView->setCursor(Qt::CrossCursor);
        ui->graphicsView->setDragMode(QGraphicsView::RubberBandDrag);
        break;
    case 4:    // paste from Buffer
        scene->clearSelection();
        ui->graphicsView->setInteractive(true);
        ui->graphicsView->setCursor(QCursor(QPixmap(":/cur_pasta.png"), 0, 0));
        ui->graphicsView->setDragMode(QGraphicsView::NoDrag);
        break;
    default:
        break;
    }
}

bool leveledit::DrawObjects(QProgressDialog &progress)
{
    int DataSize = progress.maximum();
    int TotalSteps = 6;

    if(!progress.wasCanceled())
        progress.setLabelText(tr("1/%1 Loading user data").arg(TotalSteps));

    scene->loadUserData(LvlData, progress);

    if(progress.wasCanceled()) return false;

    if(!progress.wasCanceled())
        progress.setLabelText(tr("1/%1 Applying Backgrounds").arg(TotalSteps));
    scene->makeSectionBG(LvlData, progress);

    if(progress.wasCanceled()) return false;

    if(!progress.wasCanceled())
        progress.setLabelText(tr("2/%1 Applying BGOs...").arg(TotalSteps));
    scene->setBGO(LvlData, progress);

    if(progress.wasCanceled()) return false;

    if(!progress.wasCanceled())
        progress.setLabelText(tr("3/%1 Applying Blocks...").arg(TotalSteps));
    scene->setBlocks(LvlData, progress);

    if(progress.wasCanceled()) return false;

    if(!progress.wasCanceled())
        progress.setLabelText(tr("4/%1 Applying NPCs...").arg(TotalSteps));
    scene->setNPC(LvlData, progress);

    if(progress.wasCanceled()) return false;

    if(!progress.wasCanceled())
        progress.setLabelText(tr("5/%1 Applying Waters...").arg(TotalSteps));
    scene->setWaters(LvlData, progress);

    if(progress.wasCanceled()) return false;

    if(!progress.wasCanceled())
        progress.setLabelText(tr("6/%1 Applying Doors...").arg(TotalSteps));
    scene->setDoors(LvlData, progress);

    if(progress.wasCanceled()) return false;

    scene->setPlayerPoints();

    scene->drawSpace(LvlData);


    if(scene->opts.animationEnabled)
        scene->startBlockAnimation();//Apply block animation

    /*
    scene->setSceneRect(LvlData.sections[0].size_left-1000,
                        LvlData.sections[0].size_top-1000,
                        LvlData.sections[0].size_right+1000,
                        LvlData.sections[0].size_bottom+1000);
    */

    if(!sceneCreared)
    {
    ui->graphicsView->setScene(scene);
    sceneCreared = true;
    }

    if(!progress.wasCanceled())
        progress.setValue(DataSize);
    return true;
}

bool leveledit::save()
{
    if (isUntitled) {
        return saveAs();
    } else {
        return saveFile(curFile);
    }
}

bool leveledit::saveAs()
{
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save As"),
        curFile, tr("SMBX64 (1.3) Level file (*.lvl)"));
    if (fileName.isEmpty())
        return false;

    return saveFile(fileName);
}

bool leveledit::saveFile(const QString &fileName)
{
    //Write disabled for safe
    /*
    QFile file(fileName);
    if (!file.open(QFile::WriteOnly | QFile::Text)) {
        QMessageBox::warning(this, tr("MDI"),
                             tr("Cannot write file %1:\n%2.")
                             .arg(fileName)
                             .arg(file.errorString()));
        return false;
    }*/


    /*
    QTextStream out(&file);
    QApplication::setOverrideCursor(Qt::WaitCursor);
    //out << toPlainText();
    QApplication::restoreOverrideCursor();

    setCurrentFile(fileName);
    */

    QMessageBox::information(this, tr("Dummy"),
                         tr("File %1 will not be saved, saving levels is not implemented in this version.")
                         .arg(fileName));

    return true;
}



QString leveledit::userFriendlyCurrentFile()
{
    return strippedName(curFile);
}

void leveledit::closeEvent(QCloseEvent *event)
{
    if (maybeSave()) {
        scene->uBGOs.clear();
        scene->uBGs.clear();
        scene->uBlocks.clear();
        scene->clear();
        //ui->graphicsView->cl
        event->accept();
    } else {
        event->ignore();
    }
}

/*
void leveledit::focusInEvent( QFocusEvent * focusInEvent)
{

}
*/

void leveledit::documentWasModified()
{
    LvlData.modified = true;
}

bool leveledit::maybeSave()
{
    if (LvlData.modified) {
    QMessageBox::StandardButton ret;
        ret = QMessageBox::warning(this, userFriendlyCurrentFile()+tr(" not saved"),
                     tr("'%1' has been modified.\n"
                        "Do you want to save your changes?")
                     .arg(userFriendlyCurrentFile()),
                     QMessageBox::Save | QMessageBox::Discard
             | QMessageBox::Cancel);
        if (ret == QMessageBox::Save)
            return save();
        else if (ret == QMessageBox::Cancel)
            return false;
    }

    return true;
}

void leveledit::setCurrentFile(const QString &fileName)
{
    curFile = QFileInfo(fileName).canonicalFilePath();
    isUntitled = false;
    //document()->setModified(false);
    setWindowModified(false);
    setWindowTitle(userFriendlyCurrentFile() + "[*]");
}

QString leveledit::strippedName(const QString &fullFileName)
{
    return QFileInfo(fullFileName).fileName();
}
