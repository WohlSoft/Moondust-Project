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
#include "../leveledit.h"

#include "item_block.h"
#include "item_bgo.h"

LvlScene::LvlScene(dataconfigs &configs, LevelData &FileData, QObject *parent) : QGraphicsScene(parent)
{
    setItemIndexMethod(NoIndex);

    //Set the background GraphicsItem's points
    for(int i=0;i<22;i++)
        BgItem.push_back(new QGraphicsPixmapItem);

    //Pointerss
    pConfigs = &configs; // Pointer to Main Configs
    LvlData = &FileData; //Ad pointer to level data


    //Options
    opts.animationEnabled = true;
    opts.collisionsEnabled = true;
    grid = true;

    //Indexes
    index_blocks = pConfigs->index_blocks; //Applaying blocks indexes
    index_bgo = pConfigs->index_bgo;
    index_npc = pConfigs->index_npc;

    //Editing mode
    EditingMode = 0;
    EraserEnabled = false;
    PasteFromBuffer = false;
    disableMoveItems = false;

    //Editing process flags
    IsMoved = false;
    haveSelected = false;

    contextMenuOpened = false;

    //Events flags
    wasPasted = false;  //call to cursor reset to normal select
    doCopy = false;     //call to copy
    doCut = false;      //call to cut


    QPixmap cur(QSize(1,1));
    cur.fill(Qt::black);
    cursor = addPixmap(QPixmap(cur));
    cursor->setZValue(1000);
    cursor->hide();

    //set dummy images if target not exist or wrong
    uBlockImg = QPixmap(QApplication::applicationDirPath() + "/" + "data/unknown_block.gif");
    npcmask = QBitmap(QApplication::applicationDirPath() + "/" + "data/unknown_npcm.gif");
    uNpcImg = QPixmap(QApplication::applicationDirPath() + "/" + "data/unknown_npc.gif");
    uNpcImg.setMask(npcmask);
    uBgoImg = QPixmap(QApplication::applicationDirPath() + "/" + "data/unknown_bgo.gif");


    //set Default Z Indexes
    bgZ = -1000;
    blockZs = -150; // sizable blocks
    bgoZb = -100; // backround BGO
    npcZb = -50; // standart NPC

    blockZ = 1; // standart block
    playerZ = 5; //player Point

    bgoZf = 50; // foreground BGO

    blockZl = 100;
    npcZf = 150; // foreground NPC
    waterZ = 500;
    doorZ = 700;
    spaceZ1 = 1000; // interSection space layer
    spaceZ2 = 1020; // section Border

    //HistoryIndex
    historyIndex=0;

    historyChanged = false;

    //Locks
    lock_bgo=false;
    lock_block=false;
    lock_npc=false;
    lock_door=false;
    lock_water=false;

}

LvlScene::~LvlScene()
{
    uBGs.clear();
    uBGOs.clear();
    uBlocks.clear();

    foreach(QGraphicsPixmapItem * it, BgItem)
        free(it);
}



QGraphicsItem * LvlScene::itemCollidesWith(QGraphicsItem * item)
{
    qlonglong leftA, leftB;
    qlonglong rightA, rightB;
    qlonglong topA, topB;
    qlonglong bottomA, bottomB;
    //qreal betweenZ;

    QList<QGraphicsItem *> collisions = collidingItems(item, Qt::IntersectsItemBoundingRect);
    foreach (QGraphicsItem * it, collisions) {
            if (it == item)
                 continue;
            if(item->data(0).toString()=="Water")
                return NULL;
            if(item->data(0).toString()=="Door_exit")
                return NULL;
            if(item->data(0).toString()=="Door_enter")
                return NULL;

            leftA = item->scenePos().x();
            rightA = item->scenePos().x()+item->data(9).toLongLong();
            topA = item->scenePos().y();
            bottomA = item->scenePos().y()+item->data(10).toLongLong();

            leftB = it->scenePos().x();
            rightB = it->scenePos().x()+it->data(9).toLongLong();
            topB = it->scenePos().y();
            bottomB = it->scenePos().y()+it->data(10).toLongLong();

            if(it->data(0).toString()=="Block")
                WriteToLog(QtDebugMsg, QString(" >>Collision with block detected"));

              if((item->data(0).toString()=="Block")||(item->data(0).toString()=="NPC")
                      ||(item->data(0).toString()=="BGO"))
              {
                  if(item->data(0).toString()!=it->data(0).toString()) continue;

                  if(item->data(3).toString()=="sizable")
                  {//sizable Block
                      WriteToLog(QtDebugMsg, QString("sizable block") );
                      continue;
                  }

                  if(item->data(0).toString()=="BGO")
                    if(item->data(1).toInt()!=it->data(1).toInt()) continue;

                     if( bottomA <= topB )
                     { continue; }
                     if( topA >= bottomB )
                     { continue; }
                     if( rightA <= leftB )
                     { continue; }
                     if( leftA >= rightB )
                     { continue; }

                     if(it->data(3).toString()!="sizable")
                        return it;
              }

    }
    return NULL;
}

