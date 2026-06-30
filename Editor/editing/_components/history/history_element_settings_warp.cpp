#include "history_element_settings_warp.h"

#include <common_features/logger.h>
#include <editing/_components/history/item_searcher.h>
#include <editing/_scenes/level/items/item_door.h>

#include <common_features/main_window_ptr.h>
#include <main_window/dock/lvl_warp_props.h>

HistoryElementSettingsWarp::HistoryElementSettingsWarp(int array_id, HistorySettings::LevelSettingSubType subtype, QVariant extraData, QObject *parent) :
    QObject(parent),
    m_array_id(array_id),
    m_subtype(subtype),
    m_modData(extraData)
{}

HistoryElementSettingsWarp::~HistoryElementSettingsWarp()
{}

QString HistoryElementSettingsWarp::getHistoryName()
{
    if(qobject_cast<LvlScene*>(m_scene)){
        return HistorySettings::settingToString(m_subtype);
    }
    return QString("<unknown>");
}

void HistoryElementSettingsWarp::undo()
{
    if(!m_scene)
        return;

    LvlScene* lvlScene;
    if(!(lvlScene = qobject_cast<LvlScene*>(m_scene)))
        return;

    HistorySettings::LevelSettingSubType subtype = m_subtype;
    int array_id = m_array_id;
    QVariant extraData = m_modData;
    LevelDoor  *doorp;
    bool found = false;
    bool restorePointState = false;
    QList<QVariant> pointState;

    for(int i = 0; i < lvlScene->m_data->doors.size(); i++)
    {
        if(lvlScene->m_data->doors[i].meta.array_id == (unsigned int)array_id)
        {
            found = true;
            doorp = &lvlScene->m_data->doors[i];
            break;
        }
    }

    if(!found)
        return;


    if(subtype == HistorySettings::SETTING_TWOWAY)
        doorp->two_way = !extraData.toBool();
    else if(subtype == HistorySettings::SETTING_NOVEHICLE)
        doorp->novehicles = !extraData.toBool();
    else if(subtype == HistorySettings::SETTING_ALLOWNPC)
        doorp->allownpc = !extraData.toBool();
    else if(subtype == HistorySettings::SETTING_ALLOWNPC_IL)
        doorp->allownpc_interlevel = !extraData.toBool();
    else if(subtype == HistorySettings::SETTING_LOCKED)
        doorp->locked = !extraData.toBool();
    else if(subtype == HistorySettings::SETTING_NEED_A_BOMB)
        doorp->need_a_bomb = !extraData.toBool();
    else if(subtype == HistorySettings::SETTING_HIDE_STAR_NUMBER)
        doorp->star_num_hide = !extraData.toBool();
    else if(subtype == HistorySettings::SETTING_ENABLE_CANNON)
        doorp->cannon_exit = !extraData.toBool();
    else if(subtype == HistorySettings::SETTING_W_SPECIAL_STATE_REQUIRED)
        doorp->special_state_required = !extraData.toBool();
    else if(subtype == HistorySettings::SETTING_W_NEEDS_FLOOR)
        doorp->stood_state_required = !extraData.toBool();
    else if(subtype == HistorySettings::SETTING_HIDE_LEVEL_ENTER_SCENE)
        doorp->hide_entering_scene = !extraData.toBool();
    else if(subtype == HistorySettings::SETTING_WARPTYPE)
        doorp->type = extraData.toList()[0].toInt();
    else if(subtype == HistorySettings::SETTING_TRANSITTYPE)
        doorp->transition_effect = extraData.toList()[0].toInt();
    else if(subtype == HistorySettings::SETTING_NEEDASTAR)
        doorp->stars = extraData.toList()[0].toInt();
    else if(subtype == HistorySettings::SETTING_NEEDASTAR_MSG)
        doorp->stars_msg = extraData.toList()[0].toString();
    else if(subtype == HistorySettings::SETTING_CANNON_SPEED)
        doorp->cannon_exit_speed = extraData.toList()[0].toFloat();
    else if(subtype == HistorySettings::SETTING_ENTRDIR)
        doorp->idirect = extraData.toList()[0].toInt();
    else if(subtype == HistorySettings::SETTING_EXITDIR)
        doorp->odirect = extraData.toList()[0].toInt();
    else if(subtype == HistorySettings::SETTING_LAYER)
        doorp->layer = extraData.toList()[0].toString();
    else if(subtype == HistorySettings::SETTING_EV_WARP_ENTER)
        doorp->event_enter = extraData.toList()[0].toString();
    else if(subtype == HistorySettings::SETTING_EV_WARP_EXIT)
        doorp->event_exit = extraData.toList()[0].toString();
    else if(subtype == HistorySettings::SETTING_LEVELEXIT)
    {
        auto d = extraData.toList();
        doorp->lvl_o = !d[0].toBool();
        restorePointState = true;
        pointState = d;
    }
    else if(subtype == HistorySettings::SETTING_LEVELENTR)
    {
        auto d = extraData.toList();
        doorp->lvl_i = !d[0].toBool();
        restorePointState = true;
        pointState = d;
    }
    else if(subtype == HistorySettings::SETTING_LEVELWARPTO)
        doorp->warpto = extraData.toList()[0].toInt();

    if(restorePointState && pointState.size() >= 13)
    {
        const bool oldIn = pointState[1].toBool();
        const int oldIX = pointState[2].toInt();
        const int oldIY = pointState[3].toInt();

        const bool oldOut = pointState[4].toBool();
        const int oldOX = pointState[5].toInt();
        const int oldOY = pointState[6].toInt();

        const bool newIn = pointState[7].toBool();
        const bool newOut = pointState[10].toBool();

        qDebug().noquote().nospace() << "Undo Warp Setup: New: I=" << oldIn << ">>" << newIn << " (x=" << oldIX << ", y=" << oldIY << ") " <<
                                        "O=" << oldOut << ">>" << newOut << " (x=" << oldOX << ", y=" << oldOY << ")";

        if(oldIn != newIn)
        {
            if(!newIn && oldIn) // Place the point back
            {
                doorp->ix = oldIX;
                doorp->iy = oldIY;
                doorp->isSetIn = true;
                lvlScene->placeDoorEnter(*doorp);
            }
            else if(newIn && !oldIn) // Remove item
                removeDoorEntry(doorp, true);
        }

        if(oldOut != newOut)
        {
            if(!newOut && oldOut) // Place the point back
            {
                doorp->ox = oldOX;
                doorp->oy = oldOY;
                doorp->isSetOut = true;
                lvlScene->placeDoorExit(*doorp);
            }
            else if(newOut && !oldOut) // Remove item
                removeDoorEntry(doorp, false);
        }
    }
    else
        lvlScene->doorPointsSync(array_id);

    MainWinConnect::pMainWin->dock_LvlWarpProps->setDoorData(-2);
}

