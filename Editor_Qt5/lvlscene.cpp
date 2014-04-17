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
#include <QPainter>
#include <QMessageBox>
#include <QApplication>
#include <QGraphicsItem>
#include <QProgressDialog>
#include <QtCore>
#include "dataconfigs.h"
#include <QDebug>

LvlScene::LvlScene(QObject *parent) : QGraphicsScene(parent)
{
    //bgoback->setZValue(-10);
    //npcback->setZValue(-9);
    //blocks->setZValue(0);
    //npcfore->setZValue(5);
    //bgofore->setZValue(9);
    //cursor->setZValue(15);
}


//Search and load custom User's files
void LvlScene::loadUserData(LevelData FileData, QProgressDialog &progress, dataconfigs &configs)
{
    int i, total=0;

    UserBGs uBG;
    UserBlocks uBlock;
    UserBGOs uBGO;

    QString uLVLDs = FileData.path + "/" + FileData.filename + "/";
    QString uLVLD = FileData.path + "/" + FileData.filename;
    QString uLVLs = FileData.path + "/";


    //Backgrounds
    for(i=0; i<configs.main_bg.size(); i++) //Add user images
        {
        if(!progress.wasCanceled())
            progress.setLabelText("Search User Backgrounds "+QString::number(i)+"/"+QString::number(configs.main_bg.size()));

            if((QFile::exists(uLVLD) ) &&
                  (QFile::exists(uLVLDs + configs.main_bg[i].image_n)) )
            {
                uBG.image = QPixmap( uLVLDs + configs.main_bg[i].image_n );
                uBG.id = configs.main_bg[i].id;
                uBGs.push_back(uBG);
            }
            else
            if(QFile::exists(uLVLs + configs.main_bg[i].image_n) )
            {
                uBG.image = QPixmap( uLVLs + configs.main_bg[i].image_n );
                uBG.id = configs.main_bg[i].id;
                uBGs.push_back(uBG);
            }
        total++;
        if(!progress.wasCanceled())
            progress.setValue(progress.value()+1);
        }


    //Load user images
    for(i=0; i<configs.main_block.size(); i++) //Add user images
    {

        if(!progress.wasCanceled())
            progress.setLabelText("Search User Blocks "+QString::number(i)+"/"+QString::number(configs.main_block.size()));

            if((QFile::exists(uLVLD) ) &&
                  (QFile::exists(uLVLDs + configs.main_block[i].image_n)) )
            {
                if(QFile::exists(uLVLDs + configs.main_block[i].mask_n))
                    uBlock.mask = QBitmap(uLVLDs + configs.main_block[i].mask_n );
                else
                    uBlock.mask = configs.main_block[i].mask;

                uBlock.image = QPixmap(uLVLDs + configs.main_block[i].image_n );

                if((uBlock.image.height()!=uBlock.mask.height())||(uBlock.image.width()!=uBlock.mask.width()))
                    uBlock.mask = uBlock.mask.copy(0,0,uBlock.image.width(),uBlock.image.height());
                uBlock.image.setMask(uBlock.mask);
                uBlock.id = configs.main_block[i].id;
                uBlocks.push_back(uBlock);
            }
            else
            if(QFile::exists(uLVLs + configs.main_block[i].image_n) )
            {
                if(QFile::exists(uLVLs + configs.main_block[i].mask_n))
                    uBlock.mask = QBitmap(uLVLs + configs.main_block[i].mask_n );
                else
                    uBlock.mask = configs.main_block[i].mask;

                uBlock.image = QPixmap(uLVLs + configs.main_block[i].image_n );

                if((uBlock.image.height()!=uBlock.mask.height())||(uBlock.image.width()!=uBlock.mask.width()))
                    uBlock.mask = uBlock.mask.copy(0,0,uBlock.image.width(),uBlock.image.height());

                uBlock.image.setMask(uBlock.mask);
                uBlock.id = configs.main_block[i].id;
                uBlocks.push_back(uBlock);
            }

    if(!progress.wasCanceled())
        progress.setValue(progress.value()+1);
    }

    //Load BGO
    for(i=0; i<configs.main_bgo.size(); i++) //Add user images
    {
        if(!progress.wasCanceled())
            progress.setLabelText("Search User BGOs "+QString::number(i)+"/"+QString::number(configs.main_bgo.size()));

            if((QFile::exists(uLVLD) ) &&
                  (QFile::exists(uLVLDs + configs.main_bgo[i].image_n)) )
            {
                if(QFile::exists(uLVLDs + configs.main_bgo[i].mask_n))
                    uBGO.mask = QBitmap(uLVLDs + configs.main_bgo[i].mask_n );
                else
                    uBGO.mask = configs.main_bgo[i].mask;

                uBGO.image = QPixmap(uLVLDs + configs.main_bgo[i].image_n );

                if((uBGO.image.height()!=uBGO.mask.height())||(uBGO.image.width()!=uBGO.mask.width()))
                    uBGO.mask = uBGO.mask.copy(0,0,uBGO.image.width(),uBGO.image.height());
                uBGO.image.setMask(uBGO.mask);
                uBGO.id = configs.main_bgo[i].id;
                uBGOs.push_back(uBGO);
            }
            else
            if(QFile::exists(uLVLs + configs.main_bgo[i].image_n) )
            {
                if(QFile::exists(uLVLs + configs.main_bgo[i].mask_n))
                    uBGO.mask = QBitmap(uLVLs + configs.main_bgo[i].mask_n );
                else
                    uBGO.mask = configs.main_bgo[i].mask;

                uBGO.image = QPixmap(uLVLs + configs.main_bgo[i].image_n );

                if((uBGO.image.height()!=uBGO.mask.height())||(uBGO.image.width()!=uBGO.mask.width()))
                    uBGO.mask = uBGO.mask.copy(0,0,uBGO.image.width(),uBGO.image.height());

                uBGO.image.setMask(uBGO.mask);
                uBGO.id = configs.main_bgo[i].id;
                uBGOs.push_back(uBGO);
            }
    if(!progress.wasCanceled())
        progress.setValue(progress.value()+1);
    }

}


