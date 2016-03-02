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

#include <common_features/logger.h>
#include <common_features/mainwinconnect.h>
#include <common_features/graphics_funcs.h>
#include <main_window/dock/lvl_warp_props.h>

#include "item_block.h"
#include "item_bgo.h"
#include "item_npc.h"
#include "item_water.h"
#include "item_door.h"
#include "../newlayerbox.h"


ItemDoor::ItemDoor(QGraphicsItem *parent)
    : LvlBaseItem(parent)
{
    construct();
}

ItemDoor::ItemDoor(LvlScene *parentScene, QGraphicsItem *parent)
    : LvlBaseItem(parentScene, parent)
{
    construct();
    if(!parentScene) return;
    setScenePoint(parentScene);
    m_scene->addItem(this);
    setLocked(m_scene->lock_door);
}

void ItemDoor::construct()
{
    m_gridSize=16;
    m_itemSize = QSize(32,32);
    this->setData(ITEM_WIDTH, 32);
    this->setData(ITEM_HEIGHT, 32);
    m_grp = NULL;
    m_doorLabel = NULL;
}

ItemDoor::~ItemDoor()
{
    if(m_doorLabel!=NULL) delete m_doorLabel;
    if(m_grp!=NULL) delete m_grp;
    m_scene->unregisterElement(this);
}



