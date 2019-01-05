/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2014-2019 Vitaly Novichkov <admin@wohlnet.ru>
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

    uint64_t i;
    uint64_t players_total = 0;

    QString player_ini = getFullIniPath("lvl_characters.ini");
    if(player_ini.isEmpty())
        return;

    IniProcessing setup(player_ini);

    if(!openSection(&setup, "main-characters"))
        return;
    {
        setup.read("total", players_total, 0u);
    }
    closeSection(&setup);

    ConfStatus::total_characters = static_cast<long>(players_total);

    if(players_total == 0)
    {
        return;
    }

    main_characters.reserve(static_cast<int>(players_total));
    for(i = 1; i <= players_total; i++)
    {
        obj_player splayer;
        splayer.wld_offset_y = 0;

        //Default size of frame is 100x100, but re-calculates from matrix size and size of target sprite
        splayer.frame_width  = 100;
        splayer.frame_height = 100;
        splayer.statesCount  = 0;

        if(!openSection(&setup, QString("character-%1").arg(i).toStdString()))
            return;
        {
            splayer.id = i;
            setup.read("name", splayer.name, QString("player %1").arg(i));
            if(splayer.name.isEmpty())
            {
                addError(QString("Player-%1 Item name isn't defined").arg(i));
                closeSection(&setup);
                continue;
            }
            setup.read("sprite-folder", splayer.sprite_folder, QString("player-%1").arg(i));
            setup.read("sprite-folder", splayer.state_type, 0);
            setup.read("matrix-width",  splayer.matrix_width, 10);
            setup.read("matrix-height", splayer.matrix_height, 10);
            setup.read("script-file",   splayer.script, "");
            setup.read("states-number", splayer.statesCount, 0);
            if(splayer.statesCount == 0)
            {
                addError(QString("player-%1 has no states!").arg(i));
                closeSection(&setup);
                continue;
            }
        }
        closeSection(&setup);

        for(int j=1; j<=splayer.statesCount; j++)
        {
            obj_player_state pstate;
            openSection(&setup, QString("character-%1-state-%2").arg(i).arg(j).toStdString() );
            {
                setup.read("name", pstate.name, QString("State %1").arg(j));
            }
            closeSection(&setup);

            splayer.states.push_back(pstate);
        }

        main_characters.push_back(splayer);
    }

}

