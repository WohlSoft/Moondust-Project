/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2014-2015 Vitaly Novichkov <admin@wohlnet.ru>
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

#include <common_features/mainwinconnect.h>
#include <common_features/logger.h>
#include <editing/_dialogs/itemselectdialog.h>
#include <PGE_File_Formats/file_formats.h>
#include <common_features/util.h>

#include "item_block.h"
#include "item_bgo.h"
#include "item_npc.h"
#include "item_water.h"
#include "item_door.h"
#include "../itemmsgbox.h"
#include "../newlayerbox.h"

ItemNPC::ItemNPC(bool noScene, QGraphicsPixmapItem *parent)
    : QGraphicsPixmapItem(parent)
{
    DisableScene = noScene;
    construct();
}

ItemNPC::ItemNPC(LvlScene *parentScene, QGraphicsPixmapItem *parent)
    : QGraphicsPixmapItem(parent)
{
    DisableScene=false;
    construct();
    if(!parentScene) return;
    setScenePoint(parentScene);
    scene->addItem(this);
    setLocked(scene->lock_npc);
}

void ItemNPC::construct()
{
    setShapeMode(QGraphicsPixmapItem::BoundingRectShape);
    generatorArrow = NULL;
    includedNPC = NULL;
    animated = false;
    aniDirect=false;
    aniBiDirect=false;
    curDirect = -1;
    frameStep = 1;
    gridSize = 1;
    frameSize=1;

    extAnimator=false;
    animatorID=-1;
    imageSize = QRectF(0,0,10,10);

    CurrentFrame=0; //Real frame
    frameCurrent=0; //Timer frame

    imgOffsetX=0;
    imgOffsetY=0;

    frameFirst=0; //from first frame
    frameLast=-1; //to unlimited frameset
    //image = new QGraphicsPixmapItem;

    isLocked=false;

    timer=NULL;

    mouseLeft=false;
    mouseMid=false;
    mouseRight=false;

    setData(ITEM_TYPE, "NPC"); // ObjType
    setData(ITEM_IS_ITEM, 1);
}




ItemNPC::~ItemNPC()
{
    if(includedNPC!=NULL) delete includedNPC;
    if(grp!=NULL) delete grp;
    if(timer) delete timer;
    if(!DisableScene) scene->unregisterElement(this);
}



void ItemNPC::mousePressEvent ( QGraphicsSceneMouseEvent * mouseEvent )
{
    if((this->flags()&QGraphicsItem::ItemIsSelectable)==0)
    {
        QGraphicsItem::mousePressEvent(mouseEvent); return;
    }

    if(!DisableScene)
        if(scene->DrawMode)
        {
            unsetCursor();
            ungrabMouse();
            this->setSelected(false);
            return;
        }

    //Discard multi-mouse keys
    if((mouseLeft)||(mouseMid)||(mouseRight))
    {
        mouseEvent->accept();
        return;
    }

    if( mouseEvent->buttons() & Qt::LeftButton )
        mouseLeft=true;
    if( mouseEvent->buttons() & Qt::MiddleButton )
        mouseMid=true;
    if( mouseEvent->buttons() & Qt::RightButton )
        mouseRight=true;

    QGraphicsPixmapItem::mousePressEvent(mouseEvent);
}

void ItemNPC::mouseReleaseEvent(QGraphicsSceneMouseEvent *mouseEvent)
{
    int multimouse=0;
    bool callContext=false;
    if(((mouseMid)||(mouseRight))&&( mouseLeft^(mouseEvent->buttons() & Qt::LeftButton) ))
        multimouse++;
    if( (((mouseLeft)||(mouseRight)))&&( mouseMid^(mouseEvent->buttons() & Qt::MiddleButton) ))
        multimouse++;
    if((((mouseLeft)||(mouseMid)))&&( mouseRight^(mouseEvent->buttons() & Qt::RightButton) ))
        multimouse++;
    if(multimouse>0)
    {
        mouseEvent->accept(); return;
    }

    if( mouseLeft^(mouseEvent->buttons() & Qt::LeftButton) )
        mouseLeft=false;

    if( mouseMid^(mouseEvent->buttons() & Qt::MiddleButton) )
        mouseMid=false;

    if( mouseRight^(mouseEvent->buttons() & Qt::RightButton) )
    {
        if(!scene->IsMoved) callContext=true;
        mouseRight=false;
    }

    QGraphicsItem::mouseReleaseEvent(mouseEvent);

    if(DisableScene) return;
    /////////////////////////CONTEXT MENU:///////////////////////////////
    if((callContext)&&(!scene->contextMenuOpened))
    {
        if((!scene->lock_npc)&&(!scene->DrawMode)&&(!isLocked))
        {
            contextMenu(mouseEvent);
        }
    }
}

