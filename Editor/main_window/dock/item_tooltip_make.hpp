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

#pragma once
#ifndef ITEM_TOOLTIP_MAKE_HPP
#define ITEM_TOOLTIP_MAKE_HPP

#include <QString>

// For world map elements
template<class SetupStruct>
QString makeToolTipSimple(const QString &title, SetupStruct &setup)
{
    QString out = QString(
                        "<h2>%1</h2>"
                        "<table>"
                        "<tr><th style=\"text-align:left;\">ID:</th><td>%2</td></tr>"
                        "</table>"
                   )
                   .arg(setup.name.isEmpty() ? title : setup.name)
                   .arg(setup.id);

    if(!setup.description.isEmpty())
        out.append(QString("<br/><br/>%1").arg(setup.description));

    return out;
}

// For regular item box (Don't show group and category)

template<class SetupStruct>
QString makeToolTip(const QString &title, SetupStruct &setup)
{
    QString out = QString(
                        "<h2>%1</h2>"
                        "<table>"
                        "<tr><th style=\"text-align:left;\">ID:</th><td>%2-%3</td></tr>"
                        "</table>")
                    .arg(setup.name)
                    .arg(title).arg(setup.id);

    if(!setup.description.isEmpty())
        out.append(QString("<br/><br/>%1").arg(setup.description));

    return out;
}

// For tileset item box (Show group and category)

template<class SetupStruct>
QString makeToolTipForTileset(const QString &title, SetupStruct &setup)
{
    QString out = QString(
                        "<h2>%1</h2>"
                        "<table>"
                        "<tr><th style=\"text-align:left;\">ID:</th><td>%2-%3</td></tr>"
                        "<tr><th style=\"text-align:left;\">Group:</th><td>%4</td></tr>"
                        "<tr><th style=\"text-align:left;\">Category:</th><td>%5</td></tr>"
                        "</table>"
                   )
                    .arg(setup.name)
                    .arg(title)
                    .arg(setup.id)
                    .arg(setup.group)
                    .arg(setup.category);

    if(!setup.description.isEmpty())
        out.append(QString("<br/><br/>%1").arg(setup.description));

    return out;
}


#endif // ITEM_TOOLTIP_MAKE_HPP
