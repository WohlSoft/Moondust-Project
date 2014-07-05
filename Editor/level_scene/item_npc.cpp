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

#include "../common_features/logger.h"

#include "item_npc.h"
#include "itemmsgbox.h"

#include "newlayerbox.h"

#include "../common_features/mainwinconnect.h"

ItemNPC::ItemNPC(bool noScene, QGraphicsPixmapItem *parent)
    : QGraphicsPixmapItem(parent)
{
    setShapeMode(QGraphicsPixmapItem::BoundingRectShape);
    generatorArrow = NULL;
    includedNPC = NULL;
    DisableScene = noScene;
    animated = false;
    aniDirect=false;
    aniBiDirect=false;
    curDirect = -1;
    frameStep = 1;
    gridSize = 1;
    frameSize=1;

    CurrentFrame=0;

    imgOffsetX=0;
    imgOffsetY=0;

    frameFirst=0; //from first frame
    frameLast=-1; //to unlimited frameset
    //image = new QGraphicsPixmapItem;

    isLocked=false;

    timer=NULL;
}


ItemNPC::~ItemNPC()
{
    //WriteToLog(QtDebugMsg, "!<-NPC destroyed->!");
    if(includedNPC!=NULL) delete includedNPC;
    if(grp!=NULL) delete grp;
    if(timer) delete timer;
}


void ItemNPC::mousePressEvent ( QGraphicsSceneMouseEvent * mouseEvent )
{
    if(!DisableScene)
        if(scene->DrawMode)
        {
            unsetCursor();
            ungrabMouse();
            this->setSelected(false);
            return;
        }
    QGraphicsPixmapItem::mousePressEvent(mouseEvent);
}

