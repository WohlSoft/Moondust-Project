/*
 * SMBX64 Playble Character Sprite Calibrator, a free tool for playable srite design
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

#include <QPixmap>
#include <QtDebug>
#include <QSettings>
#include <QFileDialog>

#include "calibrationmain.h"
#include "ui_calibrationmain.h"
#include "about/about.h"
#include "frame_matrix/matrix.h"
#include "animator/animate.h"
#include "animator/aniFrames.h"
#include "main/globals.h"
#include "main/app_path.h"


void CalibrationMain::createDirs()
{
    if(!QDir(ApplicationPath + "/calibrator/spriteconf").exists())
        QDir().mkpath(ApplicationPath + "/calibrator/spriteconf");
}

void CalibrationMain::loadConfig(Calibration &dst, QString fileName, Calibration *merge_with)
{
    createDirs();
    m_currentConfig = fileName;
    QFileInfo ourFile(fileName);
    QString ini_sprite;
    //Load Customized
    if(QFileInfo(ourFile.absoluteDir().path() + "/" + ourFile.baseName() + ".ini").exists())
        //Load Customized
        ini_sprite = ourFile.absoluteDir().path() + "/" + ourFile.baseName() + ".ini";
    else
        //Load Default
        ini_sprite = ApplicationPath + "/calibrator/spriteconf/" + ourFile.baseName() + ".ini";
    QSettings conf(ini_sprite, QSettings::IniFormat);

    conf.beginGroup("common");
    {
        dst.frameWidth = conf.value("width", -1).toInt();
        dst.frameHeight = conf.value("height", -1).toInt();
        dst.frameHeightDuck = conf.value("height-duck", -1).toInt();
        dst.frameGrabOffsetX = conf.value("grab-offset-x", 0).toInt();
        dst.frameGrabOffsetY = conf.value("grab-offset-y", 0).toInt();
        dst.frameOverTopGrab = conf.value("over-top-grab", false).toBool();
    }
    conf.endGroup();

    int usedCount = 0;

    for(int x = 0; x < 10; x++)
    {
        for(int y = 0; y < 10; y++)
        {
            QString q = "frame-" + QString::number(x) + "-" + QString::number(y);
            CalibrationFrame frame;// = g_framesX[i][j];
            conf.beginGroup(q);
            {
                frame.used = !conf.allKeys().isEmpty();
                frame.H = conf.value("height", 100).toUInt();
                frame.W = conf.value("width", 100).toUInt();
                frame.offsetX = conf.value("offsetX", 0).toInt();
                frame.offsetY = conf.value("offsetY", 0).toInt();
                frame.isDuck = conf.value("duck", false).toBool();
                frame.isRightDir = conf.value("isRightDir", false).toBool();
                frame.showGrabItem = conf.value("showGrabItem", false).toBool();
            }
            conf.endGroup();

            if(frame.used)
                usedCount++;

            if(dst.frameWidth < 0)
            {
                if(frame.used)
                    dst.frameWidth = static_cast<int>(frame.W);
            }
            if(dst.frameHeight < 0)
            {
                if(frame.used)
                    dst.frameHeight = static_cast<int>(frame.H);
            }
            if(dst.frameHeightDuck < 0)
            {
                if(frame.used)
                    dst.frameHeightDuck = static_cast<int>(frame.H);
            }
            dst.frames.insert({x, y}, frame);
        }
    }

    if(usedCount == 0) // Default config when nothing was before this
    {
        dst.frameWidth = 25;
        dst.frameHeight = 50;
        dst.frameHeightDuck = 25;
    }

    dst.animations.clear();

    QStringList a = conf.childGroups();
    QSet<QString> animations;

    QRegExp aniKey("Animation([A-Za-z]+)_[LR]");

    for(auto &gn : a)
    {
        if(aniKey.exactMatch(gn))
        {
            animations.insert(aniKey.cap(1));
        }
    }

    for(const auto &a : animations)
        getSpriteAniData(dst, conf, a);

    //get Animation frameSets
//    getSpriteAniData(conf, "Idle");
//    getSpriteAniData(conf, "Run");
//    getSpriteAniData(conf, "JumpFloat");
//    getSpriteAniData(conf, "JumpFall");
//    getSpriteAniData(conf, "SpinJump");
//    getSpriteAniData(conf, "Sliding");
//    getSpriteAniData(conf, "Climbing");
//    getSpriteAniData(conf, "Fire");
//    getSpriteAniData(conf, "SitDown");
//    getSpriteAniData(conf, "Dig");
//    getSpriteAniData(conf, "GrabIdle");
//    getSpriteAniData(conf, "GrabRun");
//    getSpriteAniData(conf, "GrabJump");
//    getSpriteAniData(conf, "GrabSitDown");
//    getSpriteAniData(conf, "RacoonRun");
//    getSpriteAniData(conf, "RacoonFloat");
//    getSpriteAniData(conf, "RacoonFly");
//    getSpriteAniData(conf, "RacoonFlyDown");
//    getSpriteAniData(conf, "RacoonTail");
//    getSpriteAniData(conf, "Swim");
//    getSpriteAniData(conf, "SwimUp");
//    getSpriteAniData(conf, "OnYoshi");
//    getSpriteAniData(conf, "OnYoshiSit");
//    getSpriteAniData(conf, "PipeUpDown");
//    getSpriteAniData(conf, "PipeUpDownRear");
//    getSpriteAniData(conf, "SlopeSlide");
//    getSpriteAniData(conf, "TanookiStatue");
//    getSpriteAniData(conf, "SwordAttak");
//    getSpriteAniData(conf, "JumpSwordUp");
//    getSpriteAniData(conf, "JumpSwordDown");
//    getSpriteAniData(conf, "DownSwordAttak");
//    getSpriteAniData(conf, "Hurted");

}

void CalibrationMain::getSpriteAniData(Calibration &dst, QSettings &set, QString name)
{
    AniFrameSet frameSet;
    AniFrame frameXY;
    int frameTotal, i;

    set.beginGroup("Animation" + name + "_L");
    frameTotal = set.value("frames", "0").toInt();
    for(i = 0; i < frameTotal; i++)
    {
        frameXY.x = set.value(QString("frame" + QString::number(i) + "x"), "0").toInt();
        frameXY.y = set.value(QString("frame" + QString::number(i) + "y"), "0").toInt();
        frameSet.L.push_back(frameXY);
    }
    set.endGroup();

    set.beginGroup("Animation" + name + "_R");
    frameTotal = set.value("frames", "0").toInt();
    for(i = 0; i < frameTotal; i++)
    {
        frameXY.x = set.value(QString("frame" + QString::number(i) + "x"), "0").toInt();
        frameXY.y = set.value(QString("frame" + QString::number(i) + "y"), "0").toInt();
        frameSet.R.push_back(frameXY);
    }
    set.endGroup();
    frameSet.name = name;

    dst.animations.insert(frameSet.name, frameSet);
}


void CalibrationMain::setSpriteAniData(Calibration &src, QSettings &set)
{
    int i;

    for(auto &cal : src.animations)
    {
        if(cal.L.size() > 0)
        {
            set.beginGroup("Animation" + cal.name + "_L");
            set.setValue("frames", cal.L.size());
            for(i = 0; i < cal.L.size(); i++)
            {
                set.setValue("frame" + QString::number(i) + "x", cal.L[i].x);
                set.setValue("frame" + QString::number(i) + "y", cal.L[i].y);
            }
            set.endGroup();
        }

        if(cal.R.size() > 0)
        {
            set.beginGroup("Animation" + cal.name + "_R");
            set.setValue("frames", cal.R.size());
            for(i = 0; i < cal.R.size(); i++)
            {
                set.setValue("frame" + QString::number(i) + "x", cal.R[i].x);
                set.setValue("frame" + QString::number(i) + "y", cal.R[i].y);
            }
            set.endGroup();
        }
    }
}


bool CalibrationMain::saveConfig(Calibration &src, QString fileName, bool customPath)
{
    createDirs();
    QFileInfo ourFile(fileName);
    QString ini_sprite;
    ini_sprite = ourFile.absoluteDir().path() + "/" + ourFile.baseName() + ".ini";

    if(customPath)
    {
        ini_sprite = QFileDialog::getSaveFileName(this, "Save calibration settings", ini_sprite, "*.ini");
        if(ini_sprite.isEmpty())
            return false;
    }

    //ini_sprite = ApplicationPath + "/calibrator/spriteconf/" + ourFile.baseName() + ".ini";
    QSettings conf(ini_sprite, QSettings::IniFormat);

    conf.clear();

    conf.beginGroup("common");
    {
        conf.setValue("width", src.frameWidth);
        conf.setValue("height", src.frameHeight);
        conf.setValue("height-duck", src.frameHeightDuck);
        conf.setValue("grab-offset-x", src.frameGrabOffsetX);
        conf.setValue("grab-offset-y", src.frameGrabOffsetY);
        conf.setValue("over-top-grab", src.frameOverTopGrab);
    }
    conf.endGroup();

    for(auto it = src.frames.begin(); it != src.frames.end(); ++it)
    {
        auto pos = it.key();
        auto &frame = it.value();

        if(!frame.used)
            continue;

        conf.beginGroup("frame-" + QString::number(pos.first) + "-" + QString::number(pos.second));
        // Don't save width and height parameters!
        //conf.setValue("height", (framesX[i][j].isDuck?frameHeightDuck:frameHeight));
        //conf.setValue("width", frameWidth);
        conf.setValue("offsetX", frame.offsetX);
        conf.setValue("offsetY", frame.offsetY);
        // This filed is no longer useful, the "usage" logic will use presence of the frame section in the file, however
        conf.setValue("used", frame.used); // Keep it writing for backward compatibility with other applications
        if(frame.isDuck)
            conf.setValue("duck", frame.isDuck);
        if(frame.isRightDir)
            conf.setValue("isRightDir", frame.isRightDir);
        if(frame.showGrabItem)
            conf.setValue("showGrabItem", frame.showGrabItem);
        conf.endGroup();
    }

    setSpriteAniData(src, conf);

    QMessageBox::information(this, tr("Saved"), tr("Configuration saved in file") + "\n" + ini_sprite);

    return true;
}
