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

#ifndef ANIMATIONEDIT_H
#define ANIMATIONEDIT_H

#include <QDialog>
#include <QListWidget>
#include "../calibrationmain.h"
#include "../frame_matrix/matrix.h"
#include "../animator/aniFrames.h"

namespace Ui {
class AnimationEdit;
}

class AnimationEdit : public QDialog
{
    Q_OBJECT
    
public:
    explicit AnimationEdit(QList<QVector<frameOpts > >  &framesX, SpriteScene * Scene, FrameSets &frmConfs, QWidget *parent = 0);
    ~AnimationEdit();
    QList<QVector<frameOpts> > SrcFrames;

    void addFrameL(int x, int y);
    void addFrameR(int x, int y);
    void applyFrameSet();

    AniFrameSet currentFrameSet;
    FrameSets   frameList;

    SpriteScene *SrcScene;
    
private slots:
    void on_AddLeft_clicked();
    void on_SetLeft_clicked();
    void on_DelLeft_clicked();

    void on_AddRight_clicked();
    void on_SetRight_clicked();
    void on_DelRight_clicked();

    void on_FramesSets_itemClicked(QListWidgetItem *item);


private:
    Ui::AnimationEdit *ui;
};

#endif // ANIMATIONEDIT_H
