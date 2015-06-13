#include <PGE_File_Formats/file_formats.h>
#include "episode_state.h"

#include <gui/pge_msgbox.h>

#include <QFile>

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

    saveFileName="";
    _episodePath="";
}

bool EpisodeState::load()
{
    QString file= _episodePath+saveFileName;
    QFile _f(file);
    if(!_f.exists(file)) return false;
    if (_f.open(QIODevice::ReadOnly))
    {
        QString fileRaw;
        QTextStream inStr(&_f);
        GamesaveData FileData;
        inStr.setCodec("UTF-8");
        fileRaw = inStr.readAll();
        FileData = FileFormats::ReadExtendedSaveFile(fileRaw, file);

        if(FileData.ReadFileValid)
        {
            game_state = FileData;
            episodeIsStarted=true;
            return true;
        }
        else
        {
            PGE_MsgBox::error(file+"\n"+FileFormats::errorString);
        }
    }
    return false;
}

bool EpisodeState::save()
{
    if(!isEpisode) return false;
    QString file= _episodePath+saveFileName;
    QFile outFile(file);
    if (!outFile.open(QFile::WriteOnly | QFile::Text)) {
        return false;
    }
    QTextStream out(&outFile);
    out.setCodec("UTF-8");
    out << FileFormats::WriteExtendedSaveFile(game_state);
    outFile.close();
    return true;
}

PlayerState EpisodeState::getPlayerState(int playerID)
{
    PlayerState ch;
    ch.characterID=1;
    ch.stateID=1;
    ch._chsetup = FileFormats::dummySavCharacterState();
    ch._chsetup.id=1;
    ch._chsetup.state=1;

    if(!game_state.currentCharacter.isEmpty() && (playerID>0) && (playerID<=game_state.currentCharacter.size()) )
        ch.characterID = game_state.currentCharacter[playerID-1];

    for(int i=0;i<game_state.characterStates.size(); i++)
    {
        if(game_state.characterStates[i].id==ch.characterID)
        {
            ch.stateID=game_state.characterStates[i].state;
            ch._chsetup=game_state.characterStates[i];
            break;
        }
    }
        return ch;
}

void EpisodeState::setPlayerState(int playerID, PlayerState &state)
{
    if(playerID<1) return;
    if(state.characterID<1) return;
    if(state.stateID<1) return;

    state._chsetup.id=state.characterID;
    state._chsetup.state=state.stateID;

    if(playerID>game_state.currentCharacter.size())
    {
        while(playerID>game_state.currentCharacter.size())
            game_state.currentCharacter.push_back(state.characterID);
    }
    else
        game_state.currentCharacter[playerID-1]=state.characterID;

    bool found=false;
    for(int i=0;i<game_state.characterStates.size(); i++)
    {
        if(game_state.characterStates[i].id==state.characterID)
        {
            game_state.characterStates[i]=state._chsetup;
            found=true;
            break;
        }
    } if(!found) game_state.characterStates.push_back(state._chsetup);
}

