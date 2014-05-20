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

#include "../common_features/logger.h"

#include "item_npc.h"
#include "itemmsgbox.h"

#include "newlayerbox.h"

ItemNPC::ItemNPC(QGraphicsPixmapItem *parent)
    : QGraphicsPixmapItem(parent)
{
    animated = false;
    aniDirect=false;
    aniBiDirect=false;
    curDirect = -1;
    frameStep = 1;
    gridSize = 1;

    imgOffsetX=0;
    imgOffsetY=0;

    frameFirst=0; //from first frame
    frameLast=-1; //to unlimited frameset
    //image = new QGraphicsPixmapItem;

    isLocked=false;
}


ItemNPC::~ItemNPC()
{
 //   WriteToLog(QtDebugMsg, "!<-Block destroyed->!");
}

void ItemNPC::contextMenuEvent( QGraphicsSceneContextMenuEvent * event )
{
    if((!scene->lock_npc)&&(!isLocked))
    {
        //Remove selection from non-block items
        if(this->isSelected())
        {
            foreach(QGraphicsItem * SelItem, scene->selectedItems() )
            {
                if(SelItem->data(0).toString()!="NPC") SelItem->setSelected(false);
            }
        }
        else
        {
            scene->clearSelection();
            this->setSelected(true);
        }


        this->setSelected(1);
        ItemMenu->clear();

        QMenu * LayerName = ItemMenu->addMenu(tr("Layer: ")+QString("[%1]").arg(npcData.layer));

        QAction *setLayer;
        QList<QAction *> layerItems;

        QAction * newLayer = LayerName->addAction(tr("Add to new layer..."));
        LayerName->addSeparator();

        foreach(LevelLayers layer, scene->LvlData->layers)
        {
            //Skip system layers
            if((layer.name=="Destroyed Blocks")||(layer.name=="Spawned NPCs")) continue;

            setLayer = LayerName->addAction( layer.name+((layer.hidden)?" [hidden]":"") );
            setLayer->setData(layer.name);
            setLayer->setCheckable(true);
            setLayer->setEnabled(true);
            setLayer->setChecked( layer.name==npcData.layer );
            layerItems.push_back(setLayer);
        }

        ItemMenu->addSeparator();

        QMenu * chDir = ItemMenu->addMenu(
                    tr("Set %1").arg(
                    (localProps.direct_alt_title!="") ?
                        localProps.direct_alt_title : tr("Direction") ) );

        QAction *setLeft = chDir->addAction( (localProps.direct_alt_left!="") ? localProps.direct_alt_left : tr("Left"));
            setLeft->setCheckable(true);
            setLeft->setChecked(npcData.direct==-1);

        QAction *setRand = chDir->addAction(tr("Random"));
            setRand->setVisible( !localProps.direct_disable_random );
            setRand->setCheckable(true);
            setRand->setChecked(npcData.direct==0);

        QAction *setRight = chDir->addAction( (localProps.direct_alt_right!="") ? localProps.direct_alt_right : tr("Right") );
            setRight->setCheckable(true);
            setRight->setChecked(npcData.direct==1);

        ItemMenu->addSeparator();

        QAction *fri = ItemMenu->addAction(tr("Friendly"));
            fri->setCheckable(1);
            fri->setChecked( npcData.friendly );

        QAction *stat = ItemMenu->addAction(tr("Not movable"));
            stat->setCheckable(1);
            stat->setChecked( npcData.nomove );


        QAction *msg = ItemMenu->addAction(tr("Set message..."));

        ItemMenu->addSeparator();

        QAction *boss = ItemMenu->addAction(tr("Legacy boss"));
            boss->setCheckable(1);
            boss->setChecked( npcData.legacyboss );

        ItemMenu->addSeparator();

        QAction *copyNpc = ItemMenu->addAction(tr("Copy"));
        QAction *cutNpc = ItemMenu->addAction(tr("Cut"));
        ItemMenu->addSeparator();
        QAction *remove = ItemMenu->addAction(tr("Remove"));

        scene->contextMenuOpened = true; //bug protector
QAction *selected = ItemMenu->exec(event->screenPos());

        if(!selected)
        {
            WriteToLog(QtDebugMsg, "Context Menu <- NULL");
            scene->contextMenuOpened = true;
            return;
        }
        event->accept();

        if(selected==cutNpc)
        {
            scene->doCut = true ;
            scene->contextMenuOpened = false;
        }
        else
        if(selected==copyNpc)
        {
            scene->doCopy = true ;
            scene->contextMenuOpened = false;
        }
        else
        if(selected==fri)
        {
            //apply to all selected items.
            LevelData selData;
            foreach(QGraphicsItem * SelItem, scene->selectedItems() )
            {
                if(SelItem->data(0).toString()=="NPC")
                    ((ItemNPC *) SelItem)->setFriendly(fri->isChecked());
                selData.npc.push_back(((ItemNPC *) SelItem)->npcData);
            }
            scene->addChangeSettingsHistory(selData, LvlScene::SETTING_FRIENDLY, QVariant(fri->isChecked()));
            scene->contextMenuOpened = false;
        }
        else
        if(selected==stat)
        {
            //apply to all selected items.
            LevelData selData;
            foreach(QGraphicsItem * SelItem, scene->selectedItems() )
            {
                if(SelItem->data(0).toString()=="NPC")
                    ((ItemNPC *) SelItem)->setNoMovable(stat->isChecked());
                selData.npc.push_back(((ItemNPC *) SelItem)->npcData);
            }
            scene->addChangeSettingsHistory(selData, LvlScene::SETTING_NOMOVEABLE, QVariant(stat->isChecked()));
            scene->contextMenuOpened = false;
        }
        else
            if(selected==msg)
            {
                LevelData selData;
                QList<QVariant> modText;
                modText.push_back(QVariant(npcData.msg));

                ItemMsgBox * msgBox = new ItemMsgBox(npcData.msg);
                msgBox->setWindowFlags (Qt::Window | Qt::WindowTitleHint | Qt::WindowCloseButtonHint);
                msgBox->setGeometry(QStyle::alignedRect(Qt::LeftToRight, Qt::AlignCenter, msgBox->size(), qApp->desktop()->availableGeometry()));
                if(msgBox->exec()==QDialog::Accepted)
                {

                    //apply to all selected items.
                    foreach(QGraphicsItem * SelItem, scene->selectedItems() )
                    {
                        if(SelItem->data(0).toString()=="NPC")
                            ((ItemNPC *) SelItem)->setMsg( msgBox->currentText );
                        selData.npc.push_back(((ItemNPC *) SelItem)->npcData);
                    }
                    modText.push_back(QVariant(npcData.msg));
                    scene->addChangeSettingsHistory(selData, LvlScene::SETTING_MESSAGE, QVariant(modText));
                }
                scene->contextMenuOpened = false;
            }
            else
        if(selected==boss)
        {
            //apply to all selected items.
            LevelData selData;
            foreach(QGraphicsItem * SelItem, scene->selectedItems() )
            {
                if(SelItem->data(0).toString()=="NPC")
                    ((ItemNPC *) SelItem)->setLegacyBoss(boss->isChecked());
                selData.npc.push_back(((ItemNPC *) SelItem)->npcData);
            }
            scene->addChangeSettingsHistory(selData, LvlScene::SETTING_BOSS, QVariant(boss->isChecked()));
            scene->contextMenuOpened = false;
        }
        else
        if(selected==setLeft)
        {
            LevelData selData;
            QList<QVariant> modDir;
            modDir.push_back(QVariant(npcData.direct));
            foreach(QGraphicsItem * SelItem, scene->selectedItems() )
            {
                if(SelItem->data(0).toString()=="NPC")
                    ((ItemNPC *) SelItem)->changeDirection(-1);
                selData.npc.push_back(((ItemNPC *) SelItem)->npcData);
            }
            modDir.push_back(QVariant(-1));
            scene->addChangeSettingsHistory(selData, LvlScene::SETTING_DIRECTION, QVariant(modDir));
            scene->contextMenuOpened = false;
        }
        if(selected==setRand)
        {
            LevelData selData;
            QList<QVariant> modDir;
            modDir.push_back(QVariant(npcData.direct));
            foreach(QGraphicsItem * SelItem, scene->selectedItems() )
            {
                if(SelItem->data(0).toString()=="NPC")
                    ((ItemNPC *) SelItem)->changeDirection(0);
                selData.npc.push_back(((ItemNPC *) SelItem)->npcData);
            }
            modDir.push_back(QVariant(0));
            scene->addChangeSettingsHistory(selData, LvlScene::SETTING_DIRECTION, QVariant(modDir));
            scene->contextMenuOpened = false;
        }
        if(selected==setRight)
        {
            LevelData selData;
            QList<QVariant> modDir;
            modDir.push_back(QVariant(npcData.direct));
            foreach(QGraphicsItem * SelItem, scene->selectedItems() )
            {
                if(SelItem->data(0).toString()=="NPC")
                    ((ItemNPC *) SelItem)->changeDirection(1);
                selData.npc.push_back(((ItemNPC *) SelItem)->npcData);
            }
            modDir.push_back(QVariant(1));
            scene->addChangeSettingsHistory(selData, LvlScene::SETTING_DIRECTION, QVariant(modDir));
            scene->contextMenuOpened = false;
        }
        else
        if(selected==remove)
        {
            foreach(QGraphicsItem * SelItem, scene->selectedItems() )
            {
                if(SelItem->data(0).toString()=="NPC")
                {
                    ((ItemNPC *)SelItem)->removeFromArray();
                    scene->removeItem(SelItem);
                }
            }
            scene->contextMenuOpened = false;
        }
        else
        {
            bool itemIsFound=false;
            QString lName;
            if(selected==newLayer)
            {
                scene->contextMenuOpened = false;
                ToNewLayerBox * layerBox = new ToNewLayerBox(scene->LvlData);
                layerBox->setWindowFlags (Qt::Window | Qt::WindowTitleHint | Qt::WindowCloseButtonHint);
                layerBox->setGeometry(QStyle::alignedRect(Qt::LeftToRight, Qt::AlignCenter, layerBox->size(), qApp->desktop()->availableGeometry()));
                if(layerBox->exec()==QDialog::Accepted)
                {
                    itemIsFound=true;
                    lName = layerBox->lName;

                    //Store new layer into array
                    LevelLayers nLayer;
                    nLayer.name = lName;
                    nLayer.hidden = layerBox->lHidden;
                    scene->LvlData->layers_array_id++;
                    nLayer.array_id = scene->LvlData->layers_array_id;
                    scene->LvlData->layers.push_back(nLayer);

                    scene->SyncLayerList=true; //Refresh layer list
                }
            }
            else
            foreach(QAction * lItem, layerItems)
            {
                if(selected==lItem)
                {
                    itemIsFound=true;
                    lName = lItem->data().toString();
                    //FOUND!!!
                 break;
                }//Find selected layer's item
            }

            if(itemIsFound)
            {
                foreach(LevelLayers lr, scene->LvlData->layers)
                { //Find layer's settings
                    if(lr.name==lName)
                    {
                        foreach(QGraphicsItem * SelItem, scene->selectedItems() )
                        {

                            if(SelItem->data(0).toString()=="NPC")
                            {
                            ((ItemNPC *) SelItem)->npcData.layer = lr.name;
                            ((ItemNPC *) SelItem)->setVisible(!lr.hidden);
                            ((ItemNPC *) SelItem)->arrayApply();
                            }
                        }
                    break;
                    }
                }//Find layer's settings
             scene->contextMenuOpened = false;
            }

//            foreach(QAction * lItem, layerItems)
//            {
//                if(selected==lItem)
//                {
//                    foreach(LevelLayers lr, scene->LvlData->layers)
//                    { //Find layer's settings
//                        if(lr.name==lItem->data().toString())
//                        {
//                            foreach(QGraphicsItem * SelItem, scene->selectedItems() )
//                            {

//                                if(SelItem->data(0).toString()=="NPC")
//                                {
//                                ((ItemNPC *) SelItem)->npcData.layer = lr.name;
//                                ((ItemNPC *) SelItem)->setVisible(!lr.hidden);
//                                ((ItemNPC *) SelItem)->arrayApply();
//                                }
//                            }
//                        break;
//                        }
//                    }//Find layer's settings
//                 scene->contextMenuOpened = false;
//                 break;
//                }//Find selected layer's item
//            }
        }
    }
    else
    {
        QGraphicsPixmapItem::contextMenuEvent(event);
    }
}