void ItemNPC::contextMenu( QGraphicsSceneMouseEvent * mouseEvent )
{
    scene->contextMenuOpened = true; //bug protector

    //Remove selection from non-block items
    if(!this->isSelected())
    {
        scene->clearSelection();
        this->setSelected(true);
    }

    this->setSelected(1);
    QMenu ItemMenu;

    QMenu * LayerName = ItemMenu.addMenu(tr("Layer: ")+QString("[%1]").arg(npcData.layer).replace("&", "&&&"));

    QAction *setLayer;
    QList<QAction *> layerItems;

    QAction * newLayer = LayerName->addAction(tr("Add to new layer..."));
    LayerName->addSeparator()->deleteLater();

    foreach(LevelLayer layer, scene->LvlData->layers)
    {
        //Skip system layers
        if((layer.name=="Destroyed Blocks")||(layer.name=="Spawned NPCs")) continue;

        setLayer = LayerName->addAction( layer.name.replace("&", "&&&")+((layer.hidden)?" [hidden]":"") );
        setLayer->setData(layer.name);
        setLayer->setCheckable(true);
        setLayer->setEnabled(true);
        setLayer->setChecked( layer.name==npcData.layer );
        layerItems.push_back(setLayer);
    }
    ItemMenu.addSeparator()->deleteLater();
    QString NPCpath1 = scene->LvlData->path+QString("/npc-%1.txt").arg( npcData.id );
    QString NPCpath2 = scene->LvlData->path+"/"+scene->LvlData->filename+QString("/npc-%1.txt").arg( npcData.id );

    QAction *newNpc;
        if( (!scene->LvlData->untitled)&&((QFile().exists(NPCpath2)) || (QFile().exists(NPCpath1))) )
            newNpc = ItemMenu.addAction(tr("Edit NPC-Configuration"));
        else
            newNpc = ItemMenu.addAction(tr("New NPC-Configuration"));
        newNpc->setEnabled(!scene->LvlData->untitled);
        ItemMenu.addSeparator();

    QMenu * chDir = ItemMenu.addMenu(
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
            ItemMenu.addSeparator();
    QAction *fri = ItemMenu.addAction(tr("Friendly"));
        fri->setCheckable(1);
        fri->setChecked( npcData.friendly );
    QAction *stat = ItemMenu.addAction(tr("Doesn't move"));
        stat->setCheckable(1);
        stat->setChecked( npcData.nomove );
    QAction *msg = ItemMenu.addAction(tr("Set message..."));
        ItemMenu.addSeparator();
    QAction *boss = ItemMenu.addAction(tr("Set as Boss"));
        boss->setCheckable(1);
        boss->setChecked( npcData.is_boss );
        ItemMenu.addSeparator();
    QAction *transform = ItemMenu.addAction(tr("Transform into"));
    QAction *transform_all_s = ItemMenu.addAction(tr("Transform all %1 in this section into").arg("NPC-%1").arg(npcData.id));
    QAction *transform_all = ItemMenu.addAction(tr("Transform all %1 into").arg("NPC-%1").arg(npcData.id));
        ItemMenu.addSeparator();
    QAction *copyNpc = ItemMenu.addAction(tr("Copy"));
    QAction *cutNpc = ItemMenu.addAction(tr("Cut"));
        ItemMenu.addSeparator();
    QAction *remove = ItemMenu.addAction(tr("Remove"));
        ItemMenu.addSeparator();
    QAction *props = ItemMenu.addAction(tr("Properties..."));


QAction *selected = ItemMenu.exec(mouseEvent->screenPos());

    if(!selected)
    {
        WriteToLog(QtDebugMsg, "Context Menu <- NULL");
        return;
    }

    if(selected==cutNpc)
    {
        //scene->doCut = true ;
        MainWinConnect::pMainWin->on_actionCut_triggered();
    }
    else
    if(selected==copyNpc)
    {
        //scene->doCopy = true ;
        MainWinConnect::pMainWin->on_actionCopy_triggered();
    }
    else
    if((selected==transform)||(selected==transform_all)||(selected==transform_all_s))
    {
        LevelData oldData;
        LevelData newData;

        int transformTO;
        ItemSelectDialog * npcList = new ItemSelectDialog(scene->pConfigs, ItemSelectDialog::TAB_NPC,0,0,0,0,0,0,0,0,0,MainWinConnect::pMainWin);
        npcList->removeEmptyEntry(ItemSelectDialog::TAB_NPC);
        util::DialogToCenter(npcList, true);

        if(npcList->exec()==QDialog::Accepted)
        {
            QList<QGraphicsItem *> our_items;
            bool sameID=false;
            transformTO = npcList->npcID;
            unsigned long oldID = npcData.id;

            if(selected==transform)
                our_items=scene->selectedItems();
            else
            if(selected==transform_all)
            {
                our_items=scene->items();
                sameID=true;
            }
            else if(selected==transform_all_s)
            {
                bool ok=false;
                long mg = QInputDialog::getInt(NULL, tr("Margin of section"),
                               tr("Please select, how far items out of section should be removed too (in pixels)"),
                               32, 0, 214948, 1, &ok);
                if(!ok) goto cancelTransform;
                LevelSection &s=scene->LvlData->sections[scene->LvlData->CurSection];
                QRectF section;
                section.setLeft(s.size_left-mg);
                section.setTop(s.size_top-mg);
                section.setRight(s.size_right+mg);
                section.setBottom(s.size_bottom+mg);
                our_items=scene->items(section, Qt::IntersectsItemShape);
                sameID=true;
            }

            foreach(QGraphicsItem * SelItem, our_items )
            {
                if(SelItem->data(ITEM_TYPE).toString()=="NPC")
                {
                    if((!sameID)||(((ItemNPC *) SelItem)->npcData.id==oldID))
                    {
                        oldData.npc.push_back( ((ItemNPC *) SelItem)->npcData );
                        ((ItemNPC *) SelItem)->transformTo(transformTO);
                        newData.npc.push_back( ((ItemNPC *) SelItem)->npcData );
                    }
                }
            }
            cancelTransform:;
        }
        delete npcList;
        if(!newData.npc.isEmpty())
            scene->addTransformHistory(newData, oldData);
    }
    else
    if(selected==newNpc){
        //QString path1 = scene->LvlData->path+QString("/npc-%1.txt").arg( npcData.id );
        //QString path2 = scene->LvlData->path+"/"+scene->LvlData->filename+QString("/npc-%1.txt").arg( npcData.id );

        WriteToLog(QtDebugMsg, QString("NPC.txt path 1: %1").arg(NPCpath1));
        WriteToLog(QtDebugMsg, QString("NPC.txt path 2: %1").arg(NPCpath2));
        if( (!scene->LvlData->untitled) && (QFileInfo( NPCpath2 ).exists()) )
        {
            MainWinConnect::pMainWin->OpenFile( NPCpath2 );
        }
        else
        if( (!scene->LvlData->untitled) && (QFileInfo( NPCpath1 ).exists()) )
        {
            MainWinConnect::pMainWin->OpenFile( NPCpath1 );
        }
        else
        {
            NpcEdit *child = MainWinConnect::pMainWin->createNPCChild();
            child->newFile(npcData.id);
            child->show();
        }
    }
    else
    if(selected==fri)
    {
        //apply to all selected items.
        LevelData selData;
        foreach(QGraphicsItem * SelItem, scene->selectedItems() )
        {
            if(SelItem->data(ITEM_TYPE).toString()=="NPC")
            {
                selData.npc.push_back(((ItemNPC *) SelItem)->npcData);
                ((ItemNPC *) SelItem)->setFriendly(fri->isChecked());
            }
        }
        scene->addChangeSettingsHistory(selData, HistorySettings::SETTING_FRIENDLY, QVariant(fri->isChecked()));
    }
    else
    if(selected==stat)
    {
        //apply to all selected items.
        LevelData selData;
        foreach(QGraphicsItem * SelItem, scene->selectedItems() )
        {
            if(SelItem->data(ITEM_TYPE).toString()=="NPC")
            {
                selData.npc.push_back(((ItemNPC *) SelItem)->npcData);
                ((ItemNPC *) SelItem)->setNoMovable(stat->isChecked());
            }
        }
        scene->addChangeSettingsHistory(selData, HistorySettings::SETTING_NOMOVEABLE, QVariant(stat->isChecked()));
    }
    else
        if(selected==msg)
        {
            LevelData selData;

            ItemMsgBox msgBox(Opened_By::NPC, npcData.msg, npcData.friendly, "", "", MainWinConnect::pMainWin);
            util::DialogToCenter(&msgBox, true);
            if(msgBox.exec()==QDialog::Accepted)
            {

                //apply to all selected items.
                foreach(QGraphicsItem * SelItem, scene->selectedItems() )
                {
                    if(SelItem->data(ITEM_TYPE).toString()=="NPC")
                    {
                        selData.npc.push_back(((ItemNPC *) SelItem)->npcData);
                        ((ItemNPC *) SelItem)->setMsg( msgBox.currentText );
                        ((ItemNPC *) SelItem)->setFriendly( msgBox.isFriendlyChecked() );
                    }
                }
                scene->addChangeSettingsHistory(selData, HistorySettings::SETTING_MESSAGE, QVariant(msgBox.currentText));
                scene->addChangeSettingsHistory(selData, HistorySettings::SETTING_FRIENDLY, QVariant(msgBox.isFriendlyChecked()));
            }
            //delete msgBox;
        }
        else
    if(selected==boss)
    {
        //apply to all selected items.
        LevelData selData;
        foreach(QGraphicsItem * SelItem, scene->selectedItems() )
        {
            if(SelItem->data(ITEM_TYPE).toString()=="NPC")
            {
                selData.npc.push_back(((ItemNPC *) SelItem)->npcData);
                ((ItemNPC *) SelItem)->setLegacyBoss(boss->isChecked());
            }
        }
        scene->addChangeSettingsHistory(selData, HistorySettings::SETTING_BOSS, QVariant(boss->isChecked()));
    }
    else
    if(selected==setLeft)
    {
        LevelData selData;
        foreach(QGraphicsItem * SelItem, scene->selectedItems() )
        {
            if(SelItem->data(ITEM_TYPE).toString()=="NPC")
            {
                selData.npc.push_back(((ItemNPC *) SelItem)->npcData);
                ((ItemNPC *) SelItem)->changeDirection(-1);
            }
        }
        scene->addChangeSettingsHistory(selData, HistorySettings::SETTING_DIRECTION, QVariant(-1));
    }
    if(selected==setRand)
    {
        LevelData selData;
        foreach(QGraphicsItem * SelItem, scene->selectedItems() )
        {
            if(SelItem->data(ITEM_TYPE).toString()=="NPC")
            {
                selData.npc.push_back(((ItemNPC *) SelItem)->npcData);
                ((ItemNPC *) SelItem)->changeDirection(0);
            }
        }
        scene->addChangeSettingsHistory(selData, HistorySettings::SETTING_DIRECTION, QVariant(0));
    }
    if(selected==setRight)
    {
        LevelData selData;
        foreach(QGraphicsItem * SelItem, scene->selectedItems() )
        {
            if(SelItem->data(ITEM_TYPE).toString()=="NPC")
            {
                selData.npc.push_back(((ItemNPC *) SelItem)->npcData);
                ((ItemNPC *) SelItem)->changeDirection(1);
            }
        }
        scene->addChangeSettingsHistory(selData, HistorySettings::SETTING_DIRECTION, QVariant(1));
    }
    else
    if(selected==remove)
    {
        scene->removeSelectedLvlItems();
    }
    else
    if(selected==props)
    {
        scene->openProps();
    }
    else
    if(selected==newLayer)
    {
        scene->setLayerToSelected();
    }
    else
    {
        //Fetch layers menu
        foreach(QAction * lItem, layerItems)
        {
            if(selected==lItem)
            {
                //FOUND!!!
                scene->setLayerToSelected(lItem->data().toString());
                break;
            }//Find selected layer's item
        }
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
    npcData.is_boss=boss;
    arrayApply();//Apply changes into array
}

void ItemNPC::changeDirection(int dir)
{
    npcData.direct = dir;

    if(!extAnimator)
    {
        setAnimation(localProps.frames, localProps.framespeed, localProps.framestyle, dir,
        localProps.custom_animate,
            localProps.custom_ani_fl,
            localProps.custom_ani_el,
            localProps.custom_ani_fr,
            localProps.custom_ani_er,
        true);
    }
    else
    {

        direction = dir;
        if(direction==0) //if direction=random
        {
            direction=((0==qrand()%2)?-1:1); //set randomly 1 or -1
        }
        curDirect = direction;

        imgOffsetX = (int)round( - ( ( (double)localProps.gfx_w - (double)localProps.width ) / 2 ) );
        imgOffsetY = (int)round( - (double)localProps.gfx_h + (double)localProps.height + (double)localProps.gfx_offset_y);

        setOffset( imgOffsetX+(-((double)localProps.gfx_offset_x)*curDirect), imgOffsetY );

        offseted = imageSize;
        offseted.setLeft(offseted.left()+this->offset().x());
        offseted.setTop(offseted.top()+this->offset().y());
        offseted.setRight(offseted.right()+this->offset().x());
        offseted.setBottom(offseted.bottom()+this->offset().y());

        update();

    }

    arrayApply();
}

void ItemNPC::transformTo(long target_id)
{
    if(!scene)
        return;

    if(target_id<1) return;

    bool noimage=true;
    long item_i=0;
    long animator=0;
    obj_npc mergedSet;

    scene->getConfig_NPC(target_id, item_i, animator, mergedSet, &noimage);
    if(noimage) return;

    npcData.id = target_id;

    setNpcData(npcData, &mergedSet, &animator);
    arrayApply();

    if(!scene->opts.animationEnabled)
        scene->update();
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
        delete generatorArrow;
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
    foreach(LevelLayer lr, scene->LvlData->layers)
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

    npcData.x = qRound(this->scenePos().x());
    npcData.y = qRound(this->scenePos().y());

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

    //Update R-tree innex
    scene->unregisterElement(this);
    scene->registerElement(this);
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
        scene->LvlData->npc.removeAt(npcData.index);
    }
    else
    for(int i=0; i<scene->LvlData->npc.size(); i++)
    {
        if(scene->LvlData->npc[i].array_id == npcData.array_id)
        {
            scene->LvlData->npc.removeAt(i); break;
        }
    }
}


void ItemNPC::returnBack()
{
    this->setPos(npcData.x, npcData.y);
}

QPoint ItemNPC::gridOffset()
{
    return QPoint(localProps.grid_offset_x, localProps.grid_offset_y);
}

int ItemNPC::getGridSize()
{
    return gridSize;
}

QPoint ItemNPC::sourcePos()
{
    return QPoint(npcData.x, npcData.y);
}


void ItemNPC::setMainPixmap(const QPixmap &pixmap)
{
    mainImage = pixmap;
    this->setPixmap(mainImage);
    double BurriedOffset = 0;
    imageSize = pixmap.rect();

    if(!DisableScene)
        BurriedOffset=(((scene->pConfigs->marker_npc.buried == npcData.id)&&(localProps.gfx_offset_y==0))? (double)localProps.gfx_h : 0 );

    imgOffsetX = (int)round( - ( ( (double)localProps.gfx_w - (double)localProps.width ) / 2 ) );
    imgOffsetY = (int)round( - (double)localProps.gfx_h + (double)localProps.height + (double)localProps.gfx_offset_y
                             - BurriedOffset);

    //grid_attach_style

    setOffset( imgOffsetX+(-((double)localProps.gfx_offset_x)*curDirect), imgOffsetY );

    offseted = imageSize;
    offseted.setLeft(offseted.left()+this->offset().x());
    offseted.setTop(offseted.top()+this->offset().y());
    offseted.setRight(offseted.right()+this->offset().x());
    offseted.setBottom(offseted.bottom()+this->offset().y());
}

void ItemNPC::setNpcData(LevelNPC inD, obj_npc *mergedSet, long *animator_id)
{
    npcData = inD;
    if(!scene) return;

    if(mergedSet)
    {
        localProps = (*mergedSet);
        if(localProps.foreground)
            setZValue(scene->Z_npcFore);
        else
        if(localProps.background)
            setZValue(scene->Z_npcBack);
        else
            setZValue(scene->Z_npcStd);

        if((localProps.container)&&(npcData.special_data>0))
            setIncludedNPC(npcData.special_data, true);

        npcData.is_star = localProps.is_star;
    }

    if(animator_id)
        setAnimator(*animator_id);

    setPos( npcData.x, npcData.y );

    changeDirection(npcData.direct);

    setGenerator(npcData.generator,
                 npcData.generator_direct,
                 npcData.generator_type, true);

    setData(ITEM_ID, QString::number(npcData.id) );
    setData(ITEM_ARRAY_ID, QString::number(npcData.array_id) );

    setData(ITEM_NPC_BLOCK_COLLISION,  QString::number((int)localProps.collision_with_blocks) );
    setData(ITEM_NPC_NO_NPC_COLLISION, QString::number((int)localProps.no_npc_collions) );

    setData(ITEM_WIDTH,  QString::number(localProps.width) ); //width
    setData(ITEM_HEIGHT, QString::number(localProps.height) ); //height

    scene->unregisterElement(this);
    scene->registerElement(this);
}


QRectF ItemNPC::boundingRect() const
{
    if(!animated)
        return QRectF(0+imgOffsetX+(-((double)localProps.gfx_offset_x)*curDirect), 0+imgOffsetY, imageSize.width(), imageSize.height());
    else
        return QRectF(0+imgOffsetX+(-((double)localProps.gfx_offset_x)*curDirect), 0+imgOffsetY, localProps.gfx_w, localProps.gfx_h);
}


void ItemNPC::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    if(!extAnimator)
    {
        QGraphicsPixmapItem::paint(painter,option, widget); return;
    }

    if(animatorID<0)
    {
        painter->drawRect(QRect(imgOffsetX,imgOffsetY,1,1));
        return;
    }

    if(scene->animates_NPC.size()>animatorID)
        painter->drawPixmap(offseted, scene->animates_NPC[animatorID]->image(curDirect), scene->animates_NPC[animatorID]->image(curDirect).rect());
    else
        painter->drawRect(QRect(0,0,32,32));

    if(this->isSelected())
    {
        painter->setPen(QPen(QBrush(Qt::black), 2, Qt::SolidLine));
        painter->drawRect(offseted.x()+1,imgOffsetY+1,imageSize.width()-2,imageSize.height()-2);
        painter->setPen(QPen(QBrush(Qt::magenta), 2, Qt::DotLine));
        painter->drawRect(offseted.x()+1/*imgOffsetX+1+(offset().x()/2)*/,imgOffsetY+1,imageSize.width()-2,imageSize.height()-2);
    }

}

