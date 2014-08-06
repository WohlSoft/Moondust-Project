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

#include "tilesetitembutton.h"


TilesetItemButton::TilesetItemButton(dataconfigs *conf, QFrame *parent) :
    QFrame(parent)
{
    m_config = conf;
    setFrameStyle(QFrame::Panel | QFrame::Raised);
    setLineWidth(2);
}



dataconfigs *TilesetItemButton::config() const
{
    return m_config;
}

void TilesetItemButton::setConfig(dataconfigs *config)
{
    m_config = config;
}

void TilesetItemButton::paintEvent(QPaintEvent *ev)
{
    QPainter painter;
    painter.begin(this);
    painter.fillRect(ev->rect(), Qt::black);

    painter.end();

    QFrame::paintEvent(ev);
}

void TilesetItemButton::mousePressEvent(QMouseEvent *)
{
    setFrameStyle(QFrame::Panel | QFrame::Sunken);
}

void TilesetItemButton::mouseReleaseEvent(QMouseEvent *)
{
    setFrameStyle(QFrame::Panel | QFrame::Raised);
}

