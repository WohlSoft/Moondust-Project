#ifndef MUSIC_PLAYER_H
#define MUSIC_PLAYER_H

#include <QString>
#include <QMediaPlayer>

class GlobalMusicPlayer
{
    public:
        static QMediaPlaylist CurrentMusic;

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