void ItemNPC::contextMenuEvent( QGraphicsSceneContextMenuEvent * event )
{
    if(DisableScene)
    {
        QGraphicsPixmapItem::contextMenuEvent(event);
        return;
    }

    if((!scene->lock_npc)&&(!scene->DrawMode)&&(!isLocked))
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
        LayerName->deleteLater();

        QAction * newLayer = LayerName->addAction(tr("Add to new layer..."));
        newLayer->deleteLater();
        LayerName->addSeparator()->deleteLater();

        foreach(LevelLayers layer, scene->LvlData->layers)
        {
            //Skip system layers
            if((layer.name=="Destroyed Blocks")||(layer.name=="Spawned NPCs")) continue;

            setLayer = LayerName->addAction( layer.name+((layer.hidden)?" [hidden]":"") );
            setLayer->setData(layer.name);
            setLayer->setCheckable(true);
            setLayer->setEnabled(true);
            setLayer->setChecked( layer.name==npcData.layer );
            setLayer->deleteLater();
            layerItems.push_back(setLayer);
        }

        ItemMenu->addSeparator()->deleteLater();
        QAction *newNPC = ItemMenu->addAction(tr("New NPC-Configuration"));
        newNPC->deleteLater();
        ItemMenu->addSeparator()->deleteLater();

        QMenu * chDir = ItemMenu->addMenu(
                    tr("Set %1").arg(
                    (localProps.direct_alt_title!="") ?
                        localProps.direct_alt_title : tr("Direction") ) );
        chDir->deleteLater();

        QAction *setLeft = chDir->addAction( (localProps.direct_alt_left!="") ? localProps.direct_alt_left : tr("Left"));
            setLeft->setCheckable(true);
            setLeft->setChecked(npcData.direct==-1);
            setLeft->deleteLater();

        QAction *setRand = chDir->addAction(tr("Random"));
            setRand->setVisible( !localProps.direct_disable_random );
            setRand->setCheckable(true);
            setRand->setChecked(npcData.direct==0);
            setRand->deleteLater();

        QAction *setRight = chDir->addAction( (localProps.direct_alt_right!="") ? localProps.direct_alt_right : tr("Right") );
            setRight->setCheckable(true);
            setRight->setChecked(npcData.direct==1);
            setRight->deleteLater();

        ItemMenu->addSeparator()->deleteLater();;

        QAction *fri = ItemMenu->addAction(tr("Friendly"));
            fri->setCheckable(1);
            fri->setChecked( npcData.friendly );
            fri->deleteLater();

        QAction *stat = ItemMenu->addAction(tr("Not movable"));
            stat->setCheckable(1);
            stat->setChecked( npcData.nomove );
            stat->deleteLater();


        QAction *msg = ItemMenu->addAction(tr("Set message..."));
            msg->deleteLater();

        ItemMenu->addSeparator()->deleteLater();;

        QAction *boss = ItemMenu->addAction(tr("Set as Boss"));
            boss->setCheckable(1);
            boss->setChecked( npcData.legacyboss );

        ItemMenu->addSeparator()->deleteLater();;

        QAction *copyNpc = ItemMenu->addAction(tr("Copy"));
            copyNpc->deleteLater();
        QAction *cutNpc = ItemMenu->addAction(tr("Cut"));
            cutNpc->deleteLater();
        ItemMenu->addSeparator()->deleteLater();;
        QAction *remove = ItemMenu->addAction(tr("Remove"));
            remove->deleteLater();
        ItemMenu->addSeparator()->deleteLater();;
        QAction *props = ItemMenu->addAction(tr("Properties..."));
            props->deleteLater();

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
            //scene->doCut = true ;
            MainWinConnect::pMainWin->on_actionCut_triggered();
            scene->contextMenuOpened = false;
        }
        else
        if(selected==copyNpc)
        {
            //scene->doCopy = true ;
            MainWinConnect::pMainWin->on_actionCopy_triggered();
            scene->contextMenuOpened = false;
        }
        else
        if(selected==newNPC){
            npcedit *child = MainWinConnect::pMainWin->createNPCChild();
            child->newFile(npcData.id);
            child->show();
        }
        else
        if(selected==fri)
        {
            //apply to all selected items.
            LevelData selData;
            foreach(QGraphicsItem * SelItem, scene->selectedItems() )
            {
                if(SelItem->data(0).toString()=="NPC")
                {
                    selData.npc.push_back(((ItemNPC *) SelItem)->npcData);
                    ((ItemNPC *) SelItem)->setFriendly(fri->isChecked());
                }
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
                {
                    selData.npc.push_back(((ItemNPC *) SelItem)->npcData);
                    ((ItemNPC *) SelItem)->setNoMovable(stat->isChecked());
                }
            }
            scene->addChangeSettingsHistory(selData, LvlScene::SETTING_NOMOVEABLE, QVariant(stat->isChecked()));
            scene->contextMenuOpened = false;
        }
        else
            if(selected==msg)
            {
                LevelData selData;

                ItemMsgBox * msgBox = new ItemMsgBox(npcData.msg);
                msgBox->setWindowFlags (Qt::Window | Qt::WindowTitleHint | Qt::WindowCloseButtonHint);
                msgBox->setGeometry(QStyle::alignedRect(Qt::LeftToRight, Qt::AlignCenter, msgBox->size(), qApp->desktop()->availableGeometry()));
                if(msgBox->exec()==QDialog::Accepted)
                {

                    //apply to all selected items.
                    foreach(QGraphicsItem * SelItem, scene->selectedItems() )
                    {
                        if(SelItem->data(0).toString()=="NPC")
                        {
                            selData.npc.push_back(((ItemNPC *) SelItem)->npcData);
                            ((ItemNPC *) SelItem)->setMsg( msgBox->currentText );
                        }
                    }
                    scene->addChangeSettingsHistory(selData, LvlScene::SETTING_MESSAGE, QVariant(msgBox->currentText));
                }
                delete msgBox;
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
                {
                    selData.npc.push_back(((ItemNPC *) SelItem)->npcData);
                    ((ItemNPC *) SelItem)->setLegacyBoss(boss->isChecked());
                }
            }
            scene->addChangeSettingsHistory(selData, LvlScene::SETTING_BOSS, QVariant(boss->isChecked()));
            scene->contextMenuOpened = false;
        }
        else
        if(selected==setLeft)
        {
            LevelData selData;
            foreach(QGraphicsItem * SelItem, scene->selectedItems() )
            {
                if(SelItem->data(0).toString()=="NPC")
                {
                    selData.npc.push_back(((ItemNPC *) SelItem)->npcData);
                    ((ItemNPC *) SelItem)->changeDirection(-1);
                }
            }
            scene->addChangeSettingsHistory(selData, LvlScene::SETTING_DIRECTION, QVariant(-1));
            scene->contextMenuOpened = false;
        }
        if(selected==setRand)
        {
            LevelData selData;
            foreach(QGraphicsItem * SelItem, scene->selectedItems() )
            {
                if(SelItem->data(0).toString()=="NPC")
                {
                    selData.npc.push_back(((ItemNPC *) SelItem)->npcData);
                    ((ItemNPC *) SelItem)->changeDirection(0);
                }
            }
            scene->addChangeSettingsHistory(selData, LvlScene::SETTING_DIRECTION, QVariant(0));
            scene->contextMenuOpened = false;
        }
        if(selected==setRight)
        {
            LevelData selData;
            foreach(QGraphicsItem * SelItem, scene->selectedItems() )
            {
                if(SelItem->data(0).toString()=="NPC")
                {
                    selData.npc.push_back(((ItemNPC *) SelItem)->npcData);
                    ((ItemNPC *) SelItem)->changeDirection(1);
                }
            }
            scene->addChangeSettingsHistory(selData, LvlScene::SETTING_DIRECTION, QVariant(1));
            scene->contextMenuOpened = false;
        }
        else
        if(selected==remove)
        {
            bool deleted=false;
            LevelData selData;
            scene->contextMenuOpened = false; //will be disabled by remove anyway
            foreach(QGraphicsItem * SelItem, scene->selectedItems() )
            {
                if(SelItem->data(0).toString()=="NPC")
                {
                    selData.npc.push_back(((ItemNPC *) SelItem)->npcData);
                    ((ItemNPC *)SelItem)->removeFromArray();
                    scene->removeItem(SelItem);
                    delete SelItem;
                    deleted = true;
                }
            }
            //as this object isn't valid anymore we need to use MainWinConnect
            if(deleted) MainWinConnect::pMainWin->activeLvlEditWin()->scene->addRemoveHistory(selData);
        }
        else
        if(selected==props)
        {
            scene->openProps();
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

                    //scene->SyncLayerList=true; //Refresh layer list
                    MainWinConnect::pMainWin->setLayerToolsLocked(true);
                    MainWinConnect::pMainWin->setLayersBox();
                    MainWinConnect::pMainWin->setLayerToolsLocked(false);
                }
                delete layerBox;
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
                LevelData modData;
                foreach(LevelLayers lr, scene->LvlData->layers)
                { //Find layer's settings
                    if(lr.name==lName)
                    {
                        foreach(QGraphicsItem * SelItem, scene->selectedItems() )
                        {

                            if(SelItem->data(0).toString()=="NPC")
                            {
                                modData.npc.push_back(((ItemNPC*) SelItem)->npcData);
                                ((ItemNPC *) SelItem)->npcData.layer = lr.name;
                                ((ItemNPC *) SelItem)->setVisible(!lr.hidden);
                                ((ItemNPC *) SelItem)->arrayApply();
                            }
                        }
                        if(selected==newLayer){
                            scene->addChangedNewLayerHistory(modData, lr);
                        }
                        break;
                    }
                }//Find layer's settings
                if(selected!=newLayer){
                    scene->addChangedLayerHistory(modData, lName);
                }
                scene->contextMenuOpened = false;
            }
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

    //AnimationStop();

    setAnimation(localProps.frames, localProps.framespeed, localProps.framestyle, dir,
    localProps.custom_animate,
        localProps.custom_ani_fl,
        localProps.custom_ani_el,
        localProps.custom_ani_fr,
        localProps.custom_ani_er,
    true);
    arrayApply();
}

