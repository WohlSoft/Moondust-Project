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
#include "item_npc.h"


QPoint LvlScene::applyGrid(QPoint source, int gridSize, QPoint gridOffset)
{
    int gridX, gridY;
    if((grid)&&(gridSize>0))
    { //ATTACH TO GRID
        gridX = ((int)source.x() - (int)source.x() % gridSize);
        gridY = ((int)source.y() - (int)source.y() % gridSize);

        if((int)source.x()<0)
        {
            if( (int)source.x() < gridOffset.x()+gridX - (int)(gridSize/2) )
                gridX -= gridSize;
        }
        else
        {
            if( (int)source.x() > gridOffset.x()+gridX + (int)(gridSize/2) )
                gridX += gridSize;
        }

        if((int)source.y()<0)
        {if( (int)source.y() < gridOffset.y()+gridY - (int)(gridSize/2) )
            gridY -= gridSize;
        }
        else {if( (int)source.y() > gridOffset.y()+gridY + (int)(gridSize/2) )
         gridY += gridSize;
        }

        return QPoint(gridOffset.x()+gridX, gridOffset.y()+gridY);

    }
    else
        return source;
}


QPixmap LvlScene::getNPCimg(unsigned long npcID)
{
    bool noimage=true, found=false;
    bool isUser=false, isUserTxt=false;
    int j;
    QPixmap tempI;
    int gfxH = 0;

    //Check Index exists
    if(npcID < (unsigned int)index_npc.size())
    {
        j = index_npc[npcID].i;

        if(pConfigs->main_npc[j].id == npcID)
            found=true;
    }

    //if Index found
    if(found)
    {   //get neccesary element directly
        if(index_npc[npcID].type==1)
        {
            isUser=true;
            if(uNPCs[index_npc[npcID].i].withImg)
            {
                noimage=false;
                tempI = uNPCs[index_npc[npcID].i].image;
            }
            if(uNPCs[index_npc[npcID].i].withTxt)
                gfxH = uNPCs[index_npc[npcID].i].merged.gfx_h;
            else
                gfxH = pConfigs->main_npc[index_npc[npcID].gi].height;
        }

        if(!noimage)
        {
            tempI = pConfigs->main_npc[(isUser) ? index_npc[npcID].gi : index_npc[npcID].i].image;
            noimage=false;
        }

    }
    else
    {
        //found neccesary element in arrays and select
        for(j=0;j<uNPCs.size();j++)
        {
            if(uNPCs[j].id == npcID)
            {
                if(uNPCs[j].withImg)
                {
                    isUser=true;
                    noimage=false;
                    tempI = uNPCs[j].image;
                }
                if(uNPCs[j].withTxt)
                {
                    isUserTxt = true;
                    gfxH = uNPCs[j].merged.gfx_h;
                }
                break;
            }
        }

        for(j=0;j<pConfigs->main_npc.size();j++)
        {
            if(pConfigs->main_npc[j].id==npcID)
            {
                noimage=false;
                if(!isUser)
                    tempI = pConfigs->main_npc[j].image;
                if(!isUserTxt)
                    gfxH =  pConfigs->main_npc[j].gfx_h;
                break;
            }
        }
    }

    if((noimage)||(tempI.isNull()))
    {
        return uNpcImg;
    }

    return tempI.copy(0,0, tempI.width(), gfxH );
}


