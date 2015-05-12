
#include <gui/pge_msgbox.h>

#include "obj_music.h"
#include "config_manager.h"

#include <QSettings>
#include <QFileInfo>

unsigned long ConfigManager::music_custom_id;
unsigned long ConfigManager::music_w_custom_id;
QHash<int, obj_music> ConfigManager::main_music_lvl;
QHash<int, obj_music> ConfigManager::main_music_wld;
QHash<int, obj_music> ConfigManager::main_music_spc;


obj_music::obj_music()
{
    id=0;
}

bool ConfigManager::loadMusic(QString rootPath, QString iniFile, bool isCustom)
{
    unsigned int i;

    obj_music smusic_lvl;
    obj_music smusic_wld;
    obj_music smusic_spc;

    unsigned long music_lvl_total=0;
    unsigned long music_wld_total=0;
    unsigned long music_spc_total=0;

    QString music_ini = iniFile;

    if(!QFile::exists(music_ini))
    {
        addError(QString("ERROR LOADING music.ini: file does not exist"), QtCriticalMsg);
        return false;
    }

    QSettings musicset(music_ini, QSettings::IniFormat);
    musicset.setIniCodec("UTF-8");

    if(!isCustom)
    {
        main_music_lvl.clear();   //Clear old
        main_music_wld.clear();   //Clear old
        main_music_spc.clear();   //Clear old
    }

    musicset.beginGroup("music-main");
        music_lvl_total = musicset.value("total-level", "0").toInt();
        music_wld_total = musicset.value("total-world", "0").toInt();
        music_spc_total = musicset.value("total-special", "0").toInt();

        music_custom_id = musicset.value("level-custom-music-id", "24").toInt();
        music_w_custom_id = musicset.value("world-custom-music-id", "17").toInt();
        total_data +=music_lvl_total;
        total_data +=music_wld_total;
        total_data +=music_spc_total;
    musicset.endGroup();

    //////////////////////////////

    if(!isCustom && music_lvl_total==0)
    {
        PGE_MsgBox::fatal("ERROR LOADING music.ini: number of Level Music items not define, or empty config");
        return false;
    }
    if(!isCustom && music_wld_total==0)
    {
        PGE_MsgBox::fatal("ERROR LOADING music.ini: number of World Music items not define, or empty config");
        return false;
    }
    if(!isCustom && music_spc_total==0)
    {
        PGE_MsgBox::fatal("ERROR LOADING music.ini: number of Special Music items not define, or empty config");
        return false;
    }

    //World music
    for(i=1; i<=music_wld_total; i++)
    {
        musicset.beginGroup( QString("world-music-"+QString::number(i)) );
            smusic_wld.name = musicset.value("name", "").toString();
            if(smusic_wld.name.isEmpty())
            {
                if(!isCustom) //Show errors if error caused with the internal stuff folder
                {
                    addError(QString("WLD-Music-%1 Item name isn't defined").arg(i));
                }
                goto skipWldMusic;
            }

            smusic_wld.file = musicset.value("file", "").toString();
            if(smusic_wld.file.isEmpty()&&(i != music_w_custom_id))
            {
                if(!isCustom) //Show errors if error caused with the internal stuff folder
                {
                    addError(QString("WLD-Music-%1 Item file isn't defined").arg(i));
                }
                goto skipWldMusic;
            }

            smusic_wld.absPath = rootPath + smusic_wld.file;
            if((smusic_wld.id!=music_w_custom_id)&&(!QFileInfo(smusic_wld.absPath).exists()))
            {
                if(!isCustom) //Show errors if error caused with the internal stuff folder
                {
                    addError(QString("WLD-Music-%1: file %2 not exist").arg(i).arg(smusic_wld.absPath));
                }
                goto skipWldMusic;
            }

            smusic_wld.id = i;
            main_music_wld[i] = smusic_wld;
        skipWldMusic:
        musicset.endGroup();

        if( musicset.status() != QSettings::NoError )
        {
            if(!isCustom)
                addError(QString("ERROR LOADING music.ini N:%1 (world music %2)").arg(musicset.status()).arg(i), QtCriticalMsg);
            break;
        }
    }

    //Special music
    for(i=1; i<=music_spc_total; i++)
    {
        musicset.beginGroup( QString("special-music-"+QString::number(i)) );
            smusic_spc.name = musicset.value("name", "").toString();
            if(smusic_spc.name.isEmpty())
            {
                if(!isCustom) //Show errors if error caused with the internal stuff folder
                {
                    addError(QString("SPC-Music-%1 Item name isn't defined").arg(i));
                }
                goto skipSpcMusic;
            }
            smusic_spc.file = musicset.value("file", "").toString();
            if(smusic_spc.file.isEmpty())
            {
                if(!isCustom) //Show errors if error caused with the internal stuff folder
                {
                    addError(QString("SPC-Music-%1 Item file isn't defined").arg(i));
                }
                goto skipSpcMusic;
            }

            smusic_spc.absPath = rootPath + smusic_spc.file;
            if(!QFileInfo(smusic_spc.absPath).exists())
            {
                if(!isCustom) //Show errors if error caused with the internal stuff folder
                {
                    addError(QString("Special-Music-%1: file %2 not exist").arg(i).arg(smusic_spc.absPath));
                }
                goto skipSpcMusic;
            }

            smusic_spc.id = i;
            main_music_spc[i] = smusic_spc;
        skipSpcMusic:
        musicset.endGroup();

        if( musicset.status() != QSettings::NoError )
        {
            addError(QString(QString("ERROR LOADING music.ini N:%1 (special music %2)").arg(musicset.status()).arg(i)), QtCriticalMsg);
            PGE_MsgBox::error(QString(QString("ERROR LOADING music.ini N:%1 (special music %2)").arg(musicset.status()).arg(i)));
            break;
        }
    }

    //Level music
    for(i=1; i<=music_lvl_total; i++)
    {
        musicset.beginGroup( QString("level-music-"+QString::number(i)) );
            smusic_lvl.name = musicset.value("name", "").toString();
            if(smusic_lvl.name.isEmpty())
            {
                if(!isCustom) //Show errors if error caused with the internal stuff folder
                {
                    addError(QString("LVL-Music-%1 Item name isn't defined").arg(i));
                }
                goto skipLvlMusic;
            }

            smusic_lvl.file = musicset.value("file", "").toString();
            if(smusic_lvl.file.isEmpty()&&(i != music_custom_id))
            {
                if(!isCustom) //Show errors if error caused with the internal stuff folder
                {
                    addError(QString("LVL-Music-%1 Item file isn't defined").arg(i));
                }
                goto skipLvlMusic;
            }

            smusic_lvl.absPath = rootPath + smusic_lvl.file;
            if((smusic_lvl.id!=music_custom_id)&&(!QFileInfo(smusic_lvl.absPath).exists()))
            {
                if(!isCustom) //Show errors if error caused with the internal stuff folder
                {
                    addError(QString("LVL-Music-%1: file %2 not exist").arg(i).arg(smusic_lvl.absPath));
                }
                goto skipLvlMusic;
            }

            smusic_lvl.id = i;
            main_music_lvl[i] = smusic_lvl;
        skipLvlMusic:
        musicset.endGroup();

        if( musicset.status() != QSettings::NoError )
        {
            if(!isCustom)
            {
                addError(QString("ERROR LOADING music.ini N:%1 (level-music %2)").arg(musicset.status()).arg(i), QtCriticalMsg);
                PGE_MsgBox::error(QString("ERROR LOADING music.ini N:%1 (level-music %2)").arg(musicset.status()).arg(i));
            }
            break;
        }
    }

    return true;
}

QString ConfigManager::getWldMusic(unsigned long musicID, QString customMusic)
{
    if(musicID==music_w_custom_id)
        return customMusic;
    else
    if(main_music_wld.contains(musicID))
        return main_music_wld[musicID].absPath;
    else
        return "";
}

QString ConfigManager::getLvlMusic(unsigned long musicID, QString customMusic)
{
    if(musicID==music_custom_id)
        return customMusic;
    else
    if(main_music_lvl.contains(musicID))
        return main_music_lvl[musicID].absPath;
    else
        return "";
}

QString ConfigManager::getSpecialMusic(unsigned long musicID)
{
    if(main_music_spc.contains(musicID))
        return main_music_spc[musicID].absPath;
    else
        return "";
}





