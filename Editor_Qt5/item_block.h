#ifndef ITEM_BLOCK_H
#define ITEM_BLOCK_H

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
#include <math.h>
#include <QMenu>


class ItemBlock : public QObject, public QGraphicsPixmapItem
{
    Q_OBJECT
public:
    ItemBlock(QGraphicsPixmapItem *parent=0);
    ~ItemBlock();

    void setMainPixmap(const QPixmap &pixmap);
    void setContextMenu(QMenu &menu);

    QRectF boundingRect() const;

    QPixmap mainImage;
    QMenu *ItemMenu;
//    QGraphicsScene * scene;
//    QGraphicsPixmapItem * image;

    //////Animation////////
    void setAnimation(QPixmap &sprite, int frames);
    void draw();
    QPoint fPos() const;
    void setFrame(int);

protected:
    virtual void contextMenuEvent( QGraphicsSceneContextMenuEvent * event );

private slots:
    void nextFrame();

private:
    bool animated;
    QPixmap farameSet;
    int frameCurrent;
    QTimer * timer;
    QPoint framePos;
    int framesQ;
    int frameSize; // size of one frame
    int frameWidth; // sprite width
    int frameHeight; //sprite height
    QPixmap currentImage;
};

#endif // ITEM_BLOCK_H
