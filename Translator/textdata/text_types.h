/*
 * Moondust Translator, a free tool for internationalisation of levels and episodes
 * This is a part of the Moondust Project, a free platform for game making
 * Copyright (c) 2023-2023 Vitaly Novichkov <admin@wohlnet.ru>
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

#ifndef TEXT_TYPES_H
#define TEXT_TYPES_H

namespace TextTypes
{

enum SourceType
{
    S_WORLD = 0,
    S_LEVEL,
    S_SCRIPT
};

enum State
{
    ST_BLANK = 0,
    ST_UNFINISHED,
    ST_FINISHED,
    ST_VANISHED
};

enum L_DType
{
    LDT_EVENT = 0,
    LDT_NPC,
    LDT_TITLE
};

enum W_DType
{
    WDT_LEVEL = 0,
    WDT_TITLE,
    WDT_CREDITS
};

enum S_DType
{
    SDT_LINE = 0
};

}

#endif // TEXT_TYPES_H