//Change arrtibutes
void ItemNPC::setFriendly(bool fri)
{
    npcData.friendly=fri;
    arrayApply(); //Apply changes into array
}

void ItemNPC::setMsg(QString message)
{
    npcData.msg=message;
    arrayApply();//Apply changes into array
}

void ItemNPC::setNoMovable(bool stat)
{
    npcData.nomove=stat;
    arrayApply();//Apply changes into array
}

void ItemNPC::setLegacyBoss(bool boss)
{
    npcData.legacyboss=boss;
    arrayApply();//Apply changes into array
}

void ItemNPC::changeDirection(int dir)
{
    npcData.direct = dir;

    setAnimation(localProps.frames, localProps.framespeed, localProps.framestyle, dir,
    localProps.custom_animate,
        localProps.custom_ani_fl,
        localProps.custom_ani_el,
        localProps.custom_ani_fr,
        localProps.custom_ani_er,
    true);

    arrayApply();
}

///////////////////MainArray functions/////////////////////////////
void ItemNPC::arrayApply()
{
    bool found=false;

    if(npcData.index < (unsigned int)scene->LvlData->npc.size())
    { //Check index
        if(npcData.array_id == scene->LvlData->npc[npcData.index].array_id)
            found=true;
    }

    //Apply current data in main array
    if(found)
    { //directlry
        scene->LvlData->npc[npcData.index] = npcData; //apply current npcData
    }
    else
    for(int i=0; i<scene->LvlData->npc.size(); i++)
    { //after find it into array
        if(scene->LvlData->npc[i].array_id == npcData.array_id)
        {
            npcData.index = i;
            scene->LvlData->npc[i] = npcData;
            break;
        }
    }
}

