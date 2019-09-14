/*
 * Moondust, a free game engine for platform game making
 * Copyright (c) 2014-2019 Vitaly Novichkov <admin@wohlnet.ru>
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
#include <script/lua_global.h>

#include <luabind/luabind.hpp>
#include <lua_includes/lua.hpp>

class EpisodeState;

class Binding_Core_Data
{
public:
    enum DataType
    {
        DATA_LEVEL = 0,
        DATA_WORLD,
        DATA_GLOBAL,
        DATA_VOLATILE_LEVEL,
        DATA_VOLATILE_WORLD,
        DATA_VOLATILE_GLOBAL
    };
    Binding_Core_Data(DataType dataType, lua_State* L);
    Binding_Core_Data(DataType dataType, bool useSaveSlot, lua_State* L);

    Binding_Core_Data(DataType dataType, const std::string &sectionName, lua_State* L);
    Binding_Core_Data(DataType dataType, const std::string &sectionName, bool useSaveSlot, lua_State* L);

    ~Binding_Core_Data();

    void set(const std::string &key, const std::string &value);
    std::string get(const std::string &key) const;
    luabind::object get(lua_State* L) const;

    void save();
    void save(const std::string &sectionName);

    DataType dataType() const;
    void setDataType(DataType dataType);

    std::string sectionName() const;
    void setSectionName(const std::string &sectionName);

    bool useSaveSlot() const;
    void setUseSaveSlot(bool useSaveSlot);

    static luabind::scope bindToLua();

private:
    DataType m_dataType = DATA_WORLD;
    std::string m_sectionName = "default";
    std::string m_levelFileName = "unknown";
    typedef std::unordered_map<std::string, std::string>    DataList;
    DataList m_data;
    bool m_useSaveSlot = true;
    bool m_isVolatile = false;

    EpisodeState *m_episodeState = nullptr;


    void init(lua_State* L);
};

#endif // BINDING_CORE_GRAPHICS_H
