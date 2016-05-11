#include "episode_box.h"

#include <QDir>
#include <QStringList>
#include <QDirIterator>
#include <QtDebug>

EpisodeBox_level::EpisodeBox_level()
{
    ftype = F_NONE;
    m_wasOverwritten = false;
}

EpisodeBox_level::EpisodeBox_level(const EpisodeBox_level &e)
{
    d             = e.d;
    ftype         = e.ftype;
    fPath         = e.fPath;
    music_entries = e.music_entries;
    m_wasOverwritten = e.m_wasOverwritten;
}

EpisodeBox_level::~EpisodeBox_level()
{}

bool EpisodeBox_level::open(QString filePath)
{
    fPath=filePath;

    PGE_FileFormats_misc::TextFileInput file;
    if(file.open(fPath))
    {
        PGESTRING firstLine = file.read(8);
        file.close();
        if( PGE_StartsWith( firstLine, "SMBXFile" ) )
        {
            ftype = F_LVL38A;
            FileFormats::ReadSMBX38ALvlFileF(filePath, d);
        }
        else if( PGE_DetectSMBXFile( firstLine ) )
        {
            ftype = F_LVL;
            FileFormats::ReadSMBX64LvlFileF(filePath, d);
        }
        else
        {
            ftype = F_LVLX;
            FileFormats::ReadExtendedLvlFileF(filePath, d);
        }
        qDebug()<< "Opened level, is valid = " << d.ReadFileValid << filePath;
    }

    //Collect music entries!
    if(d.ReadFileValid)
    {
        QDir fullPath(d.path);
        //From sections
        for(int i=0; i<d.sections.size(); i++)
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
        for(int i=0; i < d.events.size(); i++)
        {
            for(int j=0;j < d.events[i].sets.size(); j++)
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
    }

    return d.ReadFileValid;
}

bool EpisodeBox_level::renameMusic(QString oldMus, QString newMus)
{
    bool modified=false;
    QDir fullPath(d.path);
    for(int i=0; i < music_entries.size(); i++)
    {
        MusicField &mus = music_entries[i];
        if( mus.absolutePath.compare(oldMus, Qt::CaseInsensitive) == 0 )
        {
            *(mus.field) = fullPath.relativeFilePath( newMus );
            modified=true;
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
    if(!d.ReadFileValid) return;

    qDebug() << "reSaving Level: " << fPath << ", ftype="<<ftype;

    if(ftype==F_LVL)
    {
        if(!FileFormats::SaveLevelFile(d, fPath, FileFormats::LVL_SMBX64, 64))
            return;
    }
    else if(ftype==F_LVL38A)
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


EpisodeBox_world::EpisodeBox_world()
{}

EpisodeBox_world::EpisodeBox_world(const EpisodeBox_world &w)
{
    d       = w.d;
    fPath   = w.fPath;
    music_entries = w.music_entries;
    m_wasOverwritten = w.m_wasOverwritten;
}

EpisodeBox_world::~EpisodeBox_world()
{}

bool EpisodeBox_world::open(QString filePath)
{
    fPath=filePath;
    FileFormats::OpenWorldFile(filePath, d);

    qDebug()<< "Opened world, valud=" << d.ReadFileValid << filePath;
    //Collect music entries!
    if(d.ReadFileValid)
    {
        QDir fullPath(d.path);
        //From music boxes
        for(int i=0; i<d.music.size(); i++)
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
    }
    return d.ReadFileValid;
}

bool EpisodeBox_world::renameMusic(QString oldMus, QString newMus)
{
    bool modified=false;
    QDir fullPath(d.path);
    for(int i=0; i < music_entries.size(); i++)
    {
        MusicField &mus = music_entries[i];
        if( mus.absolutePath.compare(oldMus, Qt::CaseInsensitive) == 0 )
        {
            *(mus.field) = fullPath.relativeFilePath( newMus );
            modified=true;
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
    if(!d.ReadFileValid) return;
    FileFormats::WriteExtendedWldFileF(fPath, d);
}


EpisodeBox::EpisodeBox() {}

EpisodeBox::~EpisodeBox(){}

void EpisodeBox::openEpisode(QString dirPath)
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
    filters << "*.wldx";
    dr.setSorting( QDir::NoSort );
    dr.setNameFilters( filters );
    QDirIterator dirsList( dirPath, filters,
                          QDir::Files|QDir::NoSymLinks|QDir::NoDotAndDotDot,
                          QDirIterator::Subdirectories );
    while( dirsList.hasNext() )
    {
        dirsList.next();
        QString file = dr.relativeFilePath( dirsList.filePath() );
        if(file.endsWith(".lvl", Qt::CaseInsensitive) || file.endsWith(".lvlx", Qt::CaseInsensitive))
        {
            EpisodeBox_level l;
            if( l.open( epPath + "/" + file ) ) //Push only valid files!!!
                d.push_back(l);
        } else {
            EpisodeBox_world l;
            if( l.open(epPath+"/"+file) ) //Push only valid files!!!
                dw.push_back(l);
        }
    }
}

long EpisodeBox::overwrittenLevels()
{
    long count=0;
    for(int i=0;i<d.size(); i++)
    {
        if(d[i].m_wasOverwritten)
            count++;
    }
    return count;
}

long EpisodeBox::overwrittenWorlds()
{
    long count=0;
    for(int i=0;i<dw.size(); i++)
    {
        if(dw[i].m_wasOverwritten)
            count++;
    }
    return count;
}

void EpisodeBox::renameMusic(QString oldMus, QString newMus)
{
    for(int i=0;i<d.size(); i++)
    {
        d[i].renameMusic(oldMus, newMus);
    }
    for(int i=0;i<dw.size(); i++)
    {
        dw[i].renameMusic(oldMus, newMus);
    }
}