obj_npc LvlScene::mergeNPCConfigs(obj_npc &global, NPCConfigFile &local)
{
    obj_npc merged;
    merged = global;
    merged.image = QPixmap();   //Clear image values
    merged.mask = QPixmap();

//    int gfxoffsetx;
//    bool en_gfxoffsetx;
    merged.gfx_offset_x = (local.en_gfxoffsetx)?local.gfxoffsetx:global.gfx_offset_x;

//    int gfxoffsety;
//    bool en_gfxoffsety;
    merged.gfx_offset_y = (local.en_gfxoffsety)?local.gfxoffsety:global.gfx_offset_y;

//    unsigned int width;
//    bool en_width;
    merged.width = (local.en_width)?local.width:global.width;

//    unsigned int height;
//    bool en_height;
    //merged. = (local.en_)?local.:global.;
    merged.height = (local.en_height)?local.height:global.height;


    //Copy fixture size to GFX size if that greater
    if(global.height == (unsigned int)global.gfx_h)
        merged.gfx_h = merged.height;
    else
        merged.gfx_h = global.gfx_h;

    //Copy fixture size to GFX size if that greater
    if(global.width == (unsigned int)global.gfx_w)
        merged.gfx_w = merged.width;
    else
        merged.gfx_w = global.gfx_w;

    if(merged.grid_attach_style)
        merged.grid_offset_x = -qRound( qreal(merged.gfx_w % merged.grid)/2 )+16;
    else
        merged.grid_offset_x = -qRound( qreal(merged.gfx_w % merged.grid)/2 );

    merged.grid_offset_y = -merged.height % merged.grid;

//    unsigned int gfxwidth;
//    bool en_gfxwidth;
    merged.gfx_w = (local.en_gfxwidth)?local.gfxwidth:merged.gfx_w;

//    unsigned int gfxheight;
//    bool en_gfxheight;
    merged.gfx_h = (local.en_gfxheight)?local.gfxheight:merged.gfx_h;


//    unsigned int score;
//    bool en_score;
    merged.score = (local.en_score)?local.score:global.score;

//    bool playerblock;
//    bool en_playerblock;
    merged.block_player = (local.en_playerblock)?local.playerblock:global.block_player;


//    bool playerblocktop;
//    bool en_playerblocktop;
    merged.block_player_top = (local.en_playerblocktop)?local.playerblocktop:global.block_player_top;

//    bool npcblock;
//    bool en_npcblock;
    merged.block_npc = (local.en_npcblock)?local.npcblock:global.block_npc;

//    bool npcblocktop;
//    bool en_npcblocktop;
    merged.block_npc_top = (local.en_npcblocktop)?local.npcblocktop:global.block_npc_top;


//    bool grabside;
//    bool en_grabside;
    merged.grab_side = (local.en_grabside)?local.grabside:global.grab_side;


//    bool grabtop;
//    bool en_grabtop;
    merged.grab_top = (local.en_grabtop)?local.grabtop:global.grab_top;


//    bool jumphurt;
//    bool en_jumphurt;
    merged.kill_on_jump = (local.en_jumphurt)? (!local.jumphurt) : global.kill_on_jump ;

//    bool nohurt;
//    bool en_nohurt;
    merged.hurt_player = (local.en_nohurt)?!local.nohurt:global.hurt_player;

//    bool noblockcollision;
//    bool en_noblockcollision;
    merged.collision_with_blocks = (local.en_noblockcollision)?(!local.noblockcollision):global.collision_with_blocks;

//    bool cliffturn;
//    bool en_cliffturn;
    merged.turn_on_cliff_detect = (local.en_cliffturn)?local.cliffturn:global.turn_on_cliff_detect;


//    bool noyoshi;
//    bool en_noyoshi;
    merged.can_be_eaten = (local.en_noyoshi)?(!local.noyoshi):global.can_be_eaten;


//    bool foreground;
//    bool en_foreground;
    merged.foreground = (local.en_foreground)?local.foreground:global.foreground;

//    float speed;
//    bool en_speed;
    merged.speed = (local.en_speed) ? global.speed * local.speed : global.speed;


//    bool nofireball;
//    bool en_nofireball;
    merged.kill_by_fireball = (local.en_nofireball)?(!local.nofireball):global.kill_by_fireball;

//    bool nogravity;
//    bool en_nogravity;
    merged.gravity = (local.en_nogravity)?(!local.nogravity):global.gravity;

//    unsigned int frames;
//    bool en_frames;
    merged.frames = (local.en_frames)?local.frames:global.frames;

//    unsigned int framespeed;
//    bool en_framespeed;
    merged.framespeed = (local.en_framespeed)? qRound( qreal(global.framespeed) / qreal(8 / local.framespeed) ) : global.framespeed;


//    unsigned int framestyle;
//    bool en_framestyle;
    merged.framestyle = (local.en_framestyle)?local.framestyle:global.framestyle;

    if((local.en_frames)||(local.en_framestyle))
        merged.ani_bidir = false; //Disable bidirectional animation

//    bool noiceball;
//    bool en_noiceball;
    merged.freeze_by_iceball = (local.en_noiceball)?(!local.noiceball):global.freeze_by_iceball;


//    bool nohammer;
//    bool en_nohammer;
    merged.kill_hammer = (local.en_nohammer)?(!local.nohammer):global.kill_hammer;

//    bool noshell;
//    bool en_noshell;
    merged.kill_by_npc = (local.en_noshell)?(!local.noshell):global.kill_by_npc;

    return merged;
}


