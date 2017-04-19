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

#pragma once
#ifndef ITEM_TOOLTIP_MAKE_HPP
#define ITEM_TOOLTIP_MAKE_HPP

#include <QString>

// For world map elements
template<class SetupStruct>
QString makeToolTipSimple(const QString &title, SetupStruct &setup)
{
    if(setup.description.isEmpty())
        return QString(
                   "<table>"
                   "<caption>%1</caption>"
                   "<tr><th style=\"text-align:left;\">ID:</th><td>%2</td></tr>"
                   "</table>"
               )
               .arg(title)
               .arg(setup.id);
    else
        return QString(
                   "<table>"
                   "<caption>%1</caption>"
                   "<tr><th style=\"text-align:left;\">ID:</th><td>%2</td></tr>"
                   "</table>"
                   "<br/><br/>%3"
               )
               .arg(title)
               .arg(setup.id)
               .arg(setup.description);
}

// For regular item box (Don't show group and category)

template<class SetupStruct>
QString makeToolTip(const QString &title, SetupStruct &setup)
{
    if(setup.description.isEmpty())
        return QString(
                   "<table>"
                   "<caption>%1</caption>"
                   "<tr><th style=\"text-align:left;\">ID:</th><td>%2</td></tr>"
                   "<tr><th style=\"text-align:left;\">Name:</th><td>%3</td></tr>"
                   "</table>"
               )
               .arg(title)
               .arg(setup.id)
               .arg(setup.name);
    else
        return QString(
                   "<table>"
                   "<caption>%1</caption>"
                   "<tr><th style=\"text-align:left;\">ID:</th><td>%2</td></tr>"
                   "<tr><th style=\"text-align:left;\">Name:</th><td>%3</td></tr>"
                   "</table>"
                   "<br/><br/>%4"
               )
               .arg(title)
               .arg(setup.id)
               .arg(setup.name)
               .arg(setup.description);
}

// For tileset item box (Show group and category)

template<class SetupStruct>
QString makeToolTipForTileset(const QString &title, SetupStruct &setup)
{
    if(setup.description.isEmpty())
        return QString(
                   "<table>"
                   "<caption>%1</caption>"
                   "<tr><th style=\"text-align:left;\">ID:</th><td>%2</td></tr>"
                   "<tr><th style=\"text-align:left;\">Name:</th><td>%3</td></tr>"
                   "<tr><th style=\"text-align:left;\">Group:</th><td>%4</td></tr>"
                   "<tr><th style=\"text-align:left;\">Category:</th><td>%5</td></tr>"
                   "</table>"
               )
               .arg(title)
               .arg(setup.id)
               .arg(setup.name)
               .arg(setup.group)
               .arg(setup.category);
    else
        return QString(
                   "<table>"
                   "<caption>%1</caption>"
                   "<tr><th style=\"text-align:left;\">ID:</th><td>%2</td></tr>"
                   "<tr><th style=\"text-align:left;\">Name:</th><td>%3</td></tr>"
                   "<tr><th style=\"text-align:left;\">Group:</th><td>%4</td></tr>"
                   "<tr><th style=\"text-align:left;\">Category:</th><td>%5</td></tr>"
                   "</table>"
                   "<br/><br/>%6"
               )
               .arg(title)
               .arg(setup.id)
               .arg(setup.name)
               .arg(setup.group)
               .arg(setup.category)
               .arg(setup.description);
}


#endif // ITEM_TOOLTIP_MAKE_HPP