void ItemNPC::setScenePoint(LvlScene *theScene)
{
    scene = theScene;
    grp = new QGraphicsItemGroup(this);
}


void ItemNPC::setAnimator(long aniID)
{
    if(DisableScene) return;

    if(aniID<scene->animates_NPC.size())
    imageSize = QRectF(0,0,
                scene->animates_NPC[aniID]->image(-1).width(),
                scene->animates_NPC[aniID]->image(-1).height()
                );

    this->setData(ITEM_WIDTH, QString::number(qRound(imageSize.width())) ); //width
    this->setData(ITEM_HEIGHT, QString::number(qRound(imageSize.height())) ); //height

    animatorID = aniID;
    extAnimator = true;
    animated = true;

    double BurriedOffset = 0;
    if(!DisableScene)
        BurriedOffset=(((scene->pConfigs->marker_npc.buried == npcData.id)&&(localProps.gfx_offset_y==0))? (double)localProps.gfx_h : 0 );

    imgOffsetX = (int)round( - ( ( (double)localProps.gfx_w - (double)localProps.width ) / 2 ) );
    imgOffsetY = (int)round( - (double)localProps.gfx_h + (double)localProps.height + (double)localProps.gfx_offset_y
                             - BurriedOffset);

    setPixmap(QPixmap(imageSize.width(),
                      imageSize.height()));

    setOffset( imgOffsetX+(-((double)localProps.gfx_offset_x)*curDirect), imgOffsetY );

    offseted = imageSize;
    offseted.setLeft(offseted.left()+this->offset().x());
    offseted.setTop(offseted.top()+this->offset().y());
    offseted.setRight(offseted.right()+this->offset().x());
    offseted.setBottom(offseted.bottom()+this->offset().y());
}