////////////////////////////////// Place new ////////////////////////////////

void LvlScene::placeBlock(LevelBlock &block, bool toGrid)
{
    bool noimage=true, found=false;
    bool isUser=false;
    int j;

    QGraphicsItem *npc = NULL;
    QGraphicsItemGroup *includedNPC;
    ItemBlock *BlockImage = new ItemBlock;

    noimage=true;
    isUser=false;

    //Check Index exists
    if(block.id < (unsigned int)index_blocks.size())
    {
        j = index_blocks[block.id].i;

        if(pConfigs->main_block[j].id == block.id)
            found=true;
    }

    //if Index found
    if(found)
    {   //get neccesary element directly
        if(index_blocks[block.id].type==1)
        {
            isUser=true;
            noimage=false;
            tImg = uBlocks[index_blocks[block.id].i].image;
        }
        else
        {
            tImg = pConfigs->main_block[index_blocks[block.id].i].image;
            noimage=false;
        }
    }
    else
    {
        //found neccesary element in arrays and select
        for(j=0;j<uBlocks.size();j++)
        {
            if(uBlocks[j].id == block.id)
            {
                isUser=true;
                noimage=false;
                tImg = uBlocks[j].image;
                break;
            }
        }

        for(j=0;j<pConfigs->main_block.size();j++)
        {
            if(pConfigs->main_block[j].id==block.id)
            {
                noimage=false;
                if(!isUser)
                    tImg = pConfigs->main_block[j].image; break;
            }
        }
    }

    if((noimage)||(tImg.isNull()))
    {
        //if(block.id==89) WriteToLog(QtDebugMsg, QString("Block 89 is %1, %2").arg(noimage).arg(tImg.isNull()));
        tImg = uBlockImg;
    }

    BlockImage->setBlockData(block, pConfigs->main_block[j].sizable);
    BlockImage->setMainPixmap(tImg);
    addItem(BlockImage);

    BlockImage->setContextMenu(blockMenu);

    if((!noimage) && (pConfigs->main_block[j].animated))
    {
        BlockImage->setAnimation(pConfigs->main_block[j].frames, pConfigs->main_block[j].framespeed, pConfigs->main_block[j].algorithm);
        BlockImage->setData(4, "animated");
    }

    includedNPC = new QGraphicsItemGroup(BlockImage);

    //Set pointers
    BlockImage->setScenePoint(this);
    BlockImage->setGroupPoint(includedNPC);
    BlockImage->setNPCItemPoint(npc);

    if(block.invisible)
        BlockImage->setOpacity(qreal(0.5));

    QPoint newPos = QPoint(block.x, block.y);
    if(toGrid)
    {
        newPos = applyGrid(QPoint(block.x, block.y), 32);
        block.x = newPos.x();
        BlockImage->blockData.x = newPos.x();
        block.y = newPos.y();
        BlockImage->blockData.y = newPos.y();
    }

    BlockImage->setPos(QPointF(newPos));

    //////////////////////////////Included NPC////////////////////////////////////////
    if(block.npc_id != 0)
    {
        BlockImage->setIncludedNPC(block.npc_id);
    }
    //////////////////////////////////////////////////////////////////////////////////

    if(pConfigs->main_block[j].sizable)
    {
        BlockImage->setZValue( blockZs + ((double)block.y/(double)100000000000) + 1 - ((double)block.w * (double)0.0000000000000001) ); // applay sizable block Z
        //sbZ += 0.0000000001;
    }
    else
    {
        if(pConfigs->main_block[j].view==1)
            BlockImage->setZValue(blockZl); // applay lava block Z
        else
            BlockImage->setZValue(blockZ); // applay standart block Z
    }

    BlockImage->setFlag(QGraphicsItem::ItemIsSelectable, (!lock_block));
    BlockImage->setFlag(QGraphicsItem::ItemIsMovable, (!lock_block));

    BlockImage->setData(0, "Block");
    BlockImage->setData(1, QString::number(block.id) );
    BlockImage->setData(2, QString::number(block.array_id) );


    if(pConfigs->main_block[j].sizable)
    {
        BlockImage->setData(3, "sizable" );
    }
    else
        BlockImage->setData(3, "standart" );

    BlockImage->setData(9, QString::number(block.w) ); //width
    BlockImage->setData(10, QString::number(block.h) ); //height
    if(PasteFromBuffer) BlockImage->setSelected(true);
}


