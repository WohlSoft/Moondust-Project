/*
 * Moondust, a free game engine for platform game making
 * Copyright (c) 2014-2020 Vitaly Novichkov <admin@wohlnet.ru>
 *
 * This software is licensed under a dual license system (MIT or GPL version 3 or later).
 * This means you are free to choose with which of both licenses (MIT or GPL version 3 or later)
 * you want to use this software.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *
 * You can see text of MIT license in the LICENSE.mit file you can see in Engine folder,
 * or see https://mit-license.org/.
 *
 * You can see text of GPLv3 license in the LICENSE.gpl3 file you can see in Engine folder,
 * or see <http://www.gnu.org/licenses/>.
 */

#include <regex>
#include <set>

#include <common_features/fmt_format_ne.h>
#include <IniProcessor/ini_processing.h>
#include <Utils/files.h>
#include <DirManager/dirman.h>
#include <common_features/logger.h>
#include "player_calibration.h"

template <class T>
inline const T &pgeConstReference(const T &t)
{
    return t;
}

void PlayerCalibration::init(size_t, size_t)
{
    frames.clear();
}

bool PlayerCalibration::load(std::string fileName, PlayerCalibration *merge_with)
{
#define pMerge(param, def) (merge_with ? pgeConstReference(merge_with->param) : pgeConstReference(def))
    std::string folderPath  = DirMan(Files::dirname(fileName)).absolutePath();
    std::string baseName    = Files::basenameNoSuffix(fileName);

    std::string ini_sprite  = folderPath + "/" + baseName + ".ini";
    std::string group;

    //Load Customized
    if(!Files::fileExists(ini_sprite))
        return false;

    IniProcessing conf(ini_sprite);
    conf.beginGroup("common");
    {
        conf.read("matrix-width",  matrixWidth,  pMerge(matrixWidth, 10u));
        conf.read("matrix-height", matrixHeight, pMerge(matrixHeight, 10u));
        conf.read("width", frameWidth, pMerge(frameWidth, -1));
        conf.read("height", frameHeight, pMerge(frameHeight, -1));
        conf.read("height-duck", frameHeightDuck, pMerge(frameHeightDuck, -1));
        conf.read("grab-offset-x", grabOffsetX, pMerge(grabOffsetX, 0));
        conf.read("grab-offset-y", grabOffsetY, pMerge(grabOffsetY, 0));
        conf.read("over-top-grab", grabOverTop, pMerge(grabOverTop, false));
    }
    conf.endGroup();

    if(!merge_with)
        init(matrixWidth, matrixHeight);

    std::regex aniKey("Animation([A-Za-z]+)_[LR]");
    std::regex frameKey("frame-(\\d+)-(\\d+)");
    std::vector<std::string> a = conf.childGroups();
    std::set<std::string> animationKeys;
    std::set<FramePos> framesKeys;

    for(auto &gn : a)
    {
        std::smatch res;
        if(std::regex_search(gn, res, aniKey))
            animationKeys.insert(res[1]);
        else if(std::regex_search(gn, res, frameKey))
        {
            framesKeys.insert({std::atoi(res[1].str().c_str()), std::atoi(res[2].str().c_str())});
        }
    }

    for(const auto &key : framesKeys)
    {
        int x = key.first;
        int y = key.second;
        bool ok = false;

        CalibrationFrame &f = frame(x, y, &ok);
        if(!ok)
        {
            pLogCritical("obj_player_calibration: missing frame %u x %u!", (unsigned int)x, (unsigned int)y);
            return false;
        }

        group = fmt::format_ne("frame-{0}-{1}", x, y);
        //sprintf(group, "frame-%lu-%lu", (unsigned long)x, (unsigned long)y);
        conf.beginGroup(group.c_str());
        {
            conf.read("height", f.h, 100);
            conf.read("width", f.w, 100);
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
                frameWidth = static_cast<int>(f.w);
        }

        if(frameHeight < 0)
        {
            if(f.used)
                frameHeight = static_cast<int>(f.h);
        }

        if(frameHeightDuck < 0)
        {
            if(f.used)
                frameHeightDuck = static_cast<int>(f.h);
        }
    }

    animations.clear();

    if(!merge_with)
        fillDefaultAniData();
    else
        animations = merge_with->animations;

    //get Animation frameSets
    for(const auto &a : animationKeys)
        getSpriteAniData(conf, a);

    return true;
#undef pMerge
}

CalibrationFrame &PlayerCalibration::frame(size_t x, size_t y, bool *ok)
{
    static CalibrationFrame dummy;

    if(ok)
        *ok = false;

    FramePos p(x, y);

    auto fr = frames.find(p);
    if(fr == frames.end())
    {
        frames.insert({p, dummy});
        fr = frames.find(p);
    }

    if(ok)
        *ok = true;

    return fr->second;
}

void PlayerCalibration::getSpriteAniData(IniProcessing &set, const std::string &name)
{
    AniFrameSet frameSet;
    AniFrame frameXY;
    size_t frameTotal, i;
    std::string group[2];
    std::vector<AniFrame > *frameSets[2] = {&frameSet.L, &frameSet.R};

    group[0] = fmt::format_ne("Animation{0}_L", name);
    group[1] = fmt::format_ne("Animation{0}_R", name);

    for(int side = 0; side < 2; side++)
    {
        set.beginGroup(group[side].c_str());
        {
            set.read("frames", frameTotal, 0);
            frameSets[side]->reserve(frameTotal);
            for(i = 0; i < frameTotal; i++)
            {
                set.read(fmt::format_ne("frame{0}x", i).c_str(), frameXY.x, 0);
                set.read(fmt::format_ne("frame{0}y", i).c_str(), frameXY.y, 0);
                frameSets[side]->push_back(frameXY);
            }
        }
        set.endGroup();
    }

    frameSet.name = name;
    if(animations.find(name) != animations.end())
        animations.erase(name);
    animations.insert({name, frameSet});
}

void PlayerCalibration::fillDefaultAniData()
{
    std::vector<std::string> defData =
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
        animations.insert({a, frameSet});
    }
}
