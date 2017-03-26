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
    m_alignment = align;
    m_itemsOnScreen = 5;
    m_currentItem = 0;
    m_line = 0;
    m_offset = 0;
    m_arrowUpViz = false;
    m_arrowDownViz = false;
    m_endSelection = false;
    m_accept = false;
    m_is_keygrab = false;
    m_item = nullptr;

    m_menuRect.setRect(260, 380, 350, ConfigManager::setup_menus.item_height);
    m_item_height = ConfigManager::setup_menus.item_height;
    m_width_limit = PGE_Window::Width - 100;
    m_text_len_limit = 0;
    m_menuItemGap = itemGap;

    /// Init menu font
    ConfigManager::setup_menus.font_id = FontManager::getFontID(ConfigManager::setup_menus.font_name);
    m_font_id = ConfigManager::setup_menus.font_id;
    m_font_offset = ConfigManager::setup_menus.font_offset;

    if(ConfigManager::setup_menus.selector.empty())
        m_selector.w = 0;
    else
        GlRenderer::loadTextureP(m_selector, ConfigManager::setup_menus.selector);

    if(ConfigManager::setup_menus.scrollerUp.empty())
        m_scroll_up.w = 0;
    else
        GlRenderer::loadTextureP(m_scroll_up, ConfigManager::setup_menus.scrollerUp);

    if(ConfigManager::setup_menus.scrollerDown.empty())
        m_scroll_down.w = 0;
    else
        GlRenderer::loadTextureP(m_scroll_down, ConfigManager::setup_menus.scrollerDown);
}

PGE_Menu::PGE_Menu(const PGE_Menu &menu)
{
    m_menuRect =   menu.m_menuRect;

    /*******Key grabbing********/
    m_item     = menu.m_item;
    m_is_keygrab = menu.m_is_keygrab;
    /*******Key grabbing********/

    m_alignment      = menu.m_alignment;
    m_itemsOnScreen = menu.m_itemsOnScreen;
    m_currentItem   = menu.m_currentItem;
    m_line          = menu.m_line;
    m_offset        = menu.m_offset;
    m_arrowUpViz     = menu.m_arrowUpViz;
    m_arrowDownViz   = menu.m_arrowDownViz;
    m_endSelection  = menu.m_endSelection;
    m_accept        = menu.m_accept;
    m_items_bool    = menu.m_items_bool;
    m_items_int     = menu.m_items_int;
    m_items_named_int = menu.m_items_named_int;
    m_items_normal   = menu.m_items_normal;
    m_items_keygrabs = menu.m_items_keygrabs;

    m_items  = menu.m_items;
    m_items_index = menu.m_items_index;
    m_selector = menu.m_selector;
    m_scroll_up = menu.m_selector;
    m_scroll_down = menu.m_scroll_down;
    m_item_height = menu.m_item_height;
    m_width_limit = menu.m_width_limit;
    m_text_len_limit = menu.m_text_len_limit;
    m_text_len_limit_strict = menu.m_text_len_limit_strict;
    m_menuItemGap = menu.m_menuItemGap;

    m_font_id = menu.m_font_id;
    m_font_offset = menu.m_font_offset;
}

PGE_Menu::~PGE_Menu()
{
    clear();
    GlRenderer::deleteTexture(m_selector);
    GlRenderer::deleteTexture(m_scroll_up);
    GlRenderer::deleteTexture(m_scroll_down);
}

