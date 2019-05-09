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

#ifndef EPISODESTATE_H
#define EPISODESTATE_H

#include <string>
#include <stdint.h>
#include <PGE_File_Formats/save_filedata.h>

struct PlayLevelResult
{
    std::string levelfile;
};

struct PlayEpisodeResult
{
    std::string worldfile;
    std::string savefile;
    int         character = 0;
};

class LvlExit
{
    public:
        enum ExitLevelCodes
        {
            EXIT_ReplayRequest = -5,
            EXIT_MenuExit = -3,
            EXIT_Error = -2,
            EXIT_PlayerDeath = -1,
            EXIT_Closed = -4,
            EXIT_Neutral = 0,
            EXIT_Card = 1,
            EXIT_Ball = 2,
            EXIT_OffScreen = 3,
            EXIT_Key = 4,
            EXIT_Crystal = 5,
            EXIT_Warp = 6,
            EXIT_Star = 7,
            EXIT_Tape = 8
        };
};

class WldExit
{
    public:
        enum ExitWorldCodes
        {
            EXIT_close = -2,
            EXIT_error = -1,
            EXIT_exitWithSave = 1,
            EXIT_exitNoSave = 2,
            EXIT_beginLevel = 0
        };
};

struct PlayerState
{
    uint32_t characterID = 1;
    uint32_t stateID = 1;
    saveCharState _chsetup;
};

class EpisodeState
{
public:
    EpisodeState();
    ~EpisodeState() = default;
    void reset();//!< Sets initial state of episode
    bool load();
    bool save();
    int  numOfPlayers;//!< Number of players
    bool episodeIsStarted;
    bool isEpisode;
    bool isHubLevel;
    bool isTestingModeL;
    bool isTestingModeW;
    GamesaveData game_state;

    PlayerState getPlayerState(int playerID);
    void setPlayerState(int playerID, PlayerState &state);

    saveCharState getPlayableCharacterSetup(int playerID, uint32_t characterId);
    void setPlayableCharacterSetup(int playerID, uint32_t characterId, const saveCharState &state);

    std::string WorldFile;
    std::string WorldPath;
    std::string saveFileName;
    std::string _episodePath;
    int _recent_ExitCode_level;
    int _recent_ExitCode_world;

    std::string LevelFile;
    std::string LevelFile_hub;
    std::string LevelPath;
    unsigned long LevelTargetWarp;
    int gameType;
    enum gameTypes
    {
        Testing = 0,
        Episode = 1,
        Battle,
        Race
    };
    bool replay_on_fail;
};

#endif // EPISODESTATE_H
