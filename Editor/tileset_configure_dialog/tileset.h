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

#ifndef TILESET_H
#define TILESET_H

#include <QWidget>

class tileset : public QWidget
{
    Q_OBJECT
public:
    explicit tileset(QWidget *parent = 0, int m_baseSize = 32, int rows = 3, int cols = 3);

    void clear();

    int rows() const;
    void setRows(int rows);

    int cols() const;
    void setCols(int cols);

    int getBaseSize() const;
    void setBaseSize(int value);

signals:

public slots:


protected:
    void paintEvent(QPaintEvent* ev);
    void dragEnterEvent(QDragEnterEvent *event);
    void dragLeaveEvent(QDragLeaveEvent *event);
    void dragMoveEvent(QDragMoveEvent *event);
    void dropEvent(QDropEvent *event);
    void mousePressEvent(QMouseEvent * event);


private:
    int findPiece(const QRect &pieceRect) const;
    const QRect targetSquare(const QPoint &position) const;

    QList<QPixmap> piecePixmaps;
    QList<QRect> pieceRects;
    QRect highlightedRect;
    int inPlace;

    int m_rows;
    int m_cols;
    int m_baseSize;

    void updateSize();
};

#endif // TILESET_H
