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

#include "episode_box.h"

#include <QDir>
#include <QStringList>
#include <QDirIterator>
#include <QtDebug>

void EpisodeBox_level::buildEntriesCache()
{
    music_entries.clear();
    level_entries.clear();
    //Collect music and level entries!
    if(d.meta.ReadFileValid)
    {
        QDir fullPath(d.meta.path);
        //From sections
        for(int i = 0; i < d.sections.size(); i++)
        {
            QString &musFile = d.sections[i].music_file;

            if(musFile.isEmpty())
                continue;

            musFile.replace('\\', '/');
            musFile.replace("//", "/");
            QString mFile = fullPath.absoluteFilePath(musFile);

            MusicField mus;
            mus.absolutePath = mFile;
            mus.field = &musFile;
            music_entries.push_back(mus);
        }

        //For custom musics per events
        for(int i = 0; i < d.events.size(); i++)
        {
            for(int j = 0; j < d.events[i].sets.size(); j++)
            {
                QString &musFile = d.events[i].sets[j].music_file;

                if(musFile.isEmpty())
                    continue;

                musFile.replace('\\', '/');
                musFile.replace("//", "/");
                QString mFile = fullPath.absoluteFilePath(musFile);

                MusicField mus;
                mus.absolutePath = mFile;
                mus.field = &musFile;
                music_entries.push_back(mus);
            }
        }

        //For level paths
        for(int i = 0; i < d.doors.size(); i++)
        {
            QString &lvlFile = d.doors[i].lname;

            if(lvlFile.isEmpty())
                continue;

            lvlFile.replace('\\', '/');
            lvlFile.replace("//", "/");
            if(!lvlFile.endsWith(".lvl", Qt::CaseInsensitive) && !lvlFile.endsWith(".lvlx", Qt::CaseInsensitive))
                lvlFile.append(".lvl");
            QString mFile = fullPath.absoluteFilePath(lvlFile);

            MusicField lvl;
            lvl.absolutePath = mFile;
            lvl.field = &lvlFile;
            level_entries.push_back(lvl);
        }
    }
}

bool EpisodeBox_level::open(const QString &filePath)
{
    fPath = filePath;

    PGE_FileFormats_misc::TextFileInput file;
    if(file.open(fPath))
    {
        PGESTRING firstLine;
        file.read(firstLine, 8);
        file.close();
        if(firstLine.startsWith("SMBXFile"))
        {
            ftype = F_LVL38A;
            FileFormats::ReadSMBX38ALvlFileF(filePath, d);
        }
        else if(PGE_FileFormats_misc::PGE_DetectSMBXFile(firstLine))
        {
            ftype = F_LVL;
            FileFormats::ReadSMBX64LvlFileF(filePath, d);
            ftypeVer = static_cast<int>(d.meta.RecentFormatVersion);
        }
        else
        {
            ftype = F_LVLX;
            FileFormats::ReadExtendedLvlFileF(filePath, d);
        }
        qDebug() << "Opened level, is valid = " << d.meta.ReadFileValid << filePath;
    }

    dataPath = d.meta.path + "/" + d.meta.filename;

    return d.meta.ReadFileValid;
}

QString EpisodeBox_level::findFileAliasCaseInsensitive(const QString &file)
{
    QDir fullPath(d.meta.path);
    for(MusicField &mus : music_entries)
    {
        if(mus.absolutePath.compare(file, Qt::CaseInsensitive) == 0)
            return *(mus.field);
    }

    for(MusicField &lvl : level_entries)
    {
        if(lvl.absolutePath.compare(file, Qt::CaseInsensitive) == 0)
            return *(lvl.field);
    }

    return QString();
}

