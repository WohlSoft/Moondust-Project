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

#include "item_playerpoint.h"
#include "../common_features/themes.h"

ItemPlayerPoint::ItemPlayerPoint(QGraphicsItem *parent) :
    QGraphicsPixmapItem(parent)
{
    scene=NULL;
    mouseLeft=false;
    mouseMid=false;
    mouseRight=false;
}


void ItemPlayerPoint::mousePressEvent(QGraphicsSceneMouseEvent *mouseEvent)
{
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

void ItemPlayerPoint::mouseReleaseEvent(QGraphicsSceneMouseEvent *mouseEvent)
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

    QGraphicsPixmapItem::mouseReleaseEvent(mouseEvent);

    if((callContext)&&(!scene->contextMenuOpened))
    {
        if(!scene->DrawMode)
        {
            scene->contextMenuOpened = true; //bug protector
        }

        //Remove selection from non-block items
        if(!this->isSelected())
        {
            scene->clearSelection();
            this->setSelected(true);
        }

        this->setSelected(1);
        ItemMenu.clear();

        bool isLvlx = !scene->LvlData->smbx64strict;

        QMenu * chDir = ItemMenu.addMenu(
                    tr("Set %1").arg(tr("Direction")) );
        chDir->deleteLater();
        chDir->setEnabled(isLvlx);

        QAction *setLeft = chDir->addAction( tr("Left"));
            setLeft->setCheckable(true);
            setLeft->setChecked(pointData.direction==-1);
            setLeft->deleteLater();

        QAction *setRight = chDir->addAction( tr("Right") );
            setRight->setCheckable(true);
            setRight->setChecked(pointData.direction==1);
            setRight->deleteLater();

        ItemMenu.addSeparator()->deleteLater();

        QAction * remove = ItemMenu.addAction(tr("Remove"));
        remove->deleteLater();

        QAction *selected = ItemMenu.exec(mouseEvent->screenPos());

                if(!selected)
                {
                    WriteToLog(QtDebugMsg, "Context Menu <- NULL");
                    return;
                }

                if(selected==remove)
                {
                    scene->removeSelectedLvlItems();
                }
                else
                if(selected==setLeft)
                {
                    //LevelData selData;
                    foreach(QGraphicsItem * SelItem, scene->selectedItems() )
                    {
                        if(SelItem->data(0).toString()=="playerPoint")
                        {
                            //selData.npc.push_back(((ItemNPC *) SelItem)->npcData);
                            dynamic_cast<ItemPlayerPoint *>(SelItem)->changeDirection(-1);
                        }
                    }
                    //scene->addChangeSettingsHistory(selData, LvlScene::SETTING_DIRECTION, QVariant(-1));
                }
                else
                if(selected==setRight)
                {
                    //LevelData selData;
                    foreach(QGraphicsItem * SelItem, scene->selectedItems() )
                    {
                        if(SelItem->data(0).toString()=="playerPoint")
                        {
                            //selData.npc.push_back(((ItemPlayerPoint *) SelItem)->npcData);
                            dynamic_cast<ItemPlayerPoint *>(SelItem)->changeDirection(1);
                        }
                    }
                    //scene->addChangeSettingsHistory(selData, LvlScene::SETTING_DIRECTION, QVariant(1));
                }
    }

}


void ItemPlayerPoint::changeDirection(int dir)
{
    pointData.direction = dir;

    QPixmap mirrowed=QPixmap::fromImage(currentImage.toImage().mirrored(true,false));
    if(pointData.direction<0)
        setPixmap(mirrowed);
    else
        setPixmap(currentImage);

    arrayApply();
}

void ItemPlayerPoint::setPointData(PlayerPoint pnt, bool init)
{
    pointData = pnt;

    if(!init)
    {
        bool found=false;
        int q=0;
        for(q=0; q < scene->LvlData->players.size();q++)
        {
             if(scene->LvlData->players[q].id == pointData.id)
             {
                 found=true;
                 break;
             }
        }

        if(!found)
        {
            q = scene->LvlData->players.size();
            scene->LvlData->players.push_back(pointData);
        }
        else
            scene->LvlData->players[q]=pointData;
    }

    this->setPos(pointData.x, pointData.y);
    this->setZValue(scene->Z_Player);
    this->setData(0, "playerPoint");
    this->setData(2, QString::number(pointData.id));
    this->setFlag(QGraphicsItem::ItemIsSelectable, true);
    this->setFlag(QGraphicsItem::ItemIsMovable, true);

    switch(pointData.id)
    {
    case 1:
        currentImage = Themes::Image(Themes::player1); break;
    case 2:
        currentImage = Themes::Image(Themes::player2); break;
    default:
        currentImage = Themes::Image(Themes::player_point); break;
    }

    this->setOffset(qRound(qreal(pnt.w-currentImage.width())/2.0), pnt.h-currentImage.height() );

    QPixmap mirrowed=QPixmap::fromImage(currentImage.toImage().mirrored(true,false));
    if(pointData.direction<0)
        setPixmap(mirrowed);
    else
        setPixmap(currentImage);

}



void ItemPlayerPoint::arrayApply()
{

    pointData.x = qRound(this->scenePos().x());
    pointData.y = qRound(this->scenePos().y());

    for(int i=0; i<scene->LvlData->players.size();i++)
    {
        if(pointData.id==scene->LvlData->players[i].id)
        {
            scene->LvlData->players[i] = pointData;
            break;
        }
    }

}

void ItemPlayerPoint::removeFromArray()
{
    for(int i=0; i<scene->LvlData->players.size();i++)
    {
        if(pointData.id==scene->LvlData->players[i].id)
        {
            scene->LvlData->players.remove(i);
            break;
        }
    }
}

void ItemPlayerPoint::setScenePoint(LvlScene *theScene)
{
    scene = theScene;
}
