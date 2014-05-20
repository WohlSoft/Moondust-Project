#include "grid.h"

Grid::Grid()
{}

QPoint Grid::applyGrid(QPoint source, int gridSize, QPoint gridOffset)
{
    int gridX, gridY;
    if(gridSize>0)
    { //ATTACH TO GRID
        gridX = ((int)source.x() - (int)source.x() % gridSize);
        gridY = ((int)source.y() - (int)source.y() % gridSize);

        if((int)source.x()<0)
        {
            if( (int)source.x() < gridOffset.x()+gridX - (int)(gridSize/2) )
                gridX -= gridSize;
        }
        else
        {
            if( (int)source.x() > gridOffset.x()+gridX + (int)(gridSize/2) )
                gridX += gridSize;
        }

        if((int)source.y()<0)
        {if( (int)source.y() < gridOffset.y()+gridY - (int)(gridSize/2) )
            gridY -= gridSize;
        }
        else {if( (int)source.y() > gridOffset.y()+gridY + (int)(gridSize/2) )
         gridY += gridSize;
        }

        return QPoint(gridOffset.x()+gridX, gridOffset.y()+gridY);

    }
    else
        return source;
}
