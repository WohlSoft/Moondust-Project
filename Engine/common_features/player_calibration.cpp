/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
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

#include <QSettings>
#include <QFileInfo>
#include <QDir>

#include "player_calibration.h"


void obj_player_calibration::init(int x, int y)
{
    QList<frameOpts > framesY;
    frameOpts spriteData;
    // Write default values
    for(int i=0; i<x; i++)
    {
        framesY.clear();
        for(int j=0; j<y; j++)
        {
            framesY.push_back(spriteData);
        }
        framesX.push_back(framesY);
    }
}

bool obj_player_calibration::load(QString fileName)
{
    QFileInfo ourFile(fileName);
    QString ini_sprite;
    //Load Customized
    if(QFileInfo(ourFile.absoluteDir().path()+"/"+ourFile.baseName() + ".ini").exists())
        ini_sprite = ourFile.absoluteDir().path()+"/"+ourFile.baseName() + ".ini";
    else
        return false;
    QSettings conf(ini_sprite, QSettings::IniFormat);

    conf.beginGroup("common");
        frameWidth = conf.value("width", "-1").toInt();
        frameHeight = conf.value("height", "-1").toInt();
        frameHeightDuck = conf.value("height-duck", "-1").toInt();
        frameGrabOffsetX = conf.value("grab-offset-x", "0").toInt();
        frameGrabOffsetY = conf.value("grab-offset-y", "0").toInt();
        frameOverTopGrab = conf.value("over-top-grab", "false").toBool();
    conf.endGroup();
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
            framesX[i][j].used = conf.value("used", "false").toBool();
            framesX[i][j].isDuck = conf.value("duck", "false").toBool();
            framesX[i][j].isRightDir = conf.value("isRightDir", "false").toBool();
            framesX[i][j].showGrabItem = conf.value("showGrabItem", "false").toBool();
            conf.endGroup();

            if(frameWidth < 0)
            {
                if(framesX[i][j].used)
                    frameWidth=framesX[i][j].W;
            }
            if(frameHeight < 0)
            {
                if(framesX[i][j].used)
                    frameHeight=framesX[i][j].H;
            }
            if(frameHeightDuck < 0)
            {
                if(framesX[i][j].used)
                    frameHeightDuck=framesX[i][j].H;
            }
        }
    }

    AniFrames.set.clear();
    //get Animation frameSets
    getSpriteAniData(conf, "Idle");
    getSpriteAniData(conf, "Run");
    getSpriteAniData(conf, "JumpFloat");
    getSpriteAniData(conf, "JumpFall");
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
    getSpriteAniData(conf, "SwimUp");
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
    return true;
}

void obj_player_calibration::getSpriteAniData(QSettings &set, QString name)
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
    AniFrames.set.push_back(frameSet);
}
