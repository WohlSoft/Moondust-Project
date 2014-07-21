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
#include "../data_configs/data_configs.h"

class tileset : public QWidget
{
    Q_OBJECT
public:
    enum TilesetType{
        LEVELTILESET_BLOCK = 0,
        LEVELTILESET_BGO,
        LEVELTILESET_NPC,
        WORLDTILESET_TILE
    };

    explicit tileset(dataconfigs *conf, TilesetType type, QWidget *parent = 0, int m_baseSize = 32, int rows = 3, int cols = 3);

    void clear();

    int rows() const;
    int cols() const;
    int getBaseSize() const;
    TilesetType type() const;
    bool editMode() const;

    struct SimpleTilesetItem{
        unsigned int row,col,id;
    };

    struct SimpleTileset{
        unsigned int rows, cols, baseSize;
        TilesetType type;
        QList<SimpleTilesetItem> items;
    };

    SimpleTileset toSimpleTileset();
    void loadSimpleTileset(const SimpleTileset &tileset);

    static void SaveSimpleTileset(const QString &path, const SimpleTileset &tileset);
    static bool OpenSimpleTileset(const QString &path, SimpleTileset &tileset);

signals:
    void clickedItem(tileset::TilesetType type, long id);

public slots:
    void setRows(int rows);
    void setCols(int cols);
    void setBaseSize(int value);
    void setType(const TilesetType &type);
    void setEditMode(bool editMode);

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
    QPixmap getScaledPixmapById(const unsigned int &id) const;
    QString getMimeType();

    QList<QPixmap> piecePixmaps;
    QList<QRect> pieceRects;
    QList<long> pieceID;
    QRect highlightedRect;
    int inPlace;

    int m_rows;
    int m_cols;
    int m_baseSize;
    dataconfigs *m_conf;
    TilesetType m_type;
    bool m_editMode;

    void updateSize();
    void removeOuterItems(QRect updatedRect);
};

#endif // TILESET_H
