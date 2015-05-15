/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2015 Vitaly Novichkov <admin@wohlnet.ru>
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
    is_keygrab=false;
    m_item=NULL;

    menuRect = QRect(260,380, 350, 30);

    if(ConfigManager::setup_menus.selector.isEmpty())
        _selector.w=0;
    else
        _selector = GraphicsHelps::loadTexture(_selector, ConfigManager::setup_menus.selector);

    if(ConfigManager::setup_menus.scrollerUp.isEmpty())
        _scroll_up.w=0;
    else
        _scroll_up = GraphicsHelps::loadTexture(_scroll_up, ConfigManager::setup_menus.scrollerUp);

    if(ConfigManager::setup_menus.scrollerDown.isEmpty())
        _scroll_down.w=0;
    else
        _scroll_down = GraphicsHelps::loadTexture(_scroll_down, ConfigManager::setup_menus.scrollerDown);
}

PGE_Menu::~PGE_Menu()
{
    clear();
    if(_selector.w>0)
    {
        glDisable(GL_TEXTURE_2D);
        glDeleteTextures(1, &_selector.texture );
    }
    if(_scroll_up.w>0)
    {
        glDisable(GL_TEXTURE_2D);
        glDeleteTextures(1, &_scroll_up.texture );
    }
    if(_scroll_down.w>0)
    {
        glDisable(GL_TEXTURE_2D);
        glDeleteTextures(1, &_scroll_down.texture );
    }
}

void PGE_Menu::addMenuItem(QString value, QString title, std::function<void()> _extAction)
{
    PGE_Menuitem item;
    item.value = value;
    item.type=PGE_Menuitem::ITEM_Normal;
    item.title = (title.isEmpty() ? value : title);
//    item.textTexture = FontManager::TextToTexture(item.title,
//                                                  QRect(0,0, abs(PGE_Window::Width-menuRect.x()), menuRect.height()),
//                                                  Qt::AlignLeft | Qt::AlignVCenter, true );
    item.extAction=_extAction;
    _items_normal.push_back(item);
    _items.push_back( &_items_normal.last() );
}

void PGE_Menu::addBoolMenuItem(bool *flag, QString value, QString title, std::function<void()> _extAction)
{
    PGE_BoolMenuItem item;
    item.flag = flag;
    item.value = value;
    item.title = (title.isEmpty() ? "unknown flag" : title);
//    item.textTexture = FontManager::TextToTexture(item.title,
//                                                  QRect(0,0, abs(PGE_Window::Width-menuRect.x()), menuRect.height()),
//                                                  Qt::AlignLeft | Qt::AlignVCenter, true );
    item.extAction=_extAction;
    _items_bool.push_back(item);
    _items.push_back( &_items_bool.last() );
}

void PGE_Menu::addIntMenuItem(int *intvalue, int min, int max, QString value, QString title, bool rotate, std::function<void()> _extAction)
{
    PGE_IntMenuItem item;
    item.intvalue = intvalue;
    item.value = value;
    item.min=min;
    item.max=max;
    item.allowRotation=rotate;
    item.title = (title.isEmpty() ? "unknown integer" : title);
//    item.textTexture = FontManager::TextToTexture(item.title,
//                                                  QRect(0,0, abs(PGE_Window::Width-menuRect.x()), menuRect.height()),
//                                                  Qt::AlignLeft | Qt::AlignVCenter, true );
    item.extAction=_extAction;
    _items_int.push_back(item);
    _items.push_back( &_items_int.last() );
}

void PGE_Menu::addNamedIntMenuItem(int *intvalue, QList<IntAssocItem> _items, QString value, QString title, bool rotate, std::function<void()> _extAction)
{
    PGE_NamedIntMenuItem item;
    item.intvalue = intvalue;
    item.value = value;
    item.items=_items;
    if(intvalue)
        for(int i=0; i<_items.size(); i++)
        {
            if(*intvalue==_items[i].value)
            { item.curItem=i; break; }
        }
    item.allowRotation=rotate;
    item.title = (title.isEmpty() ? "unknown named integer" : title);
//    item.textTexture = FontManager::TextToTexture(item.title,
//                                                  QRect(0,0, abs(PGE_Window::Width-menuRect.x()), menuRect.height()),
//                                                  Qt::AlignLeft | Qt::AlignVCenter, true );
    item.extAction=_extAction;
    _items_named_int.push_back(item);
    PGE_Menu::_items.push_back( &_items_named_int.last() );
}

