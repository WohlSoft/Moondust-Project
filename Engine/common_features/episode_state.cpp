#include <PGE_File_Formats/file_formats.h>
#include "episode_state.h"

EpisodeState::EpisodeState()
{
    reset();
}

EpisodeState::~EpisodeState()
{}

void EpisodeState::reset()
{
    episodeIsStarted=false;
    isEpisode=false;
    isHubLevel=false;

    numOfPlayers = 1;

    LevelFile_hub.clear();
    replay_on_fail=false;
    game_state = FileFormats::dummySaveDataArray();
    gameType=Testing;
    LevelTargetWarp=0;
}

PlayerState EpisodeState::getPlayerState(int playerID)
{
        PlayerState ch;
        ch.characterID=1;
        ch.stateID=1;
        ch._chsetup = FileFormats::dummySavCharacterState();

        if(!game_state.currentCharacter.isEmpty() && (playerID>0) && (playerID<=game_state.currentCharacter.size()) )
            ch.characterID = game_state.currentCharacter[playerID-1];
        if(!game_state.characterStates.isEmpty() && (ch.characterID>0) && (ch.characterID<=game_state.characterStates.size()) )
        {
            ch.stateID = game_state.characterStates[ch.characterID-1].state;
            ch._chsetup = game_state.characterStates[ch.characterID-1];
        }
        return ch;
}

void EpisodeState::setPlayerState(int playerID, PlayerState &state)
{
    if(playerID<1) return;
    if(state.characterID<1) return;
    if(state.stateID<1) return;

    if(playerID>game_state.currentCharacter.size())
        game_state.currentCharacter.push_back(state.characterID);
    else
        game_state.currentCharacter[playerID-1]=state.characterID;

    if(state.characterID>game_state.characterStates.size())
        game_state.characterStates.push_back(state._chsetup);
    else
        game_state.characterStates[state.characterID-1] = state._chsetup;
}

