/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2014-2023 Vitaly Novichkov <admin@wohlnet.ru>
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

#include <QClipboard>

#include <mainwindow.h>
#include <common_features/logger.h>
#include <common_features/graphics_funcs.h>
#include <main_window/dock/lvl_warp_props.h>
#include <Utils/dir_list_ci_qt.h>

#include "../lvl_history_manager.h"
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
    setLocked(m_scene->m_lockDoor);
}

void ItemDoor::construct()
{
    m_gridSize = 16;
    m_itemSize = QSize(32, 32);
    this->setData(ITEM_WIDTH, 32);
    this->setData(ITEM_HEIGHT, 32);
    m_grp = nullptr;
    m_doorLabel = nullptr;
    m_arrowEnter = nullptr;
    m_arrowExit = nullptr;
}

ItemDoor::~ItemDoor()
{
    if(m_doorLabel != nullptr) delete m_doorLabel;
    if(m_arrowEnter != nullptr) delete m_arrowEnter;
    if(m_arrowExit != nullptr) delete m_arrowExit;
    if(m_grp != nullptr) delete m_grp;
    m_scene->unregisterElement(this);
}

static void updateWarpDirection(LvlScene *m_scene, bool isEnter, bool isExit, int enterDir, int exitDir)
{
    for(QGraphicsItem *SelItem : m_scene->selectedItems())
    {
        bool sIsEnter = SelItem->data(ITEM_TYPE).toString() == "Door_enter";
        bool sIsExit = SelItem->data(ITEM_TYPE).toString() == "Door_exit";

        if((isEnter && sIsEnter) || (isExit && sIsExit))
        {
            QList<QVariant> dirData;

            auto d = qgraphicsitem_cast<ItemDoor *>(SelItem);
            Q_ASSERT(d);

            if(sIsExit)
            {
                dirData.push_back(d->m_data.odirect);
                dirData.push_back(exitDir);
                d->m_data.odirect = exitDir;
            }
            else if(sIsEnter)
            {
                dirData.push_back(d->m_data.idirect);
                dirData.push_back(enterDir);
                d->m_data.idirect = enterDir;
            }

            m_scene->m_history->addChangeWarpSettings(static_cast<int>(d->m_data.meta.array_id),
                                                      isEnter ? HistorySettings::SETTING_ENTRDIR : HistorySettings::SETTING_EXITDIR,
                                                      QVariant(dirData));

            d->arrayApply();
            d->refreshArrows();
        }
    }

    m_scene->m_mw->dock_LvlWarpProps->setDoorData(-2);
}

