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

#include "obj_player.h"
#include "data_configs.h"
#include <main_window/global_settings.h>

void dataconfigs::loadPlayers()
{
    main_characters.clear();

    unsigned long i;
    unsigned long players_total=0;

    QString player_ini = getFullIniPath("lvl_characters.ini");
    if(player_ini.isEmpty())
        return;

    QSettings setup(player_ini, QSettings::IniFormat);
    setup.setIniCodec("UTF-8");

    if(!openSection(&setup, "main-characters")) return;
        players_total = static_cast<unsigned long>(setup.value("total", 0u).toUInt());
    closeSection(&setup);

    ConfStatus::total_characters = static_cast<long>(players_total);

    if(players_total == 0)
    {
        return;
    }

    main_characters.reserve(static_cast<int>(players_total));
    for(i=1; i<=players_total; i++)
    {
        obj_player splayer;
        splayer.wld_offset_y = 0;

        //Default size of frame is 100x100, but re-calculates from matrix size and size of target sprite
        splayer.frame_width  = 100;
        splayer.frame_height = 100;
        splayer.statesCount  = 0;

        if(!openSection(&setup, QString("character-%1").arg(i)))
            return;
            splayer.id = i;
            splayer.name = setup.value("name", QString("player %1").arg(i) ).toString();
            if(splayer.name.isEmpty())
            {
                addError(QString("Player-%1 Item name isn't defined").arg(i));
                closeSection(&setup);
                continue;
            }
            splayer.sprite_folder = setup.value("sprite-folder", QString("player-%1").arg(i) ).toString();
            splayer.state_type =  setup.value("sprite-folder", 0 ).toInt();
            splayer.matrix_width = setup.value("matrix-width", 10 ).toInt();
            splayer.matrix_height = setup.value("matrix-height", 10 ).toInt();
            splayer.script =  setup.value("script-file", "" ).toString();
            splayer.statesCount = setup.value("states-number", 0 ).toInt();
            if(splayer.statesCount == 0)
            {
                addError(QString("player-%1 has no states!").arg(i));
                closeSection(&setup);
                continue;
            }
        closeSection(&setup);

        for(int j=1; j<=splayer.statesCount; j++)
        {
            obj_player_state pstate;
            openSection(&setup, QString("character-%1-state-%2").arg(i).arg(j) );
                pstate.name = setup.value("name", QString("State %1").arg(j) ).toString();
            closeSection(&setup);

            splayer.states.push_back(pstate);
        }

        main_characters.push_back(splayer);
    }

}

