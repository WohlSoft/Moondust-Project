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
class MouseScene;

class CalibrationMain : public QWidget
{
    Q_OBJECT

    QString m_lastOpenDir;
    QString m_currentFile;
    int m_curFrameX = 0, m_curFrameY = 0;

    Calibration m_calibration;
    Calibration m_calibrationDefault;
    CalibrationFrame m_clipboard;

    Matrix *m_matrix = nullptr;
public:
    explicit CalibrationMain(QWidget *parent = nullptr);
    ~CalibrationMain();

    void translateMenus();

    //! Scene for drawing of playable character preview
    MouseScene *m_scene;

    void fillDefaultAniData(Calibration &dst);
    void loadSpriteAniEntry(Calibration &dst, QSettings &set, QString name);
    void saveSpriteAniData(Calibration &src, QSettings &set, Calibration *merge_with);

    QString m_currentConfig;
    void loadConfig(Calibration &dst, QString fileName, Calibration *merge_with = nullptr);
    bool saveConfig(Calibration &src, QString fileName, bool customPath = false, Calibration *merge_with = nullptr);
    bool importFrom38A(Calibration &dst, QString imageName, QString fileName);
    bool exportTo38A(Calibration &src, QString imageName, QString fileName);
    void openFile(QString fileName);

    void createDirs();

    bool m_wasCanceled = false;

    QPixmap m_xImageSprite;

protected:
    void closeEvent(QCloseEvent *event);

private slots:
    void languageSwitched();

    void frameSelected(int x, int y);

    void on_AboutButton_clicked();
    void on_Matrix_clicked();
    void on_AnimatorButton_clicked();
    void on_calibrateImage_clicked();
    void exportHitboxesMap();

    void on_EnableFrame_clicked(bool checked);

    void on_FrameX_valueChanged(int);
    void on_FrameY_valueChanged(int);
    void on_Height_valueChanged(int arg1);
    void on_Height_duck_valueChanged(int arg1);
    void on_Width_valueChanged(int arg1);

    void on_grabOffsetX_valueChanged(int arg1);
    void on_grabOffsetY_valueChanged(int arg1);

    void on_grabTop_clicked();
    void on_grabSide_clicked();

    void on_OffsetX_valueChanged(int arg1);
    void on_OffsetY_valueChanged(int arg1);

    void on_isDuckFrame_clicked(bool checked);
    void on_isRightDirect_clicked(bool checked);
    void on_showGrabItem_clicked(bool checked);
    void on_mountRiding_clicked(bool checked);

    void on_CopyButton_clicked();
    void on_PasteButton_clicked();

    bool on_OpenSprite_clicked();

    void on_applyToAll_clicked();

    bool trySave();
    void updateControls();
    void initScene();
    void enableFrame();
    void updateScene();

private:
    int m_frmX = 0;
    int m_frmY = 0;

    bool m_lockControls = false;
    bool m_wasModified = false;

    QPoint m_framePos;
    QGraphicsPixmapItem m_currentImageItem;
    QPixmap             m_currentPixmap;
    QGraphicsRectItem m_frameBox_gray;
    QGraphicsRectItem m_hitBox_green;

    QGraphicsPixmapItem m_mountItem;
    QPixmap             m_mountPixmap;
    QPixmap             m_mountDuckPixmap;

    QGraphicsLineItem m_grabLineX;
    QGraphicsLineItem m_grabLineY;

    Ui::CalibrationMain *ui;
    QString     m_titleCache;

    QMenu       m_saveMenu;
    QAction    *m_saveMenuQuickSave = nullptr;
    QAction    *m_saveMenuSaveAs = nullptr;

    QMenu       m_toolsMenu;
    QAction    *m_toolsExportHitboxMap = nullptr;
    QAction    *m_toolsImport38A = nullptr;
    QAction    *m_toolsExport38A = nullptr;

    QMenu       m_langMenu;
    Translator  m_translator;
};

#endif // CALIBRATIONMAIN_H
