/*
 * Moondust, a free game engine for platform game making
 * Copyright (c) 2014-2021 Vitaly Novichkov <admin@wohlnet.ru>
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

#ifndef VSYNC_VALIDATOR_H
#define VSYNC_VALIDATOR_H

#include <SDL2/SDL_types.h>

class Scene;

/**
 * \brief A unit test to detect incorrect setup of V-Sync on a side of video card driver
 */
class VSyncValidator
{
    //! Steps passed
    int m_steps = 0;
    //! A time to run this unit test
    double m_timeLimit = 120.0;
    //! Maximum steps allowed to accept this unit test as passed
    int m_maxStepsAllowed = 100;
    //! Was test completed
    bool m_testDone = false;
    //! Was test passed or failed
    bool m_testPassed = false;
    //! Parent scene
    Scene * m_parentScene = nullptr;
    //! Ticks moment of previous update stamp
    Uint32 m_prevTick = 0;

public:
    explicit VSyncValidator(Scene *parent = nullptr, double tickDelay = 17);

    /**
     * \brief Is this test completed
     * @return true if test completed
     */
    bool isComplete();

    /**
     * \brief Update state of this unit test
     */
    void update();
};

#endif //VSYNC_VALIDATOR_H
