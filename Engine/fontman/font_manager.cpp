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

#include "font_manager.h"

#include <QtDebug>

void FontManager::init()
{
    int ok = TTF_Init();
    if(ok==-1)
        qDebug() << "Can't load Font manager: " << TTF_GetError() << "\n";
}

void FontManager::quit()
{
    TTF_Quit();
}

TTF_Font *FontManager::buildFont(QString _fontPath, GLint size)
{
    TTF_Font *temp_font = TTF_OpenFont(_fontPath.toLocal8Bit(), size);
    if(!temp_font)
        qDebug() << "TTF_OpenFont: " << TTF_GetError() << "\n";

    TTF_SetFontStyle( temp_font, TTF_STYLE_NORMAL );

    return temp_font;
}