QGraphicsItem * LvlScene::itemCollidesCursor(QGraphicsItem * item)
{
    QList<QGraphicsItem *> collisions = collidingItems(item);
    foreach (QGraphicsItem * it, collisions) {
            if (it == item)
                 continue;
            if( (
                    (it->data(0).toString()=="Block")||
                    (it->data(0).toString()=="BGO")||
                    (it->data(0).toString()=="NPC")||
                    (it->data(0).toString()=="door_exit")||
                    (it->data(0).toString()=="door_enter")||
                    (it->data(0).toString()=="water")
              )&&(it->isVisible() ) )
                return it;
    }
    return NULL;
}

/*
QGraphicsItem * LvlScene::itemCollidesMouse(QGraphicsItem * item)
{
    QList<QGraphicsItem *> collisions = collidingItems(item, Qt::ContainsItemShape);
    foreach (QGraphicsItem * it, collisions) {
            if (it == item)
                 continue;
            if(item->data(0).toString()=="Water")
                return NULL;
            if(item->data(0).toString()=="Door_exit")
                return NULL;
            if(item->data(0).toString()=="Door_enter")
                return NULL;

        if( item->data(0).toString() ==  it->data(0).toString() )
            return it;
    }
    return NULL;
}
*/



void LvlScene::drawSpace(LevelData FileData/*, dataconfigs &configs*/)
{
    WriteToLog(QtDebugMsg, QString("Draw intersection space-> Find and remove current"));
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

    int i, j;
    long l, r, t, b;
         //x, y, h, w;

    WriteToLog(QtDebugMsg, QString("Find minimal"));
    j=-1;
    do
    {
        j++;
        l = FileData.sections[j].size_left;
        r = FileData.sections[j].size_right;
        t = FileData.sections[j].size_top;
        b = FileData.sections[j].size_bottom;
    }
    while(
          ((FileData.sections[j].size_left==0) &&
          (FileData.sections[j].size_right==0) &&
          (FileData.sections[j].size_top==0) &&
          (FileData.sections[j].size_bottom==0)) && (j<FileData.sections.size())
    );

    for(i=0;i<FileData.sections.size(); i++)
    {

        if(
                (FileData.sections[i].size_left==0) &&
                (FileData.sections[i].size_right==0) &&
                (FileData.sections[i].size_top==0) &&
                (FileData.sections[i].size_bottom==0))
            continue;

        if(FileData.sections[i].size_left < l)
            l = FileData.sections[i].size_left;
        if(FileData.sections[i].size_right > r)
            r = FileData.sections[i].size_right;
        if(FileData.sections[i].size_top < t)
            t = FileData.sections[i].size_top;
        if(FileData.sections[i].size_bottom > b)
            b = FileData.sections[i].size_bottom;
    }

    WriteToLog(QtDebugMsg, QString("Draw polygon"));

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


    WriteToLog(QtDebugMsg, QString("Draw editing hole"));
    drawing.clear();
    drawing.push_back(QPoint(l-1, t-1));
    drawing.push_back(QPoint(r+1, t-1));
    drawing.push_back(QPoint(r+1, b+1));
    drawing.push_back(QPoint(l-1, b+1));
    drawing.push_back(QPoint(l-1, t-1));

    bigSpace = bigSpace.subtracted(QPolygon(drawing));

    WriteToLog(QtDebugMsg, QString("add polygon to Item"));
    item = addPolygon(bigSpace, QPen(Qt::NoPen), QBrush(Qt::black));//Add inactive space
    item2 = addPolygon(QPolygon(drawing), QPen(Qt::red, 2));
    item->setZValue(spaceZ1);
    item2->setZValue(spaceZ2);
    item->setOpacity(qreal(0.4));
    item->setData(0, "Space");
    item2->setData(0, "SectionBorder");

}