void ItemNPC::removeFromArray()
{
    bool found=false;
    if(npcData.index < (unsigned int)scene->LvlData->npc.size())
    { //Check index
        if(npcData.array_id == scene->LvlData->npc[npcData.index].array_id)
            found=true;
    }
    if(found)
    { //directlry
        scene->LvlData->npc.remove(npcData.index);
    }
    else
    for(int i=0; i<scene->LvlData->npc.size(); i++)
    {
        if(scene->LvlData->npc[i].array_id == npcData.array_id)
        {
            scene->LvlData->npc.remove(i); break;
        }
    }
}

void ItemNPC::setMainPixmap(const QPixmap &pixmap)
{
    mainImage = pixmap;
    this->setPixmap(mainImage);

    imgOffsetX = (int)round( - ( ( (double)localProps.gfx_w - (double)localProps.width ) / 2 ) );
    imgOffsetY = (int)round( - (double)localProps.gfx_h + (double)localProps.height + (double)localProps.gfx_offset_y );

    //grid_attach_style

    setOffset( imgOffsetX+(-((double)localProps.gfx_offset_x)*curDirect), imgOffsetY );
}

void ItemNPC::setNpcData(LevelNPC inD)
{
    npcData = inD;
}


QRectF ItemNPC::boundingRect() const
{
    if(!animated)
        return QRectF(0+imgOffsetX+(-((double)localProps.gfx_offset_x)*curDirect), 0+imgOffsetY, mainImage.width(), mainImage.height());
    else
        return QRectF(0+imgOffsetX+(-((double)localProps.gfx_offset_x)*curDirect), 0+imgOffsetY, localProps.gfx_w, localProps.gfx_h);
}

