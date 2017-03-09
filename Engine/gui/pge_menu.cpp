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

#include "pge_menu.h"
#include <graphics/window.h>
#include <graphics/gl_renderer.h>
#include <fontman/font_manager.h>
#include <common_features/graphics_funcs.h>
#include <data_configs/config_manager.h>
#include <audio/pge_audio.h>
#include <controls/controller_key_map.h>

PGE_Menu::PGE_Menu(menuAlignment align, int itemGap)
{
    alignment = align;
    _itemsOnScreen = 5;
    _currentItem = 0;
    _line = 0;
    _offset = 0;
    arrowUpViz = false;
    arrowDownViz = false;
    _EndSelection = false;
    _accept = false;
    is_keygrab = false;
    m_item = nullptr;

    menuRect.setRect(260, 380, 350, ConfigManager::setup_menus.item_height);
    _item_height = ConfigManager::setup_menus.item_height;
    _width_limit = PGE_Window::Width - 100;
    _text_len_limit = 0;
    menuItemGap = itemGap;

    /// Init menu font
    ConfigManager::setup_menus.font_id = FontManager::getFontID(ConfigManager::setup_menus.font_name);
    _font_id = ConfigManager::setup_menus.font_id;
    _font_offset = ConfigManager::setup_menus.font_offset;

    if(ConfigManager::setup_menus.selector.empty())
        _selector.w = 0;
    else
        GlRenderer::loadTextureP(_selector, ConfigManager::setup_menus.selector);

    if(ConfigManager::setup_menus.scrollerUp.empty())
        _scroll_up.w = 0;
    else
        GlRenderer::loadTextureP(_scroll_up, ConfigManager::setup_menus.scrollerUp);

    if(ConfigManager::setup_menus.scrollerDown.empty())
        _scroll_down.w = 0;
    else
        GlRenderer::loadTextureP(_scroll_down, ConfigManager::setup_menus.scrollerDown);
}

PGE_Menu::PGE_Menu(const PGE_Menu &menu)
{
    menuRect =   menu.menuRect;

    /*******Key grabbing********/
    m_item     = menu.m_item;
    is_keygrab = menu.is_keygrab;
    /*******Key grabbing********/

    alignment      = menu.alignment;
    _itemsOnScreen = menu._itemsOnScreen;
    _currentItem   = menu._currentItem;
    _line          = menu._line;
    _offset        = menu._offset;
    arrowUpViz     = menu.arrowUpViz;
    arrowDownViz   = menu.arrowDownViz;
    _EndSelection  = menu._EndSelection;
    _accept        = menu._accept;
    _items_bool    = menu._items_bool;
    _items_int     = menu._items_int;
    _items_named_int = menu._items_named_int;
    _items_normal   = menu._items_normal;
    _items_keygrabs = menu._items_keygrabs;

    _items  = menu._items;
    _items_index = menu._items_index;
    _selector = menu._selector;
    _scroll_up = menu._selector;
    _scroll_down = menu._scroll_down;
    _item_height = menu._item_height;
    _width_limit = menu._width_limit;
    _text_len_limit = menu._text_len_limit;
    _text_len_limit_strict = menu._text_len_limit_strict;
    menuItemGap = menu.menuItemGap;

    _font_id = menu._font_id;
    _font_offset = menu._font_offset;
}

PGE_Menu::~PGE_Menu()
{
    clear();
    GlRenderer::deleteTexture(_selector);
    GlRenderer::deleteTexture(_scroll_up);
    GlRenderer::deleteTexture(_scroll_down);
}

