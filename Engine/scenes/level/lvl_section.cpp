/*
 * Moondust, a free game engine for platform game making
 * Copyright (c) 2014-2021 Vitaly Novichkov <admin@wohlnet.ru>
 *
 * This software is licensed under a dual license system (MIT or GPL version 3 or later).
 * This means you are free to choose with which of both licenses (MIT or GPL version 3 or later)
 * you want to use this software.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *
 * You can see text of MIT license in the LICENSE.mit file you can see in Engine folder,
 * or see https://mit-license.org/.
 *
 * You can see text of GPLv3 license in the LICENSE.gpl3 file you can see in Engine folder,
 * or see <http://www.gnu.org/licenses/>.
 */

#include "lvl_section.h"
#include "lvl_base_object.h"
#include <PGE_File_Formats/file_formats.h>
#include <audio/play_music.h>
#include <data_configs/config_manager.h>
#include <algorithm>

#include "lvl_physenv.h"

LVL_Section::LVL_Section()
{
    m_data = FileFormats::CreateLvlSection();
    m_isInit = false;
    m_curMus = 0;
    m_curBgID = 0;
    m_isAutoscroll = false;
    m_autoscrollVelocityX = 0.0;
    m_autoscrollVelocityY = 0.0;
}

LVL_Section::LVL_Section(const LVL_Section &m_sct)
{
    m_isInit = m_sct.m_isInit;
    m_sectionBox = m_sct.m_sectionBox;
    m_limitBox = m_sct.m_limitBox;
    m_data = m_sct.m_data;
    m_scene = m_sct.m_scene;
    m_music_root = m_sct.m_music_root;
    m_curMus = m_sct.m_curMus;
    m_curCustomMus = m_sct.m_curCustomMus;
    m_curBgID = m_sct.m_curBgID;
    m_isAutoscroll = m_sct.m_isAutoscroll;
    m_autoscrollVelocityX = m_sct.m_autoscrollVelocityX;
    m_autoscrollVelocityY = m_sct.m_autoscrollVelocityY;
}

LVL_Section::~LVL_Section()
{
    //tree.RemoveAll();
}

void LVL_Section::setScene(LevelScene *scene)
{
    m_scene = scene;
}

void LVL_Section::setData(const LevelSection &_d)
{
    m_data = _d;
    m_isInit = true;
    resetMusic();
    m_curBgID = m_data.background;
    changeSectionBorders(m_data.size_left, m_data.size_top, m_data.size_right, m_data.size_bottom);
}

void LVL_Section::changeSectionBorders(long left, long top, long right, long bottom)
{
    m_sectionBox.setTopLeft(left, top);
    m_sectionBox.setBottomRight(right, bottom);
    m_limitBox = m_sectionBox;
    m_background.setBox(m_sectionBox);
}

void LVL_Section::changeLimitBorders(long left, long top, long right, long bottom)
{
    m_limitBox.setTopLeft(left, top);
    m_limitBox.setBottomRight(right, bottom);
}

void LVL_Section::resetLimits()
{
    m_limitBox = m_sectionBox;
}

void LVL_Section::setMusicRoot(std::string _path)
{
    m_music_root = _path;
    if(!m_music_root.empty())
    {
        if(m_music_root.back() != '/')
            m_music_root.push_back('/');
        m_music_root_ci.setCurDir(m_music_root);
    }
}

void LVL_Section::playMusic()
{
    std::string musFile = m_curCustomMus;
    std::replace(musFile.begin(), musFile.end(), '\\', '/');
    musFile = ConfigManager::getLvlMusic(m_curMus, m_music_root + m_music_root_ci.resolveFileCase(musFile));

    if(!musFile.empty())
    {
        PGE_MusPlayer::openFile(musFile);
        PGE_MusPlayer::play();
    }
    else
        PGE_MusPlayer::stop();
}

void LVL_Section::resetMusic()
{
    m_curMus = m_data.music_id;
    m_curCustomMus = m_data.music_file;
}

void LVL_Section::setMusic(unsigned int musID)
{
    m_curMus = musID;
}

void LVL_Section::setMusic(std::string musFile)
{
    m_curCustomMus = musFile;
}

void LVL_Section::renderBackground(double x, double y, double w, double h)
{
    m_background.drawBack(x, y, w, h);
}

void LVL_Section::renderInScene(double x, double y, double w, double h)
{
    m_background.drawInScene(x, y, w, h);
}

void LVL_Section::renderForeground(double x, double y, double w, double h)
{
    m_background.drawFront(x, y, w, h);
}

unsigned long LVL_Section::getBgId()
{
    return m_curBgID;
}

void LVL_Section::initBG()
{
    setBG(m_curBgID);
}

void LVL_Section::setBG(uint64_t bgID)
{
    if(m_background.isInit() && (bgID == m_background.curBgId()))
        return;
    if((bgID > 0) && ConfigManager::lvl_bg_indexes.contains(bgID))
        m_background.setBg(ConfigManager::lvl_bg_indexes[bgID], m_scene);
    else
        m_background.setBlank();

    m_curBgID = bgID;
}

void LVL_Section::resetBG()
{
    setBG(m_data.background);
}

PGE_RectF LVL_Section::sectionRect()
{
    return m_sectionBox;
}

PGE_RectF LVL_Section::sectionLimitBox()
{
    return m_limitBox;
}

bool LVL_Section::isWrapH()
{
    return m_data.wrap_h;
}

bool LVL_Section::isWrapV()
{
    return m_data.wrap_v;
}

bool LVL_Section::LeftOnly()
{
    return m_data.lock_right_scroll;
}

bool LVL_Section::RightOnly()
{
    return m_data.lock_left_scroll;
}

bool LVL_Section::ExitOffscreen()
{
    return m_data.OffScreenEn;
}

int LVL_Section::getPhysicalEnvironment()
{
    return m_data.underwater ? LVL_PhysEnv::Env_Water : LVL_PhysEnv::Env_Air ;
}

//void LVL_Section::registerElement(PGE_Phys_Object *item)
//{
//    RPoint lt = {item->m_treemap.m_posX_registered, item->m_treemap.m_posY_registered};
//    RPoint rb = {item->m_treemap.m_posX_registered + item->m_treemap.m_width_registered, item->m_treemap.m_posY_registered + item->m_treemap.m_height_registered};
//    tree.Insert(lt, rb, item);
//}

//void LVL_Section::unregisterElement(PGE_Phys_Object *item)
//{
//    RPoint lt = {item->m_treemap.m_posX_registered, item->m_treemap.m_posY_registered};
//    RPoint rb = {item->m_treemap.m_posX_registered + item->m_treemap.m_width_registered, item->m_treemap.m_posY_registered + item->m_treemap.m_height_registered};
//    tree.Remove(lt, rb, item);
//}


//bool _TreeSearchCallback(PGE_Phys_Object *item, void *arg)
//{
//    R_itemList *list = static_cast<R_itemList * >(arg);

//    if(list)
//    {
//        if(item)(*list).push_back(item);
//    }

//    return true;
//}

//void LVL_Section::queryItems(PGE_RectF zone, R_itemList *resultList)
//{
//    RPoint lt = {zone.left(), zone.top()};
//    RPoint rb = {zone.right(), zone.bottom()};
//    tree.Search(lt, rb, _TreeSearchCallback, (void *)resultList);
//}

//void LVL_Section::queryItems(double x, double y, R_itemList *resultList)
//{
//    PGE_RectF zone = PGE_RectF(x, y, 1, 1);
//    queryItems(zone, resultList);
//}