void PGE_Menu::addMenuItem(std::string item_key, std::string title,
                           std::function<void()> _extAction, bool enabled)
{
    PGE_Menuitem item;
    item.item_key = item_key;
    item.type = PGE_Menuitem::ITEM_Normal;
    item.title = (title.empty() ? item_key : title);
    item._font_id = m_font_id;
    item.m_enabled = enabled;
    if(m_text_len_limit_strict)
    {
        //Crop lenght
        item.title = FontManager::cropText(item.title, m_text_len_limit);
        item._width = FontManager::textSize(item.title, m_font_id, 0, true).w();
    }
    else
    {
        //Capture limited lenght, but don't crop
        std::string temp = FontManager::cropText(item.title, m_text_len_limit);
        item._width = FontManager::textSize(temp, m_font_id, 0, true).w();
    }
    item.extAction = _extAction;
    m_items_normal.push_back(item);
    PGE_Menuitem *itemP = &m_items_normal.back();
    m_items.push_back(itemP);
    m_items_index[item_key] = itemP;
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
    item._font_id = m_font_id;
    item.m_enabled = enabled;
    if(m_text_len_limit_strict)
    {
        //Crop lenght
        item.title = FontManager::cropText(item.title, m_text_len_limit);
        item._width = FontManager::textSize(item.title, m_font_id, 0, true).w();
    }
    else
    {
        //Capture limited lenght, but don't crop
        std::string temp = FontManager::cropText(item.title, m_text_len_limit);
        item._width = FontManager::textSize(temp, m_font_id, 0, true).w();
    }
    item.extAction = _extAction;
    m_items_bool.push_back(item);
    PGE_Menuitem *itemP = &m_items_bool.back();
    m_items.push_back(itemP);
    m_items_index[item_key] = itemP;
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
    item._font_id = m_font_id;
    item.m_enabled = enabled;
    if(m_text_len_limit_strict)
    {
        //Crop lenght
        item.title = FontManager::cropText(item.title, m_text_len_limit);
        item._width = FontManager::textSize(item.title, m_font_id, 0, true).w();
    }
    else
    {
        //Capture limited lenght, but don't crop
        std::string temp = FontManager::cropText(item.title, m_text_len_limit);
        item._width = FontManager::textSize(temp, m_font_id, 0, true).w();
    }
    item.extAction = _extAction;
    m_items_int.push_back(item);
    PGE_Menuitem *itemP = &m_items_int.back();
    m_items.push_back(itemP);
    m_items_index[item_key] = itemP;
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
    item._font_id = m_font_id;
    item.m_enabled = enabled;
    if(m_text_len_limit_strict)
    {
        //Crop lenght
        item.title = FontManager::cropText(item.title, m_text_len_limit);
        item._width = FontManager::textSize(item.title, m_font_id, 0, true).w();
    }
    else
    {
        //Capture limited lenght, but don't crop
        std::string temp = FontManager::cropText(item.title, m_text_len_limit);
        item._width = FontManager::textSize(temp, m_font_id, 0, true).w();
    }
    item.extAction = _extAction;
    m_items_named_int.push_back(item);
    PGE_Menuitem *itemP = &m_items_named_int.back();
    this->m_items.push_back(itemP);
    m_items_index[item_key] = itemP;
    refreshRect();
}

void PGE_Menu::addKeyGrabMenuItem(KM_Key *key, std::string item_key, std::string title,
                                  SDL_Joystick *joystick_device, bool enabled)
{
    PGE_KeyGrabMenuItem item;
    item.targetKey = key;
    item.item_key = item_key;
    item.title = (title.empty() ? "unknown key-grabber" : title);
    item._font_id = m_font_id;
    item.m_enabled = enabled;
    if(m_text_len_limit_strict)
    {
        //Crop lenght
        item.title = FontManager::cropText(item.title, m_text_len_limit);
        item._width = FontManager::textSize(item.title, m_font_id, 0, true).w();
    }
    else
    {
        //Capture limited lenght, but don't crop
        std::string temp = FontManager::cropText(item.title, m_text_len_limit);
        item._width = FontManager::textSize(temp, m_font_id, 0, true).w();
    }

    if(joystick_device)
    {
        item.joystick_mode = true;
        item.joystick_device = joystick_device;
    }

    item.menu = this;
    m_items_keygrabs.push_back(item);
    PGE_Menuitem *itemP = &m_items_keygrabs.back();
    m_items.push_back(itemP);
    m_items_index[item_key] = itemP;
    refreshRect();
}

void PGE_Menu::setEnabled(std::string menuitem_key, bool enabled)
{
    MenuIndex::iterator it = m_items_index.find(menuitem_key);
    if(it != m_items_index.end())
        it->second->m_enabled = enabled;
}

void PGE_Menu::setValueOffset(int offset)
{
    if(m_items.empty())
        return;
    if(offset <= 0)
        return;
    m_items.back()->valueOffset = offset;
}

void PGE_Menu::setItemWidth(int width)
{
    if(m_items.empty())
        return;
    if(width <= 0)
        return;
    m_items.back()->_width = width;
}



