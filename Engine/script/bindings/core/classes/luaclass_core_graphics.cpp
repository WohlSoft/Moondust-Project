#include "luaclass_core_graphics.h"
#include <common_features/graphics_funcs.h>
#include <data_configs/config_manager.h>
#include <scenes/scene.h>
#include <Utils/files.h>

//#include <luabind/adopt_policy.hpp>
//#include <luabind/out_value_policy.hpp>

#define DEFAULT_ZOFFSET  400.0L

static inline bool calculateSides(
    float xPos,
    float yPos,
    float sourceX,
    float sourceY,
    float width,
    float height,
    float txW,
    float txH,

    float &x,
    float &y,
    float &w,
    float &h,
    float &left,
    float &top,
    float &right,
    float &bottom)
{
    float sX = sourceX, sY = sourceY;
    x = xPos;
    y = yPos;
    w = width;
    h = height;

    if(w <= 0.0f)
        return false;

    if(h <= 0.0f)
        return false;

    if(sX < 0.0f)
    {
        if(sX < -txW)
            return false;//Don't draw out of view!
        x = x - sX;
        sX = 0.0f;
    }
    else if(sX > txW)
        return false; //Don't draw if out of view!
    if(sY < 0.0f)
    {
        if(sY < -txH)
            return false;//Don't draw out of view!
        y = y - sY;
        sY = 0.0f;
    }
    else if(sourceY > txH)
        return false;//Don't draw if out of view!

    if(w > (txW - sX))
    {
        w = (txW - sX);
    }

    if(h > (txH - sY))
    {
        h = (txH - sY);
    }

    left   = sX / txW;
    top    = sY / txH;
    right  = (sX + w) / txW;
    bottom = (sY + h) / txH;

    return true;
}


//! Cached textures
Binding_Core_Graphics::TexturesHash Binding_Core_Graphics::textureCache;

//! Current path to level/world custom directory (where look for image files by default)
std::string Binding_Core_Graphics::rootPath;

void Binding_Core_Graphics::setRootPath(std::string path)
{
    rootPath = path;
    if(!rootPath.empty())
    {
        if(rootPath.back() != '/')
            rootPath.push_back('/');
    }
}

PGE_Texture *Binding_Core_Graphics::loadImage(std::string filePath)
{
    PGE_Texture texture;
    if(!Files::isAbsolute(filePath))
    {
        filePath = rootPath + filePath;
    }

    TexturesHash::iterator tc = textureCache.find(filePath);
    if(tc != textureCache.end())
    {
        //Don't load same texture!
        return &tc->second;
    }

    GlRenderer::loadTextureP(texture, filePath);
    textureCache.insert({filePath, texture});
    return &textureCache[filePath];
}

luabind::adl::object Binding_Core_Graphics::getPixelData(const PGE_Texture *img, int &width, int &height, lua_State *L)
{
    if(!img || !img->inited || (img->texture == 0))
    {
        luaL_error(L, "Internal error: Failed to find image resource!");
        return luabind::object();
    }
    luabind::object returnTable = luabind::newtable(L);
    size_t bufferSize = size_t(GlRenderer::getPixelDataSize(img));
    if(bufferSize == 0)
    {
        luaL_error(L, "Internal error: Invalid image resource!");
        return luabind::object();
    }
    unsigned char *buffer = new unsigned char[bufferSize];
    GlRenderer::getPixelData(img, buffer);
    size_t i = 1;
    for(i = 1; i <= bufferSize; ++i)
    {
        returnTable[i] = buffer[i];
    }
    width  = img->w;
    height = img->h;
    return returnTable;
}

void Binding_Core_Graphics::drawImage(const PGE_Texture *texture, float xPos, float yPos, lua_State *L)
{
    drawImageWP(texture, xPos, yPos, DEFAULT_ZOFFSET, L);
}

void Binding_Core_Graphics::drawImage(const PGE_Texture *texture, float xPos, float yPos, float opacity, lua_State *L)
{
    drawImageWP(texture, xPos, yPos, opacity, DEFAULT_ZOFFSET, L);
}

