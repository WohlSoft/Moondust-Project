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

#ifndef LVL_SECTION_H
#define LVL_SECTION_H

#include <PGE_File_Formats/lvl_filedata.h>
#include <common_features/rectf.h>
#include <Utils/dir_list_ci.h>

#include "lvl_backgrnd.h"

typedef std::vector<PGE_Phys_Object * > R_itemList;

class PGE_Phys_Object;
class PGE_LevelCamera;
class LevelScene;

class LVL_Section
{
    friend class PGE_Phys_Object;
    friend class PGE_LevelCamera;
public:
    LVL_Section();
    LVL_Section(const LVL_Section &_sct);
    ~LVL_Section();

    /*!
     * \brief Give actual scene pointer (needed for in-scene background render)
     * \param scene Actual level scene pointer
     */
    void setScene(LevelScene *scene);

    /*!
     * \brief Initialize section with level file data
     * \param _d Reference to actual section data
     */
    void setData(const LevelSection &_d);

    LevelSection m_data;
    //void registerElement(PGE_Phys_Object *item);
    //void unregisterElement(PGE_Phys_Object *item);
    //void queryItems(PGE_RectF zone, R_itemList *resultList);
    //void queryItems(double x, double y, R_itemList *resultList);

    void changeSectionBorders(long left, long top, long right, long bottom);
    void changeLimitBorders(long left, long top, long right, long bottom);
    void resetLimits();

    void initBG();
    void setMusicRoot(std::string rootPath, std::string dataDirName);
    unsigned long getBgId();

    void playMusic();
    void resetMusic();
    void setMusic(unsigned int musID);
    void setMusic(std::string musFile);

    void renderBackground(double x, double y, double w, double h);
    void renderInScene(double x, double y, double w, double h);
    void renderForeground(double x, double y, double w, double h);

    void setBG(uint64_t bgID);
    void resetBG();

    bool m_isAutoscroll;
    double m_autoscrollVelocityX;
    double m_autoscrollVelocityY;

    PGE_RectF sectionRect();
    PGE_RectF sectionLimitBox();

    bool isWrapH();
    bool isWrapV();
    bool LeftOnly();
    bool RightOnly();
    bool ExitOffscreen();

    int  getPhysicalEnvironment();

private:
    //typedef double RPoint[2];
    //typedef RTree<PGE_Phys_Object *, double, 2, double > IndexTree;
    //IndexTree tree;

    LevelScene      *m_scene = nullptr;

    DirListCI       m_music_root_ci;
    std::string     m_music_root;
    std::string     m_music_dataDirName;
    uint64_t        m_curMus = 0l;
    std::string     m_curCustomMus;
    uint64_t        m_curBgID = 0l;
    LVL_Background  m_background;

    bool m_isInit = false;

    /// Limits of section motion
    PGE_RectF m_limitBox;

    /// Default section box
    PGE_RectF m_sectionBox;
};

#endif // LVL_SECTION_H