///////////////////////////////BACKGROUND IMAGE/////////////////////////////////////////
void LvlScene::makeSectionBG(LevelData FileData, QProgressDialog &progress)
//void LvlScene::makeSectionBG(int x, int y, int w, int h)
{
    int i, total=0;
    WriteToLog(QtDebugMsg, QString("Applay Backgrounds"));

    //Load Backgrounds
    for(i=0; i<FileData.sections.size(); i++)
    {
        setSectionBG(FileData.sections[i]);

        total++;

        if(!progress.wasCanceled())
            progress.setValue(progress.value()+1);
        else return;
    }

}

void LvlScene::setSectionBG(LevelSection section)
{
    QGraphicsItem * item;
    QBrush brush;
    QPen pen;
    QPixmap image = QPixmap(QApplication::applicationDirPath() + "/" + "data/nobg.gif");
    QPixmap img;
    QPixmap img2; //Second image buffer
    //need a BGitem

    bool isUser1=false, isUser2=false, noimage=false;
    long x,y,h,w, j;

    if(
        (section.size_left!=0) ||
        (section.size_top!=0)||
        (section.size_bottom!=0)||
        (section.size_right!=0)
      )
    { //Don't draw on reserved section entry
        x=section.size_left;
        y=section.size_top;
        w=section.size_right;
        h=section.size_bottom;

        WriteToLog(QtDebugMsg, "Check for user images");

        isUser1=false; // user's images are exist
        isUser2=false; // user's images are exist
        noimage=true;
        j = 0;
        if(section.background != 0 )
        {
            //Find user image
            for(j=0;j<uBGs.size();j++)
            {
                if(uBGs[j].id==section.background)
                {
                    noimage=false;
                    if((uBGs[j].q==0)||(uBGs[j].q==2)) //set first image
                        {img = uBGs[j].image; isUser1=true;}
                    if((uBGs[j].q>=1)){ // set Second image
                        img2 = uBGs[j].second_image; isUser2=true;}
                    break;
                }
            } //If not exist, will be used default

            for(j=0;j<pConfigs->main_bg.size();j++)
            {
                if(pConfigs->main_bg[j].id==section.background)
                {
                    noimage=false;
                      if(!isUser1)
                          img = pConfigs->main_bg[j].image;
                      if(!isUser2)
                          img2 = pConfigs->main_bg[j].second_image;
                    break;
                }
            }
            if((noimage)&&(!isUser1))
            {
                WriteToLog(QtWarningMsg, "Image not found");
                img=image;
            }
        }
        else noimage=true;

        //pConfigs->main_bg[j].type;

        brush = QBrush(QColor(0, 0, 0));
        //QBrush brush(QColor(255, 255, 255));
        pen = QPen(Qt::NoPen);
        //for (int i = 0; i < 11; i++) {

        //item = addRect(QRectF(x, y, , ), pen, brush);

        if((!noimage)&&(!img.isNull()))
        {
            item = addPixmap(image);
            DrawBG(x, y, w, h, img, img2, pConfigs->main_bg[j], BgItem[section.id]);
            BgItem[section.id]->setParentItem(item);
            item->setPos(x, y);
        }
        else
            item = addRect(x, y, (long)fabs(x-w), (long)fabs(y-h), pen, brush);

        WriteToLog(QtDebugMsg, QString("Item placed to x=%1 y=%2 h=%3 w=%4").arg(x).arg(y)
                   .arg((long)fabs(x-w)).arg((long)fabs(y-h)));
        //

        item->setData(0, "BackGround"+QString::number(section.id) );
        item->setZValue(bgZ);
    } //Don't draw on reserved section entry

}

