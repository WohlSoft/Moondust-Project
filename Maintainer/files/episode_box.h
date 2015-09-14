#ifndef EPISODE_BOX_H
#define EPISODE_BOX_H

#include <QString>
#include <PGE_File_Formats/file_formats.h>

class EpisodeBox_level{
public:
    enum fType{
        F_NONE=-1,
        F_LVL=0,
        F_LVLX=1,
    };
    fType ftype;
    LevelData d;
    QString fPath;

    EpisodeBox_level();
    EpisodeBox_level(const EpisodeBox_level&e);
    ~EpisodeBox_level();
    bool open(QString filePath);
    void renameMusic(QString oldMus, QString newMus);
    void save();
};

class EpisodeBox_world{
public:
    WorldData d;
    QString fPath;

    EpisodeBox_world();
    EpisodeBox_world(const EpisodeBox_world&w);
    ~EpisodeBox_world();
    bool open(QString filePath);
    void save();
};

class EpisodeBox{
public:
    EpisodeBox();
    ~EpisodeBox();
    void openEpisode(QString dirPath);
    void renameMusic(QString oldMus, QString newMus, long *overwritten_levels=0, long *overwritten_worlds=0);
    QString epPath;
    QList<EpisodeBox_level> d;
    QList<EpisodeBox_world> dw;
};


#endif // EPISODE_BOX_H

