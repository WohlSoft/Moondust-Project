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

#ifndef PGE_FADER_H
#define PGE_FADER_H

class PGE_Fader
{
public:
    PGE_Fader();
    PGE_Fader(const PGE_Fader&f);
    ~PGE_Fader();

    void setFade(int speed, float target, float step);
    void setRatio(float ratio);
    bool tickFader(float ticks);
    const float &fadeRatio();
    bool isFading();
    bool isFull();
    void setFull();
    bool isNull();
    void setNull();

    int ticksLeft();
private:
    void fadeStep();
    float cur_ratio;
    float target_ratio;
    float fade_step;
    int fadeSpeed;
    float manual_ticks;
};

#endif // PGE_FADER_H
