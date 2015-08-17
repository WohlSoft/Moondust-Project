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

#include "lvl_section.h"
#include "lvl_base_object.h"
#include <PGE_File_Formats/file_formats.h>
#include <audio/SdlMusPlayer.h>
#include <data_configs/config_manager.h>

#include "lvl_physenv.h"

LVL_Section::LVL_Section()
{
    data= FileFormats::dummyLvlSection();
    isInit=false;
    curMus=0;
    curBgID=0;
    isAutoscroll=false;
    _autoscrollVelocityX=0.0f;
    _autoscrollVelocityY=0.0f;
}

LVL_Section::LVL_Section(const LVL_Section &_sct)
{
    isInit=_sct.isInit;
    sectionBox=_sct.sectionBox;
    limitBox=_sct.limitBox;
    data=_sct.data;
    music_root=_sct.music_root;
    curMus=_sct.curMus;
    curCustomMus=_sct.curCustomMus;
    curBgID=_sct.curBgID;
    isAutoscroll=_sct.isAutoscroll;
    _autoscrollVelocityX=_sct._autoscrollVelocityX;
    _autoscrollVelocityY=_sct._autoscrollVelocityY;
}

LVL_Section::~LVL_Section()
{
    tree.RemoveAll();
}

void LVL_Section::setData(LevelSection _d)
{
    data=_d;
    isInit=true;
    resetMusic();
    curBgID = data.background;
    changeSectionBorders(data.size_left, data.size_top, data.size_right, data.size_bottom);
}

void LVL_Section::changeSectionBorders(long left, long top, long right, long bottom)
{
    sectionBox.setTopLeft(left, top);
    sectionBox.setBottomRight(right, bottom);
    limitBox = sectionBox;
    _background.setBox(sectionBox);
}

void LVL_Section::changeLimitBorders(long left, long top, long right, long bottom)
{
    limitBox.setTopLeft(left, top);
    limitBox.setBottomRight(right, bottom);
}

void LVL_Section::resetLimits()
{
    limitBox=sectionBox;
}

void LVL_Section::setMusicRoot(QString _path)
{
    music_root=_path;
    if(!music_root.endsWith('/'))
        music_root.append('/');
}

void LVL_Section::playMusic()
{
    QString musFile = ConfigManager::getLvlMusic(curMus, music_root+curCustomMus.replace('\\', '/'));
    if(!musFile.isEmpty())
    {
        PGE_MusPlayer::MUS_openFile(musFile);
        PGE_MusPlayer::MUS_playMusic();
    }
    else
        PGE_MusPlayer::MUS_stopMusic();
}

void LVL_Section::resetMusic()
{
    curMus = data.music_id;
    curCustomMus = data.music_file;
}

void LVL_Section::setMusic(int musID)
{
    curMus = musID;
}

void LVL_Section::setMusic(QString musFile)
{
    curCustomMus = musFile;
}

void LVL_Section::renderBG(float x, float y, float w, float h)
{
    _background.draw(x, y, w, h);
}

int LVL_Section::getBgId()
{
    return curBgID;
}

void LVL_Section::initBG()
{
    setBG(curBgID);
}

void LVL_Section::setBG(int bgID)
{
    if(_background.isInit() && (bgID==_background.curBgId())) return;

    if(ConfigManager::lvl_bg_indexes.contains(bgID))
    {
        obj_BG*bgSetup = &ConfigManager::lvl_bg_indexes[bgID];
        _background.setBg(*bgSetup);
    }
    else
        _background.setNone();

    curBgID=bgID;
}

void LVL_Section::resetBG()
{
    setBG(data.background);
}

PGE_RectF LVL_Section::sectionRect()
{
    return sectionBox;
}

PGE_RectF LVL_Section::sectionLimitBox()
{
    return limitBox;
}

bool LVL_Section::isWarp()
{
    return data.IsWarp;
}

bool LVL_Section::RightOnly()
{
    return data.lock_left_scroll;
}

bool LVL_Section::ExitOffscreen()
{
    return data.OffScreenEn;
}

int LVL_Section::getPhysicalEnvironment()
{
    return data.underwater ? LVL_PhysEnv::Env_Water : LVL_PhysEnv::Env_Air ;
}

void LVL_Section::registerElement(PGE_Phys_Object *item)
{
    RPoint lt={item->_posX, item->_posY};
    RPoint rb={item->_posX+item->_width, item->_posY+item->_height};
    tree.Insert(lt, rb, item);
}

void LVL_Section::unregisterElement(PGE_Phys_Object *item)
{
    RPoint lt={item->_posX, item->_posY};
    RPoint rb={item->_posX+item->_width, item->_posY+item->_height};
    tree.Remove(lt, rb, item);
}


bool _TreeSearchCallback(PGE_Phys_Object* item, void* arg)
{
    R_itemList* list = static_cast<R_itemList* >(arg);
    if(list)
    {
        if(item) (*list).push_back(item);
    }
    return true;
}

void LVL_Section::queryItems(PGE_RectF zone, R_itemList *resultList)
{
    RPoint lt={zone.left(), zone.top()};
    RPoint rb={zone.right(), zone.bottom()};
    tree.Search(lt, rb, _TreeSearchCallback, (void*)resultList);
}

void LVL_Section::queryItems(double x, double y, R_itemList *resultList)
{
    PGE_RectF zone=PGE_RectF(x, y, 1, 1);
    queryItems(zone, resultList);
}

