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

#include "tileset.h"
#include <qpainter.h>
#include <QPaintEvent>
#include <QMimeData>
#include <QDrag>

tileset::tileset(QWidget *parent, int baseSize, int rows, int cols) :
    QWidget(parent)
{
    setAcceptDrops(true);
    m_baseSize = baseSize;
    m_rows = rows;
    m_cols = cols;
    updateSize();
}

void tileset::clear()
{
    piecePixmaps.clear();
    pieceRects.clear();
    highlightedRect = QRect();
    inPlace = 0;
    update();
}

void tileset::paintEvent(QPaintEvent *ev)
{
    QPainter painter;
    painter.begin(this);
    painter.fillRect(ev->rect(), Qt::white);

    if (highlightedRect.isValid()) {
        painter.setBrush(QColor("#ffcccc"));
        painter.setPen(Qt::NoPen);
        painter.drawRect(highlightedRect.adjusted(0, 0, -1, -1));
    }

    if(pieceRects.isEmpty()){
        painter.drawText(ev->rect(), Qt::AlignCenter, tr("Drag & Drop items to this box!"));
    }else{
        for (int i = 0; i < pieceRects.size(); ++i) {
            painter.drawPixmap(pieceRects[i], piecePixmaps[i]);
        }
    }

    painter.end();
}

void tileset::dragEnterEvent(QDragEnterEvent *event)
{
    if (event->mimeData()->hasFormat("image/x-pge-piece"))
        event->accept();
    else
        event->ignore();
}

void tileset::dragLeaveEvent(QDragLeaveEvent *event)
{
    QRect updateRect = highlightedRect;
    highlightedRect = QRect();
    update(updateRect);
    event->accept();
}

void tileset::dragMoveEvent(QDragMoveEvent *event)
{
    QRect updateRect = highlightedRect.united(targetSquare(event->pos()));

    if (event->mimeData()->hasFormat("image/x-pge-piece")
        && findPiece(targetSquare(event->pos())) == -1) {

        highlightedRect = targetSquare(event->pos());
        event->setDropAction(Qt::MoveAction);
        event->accept();
    } else {
        highlightedRect = QRect();
        event->ignore();
    }

    update(updateRect);
}

void tileset::dropEvent(QDropEvent *event)
{
    if (event->mimeData()->hasFormat("image/x-pge-piece")
        && findPiece(targetSquare(event->pos())) == -1) {

        QByteArray pieceData = event->mimeData()->data("image/x-pge-piece");
        QDataStream stream(&pieceData, QIODevice::ReadOnly);
        QRect square = targetSquare(event->pos());
        QPixmap pixmap;
        stream >> pixmap/* >> location*/;

        piecePixmaps.append(pixmap);
        pieceRects.append(square);

        highlightedRect = QRect();
        update();

        event->setDropAction(Qt::MoveAction);
        event->accept();

    } else {
        highlightedRect = QRect();
        event->ignore();
    }
}

void tileset::mousePressEvent(QMouseEvent *event)
{
    if(event->button() == Qt::RightButton){
        piecePixmaps.removeAt(findPiece(targetSquare(event->pos())));
        pieceRects.removeAt(findPiece(targetSquare(event->pos())));
        update();
        return;
    }

    QRect square = targetSquare(event->pos());
    int found = findPiece(square);

    if (found == -1)
        return;

    QPixmap pixmap = piecePixmaps[found];
    piecePixmaps.removeAt(found);
    pieceRects.removeAt(found);
    update();

    QByteArray itemData;
    QDataStream dataStream(&itemData, QIODevice::WriteOnly);

    dataStream << pixmap;

    QMimeData *mimeData = new QMimeData;
    mimeData->setData("image/x-pge-piece", itemData);

    QDrag *drag = new QDrag(this);
    drag->setMimeData(mimeData);
    drag->setHotSpot(event->pos() - square.topLeft());
    drag->setPixmap(pixmap);

    if (drag->start(Qt::MoveAction) == 0) {
        piecePixmaps.insert(found, pixmap);
        pieceRects.insert(found, square);
        update();
    }
}

int tileset::findPiece(const QRect &pieceRect) const
{
    for (int i = 0; i < pieceRects.size(); ++i) {
        if (pieceRect == pieceRects[i])
            return i;
    }
    return -1;
}

const QRect tileset::targetSquare(const QPoint &position) const
{
    return QRect(position.x()/getBaseSize() * getBaseSize(), position.y()/getBaseSize() * getBaseSize(), getBaseSize(), getBaseSize());
}
int tileset::getBaseSize() const
{
    return m_baseSize;
}

void tileset::setBaseSize(int value)
{
    m_baseSize = value;
    updateSize();
    update();
}


void tileset::updateSize()
{
    setMaximumSize(QSize(m_baseSize*m_cols, m_baseSize*m_rows));
    setMinimumSize(QSize(m_baseSize*m_cols, m_baseSize*m_rows));
}
int tileset::cols() const
{
    return m_cols;
}

void tileset::setCols(int cols)
{
    m_cols = cols;
    updateSize();
    update();
}

int tileset::rows() const
{
    return m_rows;
}

void tileset::setRows(int rows)
{
    m_rows = rows;
    updateSize();
    update();
}

