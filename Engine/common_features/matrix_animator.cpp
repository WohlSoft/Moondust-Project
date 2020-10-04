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

#include "matrix_animator.h"
#include "logger.h"
#include <gui/pge_msgbox.h>

#include <cassert>
#include <cmath>
#include <cstdlib>
#include <utility>

MatrixAnimator::MatrixAnimator():
    m_width(0.0),
    m_height(0.0),
    m_width_f(0.0),
    m_height_f(0.0),
    m_nextFrameDelay(128.0),
    m_frameDelay(128),
    m_frameDelay_once(128),
    m_currentFrameIndex(0),
    m_direction(1),
    m_once(false),
    m_once_fixed_speed(false),
    m_once_locked(false),
    m_once_play_again(false),
    m_once_play_again_skip_last_frames(0),
    m_once_play_again_direction(0),
    m_current_sequance(Idle),
    m_backup_sequance(Idle),
    m_sequenceP(nullptr)
{
    buildRect();
}

MatrixAnimator::MatrixAnimator(int width, int height):
    m_width(std::fabs(static_cast<double>(width))),
    m_height(std::fabs(static_cast<double>(height))),
    m_width_f(m_width == 0.0 ? 0.0 : (1.0 / m_width)),
    m_height_f(m_height == 0.0 ? 0.0 : (1.0 / m_height)),
    m_nextFrameDelay(128.0),
    m_frameDelay(128),
    m_frameDelay_once(128),
    m_currentFrameIndex(0),
    m_direction(1),
    m_once(false),
    m_once_fixed_speed(false),
    m_once_locked(false),
    m_once_play_again(false),
    m_once_play_again_skip_last_frames(0),
    m_once_play_again_direction(0),
    m_current_sequance(Idle),
    m_backup_sequance(Idle),
    m_sequenceP(nullptr)
{
    buildRect();
}

MatrixAnimator::MatrixAnimator(const MatrixAnimator &a):
    m_width(a.m_width),
    m_height(a.m_height),
    m_width_f(a.m_width_f),
    m_height_f(a.m_height_f),
    m_nextFrameDelay(a.m_nextFrameDelay),
    m_frameDelay(a.m_frameDelay),
    m_frameDelay_once(a.m_frameDelay_once),
    m_currentFrameIndex(a.m_currentFrameIndex),
    m_direction(a.m_direction),
    m_once(a.m_once),
    m_once_fixed_speed(a.m_once_fixed_speed),
    m_once_locked(a.m_once_locked),
    m_once_play_again(a.m_once_play_again),
    m_once_play_again_skip_last_frames(a.m_once_play_again_skip_last_frames),
    m_once_play_again_direction(a.m_once_play_again_direction),
    m_current_sequance(a.m_current_sequance),
    m_backup_sequance(a.m_backup_sequance),
    m_sequence(a.m_sequence),
    m_sequenceP(&m_sequence),
    s_bank_left(a.s_bank_left),
    s_bank_right(a.s_bank_right)
{
    setDirection(m_direction, true);
    buildRect();
}

void MatrixAnimator::setFrameSequance(std::vector<MatrixAnimatorFrame> _sequence)
{
    m_currentFrameIndex = 0;
    m_sequence.clear();
    m_sequence = std::move(_sequence);
    m_sequenceP = &m_sequence;
    buildRect();
}

void MatrixAnimator::setFrameSpeed(int speed)
{
    if(m_once && m_once_fixed_speed)
        return;

    if(m_frameDelay == speed)
        return;

    if(speed <= 0)
    {
        m_frameDelay = -1;//Stop animation if <=0
        return;
    }

    m_nextFrameDelay = ((m_frameDelay - speed) < 1) ? m_nextFrameDelay : m_nextFrameDelay - (m_frameDelay - speed);
    m_frameDelay = std::abs(speed);
}

