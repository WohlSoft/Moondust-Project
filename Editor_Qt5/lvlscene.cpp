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
            item->setZValue(-30);
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
        img=img.copy(0, 0, img.width(), (int)round(img.height()/configs.main_block[j].frames));
    }

    box = addPixmap(QPixmap(img));
    box->setPos(block.x, block.y);

    if(block.invisible)
    box->setOpacity(qreal(0.5));

    if(block.npc_id!=0)
    {
        npc = addPixmap(QPixmap(npci));
        npc->setPos(block.x, block.y);
        npc->setZValue(1);
        npc->setOpacity(qreal(0.3));
    }

    box->setFlag(QGraphicsItem::ItemIsSelectable, true);
    box->setData(0, "Block");
    box->setData(1, QString::number(block.id) );
    box->setData(2, QString::number(block.array_id) );

    if(configs.main_block[j].sizeble)
        box->setZValue(-12);
    else
    if(configs.main_block[j].view==1)
        box->setZValue(1);
    else
        box->setZValue(0);

}

/////////////////////SET Block Objects/////////////////////////////////////////////
void LvlScene::setBlocks(LevelData FileData, QProgressDialog &progress, dataconfigs &configs)
{
    int i=0;

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

        box->setData(0, "BGO"); // ObjType

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

        box = addRect(x, y, w, h, QPen(((FileData.water[i].quicksand)?Qt::yellow:Qt::green), 4), Qt::NoBrush);

        //box->setPos(FileData.water[i].x, FileData.water[i].y);
        box->setFlag(QGraphicsItem::ItemIsSelectable,true);
        box->setZValue(10);
        box->setData(0, "Water"); // ObjType
        box->setData(1, QString::number(0) );
        box->setData(2, QString::number(FileData.water[i].array_id) );

        if(!progress.wasCanceled())
            progress.setValue(progress.value()+1);
    }

}

