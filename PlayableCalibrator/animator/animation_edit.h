/*
 * Playble Character Calibrator, a free tool for playable character sprite setup tune
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

#pragma once
#ifndef ANIMATIONEDIT_H
#define ANIMATIONEDIT_H

#include <QDialog>
#include <QPixmap>
#include <QListWidget>
#include <QTimer>

#include "calibration_main.h"
#include "frame_matrix/matrix.h"
#include "main/calibration.h"


namespace Ui {
class AnimationEdit;
}

class AnimationEdit : public QDialog
{
    Q_OBJECT

    Calibration *m_conf = nullptr;
    CalibrationMain *m_mw = nullptr;
public:
    explicit AnimationEdit(Calibration *conf, QObject *mw, QWidget *parent = nullptr);
    ~AnimationEdit();

    void addFrameL(int x, int y);
    void addFrameR(int x, int y);
    void applyFrameSet();

    Calibration::AnimationSet m_frameList;

private slots:
    void on_AddLeft_clicked();
    void on_SetLeft_clicked();
    void on_DelLeft_clicked();

    void on_AddRight_clicked();
    void on_SetRight_clicked();
    void on_DelRight_clicked();

    void on_FramesSets_currentItemChanged(QListWidgetItem *item, QListWidgetItem *);

    void on_FramesL_itemSelectionChanged();
    void on_FramesR_itemSelectionChanged();
    void on_FramesL_currentItemChanged(QListWidgetItem *item, QListWidgetItem *);
    void on_FramesR_currentItemChanged(QListWidgetItem *item, QListWidgetItem *);
    void on_FramesL_itemClicked(QListWidgetItem *item);
    void on_FramesR_itemClicked(QListWidgetItem *item);
    void nextFrame();

    void play();
    void pause();

    void on_playLeft_clicked();

    void on_playRight_clicked();

    void on_frameSpeed_valueChanged(int arg1);

private:
    void showFrame(int x, int y);
    int m_direction = 1;
    QVector<AniFrame > m_frames;
    int m_currentFrame;
    QTimer m_ticker;
    QPixmap m_currentFrameImg;
    Ui::AnimationEdit *ui;
};

#endif // ANIMATIONEDIT_H