void LvlScene::drawSpace(LevelData FileData/*, dataconfigs &configs*/)
{
    foreach(QGraphicsItem * spaceItem, items())
    {
        if(spaceItem->data(0).toString()=="Space")
        {
            removeItem(spaceItem);
            break;
        }
    }
    foreach(QGraphicsItem * spaceItem, items())
    {
        if(spaceItem->data(0).toString()=="SectionBorder")
        {
            removeItem(spaceItem);
            break;
        }
    }

    QPolygon bigSpace;
    QGraphicsItem * item;
    QGraphicsItem * item2;
    QVector<QPoint > drawing;

    int i;//, j;
    long l, r, t, b;
         //x, y, h, w;

    l = FileData.sections[0].size_left;
    r = FileData.sections[0].size_right;
    t = FileData.sections[0].size_top;
    b = FileData.sections[0].size_bottom;

    for(i=0;i<FileData.sections.size(); i++)
    {
        if(FileData.sections[i].size_left < l)
            l = FileData.sections[i].size_left;
        if(FileData.sections[i].size_right > r)
            r = FileData.sections[i].size_right;
        if(FileData.sections[i].size_top < t)
            t = FileData.sections[i].size_left;
        if(FileData.sections[i].size_bottom > b)
            b = FileData.sections[i].size_right;
    }

    drawing.clear();
    drawing.push_back(QPoint(l-1000, t-1000));
    drawing.push_back(QPoint(r+1000, t-1000));
    drawing.push_back(QPoint(r+1000, b+1000));
    drawing.push_back(QPoint(l-1000, b+1000));
    drawing.push_back(QPoint(l-1000, t+1000));

    bigSpace = QPolygon(drawing);

    l = FileData.sections[FileData.CurSection].size_left;
    r = FileData.sections[FileData.CurSection].size_right;
    t = FileData.sections[FileData.CurSection].size_top;
    b = FileData.sections[FileData.CurSection].size_bottom;


    drawing.clear();
    drawing.push_back(QPoint(l, t));
    drawing.push_back(QPoint(r, t));
    drawing.push_back(QPoint(r, b));
    drawing.push_back(QPoint(l, b));
    drawing.push_back(QPoint(l, t));

    bigSpace = bigSpace.subtracted(QPolygon(drawing));

    item = addPolygon(bigSpace, QPen(Qt::NoPen), QBrush(Qt::black));//Add inactive space
    item2 = addPolygon(QPolygon(drawing), QPen(Qt::red, 4));
    item->setZValue(300);
    item2->setZValue(310);
    item->setOpacity(qreal(0.4));
    item->setData(0, "Space");
    item2->setData(0, "SectionBorder");

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
    bool isUser=false, noimage=false;
    long x,y,h,w;

    //Load Backgrounds
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
            item->setZValue(-400);
        }
    //}
    total++;

    if(!progress.wasCanceled())
        progress.setValue(progress.value()+1);
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
    box->setPos(x, y);
    if (itemCollidesWith(box))
        removeItem(box);
    else
        box->setData(0, "Box");
}

