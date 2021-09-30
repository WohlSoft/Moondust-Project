/*
 * Playble Character Calibrator, a free tool for playable character sprite setup tune
 * This is a part of the Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2017-2021 Vitaly Novichkov <admin@wohlnet.ru>
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

#include "calibration_main.h"
#include <ui_calibration_main.h>

#include "main/app_path.h"

#include "about/about.h"
#include "frame_matrix/matrix.h"
#include "animator/animator.h"
#include "image_calibration/image_calibrator.h"

#include "main/graphics.h"
#include "main/app_path.h"
#include "main/calibration.h"
#include "main/mouse_scene.h"

#include "qfile_dialogs_default_options.hpp"



CalibrationMain::CalibrationMain(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CalibrationMain)
{
    m_clipboard.w = 100;
    m_clipboard.h = 100;
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

    {
        m_toolsExportHitboxMap = m_toolsMenu.addAction("");
        m_toolsMenu.addSeparator();
        m_toolsImport38A = m_toolsMenu.addAction("");
        m_toolsExport38A = m_toolsMenu.addAction("");

        ui->toolsButton->setMenu(&m_toolsMenu);

        QObject::connect(m_toolsExportHitboxMap, static_cast<void(QAction::*)(bool)>(&QAction::triggered), [this](bool)
        {
            exportHitboxesMap();
        });

        QObject::connect(m_toolsImport38A, static_cast<void(QAction::*)(bool)>(&QAction::triggered), [this](bool)
        {
            if(!trySave())
                return;

            QString fileName_DATA = QFileDialog::getOpenFileName(this,
                                    tr("Open SMBX-38A level file"),
                                    (m_lastOpenDir.isEmpty() ? AppPathManager::userAppDir() : m_lastOpenDir),
                                    tr("SMBX-38A level files", "Type of file to open") + " (*.lvl);;" +
                                    tr("All Files", "Type of file to open") + " (*.*)", nullptr, c_fileDialogOptions);

            if(fileName_DATA == nullptr)
                return;

            QFileInfo s(m_currentFile);

            if(!importFrom38A(m_calibration, s.fileName(), fileName_DATA))
            {
                QMessageBox::warning(this,
                                     tr("File opening error"),
                                     tr("Can't import calibration data from this file: the file doesn't "
                                        "contain calibration settings, or this file is not a valid SMBX-38A level file."),
                                     QMessageBox::Ok);
                return;
            }

            initScene();
            updateControls();
            updateScene();

            m_wasModified = false;
        });

        QObject::connect(m_toolsExport38A, static_cast<void(QAction::*)(bool)>(&QAction::triggered), [this](bool)
        {
            QString san_ba_level = QFileDialog::getOpenFileName(this,
                                   tr("Export calibration settings into SMBX-38A level file"),
                                   (m_lastOpenDir.isEmpty() ? AppPathManager::userAppDir() : m_lastOpenDir),
                                   "*.lvl", nullptr, c_fileDialogOptions);
            if(san_ba_level.isEmpty())
                return;

            QFileInfo s(m_currentFile);

            if(!exportTo38A(m_calibration, s.fileName(), san_ba_level))
            {
                QMessageBox::warning(this,
                                     tr("File saving error"),
                                     tr("Can't export calibration data into this file: this is not a valid SMBX-38A level file."),
                                     QMessageBox::Ok);
                return;
            }

            QMessageBox::information(this,
                                     tr("Saved"),
                                     tr("The level file has been patched!") + "\n" + san_ba_level);
        });
    }

    translateMenus();

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


    m_scene = new MouseScene(ui->PreviewGraph);
    QObject::connect(m_scene, &MouseScene::deltaX,
                     this, [this](Qt::MouseButton button, int delta)->void
    {
        switch(button)
        {
        case Qt::LeftButton:
            ui->OffsetX->setValue(ui->OffsetX->value() + delta);
            break;
        case Qt::RightButton:
            if(!ui->showGrabItem->isChecked())
                break;
            if(!ui->isRightDirect->isChecked())
                delta *= -1;
            ui->grabOffsetX->setValue(ui->grabOffsetX->value() + delta);
            m_calibration.grabOffsetX = ui->grabOffsetX->value();
            enableFrame();
            updateScene();
            break;
        default:
            break;
        }
    });

    QObject::connect(m_scene, &MouseScene::deltaY,
                     this, [this](Qt::MouseButton button, int delta)->void
    {
        switch(button)
        {
        case Qt::LeftButton:
            ui->OffsetY->setValue(ui->OffsetY->value() + delta);
            break;
        case Qt::RightButton:
            if(!ui->showGrabItem->isChecked())
                break;
            ui->grabOffsetY->setValue(ui->grabOffsetY->value() + delta);
            m_calibration.grabOffsetY = ui->grabOffsetY->value();
            enableFrame();
            updateScene();
            break;
        default:
            break;
        }
    });

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

    m_mountPixmap = QPixmap(":/images/mount.png");
    m_mountDuckPixmap = QPixmap(":/images/mount_duck.png");
    m_mountItem.setPixmap(m_mountPixmap);
    m_mountItem.setZValue(-9);
    m_mountItem.setVisible(false);

    sc->addItem(&m_frameBox_gray);
    sc->addItem(&m_currentImageItem);
    sc->addItem(&m_hitBox_green);
    sc->addItem(&m_grabLineX);
    sc->addItem(&m_grabLineY);
    sc->addItem(&m_mountItem);

    m_matrix = new Matrix(&m_calibration, this, this);
    m_matrix->setModal(false);
    m_matrix->changeGridSize(m_calibration.matrixWidth, m_calibration.matrixHeight);

    QObject::connect(m_matrix, &Matrix::frameSelected,
                     this, &CalibrationMain::frameSelected);
    QObject::connect(m_matrix, &Matrix::currentFrameSwitched,
                     ui->EnableFrame, &QCheckBox::setChecked);

    if(m_currentFile.isEmpty())
    {
        if(!on_OpenSprite_clicked())
        {
            m_wasCanceled = true;
            return;
        }
    }
    else
        openFile(m_currentFile);
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
    translateMenus();
}

void CalibrationMain::frameSelected(int x, int y)
{
    m_lockControls = true;
    ui->FrameX->setValue(x);
    ui->FrameY->setValue(y);
    m_lockControls = false;
    initScene();
    updateControls();
    updateScene();
}

void CalibrationMain::translateMenus()
{
    if(m_saveMenuQuickSave)
        m_saveMenuQuickSave->setText(tr("Save in the same folder with image file"));
    if(m_saveMenuSaveAs)
        m_saveMenuSaveAs->setText(tr("Save into custom place..."));


    if(m_toolsExportHitboxMap)
    {
        m_toolsExportHitboxMap->setText(tr("Export a map of hitboxes as image..."));
        m_toolsExportHitboxMap->setToolTip(tr("Useful for tests or sprites creation from the scratch"));
    }
    if(m_toolsImport38A)
        m_toolsImport38A->setText(tr("Import data from SMBX-38A level..."));

    if(m_toolsExport38A)
        m_toolsExport38A->setText(tr("Export data into SMBX-38A level..."));
}


void CalibrationMain::on_FrameX_valueChanged(int)
{
    if(m_lockControls) return;
    initScene();
    updateControls();
    updateScene();
    m_matrix->setFrame(m_frmX, m_frmY);
}

void CalibrationMain::on_FrameY_valueChanged(int)
{
    if(m_lockControls) return;
    initScene();
    updateControls();
    updateScene();
    m_matrix->setFrame(m_frmX, m_frmY);
}



void CalibrationMain::on_Height_valueChanged(int arg1)
{
    if(m_lockControls) return;
    m_calibration.frames[ {m_frmX, m_frmY}].h = static_cast<unsigned>(arg1);
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
    m_calibration.frames[ {m_frmX, m_frmY}].w = static_cast<unsigned>(arg1);
    m_calibration.frameWidth = arg1;
    enableFrame();
    updateScene();
}


void CalibrationMain::on_OffsetX_valueChanged(int arg1)
{
    if(m_lockControls) return;
    m_calibration.frames[ {m_frmX, m_frmY}].offsetX = arg1;
    enableFrame();
    updateScene();
}


void CalibrationMain::on_OffsetY_valueChanged(int arg1)
{
    if(m_lockControls) return;
    m_calibration.frames[ {m_frmX, m_frmY}].offsetY = arg1;
    enableFrame();
    updateScene();
}


void CalibrationMain::on_CopyButton_clicked()
{
    m_clipboard = m_calibration.frames[ {m_frmX, m_frmY}];
}


void CalibrationMain::on_PasteButton_clicked()
{
    m_calibration.frames[ {m_frmX, m_frmY}] = m_clipboard;

    updateControls();
    updateScene();
    m_wasModified = true;
}

void CalibrationMain::on_isDuckFrame_clicked(bool checked)
{
    m_calibration.frames[ {m_frmX, m_frmY}].isDuck = checked;
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
                            tr("All Files", "Type of image file to open") + " (*.*)", nullptr, c_fileDialogOptions);

    if(fileName_DATA == nullptr)
        return false;

    openFile(fileName_DATA);
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
                                      QMessageBox::Yes | QMessageBox::No);
    if(reply != QMessageBox::Yes)
        return;

    m_clipboard = m_calibration.frames[ {m_frmX, m_frmY}];

    for(int x = 0; x < 10; x++)
        for(int y = 0; y < 10; y++)
            m_calibration.frames[ {x, y}] = m_clipboard;

    m_wasModified = true;
}

bool CalibrationMain::trySave()
{
    if(m_wasModified)
    {
        int ret = QMessageBox::question(this,
                                        tr("Calibration is not saved!"),
                                        tr("Configuration was modified and not saved. Do you want to save it?"),
                                        QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);
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
    {
        m_calibration.frames[ {m_frmX, m_frmY}].used = checked;
        m_matrix->setFrameEnabled(m_frmX, m_frmY, checked);
    }
    m_wasModified = true;
}

void CalibrationMain::on_isRightDirect_clicked(bool checked)
{
    if(m_lockControls) return;
    m_calibration.frames[ {m_frmX, m_frmY}].isRightDir = checked;
    enableFrame();
    updateScene();
}

void CalibrationMain::on_showGrabItem_clicked(bool checked)
{
    if(m_lockControls) return;
    m_calibration.frames[ {m_frmX, m_frmY}].showGrabItem = checked;
    enableFrame();
    updateScene();
}

void CalibrationMain::on_mountRiding_clicked(bool checked)
{
    if(m_lockControls) return;
    m_calibration.frames[ {m_frmX, m_frmY}].isMountRiding = checked;
    enableFrame();
    updateScene();
}



void CalibrationMain::on_Matrix_clicked()
{
    m_matrix->show();
    QRect g = this->frameGeometry();
    m_matrix->move(g.right(), g.top());
    m_matrix->update();
    m_matrix->repaint();
}


void CalibrationMain::on_AnimatorButton_clicked()
{
    m_matrix->hide();
    this->hide();
    auto old = m_calibration.animations;
    Animator dialog(m_calibration, this);
    dialog.setWindowFlags(Qt::Window | Qt::WindowCloseButtonHint);
    dialog.exec();
    if(old != m_calibration.animations)
        m_wasModified = true;
    this->show();
    this->raise();
}


void CalibrationMain::on_calibrateImage_clicked()
{
    m_matrix->hide();

    ImageCalibrator imgCalibrator(&m_calibration, this);
    imgCalibrator.setWindowFlags(Qt::Window | Qt::WindowCloseButtonHint);
    if(!imgCalibrator.init(m_currentFile))
        return;

    imgCalibrator.m_scene = m_scene;

    this->hide();
    imgCalibrator.exec();
    this->show();
    this->raise();
    openFile(imgCalibrator.m_targetPath);
}



void CalibrationMain::on_grabOffsetX_valueChanged(int arg1)
{
    if(ui->grabOffsetX->hasFocus())
    {
        m_calibration.grabOffsetX = arg1;
        enableFrame();
        updateScene();
    }
}

void CalibrationMain::on_grabOffsetY_valueChanged(int arg1)
{
    if(ui->grabOffsetY->hasFocus())
    {
        m_calibration.grabOffsetY = arg1;
        enableFrame();
        updateScene();
    }
}

void CalibrationMain::enableFrame()
{
    m_wasModified = true;
    m_calibration.frames[ {m_frmX, m_frmY}].used = true;
    if(!ui->EnableFrame->isChecked())
    {
        ui->EnableFrame->setChecked(true);
        m_matrix->setFrameEnabled(m_frmX, m_frmY, true);
    }
}

void CalibrationMain::updateControls()
{
    m_frmX = ui->FrameX->value();
    m_frmY = ui->FrameY->value();
    m_lockControls = true;

    auto &frame = m_calibration.frames[ {m_frmX, m_frmY}];

    ui->Height->setValue(m_calibration.frameHeight);
    ui->Height_duck->setValue(m_calibration.frameHeightDuck);
    ui->Width->setValue(m_calibration.frameWidth);
    ui->OffsetX->setValue(frame.offsetX);
    ui->OffsetY->setValue(frame.offsetY);

    ui->grabOffsetX->setValue(m_calibration.grabOffsetX);
    ui->grabOffsetY->setValue(m_calibration.grabOffsetY);

    if(m_calibration.grabOverTop)
        ui->grabTop->setChecked(true);
    else
        ui->grabSide->setChecked(true);

    ui->EnableFrame->setChecked(frame.used);
    ui->isDuckFrame->setChecked(frame.isDuck);
    ui->isRightDirect->setChecked(frame.isRightDir);
    ui->showGrabItem->setChecked(frame.showGrabItem);
    ui->mountRiding->setChecked(frame.isMountRiding);
    m_lockControls = false;
}


void CalibrationMain::updateScene()
{
    int x, y, h, w, relX, relY;
    bool isRight;
    auto &frame = m_calibration.frames[ {m_frmX, m_frmY}];
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

    if(m_calibration.grabOverTop)
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
        m_grabLineX.setLine(relX + (isRight ? 0 : w) + m_calibration.grabOffsetX * (isRight ? 1 : -1),
                            relY + h / 2 + m_calibration.grabOffsetY,
                            relX + (isRight ? 0 : w) + m_calibration.grabOffsetX * (isRight ? 1 : -1) + (isRight ? 20 : -20),
                            relY + h / 2 + m_calibration.grabOffsetY);

        m_grabLineY.setLine(relX + (isRight ? 0 : w) + m_calibration.grabOffsetX * (isRight ? 1 : -1),
                            relY + h / 2 + m_calibration.grabOffsetY,

                            relX + (isRight ? 0 : w) + m_calibration.grabOffsetX * (isRight ? 1 : -1),
                            relY + h / 2 + m_calibration.grabOffsetY - 20);

        m_grabLineX.setVisible(frame.showGrabItem);
        m_grabLineY.setVisible(frame.showGrabItem);
    }

    m_mountItem.setVisible(frame.isMountRiding);

    if(frame.isMountRiding)
    {
        auto &pm = frame.isDuck ? m_mountDuckPixmap : m_mountPixmap;
        int mw = pm.width();
        int mh = pm.height() / 2;

        m_mountItem.setPixmap(pm.copy(0, isRight ? mh : 0, mw, mh));

        x = m_hitBox_green.scenePos().x() + (w / 2) - (mw / 2);
        y = m_hitBox_green.scenePos().y() + h - mh + 2;
        m_mountItem.setPos(x, y);
    }
}

void CalibrationMain::initScene()
{
    m_frmX = ui->FrameX->value();
    m_frmY = ui->FrameY->value();

    m_framePos.setX(m_frmX * 100);
    m_framePos.setY(m_frmY * 100);

    QRectF rect = ui->PreviewGraph->scene()->sceneRect();

    m_frameBox_gray.setPos(rect.width() / 2 - 50, rect.height() / 2 - 50);
    m_currentPixmap = m_xImageSprite.copy(QRect(m_framePos.x(), m_framePos.y(), 100, 100));
    m_currentImageItem.setPixmap(QPixmap(m_currentPixmap));
    m_currentImageItem.setPos(m_frameBox_gray.scenePos().x(), m_frameBox_gray.scenePos().y());
}

void CalibrationMain::on_grabTop_clicked()
{
    m_calibration.grabOverTop = true;
    enableFrame();
    updateScene();
}

void CalibrationMain::on_grabSide_clicked()
{
    m_calibration.grabOverTop = false;
    enableFrame();
    updateScene();
}
