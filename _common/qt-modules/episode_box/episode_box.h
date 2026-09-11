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
    //! Absolute path to the music file
    QString  absolutePath;
    //! The pointer to the physical field container
    QString *field;
    //! If a part of field used, the offset
    int start = -1;
    //! If a part of field is used, the length
    int length = -1;
};

class EpisodeBox_Base
{
public:
    EpisodeBox_Base() = default;
    virtual ~EpisodeBox_Base() = default;

    QString fPath;
    QString fPathOrig;
    QString dataPath;
    bool m_wasOverwritten = false;

    void setSavePath(const QString &oldRootPath, const QString &newDirPath);

    virtual QString findFileAliasCaseInsensitive(const QString &file);

    bool renameFile(const QString &oldFile, const QString &newFile);

    virtual bool renameMusic(const QString &oldMus, const QString &newMus, bool isBulk = false);
    virtual bool renameLevel(const QString &oldLvl, const QString &newLvl, bool isBulk = false);
    virtual bool renameSFX(const QString &oldSFX, const QString &newSFX, bool isBulk = false);

    virtual void save();

protected:
    void makeSaveDir();
};

class EpisodeBox_level : public EpisodeBox_Base
{
public:
    enum fType
    {
        F_NONE = -1,
        F_LVL = 0,
        F_LVLX = 1,
        F_LVL38A = 2
    };

    fType ftype = F_NONE;
    int   ftypeVer = 0;
    LevelData d;

    void buildEntriesCache();
    QList<MusicField> music_entries;
    QList<MusicField> sound_entries;
    QList<MusicField> level_entries;

    EpisodeBox_level() = default;
    EpisodeBox_level(const EpisodeBox_level &e) = default;
    ~EpisodeBox_level() = default;

    EpisodeBox_level &operator=(const EpisodeBox_level &e) = default;

    bool open(const QString &filePath);

    QString findFileAliasCaseInsensitive(const QString &file) override;

    bool renameMusic(const QString &oldMus, const QString &newMus, bool isBulk = false) override;
    bool renameLevel(const QString &oldLvl, const QString &newLvl, bool isBulk = false) override;
    bool renameSFX(const QString &oldSFX, const QString &newSFX, bool isBulk = false) override;
    void save() override;
};

class EpisodeBox_world : public EpisodeBox_Base
{
public:
    enum fType
    {
        F_NONE = -1,
        F_WLD = 0,
        F_WLDX = 1,
        F_WLD38A = 2
    };

    fType ftype = F_NONE;
    int   ftypeVer = 0;
    WorldData d;

    void buildEntriesCache();
    QList<MusicField> music_entries;
    QList<MusicField> level_entries;

    EpisodeBox_world() = default;
    EpisodeBox_world(const EpisodeBox_world&w) = default;
    ~EpisodeBox_world() = default;

    EpisodeBox_world &operator=(const EpisodeBox_world &w) = default;

    bool open(const QString &filePath);

    QString findFileAliasCaseInsensitive(const QString &file) override;

    bool renameMusic(const QString &oldMus, const QString &newMus, bool isBulk = false) override;
    bool renameLevel(const QString &oldLvl, const QString &newLvl, bool isBulk = false) override;
    void save() override;
};

class Episode_music_ini : public EpisodeBox_Base
{
    static QString fieldToFile(const MusicField &mus);
    static void updateField(MusicField &field, const QString &newFile);

    bool m_isSoundsIni = false;
    QString m_contentPath;
public:
    Episode_music_ini() = default;
    Episode_music_ini(const Episode_music_ini &w) = default;
    ~Episode_music_ini() = default;

    Episode_music_ini &operator=(const Episode_music_ini &w) = default;

    QList<QString> file_lines;

    void buildEntriesCache();
    QList<MusicField> music_entries;

    bool open(const QString &filePath, bool isSoundsIni, const QString &contentPath = QString());

    QString findFileAliasCaseInsensitive(const QString &file) override;

    bool renameMusic(const QString &oldMus, const QString &newMus, bool isBulk = false) override;
    void save() override;
};


class EpisodeBox
{
    void addEntry(const QString &file);
public:
    EpisodeBox();
    ~EpisodeBox();

    bool openEpisode(const QString &dirPath, bool recursive);
    void clear();

    void setSavePath(const QString &newDirPath);

    QString findFileAliasCaseInsensitive(QString file);
    void renameFile(QString oldFile, QString newFile);
    void renameMusic(QString oldMus, QString newMus);
    void renameSFX(QString oldSFX, QString newSFX);
    void renameLevel(QString oldLvl, QString newLvl);
    long overwrittenLevels();
    long overwrittenWorlds();
    long overwrittenMusicInis();
    long overwrittenSoundInis();
    int  totalElements();

    QStringList getOverridenFiles(bool origPaths = false);

    // Path to the episode
    QString epPath;
    QString epPathOrig;

    //! Episode's level files
    QList<EpisodeBox_level> d;
    //! Episode's world maps
    QList<EpisodeBox_world> dw;
    //! Episode's music.ini files
    QList<Episode_music_ini> mus_ini;
    //! Episode's sounds.ini files
    QList<Episode_music_ini> snd_ini;
    //! List of all unique music files
    QSet<QString> m_musicFiles;
    //! List of all unique SFX sound files
    QSet<QString> m_soundFiles;
};


#endif // EPISODE_BOX_H

