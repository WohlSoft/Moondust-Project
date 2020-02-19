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
#include <main_window/global_settings.h>
#include "data_configs.h"

long dataconfigs::getSndI(unsigned long itemID)
{
    if((itemID>0) && main_sound.contains(int(itemID)))
        return long(itemID);
    return 0;
}

void dataconfigs::loadSound()
{
    unsigned int i;

    obj_sound sound;

    unsigned long sound_total=0;

    QString sound_ini = getFullIniPath("sounds.ini");
    if(sound_ini.isEmpty())
        return;

    IniProcessing soundset(sound_ini);

    main_sound.clear();   //Clear old

    if(!openSection(&soundset, "sound-main")) return;
    {
        soundset.read("total", sound_total, 0);
        total_data +=sound_total;
    }
    closeSection(&soundset);

    emit progressPartNumber(9);
    emit progressMax(int(sound_total));
    emit progressValue(0);
    emit progressTitle(QObject::tr("Loading Sound..."));

    ConfStatus::total_sound = long(sound_total);


    if(ConfStatus::total_sound==0)
    {
        addError(QString("ERROR LOADING sounds.ini: number of items not define, or empty config"), PGE_LogLevel::Critical);
        return;
    }
    //////////////////////////////

    main_sound.allocateSlots(int(sound_total));

    //Sound
    for(i=1; i <= sound_total; i++)
    {
        bool valid=true;
        emit progressValue(int(i));

        if(!openSection(&soundset, QString("sound-%1").arg(i).toStdString()) )
            break;
        {
            soundset.read("name", sound.name, "");
            if(sound.name.isEmpty())
            {
                valid = false;
                addError(QString("Sound-%1 Item name isn't defined").arg(i));
            }

            soundset.read("file", sound.file, "");
            if(sound.file.isEmpty())
            {
                valid = false;
                addError(QString("Sound-%1 Item file isn't defined").arg(i));
            }

            sound.hidden = soundset.value("hidden", "0").toBool();
            sound.id = i;
            main_sound.storeElement(int(i), sound, valid);
        }
        closeSection(&soundset);

        if( soundset.lastError() != IniProcessing::ERR_OK )
        {
            addError(QString("ERROR LOADING sounds.ini N:%1 (sound %2)").arg(soundset.lastError()).arg(i), PGE_LogLevel::Critical);
            break;
        }
    }
}
