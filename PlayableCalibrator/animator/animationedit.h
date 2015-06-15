/*
 * SMBX64 Playble Character Sprite Calibrator, a free tool for playable srite design
 * This is a part of the Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2015 Vitaly Novichkov <admin@wohlnet.ru>
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

#ifndef ANIMATIONEDIT_H
#define ANIMATIONEDIT_H

#include <QDialog>
#include <QListWidget>
#include "../calibrationmain.h"
#include "../frame_matrix/matrix.h"
#include "../animator/aniFrames.h"

#include <QTimer>

namespace Ui {
class AnimationEdit;
}

class AnimationEdit : public QDialog
{
    Q_OBJECT
    
public:
    explicit AnimationEdit(FrameSets &frmConfs, QWidget *parent = 0);
    ~AnimationEdit();
    QList<QVector<frameOpts> > SrcFrames;

    void addFrameL(int x, int y);
    void addFrameR(int x, int y);
    void applyFrameSet();

    AniFrameSet currentFrameSet;
    FrameSets   frameList;
    
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

    void nextFrame();

    void play();
    void pause();

    void on_playLeft_clicked();

    void on_playRight_clicked();

    void on_frameSpeed_valueChanged(int arg1);

private:
    void showFrame(int x, int y);
    int direction;
    QVector<AniFrame > frames;
    int currentFrame;
    QTimer ticker;
    QPixmap current_frame;
    Ui::AnimationEdit *ui;
};

#endif // ANIMATIONEDIT_H