void PGE_Menu::addMenuItem(std::string item_key, std::string title,
                           std::function<void()> _extAction, bool enabled)
{
    PGE_Menuitem item;
    item.item_key = item_key;
    item.type = PGE_Menuitem::ITEM_Normal;
    item.title = (title.empty() ? item_key : title);
    item._font_id = _font_id;
    item.m_enabled = enabled;
    if(_text_len_limit_strict)
    {
        //Crop lenght
        item.title = FontManager::cropText(item.title, _text_len_limit);
        item._width = FontManager::textSize(item.title, _font_id, 0, true).w();
    }
    else
    {
        //Capture limited lenght, but don't crop
        std::string temp = FontManager::cropText(item.title, _text_len_limit);
        item._width = FontManager::textSize(temp, _font_id, 0, true).w();
    }
    item.extAction = _extAction;
    _items_normal.push_back(item);
    PGE_Menuitem *itemP = &_items_normal.back();
    _items.push_back(itemP);
    _items_index[item_key] = itemP;
    refreshRect();
}

void PGE_Menu::addBoolMenuItem(bool *flag,
                               std::string item_key, std::string title,
                               std::function<void()> _extAction, bool enabled)
{
    PGE_BoolMenuItem item;
    item.flag = flag;
    item.item_key = item_key;
    item.title = (title.empty() ? "unknown flag" : title);
    item._font_id = _font_id;
    item.m_enabled = enabled;
    if(_text_len_limit_strict)
    {
        //Crop lenght
        item.title = FontManager::cropText(item.title, _text_len_limit);
        item._width = FontManager::textSize(item.title, _font_id, 0, true).w();
    }
    else
    {
        //Capture limited lenght, but don't crop
        std::string temp = FontManager::cropText(item.title, _text_len_limit);
        item._width = FontManager::textSize(temp, _font_id, 0, true).w();
    }
    item.extAction = _extAction;
    _items_bool.push_back(item);
    PGE_Menuitem *itemP = &_items_bool.back();
    _items.push_back(itemP);
    _items_index[item_key] = itemP;
    refreshRect();
}

void PGE_Menu::addIntMenuItem(int *intvalue, int min, int max,
                              std::string item_key, std::string title,
                              bool rotate, std::function<void()> _extAction, bool enabled)
{
    PGE_IntMenuItem item;
    item.intvalue = intvalue;
    item.item_key = item_key;
    item.min = min;
    item.max = max;
    item.allowRotation = rotate;
    item.title = (title.empty() ? "unknown integer" : title);
    item._font_id = _font_id;
    item.m_enabled = enabled;
    if(_text_len_limit_strict)
    {
        //Crop lenght
        item.title = FontManager::cropText(item.title, _text_len_limit);
        item._width = FontManager::textSize(item.title, _font_id, 0, true).w();
    }
    else
    {
        //Capture limited lenght, but don't crop
        std::string temp = FontManager::cropText(item.title, _text_len_limit);
        item._width = FontManager::textSize(temp, _font_id, 0, true).w();
    }
    item.extAction = _extAction;
    _items_int.push_back(item);
    PGE_Menuitem *itemP = &_items_int.back();
    _items.push_back(itemP);
    _items_index[item_key] = itemP;
    refreshRect();
}

void PGE_Menu::addNamedIntMenuItem(int *intvalue, std::vector<NamedIntItem> _items,
                                   std::string item_key, std::string title,
                                   bool rotate, std::function<void()> _extAction, bool enabled)
{
    PGE_NamedIntMenuItem item;
    item.intvalue = intvalue;
    item.item_key = item_key;
    item.items = _items;
    if(intvalue)
        for(size_t i = 0; i < _items.size(); i++)
        {
            if(*intvalue == _items[i].value)
            {
                item.curItem = i;
                break;
            }
        }
    item.allowRotation = rotate;
    item.title = (title.empty() ? "unknown named integer" : title);
    item._font_id = _font_id;
    item.m_enabled = enabled;
    if(_text_len_limit_strict)
    {
        //Crop lenght
        item.title = FontManager::cropText(item.title, _text_len_limit);
        item._width = FontManager::textSize(item.title, _font_id, 0, true).w();
    }
    else
    {
        //Capture limited lenght, but don't crop
        std::string temp = FontManager::cropText(item.title, _text_len_limit);
        item._width = FontManager::textSize(temp, _font_id, 0, true).w();
    }
    item.extAction = _extAction;
    _items_named_int.push_back(item);
    PGE_Menuitem *itemP = &_items_named_int.back();
    this->_items.push_back(itemP);
    _items_index[item_key] = itemP;
    refreshRect();
}

