/*
 * Moondust, a free game engine for platform game making
 * Copyright (c) 2014-2024 Vitaly Novichkov <admin@wohlnet.ru>
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

#ifndef BINDING_CORE_GRAPHICS_H
#define BINDING_CORE_GRAPHICS_H

#include <unordered_map>
#include <graphics/gl_renderer.h>
#include <script/lua_global.h>

#include <luabind/luabind.hpp>
#include <lua_includes/lua.hpp>

class Binding_Core_Graphics
{
public:
    static void setRootPath(std::string path);

    /*!
     * \brief Load image file and store a texture
     * \param filePath Relative or absolute path to the texture file
     * \return Pointer to texture object
     */
    static PGE_Texture* loadImage(std::string filePath);

    static luabind::object getPixelData(const PGE_Texture*img, int& width, int& height, lua_State *L);

    static void drawImage(const PGE_Texture*texture, float xPos, float yPos, lua_State *L);
    static void drawImage(const PGE_Texture*texture, float xPos, float yPos, float opacity, lua_State *L);

    static void drawImage(const PGE_Texture*texture, float xPos, float yPos, float sourceX, float sourceY, float width, float height, lua_State *L);
    static void drawImage(const PGE_Texture*texture, float xPos, float yPos, float sourceX, float sourceY, float width, float height, float opacity, lua_State *L);

    static void drawImageWP(const PGE_Texture*texture, float xPos, float yPos, long double zlayer, lua_State *L);
    static void drawImageWP(const PGE_Texture*texture, float xPos, float yPos, float opacity, long double zlayer, lua_State *L);

    static void drawImageWP(const PGE_Texture*texture, float xPos, float yPos, float sourceX, float sourceY, float width, float height, long double zlayer, lua_State *L);
    static void drawImageWP(const PGE_Texture*texture, float xPos, float yPos, float sourceX, float sourceY, float width, float height, float opacity, long double zlayer, lua_State *L);

    static void drawImageToScene(const PGE_Texture*texture, double xPos, double yPos, lua_State *L);
    static void drawImageToScene(const PGE_Texture*texture, double xPos, double yPos, float opacity, lua_State *L);
    static void drawImageToScene(const PGE_Texture*texture, double xPos, double yPos, float sourceX, float sourceY, float width, float height, lua_State *L);
    static void drawImageToScene(const PGE_Texture*texture, double xPos, double yPos, float sourceX, float sourceY, float width, float height, float opacity, lua_State *L);

    static void drawImageToSceneWP(const PGE_Texture*texture, double xPos, double yPos, long double zlayer, lua_State *L);
    static void drawImageToSceneWP(const PGE_Texture*texture, double xPos, double yPos, float opacity, long double zlayer, lua_State *L);
    static void drawImageToSceneWP(const PGE_Texture*texture, double xPos, double yPos, float sourceX, float sourceY, float width, float height, long double zlayer, lua_State *L);
    static void drawImageToSceneWP(const PGE_Texture*texture, double xPos, double yPos, float sourceX, float sourceY, float width, float height, float opacity, long double zlayer, lua_State *L);

    static double alignToCenter(double x, double width);
    static double alignToHCenter(double x, double width);
    static double alignToVCenter(double y, double height);

    /*!
     * Remove all cached textures
     */
    static void clearCache();

    typedef std::unordered_map<std::string, PGE_Texture> TexturesHash;
    //! Cached textures
    static TexturesHash textureCache;

    //! Current path to level/world custom directory (where look for image files by default)
    static std::string rootPath;

    static luabind::scope PGETexture_bindToLua();
    static luabind::scope bindToLua();
};

#endif // BINDING_CORE_GRAPHICS_H
