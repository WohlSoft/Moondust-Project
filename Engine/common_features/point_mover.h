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

#ifndef POINTMOVER_H
#define POINTMOVER_H

class PointMover
{
public:
    PointMover();
    PointMover(const PointMover&mp);

    void start(double posX, double posY, double toPosX, double toPosY, double speed);
    void startAuto(double posX, double posY, double toPosX, double toPosY, double timeMS);

    double posX();
    double posY();

    bool iterate(double tickTime);
    void makeSpeed();

private:
    bool passedTargetX();
    bool passedTargetY();

    bool    m_moving;
    double  m_speed;
    double  m_posX;
    double  m_posY;
    double  m_speedX;
    double  m_speedY;
    double  m_targetX;
    double  m_targetY;
};


/*
    struct MovingPoint
    {
        MovingPoint():
            enabled(false),
            moving(false),
            speed(0.08),
            blinker(false),
            blinkerSpeed(0.5),
            blinkerState(0.0),
            posX(0.0),
            posY(0.0),
            speedX(0.0),
            speedY(0.0),
            targetX(0.0),
            targetY(0.0),
            radius(20),
            color{1.0, 0.0, 0.0, 1.0}
        {}

        bool enabled;
        bool moving;
        double speed;
        bool    blinker;
        double  blinkerSpeed;
        double  blinkerState;

        void iterate()
        {
            if(moving)
            {
                if(passedTargetX() && passedTargetY())
                {
                    moving = false;
                    posX = targetX;
                    posY = targetY;
                    return;
                }
                if(passedTargetX())
                {
                    posX = targetX;
                    speedX = 0.0;
                }
                if(passedTargetY())
                {
                    posY = targetY;
                    speedY = 0.0;
                }
                posX += speedX;
                posY += speedY;
            }
            if(blinker)
            {
                blinkerState+=blinkerSpeed;
                while(blinkerState >= (2.0*M_PI))
                    blinkerState -= (2.0*M_PI);
            }
        }

        void makeSpeed()
        {
            double hDirect = (posX>targetX) ? 1.0 : -1.0;
            double vDirect = (posY>targetY) ? 1.0 : -1.0;
            double hdist = fabs(targetX - posX);
            double vdist = fabs(targetY - posY);
            double angle = 0.0;
            if(vdist != 0)
            {
                angle = atan(hdist/vdist);
                speedX = -sin(angle)*hDirect*speed;
                speedY = -cos(angle)*vDirect*speed;
            } else {
                speedX = -hDirect*speed;
                speedY = 0.0;
            }
        }

        bool passedTargetX()
        {
            if(speedX > 0.0)
            {
                return posX+speedX >= targetX;
            } else {
                return posX+speedX <= targetX;
            }
        }

        bool passedTargetY()
        {
            if(speedY > 0.0)
            {
                return posY+speedY >= targetY;
            } else {
                return posY+speedY <= targetY;
            }
        }

        double posX;
        double posY;
        double speedX;
        double speedY;
        double targetX;
        double targetY;
        double radius;
        double color[4];
    } m_movingPoint;
*/
#endif // POINTMOVER_H
