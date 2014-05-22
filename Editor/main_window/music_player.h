#ifndef MUSIC_PLAYER_H
#define MUSIC_PLAYER_H

#include <QString>

class GlobalMusicPlayer
{
    public:

};

class LvlMusPlay
{
public:
    static QString currentCustomMusic;
    static long currentMusicId;
    static bool musicButtonChecked;
    static bool musicForceReset;
};

#endif // MUSIC_PLAYER_H