void Binding_Core_Graphics::drawImage(const PGE_Texture *texture, float xPos, float yPos, float sourceX, float sourceY, float width, float height, lua_State *L)
{
    drawImageWP(texture, xPos, yPos, sourceX, sourceY, width, height, DEFAULT_ZOFFSET, L);
}

void Binding_Core_Graphics::drawImage(const PGE_Texture *texture, float xPos, float yPos, float sourceX, float sourceY, float width, float height, float opacity, lua_State *L)
{
    drawImageWP(texture, xPos, yPos, sourceX, sourceY, width, height, opacity, DEFAULT_ZOFFSET, L);
}


void Binding_Core_Graphics::drawImageWP(const PGE_Texture *texture, float xPos, float yPos, long double zlayer, lua_State *L)
{
    drawImageWP(texture, xPos, yPos, 1.0f, zlayer, L);
}


void Binding_Core_Graphics::drawImageWP(const PGE_Texture *texture, float xPos, float yPos, float opacity, long double zlayer, lua_State *L)
{
    if(!texture)
        return;
    LuaGlobal::getEngine(L)->getBaseScene()->renderArrayAddFunction([ = ](double /*CameraX*/, double /*CameraY*/)
    {
        GlRenderer::setTextureColor(1.0f, 1.0f, 1.0f, opacity);
        GlRenderer::renderTexture(const_cast<PGE_Texture *>(texture), xPos, yPos);
    }, zlayer);
}


void Binding_Core_Graphics::drawImageWP(const PGE_Texture *texture, float xPos, float yPos, float sourceX, float sourceY, float width, float height, long double zlayer, lua_State *L)
{
    drawImageWP(texture, xPos, yPos, sourceX, sourceY, width, height, 1.0f, zlayer, L);
}

void Binding_Core_Graphics::drawImageWP(const PGE_Texture *texture, float xPos, float yPos, float sourceX, float sourceY, float width, float height, float opacity, long double zlayer, lua_State *L)
{
    if(!texture) return;
    LuaGlobal::getEngine(L)->getBaseScene()->renderArrayAddFunction([ = ](double /*CameraX*/, double /*CameraY*/)
    {
        float x, y, w, h, left, top, right, bottom;
        if(!calculateSides(xPos, yPos,
                           sourceX, sourceY,
                           width, height,
                           float(texture->w), float(texture->h),

                           x, y, w, h,
                           left, top, right, bottom))
            return;
        GlRenderer::setTextureColor(1.0f, 1.0f, 1.0f, opacity);
        GlRenderer::renderTexture(const_cast<PGE_Texture *>(texture), x, y, w, h, top, bottom, left, right);
    }, zlayer);
}



void Binding_Core_Graphics::drawImageToScene(const PGE_Texture *texture, double xPos, double yPos, lua_State *L)
{
    drawImageToSceneWP(texture, xPos, yPos, DEFAULT_ZOFFSET, L);
}

void Binding_Core_Graphics::drawImageToScene(const PGE_Texture *texture, double xPos, double yPos, float opacity, lua_State *L)
{
    drawImageToSceneWP(texture, xPos, yPos, opacity, DEFAULT_ZOFFSET, L);
}

void Binding_Core_Graphics::drawImageToScene(const PGE_Texture *texture, double xPos, double yPos, float sourceX, float sourceY, float width, float height, lua_State *L)
{
    drawImageToSceneWP(texture, xPos, yPos, sourceX, sourceY, width, height, DEFAULT_ZOFFSET, L);
}

void Binding_Core_Graphics::drawImageToScene(const PGE_Texture *texture, double xPos, double yPos, float sourceX, float sourceY, float width, float height, float opacity, lua_State *L)
{
    drawImageToSceneWP(texture, xPos, yPos, sourceX, sourceY, width, height, opacity, DEFAULT_ZOFFSET, L);
}


