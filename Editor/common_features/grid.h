#ifndef GRID_H
#define GRID_H

#include <QPoint>

class Grid
{
public:
    Grid();
    static QPoint applyGrid(QPoint source, int gridSize, QPoint gridOffset=QPoint(0,0));
};

#endif // GRID_H
