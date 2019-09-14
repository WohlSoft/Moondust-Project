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

#include "luaclass_core_data.h"
#include <scenes/scene_level.h>
#include <scenes/scene_world.h>

Binding_Core_Data::Binding_Core_Data(Binding_Core_Data::DataType dataType, lua_State *L) :
    m_dataType(dataType),
    m_sectionName("default"),
    m_useSaveSlot(dataType != DATA_GLOBAL)
{
    init(L);
}

Binding_Core_Data::Binding_Core_Data(Binding_Core_Data::DataType dataType, bool useSaveSlot, lua_State *L) :
    m_dataType(dataType),
    m_sectionName("default"),
    m_useSaveSlot(useSaveSlot)
{
    init(L);
}

Binding_Core_Data::Binding_Core_Data(Binding_Core_Data::DataType dataType, const std::string &sectionName, lua_State *L):
    m_dataType(dataType),
    m_sectionName(sectionName),
    m_useSaveSlot(dataType != DATA_GLOBAL)
{
    init(L);
}

Binding_Core_Data::Binding_Core_Data(Binding_Core_Data::DataType dataType, const std::string &sectionName, bool useSaveSlot, lua_State *L) :
    m_dataType(dataType),
    m_sectionName(sectionName),
    m_useSaveSlot(useSaveSlot)
{
    init(L);
}

void Binding_Core_Data::init(lua_State *L)
{
    LuaLevelEngine *le = LuaGlobal::getLevelEngine(L);
    LuaWorldEngine *we = LuaGlobal::getWorldEngine(L);

    assert((le != nullptr) ^ (we != nullptr));

    if(le)
    {
        LevelScene *s = le->getScene();
        assert(s);
        m_episodeState = s->m_gameState;
    }
    else if(we)
    {
        WorldScene *s = we->getScene();
        assert(s);
        m_episodeState = s->m_gameState;
    }

    assert(m_episodeState);

    if(m_episodeState)
    {
        m_levelFileName = m_episodeState->getRelativeLevelFile();

        switch(m_dataType)
        {
        case DATA_LEVEL:
            m_data = m_episodeState->userData.getSection(GameUserDataManager::DATA_LEVEL, m_sectionName, m_levelFileName);
            break;
        case DATA_WORLD:
            m_data = m_episodeState->userData.getSection(GameUserDataManager::DATA_WORLD, m_sectionName);
            break;
        case DATA_GLOBAL:
            m_data = m_episodeState->userData.getSection(GameUserDataManager::DATA_GLOBAL, m_sectionName);
            break;
        case DATA_VOLATILE_LEVEL:
            m_data = m_episodeState->userData.getVolatileSection(GameUserDataManager::DATA_LEVEL, m_sectionName, m_levelFileName);
            break;
        case DATA_VOLATILE_WORLD:
            m_data = m_episodeState->userData.getVolatileSection(GameUserDataManager::DATA_WORLD, m_sectionName);
            break;
        case DATA_VOLATILE_GLOBAL:
            m_data = m_episodeState->userData.getVolatileSection(GameUserDataManager::DATA_GLOBAL, m_sectionName);
            break;
        }
    }
}

Binding_Core_Data::~Binding_Core_Data()
{}

void Binding_Core_Data::set(const std::string &key, const std::string &value)
{
    m_data[key] = value;
}


std::string Binding_Core_Data::get(const std::string &key) const
{
    auto it = m_data.find(key);
    return it == m_data.end() ? "" : it->second;
}

luabind::adl::object Binding_Core_Data::get(lua_State *L) const
{
    luabind::object allData = luabind::newtable(L);
    for(auto it = m_data.begin(); it != m_data.end(); ++it)
        allData[it->first] = it->second;
    return allData;
}

void Binding_Core_Data::save()
{
    assert(m_episodeState);
    if(!m_episodeState)
        return;
    switch(m_dataType)
    {
    case DATA_LEVEL:
        m_episodeState->userData.setSection(GameUserDataManager::DATA_LEVEL, m_data, m_sectionName, m_levelFileName);
        break;
    case DATA_WORLD:
        m_episodeState->userData.setSection(GameUserDataManager::DATA_WORLD, m_data, m_sectionName);
        break;
    case DATA_GLOBAL:
        m_episodeState->userData.setSection(GameUserDataManager::DATA_GLOBAL, m_data, m_sectionName);
        break;
    case DATA_VOLATILE_LEVEL:
        m_episodeState->userData.setVolatileSection(GameUserDataManager::DATA_LEVEL, m_data, m_sectionName, m_levelFileName);
        break;
    case DATA_VOLATILE_WORLD:
        m_episodeState->userData.setVolatileSection(GameUserDataManager::DATA_WORLD, m_data, m_sectionName);
        break;
    case DATA_VOLATILE_GLOBAL:
        m_episodeState->userData.setVolatileSection(GameUserDataManager::DATA_GLOBAL, m_data, m_sectionName);
        break;
    }
}

void Binding_Core_Data::save(const std::string &sectionName)
{
    setSectionName(sectionName);
    save();
}

Binding_Core_Data::DataType Binding_Core_Data::dataType() const
{
    return m_dataType;
}

void Binding_Core_Data::setDataType(Binding_Core_Data::DataType dataType)
{
    m_dataType = dataType;
}

std::string Binding_Core_Data::sectionName() const
{
     return m_sectionName;
}

void Binding_Core_Data::setSectionName(const std::string &sectionName)
{
    m_sectionName = sectionName;
}

bool Binding_Core_Data::useSaveSlot() const
{
    return m_useSaveSlot;
}

void Binding_Core_Data::setUseSaveSlot(bool useSaveSlot)
{
    m_useSaveSlot = useSaveSlot;
}

