#include "historyelementsettingswarp.h"

#include <common_features/mainwinconnect.h>
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

    for(int i = 0; i < lvlScene->LvlData->doors.size(); i++){
        if(lvlScene->LvlData->doors[i].array_id == (unsigned int)array_id){
            found = true;
            doorp = &lvlScene->LvlData->doors[i];
            break;
        }
    }

    if(!found)
        return;


    if(subtype == HistorySettings::SETTING_NOYOSHI){
        doorp->novehicles = !extraData.toBool();
    }
    else
    if(subtype == HistorySettings::SETTING_ALLOWNPC){
        doorp->allownpc = !extraData.toBool();
    }
    else
    if(subtype == HistorySettings::SETTING_LOCKED){
        doorp->locked = !extraData.toBool();
    }
    else
    if(subtype == HistorySettings::SETTING_WARPTYPE){
        doorp->type = extraData.toList()[0].toInt();
    }
    else
    if(subtype == HistorySettings::SETTING_NEEDASTAR){
        doorp->stars = extraData.toList()[0].toInt();
    }
    else
    if(subtype == HistorySettings::SETTING_ENTRDIR){
        doorp->idirect = extraData.toList()[0].toInt();
    }
    else
    if(subtype == HistorySettings::SETTING_EXITDIR){
        doorp->odirect = extraData.toList()[0].toInt();
    }
    else
    if(subtype == HistorySettings::SETTING_LAYER){
        doorp->layer = extraData.toList()[0].toString();
    }
    else
    if(subtype == HistorySettings::SETTING_LEVELEXIT){
        doorp->lvl_o = !extraData.toList()[0].toBool();
        if(!doorp->lvl_o && !doorp->isSetOut && extraData.toList().size() >= 3){
            doorp->ox = extraData.toList()[1].toInt();
            doorp->oy = extraData.toList()[2].toInt();
            doorp->isSetOut = true;
            lvlScene->placeDoorExit(*doorp);
        }
    }
    else
    if(subtype == HistorySettings::SETTING_LEVELENTR){
        doorp->lvl_i = !extraData.toList()[0].toBool();
        if(!doorp->lvl_i && !doorp->isSetIn && extraData.toList().size() >= 3){
            doorp->ix = extraData.toList()[1].toInt();
            doorp->iy = extraData.toList()[2].toInt();
            doorp->isSetIn = true;
            lvlScene->placeDoorEnter(*doorp);
        }
    }
    else
    if(subtype == HistorySettings::SETTING_LEVELWARPTO){
        doorp->warpto = extraData.toList()[0].toInt();
    }

    MainWinConnect::pMainWin->dock_LvlWarpProps->setDoorData(-2);
    lvlScene->doorPointsSync(array_id);
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

    for(int i = 0; i < lvlScene->LvlData->doors.size(); i++){
        if(lvlScene->LvlData->doors[i].array_id == (unsigned int)array_id){
            found = true;
            doorp = &lvlScene->LvlData->doors[i];
            break;
        }
    }

    if(!found)
        return;


    if(subtype == HistorySettings::SETTING_NOYOSHI){
        doorp->novehicles = extraData.toBool();
    }
    else
    if(subtype == HistorySettings::SETTING_ALLOWNPC){
        doorp->allownpc = extraData.toBool();
    }
    else
    if(subtype == HistorySettings::SETTING_LOCKED){
        doorp->locked = extraData.toBool();
    }
    else
    if(subtype == HistorySettings::SETTING_WARPTYPE){
        doorp->type = extraData.toList()[1].toInt();
    }
    else
    if(subtype == HistorySettings::SETTING_NEEDASTAR){
        doorp->stars = extraData.toList()[1].toInt();
    }
    else
    if(subtype == HistorySettings::SETTING_ENTRDIR){
        doorp->idirect = extraData.toList()[1].toInt();
    }
    else
    if(subtype == HistorySettings::SETTING_EXITDIR){
        doorp->odirect = extraData.toList()[1].toInt();
    }
    else
    if(subtype == HistorySettings::SETTING_LAYER){
        doorp->layer = extraData.toList()[1].toString();
    }
    else
    if(subtype == HistorySettings::SETTING_LEVELEXIT){
        doorp->lvl_o = extraData.toList()[0].toBool();
        if(!(((!doorp->lvl_o) && (!doorp->lvl_i)) || (doorp->lvl_i)))
        {
            doorp->ox = extraData.toList()[1].toInt();
            doorp->oy = extraData.toList()[2].toInt();
        }
    }
    else
    if(subtype == HistorySettings::SETTING_LEVELENTR){
        doorp->lvl_i = extraData.toList()[0].toBool();
        if(!(((!doorp->lvl_o) && (!doorp->lvl_i)) || ((doorp->lvl_o) && (!doorp->lvl_i))))
        {
            doorp->ix = extraData.toList()[1].toInt();
            doorp->iy = extraData.toList()[2].toInt();
        }
    }
    else
    if(subtype == HistorySettings::SETTING_LEVELWARPTO){
        doorp->warpto = extraData.toList()[1].toInt();
    }

    MainWinConnect::pMainWin->dock_LvlWarpProps->setDoorData(-2);
    lvlScene->doorPointsSync(array_id);


}
