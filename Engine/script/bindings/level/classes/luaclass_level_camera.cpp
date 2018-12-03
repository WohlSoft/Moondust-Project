/*
 * Moondust, a free game engine for platform game making
 * Copyright (c) 2014-2018 Vitaly Novichkov <admin@wohlnet.ru>
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

#include <scenes/level/lvl_camera.h>

/***
Level specific functions and classes
@module LevelCommon
*/

luabind::scope PGE_LevelCamera::bindToLua()
{
    using namespace luabind;
    /***
    Player Camera cababilities (Level-Only)
    @type LevelCamera
    */
    return class_<PGE_LevelCamera>("LevelCamera")
            /***
            In-Scene X position of camera
            @tfield double x
            */
            .property("x", &PGE_LevelCamera::posX)
            /***
            In-Scene Y position of camera
            @tfield double y
            */
            .property("y", &PGE_LevelCamera::posY)
            /***
            X screen position of camera
            @tfield double renderX
            */
            .property("renderX", &PGE_LevelCamera::renderX)
            /***
            Y screen position of camera
            @tfield double renderY
            */
            .property("renderY", &PGE_LevelCamera::renderY)
            /***
            Center X screen position of camera
            @tfield double centerX
            */
            .property("centerX", &PGE_LevelCamera::centerX)
            /***
            Center Y screen position of camera
            @tfield double centerY
            */
            .property("centerY", &PGE_LevelCamera::centerY)
            /***
            Width of camera
            @tfield int width
            */
            .property("width", &PGE_LevelCamera::w)
            /***
            Height of camera
            @tfield int height
            */
            .property("height", &PGE_LevelCamera::h);
}