void PGE_Menu::clear()
{
    m_items.clear();
    m_items_index.clear();
    m_items_normal.clear();
    m_items_bool.clear();
    m_items_int.clear();
    m_items_named_int.clear();
    m_items_keygrabs.clear();
    m_item = NULL;
    reset();
    refreshRect();
}

void PGE_Menu::selectUp()
{
    PGE_Audio::playSoundByRole(obj_sound_role::MenuScroll);

    if(m_line == 0)
    {
        if(m_offset > 0)
            m_offset--;
    }
    else
        m_line--;

    if(m_currentItem == 0)
    {
        m_currentItem = m_items.size() - 1;
        m_line = m_itemsOnScreen - 1;
        m_offset = (m_items.size() > m_itemsOnScreen) ? m_items.size() - m_itemsOnScreen : 0;
    }
    else
        m_currentItem--;
}

void PGE_Menu::selectDown()
{
    PGE_Audio::playSoundByRole(obj_sound_role::MenuScroll);

    m_currentItem++;
    if(m_line >= m_itemsOnScreen - 1)
    {
        m_offset++;
        m_line = m_itemsOnScreen - 1;
    }
    else
        m_line++;

    if(m_currentItem >= m_items.size())
    {
        m_currentItem = 0;
        m_line = 0;
        m_offset = 0;
    }
}

void PGE_Menu::scrollUp()
{
    if(m_items.size() <= m_itemsOnScreen)
        return;
    if(m_offset > 0)
    {
        m_offset--;
        m_currentItem--;
    }
}

void PGE_Menu::scrollDown()
{
    if(m_items.size() <= m_itemsOnScreen)
        return;
    if(m_offset < (m_items.size() - m_itemsOnScreen))
    {
        m_offset++;
        m_currentItem++;
    }
}

void PGE_Menu::selectLeft()
{
    if(m_items.empty())
        return;
    PGE_Menuitem *selected = m_items[m_currentItem];
    if(!selected->m_enabled)
        return;
    selected->left();
}

void PGE_Menu::selectRight()
{
    if(m_items.empty())
        return;
    PGE_Menuitem *selected = m_items[m_currentItem];
    if(!selected->m_enabled)
        return;
    selected->right();
}

void PGE_Menu::acceptItem()
{
    if(m_items.empty())
        return;

    PGE_Menuitem *selected = m_items[m_currentItem];
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
        m_endSelection = true;
        m_accept = true;
        if(size_t(m_currentItem) < m_items.size())
            selected->extAction();
    }
}

void PGE_Menu::rejectItem()
{
    PGE_Audio::playSoundByRole(obj_sound_role::MenuDo);
    m_endSelection = true;
    m_accept = false;
}

void PGE_Menu::setItemsNumber(size_t q)
{
    if(q > 0)
        m_itemsOnScreen = q;
    else
        m_itemsOnScreen = 5;
    refreshRect();
}