void PGE_Menu::addKeyGrabMenuItem(KM_Key *key, std::string item_key, std::string title,
                                  SDL_Joystick *joystick_device, bool enabled)
{
    PGE_KeyGrabMenuItem item;
    item.targetKey = key;
    item.item_key = item_key;
    item.title = (title.empty() ? "unknown key-grabber" : title);
    item._font_id = _font_id;
    item.m_enabled = enabled;
    if(_text_len_limit_strict)
    {
        //Crop lenght
        item.title = FontManager::cropText(item.title, _text_len_limit);
        item._width = FontManager::textSize(item.title, _font_id, 0, true).w();
    }
    else
    {
        //Capture limited lenght, but don't crop
        std::string temp = FontManager::cropText(item.title, _text_len_limit);
        item._width = FontManager::textSize(temp, _font_id, 0, true).w();
    }

    if(joystick_device)
    {
        item.joystick_mode = true;
        item.joystick_device = joystick_device;
    }

    item.menu = this;
    _items_keygrabs.push_back(item);
    PGE_Menuitem *itemP = &_items_keygrabs.back();
    _items.push_back(itemP);
    _items_index[item_key] = itemP;
    refreshRect();
}

void PGE_Menu::setEnabled(std::string menuitem_key, bool enabled)
{
    MenuIndex::iterator it = _items_index.find(menuitem_key);
    if(it != _items_index.end())
        it->second->m_enabled = enabled;
}

void PGE_Menu::setValueOffset(int offset)
{
    if(_items.empty())
        return;
    if(offset <= 0)
        return;
    _items.back()->valueOffset = offset;
}

void PGE_Menu::setItemWidth(int width)
{
    if(_items.empty())
        return;
    if(width <= 0)
        return;
    _items.back()->_width = width;
}



void PGE_Menu::clear()
{
    _items.clear();
    _items_index.clear();
    _items_normal.clear();
    _items_bool.clear();
    _items_int.clear();
    _items_named_int.clear();
    _items_keygrabs.clear();
    m_item = NULL;
    reset();
    refreshRect();
}

void PGE_Menu::selectUp()
{
    PGE_Audio::playSoundByRole(obj_sound_role::MenuScroll);

    _currentItem--;
    _line--;

    if(_line < 0)
    {
        _offset--;
        _line = 0;
        if(_offset < 0) _offset = 0;
    }

    if(_currentItem < 0)
    {
        _currentItem = _items.size() - 1;
        _line = _itemsOnScreen - 1;
        _offset = (int(_items.size()) > _itemsOnScreen) ? _items.size() - _itemsOnScreen : 0;
    }
}

void PGE_Menu::selectDown()
{
    PGE_Audio::playSoundByRole(obj_sound_role::MenuScroll);

    _currentItem++;
    _line++;
    if(_line > _itemsOnScreen - 1)
    {
        _offset++;
        _line = _itemsOnScreen - 1;
    }

    if(_currentItem >= int(_items.size()))
    {
        _currentItem = 0;
        _line = 0;
        _offset = 0;
    }
}

void PGE_Menu::scrollUp()
{
    if(int(_items.size()) <= _itemsOnScreen)
        return;
    if(_offset > 0)
    {
        _offset--;
        _currentItem--;
    }
}

void PGE_Menu::scrollDown()
{
    if(int(_items.size()) <= _itemsOnScreen) return;

    if(_offset < (int(_items.size()) - _itemsOnScreen))
    {
        _offset++;
        _currentItem++;
    }
}

void PGE_Menu::selectLeft()
{
    if(_items.size() <= 0) return;
    PGE_Menuitem *selected = _items[_currentItem];
    if(!selected->m_enabled) return;
    selected->left();
}

