/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2016 Vitaly Novichkov <admin@wohlnet.ru>
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

void obj_player_calibration::init(size_t x, size_t y)
{
    size_t newSize = x * y;
    frames.resize(newSize);

    // Write default values
    for(size_t i = 0; i < newSize; i++)
        frames[i] = frameOpts();
}

bool obj_player_calibration::load(QString fileName)
{
    QFileInfo ourFile(fileName);
    QString ini_sprite;

    //Load Customized
    if(QFileInfo(ourFile.absoluteDir().path() + "/" + ourFile.baseName() + ".ini").exists())
        ini_sprite = ourFile.absoluteDir().path() + "/" + ourFile.baseName() + ".ini";
    else
        return false;

    QSettings conf(ini_sprite, QSettings::IniFormat);
    conf.beginGroup("common");
    matrixWidth =       static_cast<size_t>(conf.value("matrix-width",  10u).toUInt());
    matrixHeight =      static_cast<size_t>(conf.value("matrix-height", 10u).toUInt());
    frameWidth =        conf.value("width", -1).toInt();
    frameHeight =       conf.value("height", -1).toInt();
    frameHeightDuck =   conf.value("height-duck", -1).toInt();
    frameGrabOffsetX =  conf.value("grab-offset-x", 0).toInt();
    frameGrabOffsetY =  conf.value("grab-offset-y", 0).toInt();
    frameOverTopGrab =  conf.value("over-top-grab", false).toBool();
    conf.endGroup();
    init(matrixWidth, matrixHeight);
    size_t x, y;

    for(x = 0; x < matrixWidth; x++)
    {
        for(y = 0; y < matrixHeight; y++)
        {
            frameOpts &f = frame(x, y);
            conf.beginGroup(QString("frame-%1-%2").arg(x).arg(y));
            f.H = conf.value("height", 100).toUInt();
            f.W = conf.value("width", 100).toUInt();
            f.offsetX = conf.value("offsetX", 0).toInt();
            f.offsetY = conf.value("offsetY", 0).toInt();
            f.used = conf.value("used", false).toBool();
            f.isDuck = conf.value("duck", false).toBool();
            f.isRightDir = conf.value("isRightDir", false).toBool();
            f.showGrabItem = conf.value("showGrabItem", false).toBool();
            conf.endGroup();

            if(frameWidth < 0)
            {
                if(f.used)
                    frameWidth = static_cast<int>(f.W);
            }

            if(frameHeight < 0)
            {
                if(f.used)
                    frameHeight = static_cast<int>(f.H);
            }

            if(frameHeightDuck < 0)
            {
                if(f.used)
                    frameHeightDuck = static_cast<int>(f.H);
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
    getSpriteAniData(conf, "GrabIdle");
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
    getSpriteAniData(conf, "PipeUpDownRear");
    getSpriteAniData(conf, "SlopeSlide");
    getSpriteAniData(conf, "TanookiStatue");
    getSpriteAniData(conf, "SwordAttak");
    getSpriteAniData(conf, "JumpSwordUp");
    getSpriteAniData(conf, "JumpSwordDown");
    getSpriteAniData(conf, "DownSwordAttak");
    getSpriteAniData(conf, "Hurted");
    return true;
}

frameOpts &obj_player_calibration::frame(size_t x, size_t y)
{
    size_t get = (matrixWidth * y) + x;

    if(get >= frames.size())
        throw;

    return frames[get];
}

void obj_player_calibration::getSpriteAniData(QSettings &set, QString name)
{
    AniFrameSet frameSet;
    AniFrame frameXY;
    int frameTotal, i;
    set.beginGroup(QString("Animation%1_L").arg(name));
    frameTotal = set.value("frames", 0).toInt();

    for(i = 0; i < frameTotal; i++)
    {
        frameXY.x = static_cast<size_t>(set.value(QString("frame%1x").arg(i), 0).toUInt());
        frameXY.y = static_cast<size_t>(set.value(QString("frame%1y").arg(i), 0).toUInt());
        frameSet.L.push_back(frameXY);
    }

    set.endGroup();
    set.beginGroup("Animation" + name + "_R");
    frameTotal = set.value("frames", 0).toInt();

    for(i = 0; i < frameTotal; i++)
    {
        frameXY.x = static_cast<size_t>(set.value(QString("frame%1x").arg(i), 0).toUInt());
        frameXY.y = static_cast<size_t>(set.value(QString("frame%1y").arg(i), 0).toUInt());
        frameSet.R.push_back(frameXY);
    }

    set.endGroup();
    frameSet.name = name;
    AniFrames.set.push_back(frameSet);
}
