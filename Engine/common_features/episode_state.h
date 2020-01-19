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

#ifndef EPISODESTATE_H
#define EPISODESTATE_H

#include <string>
#include <unordered_map>
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

/**
 * @brief Level exit codes
 */
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

/**
 * @brief World map exist codes
 */
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

/**
 * @brief State of player
 */
struct PlayerState
{
    //! Player's character id
    uint32_t characterID = 1;
    //! Playable character's state id
    uint32_t stateID = 1;
    //! Detail state of playable character and every it's  available state
    saveCharState _chsetup;
};

/**
 * @brief Controls user data and it's transport between episode state and the game save
 * It keeps an optimized trees which are giving a fast access to the data.
 * When data is not needed, it can be dropped into game save to be saved for a while
 * and loads data back from a game save to use them again.
 */
class GameUserDataManager
{
public:
    typedef std::unordered_map<std::string, std::string>    DataList;
    typedef std::unordered_map<std::string, DataList>       DataBank;

private:
    DataBank    data_global;
    DataBank    data_world;
    std::unordered_map<std::string, DataBank> data_all_levels;

    DataBank    data_volatile_global;
    DataBank    data_volatile_world;
    std::unordered_map<std::string, DataBank> data_volatile_all_levels;

public:
    enum DataType
    {
        DATA_LEVEL = 0,
        DATA_WORLD,
        DATA_GLOBAL
    };
    GameUserDataManager() = default;
    GameUserDataManager(const GameUserDataManager &o) = default;
    ~GameUserDataManager() = default;

    void importData(const saveUserData &in);
    void exportData(saveUserData &out);

    void clear();

    DataList getSection(DataType dataType,
                        const std::string &sectionName,
                        const std::string &fileName = std::string()) const;
    DataList getVolatileSection(DataType dataType,
                                const std::string &sectionName,
                                const std::string &fileName = std::string()) const;

    void setSection(DataType dataType,
                    const DataList &list,
                    const std::string &sectionName,
                    const std::string &fileName = std::string());
    void setVolatileSection(DataType dataType,
                            const DataList &list,
                            const std::string &sectionName,
                            const std::string &fileName = std::string());
};


/**
 * @brief Stores global episode state and manages game saves and related data
 */
class EpisodeState
{
public:
    EpisodeState();
    ~EpisodeState() = default;

    /**
     * @brief Sets initial state of episode like it playing a first time
     */
    void reset();

    /**
     * @brief Reload game save state from file
     * @return Was game save loading success?
     */
    bool load();

    /**
     * @brief Write a game save state into file
     * @return Was game save writing success?
     */
    bool save();

    /**
     * @brief Retrieve player state by player ID from a game save
     * @param playerID ID of player
     * @return Player state structure
     */
    PlayerState getPlayerState(int playerID);
    /**
     * @brief Store player state by player ID into a game save
     * @param playerID ID of player
     * @param state Player state structure
     */
    void setPlayerState(int playerID, PlayerState &state);

    /**
     * @brief Retrieve playable character's state of specific player and specific playable character from a game save
     * @param playerID ID of player
     * @param characterId playable character id of this player
     * @return Playable character's state structure
     */
    saveCharState getPlayableCharacterSetup(int playerID, uint32_t characterId);
    /**
     * @brief Store playable character's state into game save store
     * @param playerID ID of player
     * @param characterId playable character id of this player
     * @param state Playable character's state
     */
    void setPlayableCharacterSetup(int playerID, uint32_t characterId, const saveCharState &state);

    /**
     * @brief Gives a path to level file, relative at episode path
     * @return String with a path to level file, relative at episode path
     */
    std::string getRelativeLevelFile();

    /**
     * @brief Gives a path to world map file, relative at episode path
     * @return String with a path to world map file, relative at episode path
     */
    std::string getRelativeWorldFile();

    enum gameTypes
    {
        Testing = 0,
        Episode = 1,
        Battle,
        Race
    };
    //! Type of playing game
    int m_gameType = Testing;

    //! Number of players
    int  m_numOfPlayers = 1;
    //! Is episode started (true) or it's a first launch (false)?
    bool m_episodeIsStarted;
    //! Is playing level or world is in running episode (true) or it's a level/world test/debug ran (false)
    bool m_isEpisode;
    //! Is currently playing level a HUB-level (a level designed to work instead of world map to enter other levels)
    bool m_isHubLevel;
    //! Is level testing mode ran
    bool m_isTestingModeL;
    //! Is world map testing mode ran
    bool m_isTestingModeW;
    //! Automatically restart level once player have failed it (playable character was died while passing a level)
    bool m_autoRestartFailedLevel;

    //! Loadable storage of a game save state
    GamesaveData m_gameSave;

    //! Tree-built bank of user data with fast and convenient access
    GameUserDataManager m_userData;

    //! Path to world map file to start when starting a level scene
    std::string m_nextWorldFile;
    //! Path to currently running world map
    std::string m_currentWorldPath;
    //! Name of game save file
    std::string m_saveFileName;
    //! Path to currently playing episode
    std::string m_episodePath;

    //! Last exit code of recently played level
    int m_lastLevelExitCode = -1;

    //! Last exit code of recently played world map
    int m_lastWorldExitCode = -1;

    //! Path to level file to start when starting a level scene
    std::string m_nextLevelFile;
    //! Target warp ID to enter through it to destination level
    unsigned long m_nextLevelEnterWarp;
    //! Path to central hub level file
    std::string m_currentHubLevelFile;
    //! Path to currently running level file
    std::string m_currentLevelPath;
};

#endif // EPISODESTATE_H
