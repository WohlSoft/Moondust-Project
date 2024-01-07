/*
 * Moondust, a free game engine for platform game making
 * Copyright (c) 2014-2024 Vitaly Novichkov <admin@wohlnet.ru>
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

#ifndef PLAYER_CALIBRATION_H
#define PLAYER_CALIBRATION_H

class IniProcessing;
#include <vector>
#include <string>
#include <map>
#include <utility>
#include <stdint.h>
#include <stddef.h>

struct AniFrame
{
    size_t x;
    size_t y;
};

struct AniFrameSet
{
    std::vector<AniFrame > L;
    std::vector<AniFrame > R;
    std::string name;
};

struct FrameSets
{
    std::vector<AniFrameSet > set;
};

struct CalibrationFrame
{
    CalibrationFrame():
        h(0),
        w(0),
        offsetX(0),
        offsetY(0),
        used(false),
        isDuck(false),
        isRightDir(false),
        showGrabItem(false)
    {}
    unsigned int h;
    unsigned int w;
    int offsetX;
    int offsetY;
    bool used;
    bool isDuck;
    bool isRightDir;
    bool showGrabItem;
};

struct PlayerCalibration
{
    size_t matrixWidth;
    size_t matrixHeight;
    int frameWidth;
    int frameHeight;
    int frameHeightDuck;
    int grabOffsetX;
    int grabOffsetY;
    int grabOverTop;

    typedef std::pair<size_t /*x*/, size_t /*y*/> FramePos;
    typedef std::map<FramePos, CalibrationFrame> FramesSet;
    typedef std::map<std::string, AniFrameSet> AnimationSet;

    //! Collision boxes settings
    FramesSet frames;
    AnimationSet animations;       //!< Animation settings

    //! Grabber which allows take frame by X and Y
    CalibrationFrame &frame(size_t x, size_t y, bool *ok = nullptr);
    void init(size_t x, size_t y);
    bool load(std::string fileName, PlayerCalibration *merge_with = nullptr);

private:
    void getSpriteAniData(IniProcessing &set, const std::string &name);
    void fillDefaultAniData();
};

#endif // PLAYER_CALIBRATION_H