void ItemNPC::setContextMenu(QMenu &menu)
{
    ItemMenu = &menu;
}

void ItemNPC::setScenePoint(LvlScene *theScene)
{
    scene = theScene;
}


////////////////Animation///////////////////

void ItemNPC::setAnimation(int frames, int framespeed, int framestyle, int direct,
                           bool customAni, int frFL, int frEL, int frFR, int frER, bool edit)
{
    animated = true;
    framesQ = frames;
    frameSpeed = framespeed;
    frameStyle = framestyle;
    direction = direct;
    frameStep = 1;

    aniBiDirect = localProps.ani_bidir;
    customAniAlg = localProps.custom_ani_alg;

    customAnimate = customAni;

    custom_frameFL = frFL;//first left
    custom_frameEL = frEL;//end left
    custom_frameFR = frFR;//first right
    custom_frameER = frER;//enf right

    frameSize = localProps.gfx_h;

    frameWidth = localProps.gfx_w;

    frameHeight = mainImage.height();

    framePos = QPoint(0,0);
    draw();

    int dir=direction;

    if(direction==0) //if direction=random
    {
        dir=(((bool)qrand()%2)?-1:1); //set randomly 1 or -1
    }

    if(localProps.ani_directed_direct)
        aniDirect = (dir==-1) ^ (localProps.ani_direct);
    else
        aniDirect = localProps.ani_direct;

    if(customAnimate) // User defined spriteSet (example: boss)
    {
        switch(dir)
        {
        case -1: //left
            frameFirst = custom_frameFL;
            switch(customAniAlg)
            {
            case 1:
                frameStep = custom_frameEL;
                frameLast = -1; break;
            case 0:
            default:
                frameLast = custom_frameEL; break;
            }
            break;
        case 1: //Right
            frameFirst = custom_frameFR;
            switch(customAniAlg)
            {
            case 1:
                frameStep = custom_frameER;
                frameLast = -1; break;
            case 0:
            default:
                frameLast = custom_frameER; break;
            }
            break;
        default: break;
        }
    }
    else
    {
        switch(frameStyle)
        {
        case 2: //Left-Right-upper sprite
            framesQ = frames*4;
            switch(dir)
            {
            case -1: //left
                frameFirst = 0;
                frameLast = (int)(framesQ-(framesQ/4)*3)-1;
                break;
            case 1: //Right
                frameFirst = (int)(framesQ-(framesQ/4)*3);
                frameLast = (int)(framesQ/2)-1;
                break;
            default: break;
            }
            break;

        case 1: //Left-Right sprite
            framesQ=frames*2;
            switch(dir)
            {
            case -1: //left
                frameFirst = 0;
                frameLast = (int)(framesQ / 2)-1;
                break;
            case 1: //Right
                frameFirst = (int)(framesQ / 2);
                frameLast = framesQ-1;
                break;
            default: break;
            }

            break;

        case 0: //Single sprite
        default:
            frameFirst = 0;
            frameLast = -1;
            break;
        }
    }

    curDirect  = dir;
    setOffset(imgOffsetX+(-((double)localProps.gfx_offset_x)*curDirect), imgOffsetY );

    setFrame(frameFirst);

    if(!edit)
    {
        timer = new QTimer(this);
        connect(
                    timer, SIGNAL(timeout()),
                    this,
                    SLOT( nextFrame() ) );
    }
}