void LvlScene::DrawBG(int x, int y, int w, int h, QPixmap srcimg, QPixmap srcimg2, obj_BG &bgsetup, QGraphicsPixmapItem * &BgItem)
{
    QPixmap BackImg;
    QPainter * BGPaint;
    QPixmap img;
    //int i, j;
    int si_attach, attach;
    long toX, toY, bgH, bgW;

    WriteToLog(QtDebugMsg, "Draw BG Image");
    BackImg = QPixmap(QSize( (long)fabs(x-w), (long)fabs(y-h) ));
    attach = bgsetup.attached;
    long px = 0;

    if((bgsetup.type==0)&&(!bgsetup.editing_tiled))
    {   //SingleRow BG

        WriteToLog(QtDebugMsg, "SingleRow BG");
        if(attach==0)
            BackImg.fill( srcimg.toImage().pixel(0,0) );
        else
            BackImg.fill( srcimg.toImage().pixel(0,(srcimg.height()-1)) );
        BGPaint = new QPainter(&BackImg);

        px=0;
        if(attach==0)
            toY = (long)fabs(y-h)-srcimg.height();
        else
            toY = 0;
        bgW = srcimg.width();
        bgH = srcimg.height();

        do
        { //Draw row
            BGPaint->drawPixmap(px, toY, bgW, bgH, srcimg);
            px += srcimg.width();
        }
        while( px < (long)fabs(x-w) );

    }
    else if((bgsetup.type==1)&&(!bgsetup.editing_tiled))
    {   //Double BG
        WriteToLog(QtDebugMsg, "DoubleRow BG");

        si_attach = bgsetup.second_attached; // Second image attach

        //Fill empty space
        if((!srcimg2.isNull()) && (si_attach==0))
            BackImg.fill( srcimg2.toImage().pixel(0,0) );
        else
            BackImg.fill( srcimg.toImage().pixel(0,0) );

        BGPaint = new QPainter(&BackImg);

        px=0;

        toY = (long)fabs(y-h)-srcimg.height();
        bgW = srcimg.width();
        bgH = srcimg.height();

        WriteToLog(QtDebugMsg, QString("Draw first row, params: "));

        //Draw first row
            do{
                BGPaint->drawPixmap(px, toY, bgW, bgH, srcimg);
                px += srcimg.width();
            } while( px < (long)fabs(x-w) );

        WriteToLog(QtDebugMsg, "Draw second row");
        px=0;

        if(si_attach==0) // over first
            toY = (long)fabs(y-h)-srcimg.height()-srcimg2.height();
        else
        if(si_attach==1) // bottom
            toY = (long)fabs(y-h)-srcimg2.height();

        bgW = srcimg2.width();
        bgH = srcimg2.height();

        if(!srcimg2.isNull())
        {
            //Draw seconf row if it no null
            do {
                BGPaint->drawPixmap(px, toY, bgW, bgH, srcimg2);
                px += srcimg2.width();
            } while( px < (long)fabs(x-w) );
        } else WriteToLog(QtDebugMsg, "second image is Null");
    }
    else
    { // Black
        WriteToLog(QtDebugMsg, "Tiled");
        BackImg.fill( Qt::black );
        BGPaint = new QPainter(&BackImg);

        px=0;
        if(attach==0)
            toY = (long)fabs(y-h)-srcimg.height();
        else
            toY = 0;

        bgW = srcimg.width();
        bgH = srcimg.height();

        toX = 0;

        do{
            px=0;
            do
            { //Draw row
                BGPaint->drawPixmap(px, toY + toX*((attach==0)?(-1):1), bgW, bgH, srcimg);
                px += srcimg.width();
            }
            while( px < (long)fabs(x-w) );
            toX+=srcimg.height();
        }   while( toX < (long)fabs(y-h) + srcimg.height() );


    }
    BGPaint->end();

    WriteToLog(QtDebugMsg, "Drawed");

    if(!BackImg.isNull())
        img = BackImg.copy(BackImg.rect());
    else
    {
        WriteToLog(QtDebugMsg, "Drawed PixMap is null");
        img.fill( Qt::red );
    }
    WriteToLog(QtDebugMsg, "Added to QPixmap");

    BgItem->setPixmap(QPixmap(img));
    //return img;
}


void LvlScene::ChangeSectionBG(int BG_Id, LevelData &FileData)
{


    foreach (QGraphicsItem * findBG, items() )
    {
        if(findBG->data(0)=="BackGround"+QString::number(FileData.CurSection) )
        {
            WriteToLog(QtDebugMsg, QString("Remove item BackGround"+QString::number(FileData.CurSection)) );
            removeItem(findBG); break;
        }
    }

    if((BG_Id>=0) && (BG_Id <= pConfigs->main_bg.size() )) // Deny unexist ID
            FileData.sections[FileData.CurSection].background = BG_Id;

    WriteToLog(QtDebugMsg, "set Background to "+QString::number(BG_Id));
    setSectionBG(FileData.sections[FileData.CurSection]);
}



//////////////////Block////////////////////////////////////////////////////////////////////////////////////////

/* //The Trach
void LvlScene::placeBox(float x, float y)
{
    QGraphicsItem *	box = addPixmap(QPixmap(QApplication::applicationDirPath() + "/" + "data/graphics/level/block/block-8.gif"));
    box->setPos(x, y);
    if (itemCollidesWith(box))
        removeItem(box);
    else
        box->setData(0, "Box");
}
*/

// ////////////////////Sort///////////////////////////

