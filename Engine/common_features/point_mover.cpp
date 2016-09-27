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

#include "point_mover.h"
#include <cmath>

PointMover::PointMover():
    m_moving(false),
    m_speed(1.0),
    m_posX(0.0),
    m_posY(0.0),
    m_speedX(0.0),
    m_speedY(0.0),
    m_targetX(0.0),
    m_targetY(0.0)
{}

PointMover::PointMover(const PointMover &mp):
    m_moving(mp.m_moving),
    m_speed(mp.m_speed),
    m_posX(mp.m_posX),
    m_posY(mp.m_posY),
    m_speedX(mp.m_speedX),
    m_speedY(mp.m_speedY),
    m_targetX(mp.m_targetX),
    m_targetY(mp.m_targetY)
{}

void PointMover::startAuto(double posX, double posY, double toPosX, double toPosY, double timeMS)
{
    m_posX = posX;
    m_posY = posY;
    m_targetX = toPosX;
    m_targetY = toPosY;

    double distance = std::sqrt( pow( toPosX-posX, 2 ) + pow( toPosY-posY, 2 ) );
    m_speed = distance/timeMS;
    if(m_speed<1.0)
        m_speed = 1.0;
    makeSpeed();
    m_moving = true;
}

void PointMover::start(double posX, double posY, double toPosX, double toPosY, double speed)
{
    m_posX = posX;
    m_posY = posY;
    m_targetX = toPosX;
    m_targetY = toPosY;
    m_speed = speed;
    makeSpeed();
    m_moving = true;
}

double PointMover::posX()
{
    return m_posX;
}

double PointMover::posY()
{
    return m_posY;
}

bool PointMover::iterate(double tickTime)
{
    if(m_moving)
    {
        if(passedTargetX() && passedTargetY())
        {
            m_moving = false;
            m_posX = m_targetX;
            m_posY = m_targetY;
            return false;
        }

        m_posX += m_speedX*tickTime;
        m_posY += m_speedY*tickTime;

        if(passedTargetX())
        {
            m_posX = m_targetX;
            m_speedX = 0.0;
        }
        if(passedTargetY())
        {
            m_posY = m_targetY;
            m_speedY = 0.0;
        }
        return true;
    }
    return false;
}

void PointMover::makeSpeed()
{
    double hDirect = (m_posX>m_targetX) ? 1.0 : -1.0;
    double vDirect = (m_posY>m_targetY) ? 1.0 : -1.0;
    double hdist = fabs(m_targetX - m_posX);
    double vdist = fabs(m_targetY - m_posY);
    double angle = 0.0;
    if(vdist != 0)
    {
        angle = atan(hdist/vdist);
        m_speedX = -sin(angle)*hDirect*m_speed;
        m_speedY = -cos(angle)*vDirect*m_speed;
    } else {
        m_speedX = -hDirect*m_speed;
        m_speedY = 0.0;
    }
}

bool PointMover::passedTargetX()
{
    if(m_speedX > 0.0)
    {
        return m_posX+m_speedX >= m_targetX;
    } else {
        return m_posX+m_speedX <= m_targetX;
    }
}

bool PointMover::passedTargetY()
{
    if(m_speedY > 0.0)
    {
        return m_posY+m_speedY >= m_targetY;
    } else {
        return m_posY+m_speedY <= m_targetY;
    }
}
