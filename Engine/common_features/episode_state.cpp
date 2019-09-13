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

#include <PGE_File_Formats/file_formats.h>
#include "episode_state.h"

#include <gui/pge_msgbox.h>
#include <Utils/files.h>


void GameUserDataManager::importData(const saveUserData &in)
{
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

GameUserDataManager::DataList GameUserDataManager::getSection(GameUserDataManager::DataType dataType,
                                                              const std::string &fileName)
{
    switch(dataType)
    {
    default:
    case DATA_WORLD:
    {
        const auto &f = data_world.find("default");
        if(f != data_world.end())
            return f->second;
        break;
    }
    case DATA_LEVEL:
    {
        const auto &l = data_all_levels.find(fileName);
        if(l != data_all_levels.end())
        {
            const auto &f = l->second.find("default");
            if(f != l->second.end())
                return f->second;
        }
        break;
    }
    case DATA_GLOBAL:
    {
        const auto &f = data_global.find("default");
        if(f != data_global.end())
            return f->second;
        break;
    }
    }
    return DataList();
}

GameUserDataManager::DataList GameUserDataManager::getSection(GameUserDataManager::DataType dataType,
                                                              const std::string &sectionName,
                                                              const std::string &fileName)
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
                                                                      const std::string &fileName)
{
    switch(dataType)
    {
    default:
    case DATA_WORLD:
    {
        const auto &f = data_volatile_world.find("default");
        if(f != data_volatile_world.end())
            return f->second;
        break;
    }
    case DATA_LEVEL:
    {
        const auto &l = data_volatile_all_levels.find(fileName);
        if(l != data_volatile_all_levels.end())
        {
            const auto &f = l->second.find("default");
            if(f != l->second.end())
                return f->second;
        }
        break;
    }
    case DATA_GLOBAL:
    {
        const auto &f = data_volatile_global.find("default");
        if(f != data_volatile_global.end())
            return f->second;
        break;
    }
    }
    return DataList();
}

GameUserDataManager::DataList GameUserDataManager::getVolatileSection(GameUserDataManager::DataType dataType,
                                                                      const std::string &sectionName,
                                                                      const std::string &fileName)
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
                                     const std::string &fileName)
{
    switch(dataType)
    {
    default:
    case DATA_WORLD:
        data_world["default"] = list;
        break;
    case DATA_LEVEL:
        data_all_levels[fileName]["default"] = list;
        break;
    case DATA_GLOBAL:
        data_global["default"] = list;
        break;
    }
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
                                                                      const std::string &fileName)
{
    switch(dataType)
    {
    default:
    case DATA_WORLD:
        data_volatile_world["default"] = list;
        break;
    case DATA_LEVEL:
        data_volatile_all_levels[fileName]["default"] = list;
        break;
    case DATA_GLOBAL:
        data_volatile_global["default"] = list;
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
    episodeIsStarted    = false;
    isEpisode           = false;
    isHubLevel          = false;
    isTestingModeL      = false;
    isTestingModeW      = false;
    numOfPlayers        = 1;
    LevelFile_hub.clear();
    replay_on_fail      = false;
    game_state          = FileFormats::CreateGameSaveData();
    gameType            = Testing;
    LevelTargetWarp     = 0;
    _recent_ExitCode_level = 0;
    _recent_ExitCode_world = 0;
    saveFileName = "";
    _episodePath = "";
}

bool EpisodeState::load()
{
    std::string file = _episodePath + saveFileName;

    if(!Files::fileExists(file))
        return false;

    GamesaveData FileData;

    if(FileFormats::ReadExtendedSaveFileF(file, FileData))
    {
        if(FileData.meta.ReadFileValid)
        {
            game_state = FileData;
            episodeIsStarted = true;
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
    if(!isEpisode)
        return false;
    std::string file = _episodePath + saveFileName;
    return FileFormats::WriteExtendedSaveFileF(file, game_state);
}

PlayerState EpisodeState::getPlayerState(int playerID)
{
    PlayerState ch;
    ch.characterID = 1;
    ch.stateID = 1;
    ch._chsetup = FileFormats::CreateSavCharacterState();
    ch._chsetup.id = 1;
    ch._chsetup.state = 1;

    if(!game_state.currentCharacter.empty()
       && (playerID > 0)
       && (playerID <= static_cast<int>(game_state.currentCharacter.size())))
        ch.characterID = static_cast<uint32_t>(game_state.currentCharacter[playerID - 1]);

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
    if(playerID > static_cast<int>(game_state.currentCharacter.size()))
    {
        while(playerID > static_cast<int>(game_state.currentCharacter.size()))
            game_state.currentCharacter.push_back(state.characterID);
    }
    else
        game_state.currentCharacter[static_cast<size_t>(playerID - 1)] = state.characterID;

    setPlayableCharacterSetup(playerID, state.characterID, state._chsetup);
}

saveCharState EpisodeState::getPlayableCharacterSetup(int playerID, uint32_t characterId)
{
    if(playerID < 1)
        return {};
    if(characterId < 1)
        return {};

    for(auto &characterState : game_state.characterStates)
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
    if(characterId > static_cast<unsigned long>(game_state.characterStates.size()))
    {
        while(characterId > static_cast<unsigned long>(game_state.characterStates.size()))
        {
            saveCharState st = FileFormats::CreateSavCharacterState();
            st.id = (static_cast<unsigned long>(game_state.characterStates.size()) + 1);
            if(st.id == characterId)
                st = state;
            else
                st.state = 1;
            game_state.characterStates.push_back(st);
        }
    }
    else
        game_state.characterStates[static_cast<size_t>(characterId) - 1] = state;
}