void ItemDoor::contextMenu( QGraphicsSceneMouseEvent * mouseEvent )
{
    m_scene->contextMenuOpened = true; //bug protector

    //Remove selection from non-bgo items
    if(!this->isSelected())
    {
        m_scene->clearSelection();
        this->setSelected(true);
    }

    this->setSelected(1);
    QMenu ItemMenu;

    QAction *openLvl = ItemMenu.addAction(tr("Open target level: %1").arg(m_data.lname).replace("&", "&&&"));
    openLvl->setVisible( (!m_data.lname.isEmpty()) && (QFile(m_scene->LvlData->path + "/" + m_data.lname).exists()) );
    openLvl->deleteLater();

    /*************Layers*******************/
    QMenu * LayerName =     ItemMenu.addMenu(tr("Layer: ")+QString("[%1]").arg(m_data.layer).replace("&", "&&&"));
    QAction *setLayer;
    QList<QAction *> layerItems;

    QAction * newLayer =    LayerName->addAction(tr("Add to new layer..."));
        LayerName->addSeparator()->deleteLater();;
    foreach(LevelLayer layer, m_scene->LvlData->layers)
    {
        //Skip system layers
        if((layer.name=="Destroyed Blocks")||(layer.name=="Spawned NPCs")) continue;

        setLayer = LayerName->addAction( layer.name.replace("&", "&&&")+((layer.hidden)?""+tr("[hidden]"):"") );
        setLayer->setData(layer.name);
        setLayer->setCheckable(true);
        setLayer->setEnabled(true);
        setLayer->setChecked( layer.name==m_data.layer );
        layerItems.push_back(setLayer);
    }
    ItemMenu.addSeparator();
    /*************Layers*end***************/

    QAction *jumpTo=NULL;
    if(this->data(ITEM_TYPE).toString()=="Door_enter")
    {
        jumpTo =                ItemMenu.addAction(tr("Jump to exit"));
        jumpTo->setVisible( (m_data.isSetIn)&&(m_data.isSetOut) );
    }
    else
    if(this->data(ITEM_TYPE).toString()=="Door_exit")
    {
        jumpTo =                ItemMenu.addAction(tr("Jump to entrance"));
        jumpTo->setVisible( (m_data.isSetIn)&&(m_data.isSetOut) );
    }
                                ItemMenu.addSeparator();
    QAction * NoTransport =     ItemMenu.addAction(tr("No Vehicles"));
        NoTransport->setCheckable(true);
        NoTransport->setChecked( m_data.novehicles );

    QAction * AllowNPC =        ItemMenu.addAction(tr("Allow NPC"));
        AllowNPC->setCheckable(true);
        AllowNPC->setChecked( m_data.allownpc );

    QAction * Locked =          ItemMenu.addAction(tr("Locked"));
        Locked->setCheckable(true);
        Locked->setChecked( m_data.locked );
    QAction * BombNeed =        ItemMenu.addAction(tr("Need a bomb"));
        BombNeed->setCheckable(true);
        BombNeed->setChecked( m_data.need_a_bomb );
    QAction * SpecialStReq =    ItemMenu.addAction(tr("Required special state"));
        SpecialStReq->setCheckable(true);
        SpecialStReq->setChecked( m_data.special_state_required );

    /*************Copy Preferences*******************/
    ItemMenu.addSeparator();
    QMenu * copyPreferences =   ItemMenu.addMenu(tr("Copy preferences"));
    QAction *copyPosXY =        copyPreferences->addAction(tr("Position: X, Y"));
    QAction *copyPosXYWH =      copyPreferences->addAction(tr("Position: X, Y, Width, Height"));
    QAction *copyPosLTRB =      copyPreferences->addAction(tr("Position: Left, Top, Right, Bottom"));
    /*************Copy Preferences*end***************/

                                ItemMenu.addSeparator();
    QAction *remove =           ItemMenu.addAction(tr("Remove"));

                                ItemMenu.addSeparator();
    QAction *props =            ItemMenu.addAction(tr("Properties..."));

    /*****************Waiting for answer************************/
    QAction *selected = ItemMenu.exec(mouseEvent->screenPos());
    /***********************************************************/

    if(!selected)
        return;

    if(selected==openLvl)
    {
        MainWinConnect::pMainWin->OpenFile(m_scene->LvlData->path + "/" + m_data.lname);
    }
    else
    if(selected==jumpTo)
    {
        //scene->doCopy = true ;
        if(this->data(ITEM_TYPE).toString()=="Door_enter")
        {
            if(m_data.isSetOut)
            MainWinConnect::pMainWin->activeLvlEditWin()->goTo(m_data.ox, m_data.oy, true, QPoint(0, 0), true);
        }
        else
        if(this->data(ITEM_TYPE).toString()=="Door_exit")
        {
            if(m_data.isSetIn)
            MainWinConnect::pMainWin->activeLvlEditWin()->goTo(m_data.ix, m_data.iy, true, QPoint(0, 0), true);
        }
    }
    else
    if(selected==NoTransport)
    {
        LevelData modDoors;
        foreach(QGraphicsItem * SelItem, m_scene->selectedItems() )
        {
            if((SelItem->data(ITEM_TYPE).toString()=="Door_exit")||(SelItem->data(ITEM_TYPE).toString()=="Door_enter"))
            {
                if(SelItem->data(ITEM_TYPE).toString()=="Door_exit"){
                    LevelDoor door = ((ItemDoor *) SelItem)->m_data;
                    door.isSetOut = true;
                    door.isSetIn = false;
                    modDoors.doors.push_back(door);
                }else if(SelItem->data(ITEM_TYPE).toString()=="Door_enter"){
                    LevelDoor door = ((ItemDoor *) SelItem)->m_data;
                    door.isSetOut = false;
                    door.isSetIn = true;
                    modDoors.doors.push_back(door);
                }
                ((ItemDoor *) SelItem)->m_data.novehicles=NoTransport->isChecked();
                ((ItemDoor *) SelItem)->arrayApply();
            }
        }
        m_scene->addChangeSettingsHistory(modDoors, HistorySettings::SETTING_NOVEHICLE, QVariant(NoTransport->isChecked()));
        MainWinConnect::pMainWin->dock_LvlWarpProps->setDoorData(-2);
    }
    else
    if(selected==AllowNPC)
    {
        LevelData modDoors;
        foreach(QGraphicsItem * SelItem, m_scene->selectedItems() )
        {
            if((SelItem->data(ITEM_TYPE).toString()=="Door_exit")||(SelItem->data(ITEM_TYPE).toString()=="Door_enter"))
            {
                if(SelItem->data(ITEM_TYPE).toString()=="Door_exit") {
                    LevelDoor door = ((ItemDoor *) SelItem)->m_data;
                    door.isSetOut = true;
                    door.isSetIn = false;
                    modDoors.doors.push_back(door);
                } else if(SelItem->data(ITEM_TYPE).toString()=="Door_enter"){
                    LevelDoor door = ((ItemDoor *) SelItem)->m_data;
                    door.isSetOut = false;
                    door.isSetIn = true;
                    modDoors.doors.push_back(door);
                }
                ((ItemDoor *) SelItem)->m_data.allownpc=AllowNPC->isChecked();
                ((ItemDoor *) SelItem)->arrayApply();
            }
        }
        m_scene->addChangeSettingsHistory(modDoors, HistorySettings::SETTING_ALLOWNPC, QVariant(AllowNPC->isChecked()));
        MainWinConnect::pMainWin->dock_LvlWarpProps->setDoorData(-2);
    }
    else
    if(selected==Locked)
    {
        LevelData modDoors;
        foreach(QGraphicsItem * SelItem, m_scene->selectedItems() )
        {
            if((SelItem->data(ITEM_TYPE).toString()=="Door_exit")||(SelItem->data(ITEM_TYPE).toString()=="Door_enter"))
            {
                if(SelItem->data(ITEM_TYPE).toString()=="Door_exit"){
                    LevelDoor door = ((ItemDoor *) SelItem)->m_data;
                    door.isSetOut = true;
                    door.isSetIn = false;
                    modDoors.doors.push_back(door);
                }else if(SelItem->data(ITEM_TYPE).toString()=="Door_enter"){
                    LevelDoor door = ((ItemDoor *) SelItem)->m_data;
                    door.isSetOut = false;
                    door.isSetIn = true;
                    modDoors.doors.push_back(door);
                }
                ((ItemDoor *) SelItem)->m_data.locked=Locked->isChecked();
                ((ItemDoor *) SelItem)->arrayApply();
            }
        }
        m_scene->addChangeSettingsHistory(modDoors, HistorySettings::SETTING_LOCKED, QVariant(Locked->isChecked()));
        MainWinConnect::pMainWin->dock_LvlWarpProps->setDoorData(-2);
    }
    else
    if(selected==BombNeed)
    {
        LevelData modDoors;
        foreach(QGraphicsItem * SelItem, m_scene->selectedItems() )
        {
            if((SelItem->data(ITEM_TYPE).toString()=="Door_exit")||(SelItem->data(ITEM_TYPE).toString()=="Door_enter"))
            {
                if(SelItem->data(ITEM_TYPE).toString()=="Door_exit"){
                    LevelDoor door = ((ItemDoor *) SelItem)->m_data;
                    door.isSetOut = true;
                    door.isSetIn = false;
                    modDoors.doors.push_back(door);
                }else if(SelItem->data(ITEM_TYPE).toString()=="Door_enter"){
                    LevelDoor door = ((ItemDoor *) SelItem)->m_data;
                    door.isSetOut = false;
                    door.isSetIn = true;
                    modDoors.doors.push_back(door);
                }
                ((ItemDoor *) SelItem)->m_data.need_a_bomb=BombNeed->isChecked();
                ((ItemDoor *) SelItem)->arrayApply();
            }
        }
        m_scene->addChangeSettingsHistory(modDoors, HistorySettings::SETTING_NEED_A_BOMB, QVariant(BombNeed->isChecked()));
        MainWinConnect::pMainWin->dock_LvlWarpProps->setDoorData(-2);
    }
    else
    if(selected==SpecialStReq)
    {
        LevelData modDoors;
        foreach(QGraphicsItem * SelItem, m_scene->selectedItems() )
        {
            if((SelItem->data(ITEM_TYPE).toString()=="Door_exit")||(SelItem->data(ITEM_TYPE).toString()=="Door_enter"))
            {
                if(SelItem->data(ITEM_TYPE).toString()=="Door_exit"){
                    LevelDoor door = ((ItemDoor *) SelItem)->m_data;
                    door.isSetOut = true;
                    door.isSetIn = false;
                    modDoors.doors.push_back(door);
                }else if(SelItem->data(ITEM_TYPE).toString()=="Door_enter"){
                    LevelDoor door = ((ItemDoor *) SelItem)->m_data;
                    door.isSetOut = false;
                    door.isSetIn = true;
                    modDoors.doors.push_back(door);
                }
                ((ItemDoor *) SelItem)->m_data.special_state_required=SpecialStReq->isChecked();
                ((ItemDoor *) SelItem)->arrayApply();
            }
        }
        m_scene->addChangeSettingsHistory(modDoors, HistorySettings::SETTING_W_SPECIAL_STATE_REQUIRED, QVariant(SpecialStReq->isChecked()));
        MainWinConnect::pMainWin->dock_LvlWarpProps->setDoorData(-2);
    }
    else
    if(selected==copyPosXY)
    {
        QApplication::clipboard()->setText(
                            QString("X=%1; Y=%2;")
                               .arg(direction==D_Entrance ? m_data.ix : m_data.ox)
                               .arg(direction==D_Entrance ? m_data.iy : m_data.oy)
                               );
        MainWinConnect::pMainWin->showStatusMsg(tr("Preferences has been copied: %1").arg(QApplication::clipboard()->text()));
    }
    else
    if(selected==copyPosXYWH)
    {
        QApplication::clipboard()->setText(
                            QString("X=%1; Y=%2; W=%3; H=%4;")
                               .arg(direction==D_Entrance ? m_data.ix : m_data.ox)
                               .arg(direction==D_Entrance ? m_data.iy : m_data.oy)
                               .arg(m_itemSize.width())
                               .arg(m_itemSize.height())
                               );
        MainWinConnect::pMainWin->showStatusMsg(tr("Preferences has been copied: %1").arg(QApplication::clipboard()->text()));
    }
    else
    if(selected==copyPosLTRB)
    {
        QApplication::clipboard()->setText(
                            QString("Left=%1; Top=%2; Right=%3; Bottom=%4;")
                               .arg(direction==D_Entrance ? m_data.ix : m_data.ox)
                               .arg(direction==D_Entrance ? m_data.iy : m_data.oy)
                               .arg((direction==D_Entrance ? m_data.ix : m_data.ox)+m_itemSize.width())
                               .arg((direction==D_Entrance ? m_data.iy : m_data.oy)+m_itemSize.height())
                               );
        MainWinConnect::pMainWin->showStatusMsg(tr("Preferences has been copied: %1").arg(QApplication::clipboard()->text()));
    }
    else
    if(selected==remove)
    {
        m_scene->removeSelectedLvlItems();
    }
    else
    if(selected==props)
    {
        MainWinConnect::pMainWin->dock_LvlWarpProps->SwitchToDoor(m_data.array_id);
    }
    else
    if(selected==newLayer)
    {
        m_scene->setLayerToSelected();
        m_scene->applyLayersVisible();
    }
    else
    {
        //Fetch layers menu
        foreach(QAction * lItem, layerItems)
        {
            if(selected==lItem)
            {
                //FOUND!!!
                m_scene->setLayerToSelected(lItem->data().toString());
                m_scene->applyLayersVisible();
                MainWinConnect::pMainWin->dock_LvlWarpProps->setDoorData(-2);
                break;
            }//Find selected layer's item
        }
    }
}