void Binding_Core_Graphics::drawImageToSceneWP(const PGE_Texture *texture, double xPos, double yPos, long double zlayer, lua_State *L)
{
    drawImageToSceneWP(texture, xPos, yPos, 1.0f, zlayer, L);
}

void Binding_Core_Graphics::drawImageToSceneWP(const PGE_Texture *texture, double xPos, double yPos, float opacity, long double zlayer, lua_State *L)
{
    if(!texture)
        return;
    LuaGlobal::getEngine(L)->getBaseScene()->renderArrayAddFunction([ = ](double cameraX, double cameraY)
    {
        GlRenderer::setTextureColor(1.0f, 1.0f, 1.0f, opacity);
        GlRenderer::renderTexture(const_cast<PGE_Texture *>(texture), float(xPos - cameraX), float(yPos - cameraY));
    }, zlayer);
}

void Binding_Core_Graphics::drawImageToSceneWP(const PGE_Texture *texture, double xPos, double yPos, float sourceX, float sourceY, float width, float height, long double zlayer, lua_State *L)
{
    drawImageToSceneWP(texture, xPos, yPos, sourceX, sourceY, width, height, 1.0f, zlayer, L);
}

void Binding_Core_Graphics::drawImageToSceneWP(const PGE_Texture *texture, double xPos, double yPos, float sourceX, float sourceY, float width, float height, float opacity, long double zlayer, lua_State *L)
{
    if(!texture) return;
    LuaGlobal::getEngine(L)->getBaseScene()->renderArrayAddFunction([ = ](double cameraX, double cameraY)
    {
        float x, y, w, h, left, top, right, bottom;
        if(!calculateSides(float(xPos - cameraX), float(yPos - cameraY),
                           sourceX, sourceY,
                           width, height,
                           float(texture->w), float(texture->h),

                           x, y, w, h,
                           left, top, right, bottom))
            return;
        GlRenderer::setTextureColor(1.0f, 1.0f, 1.0f, opacity);
        GlRenderer::renderTexture(const_cast<PGE_Texture *>(texture), x, y, w, h, top, bottom, left, right);
    }, zlayer);
}

void Binding_Core_Graphics::clearCache()
{
    for(TexturesHash::iterator it = textureCache.begin(); it != textureCache.end(); it++)
    {
        PGE_Texture &texture = it->second;
        GlRenderer::deleteTexture(texture);
    }
    textureCache.clear();
}

double Binding_Core_Graphics::alignToCenter(double x, double width)
{
    return alignToHCenter(x, width);
}

double Binding_Core_Graphics::alignToHCenter(double x, double width)
{
    return static_cast<double>(GlRenderer::alignToCenterW(static_cast<int>(x), static_cast<int>(width)));
}

double Binding_Core_Graphics::alignToVCenter(double y, double height)
{
    return static_cast<double>(GlRenderer::alignToCenterH(static_cast<int>(y), static_cast<int>(height)));
}


/***
Global graphics functions.<br>
While using any functions of this module you must type parent name.<br>
For example, Graphics.drawImage(texture, x, y)
@module Graphics
*/

luabind::scope Binding_Core_Graphics::PGETexture_bindToLua()
{
    using namespace luabind;
    return
        /***
        The LuaImageResource refers to a PGE Engine specific type, consisting of the data of a loaded image file as OpenGL texture.
        @table LuaImageResource
        @tfield int w Width of texture
        @tfield int h Height of texture
        @tfield uint texture Index of OpenGL texture
        */
        class_<PGE_Texture>("LuaImageResource")
        //Properties
        .def_readonly("w", &PGE_Texture::w)
        .def_readonly("h", &PGE_Texture::h)
        .def_readonly("texture", &PGE_Texture::texture);
}

