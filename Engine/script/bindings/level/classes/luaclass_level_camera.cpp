#include <scenes/level/lvl_camera.h>

/***
Level specific classes
@module LevelClasses
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
