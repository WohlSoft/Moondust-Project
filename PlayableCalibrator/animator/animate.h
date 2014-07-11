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

#ifndef ANIMATE_H
#define ANIMATE_H

#include <QDialog>
#include "SpriteScene.h"
#include "calibrationmain.h"
#include "AnimationScene.h"
#include "aniFrames.h"

namespace Ui {
class Animate;
}

class Animate : public QDialog
{
    Q_OBJECT
    
public:
    explicit Animate(QList<QVector<frameOpts > >  &framesX, FrameSets &frameConf, SpriteScene * Scene, QWidget *parent = 0);
    ~Animate();
    AnimationScene *AniScene;

    FrameSets AniFrames;

    QList<QVector<frameOpts> > SrcFrames;

    SpriteScene * SrcScene;

    QString aniStyle;
    int aniDir;
    void aniFindSet();
    
private slots:
    void on_EditAnimationBtn_clicked();
    void on_Idle_clicked();
    void on_Run_clicked();
    void on_Jump_clicked();
    void on_Spin_clicked();
    void on_Sliding_clicked();
    void on_Climbing_clicked();
    void on_Fire_clicked();
    void on_SitDown_clicked();
    void on_Dig_clicked();
    void on_GrabRun_clicked();
    void on_GrabJump_clicked();
    void on_GrabSit_clicked();
    void on_RacoonRun_clicked();
    void on_RacoonFloat_clicked();
    void on_RacoonFly_clicked();
    void on_RacoonTail_clicked();
    void on_Swin_clicked();
    void on_TanookiStatue_clicked();
    void on_SwordAttak_clicked();
    void on_JumpSwordUp_clicked();
    void on_JumpSwordDown_clicked();
    void on_DownSword_clicked();
    void on_Hurted_clicked();

    void on_directLeft_clicked();

    void on_directRight_clicked();

    void on_RideOnYoshi_clicked();

    void on_RideOnYoshiSit_clicked();

    void on_RideOnYoshiSit_2_clicked();

    void on_SlopeSlide_clicked();

private:
    Ui::Animate *ui;
};

#endif // ANIMATE_H
