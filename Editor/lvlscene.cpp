#include "lvlscene.h"
#include "lvl_filedata.h"
#include <QGraphicsSceneMouseEvent>
#include <QKeyEvent>
#include <QApplication>
#include <QGraphicsItem>
#include <QProgressDialog>
#include <QtCore>

LvlScene::LvlScene(QObject *parent) : QGraphicsScene(parent)
{

}

void LvlScene::makeSectionBG(int x, int y, int w, int h)
{
    //float walls[11][4] = {{0, 0, 25, 245}, {25, 0, 425, 25}, {425, 0, 25, 245}, {25, 220, 400, 25}, {25, 60, 75, 25}, {100, 60, 25, 95}, {50, 120, 25, 25}, {175, 60, 25, 100}, {225, 90, 125, 25}, {275, 190, 25, 30}, {325, 115, 25, 30}};
    QBrush brush(QColor(255, 255, 255), QPixmap(QApplication::applicationDirPath() + "/" + "data/nobg.gif"));
    QPen pen(Qt::NoPen);
    //for (int i = 0; i < 11; i++) {
        QGraphicsItem * item =
        addRect(QRectF(QPoint(x, y), QSize(w, h)), pen, brush);
        item->setData(0, "BackGround");
    //}
}

QGraphicsItem * LvlScene::itemCollidesWith(QGraphicsItem * item)
{
    QList<QGraphicsItem *> collisions = collidingItems(item);
    foreach (QGraphicsItem * it, collisions) {
            if (it == item)
                 continue;
        return it;
    }
    return NULL;
}

void LvlScene::placeBox(float x, float y)
{
    //QImage bxc(":/images/unknown_block.gif");
    QGraphicsItem *	box = addPixmap(QPixmap(QApplication::applicationDirPath() + "/" + "data/graphics/level/block/block-8.gif"));
    box->translate(x, y);
    if (itemCollidesWith(box))
        removeItem(box);
    else
        box->setData(0, "Box");
}

void LvlScene::placeBlock(float x, float y, QPixmap &img)
{
    //QImage bxc(":/images/unknown_block.gif");
    QGraphicsItem *	box = addPixmap(QPixmap(img));
    box->translate(x, y);
    box->setFlag(QGraphicsItem::ItemIsSelectable,true);
    //if (itemCollidesWith(box))
    //    removeItem(box);
    //else
    //    box->setData(0, "Block");
}

void LvlScene::setBlocks(LevelData FileData, QProgressDialog &progress)
{
    int i=0;
    QPixmap image = QPixmap(QApplication::applicationDirPath() + "/" + "data/unknown_block.gif");
    for(i=0; i<FileData.blocks.size(); i++)
    {
        placeBlock(FileData.blocks[i].x, FileData.blocks[i].y, image);
        progress.setValue(progress.value()+1);
    }

}

void LvlScene::setBGO(LevelData FileData, QProgressDialog &progress)
{
    int i=0;
    QPixmap image = QPixmap(QApplication::applicationDirPath() + "/" + "data/unknown_bgo.gif");
    for(i=0; i<FileData.bgo.size(); i++)
    {
        placeBlock(FileData.bgo[i].x, FileData.bgo[i].y, image);
        progress.setValue(progress.value()+1);
    }

}

void LvlScene::setNPC(LevelData FileData, QProgressDialog &progress)
{
    int i=0;
    QPixmap image = QPixmap(QApplication::applicationDirPath() + "/" + "data/unknown_npc.gif");
    for(i=0; i<FileData.blocks.size(); i++)
    {
        placeBlock(FileData.npc[i].x, FileData.npc[i].y, image);
        progress.setValue(progress.value()+1);
    }

}