void PGE_Menu::addKeyGrabMenuItem(int *keyvalue, QString value, QString title)
{
    PGE_KeyGrabMenuItem item;
    item.keyValue = keyvalue;
    item.value = value;
    item.title = (title.isEmpty() ? "unknown key-grabber" : title);
//    item.textTexture = FontManager::TextToTexture(item.title,
//                                                  QRect(0,0, abs(PGE_Window::Width-menuRect.x()), menuRect.height()),
//                                                  Qt::AlignLeft | Qt::AlignVCenter, true );
    item.menu = this;
    _items_keygrabs.push_back(item);
    _items.push_back( &_items_keygrabs.last() );
}


void PGE_Menu::clear()
{
    for(int i=0;i<_items.size();i++)
    {
        glDisable(GL_TEXTURE_2D);
        glDeleteTextures(1, &_items[i]->textTexture );
    }
    _items.clear();
    _items_normal.clear();
    _items_bool.clear();
    _items_int.clear();
    _items_named_int.clear();
    _items_keygrabs.clear();
    m_item=NULL;
    reset();
}

void PGE_Menu::selectUp()
{
    PGE_Audio::playSoundByRole(obj_sound_role::MenuScroll);

    _currentItem--;
    _line--;

    if(_line<0)
    {
        _offset--;
        _line=0;
        if(_offset<0) _offset=0;
    }

    if(_currentItem<0)
    {
        _currentItem=_items.size()-1;
        _line = _itemsOnScreen-1;
        _offset = (_items.size()>_itemsOnScreen)? _items.size()-_itemsOnScreen : 0;
    }
}