void PGE_Menu::selectRight()
{
    if(_items.size() <= 0)
        return;
    PGE_Menuitem *selected = _items[_currentItem];
    if(!selected->m_enabled) return;
    selected->right();
}

void PGE_Menu::acceptItem()
{
    if(_items.size() <= 0)
        return;

    PGE_Menuitem *selected = _items[_currentItem];
    if(!selected->m_enabled)
    {
        //Do nothing if menu item is disabled!
        return;
    }

    if(selected->type == PGE_Menuitem::ITEM_Bool)
        selected->toggle();
    else if(selected->type == PGE_Menuitem::ITEM_Int)
    {}
    else if(selected->type == PGE_Menuitem::ITEM_NamedInt)
        selected->right();
    else if(selected->type == PGE_Menuitem::ITEM_KeyGrab)
    {
        PGE_Audio::playSoundByRole(obj_sound_role::PlayerClimb);
        static_cast<PGE_KeyGrabMenuItem *>(selected)->grabKey();
        setKeygrabViaKey(true);
    }
    else
    {
        PGE_Audio::playSoundByRole(obj_sound_role::MenuDo);
        _EndSelection = true;
        _accept = true;
        if((_currentItem < int(_items.size())) && (_currentItem >= 0))
            selected->extAction();
    }
}

void PGE_Menu::rejectItem()
{
    PGE_Audio::playSoundByRole(obj_sound_role::MenuDo);
    _EndSelection = true;
    _accept = false;
}

void PGE_Menu::setItemsNumber(int q)
{
    if(q > 0)
        _itemsOnScreen = q;
    else
        _itemsOnScreen = 5;
    refreshRect();
}

void PGE_Menu::sort()
{
    if(_items.size() <= 1)
    {
        autoOffset();    //Nothing to sort!
        return;
    }

    std::vector<int> beg;
    std::vector<int> end;
    beg.reserve(_items.size());
    end.reserve(_items.size());
    PGE_Menuitem *piv;
    int i = 0;
    size_t L, R, swapv;
    beg.push_back(0);
    end.push_back(_items.size());
    while(i >= 0)
    {
        L = beg[i];
        R = end[i] - 1;
        if(L < R)
        {
            piv = _items[L];
            while(L < R)
            {
                while((namefileMoreThan(_items[R], piv)) && (L < R)) R--;
                if(L < R) _items[L++] = _items[R];

                while((namefileLessThan(_items[L], piv)) && (L < R)) L++;
                if(L < R) _items[R--] = _items[L];
            }
            _items[L] = piv;
            beg.push_back(L + 1);
            end.push_back(end[i]);
            end[i++] = (L);
            if((end[i] - beg[i]) > (end[i - 1] - beg[i - 1]))
            {
                swapv = beg[i];
                beg[i] = beg[i - 1];
                beg[i - 1] = swapv;
                swapv = end[i];
                end[i] = end[i - 1];
                end[i - 1] = swapv;
            }
        }
        else
        {
            i--;
            beg.pop_back();
            end.pop_back();
        }
    }

    autoOffset();
}

bool PGE_Menu::namefileLessThan(const PGE_Menuitem *d1, const PGE_Menuitem *d2)
{
    return (d1->title.compare(d2->title) <= 0); // sort by title
}

bool PGE_Menu::namefileMoreThan(const PGE_Menuitem *d1, const PGE_Menuitem *d2)
{
    return (d1->title.compare(d2->title) >= 0); // sort by title
}

bool PGE_Menu::isSelected()
{
    return _EndSelection;
}

bool PGE_Menu::isAccepted()
{
    return _accept;
}

bool PGE_Menu::isKeyGrabbing()
{
    return is_keygrab;
}

bool PGE_Menu::processJoystickBinder()
{
    if((is_keygrab) && (m_item) && (m_item->joystick_mode))
    {
        m_item->processJoystickBind();
        return true;
    }
    return false;
}

void PGE_Menu::storeKey(int scancode)
{
    if(m_item)
        m_item->pushKey(scancode);
}