void LvlScene::placeBlock(LevelBlock block, dataconfigs &configs)
{
    bool noimage=true;//, found=false;
    bool isUser=false;
    int j;
    QPixmap img;
    QGraphicsItem *	box, * npc;

    QPixmap image = QPixmap(QApplication::applicationDirPath() + "/" + "data/unknown_block.gif");
    QBitmap npcmask = QBitmap(QApplication::applicationDirPath() + "/" + "data/unknown_npcm.gif");
    QPixmap npci = QPixmap(QApplication::applicationDirPath() + "/" + "data/unknown_npc.gif");
    npci.setMask(npcmask);

    noimage=true;
    isUser=false;

    for(j=0;j<uBlocks.size();j++)
    {
        if(uBlocks[j].id==block.id)
        {
            isUser=true;
            img = uBlocks[j].image;
            break;
        }
    }

    for(j=0;j<configs.main_block.size();j++)
    {
        if(configs.main_block[j].id==block.id)
        {
            noimage=false;
            if(!isUser)
            img = configs.main_block[j].image; break;
        }
    }
    if((noimage)||(img.isNull()))
    {
        img=image;
    }

    if((!noimage) && (configs.main_block[j].animated))
    {
        if(configs.main_block[j].algorithm==1)
            img = img.copy(0, ((int)round(img.height()/configs.main_block[j].frames))*4,
                       img.width(), (int)round(img.height()/configs.main_block[j].frames));
        else
            img=img.copy(0, 0, img.width(), (int)round(img.height()/configs.main_block[j].frames));
    }

    if(!configs.main_block[j].sizeble)
        box = addPixmap(QPixmap(img));
    else
    {
        box = addPixmap(QPixmap( drawSizebleBlock(block.w, block.h, img) ));
        box->setZValue(-150);
    }

    box->setPos(block.x, block.y);

    if(block.invisible)
    box->setOpacity(qreal(0.5));

    if(block.npc_id != 0)
    {
        npc = addPixmap(QPixmap(npci));
        npc->setPos(block.x, block.y);
        npc->setZValue(1);
        npc->setOpacity(qreal(0.4));
    }

    box->setFlag(QGraphicsItem::ItemIsSelectable, true);
    box->setData(0, "Block");
    box->setData(1, QString::number(block.id) );
    box->setData(2, QString::number(block.array_id) );

    if(configs.main_block[j].sizeble)
        box->setZValue(-150);
    else
    {
    if(configs.main_block[j].view==1)
        box->setZValue(10);
    else
        box->setZValue(1);
    }

}