void MatrixAnimator::setDirection(int _direction, bool force)
{
    if(force || (m_direction != _direction))
        m_direction = _direction;

    m_sequenceP = nullptr;

    if(m_direction < 0)
    {
        auto left = s_bank_left.find(m_current_sequance);
        //left
        if(left == s_bank_left.end())
        {
            pLogWarning("Missing animation sequence of playable character of type %d of left direction!",
                        static_cast<int>(m_current_sequance));
            left = s_bank_left.find(Idle);
            if(left == s_bank_left.end())
                pLogCritical("Playable character required to have the \"Idle\" animation sequence! However, it's missing!");
            SDL_assert_release(left != s_bank_left.end());
        }

        m_sequenceP = &(left->second);
    }
    else
    {
        auto right = s_bank_right.find(m_current_sequance);
        //right
        if(right == s_bank_right.end())
        {
            pLogWarning("Missing animation sequence of playable character of type %d of right direction!",
                        static_cast<int>(m_current_sequance));
            right = s_bank_right.find(Idle);
            if(right == s_bank_right.end())
                pLogCritical("Playable character required to have the \"Idle\" animation sequence! However, it's missing!");
            SDL_assert_release(right != s_bank_right.end());
        }

        m_sequenceP = &(right->second);
    }
    SDL_assert_release(m_sequenceP);
}

void MatrixAnimator::setSize(int width, int height)
{
    m_width = std::fabs(static_cast<double>(width));
    m_height = std::fabs(static_cast<double>(height));
    m_width_f = m_width == 0.0 ? 0.0 : (1.0 / m_width);
    m_height_f = m_height == 0.0 ? 0.0 : (1.0 / m_height);
}

PGE_SizeF MatrixAnimator::size()
{
    return PGE_SizeF(m_width, m_height);
}

PGE_SizeF MatrixAnimator::sizeOfFrame()
{
    return PGE_SizeF(m_width_f, m_height_f);
}

void MatrixAnimator::tickAnimation(double frametime)
{
    if((!m_once) && (m_frameDelay < 1)) return; //Idling animation

    m_nextFrameDelay -= std::fabs(frametime);

    while(m_nextFrameDelay <= 0.0)
    {
        nextFrame();

        if((!m_once) && (m_frameDelay < 1))
            break;

        m_nextFrameDelay += m_once ? m_frameDelay_once : m_frameDelay;
    }
}

void MatrixAnimator::nextFrame()
{
    if(!m_sequenceP || m_sequenceP->empty())
    {
        buildRect();
        return;
    }

    m_currentFrameIndex++;

    if(m_currentFrameIndex > (m_sequenceP->size() - 1 - m_once_play_again_skip_last_frames))
    {
        m_currentFrameIndex = 0;

        if(m_once)
        {
            if(m_once_play_again)
            {
                m_once_play_again = false;
                m_once_play_again_skip_last_frames = 0;
                setDirection(m_once_play_again_direction);
            }
            else
            {
                m_once = false;
                m_once_play_again_skip_last_frames = 0;
                switchAnimation(m_backup_sequance, m_direction, m_frameDelay);
            }
        }
    }

    buildRect();
}

void MatrixAnimator::buildRect()
{
    if(!m_sequenceP || m_sequenceP->empty())
        m_currentFrameRect.setRect(0.0, 0.0, m_width_f, m_height_f);
    else
    {
        MatrixAnimatorFrame &f = (*m_sequenceP)[m_currentFrameIndex];
        m_currentFrameRect.setLeft(f.x);
        m_currentFrameRect.setTop(f.y);
        m_currentFrameRect.setRight(f.x + m_width_f);
        m_currentFrameRect.setBottom(f.y + m_height_f);
        m_currentFrameOffsets.setX(f.offset_x);
        m_currentFrameOffsets.setY(f.offset_y);
    }
}

PGE_RectF MatrixAnimator::curFrame()
{
    return m_currentFrameRect;
}

PGE_PointF MatrixAnimator::curOffset()
{
    return m_currentFrameOffsets;
}

MatrixAnimator::MatrixAnimates MatrixAnimator::curAnimation()
{
    return m_current_sequance;
}

int MatrixAnimator::curFramespeed()
{
    return m_frameDelay;
}