void PGE_Menu::reset()
{
    _EndSelection = false;
    _accept = false;
    _offset = 0;
    _line = 0;
    _currentItem = 0;
    is_keygrab = false;
}

void PGE_Menu::resetState()
{
    _EndSelection = false;
    _accept = false;
}

void PGE_Menu::setMouseHoverPos(int x, int y)
{
    int item = findItem(x, y);
    if(item < 0) return;
    if(_currentItem != item)
        PGE_Audio::playSoundByRole(obj_sound_role::MenuScroll);
    setCurrentItem(item);
}

void PGE_Menu::setMouseClickPos(int x, int y)
{
    int item = findItem(x, y);
    if(item < 0) return;
    acceptItem();
}

int PGE_Menu::findItem(int x, int y)
{
    if(x > menuRect.right()) return -1;
    if(x < menuRect.left())  return -1;
    if(y < menuRect.top())  return -1;
    if(y > menuRect.bottom()) return -1;

    int pos = menuRect.y();
    for(int i = 0; (i < _itemsOnScreen) && (i < int(_items.size())); i++)
    {
        if((y > pos) && (y < (pos + _item_height)))
            return _offset + i;
        pos += _item_height + menuItemGap;
    }

    return -1;
}

PGE_Menu::menuAlignment PGE_Menu::getAlignment()
{
    return alignment;
}

const PGE_Menuitem PGE_Menu::currentItem()
{
    if(_items.size() > 0)
        return *_items[_currentItem];
    else
    {
        PGE_Menuitem dummy;
        dummy.title = "";
        dummy.item_key = "";
        return dummy;
    }
}

int PGE_Menu::currentItemI()
{
    return _currentItem;
}

void PGE_Menu::setCurrentItem(int i)
{
    //If no out of range
    if((i >= 0) && (i < int(_items.size())))
    {
        _currentItem = i;
        autoOffset();
    }
}

int PGE_Menu::line()
{
    return _line;
}

void PGE_Menu::setLine(int ln)
{
    if((ln >= 0) && (ln < _itemsOnScreen))
        _line = ln;
    else
        _line = _itemsOnScreen / 2;
}

int PGE_Menu::offset()
{
    return _offset;
}

void PGE_Menu::setOffset(int of)
{
    if((of >= 0) && (of < (int(_items.size()) - _itemsOnScreen)))
    {
        _offset = of;
        _line = _currentItem - of;
        _line = ((_line > 0) ?
                 ((_line < _itemsOnScreen) ? _line : _itemsOnScreen)
                 : 0);
        autoOffset();
    }
    else
        autoOffset();
}

///
/// \brief PGE_Menu::autoOffset
/// Automatically sets offset and line number values
void PGE_Menu::autoOffset()
{
    if(int(_items.size()) <= _itemsOnScreen)
    {
        _offset = 0;
        _line = _currentItem;
        return;
    }

    if(_currentItem - _itemsOnScreen > _offset)
    {
        _offset = _currentItem - _itemsOnScreen + 1;
        _line = _itemsOnScreen - 1;
    }
    else if(_currentItem > (_offset + _itemsOnScreen - 1))
    {
        _offset = _currentItem - _itemsOnScreen + 1;
        _line = _itemsOnScreen - 1;
    }
    else
        _line = _currentItem - _offset;
}



void PGE_Menu::setPos(int x, int y)
{
    menuRect.setX(x);
    menuRect.setY(y - _font_offset);
    refreshRect();
}

void PGE_Menu::setPos(PGE_Point p)
{
    menuRect.setX(p.x());
    menuRect.setY(p.y() - _font_offset);
    refreshRect();
}

void PGE_Menu::setSize(int w, int h)
{
    (void)(w);
    _item_height = h;
    refreshRect();
}

void PGE_Menu::setSize(PGE_Size s)
{
    _item_height = s.h();
    refreshRect();
}

void PGE_Menu::setTextLenLimit(int maxlen, bool strict)
{
    if(maxlen <= 0)
        _text_len_limit = 0;
    else
        _text_len_limit = maxlen;
    _text_len_limit_strict = strict;
}

