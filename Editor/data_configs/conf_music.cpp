/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2014-2020 Vitaly Novichkov <admin@wohlnet.ru>
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

#include <QFile>
#include <QString>
#include <main_window/global_settings.h>
#include "data_configs.h"

long DataConfig::getMusLvlI(unsigned long itemID)
{
    if((itemID>0) && main_music_lvl.contains(int(itemID)))
        return long(itemID);
    return -1;
}

long DataConfig::getMusWldI(unsigned long itemID)
{
    if((itemID>0) && main_music_wld.contains(int(itemID)))
        return long(itemID);
    return -1;
}

long DataConfig::getMusSpcI(unsigned long itemID)
{
    if((itemID>0) && main_music_spc.contains(int(itemID)))
        return long(itemID);
    return -1;
}


void DataConfig::loadMusic()
{
    unsigned int i;

    obj_music smusic_lvl;
    obj_music smusic_wld;
    obj_music smusic_spc;

    unsigned long music_lvl_total=0;
    unsigned long music_wld_total=0;
    unsigned long music_spc_total=0;

    QString music_ini = getFullIniPath("music.ini");
    if(music_ini.isEmpty())
        return;

    IniProcessing musicset(music_ini);

    main_music_lvl.clear();   //Clear old
    main_music_wld.clear();   //Clear old
    main_music_spc.clear();   //Clear old

    if(!openSection(&musicset, "music-main")) return;
    {
        musicset.read("total-level", music_lvl_total, 0);
        musicset.read("total-world", music_wld_total, 0);
        musicset.read("total-special", music_spc_total, 0);

        musicset.read("level-custom-music-id", music_custom_id,     24);
        musicset.read("world-custom-music-id", music_w_custom_id,   17);
        total_data += music_lvl_total;
        total_data += music_wld_total;
        total_data += music_spc_total;
    }
    closeSection(&musicset);

    emit progressPartNumber(8);
    emit progressMax(int(music_lvl_total + music_wld_total + music_spc_total));
    emit progressValue(0);
    emit progressTitle(QObject::tr("Loading Music..."));

    ConfStatus::total_music_lvl = long(music_lvl_total);
    ConfStatus::total_music_wld = long(music_wld_total);
    ConfStatus::total_music_spc = long(music_spc_total);

    //////////////////////////////

    if(ConfStatus::total_music_lvl==0)
    {
        addError(QString("ERROR LOADING music.ini: number of Level Music items not define, or empty config"), PGE_LogLevel::Critical);
    }
    if(ConfStatus::total_music_wld==0)
    {
        addError(QString("ERROR LOADING music.ini: number of World Music items not define, or empty config"), PGE_LogLevel::Critical);
    }
    if(ConfStatus::total_music_spc==0)
    {
        addError(QString("ERROR LOADING music.ini: number of Special Music items not define, or empty config"), PGE_LogLevel::Critical);
    }

    main_music_wld.allocateSlots(int(music_wld_total));

    //World music
    for(i=1; i<=music_wld_total; i++)
    {
        bool valid=true;
        emit progressValue(int(i));

        if(!openSection(&musicset, QString("world-music-%1").arg(i).toStdString()) )
            break;
        {
            musicset.read("name", smusic_wld.name, "");
            if(smusic_wld.name.isEmpty())
            {
                valid=false;
                addError(QString("WLD-Music-%1 Item name isn't defined").arg(i));
            }

            musicset.read("file", smusic_wld.file, "");
            if(smusic_wld.file.isEmpty())
            {
                valid=false;
                addError(QString("WLD-Music-%1 Item file isn't defined").arg(i));
            }

            smusic_wld.id = i;
            main_music_wld.storeElement(int(i), smusic_wld, valid);
        }
        closeSection(&musicset);

        if( musicset.lastError() != IniProcessing::ERR_OK )
        {
            addError(QString("ERROR LOADING music.ini N:%1 (world music %2)").arg(musicset.lastError()).arg(i), PGE_LogLevel::Critical);
            break;
        }
    }

    main_music_spc.allocateSlots(int(music_spc_total));
    //Special music
    for(i=1; i<=music_spc_total; i++)
    {
        bool valid=true;
        emit progressValue(int(i));

        if(!openSection(&musicset, QString("special-music-%1").arg(i).toStdString()) )
            break;
        {
            musicset.read("name", smusic_spc.name, "");
            if(smusic_spc.name.isEmpty())
            {
                valid=false;
                addError(QString("SPC-Music-%1 Item name isn't defined").arg(i));
            }

            musicset.read("file", smusic_spc.file, "");
            if(smusic_spc.file.isEmpty())
            {
                valid=false;
                addError(QString("SPC-Music-%1 Item file isn't defined").arg(i));
            }

            smusic_spc.id = i;
            main_music_spc.storeElement(int(i), smusic_spc, valid);
        }
        closeSection(&musicset);

        if( musicset.lastError() != IniProcessing::ERR_OK )
        {
            addError(QString(QString("ERROR LOADING music.ini N:%1 (special music %2)").arg(musicset.lastError()).arg(i)), PGE_LogLevel::Critical);
            break;
        }
    }

    main_music_lvl.allocateSlots(int(music_lvl_total));

    //Level music
    for(i=1; i<=music_lvl_total; i++)
    {
        bool valid=true;
        emit progressValue(int(i));

        if(!openSection(&musicset, QString("level-music-%1").arg(i).toStdString()) )
            break;
        {
            musicset.read("name", smusic_lvl.name, "");
            if(smusic_lvl.name.isEmpty())
            {
                addError(QString("LVL-Music-%1 Item name isn't defined").arg(i));
                valid=false;
            }

            musicset.read("file", smusic_lvl.file, "");
            if(smusic_lvl.file.isEmpty()&&(i != music_custom_id))
            {
                addError(QString("LVL-Music-%1 Item file isn't defined").arg(i));
                valid=false;
            }
            smusic_lvl.id = i;
            main_music_lvl.storeElement(int(i), smusic_lvl, valid);
        }
        closeSection(&musicset);

        if( musicset.lastError() != IniProcessing::ERR_OK )
        {
            addError(QString("ERROR LOADING music.ini N:%1 (level-music %2)").arg(musicset.lastError()).arg(i), PGE_LogLevel::Critical);
            break;
        }
    }


}
