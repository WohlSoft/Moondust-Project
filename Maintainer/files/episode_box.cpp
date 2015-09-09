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
    if(fPath.endsWith(".lvlx", Qt::CaseInsensitive))
        ftype=F_LVLX;
    else if(fPath.endsWith(".lvl", Qt::CaseInsensitive))
        ftype=F_LVL;
    d = FileFormats::OpenLevelFile(filePath, true);
    qDebug()<< "Opened level, is valid ="<<d.ReadFileValid << filePath;
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
        QFile file(fPath);
        if(!file.open(QFile::WriteOnly))
        {
            return;
        }

        QString raw = FileFormats::WriteSMBX64LvlFile(d,64);
        for(int i=0; i<raw.size(); i++)
        {
            if(raw[i]=='\n')
            {
                //Force writing CRLF to prevent fakse damage of file on SMBX in Windows
                const char bytes[2] = {0x0D, 0x0A};
                file.write((const char*)(&bytes), 2);
            }
            else
            {
                const char byte[1] = {raw[i].toLatin1()};
                file.write((const char*)(&byte), 1);
            }
        }
        file.close();
    } else {
        QFile file(fPath);
        if (!file.open(QFile::WriteOnly | QFile::Text)) {
            return;
        }
        QTextStream out(&file);
        out.setCodec("UTF-8");
        out << FileFormats::WriteExtendedLvlFile(d);
        file.close();
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
    d = FileFormats::OpenWorldFile(filePath, true);
    qDebug()<< "Opened world, valud="<<d.ReadFileValid<< filePath;
    return d.ReadFileValid;
}

void EpisodeBox_world::save()
{
    if(!d.ReadFileValid) return;

    QFile file(fPath);
    if (!file.open(QFile::WriteOnly | QFile::Text)) {
        return;
    }
    QTextStream out(&file);
    out.setCodec("UTF-8");
    out << FileFormats::WriteExtendedWldFile(d);
    file.close();
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
