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
#include <graphics/window.h>
#include <fontman/font_manager.h>

PGE_Menu::PGE_Menu()
{
    _itemsOnScreen=5;
    _currentItem = 0;
    _line=0;
    _offset=0;
    arrowUpViz=false;
    arrowDownViz=false;
    _EndSelection=false;
    _accept=false;
    renderPos = QPoint(100,100);
    menuItemRect = QRect(0,0, 400, 50);
}

PGE_Menu::~PGE_Menu()
{
    clear();
}

void PGE_Menu::addMenuItem(QString value, QString title)
{
    PGE_Menuitem item;
    item.value = value;
    item.title = (title.isEmpty() ? value : title);
    item.textTexture = FontManager::TextToTexture(item.title,
                                                  QRect(0,0, menuItemRect.width()*2, menuItemRect.height()*2),
                                                  Qt::AlignLeft | Qt::AlignTop | Qt::TextWordWrap);
    _items.push_back(item);
}

void PGE_Menu::clear()
{
    for(int i=0;i<_items.size();i++)
    {
        glDisable(GL_TEXTURE_2D);
        glDeleteTextures(1, &_items[i].textTexture );
    }
    _items.clear();
}

void PGE_Menu::selectUp()
{
    _currentItem--;
    _line--;

    if(_line<0)
    {
        _offset--;
        _line=0;
    }

    if(_currentItem<0)
    {
        _currentItem=_items.size()-1;
        _line = _itemsOnScreen-1;
        _offset=_items.size()-_itemsOnScreen;
    }
}

void PGE_Menu::selectDown()
{
    _currentItem++;
    _line++;
    if(_line > _itemsOnScreen-1)
    {
        _offset++;
        _line = _itemsOnScreen-1;
    }

    if(_currentItem >= _items.size())
    {
        _currentItem = 0;
        _line=0;
        _offset=0;
    }
}

void PGE_Menu::acceptItem()
{
    _EndSelection=true;
    _accept=true;
}

void PGE_Menu::rejectItem()
{
    _EndSelection=true;
    _accept=false;
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
    for(int i=0; i+1<_items.size(); i++)
    {
        if(namefileLessThan(_items[i], _items[i+1]))
        {
            _items.swap(i, i+1);
            i=0;
        }
    }
}

void PGE_Menu::render()
{
    for(int i=_offset, j=0; i<_offset+_itemsOnScreen && i<_items.size(); i++, j++ )
    {
        if(i==_currentItem)
        {
            glDisable(GL_TEXTURE_2D);
            glColor4f( 1.f, 1.f, 0.f, 1.0f);
            glBegin( GL_QUADS );
                glVertex2f( renderPos.x()-20+0, renderPos.y()+0 + j*menuItemRect.height());
                glVertex2f( renderPos.x()-20+10, renderPos.y()+0 + j*menuItemRect.height());
                glVertex2f( renderPos.x()-20+10, renderPos.y()+10 + j*menuItemRect.height());
                glVertex2f( renderPos.x()-20+0, renderPos.y()+10 + j*menuItemRect.height());
            glEnd();
        }
        FontManager::SDL_string_render2D(renderPos.x(),
                                        renderPos.y() + j*menuItemRect.height(),
                                        &_items[i].textTexture);
    }
}

bool PGE_Menu::itemWasSelected()
{
    return _EndSelection;
}

bool PGE_Menu::isAccepted()
{
    return _accept;
}

const PGE_Menuitem PGE_Menu::currentItem()
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

PGE_Menuitem::PGE_Menuitem()
{
    this->title = "";
    this->value = "";
    this->textTexture = 0;
}

PGE_Menuitem::~PGE_Menuitem()
{}

PGE_Menuitem::PGE_Menuitem(const PGE_Menuitem &_it)
{
    this->title = _it.title;
    this->value = _it.value;
    this->textTexture = _it.textTexture;
}