void ItemDoor::contextMenu(QGraphicsSceneMouseEvent *mouseEvent)
{
    m_scene->m_contextMenuIsOpened = true; //bug protector

    //Remove selection from non-bgo items
    if(!this->isSelected())
    {
        m_scene->clearSelection();
        this->setSelected(true);
    }

    this->setSelected(1);
    QMenu itemMenu;
    DirListCIQt ci;
    QString dstLevelPath;
    QString dstLevelName;
    DataConfig &c = *(m_scene->m_configs);
    bool shown = !m_scene->m_data->meta.smbx64strict;

    if(!m_data.lname.isEmpty())
    {
        ci.setCurDir(m_scene->m_data->meta.path);
        dstLevelName = ci.resolveFileCase(m_data.lname);
        dstLevelPath = m_scene->m_data->meta.path + "/" + dstLevelName;
    }

    QAction *openLvl = itemMenu.addAction(tr("Open target level: %1").arg(dstLevelName).replace("&", "&&"));
    openLvl->setVisible((!dstLevelName.isEmpty()) && QFile::exists(dstLevelPath));
    openLvl->deleteLater();

    /*************Layers*******************/
    QMenu *layerName =     itemMenu.addMenu(tr("Layer: ") + QString("[%1]").arg(m_data.layer).replace("&", "&&"));
    QAction *setLayer;
    QList<QAction *> layerItems;

    QAction *newLayer =    layerName->addAction(tr("Add to new layer..."));
    layerName->addSeparator()->deleteLater();

    for(const LevelLayer &layer : m_scene->m_data->layers)
    {
        //Skip system layers
        if((layer.name == "Destroyed Blocks") || (layer.name == "Spawned NPCs"))
            continue;

        QString label = layer.name + ((layer.hidden) ? " " + tr("[hidden]") : "");
        setLayer = layerName->addAction(label.replace("&", "&&"));
        setLayer->setData(layer.name);
        setLayer->setCheckable(true);
        setLayer->setEnabled(true);
        setLayer->setChecked(layer.name == m_data.layer);
        layerItems.push_back(setLayer);
    }
    itemMenu.addSeparator();
    /*************Layers*end***************/

    bool isEnter = this->data(ITEM_TYPE).toString() == "Door_enter";
    bool isExit = this->data(ITEM_TYPE).toString() == "Door_exit";

    QAction *jumpTo = nullptr;
    if(isEnter)
    {
        jumpTo =                itemMenu.addAction(tr("Jump to exit"));
        jumpTo->setVisible((m_data.isSetIn) && (m_data.isSetOut));
    }
    else if(isExit)
    {
        jumpTo =                itemMenu.addAction(tr("Jump to entrance"));
        jumpTo->setVisible((m_data.isSetIn) && (m_data.isSetOut));
    }

    QAction *dirLeft = nullptr;
    QAction *dirUp = nullptr;
    QAction *dirRight = nullptr;
    QAction *dirDown = nullptr;

    if(m_data.type == LevelDoor::WARP_PIPE)
    {
        itemMenu.addSeparator();
        QMenu *dir = itemMenu.addMenu(tr("Direction"));

        dirLeft = dir->addAction(tr("Left", "Direction of pipe"));
        dirLeft->setCheckable(true);
        dirLeft->setIcon(QIcon(isEnter ? ":/arrows/red_left.png" : ":/arrows/green_left.png"));

        dirUp = dir->addAction(tr("Up", "Direction of pipe"));
        dirUp->setCheckable(true);
        dirUp->setIcon(QIcon(isEnter ? ":/arrows/red_up.png" : ":/arrows/green_up.png"));

        dirRight = dir->addAction(tr("Right", "Direction of pipe"));
        dirRight->setCheckable(true);
        dirRight->setIcon(QIcon(isEnter ? ":/arrows/red_right.png" : ":/arrows/green_right.png"));

        dirDown = dir->addAction(tr("Down", "Direction of pipe"));
        dirDown->setCheckable(true);
        dirDown->setIcon(QIcon(isEnter ? ":/arrows/red_down.png" : ":/arrows/green_down.png"));

        if(isEnter)
        {
            switch(m_data.idirect)
            {
            case LevelDoor::ENTRANCE_LEFT:
                dirLeft->setChecked(true);
                dir->setIcon(QIcon(":/arrows/red_left.png"));
                break;
            case LevelDoor::ENTRANCE_UP:
                dirUp->setChecked(true);
                dir->setIcon(QIcon(":/arrows/red_up.png"));
                break;
            case LevelDoor::ENTRANCE_RIGHT:
                dirRight->setChecked(true);
                dir->setIcon(QIcon(":/arrows/red_right.png"));
                break;
            case LevelDoor::ENTRANCE_DOWN:
                dirDown->setChecked(true);
                dir->setIcon(QIcon(":/arrows/red_down.png"));
                break;
            }
        }
        else
        {
            switch(m_data.odirect)
            {
            case LevelDoor::EXIT_LEFT:
                dirLeft->setChecked(true);
                dir->setIcon(QIcon(":/arrows/green_left.png"));
                break;
            case LevelDoor::EXIT_UP:
                dirUp->setChecked(true);
                dir->setIcon(QIcon(":/arrows/green_up.png"));
                break;
            case LevelDoor::EXIT_RIGHT:
                dirRight->setChecked(true);
                dir->setIcon(QIcon(":/arrows/green_right.png"));
                break;
            case LevelDoor::EXIT_DOWN:
                dirDown->setChecked(true);
                dir->setIcon(QIcon(":/arrows/green_down.png"));
                break;
            }
        }
    }

    itemMenu.addSeparator();
    QAction *NoTransport =     itemMenu.addAction(tr("No Vehicles"));
    NoTransport->setCheckable(true);
    NoTransport->setChecked(m_data.novehicles);

    QAction *allowNPC =        itemMenu.addAction(tr("Allow NPC"));
    allowNPC->setCheckable(true);
    allowNPC->setChecked(m_data.allownpc);

    QAction *locked =          itemMenu.addAction(tr("Locked"));
    locked->setCheckable(true);
    locked->setChecked(m_data.locked);
    QAction *bombNeed =        itemMenu.addAction(tr("Need a bomb"));
    bombNeed->setCheckable(true);
    bombNeed->setChecked(m_data.need_a_bomb);
    bombNeed->setEnabled(shown && (c.editor.supported_features.level_warp_bomb_exit == EditorSetup::FeaturesSupport::F_ENABLED));
    bombNeed->setVisible(c.editor.supported_features.level_warp_bomb_exit != EditorSetup::FeaturesSupport::F_HIDDEN);
    QAction *specialStReq =    itemMenu.addAction(tr("Required special state"));
    specialStReq->setCheckable(true);
    specialStReq->setChecked(m_data.special_state_required);
    specialStReq->setEnabled(shown && (c.editor.supported_features.level_warp_allow_sp_state_only == EditorSetup::FeaturesSupport::F_ENABLED));
    specialStReq->setVisible(c.editor.supported_features.level_warp_allow_sp_state_only != EditorSetup::FeaturesSupport::F_HIDDEN);
    QAction *floorReq =    itemMenu.addAction(tr("Required stood state"));
    floorReq->setCheckable(true);
    floorReq->setChecked(m_data.stood_state_required);
    floorReq->setEnabled(shown && (c.editor.supported_features.level_warp_allow_needs_floor == EditorSetup::FeaturesSupport::F_ENABLED));
    floorReq->setVisible(c.editor.supported_features.level_warp_allow_needs_floor != EditorSetup::FeaturesSupport::F_HIDDEN);

    /*************Copy Preferences*******************/
    itemMenu.addSeparator();
    QMenu *copyPreferences =   itemMenu.addMenu(tr("Copy preferences"));
    QAction *copyArrayID =      copyPreferences->addAction(tr("Array-ID: %1").arg(m_data.meta.array_id));
    QAction *copyPosXY =        copyPreferences->addAction(tr("Position: X, Y"));
    QAction *copyPosXYWH =      copyPreferences->addAction(tr("Position: X, Y, Width, Height"));
    QAction *copyPosLTRB =      copyPreferences->addAction(tr("Position: Left, Top, Right, Bottom"));
    /*************Copy Preferences*end***************/

    itemMenu.addSeparator();
    QAction *remove =           itemMenu.addAction(tr("Remove"));

    itemMenu.addSeparator();
    QAction *props =            itemMenu.addAction(tr("Properties..."));

    /*****************Waiting for answer************************/
    QAction *selected = itemMenu.exec(mouseEvent->screenPos());
    /***********************************************************/

    if(!selected)
        return;

    if(selected == openLvl)
        m_scene->m_mw->OpenFile(dstLevelPath);
    else if(selected == jumpTo)
    {
        //scene->doCopy = true ;
        if(isEnter)
        {
            if(m_data.isSetOut)
                m_scene->m_mw->activeLvlEditWin()->goTo(m_data.ox, m_data.oy, true, QPoint(0, 0), true);
        }
        else if(isExit)
        {
            if(m_data.isSetIn)
                m_scene->m_mw->activeLvlEditWin()->goTo(m_data.ix, m_data.iy, true, QPoint(0, 0), true);
        }
    }
    else if(dirLeft && selected == dirLeft)
    {
        updateWarpDirection(m_scene, isEnter, isExit, LevelDoor::ENTRANCE_LEFT, LevelDoor::EXIT_LEFT);
    }
    else if(dirUp && selected == dirUp)
    {
        updateWarpDirection(m_scene, isEnter, isExit, LevelDoor::ENTRANCE_UP, LevelDoor::EXIT_UP);
    }
    else if(dirRight && selected == dirRight)
    {
        updateWarpDirection(m_scene, isEnter, isExit, LevelDoor::ENTRANCE_RIGHT, LevelDoor::EXIT_RIGHT);
    }
    else if(dirDown && selected == dirDown)
    {
        updateWarpDirection(m_scene, isEnter, isExit, LevelDoor::ENTRANCE_DOWN, LevelDoor::EXIT_DOWN);
    }
    else if(selected == NoTransport)
    {
        LevelData modDoors;
        for(QGraphicsItem *SelItem : m_scene->selectedItems())
        {
            bool sIsEnter = SelItem->data(ITEM_TYPE).toString() == "Door_enter";
            bool sIsExit = SelItem->data(ITEM_TYPE).toString() == "Door_exit";

            if(sIsEnter || sIsExit)
            {
                auto d = qgraphicsitem_cast<ItemDoor *>(SelItem);
                Q_ASSERT(d);

                if(sIsExit)
                {
                    LevelDoor door = d->m_data;
                    door.isSetOut = true;
                    door.isSetIn = false;
                    modDoors.doors.push_back(door);
                }
                else if(sIsEnter)
                {
                    LevelDoor door = d->m_data;
                    door.isSetOut = false;
                    door.isSetIn = true;
                    modDoors.doors.push_back(door);
                }

                d->m_data.novehicles = NoTransport->isChecked();
                d->arrayApply();
            }
        }
        m_scene->m_history->addChangeSettings(modDoors, HistorySettings::SETTING_NOVEHICLE, QVariant(NoTransport->isChecked()));
        m_scene->m_mw->dock_LvlWarpProps->setDoorData(-2);
    }
    else if(selected == allowNPC)
    {
        LevelData modDoors;
        for(QGraphicsItem *SelItem : m_scene->selectedItems())
        {
            bool sIsEnter = SelItem->data(ITEM_TYPE).toString() == "Door_enter";
            bool sIsExit = SelItem->data(ITEM_TYPE).toString() == "Door_exit";

            if(sIsEnter || sIsExit)
            {
                auto d = qgraphicsitem_cast<ItemDoor *>(SelItem);
                Q_ASSERT(d);

                if(sIsExit)
                {
                    LevelDoor door = d->m_data;
                    door.isSetOut = true;
                    door.isSetIn = false;
                    modDoors.doors.push_back(door);
                }
                else if(sIsEnter)
                {
                    LevelDoor door = d->m_data;
                    door.isSetOut = false;
                    door.isSetIn = true;
                    modDoors.doors.push_back(door);
                }

                d->m_data.allownpc = allowNPC->isChecked();
                d->arrayApply();
            }
        }
        m_scene->m_history->addChangeSettings(modDoors, HistorySettings::SETTING_ALLOWNPC, QVariant(allowNPC->isChecked()));
        m_scene->m_mw->dock_LvlWarpProps->setDoorData(-2);
    }
    else if(selected == locked)
    {
        LevelData modDoors;
        for(QGraphicsItem *SelItem : m_scene->selectedItems())
        {
            bool sIsEnter = SelItem->data(ITEM_TYPE).toString() == "Door_enter";
            bool sIsExit = SelItem->data(ITEM_TYPE).toString() == "Door_exit";

            if(sIsEnter || sIsExit)
            {
                auto d = qgraphicsitem_cast<ItemDoor *>(SelItem);
                Q_ASSERT(d);

                if(sIsExit)
                {
                    LevelDoor door = d->m_data;
                    door.isSetOut = true;
                    door.isSetIn = false;
                    modDoors.doors.push_back(door);
                }
                else if(sIsEnter)
                {
                    LevelDoor door = d->m_data;
                    door.isSetOut = false;
                    door.isSetIn = true;
                    modDoors.doors.push_back(door);
                }

                d->m_data.locked = locked->isChecked();
                d->arrayApply();
            }
        }
        m_scene->m_history->addChangeSettings(modDoors, HistorySettings::SETTING_LOCKED, QVariant(locked->isChecked()));
        m_scene->m_mw->dock_LvlWarpProps->setDoorData(-2);
    }
    else if(selected == bombNeed)
    {
        LevelData modDoors;
        for(QGraphicsItem *SelItem : m_scene->selectedItems())
        {
            bool sIsEnter = SelItem->data(ITEM_TYPE).toString() == "Door_enter";
            bool sIsExit = SelItem->data(ITEM_TYPE).toString() == "Door_exit";

            if(sIsEnter || sIsExit)
            {
                auto d = qgraphicsitem_cast<ItemDoor *>(SelItem);
                Q_ASSERT(d);

                if(sIsExit)
                {
                    LevelDoor door = d->m_data;
                    door.isSetOut = true;
                    door.isSetIn = false;
                    modDoors.doors.push_back(door);
                }
                else if(sIsEnter)
                {
                    LevelDoor door = d->m_data;
                    door.isSetOut = false;
                    door.isSetIn = true;
                    modDoors.doors.push_back(door);
                }

                d->m_data.need_a_bomb = bombNeed->isChecked();
                d->arrayApply();
            }
        }
        m_scene->m_history->addChangeSettings(modDoors, HistorySettings::SETTING_NEED_A_BOMB, QVariant(bombNeed->isChecked()));
        m_scene->m_mw->dock_LvlWarpProps->setDoorData(-2);
    }
    else if(selected == specialStReq)
    {
        LevelData modDoors;
        for(QGraphicsItem *SelItem : m_scene->selectedItems())
        {
            bool sIsEnter = SelItem->data(ITEM_TYPE).toString() == "Door_enter";
            bool sIsExit = SelItem->data(ITEM_TYPE).toString() == "Door_exit";

            if(sIsEnter || sIsExit)
            {
                auto d = qgraphicsitem_cast<ItemDoor *>(SelItem);
                Q_ASSERT(d);

                if(sIsExit)
                {
                    LevelDoor door = d->m_data;
                    door.isSetOut = true;
                    door.isSetIn = false;
                    modDoors.doors.push_back(door);
                }
                else if(sIsEnter)
                {
                    LevelDoor door = d->m_data;
                    door.isSetOut = false;
                    door.isSetIn = true;
                    modDoors.doors.push_back(door);
                }

                d->m_data.special_state_required = specialStReq->isChecked();
                d->arrayApply();
            }
        }
        m_scene->m_history->addChangeSettings(modDoors, HistorySettings::SETTING_W_SPECIAL_STATE_REQUIRED, QVariant(specialStReq->isChecked()));
        m_scene->m_mw->dock_LvlWarpProps->setDoorData(-2);
    }
    else if(selected == floorReq)
    {
        LevelData modDoors;
        for(QGraphicsItem *SelItem : m_scene->selectedItems())
        {
            bool sIsEnter = SelItem->data(ITEM_TYPE).toString() == "Door_enter";
            bool sIsExit = SelItem->data(ITEM_TYPE).toString() == "Door_exit";

            if(sIsEnter || sIsExit)
            {
                auto d = qgraphicsitem_cast<ItemDoor *>(SelItem);
                Q_ASSERT(d);

                if(sIsExit)
                {
                    LevelDoor door = d->m_data;
                    door.isSetOut = true;
                    door.isSetIn = false;
                    modDoors.doors.push_back(door);
                }
                else if(sIsEnter)
                {
                    LevelDoor door = d->m_data;
                    door.isSetOut = false;
                    door.isSetIn = true;
                    modDoors.doors.push_back(door);
                }

                d->m_data.stood_state_required = floorReq->isChecked();
                d->arrayApply();
            }
        }
        m_scene->m_history->addChangeSettings(modDoors, HistorySettings::SETTING_W_NEEDS_FLOOR, QVariant(floorReq->isChecked()));
        m_scene->m_mw->dock_LvlWarpProps->setDoorData(-2);
    }
    else if(selected == copyArrayID)
    {
        QApplication::clipboard()->setText(QString("%1").arg(m_data.meta.array_id));
        m_scene->m_mw->showStatusMsg(tr("Preferences have been copied: %1").arg(QApplication::clipboard()->text()));
    }
    else if(selected == copyPosXY)
    {
        QApplication::clipboard()->setText(
            QString("X=%1; Y=%2;")
            .arg(m_pointSide == D_Entrance ? m_data.ix : m_data.ox)
            .arg(m_pointSide == D_Entrance ? m_data.iy : m_data.oy)
        );
        m_scene->m_mw->showStatusMsg(tr("Preferences have been copied: %1").arg(QApplication::clipboard()->text()));
    }
    else if(selected == copyPosXYWH)
    {
        QApplication::clipboard()->setText(
            QString("X=%1; Y=%2; W=%3; H=%4;")
            .arg(m_pointSide == D_Entrance ? m_data.ix : m_data.ox)
            .arg(m_pointSide == D_Entrance ? m_data.iy : m_data.oy)
            .arg(m_itemSize.width())
            .arg(m_itemSize.height())
        );
        m_scene->m_mw->showStatusMsg(tr("Preferences have been copied: %1").arg(QApplication::clipboard()->text()));
    }
    else if(selected == copyPosLTRB)
    {
        QApplication::clipboard()->setText(
            QString("Left=%1; Top=%2; Right=%3; Bottom=%4;")
            .arg(m_pointSide == D_Entrance ? m_data.ix : m_data.ox)
            .arg(m_pointSide == D_Entrance ? m_data.iy : m_data.oy)
            .arg((m_pointSide == D_Entrance ? m_data.ix : m_data.ox) + m_itemSize.width())
            .arg((m_pointSide == D_Entrance ? m_data.iy : m_data.oy) + m_itemSize.height())
        );
        m_scene->m_mw->showStatusMsg(tr("Preferences have been copied: %1").arg(QApplication::clipboard()->text()));
    }
    else if(selected == remove)
        m_scene->removeSelectedLvlItems();
    else if(selected == props)
        m_scene->m_mw->dock_LvlWarpProps->SwitchToDoor(m_data.meta.array_id);
    else if(selected == newLayer)
    {
        m_scene->setLayerToSelected();
        m_scene->applyLayersVisible();
    }
    else
    {
        //Fetch layers menu
        for(QAction *lItem : layerItems)
        {
            if(selected == lItem)
            {
                //FOUND!!!
                m_scene->setLayerToSelected(lItem->data().toString());
                m_scene->applyLayersVisible();
                m_scene->m_mw->dock_LvlWarpProps->setDoorData(-2);
                break;
            }//Find selected layer's item
        }
    }
}