///////////////////MainArray functions/////////////////////////////
void ItemDoor::setLayer(QString layer)
{
    foreach(LevelLayer lr, m_scene->LvlData->layers)
    {
        if(lr.name==layer)
        {
            m_data.layer = layer;
            this->setVisible(!lr.hidden);
            arrayApply();
        break;
        }
    }
}

void ItemDoor::arrayApply()
{
    bool found=false;

    if((direction==D_Entrance) && m_data.isSetIn)
    {
        m_data.ix = qRound(this->scenePos().x());
        m_data.iy = qRound(this->scenePos().y());
    }
    else if((direction==D_Exit) && m_data.isSetOut )
    {
        m_data.ox = qRound(this->scenePos().x());
        m_data.oy = qRound(this->scenePos().y());
    }
    /** Explanation for cramps-man why was that dumb bug:
     *
     * when we killed point, doorData.isSetIn is false.
     * so:
     * direction==D_Entrance is true, but isSetIN - false
     * else isSetOut - true
     * so: we are applying ENTRANCE's physical coordinates into EXIT :P
     * but that will NOT appear in file if we will just make dummy modify
     * of exit point and it's true value will overwrire invalid data.
     *
     * To prevent this crap, need to also add condition
     * to check "is this point is exit"?
     * so, even if entrance point marked as "false" because "not placed" flag
     * exit point's value will not be overwritten
     *
     */

    if(m_data.index < (unsigned int)m_scene->LvlData->doors.size())
    { //Check index
        if(m_data.array_id == m_scene->LvlData->doors[m_data.index].array_id)
        {
            found=true;
        }
    }

    //Apply current data in main array
    if(found)
    { //directlry
        m_scene->LvlData->doors[m_data.index] = m_data; //apply current bgoData
    }
    else
    for(int i=0; i<m_scene->LvlData->doors.size(); i++)
    { //after find it into array
        if(m_scene->LvlData->doors[i].array_id == m_data.array_id)
        {
            m_data.index = i;
            m_scene->LvlData->doors[i] = m_data;
            break;
        }
    }

    //Sync data to his pair door item
    if(direction==D_Entrance)
    {
        if(m_data.isSetOut)
        {
            foreach(QGraphicsItem * door, m_scene->items())
            {
                if((door->data(ITEM_TYPE).toString()=="Door_exit")&&((unsigned int)door->data(ITEM_ARRAY_ID).toInt()==m_data.array_id))
                {
                    ((ItemDoor *)door)->m_data = m_data;
                    break;
                }
            }
        }
    }
    else
    {
        if(m_data.isSetIn)
        {
            foreach(QGraphicsItem * door, m_scene->items())
            {
                if((door->data(ITEM_TYPE).toString()=="Door_enter")&&((unsigned int)door->data(ITEM_ARRAY_ID).toInt()==m_data.array_id))
                {
                    ((ItemDoor *)door)->m_data = m_data;
                    break;
                }
            }
        }

    }

    //Update R-tree innex
    m_scene->unregisterElement(this);
    m_scene->registerElement(this);
}

