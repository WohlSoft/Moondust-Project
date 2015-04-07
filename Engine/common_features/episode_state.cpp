#include <PGE_File_Formats/file_formats.h>
#include "episode_state.h"

EpisodeState::EpisodeState()
{
    episodeIsStarted=false;
    replay_on_fail=false;
    game_state = FileFormats::dummySaveDataArray();
}

EpisodeState::~EpisodeState()
{

}

