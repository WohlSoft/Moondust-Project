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

#ifndef POINTMOVER_H
#define POINTMOVER_H

class PointMover
{
public:
    PointMover();
    PointMover(const PointMover &mp) = default;

    /*!
     * \brief Set position without motion
     * \param posX X position
     * \param posY Y position
     */
    void setPos(double posX, double posY);

    void setSpeed(double speed);

    void setTarget(double toPosX, double toPosY, double speed = -1.0);

    void setTargetAuto(double toPosX, double toPosY, double timeMS = -1.0);

    /*!
     * \brief Start point movement animation with speed
     * \param posX Initial X position
     * \param posY Initial Y position
     * \param toPosX Target X position
     * \param toPosY Target Y position
     * \param speed Moving speed
     */
    void start(double posX, double posY, double toPosX, double toPosY, double speed);

    /*!
     * \brief Start point moving animation with auto-calculation of speed to fit the time
     * \param posX Initial X position
     * \param posY Initial Y position
     * \param toPosX Target X position
     * \param toPosY Target Y position
     * \param timeMS Time of movement process
     */
    void startAuto(double posX, double posY, double toPosX, double toPosY, double timeMS);

    //! Current X position
    double posX();
    //! Current Y position
    double posY();

    /*!
     * \brief Iterate the moving process with given milliseconds
     * \param tickTime Time of one frame in milliseconds
     * \return is destinition passed
     */
    bool iterate(double tickTime);

    void makeSpeed();

private:
    bool passedTargetX();
    bool passedTargetY();

    bool    m_moving    = false;
    double  m_speed     = 1.0;
    double  m_posX      = 0.0;
    double  m_posY      = 0.0;
    double  m_speedX    = 0.0;
    double  m_speedY    = 0.0;
    double  m_targetX   = 0.0;
    double  m_targetY   = 0.0;
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
