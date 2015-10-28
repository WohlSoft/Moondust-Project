#ifndef EPISODESTATE_H
#define EPISODESTATE_H

#include <PGE_File_Formats/save_filedata.h>

class LvlExit
{
public:
    enum exitLevelCodes
    {
        EXIT_MenuExit=-3,
        EXIT_Error=-2,
        EXIT_PlayerDeath=-1,
        EXIT_Closed=0,
        EXIT_Card=1,
        EXIT_Ball=2,
        EXIT_OffScreen=3,
        EXIT_Key=4,
        EXIT_Crystal=5,
        EXIT_Warp=6,
        EXIT_Star=7,
        EXIT_Tape=8
    };
};

class WldExit
{
public:
    enum ExitWorldCodes
    {
        EXIT_close=-2,
        EXIT_error=-1,
        EXIT_exitWithSave=1,
        EXIT_exitNoSave=2,
        EXIT_beginLevel=0
    };
};

struct PlayerState
{
    int characterID;
    int stateID;
    saveCharState _chsetup;
};

class EpisodeState
{
public:
    EpisodeState();
    ~EpisodeState();
    void reset();//!< Sets initial state of episode
    bool load();
    bool save();
    int numOfPlayers;//!< Number of players
    bool episodeIsStarted;
    bool isEpisode;
    bool isHubLevel;
    GamesaveData game_state;

    PlayerState getPlayerState(int playerID);
    void setPlayerState(int playerID, PlayerState &state);

    QString WorldFile;
    QString WorldPath;
    QString saveFileName;
    QString _episodePath;

    QString LevelFile;
    QString LevelFile_hub;
    QString LevelPath;
        int LevelTargetWarp;
    int gameType;
    enum gameTypes
    {
        Testing=0,
        Episode=1,
        Battle,
        Race
    };
    bool replay_on_fail;
};

#endif // EPISODESTATE_H