void LvlScene::placeBGO(LevelBGO &bgo, bool toGrid)
{
    int j;
    bool noimage=true, found=false;

    ItemBGO *BGOItem = new ItemBGO;
    bool isUser=false;

    noimage=true;
    isUser=false;

    //Check Index exists
    if(bgo.id < (unsigned int)index_bgo.size())
    {
        j = index_bgo[bgo.id].i;

        if(pConfigs->main_bgo[j].id == bgo.id)
            found=true;
    }

    //if Index found
    if(found)
    {   //get neccesary element directly
        if(index_bgo[bgo.id].type==1)
        {
            isUser=true;
            noimage=false;
            tImg = uBGOs[index_bgo[bgo.id].i].image;
        }
        else
        {
            tImg = pConfigs->main_bgo[index_bgo[bgo.id].i].image;
            noimage=false;
        }
    }
    else
    {
        //fetching arrays
        for(j=0;j<uBGOs.size();j++)
        {
            if(uBGOs[j].id==bgo.id)
            {
                isUser=true;
                noimage=false;
                tImg = uBGOs[j].image;
                break;
            }
        }

        for(j=0;j<pConfigs->main_bgo.size();j++)
        {
            if(pConfigs->main_bgo[j].id==bgo.id)
            {
                noimage=false;
                if(!isUser)
                tImg = pConfigs->main_bgo[j].image; break;
            }
        }
    }

    if((noimage)||(tImg.isNull()))
    {
        tImg=uBgoImg;
    }

    BGOItem->setBGOData(bgo);
        BGOItem->gridSize = pConfigs->main_bgo[j].grid;
        BGOItem->gridOffsetX = pConfigs->main_bgo[j].offsetX;
        BGOItem->gridOffsetY = pConfigs->main_bgo[j].offsetY;
    BGOItem->setMainPixmap(tImg);
    BGOItem->setContextMenu(bgoMenu);
    addItem(BGOItem);

    //WriteToLog(QtDebugMsg, QString("BGO Item-> source data %1 %2").arg(bgo.x).arg(bgo.y));

    QPoint newPos = QPoint(bgo.x, bgo.y);
    if(toGrid)
    {
        newPos = applyGrid(QPoint(bgo.x, bgo.y), pConfigs->main_bgo[j].grid,
                           QPoint(pConfigs->main_bgo[j].offsetX, pConfigs->main_bgo[j].offsetY));
        bgo.x = newPos.x();
        bgo.y = newPos.y();
    }

    //WriteToLog(QtDebugMsg, QString("BGO Item-> new data pos 1 %1 %2").arg(bgo.x).arg(bgo.y));

    BGOItem->setPos( QPointF(newPos) );

    //WriteToLog(QtDebugMsg, QString("BGO Item-> new data pos 2 %1 %2").arg(newPos.x()).arg(newPos.y()));
    //WriteToLog(QtDebugMsg, QString("BGO Item-> new data pos 3 %1 %2").arg(BGOItem->pos().x()).arg(BGOItem->pos().y()));


    if((!noimage) && (pConfigs->main_bgo[j].animated))
    {
        //tImg=tImg.copy(0, 0, tImg.width(), (int)round(tImg.height()/pConfigs->main_bgo[j].frames));
        BGOItem->setAnimation(pConfigs->main_bgo[j].frames, pConfigs->main_bgo[j].framespeed);
        BGOItem->setData(4, "animated");
    }

    BGOItem->setFlag(QGraphicsItem::ItemIsSelectable, (!lock_bgo));
    BGOItem->setFlag(QGraphicsItem::ItemIsMovable, (!lock_bgo));

    BGOItem->setData(0, "BGO");
    BGOItem->setData(1, QString::number(bgo.id) );
    BGOItem->setData(2, QString::number(bgo.array_id) );

    BGOItem->setData(9, QString::number(tImg.width()) ); //width
    BGOItem->setData(10, QString::number(tImg.height()) ); //height

    if(pConfigs->main_bgo[j].view!=0)
        BGOItem->setZValue(bgoZf + pConfigs->main_bgo[j].zOffset);
        //bgoback->addToGroup(box);
    else
        BGOItem->setZValue(bgoZb + pConfigs->main_bgo[j].zOffset);
        //bgofore->addToGroup(box);

    BGOItem->setScenePoint(this);

    if(PasteFromBuffer) BGOItem->setSelected(true);
}