//Sizeble Block formula
QPixmap LvlScene::drawSizebleBlock(int w, int h, QPixmap srcimg)
{
    int x,y, i, j;
    int hc, wc;
    QPixmap img;
    QPixmap * sizebleImage;
    QPainter * szblock;
    x=32;
    y=32;

    sizebleImage = new QPixmap(QSize(w, h));
    sizebleImage->fill(Qt::transparent);
    szblock = new QPainter(sizebleImage);

    //L
    hc=0;
    for(i=0; i<((h-2*y) / y); i++ )
    {
        szblock->drawPixmap(0, x+hc, x, y, srcimg.copy(QRect(0, y, x, y)));
            hc+=x;
    }

    //T
    hc=0;
    for(i=0; i<( (w-2*x) / x); i++ )
    {
        szblock->drawPixmap(x+hc, 0, x, y, srcimg.copy(QRect(x, 0, x, y)) );
            hc+=x;
    }

    //B
    hc=0;
    for(i=0; i< ( (w-2*x) / x); i++ )
    {
        szblock->drawPixmap(x+hc, h-y, x, y, srcimg.copy(QRect(x, srcimg.width()-y, x, y )) );
            hc+=x;
    }

    //R
    hc=0;
    for(i=0; i<((h-2*y) / y); i++ )
    {
        szblock->drawPixmap(w-x, y+hc, x, y, srcimg.copy(QRect(srcimg.width()-x, y, x, y)));
            hc+=x;
    }

    //C
    hc=0;
    wc=0;
    for(i=0; i<((h-2*y) / y); i++ )
    {
        hc=0;
        for(j=0; j<((w-2*x) / x); j++ )
        {
        szblock->drawPixmap(x+hc, y+wc, x, y, srcimg.copy(QRect(x, y, x, y)));
            hc+=x;
        }
        wc+=y;
    }

    //Applay Sizeble formula
     //1
    szblock->drawPixmap(0,0,y,x, srcimg.copy(QRect(0,0,y,x)));
     //2
    szblock->drawPixmap(w-y, 0, y, x, srcimg.copy(QRect(srcimg.width()-y, 0, y, x)) );
     //3
    szblock->drawPixmap(w-y, h-x, y, x, srcimg.copy(QRect(srcimg.width()-y, srcimg.height()-x, y, x)) );
     //4
    szblock->drawPixmap(0, h-x, y, x, srcimg.copy(QRect(0, srcimg.height()-x, y, x)) );

    img = QPixmap( * sizebleImage);
    return img;
}

void LvlScene::sortBlockArray(QVector<LevelBlock > &blocks)
{
    LevelBlock tmp1;
    int total = blocks.size();
    long i;
    long ymin;
    long ymini;
    long sorted = 0;


        while(sorted < blocks.size())
        {
            ymin = blocks[sorted].y;
            ymini = sorted;

            for(i = sorted; i < total; i++)
            {
                if( blocks[i].y < ymin )
                {
                    ymin = blocks[i].y; ymini = i;
                }
            }
            tmp1 = blocks[ymini];
            blocks[ymini] = blocks[sorted];
            blocks[sorted] = tmp1;
            sorted++;
        }
}

/////////////////////SET Block Objects/////////////////////////////////////////////
void LvlScene::setBlocks(LevelData FileData, QProgressDialog &progress, dataconfigs &configs)
{
    int i=0;

    //Sort block by Y
    sortBlockArray(FileData.blocks);

    //Applay images to objects
    for(i=0; i<FileData.blocks.size(); i++)
    {
        if(!progress.wasCanceled())
            progress.setLabelText("Applayng Blocks "+QString::number(i)+"/"+QString::number(FileData.blocks.size()));

        //Add block to scene
        placeBlock(FileData.blocks[i], configs);

        if(!progress.wasCanceled())
            progress.setValue(progress.value()+1);
    }
}


/////////////////////SET BackGround Objects/////////////////////////////////////////////
void LvlScene::setBGO(LevelData FileData, QProgressDialog &progress, dataconfigs &configs)
{
    int i=0, j;
    bool noimage=true;//, found=false;
    QPixmap img;
    QPixmap image = QPixmap(QApplication::applicationDirPath() + "/" + "data/unknown_bgo.gif");
    //QBitmap mask;
    //QBitmap mask = QBitmap(QApplication::applicationDirPath() + "/" + "data/unknown_npcm.gif");
    //QPixmap image = QPixmap(QApplication::applicationDirPath() + "/" + "data/unknown_npc.gif");

    QGraphicsItem *	box;
    bool isUser=false;

    //Applay images to objects
    for(i=0; i<FileData.bgo.size(); i++)
    {
        if(!progress.wasCanceled())
            progress.setLabelText("Applayng BGOs "+QString::number(i)+"/"+QString::number(FileData.bgo.size()));

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
        if((noimage)||(img.isNull()))
        {
            img=image;
        }

        if((!noimage) && (configs.main_bgo[j].animated))
        {
            img=img.copy(0, 0, img.width(), (int)round(img.height()/configs.main_bgo[j].frames));
        }

        box = addPixmap(QPixmap(img));
        box->setPos(FileData.bgo[i].x, FileData.bgo[i].y);

        box->setFlag(QGraphicsItem::ItemIsSelectable, true);
        box->setData(0, "BGO");
        box->setData(1, QString::number(FileData.bgo[i].id) );
        box->setData(2, QString::number(FileData.bgo[i].array_id) );

        if(configs.main_bgo[j].view!=0)
            box->setZValue(9);
            //bgoback->addToGroup(box);
        else
            box->setZValue(-10);
            //bgofore->addToGroup(box);

        if(!progress.wasCanceled())
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
        box->setPos(FileData.npc[i].x, FileData.npc[i].y);
        box->setFlag(QGraphicsItem::ItemIsSelectable,true);
        //npcfore->addToGroup(box);
        if(FileData.npc[i].id==91)
            box->setZValue(5);
        else
            box->setZValue(-9);

        box->setData(0, "NPC"); // ObjType
        box->setData(1, QString::number(FileData.npc[i].id) );
        box->setData(2, QString::number(FileData.npc[i].array_id) );

        if(!progress.wasCanceled())
            progress.setValue(progress.value()+1);
    }

}