void ItemNPC::setIncludedNPC(int npcID, bool init)
{
    if(DisableScene)
        return;

    if(includedNPC!=NULL)
    {
        grp->removeFromGroup(includedNPC);
        scene->removeItem(includedNPC);
        delete includedNPC;
        includedNPC = NULL;
    }
    if(npcID==0)
    {
        if(!init) npcData.special_data = 0;
        if(!init) arrayApply();
        return;
    }

    QPixmap npcImg = QPixmap( scene->getNPCimg( npcID ) );
    includedNPC = scene->addPixmap( npcImg );


    //Default included NPC pos
    includedNPC->setPos(
                (
                    this->scenePos().x()+qreal((qreal(localProps.width)-qreal(npcImg.width()))/qreal(2))
                 ),
                (
                    (scene->pConfigs->marker_npc.buried == npcData.id)?
                       this->scenePos().y()
                      :this->scenePos().y()+qreal((qreal(localProps.height)-qreal(npcImg.height()))/qreal(2))
                 ));

    if(scene->pConfigs->marker_npc.bubble != npcData.id)
    {
        includedNPC->setOpacity(qreal(0.4));
        includedNPC->setZValue(this->zValue() + 0.0000010);
    }
    else
        includedNPC->setZValue(this->zValue() - 0.0000010);
    grp->addToGroup(includedNPC);

    if(!init) npcData.special_data = npcID;
    if(!init) arrayApply();

}