////////////////Animation///////////////////

void ItemNPC::setAnimation(int frames, int framespeed, int framestyle, int direct,
                           bool customAni, int frFL, int frEL, int frFR, int frER, bool edit, bool updFrames)
{
    animated = true;
    framesQ = frames;
    frameSpeed = framespeed;
    frameStyle = framestyle;
    direction = direct;
    frameStep = 1;

    frameSequance = false;

    aniBiDirect = localProps.ani_bidir;
    customAniAlg = localProps.custom_ani_alg;

    customAnimate = customAni;

    custom_frameFL = frFL;//first left
    custom_frameEL = frEL;//end left
    custom_frameFR = frFR;//first right
    custom_frameER = frER;//enf right

    bool refreshFrames = updFrames;
    if(localProps.gfx_h!=frameSize) refreshFrames = true;
    if(localProps.gfx_w!=frameWidth) refreshFrames = true;

    frameSize = localProps.gfx_h; // height of one frame
    frameWidth = localProps.gfx_w; //width of target image

    frameHeight = mainImage.height(); // Height of target image

    //Protectors
    if(frameSize<=0) frameSize=1;
    if(frameSize>mainImage.height()) frameSize = mainImage.height();

    if(frameWidth<=0) frameWidth=1;
    if(frameWidth>mainImage.width()) frameWidth = mainImage.width();

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
            case 2:
                frameSequance = true;
                frames_list = localProps.frames_left;
                frameFirst = 0;
                frameLast = frames_list.size()-1;
                break;
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
            case 2:
                frameSequance = true;
                frames_list = localProps.frames_right;
                frameFirst = 0;
                frameLast = frames_list.size()-1; break;
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
    for(int i=0; (frameSize*i < frameHeight); i++)
    {
        frames.push_back( mainImage.copy(QRect(0, frameSize*i, frameWidth, frameSize )) );
    }

    WriteToLog(QtDebugMsg, QString("NPC -> created animation frames %1").arg(frames.size()));
}

