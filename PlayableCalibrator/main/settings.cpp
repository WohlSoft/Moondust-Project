/*
 * SMBX64 Playble Character Sprite Calibrator, a free tool for playable srite design
 * This is a part of Platformer Game Engine by Wohlstand, a free platform for game making
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

#include <QPixmap>
#include <QtDebug>
#include <QSettings>

#include "calibrationmain.h"
#include "ui_calibrationmain.h"
#include "animator/SpriteScene.h"
#include "about/about.h"
#include "frame_matrix/matrix.h"
#include "animator/animate.h"
#include "animator/aniFrames.h"
#include "main/globals.h"

void CalibrationMain::loadConfig(QString fileName)
{
    currentConfig = fileName;
    QFileInfo ourFile(fileName);
    QString ini_sprite =  QApplication::applicationDirPath() + "/calibrator/spriteconf/" + ourFile.baseName() + ".ini";
    QSettings conf(ini_sprite, QSettings::IniFormat);
    int i, j;

    for(i=0; i<10;i++)
    {
        for(j=0; j<10;j++)
        {
            conf.beginGroup("frame-"+QString::number(i)+"-"+QString::number(j));
            framesX[i][j].H = conf.value("height", "100").toInt();
            framesX[i][j].W = conf.value("width", "100").toInt();
            framesX[i][j].offsetX = conf.value("offsetX", "0").toInt();
            framesX[i][j].offsetY = conf.value("offsetY", "0").toInt();
            framesX[i][j].used = conf.value("used", "true").toBool();
            conf.endGroup();
        }
    }

    AnimationFrames.set.clear();
    //get Animation frameSets
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
    getSpriteAniData(conf, "OnYoshi");
    getSpriteAniData(conf, "OnYoshiSit");
    getSpriteAniData(conf, "PipeUpDown");
    getSpriteAniData(conf, "SlopeSlide");
    getSpriteAniData(conf, "TanookiStatue");
    getSpriteAniData(conf, "SwordAttak");
    getSpriteAniData(conf, "JumpSwordUp");
    getSpriteAniData(conf, "JumpSwordDown");
    getSpriteAniData(conf, "DownSwordAttak");
    getSpriteAniData(conf, "Hurted");

}

void CalibrationMain::getSpriteAniData(QSettings &set, QString name)
{
    AniFrameSet frameSet;
    AniFrame frameXY;
    int frameTotal, i;

    set.beginGroup("Animation"+name+"_L");
        frameTotal = set.value("frames", "0").toInt();
        for(i=0;i<frameTotal;i++)
        {
            frameXY.x = set.value(QString("frame"+QString::number(i)+"x"), "0").toInt();
            frameXY.y = set.value(QString("frame"+QString::number(i)+"y"), "0").toInt();
            frameSet.L.push_back(frameXY);
        }
    set.endGroup();

    set.beginGroup("Animation"+name+"_R");
        frameTotal = set.value("frames", "0").toInt();
        for(i=0;i<frameTotal;i++)
        {
            frameXY.x = set.value(QString("frame"+QString::number(i)+"x"), "0").toInt();
            frameXY.y = set.value(QString("frame"+QString::number(i)+"y"), "0").toInt();
            frameSet.R.push_back(frameXY);
        }
    set.endGroup();
    frameSet.name = name;
    AnimationFrames.set.push_back(frameSet);
}


void CalibrationMain::setSpriteAniData(QSettings &set)
{
    int i, j;

    for(j=0;j<AnimationFrames.set.size();j++)
    {
        set.beginGroup("Animation"+AnimationFrames.set[j].name+"_L");
            set.setValue("frames", AnimationFrames.set[j].L.size());
            for(i=0;i<AnimationFrames.set[j].L.size();i++)
            {
                set.setValue("frame"+QString::number(i)+"x", AnimationFrames.set[j].L[i].x);
                set.setValue("frame"+QString::number(i)+"y", AnimationFrames.set[j].L[i].y);
            }
        set.endGroup();

        set.beginGroup("Animation"+AnimationFrames.set[j].name+"_R");
            set.setValue("frames", AnimationFrames.set[j].R.size());
            for(i=0;i<AnimationFrames.set[j].R.size();i++)
            {
                set.setValue("frame"+QString::number(i)+"x", AnimationFrames.set[j].R[i].x);
                set.setValue("frame"+QString::number(i)+"y", AnimationFrames.set[j].R[i].y);
            }
        set.endGroup();
    }
}


void CalibrationMain::saveConfig(QString fileName)
{
    QFileInfo ourFile(fileName);
    QString ini_sprite =  QApplication::applicationDirPath() + "/calibrator/spriteconf/" + ourFile.baseName() + ".ini";
    QSettings conf(ini_sprite, QSettings::IniFormat);
    int i, j;

    for(i=0; i<10;i++)
    {
        for(j=0; j<10;j++)
        {
            conf.beginGroup("frame-"+QString::number(i)+"-"+QString::number(j));
            conf.setValue("height", framesX[i][j].H);
            conf.setValue("width", framesX[i][j].W);
            conf.setValue("offsetX", framesX[i][j].offsetX);
            conf.setValue("offsetY", framesX[i][j].offsetY);
            conf.setValue("used", framesX[i][j].used);
            conf.endGroup();
        }
    }

    setSpriteAniData(conf);

    QMessageBox::information(this, tr("Saved"), tr("Configuration saved in file\n")+ini_sprite);
}
