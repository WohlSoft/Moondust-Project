/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2017 Vitaly Novichkov <admin@wohlnet.ru>
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

#include <fmt/fmt_format.h>
#include <IniProcessor/ini_processing.h>
#include <Utils/files.h>
#include <DirManager/dirman.h>
#include <common_features/logger.h>
#include "player_calibration.h"

void obj_player_calibration::init(size_t x, size_t y)
{
    size_t newSize = x * y;
    frames.resize(newSize);

    // Write default values
    for(size_t i = 0; i < newSize; i++)
        frames[i] = frameOpts();
}

bool obj_player_calibration::load(std::string fileName)
{
    try
    {
        std::string folderPath  = DirMan(fileName).absolutePath();
        std::string baseName    = Files::basename(fileName);

        std::string ini_sprite  = folderPath + "/" + baseName + ".ini";
        std::string group;

        //Load Customized
        if(!Files::fileExists(ini_sprite))
            return false;

        IniProcessing conf(ini_sprite);
        conf.beginGroup("common");
        {
            conf.read("matrix-width",  matrixWidth,  10u);
            conf.read("matrix-height", matrixHeight, 10u);
            conf.read("width", frameWidth, -1);
            conf.read("height", frameHeight, -1);
            conf.read("height-duck", frameHeightDuck, -1);
            conf.read("grab-offset-x", frameGrabOffsetX, 0);
            conf.read("grab-offset-y", frameGrabOffsetY, 0);
            conf.read("over-top-grab", frameOverTopGrab, false);
        }
        conf.endGroup();
        init(matrixWidth, matrixHeight);
        size_t x, y;

        for(x = 0; x < matrixWidth; x++)
        {
            for(y = 0; y < matrixHeight; y++)
            {
                frameOpts &f = frame(x, y);
                group = fmt::format("frame-{0}-{1}", x, y);
                //sprintf(group, "frame-%lu-%lu", (unsigned long)x, (unsigned long)y);
                conf.beginGroup(group.c_str());
                {
                    conf.read("height", f.H, 100);
                    conf.read("width", f.W, 100);
                    conf.read("offsetX", f.offsetX, 0);
                    conf.read("offsetY", f.offsetY, 0);
                    conf.read("used", f.used, false);
                    conf.read("duck", f.isDuck, false);
                    conf.read("isRightDir", f.isRightDir, false);
                    conf.read("showGrabItem", f.showGrabItem, false);
                }
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
        try
        {
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
        }
        catch(std::exception &e)
        {
            pLogCritical("Caugh std::exception exception: %s", e.what());
            return false;
        }
        catch(...)
        {
            return false;
        }
    }
    catch (fmt::FormatError e)
    {
        pLogCritical("Caugh fmt::FormatError exception: %s", e.what());
        return false;
    }
    catch(...)
    {
        pLogCritical("Caugh unknown exception!");
        return false;
    }
    return true;
}

frameOpts &obj_player_calibration::frame(size_t x, size_t y)
{
    size_t get = (matrixWidth * y) + x;

    if(get >= frames.size())
        throw;

    return frames[get];
}

void obj_player_calibration::getSpriteAniData(IniProcessing &set, const char *name)
{
    AniFrameSet frameSet;
    AniFrame frameXY;
    size_t frameTotal, i;
    std::string group[2];
    std::vector<AniFrame > *frameSets[2] = {&frameSet.L, &frameSet.R};

    group[0] = fmt::format("Animation{0}_L", name);
    group[1] = fmt::format("Animation{0}_R", name);

    for(int side = 0; side < 2; side++)
    {
        set.beginGroup(group[side].c_str());
        {
            set.read("frames", frameTotal, 0);
            frameSets[side]->reserve(frameTotal);
            for(i = 0; i < frameTotal; i++)
            {
                set.read(fmt::format("frame{0}x", i).c_str(), frameXY.x, 0);
                set.read(fmt::format("frame{0}y", i).c_str(), frameXY.y, 0);
                frameSets[side]->push_back(frameXY);
            }
        }
        set.endGroup();
    }

    frameSet.name = name;
    AniFrames.set.push_back(frameSet);
}
