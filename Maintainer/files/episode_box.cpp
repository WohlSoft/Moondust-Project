#include "episode_box.h"

#include <QDir>
#include <QStringList>

#include <QtDebug>

EpisodeBox_level::EpisodeBox_level()
{
    ftype = F_NONE;
}

EpisodeBox_level::EpisodeBox_level(const EpisodeBox_level &e)
{
    d=e.d;
    ftype=e.ftype;
    fPath=e.fPath;
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
    return d.ReadFileValid;
}

void EpisodeBox_level::renameMusic(QString oldMus, QString newMus)
{
    bool modified=false;
    for(int i=0;i<d.sections.size(); i++)
        {
            QString mFile = d.sections[i].music_file;
            if(mFile.isEmpty()) continue;
            mFile.replace("\\", "/");
            if(oldMus.endsWith(mFile, Qt::CaseInsensitive))
            {
                QString newMusFile=newMus;
                newMusFile.remove(d.path + "/");
                d.sections[i].music_file = newMusFile;
                modified=true;
            }
        }
    if(modified) save();
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
    d=w.d;
    fPath=w.fPath;
}

EpisodeBox_world::~EpisodeBox_world()
{}

bool EpisodeBox_world::open(QString filePath)
{
    fPath=filePath;
    FileFormats::OpenWorldFile(filePath, d);
    qDebug()<< "Opened world, valud="<<d.ReadFileValid<< filePath;
    return d.ReadFileValid;
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
    epPath=dirPath;
    QDir dr(dirPath);
    dr.setPath(dirPath);
    QStringList filters;
    //Files which are supports custom musics
    filters<<"*.lvl";
    filters<<"*.lvlx";
    filters<<"*.wldx";
    QStringList es=dr.entryList(filters, QDir::Files);
    foreach(QString file, es)
    {
        if(file.endsWith(".lvl", Qt::CaseInsensitive)||file.endsWith(".lvlx", Qt::CaseInsensitive))
        {
            EpisodeBox_level l;
            l.open(epPath+"/"+file);
            d.push_back(l);
        } else {
            EpisodeBox_world l;
            l.open(epPath+"/"+file);
            dw.push_back(l);
        }
    }
}

void EpisodeBox::renameMusic(QString oldMus, QString newMus, long *overwritten_levels, long *overwritten_worlds)
{
    for(int i=0;i<d.size(); i++)
    {
        bool modified=false;
        for(int j=0;j<d[i].d.sections.size(); j++)
        {
            QString mFile = d[i].d.sections[j].music_file;
            if(mFile.isEmpty()) continue;
            mFile.replace("\\", "/");
            if(oldMus.endsWith(mFile, Qt::CaseInsensitive))
            {
                QString newMusFile=newMus;
                newMusFile.remove(d[i].d.path + "/");
                d[i].d.sections[j].music_file = newMusFile;
                modified=true;
            }
        }
        if(modified)
        {
            if(overwritten_levels)
                (*overwritten_levels)++;
            d[i].save();
        }
    }
    for(int i=0;i<dw.size(); i++)
    {
        bool modified=false;
        for(int j=0;j<dw[i].d.music.size(); j++)
        {
            if(dw[i].d.music[j].music_file.isEmpty()) continue;
            dw[i].d.music[j].music_file.replace("\\", "/");
            if(dw[i].d.music[j].music_file.compare(oldMus, Qt::CaseInsensitive)==0)
            {
                dw[i].d.music[j].music_file=newMus;
                modified=true;
            }
        }
        if(modified)
        {
            if(overwritten_worlds)
                (*overwritten_worlds)++;
            dw[i].save();
        }
    }
}
