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

void CalibrationMain::loadConfig(QString fileName)
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
        g_frameWidth = conf.value("width", -1).toInt();
        g_frameHeight = conf.value("height", -1).toInt();
        g_frameHeightDuck = conf.value("height-duck", -1).toInt();
        g_frameGrabOffsetX = conf.value("grab-offset-x", 0).toInt();
        g_frameGrabOffsetY = conf.value("grab-offset-y", 0).toInt();
        g_frameOverTopGrab = conf.value("over-top-grab", false).toBool();
    }
    conf.endGroup();
    int i, j, usedCount = 0;

    for(i = 0; i < 10; i++)
    {
        for(j = 0; j < 10; j++)
        {
            QString q = "frame-" + QString::number(i) + "-" + QString::number(j);
            auto &frame = g_framesX[i][j];
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

            if(g_frameWidth < 0)
            {
                if(frame.used)
                    g_frameWidth = static_cast<int>(frame.W);
            }
            if(g_frameHeight < 0)
            {
                if(frame.used)
                    g_frameHeight = static_cast<int>(frame.H);
            }
            if(g_frameHeightDuck < 0)
            {
                if(frame.used)
                    g_frameHeightDuck = static_cast<int>(frame.H);
            }
        }
    }

    if(usedCount == 0) // Default config when nothing was before this
    {
        g_frameWidth = 25;
        g_frameHeight = 50;
        g_frameHeightDuck = 25;
    }

    g_aniFrames.set.clear();
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
    getSpriteAniData(conf, "RacoonFlyDown");
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

}

void CalibrationMain::getSpriteAniData(QSettings &set, QString name)
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
    g_aniFrames.set.push_back(frameSet);
}


void CalibrationMain::setSpriteAniData(QSettings &set)
{
    int i, j;

    for(j = 0; j < g_aniFrames.set.size(); j++)
    {
        if(g_aniFrames.set[j].L.size() > 0)
        {
            set.beginGroup("Animation" + g_aniFrames.set[j].name + "_L");
            set.setValue("frames", g_aniFrames.set[j].L.size());
            for(i = 0; i < g_aniFrames.set[j].L.size(); i++)
            {
                set.setValue("frame" + QString::number(i) + "x", g_aniFrames.set[j].L[i].x);
                set.setValue("frame" + QString::number(i) + "y", g_aniFrames.set[j].L[i].y);
            }
            set.endGroup();
        }

        if(g_aniFrames.set[j].R.size() > 0)
        {
            set.beginGroup("Animation" + g_aniFrames.set[j].name + "_R");
            set.setValue("frames", g_aniFrames.set[j].R.size());
            for(i = 0; i < g_aniFrames.set[j].R.size(); i++)
            {
                set.setValue("frame" + QString::number(i) + "x", g_aniFrames.set[j].R[i].x);
                set.setValue("frame" + QString::number(i) + "y", g_aniFrames.set[j].R[i].y);
            }
            set.endGroup();
        }
    }
}


bool CalibrationMain::saveConfig(QString fileName, bool customPath)
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
    int x, y;

    conf.clear();

    conf.beginGroup("common");
    {
        conf.setValue("width", g_frameWidth);
        conf.setValue("height", g_frameHeight);
        conf.setValue("height-duck", g_frameHeightDuck);
        conf.setValue("grab-offset-x", g_frameGrabOffsetX);
        conf.setValue("grab-offset-y", g_frameGrabOffsetY);
        conf.setValue("over-top-grab", g_frameOverTopGrab);
    }
    conf.endGroup();

    for(x = 0; x < 10; x++)
    {
        for(y = 0; y < 10; y++)
        {
            if(g_framesX[x][y].used)
            {
                conf.beginGroup("frame-" + QString::number(x) + "-" + QString::number(y));
                // Don't save width and height parameters!
                //conf.setValue("height", (framesX[i][j].isDuck?frameHeightDuck:frameHeight));
                //conf.setValue("width", frameWidth);
                conf.setValue("offsetX", g_framesX[x][y].offsetX);
                conf.setValue("offsetY", g_framesX[x][y].offsetY);
                // Don't write this field anymire, the "usage" logic will use presence of the field
                // conf.setValue("used", g_framesX[x][y].used);
                if(g_framesX[x][y].isDuck) conf.setValue("duck", g_framesX[x][y].isDuck);
                if(g_framesX[x][y].isRightDir) conf.setValue("isRightDir", g_framesX[x][y].isRightDir);
                if(g_framesX[x][y].showGrabItem) conf.setValue("showGrabItem", g_framesX[x][y].showGrabItem);
                conf.endGroup();
            }
        }
    }

    setSpriteAniData(conf);

    QMessageBox::information(this, tr("Saved"), tr("Configuration saved in file") + "\n" + ini_sprite);

    return true;
}