QString ItemDoor::getLayerName()
{
    return m_data.layer;
}

///////////////////MainArray functions/////////////////////////////
void ItemDoor::setLayer(QString layer)
{
    foreach(LevelLayer lr, m_scene->m_data->layers)
    {
        if(lr.name == layer)
        {
            m_data.layer = layer;
            this->setVisible(!lr.hidden);
            arrayApply();
            break;
        }
    }
}

void ItemDoor::refreshArrows()
{
    if(m_arrowEnter != nullptr)
    {
        m_grp->removeFromGroup(m_arrowEnter);
        m_scene->removeItem(m_arrowEnter);
        delete m_arrowEnter;
        m_arrowEnter = nullptr;
    }

    if(m_arrowExit != nullptr)
    {
        m_grp->removeFromGroup(m_arrowExit);
        m_scene->removeItem(m_arrowExit);
        delete m_arrowExit;
        m_arrowExit = nullptr;
    }

    if(m_data.type == LevelDoor::WARP_PIPE)
    {
        if((m_pointSide == D_Entrance) || m_data.two_way)
        {
            m_arrowEnter = new QGraphicsPixmapItem;
            QPixmap pix(":/arrows/warp_arrow_enter.png");
            m_arrowEnter->setPixmap(pix);
            m_scene->addItem(m_arrowEnter);
            m_arrowEnter->setOpacity(qreal(0.6));

            QPointF offset = QPoint(0, 0);
            int idirect = m_data.idirect;

            if(m_data.two_way && m_pointSide == D_Exit)
            {
                switch(m_data.odirect)
                {
                case LevelDoor::EXIT_LEFT:
                    idirect = LevelDoor::ENTRANCE_RIGHT;
                    break;
                case LevelDoor::EXIT_DOWN:
                    idirect = LevelDoor::ENTRANCE_UP;
                    break;
                case LevelDoor::EXIT_RIGHT:
                    idirect = LevelDoor::ENTRANCE_LEFT;
                    break;
                case LevelDoor::EXIT_UP:
                default:
                    idirect = LevelDoor::ENTRANCE_DOWN;
                    break;
                }
            }

            switch(idirect)
            {
            case LevelDoor::ENTRANCE_LEFT:
                m_arrowEnter->setRotation(270);
                offset.setY(32);
                break;
            case LevelDoor::ENTRANCE_DOWN:
                m_arrowEnter->setRotation(180);
                offset.setX(32);
                offset.setY(32);
                break;
            case LevelDoor::ENTRANCE_RIGHT:
                m_arrowEnter->setRotation(90);
                offset.setX(32);
                break;
            case LevelDoor::ENTRANCE_UP:
            default:
                m_arrowEnter->setRotation(0);
                break;
            }
            m_arrowEnter->setZValue(this->zValue() + 0.0000015);
            m_arrowEnter->setPos((offset.x() + scenePos().x()), (offset.y() + scenePos().y()));
            m_grp->addToGroup(m_arrowEnter);
        }

        if((m_pointSide == D_Exit) || m_data.two_way)
        {
            m_arrowExit = new QGraphicsPixmapItem;
            QPixmap pix(":/arrows/warp_arrow_exit.png");
            m_arrowExit->setPixmap(pix);
            m_scene->addItem(m_arrowExit);
            m_arrowExit->setOpacity(qreal(0.6));

            QPointF offset = QPoint(0, 0);

            int odirect = m_data.odirect;

            if(m_data.two_way && m_pointSide == D_Entrance)
            {
                switch(m_data.idirect)
                {
                case LevelDoor::ENTRANCE_LEFT:
                    odirect = LevelDoor::EXIT_RIGHT;
                    break;
                case LevelDoor::ENTRANCE_DOWN:
                    odirect = LevelDoor::EXIT_UP;
                    break;
                case LevelDoor::ENTRANCE_RIGHT:
                    odirect = LevelDoor::EXIT_LEFT;
                    break;
                case LevelDoor::ENTRANCE_UP:
                default:
                    odirect = LevelDoor::EXIT_DOWN;
                    break;
                }
            }

            switch(odirect)
            {
            case LevelDoor::EXIT_LEFT:
                m_arrowExit->setRotation(270);
                offset.setY(32);
                break;
            case LevelDoor::EXIT_DOWN:
                m_arrowExit->setRotation(180);
                offset.setX(32);
                offset.setY(32);
                break;
            case LevelDoor::EXIT_RIGHT:
                m_arrowExit->setRotation(90);
                offset.setX(32);
                break;
            case LevelDoor::EXIT_UP:
            default:
                m_arrowExit->setRotation(0);
                break;
            }
            m_arrowExit->setZValue(this->zValue() + 0.0000010);
            m_arrowExit->setPos((offset.x() + scenePos().x()), (offset.y() + scenePos().y()));
            m_grp->addToGroup(m_arrowExit);
        }
    }
}