void LvlScene::placeNPC(LevelNPC &npc, bool toGrid)
{

    int j;
    bool noimage=true, found=false;
    bool isUser=false;
    bool isUserTxt=false;
    obj_npc mergedSet;

    ItemNPC *NPCItem = new ItemNPC;

    //QGraphicsItem *	box;
    //QGraphicsTextItem *npcTxt, *npcTxt_l2;
    //QGraphicsItemGroup *npcBox;

    //    QFont font1, font2;
    //    font1.setWeight((toGrid)?50:50); //dummy expresson fo fix warning
    //    font1.setBold(1);
    //    font1.setPointSize(14);

    //    font2.setWeight(14);
    //    font2.setBold(0);
    //    font2.setPointSize(12);

    //Check Index exists
    if(npc.id < (unsigned int)index_npc.size())
    {
        j = index_npc[npc.id].gi;

        if(pConfigs->main_npc[j].id == npc.id)
            found=true;
    }

    //if Index found
    if(found)
    {   //get neccesary element directly
        if(index_npc[npc.id].type==1)
        {
            isUser=true;
            if(uNPCs[index_npc[npc.id].i].withImg)
            {
                noimage=false;
                tImg = uNPCs[index_npc[npc.id].i].image;
            }

            if(uNPCs[index_npc[npc.id].i].withTxt)
            {
                isUserTxt=true;
                mergedSet = uNPCs[index_npc[npc.id].i].merged;
            }
            else
                mergedSet = pConfigs->main_npc[index_npc[npc.id].gi];
        }

        if(!noimage)
        {
            tImg = pConfigs->main_npc[ index_npc[npc.id].gi].image;
            noimage=false;
        }
    }
    else
    {
        //fetching arrays
        for(j=0;j<uNPCs.size();j++)
        {
            if(uNPCs[j].id==npc.id)
            {
                if(uNPCs[j].withImg)
                {
                    noimage=false;
                    isUser=true;
                    tImg = uNPCs[j].image;
                }

                if(uNPCs[j].withTxt)
                {
                    isUserTxt=true;
                    mergedSet = uNPCs[j].merged;
                }
                break;
            }
        }

        for(j=0;j<pConfigs->main_npc.size();j++)
        {
            if(pConfigs->main_npc[j].id==npc.id)
            {
                noimage=false;
                if(!isUser)
                    tImg = pConfigs->main_npc[j].image;
                if(!isUserTxt)
                    mergedSet = pConfigs->main_npc[j];
                break;
            }
        }
    }

    if((noimage)||(tImg.isNull()))
    {
        tImg=uNpcImg;
    }

        //WriteToLog(QtDebugMsg, "NPC place -> set Data");
    NPCItem->setNpcData(npc);

        //WriteToLog(QtDebugMsg, "NPC place -> set Props");
    NPCItem->localProps = mergedSet;

    if(npc.generator)
        NPCItem->gridSize=16;
    else
        NPCItem->gridSize = mergedSet.grid;


        //WriteToLog(QtDebugMsg, "NPC place -> set Pixmap");
    NPCItem->setMainPixmap(tImg);

        //WriteToLog(QtDebugMsg, "NPC place -> set ContextMenu");
    NPCItem->setContextMenu(npcMenu);

        //WriteToLog(QtDebugMsg, "NPC place -> Add to scene");
    addItem(NPCItem);

    NPCItem->setScenePoint(this);

    QPoint newPos = QPoint(npc.x, npc.y);
    if(toGrid)
    {
        newPos = applyGrid(QPoint(npc.x, npc.y), NPCItem->localProps.grid,
                           QPoint(NPCItem->localProps.grid_offset_x,
                                  NPCItem->localProps.grid_offset_y));
        npc.x = newPos.x();
        npc.y = newPos.y();
    }

    //WriteToLog(QtDebugMsg, QString("BGO Item-> new data pos 1 %1 %2").arg(bgo.x).arg(bgo.y));

    //int imgOffsetX = (int)round( (-((double)mergedSet.gfx_w - (double)mergedSet.width ) / 2 ) + (double)mergedSet.gfx_offset_x);
    //int imgOffsetY = (int)round( - (double)mergedSet.gfx_h + (double)mergedSet.height + (double)mergedSet.gfx_offset_y );

    //QPoint offsettenPos = QPoint(npc.x + imgOffsetX, npc.y+imgOffsetY);

    //NPCItem->setPos( QPointF(offsettenPos) );
    NPCItem->setPos( QPointF(newPos) );

    NPCItem->setAnimation(NPCItem->localProps.frames,
                          NPCItem->localProps.framespeed,
                          NPCItem->localProps.framestyle,
                          npc.direct,
                          NPCItem->localProps.custom_animate,
                          NPCItem->localProps.custom_ani_fl,
                          NPCItem->localProps.custom_ani_el,
                          NPCItem->localProps.custom_ani_fr,
                          NPCItem->localProps.custom_ani_er);

    if(NPCItem->localProps.frames>1)
        NPCItem->setData(4, "animated");

    //box = addPixmap(QPixmap(uNpcImg));
    //npcBox = new QGraphicsItemGroup(box);

    /*
    npcTxt = new QGraphicsTextItem(QString::number(npc.id));
    npcTxt->setDefaultTextColor(Qt::black);
    npcTxt->setFont(font1);
    npcTxt->setPos(npc.x-5, npc.y-2);
    npcTxt_l2 = new QGraphicsTextItem(QString::number(npc.id));
    npcTxt_l2->setDefaultTextColor(Qt::white);
    npcTxt_l2->setFont(font2);
    npcTxt_l2->setPos(npc.x-3, npc.y);

    npcBox->addToGroup(npcTxt);
    npcBox->addToGroup(npcTxt_l2);

    npcTxt->setZValue(npcZf+0.0000001);
    npcTxt_l2->setZValue(npcZf+0.0000002);
    */

    NPCItem->setFlag(QGraphicsItem::ItemIsSelectable, (!lock_npc));
    NPCItem->setFlag(QGraphicsItem::ItemIsMovable, (!lock_npc));

    //npcfore->addToGroup(box);
    //if(npc.id==91)
    if(NPCItem->localProps.foreground)
        NPCItem->setZValue(npcZf);
    else
    if(NPCItem->localProps.background)
        NPCItem->setZValue(npcZb);
    else
        NPCItem->setZValue(npcZs);
    //else
    //    box->setZValue(npcZb);

    NPCItem->setData(0, "NPC"); // ObjType
    NPCItem->setData(1, QString::number(npc.id) );
    NPCItem->setData(2, QString::number(npc.array_id) );

    NPCItem->setData(9, QString::number(NPCItem->localProps.width) ); //width
    NPCItem->setData(10, QString::number(NPCItem->localProps.height) ); //height

    if(PasteFromBuffer) NPCItem->setSelected(true);
}


