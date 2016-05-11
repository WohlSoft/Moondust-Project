#ifndef EPISODE_BOX_H
#define EPISODE_BOX_H

#include <QString>
#include <PGE_File_Formats/file_formats.h>

struct MusicField
{
    QString  absolutePath;
    QString *field;
};

class EpisodeBox_level{
public:
    enum fType{
        F_NONE=-1,
        F_LVL=0,
        F_LVLX=1,
        F_LVL38A=2
    };
    fType ftype;
    LevelData d;
    QString fPath;
    bool m_wasOverwritten;
    QList<MusicField> music_entries;
    EpisodeBox_level();
    EpisodeBox_level(const EpisodeBox_level&e);
    ~EpisodeBox_level();
    bool open(QString filePath);
    bool renameMusic(QString oldMus, QString newMus);
    void save();
};

class EpisodeBox_world{
public:
    WorldData d;
    QString fPath;
    bool m_wasOverwritten;
    QList<MusicField> music_entries;
    EpisodeBox_world();
    EpisodeBox_world(const EpisodeBox_world&w);
    ~EpisodeBox_world();
    bool open(QString filePath);
    bool renameMusic(QString oldMus, QString newMus);
    void save();
};

class EpisodeBox{
public:
    EpisodeBox();
    ~EpisodeBox();
    void openEpisode(QString dirPath);
    void renameMusic(QString oldMus, QString newMus);
    long overwrittenLevels();
    long overwrittenWorlds();
    QString epPath;
    QList<EpisodeBox_level> d;
    QList<EpisodeBox_world> dw;
};


#endif // EPISODE_BOX_H