void ItemNPC::setGenerator(bool enable, int direction, int type, bool init)
{
    if(DisableScene)
        return;

    if(!init) npcData.generator = enable;

    if(generatorArrow!=NULL)
    {
        grp->removeFromGroup(generatorArrow);
        scene->removeItem(generatorArrow);
        free(generatorArrow);
        generatorArrow = NULL;
    }

    if(!enable)
    {
        if(!init) arrayApply();
        gridSize = localProps.grid;
        return;
    }
    else
    {
        generatorArrow = new QGraphicsPixmapItem;

        switch(type)
        {
        case 2:
            generatorArrow->setPixmap( QPixmap(":/npc/proj.png") );
            break;
        case 1:
        default:
            generatorArrow->setPixmap( QPixmap(":/npc/warp.png") );
            break;
        }
        if(!init) npcData.generator_type = type;

        scene->addItem( generatorArrow );

        gridSize = 16;

        generatorArrow->setOpacity(qreal(0.6));

        QPointF offset=QPoint(0,0);

        switch(direction)
        {
        case 2:
            generatorArrow->setRotation(270);
            offset.setY(32);
            if(!init) npcData.generator_direct = 2;
            break;
        case 3:
            generatorArrow->setRotation(180);
            offset.setX(32);
            offset.setY(32);
            if(!init) npcData.generator_direct = 3;
            break;
        case 4:
            generatorArrow->setRotation(90);
            offset.setX(32);
            if(!init) npcData.generator_direct = 4;
            break;
        case 1:
        default:
            generatorArrow->setRotation(0);
            if(!init) npcData.generator_direct = 1;
            break;
        }

        generatorArrow->setZValue(this->zValue() + 0.0000015);

        //Default Generator arrow NPC pos
        generatorArrow->setPos(
                    (
                        offset.x()+this->scenePos().x()+qreal((qreal(localProps.width) - qreal(32))/qreal(2))
                     ),
                    (
                        offset.y()+this->scenePos().y()+qreal((qreal(localProps.height) - qreal(32))/qreal(2))
                     ));

        grp->addToGroup( generatorArrow );

        if(!init) arrayApply();
    }
}

void ItemNPC::setLayer(QString layer)
{
    foreach(LevelLayers lr, scene->LvlData->layers)
    {
        if(lr.name==layer)
        {
            npcData.layer = layer;
            this->setVisible(!lr.hidden);
            arrayApply();
        break;
        }
    }
}

