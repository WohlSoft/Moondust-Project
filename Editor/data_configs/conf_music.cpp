/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2014-2016 Vitaly Novichkov <admin@wohlnet.ru>
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

#include <main_window/global_settings.h>

#include "data_configs.h"

long dataconfigs::getMusLvlI(unsigned long itemID)
{
    if((itemID>0) && main_music_lvl.contains(int(itemID)))
        return long(itemID);
    return -1;
}

long dataconfigs::getMusWldI(unsigned long itemID)
{
    if((itemID>0) && main_music_wld.contains(int(itemID)))
        return long(itemID);
    return -1;
}

long dataconfigs::getMusSpcI(unsigned long itemID)
{
    if((itemID>0) && main_music_spc.contains(int(itemID)))
        return long(itemID);
    return -1;
}


void dataconfigs::loadMusic()
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

    QSettings musicset(music_ini, QSettings::IniFormat);
    musicset.setIniCodec("UTF-8");

    main_music_lvl.clear();   //Clear old
    main_music_wld.clear();   //Clear old
    main_music_spc.clear();   //Clear old

    if(!openSection(&musicset, "music-main")) return;
        music_lvl_total = musicset.value("total-level", 0).toUInt();
        music_wld_total = musicset.value("total-world", 0).toUInt();
        music_spc_total = musicset.value("total-special", 0).toUInt();

        music_custom_id     = musicset.value("level-custom-music-id", 24).toUInt();
        music_w_custom_id   = musicset.value("world-custom-music-id", 17).toUInt();
        total_data += music_lvl_total;
        total_data += music_wld_total;
        total_data += music_spc_total;
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

        if(!openSection(&musicset, QString("world-music-%1").arg(i)) )
            break;
            smusic_wld.name = musicset.value("name", "").toString();
            if(smusic_wld.name.isEmpty())
            {
                valid=false;
                addError(QString("WLD-Music-%1 Item name isn't defined").arg(i));
            }
            smusic_wld.file = musicset.value("file", "").toString();
            if(smusic_wld.file.isEmpty())
            {
                valid=false;
                addError(QString("WLD-Music-%1 Item file isn't defined").arg(i));
            }
            smusic_wld.id = i;
            main_music_wld.storeElement(int(i), smusic_wld, valid);
        closeSection(&musicset);

        if( musicset.status() != QSettings::NoError )
        {
            addError(QString("ERROR LOADING music.ini N:%1 (world music %2)").arg(musicset.status()).arg(i), PGE_LogLevel::Critical);
            break;
        }
    }

    main_music_spc.allocateSlots(int(music_spc_total));
    //Special music
    for(i=1; i<=music_spc_total; i++)
    {
        bool valid=true;
        emit progressValue(int(i));

        if(!openSection(&musicset, QString("special-music-%1").arg(i)) )
            break;

            smusic_spc.name = musicset.value("name", "").toString();
            if(smusic_spc.name.isEmpty())
            {
                valid=false;
                addError(QString("SPC-Music-%1 Item name isn't defined").arg(i));
            }
            smusic_spc.file = musicset.value("file", "").toString();
            if(smusic_spc.file.isEmpty())
            {
                valid=false;
                addError(QString("SPC-Music-%1 Item file isn't defined").arg(i));
            }
            smusic_spc.id = i;
            main_music_spc.storeElement(int(i), smusic_spc, valid);

        closeSection(&musicset);

        if( musicset.status() != QSettings::NoError )
        {
            addError(QString(QString("ERROR LOADING music.ini N:%1 (special music %2)").arg(musicset.status()).arg(i)), PGE_LogLevel::Critical);
            break;
        }
    }

    main_music_lvl.allocateSlots(int(music_lvl_total));

    //Level music
    for(i=1; i<=music_lvl_total; i++)
    {
        bool valid=true;
        emit progressValue(int(i));

        if(!openSection(&musicset, QString("level-music-%1").arg(i)) ) break;
            smusic_lvl.name = musicset.value("name", "").toString();
            if(smusic_lvl.name.isEmpty())
            {
                addError(QString("LVL-Music-%1 Item name isn't defined").arg(i));
                valid=false;
            }
            smusic_lvl.file = musicset.value("file", "").toString();
            if(smusic_lvl.file.isEmpty()&&(i != music_custom_id))
            {
                addError(QString("LVL-Music-%1 Item file isn't defined").arg(i));
                valid=false;
            }
            smusic_lvl.id = i;
            main_music_lvl.storeElement(int(i), smusic_lvl, valid);
        closeSection(&musicset);

        if( musicset.status() != QSettings::NoError )
        {
            addError(QString("ERROR LOADING music.ini N:%1 (level-music %2)").arg(musicset.status()).arg(i), PGE_LogLevel::Critical);
            break;
        }
    }


}