bool EpisodeBox_level::renameFile(const QString &oldFile, const QString &newFile)
{
    bool modified = false;

    if(oldFile == fPath)
    {
        QFile::rename(fPath, newFile);
        fPath = newFile;
        QString newDataPath = newFile;
        int dotPos = newFile.lastIndexOf(".");

        if(dotPos > 0)
            newDataPath.remove(dotPos, newDataPath.size() - dotPos);

        QDir dDir(dataPath);
        if(dDir.exists())
        {
            if(dataPath != newDataPath)
                dDir.rename(dDir.path(), newDataPath);
        }
        else // Check if it's a possible case missmatch, fix it!
        {
            QDir parent = QFileInfo(fPath).absoluteDir();
            auto dirs = parent.entryList(QDir::Dirs|QDir::NoDotAndDotDot);
            for(auto &d : dirs)
            {
                QString dp = parent.path() + "/" + d;
                if(dp.compare(dataPath, Qt::CaseInsensitive) == 0)
                {
                    dDir.setPath(dp);
                    dDir.rename(dDir.path(), newDataPath);
                }
            }
        }
    }

    modified |= renameMusic(oldFile, newFile, true);
    modified |= renameLevel(oldFile, newFile, true);

    if(modified)
    {
        save();
        m_wasOverwritten = true;
    }

    return modified;
}

bool EpisodeBox_level::renameMusic(const QString &oldMus, const QString &newMus, bool isBulk)
{
    bool modified = false;
    QDir fullPath(d.meta.path);

    for(MusicField &mus : music_entries)
    {
        if(mus.absolutePath.compare(oldMus, Qt::CaseInsensitive) == 0)
        {
            *(mus.field) = fullPath.relativeFilePath(newMus);
            modified = true;
        }
    }

    if(!isBulk && modified)
    {
        save();
        m_wasOverwritten = true;
    }

    return modified;
}

bool EpisodeBox_level::renameLevel(const QString &oldLvl, const QString &newLvl, bool isBulk)
{
    bool modified = false;
    QDir fullPath(d.meta.path);

    for(MusicField &lvl : level_entries)
    {
        if(lvl.absolutePath.compare(oldLvl, Qt::CaseInsensitive) == 0)
        {
            *(lvl.field) = fullPath.relativeFilePath(newLvl);
            modified = true;
        }
    }

    if(!isBulk && modified)
    {
        save();
        m_wasOverwritten = true;
    }

    return modified;
}


void EpisodeBox_level::save()
{
    if(!d.meta.ReadFileValid) return;

    qDebug() << "reSaving Level: " << fPath << ", ftype=" << ftype;

    if(ftype == F_LVL)
    {
        if(!FileFormats::SaveLevelFile(d, fPath, FileFormats::LVL_SMBX64, 64))
            return;
    }
    else if(ftype == F_LVL38A)
    {
        if(!FileFormats::SaveLevelFile(d, fPath, FileFormats::LVL_SMBX38A))
            return;
    }
    else
    {
        if(!FileFormats::SaveLevelFile(d, fPath, FileFormats::LVL_PGEX))
            return;
    }
}




void EpisodeBox_world::buildEntriesCache()
{
    music_entries.clear();
    level_entries.clear();

    //Collect music and level entries!
    if(d.meta.ReadFileValid)
    {
        QDir fullPath(d.meta.path);
        //From music boxes
        for(int i = 0; i < d.music.size(); i++)
        {
            QString &musFile = d.music[i].music_file;

            if(musFile.isEmpty())
                continue;

            musFile.replace('\\', '/');
            musFile.replace("//", "/");
            QString mFile = fullPath.absoluteFilePath(musFile);

            MusicField mus;
            mus.absolutePath = mFile;
            mus.field = &musFile;
            music_entries.push_back(mus);
        }

        //Levels list
        for(int i = -1; i < d.levels.size(); i++)
        {
            //on -1 check intro level file! (just to have shorter code)
            QString &lvlFile = i < 0 ? d.IntroLevel_file : d.levels[i].lvlfile;

            if(lvlFile.isEmpty())
                continue;

            lvlFile.replace('\\', '/');
            lvlFile.replace("//", "/");
            if(!lvlFile.endsWith(".lvl", Qt::CaseInsensitive) && !lvlFile.endsWith(".lvlx", Qt::CaseInsensitive))
                lvlFile.append(".lvl");
            QString mFile = fullPath.absoluteFilePath(lvlFile);

            MusicField mus;
            mus.absolutePath = mFile;
            mus.field = &lvlFile;
            level_entries.push_back(mus);
        }
    }
}