void ItemDoor::arrayApply()
{
    bool found = false;

    if((m_pointSide == D_Entrance) && m_data.isSetIn)
    {
        m_data.ix = qRound(this->scenePos().x());
        m_data.iy = qRound(this->scenePos().y());
    }
    else if((m_pointSide == D_Exit) && m_data.isSetOut)
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

    if(m_data.meta.index < (unsigned int)m_scene->m_data->doors.size())
    {
        //Check index
        if(m_data.meta.array_id == m_scene->m_data->doors[m_data.meta.index].meta.array_id)
            found = true;
    }

    //Apply current data in main array
    if(found)
    {
        //directlry
        m_scene->m_data->doors[m_data.meta.index] = m_data; //apply current bgoData
    }
    else for(int i = 0; i < m_scene->m_data->doors.size(); i++)
    {
        //after find it into array
        if(m_scene->m_data->doors[i].meta.array_id == m_data.meta.array_id)
        {
            m_data.meta.index = i;
            m_scene->m_data->doors[i] = m_data;
            break;
        }
    }

    //Sync data to his pair door item
    if(m_pointSide == D_Entrance)
    {
        if(m_data.isSetOut)
        {
            for(QGraphicsItem *door : m_scene->items())
            {
                if((door->data(ITEM_TYPE).toString() == "Door_exit") && ((unsigned int)door->data(ITEM_ARRAY_ID).toInt() == m_data.meta.array_id))
                {
                    ItemDoor *d = (ItemDoor *)door;
                    d->m_data = m_data;
                    d->refreshArrows();
                    break;
                }
            }
        }
    }
    else
    {
        if(m_data.isSetIn)
        {
            for(QGraphicsItem *door : m_scene->items())
            {
                if((door->data(ITEM_TYPE).toString() == "Door_enter") && ((unsigned int)door->data(ITEM_ARRAY_ID).toInt() == m_data.meta.array_id))
                {
                    ItemDoor *d = (ItemDoor *)door;
                    d->m_data = m_data;
                    d->refreshArrows();
                    break;
                }
            }
        }

    }

    //Mark level as modified
    m_scene->m_data->meta.modified = true;

    //Update R-tree innex
    m_scene->unregisterElement(this);
    m_scene->registerElement(this);
}

