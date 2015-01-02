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

#include <QtMath>

#include <common_features/mainwinconnect.h>
#include <common_features/grid.h>
#include <common_features/item_rectangles.h>
#include <editing/edit_level/level_edit.h>
#include <file_formats/file_formats.h>

#include "../lvl_scene.h"
#include "../items/item_block.h"
#include "../items/item_water.h"
#include "../lvl_item_placing.h"

void LvlScene::applyResizers()
{
    if(pResizer!=NULL )
    {
        switch(pResizer->type)
        {
        case 4:
            setScreenshotSelector(false, true);
            break;
        case 3:
            setPhysEnvResizer(NULL, false, true);
            break;
        case 2:
            setBlockResizer(NULL, false, true);
            break;
        case 1:
            setEventSctSizeResizer(-1, false, true);
            break;
        case 0:
        default:
            MainWinConnect::pMainWin->on_applyResize_clicked();
        }
    }
}


void LvlScene::resetResizers()
{
    if(pResizer!=NULL )
    {
        switch(pResizer->type)
        {
        case 4:
            setScreenshotSelector(false, false);
            break;
        case 3:
            setPhysEnvResizer(NULL, false, false);
            break;
        case 2:
            setBlockResizer(NULL, false, false);
            break;
        case 1:
            setEventSctSizeResizer(-1, false, false);
            break;
        case 0:
        default:
            setSectionResizer(false, false);
            MainWinConnect::pMainWin->on_cancelResize_clicked();
        }
    }
}