void PGE_Menu::selectDown()
{
    PGE_Audio::playSoundByRole(obj_sound_role::MenuScroll);

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

void PGE_Menu::scrollUp()
{
    if(_items.size()<=_itemsOnScreen) return;
    if(_offset>0)
    {
        _offset--;
        _currentItem--;
    }
}

void PGE_Menu::scrollDown()
{
    if(_items.size()<=_itemsOnScreen) return;

    if(_offset < (_items.size()-_itemsOnScreen))
    {
        _offset++;
        _currentItem++;
    }
}

void PGE_Menu::selectLeft()
{
    if(_items.size()>0)
        _items[_currentItem]->left();
}

void PGE_Menu::selectRight()
{
    if(_items.size()>0)
        _items[_currentItem]->right();
}

void PGE_Menu::acceptItem()
{
    if(_items.size()<=0) return;

    if(_items[_currentItem]->type==PGE_Menuitem::ITEM_Bool)
    {
        _items[_currentItem]->toggle();
    }
    else if(_items[_currentItem]->type==PGE_Menuitem::ITEM_Int)
    {}
    else if(_items[_currentItem]->type==PGE_Menuitem::ITEM_NamedInt)
    {
        _items[_currentItem]->right();
    }
    else if(_items[_currentItem]->type==PGE_Menuitem::ITEM_KeyGrab)
    {
        PGE_Audio::playSoundByRole(obj_sound_role::PlayerClimb);
        static_cast<PGE_KeyGrabMenuItem*>(_items[_currentItem])->grabKey();
    }
    else
    {
        PGE_Audio::playSoundByRole(obj_sound_role::MenuDo);
        _EndSelection=true;
        _accept=true;
        if((_currentItem<_items.size())&&(_currentItem>=0))
            _items[_currentItem]->extAction();
    }
}

void PGE_Menu::rejectItem()
{
    PGE_Audio::playSoundByRole(obj_sound_role::MenuDo);
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
    while(1)
    {
        bool swaped=false;
        for(int i=0; i<_items.size()-1; i++)
        {
            if(namefileLessThan(_items[i], _items[i+1]))
            {
                _items.swap(i, i+1);

                if(_currentItem==i)
                    _currentItem=i+1;
                else
                if(_currentItem==i+1)
                    _currentItem=i;

                swaped=true;
            }
        }
        if(!swaped) break;
    }
    autoOffset();
}

bool PGE_Menu::namefileLessThan(const PGE_Menuitem *d1, const PGE_Menuitem *d2)
{
    return (QString::compare(d1->title, d2->title, Qt::CaseInsensitive)>0); // sort by title
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

void PGE_Menu::storeKey(int scancode)
{
    if(m_item)
        m_item->pushKey(scancode);
}

void PGE_Menu::reset()
{
    _EndSelection=false;
    _accept=false;
    _offset=0;
    _line=0;
    _currentItem=0;
    is_keygrab=false;
}

void PGE_Menu::resetState()
{
    _EndSelection=false;
    _accept=false;
}

void PGE_Menu::setMouseHoverPos(int x, int y)
{
    int item = findItem(x,y);
    if(item<0) return;
    if(_currentItem!=item)
        PGE_Audio::playSoundByRole(obj_sound_role::MenuScroll);
    setCurrentItem(item);
}

void PGE_Menu::setMouseClickPos(int x, int y)
{
    int item = findItem(x,y);
    if(item<0) return;
    acceptItem();
}

int PGE_Menu::findItem(int x, int y)
{
    if(x>menuRect.right()) return -1;
    if(x<menuRect.left())  return -1;
    if(y<menuRect.top())  return -1;
    if(y>menuRect.top()+menuRect.height()*_itemsOnScreen) return -1;

    int pos = menuRect.y();
    for(int i=0; (i<_itemsOnScreen) && (i<_items.size()); i++)
    {
        if( (y > pos) && ( y<(pos+menuRect.height()) ) )
        {
            return _offset+i;
        }
        pos+=menuRect.height();
    }

    return -1;
}


const PGE_Menuitem PGE_Menu::currentItem()
{
    if(_items.size()>0)
        return *_items[_currentItem];
    else
    {
        PGE_Menuitem dummy;
        dummy.title="";
        dummy.value="";
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
    if((i>=0)&&(i<_items.size()))
    {
        _currentItem=i;
        autoOffset();
    }
}

int PGE_Menu::line()
{
    return _line;
}

void PGE_Menu::setLine(int ln)
{
    if((ln>=0) && (ln<_itemsOnScreen))
        _line = ln;
    else
        _line = _itemsOnScreen/2;
}

int PGE_Menu::offset()
{
    return _offset;
}

void PGE_Menu::setOffset(int of)
{
    if((of>=0)&&(of< (_items.size()-_itemsOnScreen)))
    {
       _offset=of;
       _line = _currentItem-of;
       _line = ((_line>0)?
                   ((_line<_itemsOnScreen)?_line : _itemsOnScreen)
                   :0 );
       autoOffset();
    }
    else
    {
        autoOffset();
    }
}

///
/// \brief PGE_Menu::autoOffset
/// Automatically sets offset and line number values
void PGE_Menu::autoOffset()
{
    if(_items.size()<=_itemsOnScreen)
    {
        _offset=0;
        _line=_currentItem;
        return;
    }

    if(_currentItem-_itemsOnScreen > _offset)
    {
        _offset=_currentItem-_itemsOnScreen+1;
        _line=_itemsOnScreen-1;
    }
    else
    if(_currentItem > (_offset+_itemsOnScreen-1))
    {
        _offset=_currentItem-_itemsOnScreen+1;
        _line=_itemsOnScreen-1;
    }
    else
    {
        _line=_currentItem-_offset;
    }
}



void PGE_Menu::setPos(int x, int y)
{
    menuRect.setX(x);
    menuRect.setY(y);
}

void PGE_Menu::setPos(QPoint p)
{
    menuRect.setTopLeft(p);
}

void PGE_Menu::setSize(int w, int h)
{
    menuRect.setWidth(w);
    menuRect.setHeight(h);
}

void PGE_Menu::setSize(QSize s)
{
    menuRect.setSize(s);
}

QRect PGE_Menu::rect()
{
    return menuRect;
}

void PGE_Menu::render()
{
    //Show scrollers
    if(_items.size()>_itemsOnScreen)
    {
        if(_offset > 0)
        {
            int w=10;
            int h=10;
            if(_scroll_up.w>0)
            {
                w=_scroll_up.w;
                h=_scroll_up.h;
            }
            int posX = menuRect.x()+(menuRect.width()/2)-(h/2);
            int posY = menuRect.y()-h-4;
            if(_scroll_up.w==0)
            {
                GlRenderer::renderRect(posX, posY, w, h, 0.f, 1.f, 0.f, 1.0f);
            }
            else
            {
                GlRenderer::renderTexture(&_scroll_up, posX, posY);
            }
        }

        if(_offset< (_items.size()-_itemsOnScreen))
        {
            int w=10;
            int h=10;
            if(_scroll_down.w>0)
            {
                w=_scroll_down.w;
                h=_scroll_down.h;
            }
            int posX = menuRect.x()+(menuRect.width()/2)-(h/2);
            int posY = menuRect.y()+menuRect.height()*_itemsOnScreen+4;
            if(_scroll_down.w==0)
            {
                GlRenderer::renderRect(posX, posY, w, h, 0.f, 1.f, 0.f, 1.0f);
            }
            else
            {
                GlRenderer::renderTexture(&_scroll_down, posX, posY);
            }
        }
    }

    for(int i=_offset, j=0; i<_offset+_itemsOnScreen && i<_items.size(); i++, j++ )
    {
        int xPos = menuRect.x();
        int xPos_s = menuRect.x()-_selector.w-10;
        int yPos = menuRect.y()+ j*menuRect.height();

        if(i==_currentItem)
        {
            if(_selector.w==0)
            {
                GlRenderer::renderRect(xPos_s-10, yPos + (menuRect.height()/2)-5, 20, 20, 1.f, 1.f, 0.f, 1.0f);
            }
            else
            {
                int y_offset=0;//(menuRect.height()/2)-(_selector.h/2);
                GlRenderer::renderTexture(&_selector, xPos_s, yPos+y_offset);
            }
        }
        _items[i]->render(xPos, yPos);
    }
}

PGE_Menuitem::PGE_Menuitem()
{
    this->title = "";
    this->value = "";
    this->textTexture = 0;
    this->extAction = []()->void{};
    this->valueOffset=350;
}

PGE_Menuitem::~PGE_Menuitem()
{}

PGE_Menuitem::PGE_Menuitem(const PGE_Menuitem &_it)
{
    this->title = _it.title;
    this->value = _it.value;
    this->textTexture = _it.textTexture;
    this->type = _it.type;
    this->extAction = _it.extAction;
    this->valueOffset = _it.valueOffset;
}

void PGE_Menuitem::left() {}

void PGE_Menuitem::right() {}

void PGE_Menuitem::toggle() {}

void PGE_Menuitem::render(int x, int y)
{
    FontManager::printText(title, x, y);
    //FontManager::SDL_string_render2D(x,y, &textTexture);
}


/**************************Bool menu item************************************/
PGE_BoolMenuItem::PGE_BoolMenuItem() : PGE_Menuitem()
{
    flag=NULL;
    type=ITEM_Bool;
}

PGE_BoolMenuItem::PGE_BoolMenuItem(const PGE_BoolMenuItem &it) : PGE_Menuitem(it)
{
    this->flag = it.flag;
}

PGE_BoolMenuItem::~PGE_BoolMenuItem() {}

void PGE_BoolMenuItem::left()
{
    toggle();
}

void PGE_BoolMenuItem::right()
{
    toggle();
}

void PGE_BoolMenuItem::render(int x, int y)
{
    PGE_Menuitem::render(x, y);
    if(flag)
        FontManager::printText((*flag)?"ON":"OFF", x+valueOffset, y);
}

void PGE_BoolMenuItem::toggle()
{
    PGE_Audio::playSoundByRole(obj_sound_role::PlayerClimb);
    if(flag)
        *flag=!(*flag);
    extAction();
}



/**************************Integer menu item************************************/

PGE_IntMenuItem::PGE_IntMenuItem() : PGE_Menuitem()
{
    intvalue=NULL;
    type=ITEM_Int;
    min=0;
    max=0;
    allowRotation=false;
}

PGE_IntMenuItem::PGE_IntMenuItem(const PGE_IntMenuItem &it) : PGE_Menuitem(it)
{
    this->intvalue = it.intvalue;
    this->min = it.min;
    this->max = it.max;
    this->allowRotation = it.allowRotation;
}

PGE_IntMenuItem::~PGE_IntMenuItem()
{}

void PGE_IntMenuItem::left()
{
    if(!intvalue) return;
    PGE_Audio::playSoundByRole(obj_sound_role::PlayerClimb);
    (*intvalue)--;
    if((*intvalue)<min)
        *intvalue=allowRotation?max:min;

    extAction();
}

void PGE_IntMenuItem::right()
{
    if(!intvalue) return;
    PGE_Audio::playSoundByRole(obj_sound_role::PlayerClimb);
    (*intvalue)++;
    if((*intvalue)>max)
        *intvalue=allowRotation?min:max;

    extAction();
}

void PGE_IntMenuItem::render(int x, int y)
{
    PGE_Menuitem::render(x, y);
    if(intvalue)
        FontManager::printText(QString::number(*intvalue), x+valueOffset, y);
}


/**************************Integer menu item************************************/


/**************************Labeled Integer menu item************************************/

PGE_NamedIntMenuItem::PGE_NamedIntMenuItem() : PGE_Menuitem()
{
    intvalue=NULL;
    type=ITEM_NamedInt;
    curItem=0;
    allowRotation=false;
}

PGE_NamedIntMenuItem::PGE_NamedIntMenuItem(const PGE_NamedIntMenuItem &it) : PGE_Menuitem(it)
{
    this->intvalue = it.intvalue;
    this->items = it.items;
    this->curItem = it.curItem;
    this->allowRotation = it.allowRotation;
}

PGE_NamedIntMenuItem::~PGE_NamedIntMenuItem()
{}

void PGE_NamedIntMenuItem::left()
{
    if(!intvalue) return;
    if(items.isEmpty()) return;

    PGE_Audio::playSoundByRole(obj_sound_role::PlayerClimb);
    curItem--;
    if(curItem<0)
        curItem = allowRotation?(items.size()-1):0;
    *intvalue=items[curItem].value;
    extAction();
}

void PGE_NamedIntMenuItem::right()
{
    if(!intvalue) return;
    if(items.isEmpty()) return;
    PGE_Audio::playSoundByRole(obj_sound_role::PlayerClimb);
    curItem++;
    if(curItem>=items.size())
        curItem = allowRotation?  0  : (items.size()-1);

    *intvalue=items[curItem].value;

    extAction();
}

void PGE_NamedIntMenuItem::render(int x, int y)
{
    PGE_Menuitem::render(x, y);
    if(!items.isEmpty())
        FontManager::printText(items[curItem].label, x+valueOffset, y);
}

/**************************Labeled Integer menu item************************************/


/**************************Key Grabber menu utem************************************/
PGE_KeyGrabMenuItem::PGE_KeyGrabMenuItem() : PGE_Menuitem()
{
    keyValue=NULL;
    type=ITEM_KeyGrab;
    menu=NULL;
    chosing=false;
}

PGE_KeyGrabMenuItem::PGE_KeyGrabMenuItem(const PGE_KeyGrabMenuItem &it) : PGE_Menuitem(it)
{
    this->keyValue = it.keyValue;
    this->menu = it.menu;
    this->type = it.type;
    this->chosing = it.chosing;
}

PGE_KeyGrabMenuItem::~PGE_KeyGrabMenuItem()
{}

void PGE_KeyGrabMenuItem::grabKey()
{
    chosing=true;
    if(keyValue)
    {
        if(menu)
        {
            menu->is_keygrab=true;
            menu->m_item = this;
        }
    }
}

void PGE_KeyGrabMenuItem::pushKey(int scancode)
{
    chosing=false;
    if(keyValue)
    {
        if(scancode>=0)//if -1 - cancel grabbing of key
        {
            *keyValue=scancode;
            PGE_Audio::playSoundByRole(obj_sound_role::MenuDo);
        } else if(scancode==PGE_KEYGRAB_CANCEL)//Cancel key grabbing
        {
            PGE_Audio::playSoundByRole(obj_sound_role::BlockHit);
        }
        else //Remove control key
        {
            *keyValue=-1;
            PGE_Audio::playSoundByRole(obj_sound_role::NpcLavaBurn);
        }
        if(menu) menu->is_keygrab=false;
    }
}

void PGE_KeyGrabMenuItem::render(int x, int y)
{
    PGE_Menuitem::render(x, y);
    if(chosing)
        FontManager::printText(QString("..."), x+210, y);
    else
    if(keyValue)
        FontManager::printText(QString(SDL_GetScancodeName((SDL_Scancode)*keyValue)), x+210, y);
}

/**************************Key Grabber menu utem************************************/
