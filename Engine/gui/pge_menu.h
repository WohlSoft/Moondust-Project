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
#include <common_features/pge_texture.h>
#include <functional>

class PGE_Menu;

class PGE_Menuitem
{
public:
    PGE_Menuitem();
    virtual ~PGE_Menuitem();
    PGE_Menuitem(const PGE_Menuitem &_it);
    PGE_Menuitem operator=(const PGE_Menuitem &_it)
    {
        this->title = _it.title;
        this->value = _it.value;
        this->textTexture = _it.textTexture;
        return *this;
    }

    virtual void left();
    virtual void right();
    virtual void toggle();

    virtual void render(int x, int y);

    QString title;
    QString value;
    enum itemType{
        ITEM_Normal=0,
        ITEM_Bool,
        ITEM_Int,
        ITEM_KeyGrab,
        ITEM_StrList
    };

    itemType type;
protected:
    std::function<void()> extAction;

private:
    GLuint textTexture;
    friend class PGE_Menu;
};

class PGE_BoolMenuItem : public PGE_Menuitem
{
public:
    PGE_BoolMenuItem();
    PGE_BoolMenuItem(const PGE_BoolMenuItem &it);
    ~PGE_BoolMenuItem();
    void left();
    void right();
    void render(int x, int y);
    void toggle();

private:
    bool *flag;
    friend class PGE_Menu;
};


class PGE_IntMenuItem : public PGE_Menuitem
{
public:
    PGE_IntMenuItem();
    PGE_IntMenuItem(const PGE_IntMenuItem &it);
    ~PGE_IntMenuItem();
    void left();
    void right();
    void render(int x, int y);

private:
    int *intvalue;
    int min;
    int max;
    bool allowRotation;
    friend class PGE_Menu;
};

class PGE_KeyGrabMenuItem : public PGE_Menuitem
{
public:
    PGE_KeyGrabMenuItem();
    PGE_KeyGrabMenuItem(const PGE_KeyGrabMenuItem &it);
    ~PGE_KeyGrabMenuItem();
    void grabKey();
    void pushKey(int scancode);
    void render(int x, int y);

private:
    bool chosing;
    int *keyValue;
    PGE_Menu* menu;
    friend class PGE_Menu;
};


class PGE_Menu
{
friend class PGE_KeyGrabMenuItem;
public:
    PGE_Menu();
    ~PGE_Menu();

    void addMenuItem(QString value, QString title="",
                     std::function<void()> _extAction=([]()->void{}));
    void addBoolMenuItem(bool *flag, QString value, QString title="",
                         std::function<void()> _extAction=([]()->void{}));
    void addIntMenuItem(int *intvalue, int min, int max, QString value, QString title, bool rotate=false,
                        std::function<void()> _extAction=([]()->void{}) );
    void addKeyGrabMenuItem(int *keyvalue, QString value, QString title);

    void clear(); //!< Clean all menuitems

    void selectUp();   //!< move selection cursor up
    void selectDown(); //!< move selection cursor down
    void scrollUp();   //!< Scroll by mousewheel
    void scrollDown(); //!< Scroll by mousewheel
    void selectLeft(); //!< switch to left  (for lists)
    void selectRight();//!< switch to right (for lists)
    void acceptItem(); //!< Accept currently selected item
    void rejectItem(); //!< Reject menu
    void resetState(); //!< Reset state after acception or rejection
    void setMouseHoverPos(int x, int y);/*!< Set hovering mouse position.
                                          Will be selected item which under mouse*/

    void setMouseClickPos(int x, int y);
    int findItem(int x, int y);

    void reset();   //!< Reset menu to initial state

    void setItemsNumber(int q); //!< Set number of item which will show on screen

    void sort();   //!< Sort menu items in alphabetic order
    void render(); //!< Draw menu on screen

    bool isSelected();  //!< Is menu was accepted or rejected
    bool isAccepted();  //!< Is menu was accepted, else rejected
    bool isKeyGrabbing(); //!< Is a key grabbing mode
    void storeKey(int scancode);
    const PGE_Menuitem currentItem(); //!< Returns current menu item entry
    int currentItemI();       //!< Returns index of current menu item
    void setCurrentItem(int i); //!< Sets current index of menuitem
    int line();  //!< Returns number of current line where cursor is located on screen
    void setLine(int ln); //!< Sets current line number
    int offset();          //!< Returns scrolling offset from begin of menu list
    void setOffset(int of);//!< Sets scrolling offset from begin of menu list

    //Position and size Rectangle
    QRect rect(); //!< Returns rectangle of menu box
    void setPos(int x, int y); //!< Sets current position of menu box
    void setPos(QPoint p);     //!< Sets current position of menu box
    void setSize(int w, int h); //!< Sets size of menu box
    void setSize(QSize s);      //!< Sets size of menu box


private:
    QRect menuRect;

    /*******Key grabbing********/
    PGE_KeyGrabMenuItem *m_item;
    bool is_keygrab;
    /*******Key grabbing********/

    int _itemsOnScreen;
    int _currentItem;
    int _line;
    int _offset;
    bool arrowUpViz;
    bool arrowDownViz;
    bool _EndSelection;
    bool _accept;
    QList<PGE_BoolMenuItem > _items_bool;
    QList<PGE_IntMenuItem > _items_int;
    QList<PGE_Menuitem > _items_normal;
    QList<PGE_KeyGrabMenuItem > _items_keygrabs;

    QList<PGE_Menuitem *> _items;
    bool namefileLessThan(const PGE_Menuitem *d1, const PGE_Menuitem *d2);
    void autoOffset();
    PGE_Texture _selector;
    PGE_Texture _scroll_up;
    PGE_Texture _scroll_down;

};

#endif // PGE_MENU_H