int PGE_Menu::getMenuItemGap()
{
    return menuItemGap;
}

void PGE_Menu::refreshRect()
{
    if(alignment == menuAlignment::HORIZONTAL)
    {
        menuRect.setHeight(_item_height);

        int menuWidth = 0;
        if(int(_items.size()) < _itemsOnScreen)
        {
            for(size_t temp = 0; temp < _items.size(); temp++)
                menuWidth += _items[temp]->_width + menuItemGap;
        }
        else
        {
            int maxWidth = 0;
            for(size_t temp = 0; temp < _items.size(); temp++)
            {
                if(_items[temp]->_width > maxWidth) maxWidth = _items[temp]->_width;
            }
            menuWidth = (maxWidth + menuItemGap) * _itemsOnScreen;
            menuWidth -= menuItemGap;
        }
        menuRect.setWidth(menuWidth);
    }
    else if(alignment == menuAlignment::VERTICLE)
    {
        if(int(_items.size()) < _itemsOnScreen)
            menuRect.setHeight(_items.size() * (_item_height + menuItemGap));
        else
            menuRect.setHeight(_itemsOnScreen * (_item_height + menuItemGap));

        menuRect.setWidth(0);
        for(size_t i = 0; i < _items.size(); i++)
        {
            if(menuRect.width() < _items[i]->_width)
                menuRect.setWidth(_items[i]->_width);
        }
        if(menuRect.width() > _width_limit)
            menuRect.setWidth(_width_limit);
    }
}
bool PGE_Menu::isKeygrabViaKey() const
{
    return is_keygrabViaKey;
}

void PGE_Menu::setKeygrabViaKey(bool value)
{
    is_keygrabViaKey = value;
}


PGE_Rect PGE_Menu::rect()
{
    return menuRect;
}

PGE_Rect PGE_Menu::rectFull()
{
    PGE_Rect tRect = menuRect;
    tRect.setWidth(menuRect.width() + (_selector.w != 0 ? _selector.w : 20) + 10);

    if(int(_items.size()) > _itemsOnScreen)
    {
        if(alignment == menuAlignment::VERTICLE)
        {
            tRect.setHeight(menuRect.height() +
                            (_scroll_up.w != 0 ? _scroll_up.h : 10) +
                            (_scroll_down.w != 0 ? _scroll_down.h : 10) + 20 - _font_offset);
        }
        else if(alignment == menuAlignment::HORIZONTAL)
        {
            tRect.setWidth(menuRect.width() +
                           (_scroll_up.w != 0 ? _scroll_up.h : 10) +
                           (_scroll_down.w != 0 ? _scroll_down.h : 10));
            tRect.setHeight(menuRect.height() + (_selector.h != 0 ? _selector.h : 20) + 10);
        }
    }
    else if(alignment == menuAlignment::HORIZONTAL)
        tRect.setWidth(menuRect.width());
    return tRect;
}

int PGE_Menu::topOffset()
{
    if(int(_items.size()) > _itemsOnScreen)
        return (_scroll_up.w != 0 ? _scroll_up.h : 10) + 10 + _font_offset;
    return _font_offset;
}

