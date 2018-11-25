#include <PGE_File_Formats/file_formats.h>
#include "episode_state.h"

#include <gui/pge_msgbox.h>
#include <Utils/files.h>

EpisodeState::EpisodeState()
{
    reset();
}

EpisodeState::~EpisodeState()
{}

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
