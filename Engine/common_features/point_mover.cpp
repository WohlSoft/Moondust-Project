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

#include "point_mover.h"
#include <cmath>

PointMover::PointMover()
{}

void PointMover::setPos(double posX, double posY)
{
    m_posX = posX;
    m_posY = posY;
    m_targetX = posX;
    m_targetY = posY;
    m_moving = false;
}

void PointMover::setSpeed(double speed)
{
    m_speed = speed;
    makeSpeed();
}

void PointMover::setTarget(double toPosX, double toPosY, double speed)
{
    if(speed <= 0.0)
        speed = m_speed;
    start(m_posX, m_posY, toPosX, toPosY, speed);
}

void PointMover::setTargetAuto(double toPosX, double toPosY, double timeMS)
{
    double speed;
    if(timeMS < 0.0)
        timeMS = 1.0;

    double distance = std::sqrt(std::pow(toPosX - m_posX, 2) + std::pow(toPosY - m_posY, 2));
    speed = distance / timeMS;
    if(speed < 1.0)
        speed = 1.0;

    start(m_posX, m_posY, toPosX, toPosY, speed);
}

void PointMover::startAuto(double posX, double posY, double toPosX, double toPosY, double timeMS)
{
    m_posX = posX;
    m_posY = posY;
    m_targetX = toPosX;
    m_targetY = toPosY;

    double distance = std::sqrt(std::pow(toPosX - posX, 2) + std::pow(toPosY - posY, 2));
    m_speed = distance / timeMS;
    if(m_speed < 1.0)
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

        m_posX += m_speedX * tickTime;
        m_posY += m_speedY * tickTime;

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
    double hDirect = (m_posX > m_targetX) ? 1.0 : -1.0;
    double vDirect = (m_posY > m_targetY) ? 1.0 : -1.0;
    double hdist = std::fabs(m_targetX - m_posX);
    double vdist = std::fabs(m_targetY - m_posY);
    double angle = 0.0;
    if(vdist != 0.0)
    {
        angle = std::atan(hdist / vdist);
        m_speedX = -std::sin(angle) * hDirect * m_speed;
        m_speedY = -std::cos(angle) * vDirect * m_speed;
    }
    else
    {
        m_speedX = -hDirect * m_speed;
        m_speedY = 0.0;
    }
}

bool PointMover::passedTargetX()
{
    if(m_speedX > 0.0)
        return m_posX + m_speedX >= m_targetX;
    else
        return m_posX + m_speedX <= m_targetX;
}

bool PointMover::passedTargetY()
{
    if(m_speedY > 0.0)
        return m_posY + m_speedY >= m_targetY;
    else
        return m_posY + m_speedY <= m_targetY;
}
