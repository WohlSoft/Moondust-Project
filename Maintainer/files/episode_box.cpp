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
            if(musFile.isEmpty()) continue;
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
                if(musFile.isEmpty()) continue;
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
            if(lvlFile.isEmpty()) continue;
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

EpisodeBox_level::EpisodeBox_level()
{
    ftype = F_NONE;
    ftypeVer = 0;
    m_wasOverwritten = false;
}

EpisodeBox_level::EpisodeBox_level(const EpisodeBox_level &e)
{
    d             = e.d;
    ftype         = e.ftype;
    ftypeVer      = e.ftypeVer;
    fPath         = e.fPath;
    music_entries = e.music_entries;
    level_entries = e.level_entries;
    m_wasOverwritten = e.m_wasOverwritten;
}

EpisodeBox_level::~EpisodeBox_level()
{}

bool EpisodeBox_level::open(QString filePath)
{
    fPath = filePath;

    PGE_FileFormats_misc::TextFileInput file;
    if(file.open(fPath))
    {
        PGESTRING firstLine = file.read(8);
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

    return d.meta.ReadFileValid;
}

QString EpisodeBox_level::findFileAliasCaseInsensitive(QString file)
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

bool EpisodeBox_level::renameFile(QString oldFile, QString newFile)
{
    bool modified = false;
    modified |= renameMusic(oldFile, newFile);
    modified |= renameLevel(oldFile, newFile);
    return modified;
}

bool EpisodeBox_level::renameMusic(QString oldMus, QString newMus)
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
    if(modified)
    {
        save();
        m_wasOverwritten = true;
    }
    return modified;
}

bool EpisodeBox_level::renameLevel(QString oldLvl, QString newLvl)
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
    if(modified)
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
            if(musFile.isEmpty()) continue;
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
            if(lvlFile.isEmpty()) continue;
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

EpisodeBox_world::EpisodeBox_world()
{
    ftype = F_NONE;
    ftypeVer = 0;
    m_wasOverwritten = false;
}

EpisodeBox_world::EpisodeBox_world(const EpisodeBox_world &w)
{
    d           = w.d;
    fPath       = w.fPath;
    ftype       = w.ftype;
    ftypeVer    = w.ftypeVer;
    music_entries = w.music_entries;
    level_entries = w.level_entries;
    m_wasOverwritten = w.m_wasOverwritten;
}

EpisodeBox_world::~EpisodeBox_world()
{}

bool EpisodeBox_world::open(QString filePath)
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

bool EpisodeBox_world::renameFile(QString oldFile, QString newFile)
{
    bool modified = false;
    modified |= renameMusic(oldFile, newFile);
    modified |= renameLevel(oldFile, newFile);
    return modified;
}

bool EpisodeBox_world::renameMusic(QString oldMus, QString newMus)
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
    if(modified)
    {
        save();
        m_wasOverwritten = true;
    }
    return modified;
}

bool EpisodeBox_world::renameLevel(QString oldLvl, QString newLvl)
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
    if(modified)
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
            QString file = dr.relativeFilePath(dirsList.filePath());
            if(file.endsWith(".lvl", Qt::CaseInsensitive) || file.endsWith(".lvlx", Qt::CaseInsensitive))
            {
                EpisodeBox_level l;
                if(l.open(epPath + "/" + file))     //Push only valid files!!!
                {
                    d.push_back(l);
                    d.last().buildEntriesCache();
                }
            }
            else
            {
                EpisodeBox_world w;
                if(w.open(epPath + "/" + file)) //Push only valid files!!!
                {
                    dw.push_back(w);
                    dw.last().buildEntriesCache();
                }
            }
        }
    }
    else
    {
        QStringList dirList = dr.entryList(filters, QDir::Files | QDir::NoSymLinks | QDir::NoDotAndDotDot, QDir::NoSort);
        foreach(QString file, dirList)
        {
            if(file.endsWith(".lvl", Qt::CaseInsensitive) || file.endsWith(".lvlx", Qt::CaseInsensitive))
            {
                EpisodeBox_level l;
                if(l.open(epPath + "/" + file))     //Push only valid files!!!
                {
                    d.push_back(l);
                    d.last().buildEntriesCache();
                }
            }
            else
            {
                EpisodeBox_world w;
                if(w.open(epPath + "/" + file)) //Push only valid files!!!
                {
                    dw.push_back(w);
                    dw.last().buildEntriesCache();
                }
            }
        }
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
}

void EpisodeBox::renameLevel(QString oldLvl, QString newLvl)
{
    for(int i = 0; i < d.size(); i++)
        d[i].renameLevel(oldLvl, newLvl);
    for(int i = 0; i < dw.size(); i++)
        dw[i].renameLevel(oldLvl, newLvl);
}
