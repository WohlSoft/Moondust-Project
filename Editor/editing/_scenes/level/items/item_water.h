/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2014-2023 Vitaly Novichkov <admin@wohlnet.ru>
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
#ifndef ITEM_WATER_H
#define ITEM_WATER_H

#include <PGE_File_Formats/lvl_filedata.h>

#include "../lvl_scene.h"
#include "lvl_base_item.h"

class ItemPhysEnv : public LvlBaseItem
{
    Q_OBJECT
    void construct();
public:
    ItemPhysEnv(QGraphicsItem *parent = nullptr);
    ItemPhysEnv(LvlScene *parentScene, QGraphicsItem *parent = nullptr);
    ~ItemPhysEnv();

    void setSize(QSize sz);
    void setRectSize(QRect rect);

    void setType(int tp);
    void setPhysEnvData(LevelPhysEnv inD);

    void refreshItemSize();

    QRectF boundingRect() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *);

    void setLayer(QString layer);

    void arrayApply();
    void removeFromArray();

    void    returnBack();
    QPoint  sourcePos();

    LevelPhysEnv m_data;

    QString getLayerName();
    bool itemTypeIsLocked();
    void contextMenu(QGraphicsSceneMouseEvent *mouseEvent);

    QPainterPath shape() const;

private:
    void updateColor();

    QSize       m_waterSize;
    int         m_penWidth;

    QPen        m_pen;
    QColor      m_color;
    QBrush      m_brush;
    QBrush      m_brushSubBorder;
};

#endif // ITEM_WATER_H