luabind::scope Binding_Core_Graphics::bindToLua()
{
    using namespace luabind;
    return
        /***
        Loading of image files
        @section GraphicsLoad
        */
        namespace_("Graphics")[
            /***
            Loads a texture into the memory from image file.
            @function loadImage
            @tparam string filename The filename of the image file relative to the custom folder. Can be also a absolute path.
            @treturn LuaImageResource an image descritor of LuaImageResource type (PGE_Texture internally). When returned nil, an error has occouped.
            */
            def("loadImage", &loadImage),

            /* WIP */
            //def("getPixelData", &getPixelData, pure_out_value(_2) + pure_out_value(_3)),

            /***
            Drawing on screen
            @section GraphicsScreenDraw
            */
            /***
            Draws the texture for a frame.
            @function drawImage
            @tparam LuaImageResource texture Texture resource to draw
            @tparam float xPos X position on the screen coordinates
            @tparam float yPos Y position on the screen coordinates
            */
            def("drawImage", (void(*)(const PGE_Texture *, float, float, lua_State *))&drawImage),

            /***
            Draws the texture for a frame, with an opacity from 0.0 to 1.0.
            @function drawImage
            @tparam LuaImageResource texture Texture resource to draw
            @tparam float xPos X position on the screen coordinates
            @tparam float yPos Y position on the screen coordinates
            @tparam float opacity Opacity level (value between 1.0 [non-transparent] and 0.0 [transparent])
            */
            def("drawImage", (void(*)(const PGE_Texture *, float, float, float, lua_State *))&drawImage),

            /***
            Draws a part of the texture for a frame. Only a part of the pictures is drawn given by <i>sourceX</i> and <i>sourceY</i> with the specific width and height.
            @function drawImage
            @tparam LuaImageResource texture Texture resource to draw
            @tparam float xPos X position on the screen coordinates
            @tparam float yPos Y position on the screen coordinates
            @tparam float sourceX Source X position of a fragment on the texture
            @tparam float sourceY Source Y position of a fragment on the texture
            @tparam float width Width of a texture fragment
            @tparam float height Height of a texture fragment
             */
            def("drawImage", (void(*)(const PGE_Texture *, float, float, float, float, float, float, lua_State *))&drawImage),

            /***
            Draws a part of the texture for a frame, with an opacity from 0.0 to 1.0.
            Only a part of the pictures is drawn given by <i>sourceX</i> and <i>sourceY</i> with the specific width and height.
            @function drawImage
            @tparam LuaImageResource texture Texture resource to draw
            @tparam float xPos X position on the screen coordinates
            @tparam float yPos Y position on the screen coordinates
            @tparam float sourceX Source X position of a fragment on the texture
            @tparam float sourceY Source Y position of a fragment on the texture
            @tparam float width Width of a texture fragment
            @tparam float height Height of a texture fragment
            @tparam float opacity Opacity level (value between 1.0 [non-transparent] and 0.0 [transparent])
            */
            def("drawImage", (void(*)(const PGE_Texture *, float, float, float, float, float, float, float, lua_State *))&drawImage),

            /***
            Drawing on screen with prioritizing
            @section GraphicsScreenDrawWP
            */
            /***
            Draws the texture for a frame with prioritizing.
            The rendering of the image is prioritized by <i>zValue</i> priority
            @function drawImageWP
            @tparam LuaImageResource texture Texture resource to draw
            @tparam float xPos X position on the screen coordinates
            @tparam float yPos Y position on the screen coordinates
            @tparam double zValue A Z value, declares a render priority
            */
            def("drawImageWP", (void(*)(const PGE_Texture *, float, float, long double, lua_State *))&drawImageWP),

            /***
            Draws the texture for a frame with prioritizing, with an opacity from 0.0 to 1.0.
            @function drawImageWP
            @tparam LuaImageResource texture Texture resource to draw
            @tparam float xPos X position on the screen coordinates
            @tparam float yPos Y position on the screen coordinates
            @tparam float opacity Opacity level (value between 1.0 [non-transparent] and 0.0 [transparent])
            @tparam double zValue A Z value, declares a render priority
            */
            def("drawImageWP", (void(*)(const PGE_Texture *, float, float, float, long double, lua_State *))&drawImageWP),

            /***
            Draws a part of the texture for a frame with prioritizing. Only a part of the pictures is drawn given by <i>sourceX</i> and <i>sourceY</i> with the specific width and height.
            @function drawImageWP
            @tparam LuaImageResource texture Texture resource to draw
            @tparam float xPos X position on the screen coordinates
            @tparam float yPos Y position on the screen coordinates
            @tparam float sourceX Source X position of a fragment on the texture
            @tparam float sourceY Source Y position of a fragment on the texture
            @tparam float width Width of a texture fragment
            @tparam float height Height of a texture fragment
            @tparam double zValue A Z value, declares a render priority
             */
            def("drawImageWP", (void(*)(const PGE_Texture *, float, float, float, float, float, float, long double, lua_State *))&drawImageWP),

            /***
            Draws a part of the texture for a frame with prioritizing, with an opacity from 0.0 to 1.0.
            Only a part of the pictures is drawn given by <i>sourceX</i> and <i>sourceY</i> with the specific width and height.
            @function drawImageWP
            @tparam LuaImageResource texture Texture resource to draw
            @tparam float xPos X position on the screen coordinates
            @tparam float yPos Y position on the screen coordinates
            @tparam float sourceX Source X position of a fragment on the texture
            @tparam float sourceY Source Y position of a fragment on the texture
            @tparam float width Width of a texture fragment
            @tparam float height Height of a texture fragment
            @tparam float opacity Opacity level (value between 1.0 [non-transparent] and 0.0 [transparent])
            @tparam double zValue A Z value, declares a render priority
            */
            def("drawImageWP", (void(*)(const PGE_Texture *, float, float, float, float, float, float, float, long double, lua_State *))&drawImageWP),

            /***
            Drawing on scene
            @section GraphicsScreenDrawWP
            */
            /***
            Draws the texture for a frame by scene world coordinates.
            @function drawImageToScene
            @tparam LuaImageResource texture Texture resource to draw
            @tparam double xPos X position on the level or world scene coordinates
            @tparam double yPos Y position on the level or world scene coordinates
            */
            def("drawImageToScene", (void(*)(const PGE_Texture *, double, double, lua_State *))&drawImageToScene),

            /***
            Draws the texture for a frame by scene world coordinates, with an opacity from 0.0 to 1.0.
            @function drawImageToScene
            @tparam LuaImageResource texture Texture resource to draw
            @tparam double xPos X position on the level or world scene coordinates
            @tparam double yPos Y position on the level or world scene coordinates
            @tparam float opacity Opacity level (value between 1.0 [non-transparent] and 0.0 [transparent])
            */
            def("drawImageToScene", (void(*)(const PGE_Texture *, double, double, float, lua_State *))&drawImageToScene),

            /***
            Draws a part of the texture for a frame by scene world coordinates. Only a part of the pictures is drawn given by <i>sourceX</i> and <i>sourceY</i> with the specific width and height.
            @function drawImageToScene
            @tparam LuaImageResource texture Texture resource to draw
            @tparam double xPos X position on the level or world scene coordinates
            @tparam double yPos Y position on the level or world scene coordinates
            @tparam float sourceX Source X position of a fragment on the texture
            @tparam float sourceY Source Y position of a fragment on the texture
            @tparam float width Width of a texture fragment
            @tparam float height Height of a texture fragment
             */
            def("drawImageToScene", (void(*)(const PGE_Texture *, double, double, float, float, float, float, lua_State *))&drawImageToScene),

            /***
            Draws a part of the texture for a frame by scene world coordinates, with an opacity from 0.0 to 1.0.
            Only a part of the pictures is drawn given by <i>sourceX</i> and <i>sourceY</i> with the specific width and height.
            @function drawImageToScene
            @tparam LuaImageResource texture Texture resource to draw
            @tparam double xPos X position on the level or world scene coordinates
            @tparam double yPos Y position on the level or world scene coordinates
            @tparam float sourceX Source X position of a fragment on the texture
            @tparam float sourceY Source Y position of a fragment on the texture
            @tparam float width Width of a texture fragment
            @tparam float height Height of a texture fragment
            @tparam float opacity Opacity level (value between 1.0 [non-transparent] and 0.0 [transparent])
            */
            def("drawImageToScene", (void(*)(const PGE_Texture *, double, double, float, float, float, float, float, lua_State *))&drawImageToScene),

            /***
            Drawing on scene with prioritizing
            @section GraphicsScreenDrawWP
            */
            /***
            Draws the texture for a frame by scene world coordinates with prioritizing.
            The rendering of the image is prioritized by <i>zValue</i> priority
            @function drawImageToSceneWP
            @tparam LuaImageResource texture Texture resource to draw
            @tparam double xPos X position on the level or world scene coordinates
            @tparam double yPos Y position on the level or world scene coordinates
            @tparam double zValue A Z value, declares a render priority
            */
            def("drawImageToSceneWP", (void(*)(const PGE_Texture *, double, double, long double, lua_State *))&drawImageToSceneWP),

            /***
            Draws the texture for a frame by scene world coordinates with prioritizing, with an opacity from 0.0 to 1.0.
            @function drawImageToSceneWP
            @tparam LuaImageResource texture Texture resource to draw
            @tparam double xPos X position on the level or world scene coordinates
            @tparam double yPos Y position on the level or world scene coordinates
            @tparam float opacity Opacity level (value between 1.0 [non-transparent] and 0.0 [transparent])
            @tparam double zValue A Z value, declares a render priority
            */
            def("drawImageToSceneWP", (void(*)(const PGE_Texture *, double, double, float, long double, lua_State *))&drawImageToSceneWP),

            /***
            Draws a part of the texture for a frame by scene world coordinates with prioritizing. Only a part of the pictures is drawn given by <i>sourceX</i> and <i>sourceY</i> with the specific width and height.
            @function drawImageToSceneWP
            @tparam LuaImageResource texture Texture resource to draw
            @tparam double xPos X position on the level or world scene coordinates
            @tparam double yPos Y position on the level or world scene coordinates
            @tparam float sourceX Source X position of a fragment on the texture
            @tparam float sourceY Source Y position of a fragment on the texture
            @tparam float width Width of a texture fragment
            @tparam float height Height of a texture fragment
            @tparam double zValue A Z value, declares a render priority
            */
            def("drawImageToSceneWP", (void(*)(const PGE_Texture *, double, double, float, float, float, float, long double, lua_State *))&drawImageToSceneWP),

            /***
            Draws a part of the texture for a frame by scene world coordinates with prioritizing, with an opacity from 0.0 to 1.0.
            Only a part of the pictures is drawn given by <i>sourceX</i> and <i>sourceY</i> with the specific width and height.
            @function drawImageToSceneWP
            @tparam LuaImageResource texture Texture resource to draw
            @tparam double xPos X position on the level or world scene coordinates
            @tparam double yPos Y position on the level or world scene coordinates
            @tparam float sourceX Source X position of a fragment on the texture
            @tparam float sourceY Source Y position of a fragment on the texture
            @tparam float width Width of a texture fragment
            @tparam float height Height of a texture fragment
            @tparam float opacity Opacity level (value between 1.0 [non-transparent] and 0.0 [transparent])
            @tparam double zValue A Z value, declares a render priority
            */
            def("drawImageToSceneWP", (void(*)(const PGE_Texture *, double, double, float, float, float, float, float, long double, lua_State *))&drawImageToSceneWP),

            /***
            Alignment functions
            @section GraphicsAlign
            */
            /***
            Aligns horizontal position of object of <i>width</i> to center with <i>offset</i>.
            @function alignToHCenter
            @tparam int x X Offset of horizontal center position
            @tparam int width Width of object
            @treturn int Aligned X position of object
            */
            def("alignToHCenter", &alignToHCenter),

            /***
            Aligns vertical position of object of <i>height</i> to center with <i>offset</i>.
            @function alignToVCenter
            @tparam int y Y Offset of vertical center position
            @tparam int height Height of object
            @treturn int Aligned Y position of object
            */
            def("alignToVCenter", &alignToVCenter)
        ];
}