void ItemNPC::AnimationStart()
{
    if(!animated) return;
    if((frameLast>0)&&((frameLast-frameFirst)<=0)) return; //Don't start singleFrame animation

    frameCurrent = frameFirst;
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
        frameCurrent += frameStep;

        if ( ((frameCurrent >= frames.size()-(frameStep-1) )&&(frameLast<=-1)) ||
             ((frameCurrent > frameLast )&&(frameLast>=0)) )
            {
                if(!aniBiDirect)
                {
                     frameCurrent = frameFirst;
                    //frameCurrent = frameFirst * frameSize;
                    //framePos.setY( frameFirst * frameSize );
                }
                else
                {
                    frameCurrent -= frameStep*2;
                    aniDirect=!aniDirect;
                    //framePos.setY( framePos.y() - frameSize*frameStep );
                }
            }
    }
    else
    {
        //frameCurrent -= frameSize * frameStep;
        frameCurrent -= frameStep;

        if ( frameCurrent < frameFirst )
            {
                if(!aniBiDirect)
                {
                    frameCurrent = ((frameLast==-1)? frames.size()-1 : frameLast);
                    //frameCurrent = ( ((frameLast==-1)? frameHeight : frameLast*frameSize)-frameSize);
                    //framePos.setY( ((frameLast==-1) ? frameHeight : frameLast*frameSize)-frameSize );
                }
                else
                {
                    frameCurrent+=frameStep*2;
                    aniDirect=!aniDirect;
                    //frameCurrent += frameSize*frameStep*2;
                    //framePos.setY( framePos.y() + frameSize*frameStep );
                }
            }
    }
    setFrame( frameSequance ? frames_list[frameCurrent] : frameCurrent);
}
