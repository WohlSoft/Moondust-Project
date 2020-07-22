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

#include <mainwindow.h>
#include <common_features/grid.h>
#include <common_features/item_rectangles.h>
#include <editing/edit_level/level_edit.h>
#include <PGE_File_Formats/file_formats.h>

#include "../lvl_scene.h"
#include "../items/item_block.h"
#include "../items/item_water.h"
#include "../lvl_item_placing.h"

void LvlScene::applyResizers()
{
    if(m_resizeBox != nullptr)
    {
        switch(m_resizeBox->m_resizerType)
        {
        case ItemResizer::Resizer_Capturer:
            setScreenshotSelector(false, true);
            break;
        case ItemResizer::Resizer_PhyzEnvZone:
            setPhysEnvResizer(nullptr, false, true);
            break;
        case ItemResizer::Resizer_Item:
            setBlockResizer(nullptr, false, true);
            break;
        case ItemResizer::Resizer_InEventSection:
            setEventSctSizeResizer(-1, false, true);
            break;
        case ItemResizer::Resizer_Section:
        default:
            setSectionResizer(false, true);
        }
    }
}


void LvlScene::resetResizers()
{
    if(m_resizeBox != nullptr)
    {
        switch(m_resizeBox->m_resizerType)
        {
        case ItemResizer::Resizer_Capturer:
            setScreenshotSelector(false, false);
            break;
        case ItemResizer::Resizer_PhyzEnvZone:
            setPhysEnvResizer(nullptr, false, false);
            break;
        case ItemResizer::Resizer_Item:
            setBlockResizer(nullptr, false, false);
            break;
        case ItemResizer::Resizer_InEventSection:
            setEventSctSizeResizer(-1, false, false);
            break;
        case ItemResizer::Resizer_Section:
        default:
            setSectionResizer(false, false);
        }
    }
}

