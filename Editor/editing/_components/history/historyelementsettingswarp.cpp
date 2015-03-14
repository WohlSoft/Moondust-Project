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
    int index = -1;
    QVariant extraData = m_modData;
    LevelDoor * doorp;
    bool found = false;

    for(int i = 0; i < lvlScene->LvlData->doors.size(); i++){
        if(lvlScene->LvlData->doors[i].array_id == (unsigned int)array_id){
            found = true;
            doorp = lvlScene->LvlData->doors.data();
            index = i;
            break;
        }
    }

    if(!found)
        return;


    if(subtype == HistorySettings::SETTING_NOYOSHI){
        doorp[index].novehicles = !extraData.toBool();
    }
    else
    if(subtype == HistorySettings::SETTING_ALLOWNPC){
        doorp[index].allownpc = !extraData.toBool();
    }
    else
    if(subtype == HistorySettings::SETTING_LOCKED){
        doorp[index].locked = !extraData.toBool();
    }
    else
    if(subtype == HistorySettings::SETTING_WARPTYPE){
        doorp[index].type = extraData.toList()[0].toInt();
    }
    else
    if(subtype == HistorySettings::SETTING_NEEDASTAR){
        doorp[index].stars = extraData.toList()[0].toInt();
    }
    else
    if(subtype == HistorySettings::SETTING_ENTRDIR){
        doorp[index].idirect = extraData.toList()[0].toInt();
    }
    else
    if(subtype == HistorySettings::SETTING_EXITDIR){
        doorp[index].odirect = extraData.toList()[0].toInt();
    }
    else
    if(subtype == HistorySettings::SETTING_LEVELEXIT){
        doorp[index].lvl_o = !extraData.toList()[0].toBool();
        if(!doorp[index].lvl_o && !doorp[index].isSetOut && extraData.toList().size() >= 3){
            doorp[index].ox = extraData.toList()[1].toInt();
            doorp[index].oy = extraData.toList()[2].toInt();
            doorp[index].isSetOut = true;
            lvlScene->placeDoorExit(doorp[index]);
        }
    }
    else
    if(subtype == HistorySettings::SETTING_LEVELENTR){
        doorp[index].lvl_i = !extraData.toList()[0].toBool();
        if(!doorp[index].lvl_i && !doorp[index].isSetIn && extraData.toList().size() >= 3){
            doorp[index].ix = extraData.toList()[1].toInt();
            doorp[index].iy = extraData.toList()[2].toInt();
            doorp[index].isSetIn = true;
            lvlScene->placeDoorEnter(doorp[index]);
        }
    }
    else
    if(subtype == HistorySettings::SETTING_LEVELWARPTO){
        doorp[index].warpto = extraData.toList()[0].toInt();
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
    int index = -1;
    QVariant extraData = m_modData;
    LevelDoor * doorp;
    bool found = false;

    for(int i = 0; i < lvlScene->LvlData->doors.size(); i++){
        if(lvlScene->LvlData->doors[i].array_id == (unsigned int)array_id){
            found = true;
            doorp = lvlScene->LvlData->doors.data();
            index = i;
            break;
        }
    }

    if(!found)
        return;


    if(subtype == HistorySettings::SETTING_NOYOSHI){
        doorp[index].novehicles = extraData.toBool();
    }
    else
    if(subtype == HistorySettings::SETTING_ALLOWNPC){
        doorp[index].allownpc = extraData.toBool();
    }
    else
    if(subtype == HistorySettings::SETTING_LOCKED){
        doorp[index].locked = extraData.toBool();
    }
    else
    if(subtype == HistorySettings::SETTING_WARPTYPE){
        doorp[index].type = extraData.toList()[1].toInt();
    }
    else
    if(subtype == HistorySettings::SETTING_NEEDASTAR){
        doorp[index].stars = extraData.toList()[1].toInt();
    }
    else
    if(subtype == HistorySettings::SETTING_ENTRDIR){
        doorp[index].idirect = extraData.toList()[1].toInt();
    }
    else
    if(subtype == HistorySettings::SETTING_EXITDIR){
        doorp[index].odirect = extraData.toList()[1].toInt();
    }
    else
    if(subtype == HistorySettings::SETTING_LEVELEXIT){
        doorp[index].lvl_o = extraData.toList()[0].toBool();
        if(!(((!doorp[index].lvl_o) && (!doorp[index].lvl_i)) || (doorp[index].lvl_i)))
        {
            doorp[index].ox = extraData.toList()[1].toInt();
            doorp[index].oy = extraData.toList()[2].toInt();
        }
    }
    else
    if(subtype == HistorySettings::SETTING_LEVELENTR){
        doorp[index].lvl_i = extraData.toList()[0].toBool();
        if(!(((!doorp[index].lvl_o) && (!doorp[index].lvl_i)) || ((doorp[index].lvl_o) && (!doorp[index].lvl_i))))
        {
            doorp[index].ix = extraData.toList()[1].toInt();
            doorp[index].iy = extraData.toList()[2].toInt();
        }
    }
    else
    if(subtype == HistorySettings::SETTING_LEVELWARPTO){
        doorp[index].warpto = extraData.toList()[1].toInt();
    }

    MainWinConnect::pMainWin->dock_LvlWarpProps->setDoorData(-2);
    lvlScene->doorPointsSync(array_id);


}