void LvlScene::sortBlockArray(QVector<LevelBlock > &blocks)
{
    LevelBlock tmp1;
    int total = blocks.size();
    long i;
    unsigned long ymin;
    long ymini;
    long sorted = 0;


        while(sorted < blocks.size())
        {
            ymin = blocks[sorted].array_id;
            ymini = sorted;

            for(i = sorted; i < total; i++)
            {
                if( blocks[i].array_id < ymin )
                {
                    ymin = blocks[i].array_id; ymini = i;
                }
            }
            tmp1 = blocks[ymini];
            blocks[ymini] = blocks[sorted];
            blocks[sorted] = tmp1;
            sorted++;
        }
}

void LvlScene::sortBGOArray(QVector<LevelBGO > &bgos)
{
    LevelBGO tmp1;
    int total = bgos.size();
    long i;
    unsigned long ymin;
    unsigned long ymini;
    long sorted = 0;

        while(sorted < bgos.size())
        {
            ymin = bgos[sorted].array_id;
            ymini = sorted;

            for(i = sorted; i < total; i++)
            {
                if( bgos[i].array_id < ymin )
                {
                    ymin = bgos[i].array_id; ymini = i;
                }
            }
            tmp1 = bgos[ymini];
            bgos[ymini] = bgos[sorted];
            bgos[sorted] = tmp1;
            sorted++;
        }
}

// ///////////////////SET Block Objects/////////////////////////////////////////////
void LvlScene::setBlocks(LevelData FileData, QProgressDialog &progress)
{
    int i=0;

    //Applay images to objects
    for(i=0; i<FileData.blocks.size(); i++)
    {
        //Add block to scene
        placeBlock(FileData.blocks[i]);

        if(!progress.wasCanceled())
            progress.setValue(progress.value()+1);
        else return;
    }
}


// ///////////////////SET BackGround Objects/////////////////////////////////////////////
void LvlScene::setBGO(LevelData FileData, QProgressDialog &progress)
{
    int i=0;

    //sortBGOArray(FileData.bgo); //Sort BGOs

    //Applay images to objects
    for(i=0; i<FileData.bgo.size(); i++)
    {
        //add BGO to scene
        placeBGO(FileData.bgo[i]);

        if(!progress.wasCanceled())
            progress.setValue(progress.value()+1);
        else return;
    }

}


// ///////////////////////SET NonPlayble Characters and Items/////////////////////////////////
void LvlScene::setNPC(LevelData FileData, QProgressDialog &progress)
{
    int i=0;
    //QGraphicsItem *	box;

    for(i=0; i<FileData.npc.size(); i++)
    {
        //add NPC to scene
        placeNPC(FileData.npc[i]);

        if(!progress.wasCanceled())
            progress.setValue(progress.value()+1);
        else return;
    }

}

// ///////////////////////SET Waters/////////////////////////////////
void LvlScene::setWaters(LevelData FileData, QProgressDialog &progress)
{
    int i=0;

    for(i=0; i<FileData.water.size(); i++)
    {
        //add Water to scene
        placeWater(FileData.water[i]);

        if(!progress.wasCanceled())
            progress.setValue(progress.value()+1);
        else return;
    }

}



// ///////////////////////SET Doors/////////////////////////////////
void LvlScene::setDoors(LevelData FileData, QProgressDialog &progress)
{
    int i=0;


    for(i=0; i<FileData.doors.size(); i++)
    {

        //add Doors points to scene
        placeDoor(FileData.doors[i]);

        if(!progress.wasCanceled())
            progress.setValue(progress.value()+1);
        else
            return;
    }

}

// ////////////////////////SET Player Points/////////////////////////////////
void LvlScene::setPlayerPoints()
{
    int i=0;

    QGraphicsItem *	player;
    PlayerPoint plr;

    for(i=0; i<LvlData->players.size(); i++)
    {
        plr = LvlData->players[i];
        if((plr.h!=0)||(plr.w!=0)||(plr.x!=0)||(plr.y!=0))
        {
            player = addPixmap(QPixmap(":/player"+QString::number(i+1)+".png"));
            player->setPos(plr.x, plr.y);
            player->setZValue(playerZ);
            player->setData(0, "player"+QString::number(i+1) );
            player->setData(2, QString::number(plr.id));
            player->setFlag(QGraphicsItem::ItemIsSelectable, true);
            player->setFlag(QGraphicsItem::ItemIsMovable, true);
        }

    }

}



