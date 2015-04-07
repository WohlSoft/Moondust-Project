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
        EXIT_exitWithSave,
        EXIT_exitNoSave,
        EXIT_beginLevel=0
    };
};

class EpisodeState
{
public:
    EpisodeState();
    ~EpisodeState();
    bool episodeIsStarted;
    GamesaveData game_state;

    QString WorldFile;
    QString WorldPath;

    QString LevelFile;
    QString LevelPath;
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
