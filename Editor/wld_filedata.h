#ifndef WLD_FILEDATA_H
#define WLD_FILEDATA_H

//////////////////////World file Data//////////////////////
struct WorldTiles
{
    long x;
    long y;
    unsigned long id;
};

struct WorldScenery
{
    long x;
    long y;
    unsigned long id;
};

struct WorldPaths
{
    long x;
    long y;
    unsigned long id;
};

struct WorldLevels
{
    long x;
    long y;
    unsigned long id;
    QString lvlfile;
    QString title;
    int top_exit;
    int left_exit;
    int bottom_exit;
    int right_exit;
    unsigned long entertowarp;
    bool alwaysVisible;
    bool pathbg;
    bool gamestart;
    long gotox;
    long gotoy;
    bool bigpathbg;
};

struct WorldMusic
{
    long x;
    long y;
    unsigned long id;
};

struct WorldData
{
    bool ReadFileValid;
    QString EpisodeTitle;
    bool nocharacter1;
    bool nocharacter2;
    bool nocharacter3;
    bool nocharacter4;
    bool nocharacter5;
    QString autolevel;
    bool noworldmap;
    bool restartlevel;
    unsigned int stars;

    QString author1;
    QString author2;
    QString author3;
    QString author4;
    QString author5;

    QVector<WorldTiles > tiles;
    QVector<WorldScenery > scenery;
    QVector<WorldPaths > paths;
    QVector<WorldLevels > levels;
    QVector<WorldMusic > music;

};

#endif // WLD_FILEDATA_H
