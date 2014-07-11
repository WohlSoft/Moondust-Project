/*
 * SMBX64 Playble Character Sprite Calibrator, a free tool for playble srite design
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

#ifndef CALIBRATIONMAIN_H
#define CALIBRATIONMAIN_H

#include <QWidget>
#include "animator/SpriteScene.h"
#include <QPainter>
#include <QGraphicsScene>
#include <QtWidgets>
#include "animator/aniFrames.h"

struct frameOpts
{
    unsigned int H;
    unsigned int W;
    int offsetX;
    int offsetY;
    bool used;
};


namespace Ui {
class CalibrationMain;
}

class CalibrationMain : public QWidget
{
    Q_OBJECT
    
public:
    explicit CalibrationMain(QWidget *parent = 0);
    ~CalibrationMain();
    SpriteScene * Scene;
    FrameSets AnimationFrames;
    void getSpriteAniData(QSettings &set, QString name);
    void setSpriteAniData(QSettings &set);

    QString currentConfig;
    void loadConfig(QString fileName);
    void saveConfig(QString fileName);
    void OpenFile(QString fileName);
protected:
    void closeEvent(QCloseEvent *event);

private slots:
    void on_FrameX_valueChanged(int arg1);
    void on_FrameY_valueChanged(int arg1);
    void on_Height_valueChanged(int arg1);
    void on_Width_valueChanged(int arg1);
    void on_OffsetX_valueChanged(int arg1);
    void on_OffsetY_valueChanged(int arg1);
    void on_CopyButton_clicked();
    void on_PasteButton_clicked();
    void on_OpenSprite_clicked();

    void on_AboutButton_clicked();

    void on_SaveConfigButton_clicked();

    void on_applyToAll_clicked();

    void on_MakeTemplateB_clicked();

    void on_EnableFrame_clicked(bool checked);

    void on_Matrix_clicked();

    void on_AnimatorButton_clicked();

    void on_editSizes_clicked();

private:
    Ui::CalibrationMain *ui;
};



#endif // CALIBRATIONMAIN_H
