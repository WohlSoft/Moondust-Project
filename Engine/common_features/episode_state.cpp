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
    LevelFile_hub.clear();
    replay_on_fail=false;
    game_state = FileFormats::dummySaveDataArray();
    gameType=Testing;
    LevelTargetWarp=0;
}

