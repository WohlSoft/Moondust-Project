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

#include "grid.h"

Grid::Grid()
{}

QPoint Grid::applyGrid(QPoint source, int gridSize, QPoint gridOffset)
{
    int gridX, gridY;
    QPoint gridOffset_d;
    if(gridSize>0)
    { //ATTACH TO GRID

        if(qAbs(gridOffset.x())==gridSize)
            gridOffset_d.setX(0);
        else
            gridOffset_d.setX(gridOffset.x());

        if(qAbs(gridOffset.y())==gridSize)
            gridOffset_d.setY(0);
        else
            gridOffset_d.setY(gridOffset.y());

        gridX = ((int)source.x() - (int)source.x() % gridSize);
        gridY = ((int)source.y() - (int)source.y() % gridSize);

        if((int)source.x()<0)
        {
            if( (int)source.x() < gridOffset_d.x()+gridX - (int)(gridSize/2) )
                gridX -= gridSize;
        }
        else
        {
            if( (int)source.x() > gridOffset_d.x()+gridX + (int)(gridSize/2) )
                gridX += gridSize;
        }

        if((int)source.y()<0)
        {if( (int)source.y() < gridOffset_d.y()+gridY - (int)(gridSize/2) )
            gridY -= gridSize;
        }
        else {if( (int)source.y() > gridOffset_d.y()+gridY + (int)(gridSize/2) )
         gridY += gridSize;
        }

        return QPoint(gridOffset_d.x()+gridX, gridOffset_d.y()+gridY);

    }
    else
        return source;
}
