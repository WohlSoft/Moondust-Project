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

#ifndef ItemLevel_H
#define ItemLevel_H

#include <QGraphicsItem>
#include <QGraphicsPixmapItem>
#include <QGraphicsScene>
#include <QGraphicsSceneContextMenuEvent>
#include <QString>
#include <QPoint>
#include <QObject>
#include <QGraphicsItem>
#include <QPainter>
#include <QTimer>
#include <QMenu>
#include <math.h>

#include <file_formats/wld_filedata.h>

#include "wld_scene.h"

class ItemLevel : public QObject, public QGraphicsItem
{
    Q_OBJECT
    Q_INTERFACES(QGraphicsItem)
public:
    ItemLevel(QGraphicsItem *parent=0);
    ~ItemLevel();

    void setLevelData(WorldLevels inD);
    void setContextMenu(QMenu &menu);
    void setScenePoint(WldScene *theScene);

    QRectF boundingRect() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

    QMenu *ItemMenu;

    //////Animation////////
    void setAnimator(long aniID, long path=0, long bPath=0);

    //void setLayer(QString layer);

    void arrayApply();
    void removeFromArray();

    void setPath(bool p);
    void setbPath(bool p);

    void alwaysVisible(bool v);

    WorldLevels levelData;

    int gridSize;
    int gridOffsetX;
    int gridOffsetY;
    int imgOffsetX;
    int imgOffsetY;

    int imgOffsetXp;
    int imgOffsetYp;

    int imgOffsetXbp;
    int imgOffsetYbp;

    //Locks
    bool isLocked;
    void setLocked(bool lock);


protected:
    bool mouseLeft;
    bool mouseMid;
    bool mouseRight;
    virtual void contextMenuEvent( QGraphicsSceneContextMenuEvent * event );
    virtual void mousePressEvent ( QGraphicsSceneMouseEvent * mouseEvent );
    virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent *mouseEvent);

private:

    QRectF imageSizeTarget;

    long animatorID;
    QRectF imageSize;

    long pathID;
    QRectF imageSizeP;

    long bPathID;
    QRectF imageSizeBP;

    bool animated;
    WldScene * scene;

};

#endif // ItemLevel_H
