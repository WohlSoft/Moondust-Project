/*
 * SMBX64 Playble Character Sprite Calibrator, a free tool for playable srite design
 * This is a part of the Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2014 Vitaly Novichkov <admin@wohlnet.ru>
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

#include "animate.h"
#include <ui_animate.h>
#include "animationedit.h"
#include "aniFrames.h"
#include "../main/globals.h"
#include "../main/mw.h"

Animate::Animate(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Animate)
{
    ui->setupUi(this);

    //Here will be read AniFrames from INI

    AniScene = new AnimationScene();
    AniScene->setSceneRect(0,0, ui->AnimateView->width()-20, ui->AnimateView->height()-20);

    ui->AnimateView->setScene(AniScene);

    aniStyle = "Idle";
    aniDir = 1; //0 - left, 1 - right

    AniScene->setAnimation(AniFrames.set[0].R);
}

Animate::~Animate()
{
    delete ui;
}

void Animate::aniFindSet()
{
    foreach(AniFrameSet frms, AniFrames.set)
    {
        if(frms.name==aniStyle)
        {
            if(aniDir==1)
                AniScene->setAnimation(frms.R);
            else
                AniScene->setAnimation(frms.L);
            break;
        }
    }
}



/////////////////Slots//////////////////////////

void Animate::on_EditAnimationBtn_clicked()
{
    AnimationEdit dialog(AniFrames);
    dialog.setWindowFlags(Qt::Window | Qt::WindowCloseButtonHint);
    dialog.exec();
    AniFrames = dialog.frameList;
    aniFindSet();
}


///////////////////////////////////////////////////////////////////////////
//AnimateSwitches
/*
getSpriteAniData(conf, "Idle");
getSpriteAniData(conf, "Run");
getSpriteAniData(conf, "JumpFloat");
getSpriteAniData(conf, "SpinJump");
getSpriteAniData(conf, "Sliding");
getSpriteAniData(conf, "Climbing");
getSpriteAniData(conf, "Fire");
getSpriteAniData(conf, "SitDown");
getSpriteAniData(conf, "Dig");
getSpriteAniData(conf, "GrabRun");
getSpriteAniData(conf, "GrabJump");
getSpriteAniData(conf, "GrabSitDown");
getSpriteAniData(conf, "RacoonRun");
getSpriteAniData(conf, "RacoonFloat");
getSpriteAniData(conf, "RacoonFly");
getSpriteAniData(conf, "RacoonTail");
getSpriteAniData(conf, "Swim");
getSpriteAniData(conf, "TanookiStatue");

getSpriteAniData(conf, "OnYoshi");
getSpriteAniData(conf, "OnYoshiSit");
getSpriteAniData(conf, "PipeUpDown");
getSpriteAniData(conf, "SlopeSlide");

getSpriteAniData(conf, "SwordAttak");
getSpriteAniData(conf, "JumpSwordUp");
getSpriteAniData(conf, "JumpSwordDown");
getSpriteAniData(conf, "DownSwordAttak");
getSpriteAniData(conf, "Hurted");
*/
void Animate::on_Idle_clicked()
{
    aniStyle="Idle";
    aniFindSet();
}


void Animate::on_Run_clicked()
{
    aniStyle="Run";
    aniFindSet();
}

void Animate::on_Jump_clicked()
{
    aniStyle="JumpFloat";
    aniFindSet();
}

void Animate::on_Spin_clicked()
{
    aniStyle="SpinJump";
    aniFindSet();
}

void Animate::on_Sliding_clicked()
{
    aniStyle="Sliding";
    aniFindSet();
}

void Animate::on_Climbing_clicked()
{
    aniStyle="Climbing";
    aniFindSet();
}

void Animate::on_Fire_clicked()
{
    aniStyle="Fire";
    aniFindSet();
}

void Animate::on_SitDown_clicked()
{
    aniStyle="SitDown";
    aniFindSet();
}

void Animate::on_Dig_clicked()
{
    aniStyle="Dig";
    aniFindSet();
}

void Animate::on_GrabRun_clicked()
{
    aniStyle="GrabRun";
    aniFindSet();
}

void Animate::on_GrabJump_clicked()
{
    aniStyle="GrabJump";
    aniFindSet();
}

void Animate::on_GrabSit_clicked()
{
    aniStyle="GrabSitDown";
    aniFindSet();
}

void Animate::on_RacoonRun_clicked()
{
    aniStyle="RacoonRun";
    aniFindSet();
}

void Animate::on_RacoonFloat_clicked()
{
    aniStyle="RacoonFloat";
    aniFindSet();
}

void Animate::on_RacoonFly_clicked()
{
    aniStyle="RacoonFly";
    aniFindSet();
}

void Animate::on_RacoonTail_clicked()
{
    aniStyle="RacoonTail";
    aniFindSet();
}

void Animate::on_Swin_clicked()
{
    aniStyle="Swim";
    aniFindSet();
}

void Animate::on_TanookiStatue_clicked()
{
    aniStyle="TanookiStatue";
    aniFindSet();
}



void Animate::on_RideOnYoshi_clicked()
{
    aniStyle="OnYoshi";
    aniFindSet();
}

void Animate::on_RideOnYoshiSit_clicked()
{
    aniStyle="OnYoshiSit";
    aniFindSet();
}

void Animate::on_RideOnYoshiSit_2_clicked()
{
    aniStyle="PipeUpDown";
    aniFindSet();
}

void Animate::on_SlopeSlide_clicked()
{
    aniStyle="SlopeSlide";
    aniFindSet();
}


void Animate::on_SwordAttak_clicked()
{
    aniStyle="SwordAttak";
    aniFindSet();
}

void Animate::on_JumpSwordUp_clicked()
{
    aniStyle="JumpSwordUp";
    aniFindSet();
}

void Animate::on_JumpSwordDown_clicked()
{
    aniStyle="JumpSwordDown";
    aniFindSet();
}

void Animate::on_DownSword_clicked()
{
    aniStyle="DownSwordAttak";
    aniFindSet();
}

void Animate::on_Hurted_clicked()
{
    aniStyle="Hurted";
    aniFindSet();
}

//Set Direction
void Animate::on_directLeft_clicked()
{
    aniDir=0;
    aniFindSet();
}

void Animate::on_directRight_clicked()
{
    aniDir=1;
    aniFindSet();
}