bool EpisodeBox_world::open(const QString &filePath)
{
    fPath = filePath;
    FileFormats::OpenWorldFile(filePath, d);

    switch(d.meta.RecentFormat)
    {
    case FileFormats::WLD_PGEX:
        ftype = F_WLDX;
        break;
    case FileFormats::WLD_SMBX64:
        ftype = F_WLD;
        ftypeVer = static_cast<int>(d.meta.RecentFormatVersion);
        break;
    case FileFormats::WLD_SMBX38A:
        ftype = F_WLD38A;
        break;
    }

    dataPath = d.meta.path + "/" + d.meta.filename;

    qDebug() << "Opened world, valud=" << d.meta.ReadFileValid << filePath;
    return d.meta.ReadFileValid;
}

QString EpisodeBox_world::findFileAliasCaseInsensitive(QString file)
{
    QDir fullPath(d.meta.path);
    for(MusicField &mus : music_entries)
    {
        if(mus.absolutePath.compare(file, Qt::CaseInsensitive) == 0)
            return *(mus.field);
    }

    for(MusicField &lvl : level_entries)
    {
        if(lvl.absolutePath.compare(file, Qt::CaseInsensitive) == 0)
            return *(lvl.field);
    }

    return QString();
}

bool EpisodeBox_world::renameFile(const QString &oldFile, const QString &newFile)
{
    bool modified = false;
    if(oldFile == fPath)
    {
        QFile::rename(fPath, newFile);
        fPath = newFile;
        QString newDataPath = newFile;
        int dotPos = newFile.lastIndexOf(".");

        if(dotPos > 0)
            newDataPath.remove(dotPos, newDataPath.size() - dotPos);

        QDir dDir(dataPath);

        if(dDir.exists())
        {
            if(dataPath != newDataPath)
                dDir.rename(dDir.path(), newDataPath);
        }
        else // Check if it's a possible case missmatch, fix it!
        {
            QDir parent = QFileInfo(fPath).absoluteDir();
            auto dirs = parent.entryList(QDir::Dirs|QDir::NoDotAndDotDot);
            for(auto &d : dirs)
            {
                QString dp = parent.path() + "/" + d;
                if(dp.compare(dataPath, Qt::CaseInsensitive) == 0)
                {
                    dDir.setPath(dp);
                    dDir.rename(dDir.path(), newDataPath);
                }
            }
        }
    }

    modified |= renameMusic(oldFile, newFile, true);
    modified |= renameLevel(oldFile, newFile, true);

    if(modified)
    {
        save();
        m_wasOverwritten = true;
    }

    return modified;
}

bool EpisodeBox_world::renameMusic(const QString &oldMus, const QString &newMus, bool isBulk)
{
    bool modified = false;
    QDir fullPath(d.meta.path);

    for(MusicField &mus : music_entries)
    {
        if(mus.absolutePath.compare(oldMus, Qt::CaseInsensitive) == 0)
        {
            *(mus.field) = fullPath.relativeFilePath(newMus);
            modified = true;
        }
    }

    if(!isBulk && modified)
    {
        save();
        m_wasOverwritten = true;
    }

    return modified;
}

bool EpisodeBox_world::renameLevel(const QString &oldLvl, const QString &newLvl, bool isBulk)
{
    bool modified = false;
    QDir fullPath(d.meta.path);

    for(MusicField &lvl : level_entries)
    {
        if(lvl.absolutePath.compare(oldLvl, Qt::CaseInsensitive) == 0)
        {
            *(lvl.field) = fullPath.relativeFilePath(newLvl);
            modified = true;
        }
    }

    if(!isBulk && modified)
    {
        save();
        m_wasOverwritten = true;
    }

    return modified;
}

void EpisodeBox_world::save()
{
    if(!d.meta.ReadFileValid) return;

    if(ftype == F_WLD)
    {
        if(!FileFormats::SaveWorldFile(d, fPath, FileFormats::WLD_SMBX64, static_cast<unsigned int>(ftypeVer)))
            return;
    }
    else if(ftype == F_WLD38A)
    {
        if(!FileFormats::SaveWorldFile(d, fPath, FileFormats::WLD_SMBX38A))
            return;
    }
    else
    {
        if(!FileFormats::SaveWorldFile(d, fPath, FileFormats::WLD_PGEX))
            return;
    }
}



QString Episode_music_ini::fieldToFile(const MusicField &mus)
{
    return mus.field->mid(mus.start, mus.length);
}

void Episode_music_ini::updateField(MusicField &field, const QString &newFile)
{
    field.field->replace(field.start, field.length, newFile);
    field.length = newFile.size();
}

