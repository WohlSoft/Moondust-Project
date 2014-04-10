/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2014 Vitaly Novichkov <admin@wohlnet.ru>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA
 */

#include "lvlscene.h"
#include "lvl_filedata.h"
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsItemAnimation>
#include <QKeyEvent>
#include <QBitmap>
#include <QMessageBox>
#include <QApplication>
#include <QGraphicsItem>
#include <QProgressDialog>
#include <QtCore>
#include "dataconfigs.h"
#include <QDebug>

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
    //bgoback->setZValue(-10);
    //npcback->setZValue(-9);
    //blocks->setZValue(0);
    //npcfore->setZValue(5);
    //bgofore->setZValue(9);
    //cursor->setZValue(15);
}


///////////////////////////////BACKGROUND IMAGE/////////////////////////////////////////
void LvlScene::makeSectionBG(LevelData FileData, QProgressDialog &progress, dataconfigs &configs)
//void LvlScene::makeSectionBG(int x, int y, int w, int h)
{
    QGraphicsItem * item;
    QBrush brush;
    QPen pen;
    QPixmap img;
    QPixmap image = QPixmap(QApplication::applicationDirPath() + "/" + "data/nobg.gif");

    int i, j, total=0;
    bool isUser=false, noimage=false, found=false;
    long x,y,h,w;

    QVector<UserBGs > uBGs;
    UserBGs uBG;


    QString uLVLDs = FileData.path + "/" + FileData.filename + "/";
    QString uLVLD = FileData.path + "/" + FileData.filename;
    QString uLVLs = FileData.path + "/";

    //Load user images
    for(i=0; i<FileData.sections.size(); i++) //Add user images
    {
        found=false;
        for(j=0;j<configs.main_bg.size();j++)
        {
            if(configs.main_bg[j].id==FileData.sections[i].background)
            {
                found=true;
                break;
            }
        }

        if(found)
        {
            if((QFile::exists(uLVLD) ) &&
                  (QFile::exists(uLVLDs + configs.main_bg[j].image_n)) )
            {
                uBG.image = QPixmap( uLVLDs + configs.main_bg[j].image_n );
                uBG.id = configs.main_bg[j].id;
                uBGs.push_back(uBG);
            }
            else
            if(QFile::exists(uLVLs + configs.main_bg[j].image_n) )
            {
                uBG.image = QPixmap( uLVLs + configs.main_bg[j].image_n );
                uBG.id = configs.main_bg[j].id;
                uBGs.push_back(uBG);
            }
        }
        total++;
        progress.setValue(progress.value()+1);
    }

    for(i=0; i<FileData.sections.size(); i++)
    {
        if(
            (FileData.sections[i].size_left!=0) ||
            (FileData.sections[i].size_top!=0)||
            (FileData.sections[i].size_bottom!=0)||
            (FileData.sections[i].size_right!=0)
          )
        {
            x=FileData.sections[i].size_left;
            y=FileData.sections[i].size_top;
            w=FileData.sections[i].size_right;
            h=FileData.sections[i].size_bottom;

            //float walls[11][4] = {{0, 0, 25, 245}, {25, 0, 425, 25}, {425, 0, 25, 245}, {25, 220, 400, 25}, {25, 60, 75, 25}, {100, 60, 25, 95}, {50, 120, 25, 25}, {175, 60, 25, 100}, {225, 90, 125, 25}, {275, 190, 25, 30}, {325, 115, 25, 30}};

            isUser=false; noimage=true;
            //Find user image
            for(j=0;j<uBGs.size();j++)
            {
                if(uBGs[j].id==FileData.sections[i].background)
                {
                    isUser=true;
                    noimage=false;
                    img = uBGs[j].image;
                    break;
                }
            } //If not exist, will be used default

            for(j=0;j<configs.main_bg.size();j++)
            {
                if(configs.main_bg[j].id==FileData.sections[i].background)
                {
                    noimage=false;
                    if(!isUser)
                    img = configs.main_bg[j].image; break;
                }
            }
            if((noimage)&&(!isUser))
            {
                img=image;
            }

            brush = QBrush(QColor(255, 255, 255), img);
            //QBrush brush(QColor(255, 255, 255));
            pen = QPen(Qt::NoPen);
            //for (int i = 0; i < 11; i++) {

            item = addRect(QRectF(x, y, (long)fabs(x-w), (long)fabs(y-h)), pen, brush);
            item->setData(0, "BackGround");
            item->setZValue(-30);
        }
    //}
    total++;
    progress.setValue(total);
    }

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
    QGraphicsItem *	box = addPixmap(QPixmap(QApplication::applicationDirPath() + "/" + "data/graphics/level/block/block-8.gif"));
    box->translate(x, y);
    if (itemCollidesWith(box))
        removeItem(box);
    else
        box->setData(0, "Box");
}

void LvlScene::placeBlock(float x, float y, QPixmap &img)
{
    QGraphicsItem *	box = addPixmap(QPixmap(img));
    box->translate(x, y);
    box->setFlag(QGraphicsItem::ItemIsSelectable,true);
    //blocks->addToGroup(box);
}

