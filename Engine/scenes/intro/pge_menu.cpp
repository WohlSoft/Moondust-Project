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

#include "pge_menu.h"

PGE_Menu::PGE_Menu()
{
    _itemsOnScreen=5;
    _currentItem = 0;
    arrowUpViz=false;
    arrowDownViz=false;
    menuItemSeelcted=false;
}

PGE_Menu::~PGE_Menu()
{

}

void PGE_Menu::addMenuItem(QString value, QString title)
{
    PGE_Menuitem item;
    item.value = value;
    item.title = (title.isEmpty() ? value : title);
    _items.push_back(item);
}

void PGE_Menu::clear()
{

}

void PGE_Menu::selectUp()
{

}

void PGE_Menu::selectDown()
{

}

void PGE_Menu::acceptItem()
{

}

void PGE_Menu::rejectItem()
{

}

void PGE_Menu::setItemsNumber(int q)
{
    if(q>0)
        _itemsOnScreen = q;
    else
        _itemsOnScreen = 5;
}

void PGE_Menu::sort()
{
    //qSort<QList::iterator, PGE_Menuitem, PGE_Menuitem* >
    //  (_items.begin(), _items.end(), &namefileLessThan);
}

bool PGE_Menu::itemWasSelected()
{
    return menuItemSeelcted;
}

PGE_Menuitem PGE_Menu::currentItem()
{
    if(_items.size()>0)
        return _items[_currentItem];
    else
    {
        PGE_Menuitem dummy;
        dummy.title="";
        dummy.value="";
        return dummy;
    }
}

bool PGE_Menu::namefileLessThan(const PGE_Menuitem &d1, const PGE_Menuitem &d2)
{
    return d1.title<d2.title; // sort by title
}