void Episode_music_ini::buildEntriesCache()
{
    bool is_music_group = false;
    music_entries.clear();
    QDir fullPath(dataPath);

    for(QString &line : file_lines)
    {
        if(line.startsWith('['))
            is_music_group = line.startsWith("[world-music-", Qt::CaseInsensitive) ||
                             line.startsWith("[level-music-", Qt::CaseInsensitive) ||
                             line.startsWith("[special-music-", Qt::CaseInsensitive);

        if(is_music_group && line.startsWith("file"))
        {
            MusicField mus;
            int phase = 0; // 0 - finding =, 1 = finding first quote, 2 - reading line up to last quote
            bool isValid = true;
            bool done = false;

            for(int i = 4; i < line.size() && !done; ++i)
            {
                QChar c = line[i];
                switch(phase)
                {
                case 0:
                    if(c.isSpace())
                        continue;

                    if(c == '=')
                    {
                        ++phase;
                        continue;
                    }

                    isValid = false;
                    break;

                case 1:
                    if(c.isSpace())
                        continue;

                    if(c == '"')
                    {
                        ++phase;
                        mus.start = i + 1;
                        continue;
                    }

                    isValid = false;
                    break;

                case 2:
                    if(c == '"')
                    {
                        mus.length = i - mus.start;
                        done = true;
                        continue;
                    }
                    break;
                }
            }

            if(!isValid)
                continue;

            QString musFile = line.mid(mus.start, mus.length);

            mus.absolutePath = fullPath.absoluteFilePath(musFile);
            mus.field = &line;
            music_entries.push_back(mus);
        }
    }
}

bool Episode_music_ini::open(const QString &filePath)
{
    QFile in(filePath);
    QFileInfo info(filePath);

    if(!in.open(QIODevice::ReadOnly|QIODevice::Text))
        return false;

    QTextStream tIn(&in);
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    tIn.setEncoding(QStringConverter::Utf8);
#else
    tIn.setCodec("UTF-8");
#endif

    music_entries.clear();
    file_lines.clear();
    QString line;

    while(!tIn.atEnd())
    {
        line = tIn.readLine();
        file_lines.push_back(line.trimmed());
    }

    in.close();

    fPath = filePath;
    dataPath = info.absoluteDir().absolutePath();

    return true;
}

QString Episode_music_ini::findFileAliasCaseInsensitive(const QString &file)
{
    QDir fullPath(dataPath);

    for(MusicField &mus : music_entries)
    {
        if(mus.absolutePath.compare(file, Qt::CaseInsensitive) == 0)
            return fieldToFile(mus);
    }

    return QString();
}

bool Episode_music_ini::renameMusic(const QString &oldMus, const QString &newMus, bool isBulk)
{
    bool modified = false;
    QDir fullPath(dataPath);

    for(MusicField &mus : music_entries)
    {
        if(mus.absolutePath.compare(oldMus, Qt::CaseInsensitive) == 0)
        {
            updateField(mus, fullPath.relativeFilePath(newMus));
            modified = true;
        }
    }

    if(!isBulk && modified)
    {
        save();
        m_wasOverwritten = true;
    }

    return modified;
}

void Episode_music_ini::save()
{
    QFile out(fPath);

    if(out.open(QIODevice::WriteOnly|QIODevice::Text))
    {
        QTextStream tOut(&out);
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
        tOut.setEncoding(QStringConverter::Utf8);
#else
        tOut.setCodec("UTF-8");
#endif

        foreach(const QString &line, file_lines)
            tOut << line << "\n";

        tOut.flush();
        out.close();
    }
}




