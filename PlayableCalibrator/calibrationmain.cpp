/*
 * SMBX64 Playble Character Sprite Calibrator, a free tool for playable srite design
 * This is a part of the Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2017-2020 Vitaly Novichkov <admin@wohlnet.ru>
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

#include <QPixmap>
//#include <QtDebug>
#include <QSettings>
#include <QMenu>

#include "calibrationmain.h"
#include <ui_calibrationmain.h>

#include "main/app_path.h"

#include "about/about.h"
#include "frame_matrix/matrix.h"
#include "animator/animate.h"
#include "image_calibration/image_calibrator.h"

#include "main/graphics.h"
#include "main/app_path.h"
#include "main/calibration.h"

#include "main/mw.h"

CalibrationMain::CalibrationMain(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CalibrationMain)
{
    MW::p = this;
    m_clipboard.W = 100;
    m_clipboard.H = 100;
    m_clipboard.offsetX = 0;
    m_clipboard.offsetY = 0;
    m_clipboard.used = true;
    m_clipboard.isDuck = true;
    m_clipboard.isRightDir = false;
    m_wasCanceled = false;

    ui->setupUi(this);

    QObject::connect(&m_translator, &Translator::languageSwitched,
                     this, &CalibrationMain::languageSwitched);
    ui->language->setMenu(&m_langMenu);
    m_translator.setSettings(&m_langMenu,
                             "calibrator",
                             AppPathManager::languagesDir(),
                             AppPathManager::settingsFile());
    m_translator.initTranslator();

    {
        m_saveMenuQuickSave = m_saveMenu.addAction("");
        m_saveMenuSaveAs = m_saveMenu.addAction("");
        translateSaveMenu();

        ui->SaveConfigButton->setMenu(&m_saveMenu);

        QObject::connect(m_saveMenuQuickSave, static_cast<void(QAction::*)(bool)>(&QAction::triggered), [this](bool)
        {
            saveConfig(m_calibration, m_currentFile, false, &m_calibrationDefault);
            m_wasModified = false;
        });

        QObject::connect(m_saveMenuSaveAs, static_cast<void(QAction::*)(bool)>(&QAction::triggered), [this](bool)
        {
            if(saveConfig(m_calibration, m_currentFile, true, &m_calibrationDefault))
                m_wasModified = false;
        });
    }

    Graphics::init();

    QString inifile = AppPathManager::settingsFile();
    QSettings settings(inifile, QSettings::IniFormat);
    settings.beginGroup("Main");
    m_lastOpenDir = settings.value("lastpath", ".").toString();
    m_currentFile = settings.value("lastfile", m_currentFile).toString();
    m_frmX = settings.value("last-frame-x", ui->FrameX->value()).toInt();
    m_frmY = settings.value("last-frame-y", ui->FrameY->value()).toInt();
    m_lockControls = true;
    ui->FrameX->setValue(m_frmX);
    ui->FrameY->setValue(m_frmY);
    m_lockControls = false;
    settings.endGroup();

//    QVector<frameOpts > framesY;
//    frameOpts spriteData;

//    // Write default values
//    for(int i = 0; i < 10; i++)
//    {
//        framesY.clear();
//        for(int j = 0; j < 10; j++)
//        {
//            spriteData = g_buffer;
//            framesY.push_back(spriteData);
//        }
//        g_framesX.push_back(framesY);
//    }


    m_scene = new QGraphicsScene(ui->PreviewGraph);

    ui->PreviewGraph->setScene(m_scene);
    QGraphicsScene *sc = ui->PreviewGraph->scene();

    m_currentPixmap = QPixmap(100, 100);
    m_currentPixmap.fill(Qt::transparent);
    m_currentImageItem.setPixmap(m_currentPixmap);
    m_currentImageItem.setZValue(0);

    m_frameBox_gray.setRect(0, 0, 100, 100);
    m_frameBox_gray.setPen(QPen(Qt::gray, 1));
    m_frameBox_gray.setBrush(Qt::transparent);
    m_frameBox_gray.setZValue(-10);

    m_hitBox_green.setRect(0, 0, m_calibration.frameWidth - 1, m_calibration.frameHeight - 1);
    m_hitBox_green.setPen(QPen(Qt::green));
    m_hitBox_green.setZValue(3);

    m_grabLineX.setPen(QPen(Qt::red));
    m_grabLineY.setPen(QPen(Qt::red));

    m_grabLineX.setZValue(10);
    m_grabLineY.setZValue(10);

    sc->addItem(&m_frameBox_gray);
    sc->addItem(&m_currentImageItem);
    sc->addItem(&m_hitBox_green);
    sc->addItem(&m_grabLineX);
    sc->addItem(&m_grabLineY);

    if(m_currentFile.isEmpty())
    {
        if(!on_OpenSprite_clicked())
        {
            m_wasCanceled = true;
            return;
        }
    }
    else
        OpenFile(m_currentFile);
}

CalibrationMain::~CalibrationMain()
{
    delete ui;
    Graphics::quit();
}

void CalibrationMain::closeEvent(QCloseEvent *event)
{
    QString config = AppPathManager::settingsFile();
    QSettings opts(config, QSettings::IniFormat);
    opts.beginGroup("Main");
    {
        opts.setValue("lastpath", m_lastOpenDir);
        opts.setValue("lastfile", m_currentFile);
        opts.setValue("last-frame-x", m_frmX);
        opts.setValue("last-frame-y", m_frmY);
    }
    opts.endGroup();

    if(!trySave())
    {
        event->ignore();
        return;
    }

    event->accept();
}

void CalibrationMain::languageSwitched()
{
    ui->retranslateUi(this);
    translateSaveMenu();
}

void CalibrationMain::translateSaveMenu()
{
    if(m_saveMenuQuickSave)
        m_saveMenuQuickSave->setText(tr("Save in the same folder with image file"));
    if(m_saveMenuSaveAs)
        m_saveMenuSaveAs->setText(tr("Save into custom place..."));
}


void CalibrationMain::on_FrameX_valueChanged(int)
{
    if(m_lockControls) return;
    initScene();
    updateControls();
    updateScene();
}

void CalibrationMain::on_FrameY_valueChanged(int)
{
    if(m_lockControls) return;
    initScene();
    updateControls();
    updateScene();
}



void CalibrationMain::on_Height_valueChanged(int arg1)
{
    if(m_lockControls) return;
    m_calibration.frames[{m_frmX, m_frmY}].H = static_cast<unsigned>(arg1);
    m_calibration.frameHeight = arg1;
    enableFrame();
    updateScene();
}


void CalibrationMain::on_Height_duck_valueChanged(int arg1)
{
    if(m_lockControls) return;
    m_calibration.frameHeightDuck = arg1;
    enableFrame();
    updateScene();
}


void CalibrationMain::on_Width_valueChanged(int arg1)
{
    if(m_lockControls) return;
    m_calibration.frames[{m_frmX, m_frmY}].W = static_cast<unsigned>(arg1);
    m_calibration.frameWidth = arg1;
    enableFrame();
    updateScene();
}


void CalibrationMain::on_OffsetX_valueChanged(int arg1)
{
    if(m_lockControls) return;
    m_calibration.frames[{m_frmX, m_frmY}].offsetX = arg1;
    enableFrame();
    updateScene();
}


void CalibrationMain::on_OffsetY_valueChanged(int arg1)
{
    if(m_lockControls) return;
    m_calibration.frames[{m_frmX, m_frmY}].offsetY = arg1;
    enableFrame();
    updateScene();
}


void CalibrationMain::on_CopyButton_clicked()
{
    m_clipboard = m_calibration.frames[{m_frmX, m_frmY}];
}


void CalibrationMain::on_PasteButton_clicked()
{
    m_calibration.frames[{m_frmX, m_frmY}] = m_clipboard;

    updateControls();
    updateScene();
    m_wasModified = true;
}

void CalibrationMain::on_isDuckFrame_clicked(bool checked)
{
    m_calibration.frames[{m_frmX, m_frmY}].isDuck = checked;
    enableFrame();
    updateScene();
}



bool CalibrationMain::on_OpenSprite_clicked()
{
    if(!trySave())
        return false;

    QString fileName_DATA = QFileDialog::getOpenFileName(this,
                            tr("Open sprite file"), (m_lastOpenDir.isEmpty() ? AppPathManager::userAppDir() : m_lastOpenDir),
                            tr("GIF and PNG images", "Type of image file to open") + " (*.png *.gif);;" +
                            tr("GIF images", "Type of image file to open") + " (*.gif);;" +
                            tr("PNG images", "Type of image file to open") + " (*.png);;" +
                            tr("SMBX playble sprite", "Type of image file to open") + " (mario-*.gif peach-*.gif toad-*.gif luigi-*.gif link-*.gif);;" +
                            tr("All Files", "Type of image file to open") + " (*.*)");

    if(fileName_DATA == nullptr)
        return false;

    OpenFile(fileName_DATA);
    m_wasModified = false;
    return true;
}


void CalibrationMain::on_AboutButton_clicked()
{
    about dialog(this);
    dialog.setWindowFlags(Qt::Window | Qt::WindowCloseButtonHint);
    dialog.exec();
}



// Copy current sizes and offsets to ALL frames
void CalibrationMain::on_applyToAll_clicked()
{
    int reply = QMessageBox::question(this,
                                      tr("Warning"),
                                      tr("This action will copy settings of current hitbox to all other frames. "
                                         "Settings of all other frames will be overriden with settings of a current frame. "
                                         "Do you want to continue?"),
                                      QMessageBox::Yes|QMessageBox::No);
    if(reply != QMessageBox::Yes)
        return;

    m_clipboard = m_calibration.frames[{m_frmX, m_frmY}];

    for(int x = 0; x < 10; x++)
        for(int y = 0; y < 10; y++)
            m_calibration.frames[{x, y}] = m_clipboard;

    m_wasModified = true;
}

bool CalibrationMain::trySave()
{
    if(m_wasModified)
    {
        int ret = QMessageBox::question(this,
                        tr("Calibration is not saved!"),
                        tr("Configuration was modified and not saved. Do you want to save it?"),
                        QMessageBox::Yes|QMessageBox::No|QMessageBox::Cancel);
        if(ret == QMessageBox::Cancel)
            return false;
        else if(ret == QMessageBox::Yes)
        {
            if(!saveConfig(m_calibration, m_currentFile, true, &m_calibrationDefault))
                return false;
        }
    }
    return true;
}

//Set using this frame on template
void CalibrationMain::on_EnableFrame_clicked(bool checked)
{
    if(ui->EnableFrame->hasFocus())
        m_calibration.frames[{m_frmX, m_frmY}].used = checked;
    m_wasModified = true;
}

void CalibrationMain::on_isRightDirect_clicked(bool checked)
{
    if(m_lockControls) return;
    m_calibration.frames[{m_frmX, m_frmY}].isRightDir = checked;
    enableFrame();
    updateScene();
}

void CalibrationMain::on_showGrabItem_clicked(bool checked)
{
    if(m_lockControls) return;
    m_calibration.frames[{m_frmX, m_frmY}].showGrabItem = checked;
    enableFrame();
    updateScene();
}



void CalibrationMain::on_Matrix_clicked()
{
    Matrix dialog(&m_calibration, this);
    this->hide();
    dialog.setWindowFlags(Qt::Window | Qt::WindowCloseButtonHint);
    dialog.setFrame(m_frmX, m_frmY);
    dialog.show();
    dialog.raise();
    if(dialog.exec() == QDialog::Accepted)
    {
        ui->FrameX->setValue(dialog.frameX);
        ui->FrameY->setValue(dialog.frameY);
    }
    this->show();
    this->raise();

    m_calibration.frames = dialog.m_frameConfig;
    initScene();
    updateControls();
    updateScene();
}


void CalibrationMain::on_AnimatorButton_clicked()
{
    this->hide();
    Animate dialog(m_calibration, this);
    dialog.setWindowFlags(Qt::Window | Qt::WindowCloseButtonHint);
    dialog.exec();
    this->show();
    this->raise();
}


void CalibrationMain::on_calibrateImage_clicked()
{
    ImageCalibrator imgCalibrator(&m_calibration, this);
    imgCalibrator.setWindowFlags(Qt::Window | Qt::WindowCloseButtonHint);
    if(!imgCalibrator.init(m_currentFile)) return;

    imgCalibrator.m_scene = m_scene;

    this->hide();
    imgCalibrator.exec();
    this->show();
    this->raise();
    OpenFile(imgCalibrator.m_targetPath);
}



void CalibrationMain::on_grabOffsetX_valueChanged(int arg1)
{
    if(ui->grabOffsetX->hasFocus())
    {
        m_calibration.frameGrabOffsetX = arg1;
        enableFrame();
        updateScene();
    }
}

void CalibrationMain::on_grabOffsetY_valueChanged(int arg1)
{
    if(ui->grabOffsetY->hasFocus())
    {
        m_calibration.frameGrabOffsetY = arg1;
        enableFrame();
        updateScene();
    }
}

void CalibrationMain::enableFrame()
{
    m_wasModified = true;
    m_calibration.frames[{m_frmX, m_frmY}].used = true;
    if(!ui->EnableFrame->isChecked())
        ui->EnableFrame->setChecked(true);
}

void CalibrationMain::updateControls()
{
    m_frmX = ui->FrameX->value();
    m_frmY = ui->FrameY->value();
    m_lockControls = true;

    auto &frame = m_calibration.frames[{m_frmX, m_frmY}];

    ui->Height->setValue(m_calibration.frameHeight);
    ui->Height_duck->setValue(m_calibration.frameHeightDuck);
    ui->Width->setValue(m_calibration.frameWidth);
    ui->OffsetX->setValue(frame.offsetX);
    ui->OffsetY->setValue(frame.offsetY);

    ui->grabOffsetX->setValue(m_calibration.frameGrabOffsetX);
    ui->grabOffsetY->setValue(m_calibration.frameGrabOffsetY);

    if(m_calibration.frameOverTopGrab)
        ui->grabTop->setChecked(true);
    else
        ui->grabSide->setChecked(true);

    ui->EnableFrame->setChecked(frame.used);
    ui->isDuckFrame->setChecked(frame.isDuck);
    ui->isRightDirect->setChecked(frame.isRightDir);
    ui->showGrabItem->setChecked(frame.showGrabItem);
    m_lockControls = false;
}


void CalibrationMain::updateScene()
{
    int x, y, h, w, relX, relY;
    bool isRight;
    auto &frame = m_calibration.frames[{m_frmX, m_frmY}];
    x = frame.offsetX;
    y = frame.offsetY;
    h = frame.isDuck ? m_calibration.frameHeightDuck : m_calibration.frameHeight;
    w = m_calibration.frameWidth;
    isRight = frame.isRightDir;

    relX = m_frameBox_gray.scenePos().x();
    relY = m_frameBox_gray.scenePos().y();

    m_hitBox_green.setPos(relX + x,
                              relY + y);
    m_hitBox_green.setRect(0.0, 0.0, w, h);

    relX = m_hitBox_green.scenePos().x();
    relY = m_hitBox_green.scenePos().y();

    if(m_calibration.frameOverTopGrab)
    {
        m_grabLineX.setLine(relX,
                          relY - 1,
                          relX + w, relY - 1);

        m_grabLineY.setLine(relX + w / 2,
                          relY - 1,

                          relX + w / 2,
                          relY - 21);

        m_grabLineX.setVisible(frame.showGrabItem);
        m_grabLineY.setVisible(frame.showGrabItem);

    }
    else
    {
        m_grabLineX.setLine(relX + (isRight ? 0 : w) + m_calibration.frameGrabOffsetX * (isRight ? 1 : -1),
                          relY + h / 2 + m_calibration.frameGrabOffsetY,
                          relX + (isRight ? 0 : w) + m_calibration.frameGrabOffsetX * (isRight ? 1 : -1) + (isRight ? 20 : -20),
                          relY + h / 2 + m_calibration.frameGrabOffsetY);

        m_grabLineY.setLine(relX + (isRight ? 0 : w) + m_calibration.frameGrabOffsetX * (isRight ? 1 : -1),
                          relY + h / 2 + m_calibration.frameGrabOffsetY,

                          relX + (isRight ? 0 : w) + m_calibration.frameGrabOffsetX * (isRight ? 1 : -1),
                          relY + h / 2 + m_calibration.frameGrabOffsetY - 20);

        m_grabLineX.setVisible(frame.showGrabItem);
        m_grabLineY.setVisible(frame.showGrabItem);
    }

}

void CalibrationMain::initScene()
{
    m_frmX = ui->FrameX->value();
    m_frmY = ui->FrameY->value();

    m_FramePos.setX(m_frmX * 100);
    m_FramePos.setY(m_frmY * 100);

    QRectF rect = ui->PreviewGraph->scene()->sceneRect();

    m_frameBox_gray.setPos(rect.width() / 2 - 50, rect.height() / 2 - 50);
    m_currentPixmap = m_xImageSprite.copy(QRect(m_FramePos.x(), m_FramePos.y(), 100, 100));
    m_currentImageItem.setPixmap(QPixmap(m_currentPixmap));
    m_currentImageItem.setPos(m_frameBox_gray.scenePos().x(), m_frameBox_gray.scenePos().y());
}

void CalibrationMain::on_grabTop_clicked()
{
    m_calibration.frameOverTopGrab = true;
    enableFrame();
    updateScene();
}

void CalibrationMain::on_grabSide_clicked()
{
    m_calibration.frameOverTopGrab = false;
    enableFrame();
    updateScene();
}