void PGE_Menu::render()
{
    //Show scrollers
    if(int(_items.size()) > _itemsOnScreen)
    {
        if(_offset > 0)
        {
            int w = 10;
            int h = 10;
            if(_scroll_up.w > 0)
            {
                w = _scroll_up.w;
                h = _scroll_up.h;
            }
            int posX = menuRect.x();
            int posY = menuRect.y();

            if(alignment == menuAlignment::HORIZONTAL)
            {
                posX -= (w + 30);
                posY += h / 2;

                //scroll left texture todo
                if(_scroll_up.w == 0)
                    GlRenderer::renderRect(posX, posY, w, h, 0.f, 1.f, 0.f, 1.0f);
                else
                {
                    //scroll left texture todo
                    GlRenderer::setTextureColor(1.0f, 1.0f, 1.0f, 1.0f);
                    GlRenderer::renderTexture(&_scroll_up, posX, posY);
                }
            }
            else if(alignment == menuAlignment::VERTICLE)
            {
                posX += (menuRect.width() / 2) - (h / 2);
                posY += -h - 4;

                if(_scroll_up.w == 0)
                    GlRenderer::renderRect(posX, posY, w, h, 0.f, 1.f, 0.f, 1.0f);
                else
                {
                    GlRenderer::setTextureColor(1.0f, 1.0f, 1.0f, 1.0f);
                    GlRenderer::renderTexture(&_scroll_up, posX, posY);
                }
            }
        }

        if(_offset < (int(_items.size()) - _itemsOnScreen))
        {
            int w = 10;
            int h = 10;
            if(_scroll_down.w > 0)
            {
                w = _scroll_down.w;
                h = _scroll_down.h;
            }
            int posX = menuRect.x();
            int posY = menuRect.y();

            if(alignment == menuAlignment::HORIZONTAL)
            {
                for(int temp = _offset; temp < _offset + _itemsOnScreen; temp++)
                    if(temp == _offset + _itemsOnScreen - 1)
                        posX += _items[temp]->_width + 30;
                    else
                        posX += _items[temp]->_width + menuItemGap;
                posY += h / 2;

                //scroll right texture todo
                if(_scroll_down.w == 0)
                    GlRenderer::renderRect(posX, posY, w, h, 0.f, 1.f, 0.f, 1.0f);
                else
                {
                    //scroll right texture todo
                    GlRenderer::setTextureColor(1.0f, 1.0f, 1.0f, 1.0f);
                    GlRenderer::renderTexture(&_scroll_down, posX, posY);
                }
            }
            else if(alignment == menuAlignment::VERTICLE)
            {
                posX += (menuRect.width() / 2) - (h / 2);
                posY += (_item_height + menuItemGap) * _itemsOnScreen + 4;

                if(int(_items.size()) > 1)
                    posY -= menuItemGap;

                if(_scroll_down.w == 0)
                    GlRenderer::renderRect(posX, posY, w, h, 0.f, 1.f, 0.f, 1.0f);
                else
                {
                    GlRenderer::setTextureColor(1.0f, 1.0f, 1.0f, 1.0f);
                    GlRenderer::renderTexture(&_scroll_down, posX, posY);
                }
            }
        }
    }

    for(int i = _offset, j = 0; i < _offset + _itemsOnScreen && i < int(_items.size()); i++, j++)
    {
        int xPos = menuRect.x();
        int yPos = menuRect.y();
        int xPos_s = 0;

        if(alignment == menuAlignment::HORIZONTAL)
        {
            for(int temp = i - 1; temp >= _offset; temp--)
                xPos += _items[temp]->_width + menuItemGap;
            xPos_s = (xPos + _items[i]->_width / 2) - _selector.w / 2;
        }
        else if(alignment == menuAlignment::VERTICLE)
        {
            yPos += j * (_item_height + menuItemGap);
            xPos_s = xPos - _selector.w - 10;
        }

        if(i == _currentItem)
        {
            if(_selector.w == 0)
                GlRenderer::renderRect(xPos_s - 10, yPos + (_item_height / 2) - 5, 20, 2, 1.f, 1.f, 0.f, 1.0f);
            else
            {
                int y_offset = 0;

                if(alignment == menuAlignment::HORIZONTAL)
                    y_offset = 30;
                else if(alignment == menuAlignment::VERTICLE)
                    y_offset = (_item_height / 2) - (_selector.h / 2);
                //todo: put renderTexture inside their respective ifstatement once texture is complete
                GlRenderer::setTextureColor(1.0f, 1.0f, 1.0f, 1.0f);
                GlRenderer::renderTexture(&_selector, xPos_s, yPos + y_offset);
            }
        }
        _items[i]->render(xPos, yPos + _font_offset);
    }
}