////////////////////////////////////Animator////////////////////////////////
void LvlScene::startBlockAnimation()
{
    QList<QGraphicsItem*> ItemList = items();
    QGraphicsItem *tmp;
    for (QList<QGraphicsItem*>::iterator it = ItemList.begin(); it != ItemList.end(); it++)
    {
        if(((*it)->data(0)=="Block")&&((*it)->data(4)=="animated"))
        {
            tmp = (*it);
            ((ItemBlock *)tmp)->AnimationStart();
        }
        else
        if(((*it)->data(0)=="BGO")&&((*it)->data(4)=="animated"))
        {
            tmp = (*it);
            ((ItemBGO *)tmp)->AnimationStart();
        }
    }

}

void LvlScene::stopAnimation()
{
    QList<QGraphicsItem*> ItemList = items();
    QGraphicsItem *tmp;
    for (QList<QGraphicsItem*>::iterator it = ItemList.begin(); it != ItemList.end(); it++)
    {
        if(((*it)->data(0)=="Block")&&((*it)->data(4)=="animated"))
        {
            tmp = (*it);
            ((ItemBlock *)tmp)->AnimationStop();
        }
        else
        if(((*it)->data(0)=="BGO")&&((*it)->data(4)=="animated"))
        {
            tmp = (*it);
            ((ItemBGO *)tmp)->AnimationStop();
        }
    }

}

void LvlScene::applyLayersVisible()
{
    QList<QGraphicsItem*> ItemList = items();
    QGraphicsItem *tmp;
    for (QList<QGraphicsItem*>::iterator it = ItemList.begin(); it != ItemList.end(); it++)
    {
        if((*it)->data(0)=="Block")
        {
            tmp = (*it);
            foreach(LevelLayers layer, LvlData->layers)
            {
                if( ((ItemBlock *)tmp)->blockData.layer == layer.name)
                {
                    ((ItemBlock *)tmp)->setVisible( !layer.hidden ); break;
                }
            }
        }
        else
        if(((*it)->data(0)=="BGO")&&((*it)->data(4)=="animated"))
        {
            tmp = (*it);
            foreach(LevelLayers layer, LvlData->layers)
            {
                if( ((ItemBGO *)tmp)->bgoData.layer == layer.name)
                {
                    ((ItemBGO *)tmp)->setVisible( !layer.hidden ); break;
                }
            }
        }
    }
}


/////////////////////////////////////////////Locks////////////////////////////////
void LvlScene::setLocked(int type, bool lock)
{
    QList<QGraphicsItem*> ItemList = items();
    // setLock
    switch(type)
    {
    case 1://Block
        lock_block = lock;
        break;
    case 2://BGO
        lock_bgo = lock;
        break;
    case 3://NPC
        lock_npc = lock;
        break;
    case 4://Water
        lock_water = lock;
        break;
    case 5://Doors
        lock_door = lock;
        break;
    default: break;
    }

    for (QList<QGraphicsItem*>::iterator it = ItemList.begin(); it != ItemList.end(); it++)
    {
        switch(type)
        {
        case 1://Block
            if((*it)->data(0).toString()=="Block")
            {
                (*it)->setFlag(QGraphicsItem::ItemIsSelectable, (!lock));
                (*it)->setFlag(QGraphicsItem::ItemIsMovable, (!lock));
            }
            break;
        case 2://BGO
            if((*it)->data(0).toString()=="BGO")
            {
                (*it)->setFlag(QGraphicsItem::ItemIsSelectable, (!lock));
                (*it)->setFlag(QGraphicsItem::ItemIsMovable, (!lock));
            }
            break;
        case 3://NPC
            if((*it)->data(0).toString()=="NPC")
            {
                (*it)->setFlag(QGraphicsItem::ItemIsSelectable, (!lock));
                (*it)->setFlag(QGraphicsItem::ItemIsMovable, (!lock));
            }
            break;
        case 4://Water
            if((*it)->data(0).toString()=="Water")
            {
                (*it)->setFlag(QGraphicsItem::ItemIsSelectable, (!lock));
                (*it)->setFlag(QGraphicsItem::ItemIsMovable, (!lock));
            }
            break;
        case 5://Doors
            if(((*it)->data(0).toString()=="Door_enter")||((*it)->data(0).toString()=="Door_exit"))
            {
                (*it)->setFlag(QGraphicsItem::ItemIsSelectable, (!lock));
                (*it)->setFlag(QGraphicsItem::ItemIsMovable, (!lock));
            }
            break;
        default: break;
        }
    }

}