void HistoryElementSettingsWarp::redo()
{
    if(!m_scene)
        return;

    LvlScene* lvlScene;
    if(!(lvlScene = qobject_cast<LvlScene*>(m_scene)))
        return;

    HistorySettings::LevelSettingSubType subtype = m_subtype;
    int array_id = m_array_id;
    QVariant extraData = m_modData;
    LevelDoor *doorp;
    bool found = false;
    bool restorePointState = false;
    QList<QVariant> pointState;

    for(int i = 0; i < lvlScene->m_data->doors.size(); i++)
    {
        if(lvlScene->m_data->doors[i].meta.array_id == (unsigned int)array_id)
        {
            found = true;
            doorp = &lvlScene->m_data->doors[i];
            break;
        }
    }

    if(!found)
        return;


    if(subtype == HistorySettings::SETTING_TWOWAY)
        doorp->two_way = extraData.toBool();
    else if(subtype == HistorySettings::SETTING_NOVEHICLE)
        doorp->novehicles = extraData.toBool();
    else if(subtype == HistorySettings::SETTING_ALLOWNPC)
        doorp->allownpc = extraData.toBool();
    else if(subtype == HistorySettings::SETTING_ALLOWNPC_IL)
        doorp->allownpc_interlevel = extraData.toBool();
    else if(subtype == HistorySettings::SETTING_LOCKED)
        doorp->locked = extraData.toBool();
    else if(subtype == HistorySettings::SETTING_NEED_A_BOMB)
        doorp->need_a_bomb = extraData.toBool();
    else if(subtype == HistorySettings::SETTING_HIDE_STAR_NUMBER)
        doorp->star_num_hide = extraData.toBool();
    else if(subtype == HistorySettings::SETTING_ENABLE_CANNON)
        doorp->cannon_exit = extraData.toBool();
    else if(subtype == HistorySettings::SETTING_W_SPECIAL_STATE_REQUIRED)
        doorp->special_state_required = extraData.toBool();
    else if(subtype == HistorySettings::SETTING_W_NEEDS_FLOOR)
        doorp->stood_state_required = extraData.toBool();
    else if(subtype == HistorySettings::SETTING_HIDE_LEVEL_ENTER_SCENE)
        doorp->hide_entering_scene = extraData.toBool();
    else if(subtype == HistorySettings::SETTING_WARPTYPE)
        doorp->type = extraData.toList()[1].toInt();
    else if(subtype == HistorySettings::SETTING_TRANSITTYPE)
        doorp->transition_effect = extraData.toList()[1].toInt();
    else if(subtype == HistorySettings::SETTING_NEEDASTAR)
        doorp->stars = extraData.toList()[1].toInt();
    else if(subtype == HistorySettings::SETTING_NEEDASTAR_MSG)
        doorp->stars_msg = extraData.toList()[1].toString();
    else if(subtype == HistorySettings::SETTING_CANNON_SPEED)
        doorp->cannon_exit_speed = extraData.toList()[1].toFloat();
    else if(subtype == HistorySettings::SETTING_ENTRDIR)
        doorp->idirect = extraData.toList()[1].toInt();
    else if(subtype == HistorySettings::SETTING_EXITDIR)
        doorp->odirect = extraData.toList()[1].toInt();
    else if(subtype == HistorySettings::SETTING_LAYER)
        doorp->layer = extraData.toList()[1].toString();

    else if(subtype == HistorySettings::SETTING_EV_WARP_ENTER)
        doorp->event_enter = extraData.toList()[1].toString();
    else if(subtype == HistorySettings::SETTING_EV_WARP_EXIT)
        doorp->event_exit = extraData.toList()[1].toString();
    else if(subtype == HistorySettings::SETTING_LEVELEXIT)
    {
        auto d = extraData.toList();
        doorp->lvl_o = d[0].toBool();
        restorePointState = true;
        pointState = d;
    }
    else if(subtype == HistorySettings::SETTING_LEVELENTR)
    {
        auto d = extraData.toList();
        doorp->lvl_i = d[0].toBool();
        restorePointState = true;
        pointState = d;
    }
    else if(subtype == HistorySettings::SETTING_LEVELWARPTO)
        doorp->warpto = extraData.toList()[1].toInt();

    if(restorePointState && pointState.size() >= 13)
    {
        const bool newIn = pointState[7].toBool();
        const int newIX = pointState[8].toInt();
        const int newIY = pointState[9].toInt();

        const bool newOut = pointState[10].toBool();
        const int newOX = pointState[11].toInt();
        const int newOY = pointState[12].toInt();

        const bool oldIn = pointState[1].toBool();
        const bool oldOut = pointState[4].toBool();

        qDebug().noquote().nospace() << "Redo Warp Setup: New: I=" << newIn << ">>" << oldIn << " (x=" << newIX << ", y=" << newIY << ") " <<
                                        "O=" << newOut << ">>" << oldOut << " (x=" << newOX << ", y=" << newOY << ")";

        if(oldIn != newIn)
        {
            if(newIn && !oldIn) // Place the point back
            {
                doorp->ix = newIX;
                doorp->iy = newIY;
                doorp->isSetIn = true;
                lvlScene->placeDoorEnter(*doorp);
            }
            else if(!newIn && oldIn) // Remove item
                removeDoorEntry(doorp, true);
        }

        if(newOut != oldOut)
        {
            if(!oldOut && newOut) // Place the point back
            {
                doorp->ox = newOX;
                doorp->oy = newOY;
                doorp->isSetOut = true;
                lvlScene->placeDoorExit(*doorp);
            }
            else if(oldOut && !newOut) // Remove item
                removeDoorEntry(doorp, false);
        }
    }
    else
        lvlScene->doorPointsSync(array_id);

    MainWinConnect::pMainWin->dock_LvlWarpProps->setDoorData(-2);
}

void HistoryElementSettingsWarp::removeDoorEntry(LevelDoor *door, bool enter)
{
    LvlScene* lvlScene;
    if(!(lvlScene = qobject_cast<LvlScene*>(m_scene)))
        return;

    ItemSearcher searcher(enter ? ItemTypes::LVL_S_DoorEnter : ItemTypes::LVL_S_DoorExit);

    LevelData data;
    data.doors << *door;

    QObject::connect(&searcher, &ItemSearcher::foundDoor, this, &HistoryElementSettingsWarp::historyRemoveDoorPoints);
    searcher.find(data, lvlScene);
}

void HistoryElementSettingsWarp::historyRemoveDoorPoints(const LevelDoor &/*door*/, QGraphicsItem *item)
{
    ItemDoor *sceneItem = qgraphicsitem_cast<ItemDoor*>(item);

    if(sceneItem)
        sceneItem->removeFromArray();

    if(item)
        delete (item);
}
