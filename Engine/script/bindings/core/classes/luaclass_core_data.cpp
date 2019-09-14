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
    return
            class_<Binding_Core_Data>("Data")
                .enum_("DataTypes")
                [
                    value("DATA_LEVEL", Binding_Core_Data::DATA_LEVEL),
                    value("DATA_WORLD", Binding_Core_Data::DATA_WORLD),
                    value("DATA_GLOBAL", Binding_Core_Data::DATA_GLOBAL),
                    value("DATA_VOLATILE_LEVEL", Binding_Core_Data::DATA_VOLATILE_LEVEL),
                    value("DATA_VOLATILE_WORLD", Binding_Core_Data::DATA_VOLATILE_WORLD),
                    value("DATA_VOLATILE_GLOBAL", Binding_Core_Data::DATA_VOLATILE_GLOBAL)
                ]
            .def(constructor<Binding_Core_Data::DataType, lua_State*>())
            .def(constructor<Binding_Core_Data::DataType, std::string, lua_State*>())
            .def(constructor<Binding_Core_Data::DataType, bool, lua_State*>())
            .def(constructor<Binding_Core_Data::DataType, std::string, bool, lua_State*>())
            .def("set", &Binding_Core_Data::set)
            .def("get", static_cast<std::string(Binding_Core_Data::*)(const std::string &) const>(&Binding_Core_Data::get))
            .def("get", static_cast<luabind::object(Binding_Core_Data::*)(lua_State*) const>(&Binding_Core_Data::get))
            .def("save", static_cast<void(Binding_Core_Data::*)()>(&Binding_Core_Data::save))
            .def("save", static_cast<void(Binding_Core_Data::*)(const std::string &)>(&Binding_Core_Data::save))
            .property("dataType", &Binding_Core_Data::dataType, &Binding_Core_Data::setDataType)
            .property("sectionName", &Binding_Core_Data::sectionName, &Binding_Core_Data::setSectionName)
            .property("useSaveSlot", &Binding_Core_Data::useSaveSlot, &Binding_Core_Data::setUseSaveSlot)
    ;
}