void LvlScene::placeDoor(LevelDoors &door, bool toGrid)
{
    long ix, iy, ox, oy, h, w;
    QGraphicsItem *	enter;
    QGraphicsItem *	exit;
    QGraphicsItemGroup *enterId, *exitId;
    QGraphicsTextItem *enterTxt, *enterTxt_l2;
    QGraphicsTextItem *exitTxt, *exitTxt_l2;

    ix = door.ix;
    iy = door.iy;
    ox = door.ox;
    oy = door.oy;
    h = 32;
    w = 32;
    QFont font1, font2;
    font1.setWeight((toGrid)?50:50); //dummy expresson fo fix warning
    font1.setBold(1);
    font1.setPointSize(14);

    font2.setWeight(14);
    font2.setBold(0);
    font2.setPointSize(12);

    //font.setStyle(QFont::Times);
    //font.setStyle();

    QColor cEnter(Qt::magenta);
    QColor cExit(Qt::darkMagenta);
    cEnter.setAlpha(50);
    cExit.setAlpha(50);

    if( ((!door.lvl_o) && (!door.lvl_i)) || ((door.lvl_o) && (!door.lvl_i)) )
    {
        enter = addRect(ix, iy, w, h, QPen(Qt::magenta, 2,Qt::SolidLine), QBrush(cEnter));
        enterId = new QGraphicsItemGroup(enter);

        enterTxt = new QGraphicsTextItem(QString::number(door.array_id));
        enterTxt->setDefaultTextColor(Qt::black);
        enterTxt->setFont(font1);
        enterTxt->setPos(ix-5, iy-2);
        enterTxt_l2 = new QGraphicsTextItem(QString::number(door.array_id));
        enterTxt_l2->setDefaultTextColor(Qt::white);
        enterTxt_l2->setFont(font2);
        enterTxt_l2->setPos(ix-3, iy);

        enterId->addToGroup(enterTxt);
        enterId->addToGroup(enterTxt_l2);
        enter->setFlag(QGraphicsItem::ItemIsSelectable, (!lock_door));
        enter->setFlag(QGraphicsItem::ItemIsMovable, (!lock_door));
        enter->setZValue(doorZ);

        enterTxt->setZValue(doorZ+0.0000001);
        enterTxt_l2->setZValue(doorZ+0.0000002);
        enter->setData(0, "Door_enter"); // ObjType
        enter->setData(1, QString::number(0) );
        enter->setData(2, QString::number(door.array_id) );
    }

    if( ((!door.lvl_o) && (!door.lvl_i)) || ((door.lvl_i)) )
    {
        exit = addRect(ox, oy, w, h, QPen(Qt::darkMagenta, 2,Qt::SolidLine), QBrush(cExit));
        exitId = new QGraphicsItemGroup(exit);

        exitTxt = new QGraphicsTextItem(QString::number(door.array_id));
        exitTxt->setDefaultTextColor(Qt::black);
        exitTxt->setFont(font1);
        exitTxt->setPos(ox+10, oy+8);
        exitTxt_l2 = new QGraphicsTextItem(QString::number(door.array_id));
        exitTxt_l2->setDefaultTextColor(Qt::white);
        exitTxt_l2->setFont(font2);
        exitTxt_l2->setPos(ox+12, oy+10);

        exitId->addToGroup(exitTxt);
        exitId->addToGroup(exitTxt_l2);
        exit->setFlag(QGraphicsItem::ItemIsSelectable, (!lock_door));
        exit->setFlag(QGraphicsItem::ItemIsMovable, (!lock_door));
        exit->setZValue(doorZ);
        exitTxt->setZValue(doorZ+0.0000001);
        exitTxt_l2->setZValue(doorZ+0.0000002);
        exit->setData(0, "Door_exit"); // ObjType
        exit->setData(1, QString::number(0) );
        exit->setData(2, QString::number(door.array_id) );
    }


}


void LvlScene::placeWater(LevelWater &water, bool toGrid)
{
    long x, y, h, w;
    QGraphicsItem *	box;
    //if(!progress.wasCanceled())
    //    progress.setLabelText("Applayng water "+QString::number(i)+"/"+QString::number(FileData.water.size()));

    QPoint newPos = QPoint(water.x, water.y);

    if(toGrid)
    {
        newPos = applyGrid(QPoint(water.x, water.y), 16);
        water.x = newPos.x();
        water.y = newPos.y();
    }

    x = newPos.x();
    y = newPos.y();
    h = water.h;
    w = water.w;

    //box = addRect(x, y, w, h, QPen(((water.quicksand)?Qt::yellow:Qt::green), 4), Qt::NoBrush);

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

    box = addPolygon(QPolygon(points), QPen(((water.quicksand)?Qt::yellow:Qt::green), 4));

    box->setFlag(QGraphicsItem::ItemIsSelectable, (!lock_water));
    box->setFlag(QGraphicsItem::ItemIsMovable, (!lock_water));

    box->setZValue(waterZ);

    box->setData(0, "Water"); // ObjType
    box->setData(1, QString::number(0) );
    box->setData(2, QString::number(water.array_id) );

}
