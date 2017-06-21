/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2017 Vitaly Novichkov <admin@wohlnet.ru>
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

#include "config_bg.h"

#include <IniProcessor/ini_processing.h>
#include <Utils/maths.h>
#include "../image_size.h"
#include "../../number_limiter.h"

#include <assert.h>

bool BgSetup::parse(IniProcessing *setup, PGEString bgoImgPath, uint32_t /*defaultGrid*/, BgSetup *merge_with, PGEString *error)
{
#define pMerge(param, def) (merge_with ? (merge_with->param) : (def))
#define pMergeMe(param) (merge_with ? (merge_with->param) : (param))


#undef pMerge
#undef pMergeMe
    return true;
}