bool MatrixAnimator::installAnimationSet(PlayerCalibration &calibration)
{
    s_bank_left.clear();
    s_bank_right.clear();

    for(auto &ani : calibration.animations)
    {
        AniFrameSet &fset = ani.second;
        MatrixAnimator::MatrixAnimates seq = toEnum(fset.name);

        for(size_t j = 0; j < fset.L.size(); j++)
        {
            size_t x = fset.L[j].x;
            size_t y = fset.L[j].y;

            bool ok = false;
            const CalibrationFrame &fr = calibration.frame(x, y, &ok);
            if(!ok)
            {
                pLogWarning("MatrixAnimator: missing frame %u x %u in left frameset: %s, frame index in sequence: %u",
                            (unsigned int)x, (unsigned int)y, fset.name.c_str(), (unsigned int)j);
                continue;
            }
            MatrixAnimatorFrame frame = {};
            frame.x = static_cast<double>(x) / m_width;
            frame.y = static_cast<double>(y) / m_height;
            frame.offset_x = fr.offsetX;
            frame.offset_y = fr.offsetY;
            s_bank_left[seq].push_back(frame);
        }

        for(size_t j = 0; j < fset.R.size(); j++)
        {
            size_t x = fset.R[j].x;
            size_t y = fset.R[j].y;

            bool ok = false;
            const CalibrationFrame &fr = calibration.frame(x, y, &ok);
            if(!ok)
            {
                pLogWarning("MatrixAnimator: missing frame %u x %u in right frameset: %s, frame index in sequence: %u",
                            (unsigned int)x, (unsigned int)y, fset.name.c_str(), (unsigned int)j);
                continue;
            }
            MatrixAnimatorFrame frame = {};
            frame.x = static_cast<double>(x) / m_width;
            frame.y = static_cast<double>(y) / m_height;
            frame.offset_x = fr.offsetX;
            frame.offset_y = fr.offsetY;
            s_bank_right[seq].push_back(frame);
        }
    }

    AniBank::iterator seq_left;
    AniBank::iterator seq_right;

    seq_left  = s_bank_left.find(m_current_sequance);
    seq_right = s_bank_right.find(m_current_sequance);
    bool seq_error = false;

    if(seq_left == s_bank_left.end())
    {
        m_current_sequance = Idle;
        seq_left  = s_bank_left.find(m_current_sequance);
        if(seq_left == s_bank_left.end())
        {
            pLogCritical("Idle matrix animation for left direction is not set!");
            PGE_MsgBox::error("Idle matrix animation for left direction is not set!");
            seq_error = true;
        }
    }

    if(seq_right == s_bank_right.end())
    {
        m_current_sequance = Idle;
        seq_right = s_bank_right.find(m_current_sequance);
        if(seq_right == s_bank_right.end())
        {
            pLogCritical("Idle matrix animation for right direction is not set!");
            seq_error = true;
        }
    }

    if(seq_error)
    {
        m_sequenceP = &m_sequence;
        m_currentFrameIndex = 0;
        return false;
    }

    /*Update sequence settings*/
    if(m_direction < 0)
    {
        //left
        m_sequenceP = &(seq_left->second);
    }
    else
    {
        //right
        m_sequenceP = &(seq_right->second);
    }

    SDL_assert_release(m_sequenceP);

    if(m_currentFrameIndex >= m_sequenceP->size())
    {
        m_currentFrameIndex = 0;

        if(m_once)
        {
            m_once = false;
            m_once_fixed_speed = false;
            m_once_locked = false;
            m_once_play_again_skip_last_frames = false;
            switchAnimation(m_backup_sequance, m_direction, m_frameDelay);
        }
    }

    buildRect();
    return true;
}

void MatrixAnimator::playOnce(MatrixAnimates aniName, int _direction, int speed, bool fixed_speed, bool locked, int skipLastFrames)
{
    if(m_once)
    {
        if(m_once_locked || (m_current_sequance == aniName))
        {
            if(m_current_sequance == aniName)
            {
                m_once_play_again = true;
                m_once_play_again_direction = _direction;
                m_once_play_again_skip_last_frames = (skipLastFrames >= 0) ? skipLastFrames : 0;
            }

            return;
        }
    }

    m_once_fixed_speed = fixed_speed;
    m_once_locked = locked;
    m_once_play_again = false;
    m_once_play_again_skip_last_frames = 0;
    m_once = true;
    m_frameDelay_once = (speed > 0) ? speed : 0;
    m_currentFrameIndex = 0;
    m_backup_sequance = m_current_sequance;
    m_current_sequance = aniName;
    setDirection(_direction, true);
    buildRect();
}

void MatrixAnimator::unlock()
{
    m_once_locked = false;
}

