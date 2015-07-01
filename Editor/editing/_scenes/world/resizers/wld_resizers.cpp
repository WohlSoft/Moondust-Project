/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2014-2015 Vitaly Novichkov <admin@wohlnet.ru>
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

#include <common_features/mainwinconnect.h>
#include <common_features/grid.h>
#include <editing/edit_world/world_edit.h>
#include <PGE_File_Formats/file_formats.h>

#include "../wld_scene.h"
#include "../wld_item_placing.h"



void WldScene::applyResizers()
{
    if(pResizer!=NULL )
    {
        switch(pResizer->type)
        {
        case 0:
            setScreenshotSelector(false, true);
            break;
        default:
            break;
        }
    }
}


void WldScene::resetResizers()
{
    if(pResizer!=NULL )
    {
        switch(pResizer->type)
        {
        case 0:
            setScreenshotSelector(false, false);
            break;
        default:
            break;
        }
    }
}



