/*
 * Playble Character Calibrator, a free tool for playable character sprite setup tune
 * This is a part of the Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2017-2021 Vitaly Novichkov <admin@wohlnet.ru>
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

#include "calibration_main.h"
#include "ui_calibration_main.h"
#include "about/about.h"
#include "frame_matrix/matrix.h"
#include "animator/animator.h"
#include "main/app_path.h"
#include "main/calibration.h"

#include "qfile_dialogs_default_options.hpp"


template <class T>
inline const T &pgeConstReference(const T &t)
{
    return t;
}

void CalibrationMain::createDirs()
{
    if(!QDir(ApplicationPath + "/calibrator/spriteconf").exists())
        QDir().mkpath(ApplicationPath + "/calibrator/spriteconf");
}

void CalibrationMain::loadConfig(Calibration &dst, QString fileName, Calibration *merge_with)
{
#define pMerge(param, def) (merge_with ? pgeConstReference(merge_with->param) : pgeConstReference(def))
    createDirs();

    QSettings conf(fileName, QSettings::IniFormat);

    conf.beginGroup("common");
    {
        dst.frameWidth = conf.value("width", pMerge(frameWidth, -1)).toInt();
        dst.frameHeight = conf.value("height", pMerge(frameHeight, -1)).toInt();
        dst.frameHeightDuck = conf.value("height-duck", pMerge(frameHeightDuck, -1)).toInt();
        dst.grabOffsetX = conf.value("grab-offset-x", pMerge(grabOffsetX, 0)).toInt();
        dst.grabOffsetY = conf.value("grab-offset-y", pMerge(grabOffsetY, 0)).toInt();
        dst.grabOverTop = conf.value("over-top-grab", pMerge(grabOverTop, false)).toBool();
    }
    conf.endGroup();

    QStringList a = conf.childGroups();
    QSet<QString> animations;
    QSet<QPair<int, int > > framesKeys;

    QRegExp aniKey("Animation([A-Za-z]+)_[LR]");
    QRegExp frameKey("frame-(\\d+)-(\\d+)");

    for(auto &gn : a)
    {
        if(aniKey.exactMatch(gn))
            animations.insert(aniKey.cap(1));
        else if(frameKey.exactMatch(gn))
        {
            framesKeys.insert({frameKey.cap(1).toInt(), frameKey.cap(2).toInt()});
        }
    }

    int usedCount = 0;

    if(merge_with)
        dst.frames = merge_with->frames;

    for(const auto &key : framesKeys)
    {
        int x = key.first;
        int y = key.second;
        QString q = "frame-" + QString::number(x) + "-" + QString::number(y);

        CalibrationFrame frame;
        conf.beginGroup(q);
        {
            frame.used = !conf.allKeys().isEmpty();
            frame.h = conf.value("height", 100).toUInt();
            frame.w = conf.value("width", 100).toUInt();
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
                dst.frameWidth = static_cast<int>(frame.w);
        }
        if(dst.frameHeight < 0)
        {
            if(frame.used)
                dst.frameHeight = static_cast<int>(frame.h);
        }
        if(dst.frameHeightDuck < 0)
        {
            if(frame.used)
                dst.frameHeightDuck = static_cast<int>(frame.h);
        }
        dst.frames.insert({x, y}, frame);
    }

    if(usedCount == 0) // Default config when nothing was before this
    {
        dst.frameWidth = 25;
        dst.frameHeight = 50;
        dst.frameHeightDuck = 25;
    }

    dst.animations.clear();

    if(!merge_with)
        fillDefaultAniData(dst);
    else
        dst.animations = merge_with->animations;

    for(const auto &a : animations)
        loadSpriteAniEntry(dst, conf, a);

#undef pMerge
}

void CalibrationMain::fillDefaultAniData(Calibration &dst)
{
    QStringList defData =
    {
        "Idle",
        "Run",
        "JumpFloat",
        "JumpFall",
        "SpinJump",
        "Sliding",
        "Climbing",
        "Fire",
        "SitDown",
        "Dig",
        "GrabIdle",
        "GrabRun",
        "GrabJump",
        "GrabSitDown",
        "RacoonRun",
        "RacoonFloat",
        "RacoonFly",
        "RacoonFlyDown",
        "RacoonTail",
        "Swim",
        "SwimUp",
        "OnYoshi",
        "OnYoshiSit",
        "PipeUpDown",
        "PipeUpDownRear",
        "SlopeSlide",
        "TanookiStatue",
        "SwordAttak",
        "JumpSwordUp",
        "JumpSwordDown",
        "DownSwordAttak",
        "Hurted"
    };

    AniFrameSet frameSet;

    for(const auto &a : defData)
    {
        frameSet.name = a;
        dst.animations.insert(a, frameSet);
    }
}

void CalibrationMain::loadSpriteAniEntry(Calibration &dst, QSettings &set, QString name)
{
    AniFrameSet frameSet;
    AniFrame frameXY;
    int frameTotal, i;

    set.beginGroup("Animation" + name + "_L");
    frameTotal = set.value("frames", 0).toInt();
    for(i = 0; i < frameTotal; i++)
    {
        frameXY.x = set.value(QString("frame" + QString::number(i) + "x"), "0").toInt();
        frameXY.y = set.value(QString("frame" + QString::number(i) + "y"), "0").toInt();
        frameSet.L.push_back(frameXY);
    }
    set.endGroup();

    set.beginGroup("Animation" + name + "_R");
    frameTotal = set.value("frames", 0).toInt();
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


void CalibrationMain::saveSpriteAniData(Calibration &src, QSettings &set, Calibration *merge_with)
{
    int i;

    for(auto &cal : src.animations)
    {
        if(merge_with)
        {
            if(merge_with->animations.contains(cal.name) &&
               merge_with->animations[cal.name] == cal)
                continue; // Don't save entry if it does match to the default
        }

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


bool CalibrationMain::saveConfig(Calibration &src, QString fileName, bool customPath, Calibration *merge_with)
{
    createDirs();

    QFileInfo ourFile(fileName);
    QString ini_sprite;
    ini_sprite = ourFile.absoluteDir().path() + "/" + ourFile.baseName() + ".ini";

    if(customPath)
    {
        ini_sprite = QFileDialog::getSaveFileName(this, "Save calibration settings", ini_sprite, "*.ini", nullptr, c_fileDialogOptions);
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
        conf.setValue("grab-offset-x", src.grabOffsetX);
        conf.setValue("grab-offset-y", src.grabOffsetY);
        conf.setValue("over-top-grab", src.grabOverTop);
    }
    conf.endGroup();

    for(auto it = src.frames.begin(); it != src.frames.end(); ++it)
    {
        auto pos = it.key();
        auto &frame = it.value();

        if(merge_with)
        {
            if(merge_with->frames.contains(pos) &&
               merge_with->frames[pos] == frame)
                continue;
        }

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

    saveSpriteAniData(src, conf, merge_with);

    QMessageBox::information(this, tr("Saved"), tr("Configuration saved in file") + "\n" + ini_sprite);

    return true;
}

static const QStringList s_charNames =
{
    "mario",
    "luigi",
    "peach",
    "toad",
    "link"
};

static int nameToInt(const QString &n)
{
    int ret = s_charNames.indexOf(n.toLower());
    if(ret < 0)
        return 0;
    return ret;
}

struct SanBaEiLevelFile
{
    QStringList lines;

    bool readFile(const QString &fileName)
    {
        lines.clear();
        QFile f(fileName);
        if(!f.open(QIODevice::ReadOnly|QIODevice::Text))
            return false; // Can't open file, damn

        QString d = QString::fromUtf8(f.readAll());
        d.remove('\r');
        lines = d.split('\n', QString::SkipEmptyParts);

        if(lines.isEmpty())
            return false;

        if(!lines.first().startsWith("SMBXFile"))
        {
            lines.clear();
            return false;
        }

        return true;
    }

    bool writeFile(const QString &fileName)
    {
        QFile f(fileName);
        if(!f.open(QIODevice::WriteOnly))
            return false; // Can't open file, damn

        for(auto &l : lines)
        {
            QByteArray o = l.toUtf8();
            f.write(o);
            f.write("\r\n", 2);
        }

        f.close();

        return true;
    }
};

bool CalibrationMain::importFrom38A(Calibration &dst, QString imageName, QString fileName)
{
    QRegExp fileNameReg("(\\w+)-(\\d).png");
    if(!fileNameReg.exactMatch(imageName))
        return false; // Unknown character

    int charId = nameToInt(fileNameReg.cap(1));
    int stateId = fileNameReg.cap(2).toInt();

    SanBaEiLevelFile lev;

    if(!lev.readFile(fileName))
        return false;

    bool hasData = false;

    for(auto &l : lev.lines)
    {
        if(!l.startsWith("PX"))
            continue;

        int character;
        int state;
        auto lData = l.split('|');
        lData.pop_front();
        if(lData.isEmpty())
            continue; // Invalid
        auto idx = lData.first().split(',');
        if(idx.size() != 2)
            continue; // invalid
        lData.pop_front();

        character   = idx[0].toInt();
        state       = idx[1].toInt();

        if((charId != character) || (state != stateId))
            continue; // Not for this character!

        for(auto &s : lData)
        {
            auto e = s.split(',');
            if(e.size() < 4)
                continue; // Invalid block

            CalibrationFrame frame;
            frame.used = true;
            int x = e[0].toInt();
            int y = e[1].toInt();
            frame.offsetX = -(e[2].toInt() - 64);
            frame.offsetY = -(e[3].toInt() - 64);

            Calibration::FramePos pos = {x, y};

            if(dst.frames.contains(pos))
            {
                auto &f = dst.frames[pos];
                frame.isDuck = f.isDuck;
                frame.isRightDir = f.isRightDir;
                frame.showGrabItem = f.showGrabItem;
            }

            dst.frames.insert({x, y}, frame);
            hasData = true;
        }
    }

    return hasData;
}

bool CalibrationMain::exportTo38A(Calibration &src, QString imageName, QString fileName)
{
    QRegExp fileNameReg("(\\w+)-(\\d).png");
    if(!fileNameReg.exactMatch(imageName))
        return false; // Unknown character

    int charId = nameToInt(fileNameReg.cap(1));
    int stateId = fileNameReg.cap(2).toInt();

    SanBaEiLevelFile lev;

    if(!lev.readFile(fileName))
        return false;

    QString outputLine;

    outputLine = QString("PX|%1,%2").arg(charId).arg(stateId);

    for(auto it = src.frames.begin(); it != src.frames.end(); ++it)
    {
        auto pos = it.key();
        auto &frame = it.value();
        if(!frame.used)
            continue;

        outputLine += QString("|%1,%2,%3,%4")
                .arg(pos.first)
                .arg(pos.second)
                .arg(-frame.offsetX + 64)
                .arg(-frame.offsetY + 64);

    }

    bool hasLine = false;

    for(auto &l : lev.lines)
    {
        if(!l.startsWith("PX"))
            continue;

        int character;
        int state;
        auto lData = l.split('|');
        lData.pop_front();
        if(lData.isEmpty())
            continue; // Invalid
        auto idx = lData.first().split(',');
        if(idx.size() != 2)
            continue; // invalid
        lData.pop_front();

        character   = idx[0].toInt();
        state       = idx[1].toInt();

        if((charId != character) || (state != stateId))
            continue; // Not for this character!

        l = outputLine;

        hasLine = true;
    }

    if(!hasLine)
        lev.lines.push_back(outputLine);

    return lev.writeFile(fileName);
}