void ItemDoor::removeFromArray()
{
    if(m_pointSide == D_Entrance)
    {
        m_data.isSetIn = false;
        m_data.ix = 0;
        m_data.iy = 0;
    }
    else
    {
        m_data.isSetOut = false;
        m_data.ox = 0;
        m_data.oy = 0;
    }
    arrayApply();
}


void ItemDoor::returnBack()
{
    if(m_pointSide == D_Entrance)
        this->setPos(m_data.ix, m_data.iy);
    else
        this->setPos(m_data.ox, m_data.oy);
}

QPoint ItemDoor::sourcePos()
{
    if(m_pointSide == D_Entrance)
        return QPoint(m_data.ix, m_data.iy);
    else
        return QPoint(m_data.ox, m_data.oy);
}

bool ItemDoor::itemTypeIsLocked()
{
    if(!m_scene)
        return false;
    return m_scene->m_lockDoor;
}

void ItemDoor::setDoorData(LevelDoor inD, int doorDir, bool init)
{
    m_data = inD;
    m_pointSide = doorDir;

    long ix, iy, ox, oy;
    QColor cEnter(qRgb(0xff, 0x00, 0x7f));
    QColor cExit(qRgb(0xc4, 0x00, 0x62)); //c40062
    cEnter.setAlpha(50);
    cExit.setAlpha(50);

    ix = m_data.ix;
    iy = m_data.iy;
    ox = m_data.ox;
    oy = m_data.oy;

    m_doorLabel = new QGraphicsPixmapItem(GraphicsHelps::drawDigitFont(m_data.meta.array_id));

    if(m_pointSide == D_Entrance)
    {
        m_data.isSetIn = true;
        m_brush = QBrush(cEnter);
        m_pen = QPen(QBrush(QColor(qRgb(0xff, 0x00, 0x7f))), 2, Qt::SolidLine, Qt::SquareCap, Qt::MiterJoin);
        m_doorLabel->setPos(ix + 2, iy + 2);

        setPos(ix, iy);
        setData(ITEM_TYPE, "Door_enter"); // ObjType
    }
    else
    {
        m_data.isSetOut = true;
        m_brush = QBrush(cExit);
        m_pen = QPen(QBrush(QColor(qRgb(0xc4, 0x00, 0x62))), 2, Qt::SolidLine, Qt::SquareCap, Qt::MiterJoin);
        m_doorLabel->setPos(ox + 16, oy + 16);

        setPos(ox, oy);
        setData(ITEM_TYPE, "Door_exit"); // ObjType
    }
    m_grp->addToGroup(m_doorLabel);

    this->setFlag(QGraphicsItem::ItemIsSelectable, (!m_scene->m_lockDoor));
    this->setFlag(QGraphicsItem::ItemIsMovable, (!m_scene->m_lockDoor));

    m_doorLabel->setZValue(m_scene->Z_sys_door + 0.0000020);

    setData(ITEM_ID, QString::number(0));
    setData(ITEM_ARRAY_ID, QString::number(m_data.meta.array_id));

    setZValue(m_scene->Z_sys_door);

    refreshArrows();

    if(!init)
        arrayApply();

    m_scene->unregisterElement(this);
    m_scene->registerElement(this);
}

QRectF ItemDoor::boundingRect() const
{
    return QRectF(-1, -1, m_itemSize.width() + 2, m_itemSize.height() + 2);
}

void ItemDoor::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *)
{
    painter->setBrush(m_brush);
    painter->setPen(m_pen);
    painter->drawRect(1, 1, m_itemSize.width() - 2, m_itemSize.height() - 2);

    if(this->isSelected())
    {
        painter->setPen(QPen(QBrush(Qt::black), 2, Qt::SolidLine));
        painter->drawRect(1, 1, m_itemSize.width() - 2, m_itemSize.height() - 2);
        painter->setPen(QPen(QBrush(Qt::white), 2, Qt::DotLine));
        painter->drawRect(1, 1, m_itemSize.width() - 2, m_itemSize.height() - 2);
    }
}

void ItemDoor::setScenePoint(LvlScene *theScene)
{
    LvlBaseItem::setScenePoint(theScene);
    if(m_grp) delete m_grp;
    m_grp = new QGraphicsItemGroup(this);
    m_doorLabel = nullptr;
}
