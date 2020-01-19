/*
 * Moondust, a free game engine for platform game making
 * Copyright (c) 2014-2020 Vitaly Novichkov <admin@wohlnet.ru>
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

#include <PGE_File_Formats/file_formats.h>
#include "episode_state.h"

#include <gui/pge_msgbox.h>
#include <Utils/files.h>


void GameUserDataManager::importData(const saveUserData &in)
{
    clear(); // Remove old data if presented

    for(auto &section : in.store)
    {
        int location_clean = (section.location & saveUserData::DATA_LOCATION_MASK);
        bool is_volatile = (section.location & saveUserData::DATA_VOLATILE_FLAG);
        std::string name = section.name.empty() ? "default" : section.name;
        DataList list;

        for(auto &entry : section.data)
            list.insert({entry.key, entry.value});

        switch(location_clean)
        {
        default:
        case saveUserData::DATA_WORLD:
            if(is_volatile)
                data_volatile_world.insert({name, list});
            else
                data_world.insert({name, list});
            break;

        case saveUserData::DATA_LEVEL:
            if(is_volatile)
                data_volatile_all_levels[section.location_name].insert({name, list});
            else
                data_all_levels[section.location_name].insert({name, list});
            break;

        case saveUserData::DATA_GLOBAL:
            if(is_volatile)
                data_volatile_global.insert({name, list});
            else
                data_global.insert({name, list});
            break;
        }
    }
}

void GameUserDataManager::exportData(saveUserData &out)
{
    out.store.clear();
    for(auto &e : data_world)
    {
        saveUserData::DataSection section;
        section.name = e.first;
        section.location = saveUserData::DATA_WORLD;
        for(auto &in_entry : e.second)
        {
            saveUserData::DataEntry entry;
            entry.key = in_entry.first;
            entry.value = in_entry.second;
            section.data.push_back(entry);
        }
        out.store.push_back(section);
    }

    for(auto &e : data_volatile_world)
    {
        saveUserData::DataSection section;
        section.name = e.first;
        section.location = saveUserData::DATA_WORLD | saveUserData::DATA_VOLATILE_FLAG;
        for(auto &in_entry : e.second)
        {
            saveUserData::DataEntry entry;
            entry.key = in_entry.first;
            entry.value = in_entry.second;
            section.data.push_back(entry);
        }
        out.store.push_back(section);
    }

    for(auto &e : data_global)
    {
        saveUserData::DataSection section;
        section.name = e.first;
        section.location = saveUserData::DATA_GLOBAL;
        for(auto &in_entry : e.second)
        {
            saveUserData::DataEntry entry;
            entry.key = in_entry.first;
            entry.value = in_entry.second;
            section.data.push_back(entry);
        }
        out.store.push_back(section);
    }

    for(auto &e : data_volatile_global)
    {
        saveUserData::DataSection section;
        section.name = e.first;
        section.location = saveUserData::DATA_GLOBAL | saveUserData::DATA_VOLATILE_FLAG;
        for(auto &in_entry : e.second)
        {
            saveUserData::DataEntry entry;
            entry.key = in_entry.first;
            entry.value = in_entry.second;
            section.data.push_back(entry);
        }
        out.store.push_back(section);
    }

    for(auto &level : data_all_levels)
    {
        for(auto &e : level.second)
        {
            saveUserData::DataSection section;
            section.name = e.first;
            section.location = saveUserData::DATA_LEVEL;
            section.location_name = level.first;
            for(auto &in_entry : e.second)
            {
                saveUserData::DataEntry entry;
                entry.key = in_entry.first;
                entry.value = in_entry.second;
                section.data.push_back(entry);
            }
            out.store.push_back(section);
        }
    }

    for(auto &level : data_volatile_all_levels)
    {
        for(auto &e : level.second)
        {
            saveUserData::DataSection section;
            section.name = e.first;
            section.location = saveUserData::DATA_LEVEL | saveUserData::DATA_VOLATILE_FLAG;
            section.location_name = level.first;
            for(auto &in_entry : e.second)
            {
                saveUserData::DataEntry entry;
                entry.key = in_entry.first;
                entry.value = in_entry.second;
                section.data.push_back(entry);
            }
            out.store.push_back(section);
        }
    }
}

void GameUserDataManager::clear()
{
    data_global.clear();
    data_volatile_global.clear();
    data_world.clear();
    data_volatile_world.clear();
    data_all_levels.clear();
    data_volatile_all_levels.clear();
}

GameUserDataManager::DataList GameUserDataManager::getSection(GameUserDataManager::DataType dataType,
                                                              const std::string &sectionName,
                                                              const std::string &fileName) const
{
    switch(dataType)
    {
    default:
    case DATA_WORLD:
    {
        const auto &f = data_world.find(sectionName);
        if(f != data_world.end())
            return f->second;
        break;
    }
    case DATA_LEVEL:
    {
        const auto &l = data_all_levels.find(fileName);
        if(l != data_all_levels.end())
        {
            const auto &f = l->second.find(sectionName);
            if(f != l->second.end())
                return f->second;
        }
        break;
    }
    case DATA_GLOBAL:
    {
        const auto &f = data_global.find(sectionName);
        if(f != data_global.end())
            return f->second;
        break;
    }
    }
    return DataList();
}

GameUserDataManager::DataList GameUserDataManager::getVolatileSection(GameUserDataManager::DataType dataType,
                                                                      const std::string &sectionName,
                                                                      const std::string &fileName) const
{
    switch(dataType)
    {
    default:
    case DATA_WORLD:
    {
        const auto &f = data_volatile_world.find(sectionName);
        if(f != data_volatile_world.end())
            return f->second;
        break;
    }
    case DATA_LEVEL:
    {
        const auto &l = data_volatile_all_levels.find(fileName);
        if(l != data_volatile_all_levels.end())
        {
            const auto &f = l->second.find(sectionName);
            if(f != l->second.end())
                return f->second;
        }
        break;
    }
    case DATA_GLOBAL:
    {
        const auto &f = data_volatile_global.find(sectionName);
        if(f != data_volatile_global.end())
            return f->second;
        break;
    }
    }
    return DataList();
}

void GameUserDataManager::setSection(GameUserDataManager::DataType dataType,
                                                              const GameUserDataManager::DataList &list,
                                                              const std::string &sectionName,
                                                              const std::string &fileName)
{
    switch(dataType)
    {
    default:
    case DATA_WORLD:
        data_world[sectionName] = list;
        break;
    case DATA_LEVEL:
        data_all_levels[fileName][sectionName] = list;
        break;
    case DATA_GLOBAL:
        data_global[sectionName] = list;
        break;
    }
}

void GameUserDataManager::setVolatileSection(GameUserDataManager::DataType dataType,
                                                                      const GameUserDataManager::DataList &list,
                                                                      const std::string &sectionName,
                                                                      const std::string &fileName)
{
    switch(dataType)
    {
    default:
    case DATA_WORLD:
        data_volatile_world[sectionName] = list;
        break;
    case DATA_LEVEL:
        data_volatile_all_levels[fileName][sectionName] = list;
        break;
    case DATA_GLOBAL:
        data_volatile_global[sectionName] = list;
        break;
    }
}




EpisodeState::EpisodeState()
{
    reset();
}

void EpisodeState::reset()
{
    m_episodeIsStarted    = false;
    m_isEpisode           = false;
    m_isHubLevel          = false;
    m_isTestingModeL      = false;
    m_isTestingModeW      = false;
    m_numOfPlayers        = 1;
    m_currentHubLevelFile.clear();
    m_autoRestartFailedLevel      = false;
    m_gameSave          = FileFormats::CreateGameSaveData();
    m_userData.clear();
    m_gameType            = Testing;
    m_nextLevelEnterWarp     = 0;
    m_lastLevelExitCode = 0;
    m_lastWorldExitCode = 0;
    m_saveFileName = "";
    m_episodePath = "";
}

bool EpisodeState::load()
{
    std::string file = m_episodePath + m_saveFileName;

    if(!Files::fileExists(file))
        return false;

    GamesaveData FileData;

    if(FileFormats::ReadExtendedSaveFileF(file, FileData))
    {
        if(FileData.meta.ReadFileValid)
        {
            m_gameSave = FileData;
            m_userData.importData(m_gameSave.userData);
            m_episodeIsStarted = true;
            return true;
        }
        else
            PGE_MsgBox::error(file + "\n" + FileData.meta.ERROR_info);
    }
    else
        PGE_MsgBox::error(file + "\n" + FileData.meta.ERROR_info);

    return false;
}

bool EpisodeState::save()
{
    if(!m_isEpisode)
        return false;
    m_userData.exportData(m_gameSave.userData);
    std::string file = m_episodePath + m_saveFileName;
    return FileFormats::WriteExtendedSaveFileF(file, m_gameSave);
}

PlayerState EpisodeState::getPlayerState(int playerID)
{
    PlayerState ch;
    ch.characterID = 1;
    ch.stateID = 1;
    ch._chsetup = FileFormats::CreateSavCharacterState();
    ch._chsetup.id = 1;
    ch._chsetup.state = 1;

    if(!m_gameSave.currentCharacter.empty()
       && (playerID > 0)
       && (playerID <= static_cast<int>(m_gameSave.currentCharacter.size())))
        ch.characterID = static_cast<uint32_t>(m_gameSave.currentCharacter[playerID - 1]);

    saveCharState st = getPlayableCharacterSetup(playerID, ch.characterID);
    ch.stateID  = static_cast<uint32_t>(st.state);
    ch._chsetup = st;

    return ch;
}

void EpisodeState::setPlayerState(int playerID, PlayerState &state)
{
    if(playerID < 1)
        return;
    if(state.characterID < 1)
        return;
    if(state.stateID < 1)
        return;

    state._chsetup.id = state.characterID;
    state._chsetup.state = state.stateID;

    //If playerID bigger than stored states - append, or replace exists
    if(playerID > static_cast<int>(m_gameSave.currentCharacter.size()))
    {
        while(playerID > static_cast<int>(m_gameSave.currentCharacter.size()))
            m_gameSave.currentCharacter.push_back(state.characterID);
    }
    else
        m_gameSave.currentCharacter[static_cast<size_t>(playerID - 1)] = state.characterID;

    setPlayableCharacterSetup(playerID, state.characterID, state._chsetup);
}

saveCharState EpisodeState::getPlayableCharacterSetup(int playerID, uint32_t characterId)
{
    if(playerID < 1)
        return {};
    if(characterId < 1)
        return {};

    for(auto &characterState : m_gameSave.characterStates)
    {
        if(characterState.id == characterId)
            return characterState;
    }
    return {};
}

void EpisodeState::setPlayableCharacterSetup(int playerID, uint32_t characterId, const saveCharState &state)
{
    if(playerID < 1)
        return;
    if(characterId < 1)
        return;

    //If characterID bigger than stored entries - append, or replace exists
    if(characterId > static_cast<unsigned long>(m_gameSave.characterStates.size()))
    {
        while(characterId > static_cast<unsigned long>(m_gameSave.characterStates.size()))
        {
            saveCharState st = FileFormats::CreateSavCharacterState();
            st.id = (static_cast<unsigned long>(m_gameSave.characterStates.size()) + 1);
            if(st.id == characterId)
                st = state;
            else
                st.state = 1;
            m_gameSave.characterStates.push_back(st);
        }
    }
    else
        m_gameSave.characterStates[static_cast<size_t>(characterId) - 1] = state;
}

std::string EpisodeState::getRelativeLevelFile()
{
    if(m_isEpisode) // get relative path to level file
        return m_nextLevelFile.substr(m_episodePath.size(), std::string::npos);
    else
        return m_nextLevelFile;
}

std::string EpisodeState::getRelativeWorldFile()
{
    if(m_isEpisode) // get relative path to level file
        return m_nextWorldFile.substr(m_episodePath.size(), std::string::npos);
    else
        return m_nextWorldFile;
}
