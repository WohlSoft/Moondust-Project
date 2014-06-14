/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2014 Vitaly Novichkov <admin@wohlnet.ru>
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

#include "data_configs.h"

#include "../main_window/global_settings.h"

void dataconfigs::loadSound()
{
    unsigned int i;

    obj_sound sound;

    unsigned long sound_total=0;

    QString sound_ini = config_dir + "sounds.ini";

    if(!QFile::exists(sound_ini))
    {
        WriteToLog(QtCriticalMsg, QString("ERROR LOADING OF sounds.ini: file not exist"));
          return;
    }

    QSettings soundset(sound_ini, QSettings::IniFormat);
    soundset.setIniCodec("UTF-8");

    main_sound.clear();   //Clear old

    soundset.beginGroup("sound-main");
        sound_total = soundset.value("total", "0").toInt();

        total_data +=sound_total;
    soundset.endGroup();

    ConfStatus::total_sound = sound_total;

    //////////////////////////////

    //Sound
    for(i=1; i<=sound_total; i++)
    {
        soundset.beginGroup( QString("sound-"+QString::number(i)) );
            sound.name = soundset.value("name", "").toString();
            sound.file = soundset.value("file", "").toString();
            sound.hidden = soundset.value("hidden", "0").toBool();
            sound.id = i;
            main_sound.push_back(sound);
        soundset.endGroup();

        if( soundset.status() != QSettings::NoError )
        {
            WriteToLog(QtCriticalMsg, QString("ERROR LOADING OF sounds.ini N:%1 (sound %2)").arg(soundset.status()).arg(i));
            break;
        }
    }

}