void EpisodeBox::addEntry(const QString &file)
{
    if(file.endsWith(".lvl", Qt::CaseInsensitive) || file.endsWith(".lvlx", Qt::CaseInsensitive))
    {
        EpisodeBox_level l;
        if(l.open(epPath + "/" + file))     //Push only valid files!!!
        {
            d.push_back(std::move(l));
            EpisodeBox_level &ll = d.last();
            ll.buildEntriesCache();
            foreach(const MusicField &mus, ll.music_entries)
                m_musicFiles.insert(mus.absolutePath);
        }
    }
    else if(file.endsWith(".wld", Qt::CaseInsensitive) || file.endsWith(".wldx", Qt::CaseInsensitive))
    {
        EpisodeBox_world w;
        if(w.open(epPath + "/" + file)) //Push only valid files!!!
        {
            dw.push_back(std::move(w));
            EpisodeBox_world &ww = dw.last();
            ww.buildEntriesCache();
            foreach(const MusicField &mus, ww.music_entries)
                m_musicFiles.insert(mus.absolutePath);
        }
    }
    else if(file.compare("music.ini", Qt::CaseInsensitive) == 0 || file.endsWith("/music.ini", Qt::CaseInsensitive))
    {
        Episode_music_ini mus;
        if(mus.open(epPath + "/" + file)) //Push only valid files!!!
        {
            mus_ini.push_back(std::move(mus));
            Episode_music_ini &ww = mus_ini.last();
            ww.buildEntriesCache();
            foreach(const MusicField &mus, ww.music_entries)
                m_musicFiles.insert(mus.absolutePath);
        }
    }
}

EpisodeBox::EpisodeBox() {}

EpisodeBox::~EpisodeBox() {}

void EpisodeBox::openEpisode(QString dirPath, bool recursive)
{
    d.clear();
    dw.clear();
    epPath = dirPath;
    QDir dr(dirPath);
    dr.setPath(dirPath);
    QStringList filters;

    //Files which are supports custom musics
    filters << "*.lvl";
    filters << "*.lvlx";
    filters << "*.wld";
    filters << "*.wldx";
    filters << "music.ini";
    dr.setSorting(QDir::NoSort);
    dr.setNameFilters(filters);

    if(recursive)
    {
        QDirIterator dirsList(dirPath, filters,
                              QDir::Files | QDir::NoSymLinks | QDir::NoDotAndDotDot,
                              QDirIterator::Subdirectories);

        while(dirsList.hasNext())
        {
            dirsList.next();
            addEntry(dr.relativeFilePath(dirsList.filePath()));
        }
    }
    else
    {
        QStringList dirList = dr.entryList(filters, QDir::Files | QDir::NoSymLinks | QDir::NoDotAndDotDot, QDir::NoSort);
        foreach(const QString &file, dirList)
            addEntry(file);
    }
}

long EpisodeBox::overwrittenLevels()
{
    long count = 0;
    for(int i = 0; i < d.size(); i++)
    {
        if(d[i].m_wasOverwritten)
            count++;
    }
    return count;
}

long EpisodeBox::overwrittenWorlds()
{
    long count = 0;
    for(int i = 0; i < dw.size(); i++)
    {
        if(dw[i].m_wasOverwritten)
            count++;
    }
    return count;
}

long EpisodeBox::overwrittenMusicInis()
{
    long count = 0;

    for(int i = 0; i < mus_ini.size(); i++)
    {
        if(mus_ini[i].m_wasOverwritten)
            count++;
    }

    return count;
}

int EpisodeBox::totalElements()
{
    return d.size() + dw.size();
}

QString EpisodeBox::findFileAliasCaseInsensitive(QString file)
{
    for(int i = 0; i < d.size(); i++)
    {
        QString f = d[i].findFileAliasCaseInsensitive(file);
        if(!f.isEmpty())
            return f;
    }

    for(int i = 0; i < dw.size(); i++)
    {
        QString f = dw[i].findFileAliasCaseInsensitive(file);
        if(!f.isEmpty())
            return f;
    }
    return QString();
}

void EpisodeBox::renameFile(QString oldFile, QString newFile)
{
    for(int i = 0; i < d.size(); i++)
        d[i].renameFile(oldFile, newFile);

    for(int i = 0; i < dw.size(); i++)
        dw[i].renameFile(oldFile, newFile);
}

void EpisodeBox::renameMusic(QString oldMus, QString newMus)
{
    for(int i = 0; i < d.size(); i++)
        d[i].renameMusic(oldMus, newMus);

    for(int i = 0; i < dw.size(); i++)
        dw[i].renameMusic(oldMus, newMus);

    for(int i = 0; i < mus_ini.size(); i++)
        mus_ini[i].renameMusic(oldMus, newMus);
}

void EpisodeBox::renameLevel(QString oldLvl, QString newLvl)
{
    for(int i = 0; i < d.size(); i++)
        d[i].renameLevel(oldLvl, newLvl);

    for(int i = 0; i < dw.size(); i++)
        dw[i].renameLevel(oldLvl, newLvl);
}
