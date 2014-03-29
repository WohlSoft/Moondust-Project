#include "lvlscene.h"
#include "lvl_filedata.h"
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsItemAnimation>
#include <QKeyEvent>
#include <QBitmap>
#include <QApplication>
#include <QGraphicsItem>
#include <QProgressDialog>
#include <QtCore>
#include "dataconfigs.h"

/*
Sprite::Sprite(QPixmap *image, int frames):mPos(0,0),mCurrentFrame(0)
{
    mSpriteImage = image;
    frm = frames;
}

void Sprite::draw( QPainter* painter)
{
    painter->drawPixmap ( mPos.x(),mPos.y(), *mSpriteImage,
                                   0, mCurrentFrame, mSpriteImage->width() ,mSpriteImage->height()/frm);
}

QPoint Sprite::pos() const
{
    return mPos;
}

void Sprite::nextFrame()
{
    //following variable keeps track which
    //frame to show from sprite sheet
    mCurrentFrame += mSpriteImage->height()/frm;
    if (mCurrentFrame >= mSpriteImage->height() )
        mCurrentFrame = 0;
    mPos.setY( mPos.y() + 10 );
}
*/



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
        addRect(QRectF(x, y, (int)fabs(x-w), (int)fabs(y-h)), pen, brush);
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

void LvlScene::setBGO(LevelData FileData, QProgressDialog &progress, dataconfigs &configs)
{
    int i=0, j, noimage=1;
    QPixmap img;
    QPixmap image = QPixmap(QApplication::applicationDirPath() + "/" + "data/unknown_bgo.gif");
    //QBitmap mask;
    //QBitmap mask = QBitmap(QApplication::applicationDirPath() + "/" + "data/unknown_npcm.gif");
    //QPixmap image = QPixmap(QApplication::applicationDirPath() + "/" + "data/unknown_npc.gif");

    QVector<UserBGOs > uBGOs;
    UserBGOs uBGO;
    QString uLVLDs = FileData.path + "/" + FileData.filename + "/";
    QString uLVLD = FileData.path + "/" + FileData.filename;
    QString uLVLs = FileData.path + "/";

    //Load user images
    for(i=0; i<FileData.bgo.size(); i++) //Add user images
    {
        for(j=0;j<configs.main_bgo.size();j++)
        {
            if(configs.main_bgo[j].id==FileData.bgo[i].id)
                break;
        }

        if((QFile::exists(uLVLD) ) &&
              (QFile::exists(uLVLDs + "background-"+
              QString::number(FileData.bgo[i].id) + ".gif")) )
        {
            if(QFile::exists(uLVLDs + "background-"+QString::number(FileData.bgo[i].id) + "m.gif"))
                uBGO.mask = QBitmap(uLVLDs + "background-"+QString::number(FileData.bgo[i].id) + "m.gif" );
            else
                uBGO.mask = configs.main_bgo[j].mask;

            uBGO.image = QPixmap(uLVLDs + "background-"+
                QString::number(FileData.bgo[i].id) + ".gif" );

            if((uBGO.image.height()!=uBGO.mask.height())||(uBGO.image.width()!=uBGO.mask.width()))
                uBGO.mask = uBGO.mask.copy(0,0,uBGO.image.width(),uBGO.image.height());
            uBGO.image.setMask(uBGO.mask);
            uBGO.id = FileData.bgo[i].id;
            uBGOs.push_back(uBGO);
        }
        else
        if(QFile::exists(uLVLs + "background-"+
            QString::number(FileData.bgo[i].id) + ".gif") )
        {
            if(QFile::exists(uLVLs + "background-"+QString::number(FileData.bgo[i].id) + "m.gif"))
                uBGO.mask = QBitmap(uLVLs + "background-"+QString::number(FileData.bgo[i].id) + "m.gif" );
            else
                uBGO.mask = configs.main_bgo[j].mask;

            uBGO.image = QPixmap(uLVLs + "background-"+
                QString::number(FileData.bgo[i].id) + ".gif" );

            if((uBGO.image.height()!=uBGO.mask.height())||(uBGO.image.width()!=uBGO.mask.width()))
                uBGO.mask = uBGO.mask.copy(0,0,uBGO.image.width(),uBGO.image.height());

            uBGO.image.setMask(uBGO.mask);
            uBGO.id = FileData.bgo[i].id;
            uBGOs.push_back(uBGO);
        }

        progress.setValue(progress.value()+1);
    }

    QGraphicsItem *	box;
    int isUser=0;

    for(i=0; i<FileData.bgo.size(); i++)
    {
        noimage=1;
        isUser=0;
        for(j=0;j<uBGOs.size();j++)
        {
            if(uBGOs[j].id==FileData.bgo[i].id)
            {
                isUser=1;
                img = uBGOs[j].image;
                break;
            }
        }

        for(j=0;j<configs.main_bgo.size();j++)
        {
            if(configs.main_bgo[j].id==FileData.bgo[i].id)
            {
                noimage=1;
                if(!isUser)
                img = configs.main_bgo[j].image; break;
            }
        }
        if(!noimage)
        {
            img=image;
        }

        if((noimage) && (configs.main_bgo[j].animated))
        {
            img=img.copy(0, 0, img.width(), (int)round(img.height()/configs.main_bgo[j].frames));
        }

        box = addPixmap(QPixmap(img));
        box->translate(FileData.bgo[i].x, FileData.bgo[i].y);

        box->setFlag(QGraphicsItem::ItemIsSelectable, true);
        //box->setFlag(QGraphicsScene::BackgroundLayer);
        box->setData(0, "BGO");
        box->setData(1, QString::number(FileData.bgo[i].id) );

        progress.setValue(progress.value()+1);
    }

}

void LvlScene::setNPC(LevelData FileData, QProgressDialog &progress)
{
    int i=0;
    QBitmap mask = QBitmap(QApplication::applicationDirPath() + "/" + "data/unknown_npcm.gif");
    QPixmap image = QPixmap(QApplication::applicationDirPath() + "/" + "data/unknown_npc.gif");
    image.setMask(mask);


    for(i=0; i<FileData.npc.size(); i++)
    {
        placeBlock(FileData.npc[i].x, FileData.npc[i].y, image);
        progress.setValue(progress.value()+1);
    }

}