void ItemDoor::removeFromArray()
{
    if(direction==D_Entrance)
    {
        m_data.isSetIn=false;
        m_data.ix = 0;
        m_data.iy = 0;
    }
    else
    {
        m_data.isSetOut=false;
        m_data.ox = 0;
        m_data.oy = 0;
    }
    arrayApply();
}


void ItemDoor::returnBack()
{
    if(direction==D_Entrance)
        this->setPos(m_data.ix, m_data.iy);
    else
        this->setPos(m_data.ox, m_data.oy);
}

QPoint ItemDoor::sourcePos()
{
    if(direction==D_Entrance)
        return QPoint(m_data.ix, m_data.iy);
    else
        return QPoint(m_data.ox, m_data.oy);
}

bool ItemDoor::itemTypeIsLocked()
{
    if(!m_scene)
        return false;
    return m_scene->lock_door;
}

void ItemDoor::setDoorData(LevelDoor inD, int doorDir, bool init)
{
    m_data = inD;
    direction = doorDir;

    long ix, iy, ox, oy;
    QColor cEnter(qRgb(0xff,0x00,0x7f));
    QColor cExit(qRgb(0xc4,0x00,0x62));//c40062
    cEnter.setAlpha(50);
    cExit.setAlpha(50);

    ix = m_data.ix;
    iy = m_data.iy;
    ox = m_data.ox;
    oy = m_data.oy;

    m_doorLabel = new QGraphicsPixmapItem(GraphicsHelps::drawDegitFont(m_data.array_id));

    if(direction==D_Entrance)
    {
        m_data.isSetIn=true;
        m_brush = QBrush(cEnter);
        m_pen = QPen(QBrush(QColor(qRgb(0xff,0x00,0x7f))), 2,Qt::SolidLine, Qt::SquareCap, Qt::MiterJoin);
        m_doorLabel->setPos(ix+2, iy+2);

        setPos(ix, iy);
        setData(ITEM_TYPE, "Door_enter"); // ObjType
    }
    else
    {
        m_data.isSetOut=true;
        m_brush = QBrush(cExit);
        m_pen = QPen(QBrush(QColor(qRgb(0xc4,0x00,0x62))), 2,Qt::SolidLine, Qt::SquareCap, Qt::MiterJoin);
        m_doorLabel->setPos(ox+16, oy+16);

        setPos(ox, oy);
        setData(ITEM_TYPE, "Door_exit"); // ObjType
    }
    m_grp->addToGroup(m_doorLabel);

    this->setFlag(QGraphicsItem::ItemIsSelectable, (!m_scene->lock_door));
    this->setFlag(QGraphicsItem::ItemIsMovable, (!m_scene->lock_door));

    m_doorLabel->setZValue(m_scene->Z_sys_door+0.0000002);

    this->setData(ITEM_ID, QString::number(0) );
    this->setData(ITEM_ARRAY_ID, QString::number(m_data.array_id) );

    this->setZValue(m_scene->Z_sys_door);

    if(!init)
    {
        arrayApply();
    }

    m_scene->unregisterElement(this);
    m_scene->registerElement(this);
}

QRectF ItemDoor::boundingRect() const
{
    return QRectF(-1,-1,m_itemSize.width()+2,m_itemSize.height()+2);
}

void ItemDoor::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *)
{
    painter->setBrush(m_brush);
    painter->setPen(m_pen);
    painter->drawRect(1, 1, m_itemSize.width()-2, m_itemSize.height()-2);

    if(this->isSelected())
    {
        painter->setPen(QPen(QBrush(Qt::black), 2, Qt::SolidLine));
        painter->drawRect(1,1,m_itemSize.width()-2,m_itemSize.height()-2);
        painter->setPen(QPen(QBrush(Qt::white), 2, Qt::DotLine));
        painter->drawRect(1,1,m_itemSize.width()-2,m_itemSize.height()-2);
    }
}

void ItemDoor::setScenePoint(LvlScene *theScene)
{
    LvlBaseItem::setScenePoint(theScene);
    if(m_grp) delete m_grp;
    m_grp = new QGraphicsItemGroup(this);
    m_doorLabel = NULL;
}

