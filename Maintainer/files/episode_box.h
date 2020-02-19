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
    int   ftypeVer;
    LevelData d;
    QString fPath;
    QString dataPath;
    bool m_wasOverwritten;
    void buildEntriesCache();
    QList<MusicField> music_entries;
    QList<MusicField> level_entries;
    EpisodeBox_level();
    EpisodeBox_level(const EpisodeBox_level&e);
    ~EpisodeBox_level();
    bool open(QString filePath);
    QString findFileAliasCaseInsensitive(QString file);
    bool renameFile(QString oldFile, QString newFile);
    bool renameMusic(QString oldMus, QString newMus, bool isBulk = false);
    bool renameLevel(QString oldLvl, QString newLvl, bool isBulk = false);
    void save();
};

class EpisodeBox_world{
public:
    enum fType{
        F_NONE = -1,
        F_WLD = 0,
        F_WLDX = 1,
        F_WLD38A = 2
    };
    fType ftype;
    int   ftypeVer;
    WorldData d;
    QString fPath;
    QString dataPath;
    bool m_wasOverwritten;
    void buildEntriesCache();
    QList<MusicField> music_entries;
    QList<MusicField> level_entries;
    EpisodeBox_world();
    EpisodeBox_world(const EpisodeBox_world&w);
    ~EpisodeBox_world();
    bool open(QString filePath);
    QString findFileAliasCaseInsensitive(QString file);
    bool renameFile(QString oldFile, QString newFile);
    bool renameMusic(QString oldMus, QString newMus, bool isBulk = false);
    bool renameLevel(QString oldLvl, QString newLvl, bool isBulk = false);
    void save();
};

class EpisodeBox{
public:
    EpisodeBox();
    ~EpisodeBox();
    void openEpisode(QString dirPath, bool recursive);
    QString findFileAliasCaseInsensitive(QString file);
    void renameFile(QString oldFile, QString newFile);
    void renameMusic(QString oldMus, QString newMus);
    void renameLevel(QString oldLvl, QString newLvl);
    long overwrittenLevels();
    long overwrittenWorlds();
    int  totalElements();
    QString epPath;
    QList<EpisodeBox_level> d;
    QList<EpisodeBox_world> dw;
};


#endif // EPISODE_BOX_H

