/*
 * Playble Character Calibrator, a free tool for playable character sprite setup tune
 * This is a part of the Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2017-2026 Vitaly Novichkov <admin@wohlnet.ru>
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
#include <QDialog>
#include <QColorDialog>

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
    QMainWindow(parent),
    ui(new Ui::CalibrationMain)
{
    m_wasCanceled = false;

    ui->setupUi(this);

    QObject::connect(&m_translator, &Translator::languageSwitched,
                     this, &CalibrationMain::languageSwitched);

//    ui->menuLanguage(&m_langMenu);
    m_translator.setSettings(ui->menuLanguage,
                             "calibrator",
                             AppPathManager::languagesDir(),
                             AppPathManager::settingsFile());
    m_translator.initTranslator();

    {
        m_wallMenuNone = m_wallMenu.addAction("none");
        m_wallMenuFloor = m_wallMenu.addAction("floor");
        m_wallMenuFloorWallL = m_wallMenu.addAction("floor-wall-l");
        m_wallMenuFloorWallR = m_wallMenu.addAction("floor-wall-r");
        m_wallMenuCeiling = m_wallMenu.addAction("ceiling");
        m_wallMenuCeilingWallL = m_wallMenu.addAction("ceiling-wall-l");
        m_wallMenuCeilingWallR = m_wallMenu.addAction("ceiling-wall-r");

        ui->wallMenu->setMenu(&m_wallMenu);

        QObject::connect(m_wallMenuNone, static_cast<void(QAction::*)(bool)>(&QAction::triggered), [this](bool)
        {
            ui->preview->setWall(FrameTuneScene::WALL_NONE);
        });

        QObject::connect(m_wallMenuFloor, static_cast<void(QAction::*)(bool)>(&QAction::triggered), [this](bool)
        {
            ui->preview->setWall(FrameTuneScene::WALL_FLOOR);
        });

        QObject::connect(m_wallMenuFloorWallL, static_cast<void(QAction::*)(bool)>(&QAction::triggered), [this](bool)
        {
            ui->preview->setWall(FrameTuneScene::WALL_FLOOR_WL);
        });

        QObject::connect(m_wallMenuFloorWallR, static_cast<void(QAction::*)(bool)>(&QAction::triggered), [this](bool)
        {
            ui->preview->setWall(FrameTuneScene::WALL_FLOOR_WR);
        });

        QObject::connect(m_wallMenuCeiling, static_cast<void(QAction::*)(bool)>(&QAction::triggered), [this](bool)
        {
            ui->preview->setWall(FrameTuneScene::WALL_CEILING);
        });

        QObject::connect(m_wallMenuCeilingWallL, static_cast<void(QAction::*)(bool)>(&QAction::triggered), [this](bool)
        {
            ui->preview->setWall(FrameTuneScene::WALL_CEILING_WL);
        });

        QObject::connect(m_wallMenuCeilingWallR, static_cast<void(QAction::*)(bool)>(&QAction::triggered), [this](bool)
        {
            ui->preview->setWall(FrameTuneScene::WALL_CEILING_WR);
        });
    }

    {
        m_compatMenuCalibratorFull = m_compatMenu.addAction("Calibrator");
        m_compatMenuCalibratorFull->setCheckable(true);
        m_compatMenuCalibratorFull->setIcon(QIcon(":/images/Icon16.png"));

        m_compatMenuMoondust = m_compatMenu.addAction("Moondust");
        m_compatMenuMoondust->setCheckable(true);
        m_compatMenuMoondust->setIcon(QIcon(":/compat/moondust.ico"));

        m_compatMenuTheXTech = m_compatMenu.addAction("TheXTech");
        m_compatMenuTheXTech->setCheckable(true);
        m_compatMenuTheXTech->setIcon(QIcon(":/compat/thextech.ico"));

        m_compatMenuSMBX2 = m_compatMenu.addAction("SMBX2");
        m_compatMenuSMBX2->setCheckable(true);
        m_compatMenuSMBX2->setIcon(QIcon(":/compat/smbx2.ico"));

        m_compatMenu38A = m_compatMenu.addAction("SMBX-38A");
        m_compatMenu38A->setCheckable(true);
        m_compatMenu38A->setIcon(QIcon(":/compat/38a.ico"));

        QObject::connect(m_compatMenuCalibratorFull, static_cast<void(QAction::*)(bool)>(&QAction::triggered), [this](bool)
        {
            m_calibration.compatProfile = Calibration::COMPAT_CALIBRATOR_FULL;
            updateCompatMode();
        });

        QObject::connect(m_compatMenuMoondust, static_cast<void(QAction::*)(bool)>(&QAction::triggered), [this](bool)
        {
            m_calibration.compatProfile = Calibration::COMPAT_MOONDUST;
            updateCompatMode();
        });

        QObject::connect(m_compatMenuTheXTech, static_cast<void(QAction::*)(bool)>(&QAction::triggered), [this](bool)
        {
            m_calibration.compatProfile = Calibration::COMPAT_THEXTECH;
            updateCompatMode();
        });

        QObject::connect(m_compatMenuSMBX2, static_cast<void(QAction::*)(bool)>(&QAction::triggered), [this](bool)
        {
            m_calibration.compatProfile = Calibration::COMPAT_SMBX2;
            updateCompatMode();
        });

        QObject::connect(m_compatMenu38A, static_cast<void(QAction::*)(bool)>(&QAction::triggered), [this](bool)
        {
            m_calibration.compatProfile = Calibration::COMPAT_SMBX38A;
            updateCompatMode();
        });

        ui->compatMode->setMenu(&m_compatMenu);
        updateCompatMode();
    }

    {
        QToolButton *newAction = new QToolButton(ui->mainBar);
        newAction->setMenu(ui->menuSave);
        newAction->setIcon(ui->menuSave->icon());
        newAction->setPopupMode(QToolButton::InstantPopup);
        newAction->setToolTip(ui->menuSave->title());
        //Automatically reset label on language switching
        newAction->connect(&m_translator, &Translator::languageSwitched, [this, newAction](){
                                newAction->setToolTip(ui->menuSave->title());
                           });
        ui->mainBar->insertWidget(ui->actionSaveMenu, newAction);
        ui->actionSaveMenu->setVisible(false);
    }
//    ui->actionSaveMenu->setMenu(ui->menuSave);

    translateMenus();

    Graphics::init();

    loadAppSettings();

    m_lockControls = true;
    ui->FrameX->setValue(m_frmX);
    ui->FrameY->setValue(m_frmY);
    m_lockControls = false;

    ui->preview->setAllowScroll(true);
    QObject::connect(ui->preview, &FrameTuneScene::delta,
                     this, [this](Qt::MouseButton button, int deltaX, int deltaY)->void
    {
        switch(button)
        {
        case Qt::LeftButton:
            if(deltaX != 0)
                ui->OffsetX->setValue(ui->OffsetX->value() + deltaX);
            if(deltaY != 0)
                ui->OffsetY->setValue(ui->OffsetY->value() + deltaY);
            break;

        case Qt::RightButton:
            if(!ui->showGrabItem->isChecked())
                break;

            if(!ui->isRightDirect->isChecked())
                deltaX *= -1;

            if(deltaX != 0)
            {
                ui->grabOffsetX->setValue(ui->grabOffsetX->value() + deltaX);
                grabOffsetXupdate();
            }

            if(deltaY != 0)
            {
                ui->grabOffsetY->setValue(ui->grabOffsetY->value() + deltaY);
                grabOffsetYupdate();
            }
            break;

        default:
            break;
        }
    });

    m_animator = new Animator(m_calibration, this);
    m_animator->setModal(false);

    QObject::connect(m_animator, &Animator::settingsModified,
    [this]()->void
    {
        m_wasModified = true;
    });

    m_matrix = new Matrix(&m_calibration, this, Matrix::LabelNormal, this);
    m_matrix->setModal(false);
    m_matrix->changeGridSize(m_calibration.matrixWidth, m_calibration.matrixHeight);

    QObject::connect(&m_translator, &Translator::languageSwitched,
                     m_matrix, &Matrix::languageSwitched);
    QObject::connect(&m_translator, &Translator::languageSwitched,
                     m_animator, &Animator::languageSwitched);
    QObject::connect(m_matrix, &Matrix::frameSelected,
                     this, &CalibrationMain::frameSelected);
    QObject::connect(m_matrix, &Matrix::currentFrameSwitched,
                     ui->EnableFrame, &QCheckBox::setChecked);

    // TODO: Merge up sprite editor and hitbox tuner into the same box!
    ui->tipSpriteEditor->setVisible(false);

    QObject::connect(ui->scrollLeft, &QToolButton::clicked,
                     this, [this](bool)->void
    {
        auto o = ui->preview->getOffset();
        o.setX(o.x() + 4);
        ui->preview->setOffset(o);
    });

    QObject::connect(ui->scrollRight, &QToolButton::clicked,
                     this, [this](bool)->void
    {
        auto o = ui->preview->getOffset();
        o.setX(o.x() - 4);
        ui->preview->setOffset(o);
    });

    QObject::connect(ui->scrollUp, &QToolButton::clicked,
                     this, [this](bool)->void
    {
        auto o = ui->preview->getOffset();
        o.setY(o.y() + 4);
        ui->preview->setOffset(o);
    });

    QObject::connect(ui->scrollDown, &QToolButton::clicked,
                     this, [this](bool)->void
    {
        auto o = ui->preview->getOffset();
        o.setY(o.y() - 4);
        ui->preview->setOffset(o);
    });

    QObject::connect(ui->scrollReturn, &QToolButton::clicked,
                     this, [this](bool)->void
    {
        ui->preview->resetScroll();
    });

    QObject::connect(ui->zoomIn, &QToolButton::clicked,
                     this, [this](bool)->void
    {
        ui->preview->setZoom(ui->preview->getZoom() + 2);
    });

    QObject::connect(ui->zoomOut, &QToolButton::clicked,
                     this, [this](bool)->void
    {
        ui->preview->setZoom(ui->preview->getZoom() - 2);
    });

    windowDisable(true);

    if(!m_currentFile.isEmpty() && QFile::exists(m_currentFile))
        openFile(m_currentFile);
}

CalibrationMain::~CalibrationMain()
{
    delete ui;
    Graphics::quit();
}

void CalibrationMain::closeEvent(QCloseEvent *event)
{
    saveAppSettings();

    if(!trySave())
    {
        event->ignore();
        return;
    }

    event->accept();
}

void CalibrationMain::dragEnterEvent(QDragEnterEvent *e)
{
    if (e->mimeData()->hasUrls()) {
        e->acceptProposedAction();
    }
}

void CalibrationMain::dropEvent(QDropEvent *e)
{
    this->raise();
    this->setFocus(Qt::ActiveWindowFocusReason);

    for(const QUrl &url : e->mimeData()->urls())
    {
        const QString &fileName = url.toLocalFile();
        //qDebug() << "Dropped file:" << fileName;

        QFileInfo i(fileName);
        // Accepting PNG and GIF only
        if(i.suffix().compare("png", Qt::CaseInsensitive) &&
           i.suffix().compare("gif", Qt::CaseInsensitive))
            continue;

        openFile(fileName);
        break;
    }
}

void CalibrationMain::languageSwitched()
{
    ui->retranslateUi(this);
    translateMenus();
}

void CalibrationMain::windowDisable(bool d)
{
    ui->centralwidget->setEnabled(!d);
    ui->menuSave->setEnabled(!d);
    ui->menuEdit->setEnabled(!d);
    ui->actionAnimator->setEnabled(!d);
    ui->actionSpriteEditor->setEnabled(!d);
    ui->actionCopy_settings->setEnabled(!d);
    ui->actionPaste_settings->setEnabled(!d);
    ui->actionApplyToAll->setEnabled(!d);
    ui->actionSaveMenu->setEnabled(!d);
    ui->actionResetScroll->setEnabled(!d);
    ui->menuImport_Export->setEnabled(!d);
    ui->actionBrowseSpriteDirectory->setEnabled(!d);

    ui->preview->setEnabled(!d);
    ui->dockFrameSelect->setEnabled(!d);
    ui->dockSettings->setEnabled(!d);
    ui->dockScrolling->setEnabled(!d);
}

void CalibrationMain::frameSelected(int x, int y)
{
    m_lockControls = true;
    ui->FrameX->setValue(x);
    ui->FrameY->setValue(y);
    m_lockControls = false;
    ui->preview->setBlockRepaint(true);
    initScene();
    updateControls();
    updateScene();
    ui->preview->setBlockRepaint(false);
}

void CalibrationMain::translateMenus()
{
    if(m_wallMenuNone)
        m_wallMenuNone->setText(tr("None", "Display wall"));

    if(m_wallMenuFloor)
        m_wallMenuFloor->setText(tr("Floor", "Display wall"));
    if(m_wallMenuFloorWallL)
        m_wallMenuFloorWallL->setText(tr("Floor + left wall", "Display wall"));
    if(m_wallMenuFloorWallR)
        m_wallMenuFloorWallR->setText(tr("Floor + right wall", "Display wall"));

    if(m_wallMenuCeiling)
        m_wallMenuCeiling->setText(tr("Ceiling", "Display wall"));
    if(m_wallMenuCeilingWallL)
        m_wallMenuCeilingWallL->setText(tr("Ceiling + left wall", "Display wall"));
    if(m_wallMenuCeilingWallR)
        m_wallMenuCeilingWallR->setText(tr("Ceiling + right wall", "Display wall"));
}


void CalibrationMain::on_FrameX_valueChanged(int)
{
    if(m_lockControls) return;
    ui->preview->setBlockRepaint(true);
    initScene();
    updateControls();
    updateScene();
    m_matrix->setFrame(m_frmX, m_frmY);
    ui->preview->setBlockRepaint(false);
}

void CalibrationMain::on_FrameY_valueChanged(int)
{
    if(m_lockControls) return;
    ui->preview->setBlockRepaint(true);
    initScene();
    updateControls();
    updateScene();
    m_matrix->setFrame(m_frmX, m_frmY);
    ui->preview->setBlockRepaint(false);
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


void CalibrationMain::on_isDuckFrame_clicked(bool checked)
{
    m_calibration.frames[ {m_frmX, m_frmY}].isDuck = checked;
    enableFrame();
    updateScene();
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



void CalibrationMain::on_Matrix_clicked(bool checked)
{
    if(checked)
    {
        m_matrix->show();
        QRect g = this->frameGeometry();
        m_matrix->move(g.right(), g.top());
        m_matrix->update();
        m_matrix->repaint();
    }
    else
        m_matrix->hide();
}


void CalibrationMain::on_grabOffsetX_valueChanged(int)
{
    if(ui->grabOffsetX->hasFocus())
        grabOffsetXupdate();
}

void CalibrationMain::on_grabOffsetY_valueChanged(int)
{
    if(ui->grabOffsetY->hasFocus())
        grabOffsetYupdate();
}

void CalibrationMain::grabOffsetXupdate()
{
    m_calibration.grabOffsetX = ui->grabOffsetX->value();
    enableFrame();
    updateScene();
}

void CalibrationMain::grabOffsetYupdate()
{
    m_calibration.grabOffsetY = ui->grabOffsetY->value();
    enableFrame();
    updateScene();
}

void CalibrationMain::enableFrame()
{
    m_wasModified = true;
    m_calibration.frames[{m_frmX, m_frmY}].used = true;
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
    ui->preview->setGlobalSetup(m_calibration);
    ui->preview->setFrameSetup(m_calibration.frames[{m_frmX, m_frmY}]);
    m_animator->syncCalibration();
}

void CalibrationMain::initScene()
{
    m_frmX = ui->FrameX->value();
    m_frmY = ui->FrameY->value();

    int cellWidth = m_calibration.cellWidth;
    int cellHeight = m_calibration.cellHeight;

    ui->preview->setImage(m_xImageSprite.copy(QRect(m_frmX * cellWidth,
                                                    m_frmY * cellHeight,
                                                    cellWidth,
                                                    cellHeight)));
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

void CalibrationMain::on_resetScroll_clicked()
{
    on_actionResetScroll_triggered();
}


void CalibrationMain::on_bgColor_clicked()
{
    QColor res = QColorDialog::getColor(ui->preview->getBgColor(), this, tr("Background color"));
    if(res.isValid())
        ui->preview->setBgColor(res);
}


void CalibrationMain::on_actionOpen_triggered()
{
    openSprite();
}


void CalibrationMain::on_actionQuit_triggered()
{
    this->close();
}


void CalibrationMain::on_actionSaveSameDir_triggered()
{
    saveConfig(m_calibration, m_currentFile, false, &m_calibrationDefault);
    m_wasModified = false;
}


void CalibrationMain::on_actionSaveCustomDir_triggered()
{
    if(saveConfig(m_calibration, m_currentFile, true, &m_calibrationDefault))
        m_wasModified = false;
}


void CalibrationMain::on_actionImport38A_triggered()
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
}


void CalibrationMain::on_actionExport38A_triggered()
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

    ui->statusBar->showMessage(tr("The level file has been patched!") + "\n" + san_ba_level, 5000);
}


void CalibrationMain::on_actionExportHitboxMap_triggered()
{
    exportHitboxesMap();
}


void CalibrationMain::on_actionAbout_triggered()
{
    about dialog(this);
    dialog.setWindowFlags(Qt::Window | Qt::WindowCloseButtonHint);
    dialog.exec();
}

void CalibrationMain::on_actionResetScroll_triggered()
{
    ui->preview->resetScroll();
}


void CalibrationMain::on_actionCopy_settings_triggered()
{
    m_clipboard = m_calibration.frames[{m_frmX, m_frmY}];
}


void CalibrationMain::on_actionPaste_settings_triggered()
{
    m_calibration.frames[ {m_frmX, m_frmY}] = m_clipboard;

    updateControls();
    updateScene();
    m_wasModified = true;
}


void CalibrationMain::on_actionApplyToAll_triggered()
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

    for(int x = 0; x < m_calibration.matrixWidth; x++)
        for(int y = 0; y < m_calibration.matrixHeight; y++)
            m_calibration.frames[ {x, y}] = m_clipboard;

    m_wasModified = true;
}


void CalibrationMain::on_actionWikiPage_triggered()
{
    QDesktopServices::openUrl(QUrl("https://wohlsoft.ru/pgewiki/Playable_character_Calibrator"));
}

void CalibrationMain::updateCompatMode()
{
    m_compatMenuCalibratorFull->setChecked(false);
    m_compatMenuMoondust->setChecked(false);
    m_compatMenuTheXTech->setChecked(false);
    m_compatMenuSMBX2->setChecked(false);
    m_compatMenu38A->setChecked(false);

    switch(m_calibration.compatProfile)
    {
    case Calibration::COMPAT_UNSPECIFIED:
        ui->compatMode->setText(tr("Compat: %1").arg(tr("Unspecified", "Unspecified compatibility mode")));
        ui->compatMode->setIcon(QIcon(":/compat/unspecified.png"));
        break;

    case Calibration::COMPAT_CALIBRATOR_FULL:
        m_compatMenuCalibratorFull->setChecked(true);
        ui->compatMode->setText(tr("Compat: %1").arg("Calibrator"));
        ui->compatMode->setIcon(m_compatMenuCalibratorFull->icon());
        break;

    case Calibration::COMPAT_MOONDUST:
        m_compatMenuMoondust->setChecked(true);
        ui->compatMode->setText(tr("Compat: %1").arg("Moondust"));
        ui->compatMode->setIcon(m_compatMenuMoondust->icon());
        break;

    case Calibration::COMPAT_THEXTECH:
        m_compatMenuTheXTech->setChecked(true);
        ui->compatMode->setText(tr("Compat: %1").arg("TheXTech"));
        ui->compatMode->setIcon(m_compatMenuTheXTech->icon());
        break;

    case Calibration::COMPAT_SMBX38A:
        m_compatMenu38A->setChecked(true);
        ui->compatMode->setText(tr("Compat: %1").arg("SMBX-38A"));
        ui->compatMode->setIcon(m_compatMenu38A->icon());
        break;

    case Calibration::COMPAT_SMBX2:
        m_compatMenuSMBX2->setChecked(true);
        ui->compatMode->setText(tr("Compat: %1").arg("LunaLua/X2"));
        ui->compatMode->setIcon(m_compatMenuSMBX2->icon());
        break;
    }
}

void CalibrationMain::on_actionBrowseSpriteDirectory_triggered()
{
    if(!m_currentFile.isEmpty())
    {
        QFileInfo i(m_currentFile);
        auto d = i.absoluteDir();
        if(d.exists())
            QDesktopServices::openUrl(QUrl::fromLocalFile(d.absolutePath()));
    }
}


void CalibrationMain::on_actionAnimator_triggered()
{
    m_animator->show();
    QRect g = this->frameGeometry();
    QRect a = m_animator->frameGeometry();
    m_animator->move(g.right(), g.bottom() -a.height());
    m_animator->update();
    m_animator->repaint();

//    m_matrix->hide();
//    this->hide();
//    auto old = m_calibration.animations;
//    Animator dialog(m_calibration, this);
//    dialog.setWindowFlags(Qt::Window | Qt::WindowCloseButtonHint);
//    dialog.exec();
//    if(old != m_calibration.animations)
//        m_wasModified = true;
//    this->show();
//    this->raise();
//    if(ui->Matrix->isChecked())
//        m_matrix->show();
}


void CalibrationMain::on_actionSpriteEditor_triggered()
{
    m_matrix->hide();
    m_animator->hide();
    this->hide();

    ImageCalibrator *imgCalibrator = new ImageCalibrator(&m_calibration, this);
    imgCalibrator->setWindowFlags(Qt::Window | Qt::WindowCloseButtonHint);
    if(!imgCalibrator->init(m_currentFile))
    {
        delete imgCalibrator;
        this->show();
        this->raise();
        return;
    }

    imgCalibrator->setFrame(m_frmX, m_frmY);
    imgCalibrator->setPreviewOffset(ui->preview->getOffset());
    imgCalibrator->setPreviewZoom(ui->preview->getZoom());

    if(ui->Matrix->isChecked())
    {
        imgCalibrator->show();
        imgCalibrator->showSpriteMap();
    }

    imgCalibrator->exec();
    imgCalibrator->unInit();

    auto oldFrames = m_calibration.frames;

    openFile(imgCalibrator->m_targetPath);
    // Do update hitboxes in only condition when calibrated image got been saved
    // Otherwise, reset back the previous state
    if(imgCalibrator->hitboxNeedSave())
    {
        m_calibration.frames = oldFrames;
        m_wasModified = true;
        saveConfig(m_calibration, m_currentFile, false, &m_calibrationDefault);
        m_wasModified = false;
    }

    delete imgCalibrator;

    this->show();
    this->raise();
    if(ui->Matrix->isChecked())
        m_matrix->show();
    updateControls();
    updateScene();
}

void CalibrationMain::on_actionChangeGFXEditor_triggered()
{
    changeGfxEditor();
}


void CalibrationMain::on_actionResetFrameSettings_triggered()
{
    QDialog resetSetup(this);
    QGridLayout *l = new QGridLayout(&resetSetup);

    resetSetup.setWindowTitle(tr("Reset incorrect frame setup...", "Reset frame properties"));

    resetSetup.setLayout(l);
    QLabel *info = new QLabel(tr("Select properties to reset if they were inproperly set:", "Reset frame properties"), &resetSetup);
    l->addWidget(info , 0, 0, 1, 3);

    QCheckBox *resetDuck = new QCheckBox(tr("Is duck frame"), &resetSetup);
    l->addWidget(resetDuck, 1, 0, 1, 3);

    QCheckBox *resetRightDir = new QCheckBox(tr("Right direction"), &resetSetup);
    l->addWidget(resetRightDir, 2, 0, 1, 3);

    QCheckBox *resetShowItem = new QCheckBox(tr("Show carried item"), &resetSetup);
    l->addWidget(resetShowItem, 3, 0, 1, 3);

    QCheckBox *resetMountRide = new QCheckBox(tr("Mount riding"), &resetSetup);
    l->addWidget(resetMountRide, 4, 0, 1, 3);

    QFrame *hLine = new QFrame(&resetSetup);
    hLine->setObjectName(QString::fromUtf8("line"));
    hLine->setGeometry(QRect(320, 150, 118, 3));
    hLine->setFrameShape(QFrame::HLine);
    hLine->setFrameShadow(QFrame::Sunken);
    l->addWidget(hLine, 5, 0, 1, 3);

    QCheckBox *resetAll = new QCheckBox(tr("Reset at all frames"), &resetSetup);
    l->addWidget(resetAll, 6, 0, 1, 3);

    QPushButton *hit = new QPushButton(&resetSetup);
    hit->setText(tr("Confirm"));
    l->addWidget(hit, 7, 1, 1, 1);
    QObject::connect(hit, static_cast<void (QAbstractButton::*)(bool)>(&QAbstractButton::clicked),
                     &resetSetup, &QDialog::accept);

    int ret = resetSetup.exec();
    if(ret == QDialog::Accepted)
    {
        QFileInfo ourFile(m_currentFile);
        QString ini_default = ApplicationPath + "/calibrator/spriteconf/" + ourFile.baseName() + ".ini";

        Calibration default_conf;
        loadConfig(default_conf, ini_default);

        if(default_conf.matrixWidth != m_calibration.matrixWidth || default_conf.matrixHeight != m_calibration.matrixHeight)
        {
            QMessageBox::warning(this,
                                 tr("Incompatible sprite"),
                                 tr("Can't perform the reset to incompatible sprite (the width and/or height of matrix are different)."));
            return;
        }

        bool rstDuck = resetDuck->isChecked();
        bool rstRight = resetRightDir->isChecked();
        bool rstItem = resetShowItem->isChecked();
        bool rstMount = resetMountRide->isChecked();

        if(resetAll->isChecked())
        {
            for(auto it = m_calibration.frames.begin(); it != m_calibration.frames.end(); ++it)
            {
                auto pos = it.key();
                auto &frame = it.value();
                auto &def = default_conf.frames[pos];

                if(rstDuck)
                    frame.isDuck = def.isDuck;

                if(rstRight)
                    frame.isRightDir = def.isRightDir;

                if(rstItem)
                    frame.showGrabItem = def.showGrabItem;

                if(rstMount)
                    frame.isMountRiding = def.isMountRiding;
            }
        }
        else
        {
            auto &frame = m_calibration.frames[ {m_frmX, m_frmY}];
            auto &def = default_conf.frames[ {m_frmX, m_frmY}];

            if(rstDuck)
                frame.isDuck = def.isDuck;

            if(rstRight)
                frame.isRightDir = def.isRightDir;

            if(rstItem)
                frame.showGrabItem = def.showGrabItem;

            if(rstMount)
                frame.isMountRiding = def.isMountRiding;
        }

        m_wasModified = true;

        updateControls();
        updateScene();
    }
}

void CalibrationMain::on_actionSetDirectionByHalves_triggered()
{
    int ret = QMessageBox::question(this,
                                    tr("Set direction by halves"),
                                    tr("This action will set the \"Right direction\" option at all right frames on the matrix, "
                                       "and unset at all left frames. This action is supposed to be used with SMBX-compatible sprites. "
                                       "If your sprite is not based on SMBX standard, you will break it. Apply this action?"), QMessageBox::Yes|QMessageBox::No);
    if(ret == QMessageBox::Yes)
    {
        for(auto it = m_calibration.frames.begin(); it != m_calibration.frames.end(); ++it)
        {
            auto pos = it.key();
            auto &frame = it.value();

            frame.isRightDir = (pos.first >= m_calibration.matrixWidth / 2);
        }

        m_wasModified = true;

        updateControls();
        updateScene();
    }
}
