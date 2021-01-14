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

#ifndef BINDING_CORE_GRAPHICS_H
#define BINDING_CORE_GRAPHICS_H

#include <unordered_map>
#include <script/lua_global.h>

#include <luabind/luabind.hpp>
#include <lua_includes/lua.hpp>

class EpisodeState;

/**
 * @brief Data() class for Lua API, provides a "shelve" for cross-level data storage
 */
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

    /**
     * @brief Sets value of data entry by key
     * @param key Key of data entry
     * @param value Value of data entry
     */
    void set(const std::string &key, const std::string &value);
    /**
     * @brief Returns value of data entry by key
     * @param key Key of data entry
     * @return Value of data entry
     */
    std::string get(const std::string &key) const;

    /**
     * @brief Gives a table of all stored data
     * @param L Lua state
     * @return Lua table of all data, stored by key=value
     */
    luabind::object get(lua_State* L) const;

    /**
     * @brief Store all holding data into episode state
     */
    void save();
    /**
     * @brief Store all holding data into episode state, and rename the section
     * @param sectionName New name of section
     */
    void save(const std::string &sectionName);

    /**
     * @brief Get the type of data storage scope
     * @return type of data storage scope
     */
    DataType dataType() const;
    /**
     * @brief Change the storage scope type
     * @param dataType Data storage scope type
     */
    void setDataType(DataType dataType);

    /**
     * @brief Gives name of data section
     * @return Name of data section
     */
    std::string sectionName() const;
    /**
     * @brief Rename data section
     * @param sectionName New name of data section
     */
    void setSectionName(const std::string &sectionName);

    /**
     * @brief Useless, showing a value of "useSaveSlot" property
     * @return Value of "useSaveSlot" property
     */
    bool useSaveSlot() const;

    /**
     * @brief Useless, changing the value of "useSaveSlot" property
     * @param useSaveSlot New value of "useSaveSlot" property
     */
    void setUseSaveSlot(bool useSaveSlot);

    static luabind::scope bindToLua();

private:
    //! Storage type and scope
    DataType m_dataType = DATA_WORLD;
    //! Type of data section
    std::string m_sectionName = "default";
    //! Level filename for Level data scope
    std::string m_levelFileName = "unknown";
    //! Data store hash table
    typedef std::unordered_map<std::string, std::string>    DataList;
    //! Data store
    DataList m_data;
    //! Useless "use Save Slot" kept for backward compatibility with LunaLua
    bool m_useSaveSlot = true;
    //! Current episode state reference. If it's not set, no data will be saved or loaded
    EpisodeState *m_episodeState = nullptr;

    /**
     * @brief Internal initialization and loading of data
     * @param L State of Lua script engine
     */
    void init(lua_State* L);
};

#endif // BINDING_CORE_GRAPHICS_H