void LvlScene::setBlocks(LevelData FileData, QProgressDialog &progress)
{
    int i=0;
    QGraphicsItem *	box, * npc;
    QPixmap image = QPixmap(QApplication::applicationDirPath() + "/" + "data/unknown_block.gif");
    QBitmap npcmask = QBitmap(QApplication::applicationDirPath() + "/" + "data/unknown_npcm.gif");
    QPixmap npci = QPixmap(QApplication::applicationDirPath() + "/" + "data/unknown_npc.gif");
    npci.setMask(npcmask);

    for(i=0; i<FileData.blocks.size(); i++)
    {
        //placeBlock(FileData.blocks[i].x, FileData.blocks[i].y, image);
        box = addPixmap(QPixmap(image));
        box->translate(FileData.blocks[i].x, FileData.blocks[i].y);
        if(FileData.blocks[i].npc_id!=0)
        {
            npc = addPixmap(QPixmap(npci));
            npc->translate(FileData.blocks[i].x, FileData.blocks[i].y);
            npc->setZValue(1);
            npc->setOpacity(qreal(0.5));
        }
        box->setFlag(QGraphicsItem::ItemIsSelectable,true);
        if(FileData.blocks[i].invisible)
        box->setOpacity(qreal(0.5));
        box->setZValue(0);
        //blocks->addToGroup(box);

        progress.setValue(progress.value()+1);
    }

}


/////////////////////SET BackGround Objects/////////////////////////////////////////////
void LvlScene::setBGO(LevelData FileData, QProgressDialog &progress, dataconfigs &configs)
{
    int i=0, j;
    bool noimage=true, found=false;
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
        found=false;
        for(j=0;j<configs.main_bgo.size();j++)
        {
            if(configs.main_bgo[j].id==FileData.bgo[i].id)
            {found=true; break;}
        }

        if(found)
        {
            if((QFile::exists(uLVLD) ) &&
                  (QFile::exists(uLVLDs + configs.main_bgo[j].image_n)) )
            {
                if(QFile::exists(uLVLDs + configs.main_bgo[j].mask_n))
                    uBGO.mask = QBitmap(uLVLDs + configs.main_bgo[j].mask_n );
                else
                    uBGO.mask = configs.main_bgo[j].mask;

                uBGO.image = QPixmap(uLVLDs + configs.main_bgo[j].image_n );

                if((uBGO.image.height()!=uBGO.mask.height())||(uBGO.image.width()!=uBGO.mask.width()))
                    uBGO.mask = uBGO.mask.copy(0,0,uBGO.image.width(),uBGO.image.height());
                uBGO.image.setMask(uBGO.mask);
                uBGO.id = FileData.bgo[i].id;
                uBGOs.push_back(uBGO);
            }
            else
            if(QFile::exists(uLVLs + configs.main_bgo[j].image_n) )
            {
                if(QFile::exists(uLVLs + configs.main_bgo[j].mask_n))
                    uBGO.mask = QBitmap(uLVLs + configs.main_bgo[j].mask_n );
                else
                    uBGO.mask = configs.main_bgo[j].mask;

                uBGO.image = QPixmap(uLVLs + configs.main_bgo[j].image_n );

                if((uBGO.image.height()!=uBGO.mask.height())||(uBGO.image.width()!=uBGO.mask.width()))
                    uBGO.mask = uBGO.mask.copy(0,0,uBGO.image.width(),uBGO.image.height());

                uBGO.image.setMask(uBGO.mask);
                uBGO.id = FileData.bgo[i].id;
                uBGOs.push_back(uBGO);
            }
        }

        progress.setValue(progress.value()+1);
    }

    QGraphicsItem *	box;
    bool isUser=false;

    //Applay images to objects
    for(i=0; i<FileData.bgo.size(); i++)
    {
        noimage=true;
        isUser=false;
        for(j=0;j<uBGOs.size();j++)
        {
            if(uBGOs[j].id==FileData.bgo[i].id)
            {
                isUser=true;
                img = uBGOs[j].image;
                break;
            }
        }

        for(j=0;j<configs.main_bgo.size();j++)
        {
            if(configs.main_bgo[j].id==FileData.bgo[i].id)
            {
                noimage=false;
                if(!isUser)
                img = configs.main_bgo[j].image; break;
            }
        }
        if(noimage)
        {
            img=image;
        }

        if((!noimage) && (configs.main_bgo[j].animated))
        {
            img=img.copy(0, 0, img.width(), (int)round(img.height()/configs.main_bgo[j].frames));
        }

        box = addPixmap(QPixmap(img));
        box->translate(FileData.bgo[i].x, FileData.bgo[i].y);

        box->setFlag(QGraphicsItem::ItemIsSelectable, true);
        box->setData(0, "BGO");
        box->setData(1, QString::number(FileData.bgo[i].id) );

        if(configs.main_bgo[j].view)
            box->setZValue(9);
            //bgoback->addToGroup(box);
        else
            box->setZValue(-10);
            //bgofore->addToGroup(box);

        progress.setValue(progress.value()+1);
    }

}




/////////////////////////SET NonPlayble Characters and Items/////////////////////////////////
void LvlScene::setNPC(LevelData FileData, QProgressDialog &progress)
{
    int i=0;
    QGraphicsItem *	box;
    QBitmap mask = QBitmap(QApplication::applicationDirPath() + "/" + "data/unknown_npcm.gif");
    QPixmap image = QPixmap(QApplication::applicationDirPath() + "/" + "data/unknown_npc.gif");
    image.setMask(mask);

    for(i=0; i<FileData.npc.size(); i++)
    {
        box = addPixmap(QPixmap(image));
        box->translate(FileData.npc[i].x, FileData.npc[i].y);
        box->setFlag(QGraphicsItem::ItemIsSelectable,true);
        //npcfore->addToGroup(box);
        if(FileData.npc[i].id==91)
            box->setZValue(5);
        else
            box->setZValue(-9);

        progress.setValue(progress.value()+1);
    }

}