void ItemNPC::AnimationStart()
{
    if(!animated) return;
    timer->start(frameSpeed);
}

void ItemNPC::AnimationStop()
{
    if(!animated) return;
    timer->stop();
    setFrame(frameFirst);
}

void ItemNPC::draw()
{
    currentImage =  mainImage.copy(QRect(framePos.x(), framePos.y(), frameWidth, frameSize ));
}

QPoint ItemNPC::fPos() const
{
    return framePos;
}

void ItemNPC::setFrame(int y)
{
    frameCurrent = frameSize * y;
    if ( ((frameCurrent >= frameHeight )&&(frameLast==-1)) ||
         ((frameCurrent >= frameLast*frameSize )&&(frameLast>-1)) )
        {
        frameCurrent = frameFirst*frameSize;
        framePos.setY( frameFirst * frameSize );
        }
    else
        framePos.setY( frameCurrent );
    draw();
    this->setPixmap(QPixmap(currentImage));
}

void ItemNPC::setLocked(bool lock)
{
    this->setFlag(QGraphicsItem::ItemIsSelectable, !lock);
    this->setFlag(QGraphicsItem::ItemIsMovable, !lock);
    isLocked = lock;
}

void ItemNPC::nextFrame()
{

    if(!aniDirect)
    {
    frameCurrent += frameSize * frameStep;

    if ( ((frameCurrent >= frameHeight )&&(frameLast==-1)) ||
         ((frameCurrent > frameLast*frameSize )&&(frameLast>-1)) )
        {
            if(!aniBiDirect)
            {
            frameCurrent = frameFirst * frameSize;
            framePos.setY( frameFirst * frameSize );
            } else
            {
            frameCurrent -= frameSize*frameStep*2;
            framePos.setY( framePos.y() - frameSize*frameStep );
            aniDirect=!aniDirect;
            }
        }
    else
        framePos.setY( framePos.y() + frameSize*frameStep );
    }
    else
    {
        frameCurrent -= frameSize * frameStep;

        if (frameCurrent < (frameFirst*frameSize) )
            {
                if(!aniBiDirect)
                {
                    frameCurrent = ( ((frameLast==-1)? frameHeight : frameLast*frameSize)-frameSize);
                    framePos.setY( ((frameLast==-1) ? frameHeight : frameLast*frameSize)-frameSize );
                } else
                {
                frameCurrent += frameSize*frameStep*2;
                framePos.setY( framePos.y() + frameSize*frameStep );
                aniDirect=!aniDirect;
                }
            }
        else
            framePos.setY( framePos.y() - frameSize*frameStep );
    }

    draw();
    this->setPixmap(QPixmap(currentImage));
}