/////////////////////////SET Waters/////////////////////////////////
void LvlScene::setWaters(LevelData FileData, QProgressDialog &progress)
{
    int i=0;
    long x, y, h, w;
    QGraphicsItem *	box;

    for(i=0; i<FileData.water.size(); i++)
    {
        if(!progress.wasCanceled())
            progress.setLabelText("Applayng water "+QString::number(i)+"/"+QString::number(FileData.water.size()));

        x = FileData.water[i].x;
        y = FileData.water[i].y;
        h = FileData.water[i].h;
        w = FileData.water[i].w;

        //box = addRect(x, y, w, h, QPen(((FileData.water[i].quicksand)?Qt::yellow:Qt::green), 4), Qt::NoBrush);
        QVector<QPoint > points;
        // {{x, y},{x+w, y},{x+w,y+h},{x, y+h}}
        points.push_back(QPoint(x, y));
        points.push_back(QPoint(x+w, y));
        points.push_back(QPoint(x+w,y+h));
        points.push_back(QPoint(x, y+h));
        points.push_back(QPoint(x, y));

        points.push_back(QPoint(x, y+h));
        points.push_back(QPoint(x+w,y+h));
        points.push_back(QPoint(x+w, y));
        points.push_back(QPoint(x, y));

        box = addPolygon(QPolygon(points), QPen(((FileData.water[i].quicksand)?Qt::yellow:Qt::green), 4));

        box->setFlag(QGraphicsItem::ItemIsSelectable,true);

        box->setZValue(10);

        box->setData(0, "Water"); // ObjType
        box->setData(1, QString::number(0) );
        box->setData(2, QString::number(FileData.water[i].array_id) );

        if(!progress.wasCanceled())
            progress.setValue(progress.value()+1);
    }

}

/////////////////////////SET Doors/////////////////////////////////
void LvlScene::setDoors(LevelData FileData, QProgressDialog &progress)
{
    int i=0;
    long ix, iy, ox, oy, h, w;
    QGraphicsItem *	enter;
    QGraphicsItem *	exit;

    for(i=0; i<FileData.doors.size(); i++)
    {
        if(!progress.wasCanceled())
            progress.setLabelText("Applayng doors "+QString::number(i)+"/"+QString::number(FileData.doors.size()));

        ix = FileData.doors[i].ix;
        iy = FileData.doors[i].iy;
        ox = FileData.doors[i].ox;
        oy = FileData.doors[i].oy;
        h = 32;
        w = 32;

        enter = addRect(ix, iy, w, h, QPen(Qt::magenta, 4), Qt::NoBrush);
        exit = addRect(ox, oy, w, h, QPen(Qt::magenta, 4), Qt::NoBrush);

        enter->setFlag(QGraphicsItem::ItemIsSelectable,true);
        exit->setFlag(QGraphicsItem::ItemIsSelectable,true);

        enter->setZValue(15);
        exit->setZValue(15);

        enter->setData(0, "Door_enter"); // ObjType
        enter->setData(1, QString::number(0) );
        enter->setData(2, QString::number(FileData.doors[i].array_id) );

        exit->setData(0, "Door_exit"); // ObjType
        exit->setData(1, QString::number(0) );
        exit->setData(2, QString::number(FileData.doors[i].array_id) );

        if(!progress.wasCanceled())
            progress.setValue(progress.value()+1);
    }

}