void MatrixAnimator::switchAnimation(MatrixAnimates aniName, int _direction, int speed)
{
    if(m_once)
    {
        if(m_once_locked || (m_backup_sequance == aniName))
        {
            if(!m_once_fixed_speed)
                m_frameDelay_once = (speed > 0) ? speed : 1;

            setFrameSpeed(speed);
            m_backup_sequance = aniName;
            return;
        }
        else m_backup_sequance = aniName;

        if(m_direction != _direction)
        {
            setDirection(_direction);

            if(m_currentFrameIndex >= m_sequenceP->size())
                m_currentFrameIndex = 0;
        }
    }
    else if((m_current_sequance == aniName) && (m_direction == _direction))
    {
        setFrameSpeed(speed);
        return;
    }

    m_current_sequance = aniName;
    setFrameSpeed(speed);
    setDirection(_direction);

    if((m_current_sequance != aniName) || (m_currentFrameIndex >= m_sequenceP->size()))
        m_currentFrameIndex = 0;

    m_once = false;
    buildRect();
}


void MatrixAnimator::buildEnums()
{
    m_strToEnum["Idle"] =              MatrixAnimates::Idle;
    m_strToEnum["Run"] =               MatrixAnimates::Run;
    m_strToEnum["JumpFloat"] =         MatrixAnimates::JumpFloat;
    m_strToEnum["JumpFall"] =          MatrixAnimates::JumpFall;
    m_strToEnum["SpinJump"] =          MatrixAnimates::SpinJump;
    m_strToEnum["Sliding"] =           MatrixAnimates::Sliding;
    m_strToEnum["Climbing"] =          MatrixAnimates::Climbing;
    m_strToEnum["Fire"] =              MatrixAnimates::Fire;
    m_strToEnum["SitDown"] =           MatrixAnimates::SitDown;
    m_strToEnum["Dig"] =               MatrixAnimates::Dig;
    m_strToEnum["GrabIdle"] =          MatrixAnimates::GrabIdle;
    m_strToEnum["GrabRun"] =           MatrixAnimates::GrabRun;
    m_strToEnum["GrabJump"] =          MatrixAnimates::GrabJump;
    m_strToEnum["GrabSitDown"] =       MatrixAnimates::GrabSitDown;
    m_strToEnum["RacoonRun"] =         MatrixAnimates::RacoonRun;
    m_strToEnum["RacoonFloat"] =       MatrixAnimates::RacoonFloat;
    m_strToEnum["RacoonFly"] =         MatrixAnimates::RacoonFly;
    m_strToEnum["RacoonFlyDown"] =     MatrixAnimates::RacoonFlyDown;
    m_strToEnum["RacoonTail"] =        MatrixAnimates::RacoonTail;
    m_strToEnum["Swim"] =              MatrixAnimates::Swim;
    m_strToEnum["SwimUp"] =            MatrixAnimates::SwimUp;
    m_strToEnum["OnYoshi"] =           MatrixAnimates::OnYoshi;
    m_strToEnum["OnYoshiSit"] =        MatrixAnimates::OnYoshiSit;
    m_strToEnum["PipeUpDown"] =        MatrixAnimates::PipeUpDown;
    m_strToEnum["PipeUpDownRear"] =    MatrixAnimates::PipeUpDownRear;
    m_strToEnum["SlopeSlide"] =        MatrixAnimates::SlopeSlide;
    m_strToEnum["TanookiStatue"] =     MatrixAnimates::TanookiStatue;
    m_strToEnum["SwordAttak"] =        MatrixAnimates::SwordAttak;
    m_strToEnum["JumpSwordUp"] =       MatrixAnimates::JumpSwordUp;
    m_strToEnum["JumpSwordDown"] =     MatrixAnimates::JumpSwordDown;
    m_strToEnum["DownSwordAttak"] =    MatrixAnimates::DownSwordAttak;
    m_strToEnum["Hurted"] =            MatrixAnimates::Hurted;
}

MatrixAnimator::MatrixAnimates MatrixAnimator::toEnum(const std::string &aniName)
{
    if(m_strToEnum.empty())
        buildEnums();

    auto ani = m_strToEnum.find(aniName);

    if(ani != m_strToEnum.end())
        return ani->second;
    else
        return MatrixAnimates::Nothing;
}
