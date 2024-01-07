/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2014-2024 Vitaly Novichkov <admin@wohlnet.ru>
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

#pragma once
#ifndef ItemLevel_H
#define ItemLevel_H

#include <PGE_File_Formats/wld_filedata.h>

#include "../wld_scene.h"
#include "wld_base_item.h"

class ItemLevel : public WldBaseItem
{
    Q_OBJECT
    void construct();
public:
    ItemLevel(QGraphicsItem *parent = nullptr);
    ItemLevel(WldScene *parentScene, QGraphicsItem *parent = nullptr);
    ~ItemLevel();

    void setLevelData(WorldLevelTile inD, obj_w_level *mergedSet = nullptr,
                      long *animator_id = nullptr, long *path_id = nullptr,
                      long *bPath_id = nullptr);

    QRectF boundingRect() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

    //////Animation////////
    void setAnimator(long aniID, long path = 0, long bPath = 0);

    //void setLayer(QString layer);

    void transformTo(long target_id);

    void arrayApply();
    void removeFromArray();

    void   returnBack();
    QPoint sourcePos();

    void setShowSmallPathBG(bool p);
    void setShowBigPathBG(bool p);

    void alwaysVisible(bool v);

    WorldLevelTile m_data;
    obj_w_level m_localProps;

    bool itemTypeIsLocked();
    void contextMenu(QGraphicsSceneMouseEvent *mouseEvent);

private:
    //! Offset X of renderable image of a "small path background"
    int     m_imgOffsetXp = 0;
    //! Offset Y of renderable image of a "small path background"
    int     m_imgOffsetYp = 0;

    //! Offset X of renderable image of a "big path background"
    int     m_imgOffsetXbp = 0;
    //! Offset Y of renderable image of a "big path background"
    int     m_imgOffsetYbp = 0;

    //! Maximal size of renderable area (include all additional background images)
    QRectF  m_imageSizeTarget;

    //! ID of a "small path background" image (one of Level Entrances ID's)
    long    m_pathID = 0;
    //! Size of a "small path background" renderable image
    QRectF  m_imageSizeP;

    //! ID of a "big path background" image (one of Level Entrances ID's)
    long    m_bPathID = 0;
    //! Size of a "big path background" renderable image
    QRectF  m_imageSizeBP;
};

#endif // ItemLevel_H
