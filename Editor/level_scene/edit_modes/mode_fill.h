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

#ifndef MODE_FILL_H
#define MODE_FILL_H

#include "../../common_features/edit_mode_base.h"

class LVL_ModeFill : public EditMode
{
public:
    LVL_ModeFill(QGraphicsScene * parentScene = 0, QObject *parent = 0);
    ~LVL_ModeFill();
    void set();
    void mousePress(QGraphicsSceneMouseEvent *mouseEvent);
    void mouseMove(QGraphicsSceneMouseEvent *mouseEvent);
    void mouseRelease(QGraphicsSceneMouseEvent *mouseEvent);

    void keyPress(QKeyEvent *keyEvent);
    void keyRelease(QKeyEvent *keyEvent);
};

#endif // MODE_FILL_H
