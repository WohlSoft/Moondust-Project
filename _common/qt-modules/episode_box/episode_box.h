/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2014-2026 Vitaly Novichkov <admin@wohlnet.ru>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef EPISODE_BOX_H
#define EPISODE_BOX_H

#include <QString>
#include <QList>
#include <QSet>

#include <PGE_File_Formats/file_formats.h>


struct MusicField
{
    QString  absolutePath;
    QString *field;
};

class EpisodeBox_level
{
public:
    enum fType
    {
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

    EpisodeBox_level &operator=(const EpisodeBox_level &e);

    bool open(QString filePath);
    QString findFileAliasCaseInsensitive(QString file);
    bool renameFile(QString oldFile, QString newFile);
    bool renameMusic(QString oldMus, QString newMus, bool isBulk = false);
    bool renameLevel(QString oldLvl, QString newLvl, bool isBulk = false);
    void save();
};

class EpisodeBox_world
{
public:
    enum fType
    {
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

    EpisodeBox_world &operator=(const EpisodeBox_world &w);

    bool open(QString filePath);
    QString findFileAliasCaseInsensitive(QString file);
    bool renameFile(QString oldFile, QString newFile);
    bool renameMusic(QString oldMus, QString newMus, bool isBulk = false);
    bool renameLevel(QString oldLvl, QString newLvl, bool isBulk = false);
    void save();
};

class EpisodeBox
{
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

    // Path to the episode
    QString epPath;

    //! Episode's level files
    QList<EpisodeBox_level> d;
    //! Episode's world maps
    QList<EpisodeBox_world> dw;
    //! List of all unique music files
    QSet<QString> m_musicFiles;
};


#endif // EPISODE_BOX_H

