#include "maths.h"
#include <cmath>

Maths::Maths()
{}

Maths::~Maths()
{}

long Maths::roundTo(long src, long gridSize)
{
    long gridX;
    if(gridSize>0)
    { //ATTACH TO GRID
        gridX = (src - src % gridSize);

        if(src<0)
        {
            if( src < gridX - (long)(gridSize/2.0) )
                gridX -= gridSize;
        }
        else
        {
            if( src > gridX + (long)(gridSize/2.0) )
                gridX += gridSize;
        }
        return gridX;
    }
    else
        return src;
}

double Maths::roundTo(double src, double gridSize)
{
    double gridX;
    src=floor(src);
    if(gridSize>0)
    { //ATTACH TO GRID
        gridX = (src - (long)src % (long)gridSize);

        if(src<0)
        {
            if( src < gridX - floor(gridSize/2.0) )
                gridX -= gridSize;
        }
        else
        {
            if( src > gridX + floor(gridSize/2.0) )
                gridX += gridSize;
        }
        return gridX;
    }
    else
        return src;
}