void PGE_Menu::sort()
{
    if(m_items.size() <= 1)
    {
        autoOffset();    //Nothing to sort!
        return;
    }

    std::vector<ssize_t> beg;
    std::vector<ssize_t> end;
    beg.reserve(m_items.size());
    end.reserve(m_items.size());
    PGE_Menuitem *piv;
    ssize_t i = 0;
    ssize_t L, R, swapv;
    beg.push_back(0);
    end.push_back(ssize_t(m_items.size()));
    bool run = true;
    while(run)
    {
        L = beg[size_t(i)];
        R = end[size_t(i)] - 1;
        if(L < R)
        {
            piv = m_items[size_t(L)];
            while(L < R)
            {
                while((namefileMoreThan(m_items[size_t(R)], piv)) && (L < R)) R--;
                if(L < R) m_items[size_t(L++)] = m_items[size_t(R)];

                while((namefileLessThan(m_items[size_t(L)], piv)) && (L < R)) L++;
                if(L < R) m_items[size_t(R--)] = m_items[size_t(L)];
            }
            m_items[size_t(L)] = piv;
            beg.push_back(L + 1);
            end.push_back(end[size_t(i)]);
            end[size_t(i++)] = (L);
            if((end[size_t(i)] - beg[size_t(i)]) > (end[size_t(i - 1)] - beg[size_t(i - 1)]))
            {
                swapv = beg[size_t(i)];
                beg[size_t(i)] = beg[size_t(i - 1)];
                beg[size_t(i - 1)] = swapv;
                swapv = end[size_t(i)];
                end[size_t(i)] = end[size_t(i - 1)];
                end[size_t(i - 1)] = swapv;
            }
        }
        else
        {
            if(i == 0)
                run = false;
            else
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
    return m_endSelection;
}

bool PGE_Menu::isAccepted()
{
    return m_accept;
}

bool PGE_Menu::isKeyGrabbing()
{
    return m_is_keygrab;
}

bool PGE_Menu::processJoystickBinder()
{
    if((m_is_keygrab) && (m_item) && (m_item->joystick_mode))
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
    m_endSelection = false;
    m_accept = false;
    m_offset = 0;
    m_line = 0;
    m_currentItem = 0;
    m_is_keygrab = false;
}

void PGE_Menu::resetState()
{
    m_endSelection = false;
    m_accept = false;
}

void PGE_Menu::setMouseHoverPos(int x, int y)
{
    size_t item = findItem(x, y);
    if(item == size_t(-1))
        return;
    if(m_currentItem != item)
        PGE_Audio::playSoundByRole(obj_sound_role::MenuScroll);
    setCurrentItem(item);
}

void PGE_Menu::setMouseClickPos(int x, int y)
{
    size_t item = findItem(x, y);
    if(item == npos)
        return;
    acceptItem();
}

size_t PGE_Menu::findItem(int x, int y)
{
    if(x > m_menuRect.right())
        return npos;
    if(x < m_menuRect.left())
        return npos;
    if(y < m_menuRect.top())
        return npos;
    if(y > m_menuRect.bottom())
        return npos;

    int pos = m_menuRect.y();
    for(size_t i = 0; (i < m_itemsOnScreen) && (i < m_items.size()); i++)
    {
        if((y > pos) && (y < (pos + m_item_height)))
            return m_offset + i;
        pos += m_item_height + int(m_menuItemGap);
    }
    return npos;
}

PGE_Menu::menuAlignment PGE_Menu::getAlignment()
{
    return m_alignment;
}

const PGE_Menuitem PGE_Menu::currentItem()
{
    if(m_items.size() > 0)
        return *m_items[m_currentItem];
    else
    {
        PGE_Menuitem dummy;
        dummy.title = "";
        dummy.item_key = "";
        return dummy;
    }
}

size_t PGE_Menu::currentItemI()
{
    return m_currentItem;
}

void PGE_Menu::setCurrentItem(size_t i)
{
    //If no out of range
    if(i < m_items.size())
    {
        m_currentItem = i;
        autoOffset();
    }
}

size_t PGE_Menu::line()
{
    return m_line;
}

void PGE_Menu::setLine(size_t ln)
{
    if(ln < m_itemsOnScreen)
        m_line = ln;
    else
        m_line = m_itemsOnScreen / 2;
}

size_t PGE_Menu::offset()
{
    return m_offset;
}

void PGE_Menu::setOffset(size_t off)
{
    if(off < (m_items.size() - m_itemsOnScreen))
    {
        m_offset = off;
        m_line = m_currentItem - off;
        m_line = ((m_line > 0) ?
                 ((m_line < m_itemsOnScreen) ? m_line : m_itemsOnScreen)
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
    if(m_items.size() <= m_itemsOnScreen)
    {
        m_offset = 0;
        m_line = m_currentItem;
        return;
    }

    if(m_currentItem - m_itemsOnScreen > m_offset)
    {
        m_offset = m_currentItem - m_itemsOnScreen + 1;
        m_line = m_itemsOnScreen - 1;
    }
    else if(m_currentItem > (m_offset + m_itemsOnScreen - 1))
    {
        m_offset = m_currentItem - m_itemsOnScreen + 1;
        m_line = m_itemsOnScreen - 1;
    }
    else
        m_line = m_currentItem - m_offset;
}



void PGE_Menu::setPos(int x, int y)
{
    m_menuRect.setX(x);
    m_menuRect.setY(y - m_font_offset);
    refreshRect();
}

void PGE_Menu::setPos(PGE_Point p)
{
    m_menuRect.setX(p.x());
    m_menuRect.setY(p.y() - m_font_offset);
    refreshRect();
}

void PGE_Menu::setSize(int w, int h)
{
    (void)(w);
    m_item_height = h;
    refreshRect();
}

void PGE_Menu::setSize(PGE_Size s)
{
    m_item_height = s.h();
    refreshRect();
}

void PGE_Menu::setTextLenLimit(uint32_t maxlen, bool strict)
{
    if(maxlen == 0)
        m_text_len_limit = 0;
    else
        m_text_len_limit = maxlen;
    m_text_len_limit_strict = strict;
}

int PGE_Menu::getMenuItemGap()
{
    return m_menuItemGap;
}

void PGE_Menu::refreshRect()
{
    if(m_alignment == menuAlignment::HORIZONTAL)
    {
        m_menuRect.setHeight(m_item_height);
        int menuWidth = 0;
        if(m_items.size() < m_itemsOnScreen)
        {
            for(size_t temp = 0; temp < m_items.size(); temp++)
                menuWidth += int(m_items[temp]->_width) + m_menuItemGap;
        }
        else
        {
            int maxWidth = 0;
            for(size_t temp = 0; temp < m_items.size(); temp++)
            {
                if(m_items[temp]->_width > maxWidth)
                    maxWidth = m_items[temp]->_width;
            }
            menuWidth = (int(maxWidth) + m_menuItemGap) * int(m_itemsOnScreen);
            menuWidth -= m_menuItemGap;
        }
        m_menuRect.setWidth(int(menuWidth));
    }
    else if(m_alignment == menuAlignment::VERTICLE)
    {
        if(m_items.size() < m_itemsOnScreen)
            m_menuRect.setHeight(int(m_items.size()) * (m_item_height + m_menuItemGap));
        else
            m_menuRect.setHeight(int(m_itemsOnScreen) * (m_item_height + m_menuItemGap));
        m_menuRect.setWidth(0);
        for(size_t i = 0; i < m_items.size(); i++)
        {
            if(m_menuRect.width() < m_items[i]->_width)
                m_menuRect.setWidth(m_items[i]->_width);
        }
        if(m_menuRect.width() > m_width_limit)
            m_menuRect.setWidth(m_width_limit);
    }
}

bool PGE_Menu::isKeygrabViaKey() const
{
    return m_is_keygrabViaKey;
}

void PGE_Menu::setKeygrabViaKey(bool value)
{
    m_is_keygrabViaKey = value;
}


PGE_Rect PGE_Menu::rect()
{
    return m_menuRect;
}

PGE_Rect PGE_Menu::rectFull()
{
    PGE_Rect tRect = m_menuRect;
    tRect.setWidth(m_menuRect.width() + (m_selector.w != 0 ? m_selector.w : 20) + 10);

    if(m_items.size() > m_itemsOnScreen)
    {
        if(m_alignment == menuAlignment::VERTICLE)
        {
            tRect.setHeight(m_menuRect.height() +
                            (m_scroll_up.w != 0 ? m_scroll_up.h : 10) +
                            (m_scroll_down.w != 0 ? m_scroll_down.h : 10) + 20 - m_font_offset);
        }
        else if(m_alignment == menuAlignment::HORIZONTAL)
        {
            tRect.setWidth(m_menuRect.width() +
                           (m_scroll_up.w != 0 ? m_scroll_up.h : 10) +
                           (m_scroll_down.w != 0 ? m_scroll_down.h : 10));
            tRect.setHeight(m_menuRect.height() + (m_selector.h != 0 ? m_selector.h : 20) + 10);
        }
    }
    else if(m_alignment == menuAlignment::HORIZONTAL)
        tRect.setWidth(m_menuRect.width());
    return tRect;
}

int PGE_Menu::topOffset()
{
    if(m_items.size() > m_itemsOnScreen)
        return (m_scroll_up.w != 0 ? m_scroll_up.h : 10) + 10 + m_font_offset;
    return m_font_offset;
}

void PGE_Menu::render()
{
    //Show scrollers
    if(m_items.size() > m_itemsOnScreen)
    {
        if(m_offset > 0)
        {
            int w = 10;
            int h = 10;
            if(m_scroll_up.w > 0)
            {
                w = m_scroll_up.w;
                h = m_scroll_up.h;
            }
            int posX = m_menuRect.x();
            int posY = m_menuRect.y();

            if(m_alignment == menuAlignment::HORIZONTAL)
            {
                posX -= (w + 30);
                posY += h / 2;

                //scroll left texture todo
                if(m_scroll_up.w == 0)
                    GlRenderer::renderRect(posX, posY, w, h, 0.f, 1.f, 0.f, 1.0f);
                else
                {
                    //scroll left texture todo
                    GlRenderer::setTextureColor(1.0f, 1.0f, 1.0f, 1.0f);
                    GlRenderer::renderTexture(&m_scroll_up, posX, posY);
                }
            }
            else if(m_alignment == menuAlignment::VERTICLE)
            {
                posX += (m_menuRect.width() / 2) - (h / 2);
                posY += -h - 4;

                if(m_scroll_up.w == 0)
                    GlRenderer::renderRect(posX, posY, w, h, 0.f, 1.f, 0.f, 1.0f);
                else
                {
                    GlRenderer::setTextureColor(1.0f, 1.0f, 1.0f, 1.0f);
                    GlRenderer::renderTexture(&m_scroll_up, posX, posY);
                }
            }
        }

        if(m_offset < (m_items.size() - m_itemsOnScreen))
        {
            int w = 10;
            int h = 10;
            if(m_scroll_down.w > 0)
            {
                w = m_scroll_down.w;
                h = m_scroll_down.h;
            }
            int posX = m_menuRect.x();
            int posY = m_menuRect.y();

            if(m_alignment == menuAlignment::HORIZONTAL)
            {
                for(size_t temp = m_offset; temp < m_offset + m_itemsOnScreen; temp++)
                    if(temp == m_offset + m_itemsOnScreen - 1)
                        posX += m_items[temp]->_width + 30;
                    else
                        posX += m_items[temp]->_width + m_menuItemGap;
                posY += h / 2;

                //scroll right texture todo
                if(m_scroll_down.w == 0)
                    GlRenderer::renderRect(posX, posY, w, h, 0.f, 1.f, 0.f, 1.0f);
                else
                {
                    //scroll right texture todo
                    GlRenderer::setTextureColor(1.0f, 1.0f, 1.0f, 1.0f);
                    GlRenderer::renderTexture(&m_scroll_down, posX, posY);
                }
            }
            else if(m_alignment == menuAlignment::VERTICLE)
            {
                posX += (m_menuRect.width() / 2) - (h / 2);
                posY += (m_item_height + m_menuItemGap) * int(m_itemsOnScreen) + 4;
                if(m_items.size() > 1)
                    posY -= m_menuItemGap;
                if(m_scroll_down.w == 0)
                    GlRenderer::renderRect(posX, posY, w, h, 0.f, 1.f, 0.f, 1.0f);
                else
                {
                    GlRenderer::setTextureColor(1.0f, 1.0f, 1.0f, 1.0f);
                    GlRenderer::renderTexture(&m_scroll_down, posX, posY);
                }
            }
        }
    }

    for(size_t i = m_offset, j = 0; i < m_offset + m_itemsOnScreen && i < m_items.size(); i++, j++)
    {
        int xPos = m_menuRect.x();
        int yPos = m_menuRect.y();
        int xPos_s = 0;

        if(m_alignment == menuAlignment::HORIZONTAL)
        {
            for(size_t temp = i - 1; temp >= m_offset; temp--)
                xPos += m_items[temp]->_width + m_menuItemGap;
            xPos_s = (xPos + m_items[i]->_width / 2) - m_selector.w / 2;
        }
        else if(m_alignment == menuAlignment::VERTICLE)
        {
            yPos += int(j) * (m_item_height + m_menuItemGap);
            xPos_s = xPos - m_selector.w - 10;
        }

        if(i == m_currentItem)
        {
            if(m_selector.w == 0)
                GlRenderer::renderRect(xPos_s - 10, yPos + (m_item_height / 2) - 5, 20, 2, 1.f, 1.f, 0.f, 1.0f);
            else
            {
                int y_offset = 0;

                if(m_alignment == menuAlignment::HORIZONTAL)
                    y_offset = 30;
                else if(m_alignment == menuAlignment::VERTICLE)
                    y_offset = (m_item_height / 2) - (m_selector.h / 2);
                //todo: put renderTexture inside their respective ifstatement once texture is complete
                GlRenderer::setTextureColor(1.0f, 1.0f, 1.0f, 1.0f);
                GlRenderer::renderTexture(&m_selector, xPos_s, yPos + y_offset);
            }
        }
        m_items[i]->render(xPos, yPos + m_font_offset);
    }
}
