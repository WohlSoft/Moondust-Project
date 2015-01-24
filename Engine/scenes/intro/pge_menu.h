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

#ifndef PGE_MENU_H
#define PGE_MENU_H

#include <QString>
#include <QList>
#include <QRect>
#include <QPoint>

#undef main
#include <SDL2/SDL_opengl.h>
#undef main

class PGE_Menuitem
{
public:
    PGE_Menuitem();
    ~PGE_Menuitem();
    PGE_Menuitem(const PGE_Menuitem &_it);
    PGE_Menuitem operator=(const PGE_Menuitem &_it)
    {
        this->title = _it.title;
        this->value = _it.value;
        this->textTexture = _it.textTexture;
        return *this;
    }

    QString title;
    QString value;
private:
    GLuint textTexture;
    friend class PGE_Menu;
};

class PGE_Menu
{
public:
    PGE_Menu();
    ~PGE_Menu();

    void addMenuItem(QString value, QString title="");

    void clear();

    void selectUp();
    void selectDown();
    void acceptItem();
    void rejectItem();
    void reset();

    void setItemsNumber(int q);

    void sort();
    void render();

    bool isSelected();
    bool isAccepted();
    const PGE_Menuitem currentItem();
    int currentItemI();
    void setCurrentItem(int i);
    int line();
    void setLine(int ln);
    int offset();
    void setOffset(int of);

    //Position and size Rectangle
    QRect rect();
    void setPos(int x, int y);
    void setPos(QPoint p);
    void setSize(int w, int h);
    void setSize(QSize s);

private:
    //QPoint renderPos;
    QRect menuRect;

    int _itemsOnScreen;
    int _currentItem;
    int _line;
    int _offset;
    bool arrowUpViz;
    bool arrowDownViz;
    bool _EndSelection;
    bool _accept;
    QList<PGE_Menuitem > _items;
    bool namefileLessThan(const PGE_Menuitem &d1, const PGE_Menuitem &d2);
    void autoOffset();

};

#endif // PGE_MENU_H
