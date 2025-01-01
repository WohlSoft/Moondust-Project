/*
 * Playble Character Calibrator, a free tool for playable character sprite setup tune
 * This is a part of the Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2017-2025 Vitaly Novichkov <admin@wohlnet.ru>
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

#pragma once
#ifndef CALIBRATIONMAIN_H
#define CALIBRATIONMAIN_H

#include <QMainWindow>
#include <QPainter>
#include <QGraphicsScene>
#include <QtWidgets>
#include <QMenu>

#include <translator-qt/translator.h>

#include "main/calibration.h"

namespace Ui {
class CalibrationMain;
}

class Matrix;
class Animator;

class CalibrationMain : public QMainWindow
{
    Q_OBJECT

    QString m_lastOpenDir;
    QString m_currentFile;
    QString m_externalGfxEditorPath;
    int m_curFrameX = 0, m_curFrameY = 0;

    Calibration m_calibration;
    Calibration m_calibrationDefault;
    CalibrationFrame m_clipboard;

    Matrix *m_matrix = nullptr;
    Animator *m_animator = nullptr;
public:
    explicit CalibrationMain(QWidget *parent = nullptr);
    ~CalibrationMain();

    void translateMenus();

    void fillDefaultAniData(Calibration &dst);
    void loadSpriteAniEntry(Calibration &dst, QSettings &set, QString name);
    void saveSpriteAniData(Calibration &src, QSettings &set, Calibration *merge_with);

    QString m_currentConfig;
    void loadConfig(Calibration &dst, QString fileName, Calibration *merge_with = nullptr);
    bool saveConfig(Calibration &src, QString fileName, bool customPath = false, Calibration *merge_with = nullptr);
    void updateImageMetrics(Calibration &dst, const QPixmap &sprite);
    bool importFrom38A(Calibration &dst, QString imageName, QString fileName);
    bool exportTo38A(Calibration &src, QString imageName, QString fileName);
    void openFile(QString fileName);
    bool openSprite();

    void createDirs();

    bool m_wasCanceled = false;

    QPixmap m_xImageSprite;

    void loadAppSettings();
    void saveAppSettings();
    QString getGfxEditorPath(QWidget *parent = nullptr);
    bool changeGfxEditor(QWidget *parent = nullptr);

protected:
    void closeEvent(QCloseEvent *event);

    void dragEnterEvent(QDragEnterEvent *e);
    void dropEvent(QDropEvent *e);

private slots:
    void languageSwitched();

    void windowDisable(bool d);

    void frameSelected(int x, int y);

    void on_Matrix_clicked(bool checked);
    void exportHitboxesMap();

    void on_EnableFrame_clicked(bool checked);

    void on_FrameX_valueChanged(int);
    void on_FrameY_valueChanged(int);
    void on_Height_valueChanged(int arg1);
    void on_Height_duck_valueChanged(int arg1);
    void on_Width_valueChanged(int arg1);

    void on_grabOffsetX_valueChanged(int arg1);
    void on_grabOffsetY_valueChanged(int arg1);

    void grabOffsetXupdate();
    void grabOffsetYupdate();

    void on_grabTop_clicked();
    void on_grabSide_clicked();

    void on_OffsetX_valueChanged(int arg1);
    void on_OffsetY_valueChanged(int arg1);

    void on_isDuckFrame_clicked(bool checked);
    void on_isRightDirect_clicked(bool checked);
    void on_showGrabItem_clicked(bool checked);
    void on_mountRiding_clicked(bool checked);

    bool trySave();
    void updateControls();
    void initScene();
    void enableFrame();
    void updateScene();

    void on_resetScroll_clicked();
    void on_bgColor_clicked();

    void on_actionOpen_triggered();
    void on_actionQuit_triggered();
    void on_actionSaveSameDir_triggered();
    void on_actionSaveCustomDir_triggered();
    void on_actionImport38A_triggered();
    void on_actionExport38A_triggered();
    void on_actionExportHitboxMap_triggered();
    void on_actionAbout_triggered();
    void on_actionResetScroll_triggered();
    void on_actionCopy_settings_triggered();
    void on_actionPaste_settings_triggered();
    void on_actionApplyToAll_triggered();
    void on_actionWikiPage_triggered();

    void on_actionBrowseSpriteDirectory_triggered();
    void on_actionAnimator_triggered();
    void on_actionSpriteEditor_triggered();

    void on_actionChangeGFXEditor_triggered();

private:
    int m_frmX = 0;
    int m_frmY = 0;

    bool m_lockControls = false;
    bool m_wasModified = false;

    Ui::CalibrationMain *ui;
    QString     m_titleCache;

    QMenu       m_wallMenu;
    QAction    *m_wallMenuNone = nullptr;
    QAction    *m_wallMenuFloor = nullptr;
    QAction    *m_wallMenuFloorWallL = nullptr;
    QAction    *m_wallMenuFloorWallR = nullptr;
    QAction    *m_wallMenuCeiling = nullptr;
    QAction    *m_wallMenuCeilingWallL = nullptr;
    QAction    *m_wallMenuCeilingWallR = nullptr;

    QMenu       m_compatMenu;
    QAction    *m_compatMenuCalibratorFull = nullptr;
    QAction    *m_compatMenuMoondust = nullptr;
    QAction    *m_compatMenuTheXTech = nullptr;
    QAction    *m_compatMenuSMBX2 = nullptr;
    QAction    *m_compatMenu38A = nullptr;
    void        updateCompatMode();

    Translator  m_translator;
};

#endif // CALIBRATIONMAIN_H