luabind::scope Binding_Core_Data::bindToLua()
{
    using namespace luabind;
    /***
    Cross-level/world data storage in a game save
    @module DataClass
    */
    return
            class_<Binding_Core_Data>("Data")
            /***
            DataTypes enumeration
            @section DataTypes
            */
            .enum_("DataTypes")
            [
                /***
                Access scope: This data will can be only accessed by the current level.
                @field Data.DATA_LEVEL
                */
                value("DATA_LEVEL", Binding_Core_Data::DATA_LEVEL),
                /***
                Access scope: This data will can be accessed by any level of episode and by world maps.
                @field Data.DATA_WORLD
                */
                value("DATA_WORLD", Binding_Core_Data::DATA_WORLD),
                /***
                Access scope: This data will can be accessed by any level of episode and by world
                maps same as @{Data.DATA_WORLD}. Kept for LunaLua compatibility.
                @field Data.DATA_GLOBAL
                */
                value("DATA_GLOBAL", Binding_Core_Data::DATA_GLOBAL),
                /***
                Access scope: This data will can be only accessed by the current level. Volatile storage, means,
                this data store will NOT be written into game save file and will stay while game is running.
                Quitting game will cause all volatile data be removed.
                @field Data.DATA_VOLATILE_LEVEL
                */
                value("DATA_VOLATILE_LEVEL", Binding_Core_Data::DATA_VOLATILE_LEVEL),
                /***
                Access scope: This data will can be accessed by any level of episode and by world maps.
                Volatile storage, means, this data store will NOT be written into game save file and will stay
                while game is running. Quitting game will cause all volatile data be removed.
                @field Data.DATA_VOLATILE_WORLD
                */
                value("DATA_VOLATILE_WORLD", Binding_Core_Data::DATA_VOLATILE_WORLD),
                /***
                Access scope: This data will can be accessed by any level of episode and by world maps
                same as @{Data.DATA_VOLATILE_WORLD}.
                Volatile storage, means, this data store will NOT be written into game save file and will stay
                while game is running. Quitting game will cause all volatile data be removed.
                @field Data.DATA_VOLATILE_GLOBAL
                */
                value("DATA_VOLATILE_GLOBAL", Binding_Core_Data::DATA_VOLATILE_GLOBAL)
            ]

        /***
        This class allows you to store any custom data in a game save. It uses strings for both keys and values.
        You can set your own section-name and save location.
        @type Data
        */
            /***
            Constructs a new Data class with the save location specified with dataLocation.
            It will use "default" as section name.
            @function Data
            @tparam DataTypes dataLocation Type of data storage scope
            */
            .def(constructor<Binding_Core_Data::DataType, lua_State*>())
            /***
            Constructs a new Data class with the save location specified with dataLocation.
            It will use "sectionName" as section name.
            @function Data
            @tparam DataTypes dataLocation Type of data storage scope
            @tparam string sectionName Name of data section
            */
            .def(constructor<Binding_Core_Data::DataType, std::string, lua_State*>())
            /***
            Constructs a new Data class with the save location specified with dataLocation.
            It will use "default" as section name.
            @function Data
            @tparam DataTypes dataLocation Type of data storage scope
            @tparam bool useSaveSlot Useless field, kept for compatibility with LunaLua
            */
            .def(constructor<Binding_Core_Data::DataType, bool, lua_State*>())
            /***
            Constructs a new Data class with the save location specified with dataLocation.
            It will use "sectionName" as section name.
            @function Data
            @tparam DataTypes dataLocation Type of data storage scope
            @tparam string sectionName Name of data section
            @tparam bool useSaveSlot Useless field, kept for compatibility with LunaLua
            */
            .def(constructor<Binding_Core_Data::DataType, std::string, bool, lua_State*>())
            /***
            Sets a value in the Data class. Use the tostring() function if you want to save a number.
            @function set
            @tparam string key Key of data entry
            @tparam string value Value of data entry
            */
            .def("set", &Binding_Core_Data::set)
            /***
            Returns the value of the key. If the key is not not set, then an empty string will
            be returned. Use the tonumber() function to convert a string into a number.
            @function get
            @tparam string key Key of data entry
            @treturn Value of data entry
            */
            .def("get", static_cast<std::string(Binding_Core_Data::*)(const std::string &) const>(&Binding_Core_Data::get))
            /***
            Returns a table with a copy of the content. The key is the table-index while the value
            is the table-value. Use the tonumber() function to convert a string into a number.
            @function get
            @treturn Table with [key] = value
            */
            .def("get", static_cast<luabind::object(Binding_Core_Data::*)(lua_State*) const>(&Binding_Core_Data::get))
            /***
            Saves the data into current game save state.
            @function save
            */
            .def("save", static_cast<void(Binding_Core_Data::*)()>(&Binding_Core_Data::save))
            /***
            Saves the data into current game save state and sets a new section name.
            @function sectionName A new section name
            @tparam string key Key of data entry
            */
            .def("save", static_cast<void(Binding_Core_Data::*)(const std::string &)>(&Binding_Core_Data::save))
            /***
            The access scope type of data
            @tfield DataTypes dataType
            */
            .property("dataType", &Binding_Core_Data::dataType, &Binding_Core_Data::setDataType)
            /***
            The section name of the data.
            @tfield string SectionName
            */
            .property("sectionName", &Binding_Core_Data::sectionName, &Binding_Core_Data::setSectionName)
            /***
            Useless field, kept for compatibility with LunaLua
            @tfield bool useSaveSlot
            */
            .property("useSaveSlot", &Binding_Core_Data::useSaveSlot, &Binding_Core_Data::setUseSaveSlot)
    ;
}