///////////////////MainArray functions/////////////////////////////
void ItemNPC::arrayApply()
{
    if(DisableScene)
        return;

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
    if(DisableScene)
        return;

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
    double BurriedOffset = 0;

    if(!DisableScene)
        BurriedOffset=(((scene->pConfigs->marker_npc.buried == npcData.id)&&(localProps.gfx_offset_y==0))? (double)localProps.gfx_h : 0 );

    imgOffsetX = (int)round( - ( ( (double)localProps.gfx_w - (double)localProps.width ) / 2 ) );
    imgOffsetY = (int)round( - (double)localProps.gfx_h + (double)localProps.height + (double)localProps.gfx_offset_y
                             - BurriedOffset);

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
    grp = new QGraphicsItemGroup(this);
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

    bool refreshFrames = false;
    if(localProps.gfx_h!=frameSize) refreshFrames = true;

    frameSize = localProps.gfx_h;

    frameWidth = localProps.gfx_w;

    frameHeight = mainImage.height();

    //framePos = QPoint(0,0);
    //draw();

    int dir=direction;

    if(direction==0) //if direction=random
    {
        dir=((0==qrand()%2)?-1:1); //set randomly 1 or -1
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
            frameLast = framesQ-1;
            break;
        }
    }

    curDirect  = dir;
    setOffset(imgOffsetX+(-((double)localProps.gfx_offset_x)*curDirect), imgOffsetY );

    if(!edit)
    {
        if(timer) delete timer;
        timer = new QTimer(this);
        connect(
                    timer, SIGNAL(timeout()),
                    this,
                    SLOT( nextFrame() ) );
    }

    if(refreshFrames) createAnimationFrames();

    setFrame(frameFirst);
}

void ItemNPC::createAnimationFrames()
{
    frames.clear();
    for(int i=0; (frameSize*i <= frameHeight); i++)
    {
        frames.push_back( mainImage.copy(QRect(framePos.x(), frameSize*i, frameWidth, frameSize )) );
    }
}

void ItemNPC::AnimationStart()
{
    if(!animated) return;
    if((frameLast>0)&&((frameLast-frameFirst)<=0)) return; //Don't start singleFrame animation

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
    //currentImage =  mainImage.copy(QRect(framePos.x(), framePos.y(), frameWidth, frameSize ));
}

QPoint ItemNPC::fPos() const
{
    return framePos;
}

void ItemNPC::setFrame(int y)
{
    if(frames.isEmpty()) return;
    //frameCurrent = frameSize * y;
    CurrentFrame = y;
    //Out of range protection
    if( CurrentFrame >= frames.size()) CurrentFrame = (frameFirst<frames.size()) ? frameFirst : 0;
    if( CurrentFrame < frameFirst) CurrentFrame = (frameLast<0)? frames.size()-1 : frameLast;

    this->setPixmap(frames[CurrentFrame]);
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
        //frameCurrent += frameSize * frameStep;
        CurrentFrame += frameStep;

        if ( ((CurrentFrame >= frames.size()-(frameStep-1) )&&(frameLast<=-1)) ||
             ((CurrentFrame > frameLast )&&(frameLast>=0)) )
            {
                if(!aniBiDirect)
                {
                     CurrentFrame = frameFirst;
                    //frameCurrent = frameFirst * frameSize;
                    //framePos.setY( frameFirst * frameSize );
                }
                else
                {
                    CurrentFrame -= frameStep*2;
                    aniDirect=!aniDirect;
                    //framePos.setY( framePos.y() - frameSize*frameStep );
                }
            }
    }
    else
    {
        //frameCurrent -= frameSize * frameStep;
        CurrentFrame -= frameStep;

        if ( CurrentFrame < frameFirst )
            {
                if(!aniBiDirect)
                {
                    CurrentFrame = ((frameLast==-1)? frames.size()-1 : frameLast);
                    //frameCurrent = ( ((frameLast==-1)? frameHeight : frameLast*frameSize)-frameSize);
                    //framePos.setY( ((frameLast==-1) ? frameHeight : frameLast*frameSize)-frameSize );
                }
                else
                {
                    CurrentFrame+=frameStep*2;
                    aniDirect=!aniDirect;
                    //frameCurrent += frameSize*frameStep*2;
                    //framePos.setY( framePos.y() + frameSize*frameStep );
                }
            }
    }
    setFrame(CurrentFrame);
}
