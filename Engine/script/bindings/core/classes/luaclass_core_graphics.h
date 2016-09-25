#ifndef BINDING_CORE_GRAPHICS_H
#define BINDING_CORE_GRAPHICS_H

#include <QHash>
#include <graphics/gl_renderer.h>
#include <script/lua_global.h>

#include <luabind/luabind.hpp>
#include <lua_inclues/lua.hpp>

class Binding_Core_Graphics
{
public:
    static void setRootPath(QString path);

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

    static double alignToCenter(double x, double width);

    /*!
     * Remove all cached textures
     */
    static void clearCache();

    //! Cached textures
    static QHash<QString, PGE_Texture> textureCache;

    //! Current path to level/world custom directory (where look for image files by default)
    static QString rootPath;

    static luabind::scope PGETexture_bindToLua();
    static luabind::scope bindToLua();
};

#endif // BINDING_CORE_GRAPHICS_H
