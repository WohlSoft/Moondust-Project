#include "historyelementsettingsevent.h"
#include <common_features/mainwinconnect.h>
#include <main_window/dock/lvl_events_box.h>


HistoryElementSettingsEvent::HistoryElementSettingsEvent(int array_id, HistorySettings::LevelSettingSubType subtype, QVariant extraData, QObject* parent) :
    QObject(parent),
    m_array_id(array_id),
    m_subtype(subtype),
    m_modData(extraData)
{}

HistoryElementSettingsEvent::~HistoryElementSettingsEvent()
{}

QString HistoryElementSettingsEvent::getHistoryName()
{
    return HistorySettings::settingToString(m_subtype);
}

void HistoryElementSettingsEvent::undo()
{
    if(!m_scene)
        return;

    LvlScene* lvlScene;
    if(!(lvlScene = qobject_cast<LvlScene*>(m_scene)))
        return;

    HistorySettings::LevelSettingSubType subtype = m_subtype;
    int array_id = m_array_id;
    QVariant extraData = m_modData;
    LevelSMBX64Event * eventp;
    bool found = false;

    for(int i = 0; i < lvlScene->LvlData->events.size(); i++){
        if(lvlScene->LvlData->events[i].array_id == (unsigned int)array_id){
            found = true;
            eventp = &lvlScene->LvlData->events[i];
            break;
        }
    }

    if(!found)
        return;

    MainWinConnect::pMainWin->dock_LvlEvents->setEventToolsLocked(true);
    if(subtype == HistorySettings::SETTING_EV_AUTOSTART){
        eventp->autostart = !extraData.toBool();
    }
    else
    if(subtype == HistorySettings::SETTING_EV_SMOKE){
        eventp->nosmoke = !extraData.toBool();
    }
    else
    if(subtype == HistorySettings::SETTING_EV_LSHOWADD){
        QString layer = extraData.toString();
        if(!eventp->layers_show.isEmpty()){
            for(int i = 0; i < eventp->layers_show.size(); i++){
                if(eventp->layers_show[i] == layer){
                    eventp->layers_show.removeAt(i);
                }
            }
        }
    }
    else
    if(subtype == HistorySettings::SETTING_EV_LHIDEADD){
        QString layer = extraData.toString();
        if(!eventp->layers_hide.isEmpty()){
            for(int i = 0; i < eventp->layers_hide.size(); i++){
                if(eventp->layers_hide[i] == layer){
                    eventp->layers_hide.removeAt(i);
                }
            }
        }
    }
    else
    if(subtype == HistorySettings::SETTING_EV_LTOGADD){
        QString layer = extraData.toString();
        if(!eventp->layers_toggle.isEmpty()){
            for(int i = 0; i < eventp->layers_toggle.size(); i++){
                if(eventp->layers_toggle[i] == layer){
                    eventp->layers_toggle.removeAt(i);
                }
            }
        }
    }
    else
    if(subtype == HistorySettings::SETTING_EV_LSHOWDEL){
        QString layer = extraData.toString();
        eventp->layers_show.push_back(layer);
    }
    else
    if(subtype == HistorySettings::SETTING_EV_LHIDEDEL){
        QString layer = extraData.toString();
        eventp->layers_hide.push_back(layer);
    }
    else
    if(subtype == HistorySettings::SETTING_EV_LTOGDEL){
        QString layer = extraData.toString();
        eventp->layers_toggle.push_back(layer);
    }
    else
    if(subtype == HistorySettings::SETTING_EV_MOVELAYER){
        QString layer = extraData.toList()[0].toString();
        eventp->movelayer = layer;
    }
    else
    if(subtype == HistorySettings::SETTING_EV_SPEEDLAYERX){
        eventp->layer_speed_x = extraData.toList()[0].toDouble();
    }
    else
    if(subtype == HistorySettings::SETTING_EV_SPEEDLAYERY){
        eventp->layer_speed_y = extraData.toList()[0].toDouble();
    }
    else
    if(subtype == HistorySettings::SETTING_EV_AUTOSCRSEC){
        eventp->scroll_section = (long)extraData.toList()[0].toLongLong();
    }
    else
    if(subtype == HistorySettings::SETTING_EV_AUTOSCRX){
        eventp->move_camera_x = extraData.toList()[0].toDouble();
    }
    else
    if(subtype == HistorySettings::SETTING_EV_AUTOSCRY){
        eventp->move_camera_y = extraData.toList()[0].toDouble();
    }
    else
    if(subtype == HistorySettings::SETTING_EV_SECSIZE){
        eventp->sets[(int)extraData.toList()[0].toLongLong()].position_top = (long)extraData.toList()[1].toLongLong();
        eventp->sets[(int)extraData.toList()[0].toLongLong()].position_right = (long)extraData.toList()[2].toLongLong();
        eventp->sets[(int)extraData.toList()[0].toLongLong()].position_bottom = (long)extraData.toList()[3].toLongLong();
        eventp->sets[(int)extraData.toList()[0].toLongLong()].position_left = (long)extraData.toList()[4].toLongLong();
    }
    else
    if(subtype == HistorySettings::SETTING_EV_SECMUS){
        eventp->sets[(int)extraData.toList()[0].toLongLong()].music_id = (long)extraData.toList()[1].toLongLong();
    }
    else
    if(subtype == HistorySettings::SETTING_EV_SECBG){
        eventp->sets[(int)extraData.toList()[0].toLongLong()].background_id = (long)extraData.toList()[1].toLongLong();
    }
    else
    if(subtype == HistorySettings::SETTING_EV_MSG){
        eventp->msg = extraData.toList()[0].toString();
    }
    else
    if(subtype == HistorySettings::SETTING_EV_SOUND){
        eventp->sound_id = (long)extraData.toList()[0].toLongLong();
    }
    else
    if(subtype == HistorySettings::SETTING_EV_ENDGAME){
        eventp->end_game = (long)extraData.toList()[0].toLongLong();
    }
    else
    if(subtype == HistorySettings::SETTING_EV_KUP){
        eventp->ctrl_up = !extraData.toBool();
    }
    else
    if(subtype == HistorySettings::SETTING_EV_KDOWN){
        eventp->ctrl_down = !extraData.toBool();
    }
    else
    if(subtype == HistorySettings::SETTING_EV_KLEFT){
        eventp->ctrl_left = !extraData.toBool();
    }
    else
    if(subtype == HistorySettings::SETTING_EV_KRIGHT){
        eventp->ctrl_right = !extraData.toBool();
    }
    else
    if(subtype == HistorySettings::SETTING_EV_KRUN){
        eventp->ctrl_run = !extraData.toBool();
    }
    else
    if(subtype == HistorySettings::SETTING_EV_KALTRUN){
        eventp->ctrl_altrun = !extraData.toBool();
    }
    else
    if(subtype == HistorySettings::SETTING_EV_KJUMP){
        eventp->ctrl_jump = !extraData.toBool();
    }
    else
    if(subtype == HistorySettings::SETTING_EV_KALTJUMP){
        eventp->ctrl_altjump = !extraData.toBool();
    }
    else
    if(subtype == HistorySettings::SETTING_EV_KDROP){
        eventp->ctrl_drop = !extraData.toBool();
    }
    else
    if(subtype == HistorySettings::SETTING_EV_KSTART){
        eventp->ctrl_start = !extraData.toBool();
    }
    else
    if(subtype == HistorySettings::SETTING_EV_TRIACTIVATE){
        eventp->trigger = extraData.toList()[0].toString();
    }
    else
    if(subtype == HistorySettings::SETTING_EV_TRIDELAY){
        eventp->trigger_timer = (long)extraData.toList()[0].toLongLong();
    }


    MainWinConnect::pMainWin->dock_LvlEvents->setEventData(-2);
    MainWinConnect::pMainWin->dock_LvlEvents->setEventToolsLocked(false);
}

void HistoryElementSettingsEvent::redo()
{
    if(!m_scene)
        return;

    LvlScene* lvlScene;
    if(!(lvlScene = qobject_cast<LvlScene*>(m_scene)))
        return;

    HistorySettings::LevelSettingSubType subtype = m_subtype;
    int array_id = m_array_id;
    QVariant extraData = m_modData;
    LevelSMBX64Event * eventp;
    bool found = false;

    for(int i = 0; i < lvlScene->LvlData->events.size(); i++){
        if(lvlScene->LvlData->events[i].array_id == (unsigned int)array_id){
            found = true;
            eventp = &lvlScene->LvlData->events[i];
            break;
        }
    }

    if(!found)
        return;

    MainWinConnect::pMainWin->dock_LvlEvents->setEventToolsLocked(true);
    if(subtype == HistorySettings::SETTING_EV_AUTOSTART){
        eventp->autostart = extraData.toBool();
    }
    else
    if(subtype == HistorySettings::SETTING_EV_SMOKE){
        eventp->nosmoke = extraData.toBool();
    }
    else
    if(subtype == HistorySettings::SETTING_EV_LSHOWADD){
        QString layer = extraData.toString();
        eventp->layers_show.push_back(layer);
    }
    else
    if(subtype == HistorySettings::SETTING_EV_LHIDEADD){
        QString layer = extraData.toString();
        eventp->layers_hide.push_back(layer);
    }
    else
    if(subtype == HistorySettings::SETTING_EV_LTOGADD){
        QString layer = extraData.toString();
        eventp->layers_toggle.push_back(layer);
    }
    else
    if(subtype == HistorySettings::SETTING_EV_LSHOWDEL){
        QString layer = extraData.toString();
        if(!eventp->layers_show.isEmpty()){
            for(int i = 0; i < eventp->layers_show.size(); i++){
                if(eventp->layers_show[i] == layer){
                    eventp->layers_show.removeAt(i);
                }
            }
        }
    }
    else
    if(subtype == HistorySettings::SETTING_EV_LHIDEDEL){
        QString layer = extraData.toString();
        if(!eventp->layers_hide.isEmpty()){
            for(int i = 0; i < eventp->layers_hide.size(); i++){
                if(eventp->layers_hide[i] == layer){
                    eventp->layers_hide.removeAt(i);
                }
            }
        }
    }
    else
    if(subtype == HistorySettings::SETTING_EV_LTOGDEL){
        QString layer = extraData.toString();
        if(!eventp->layers_toggle.isEmpty()){
            for(int i = 0; i < eventp->layers_toggle.size(); i++){
                if(eventp->layers_toggle[i] == layer){
                    eventp->layers_toggle.removeAt(i);
                }
            }
        }
    }
    else
    if(subtype == HistorySettings::SETTING_EV_MOVELAYER){
        QString layer = extraData.toList()[1].toString();
        eventp->movelayer = layer;
    }
    else
    if(subtype == HistorySettings::SETTING_EV_SPEEDLAYERX){
        eventp->layer_speed_x = extraData.toList()[1].toDouble();
    }
    else
    if(subtype == HistorySettings::SETTING_EV_SPEEDLAYERY){
        eventp->layer_speed_y = extraData.toList()[1].toDouble();
    }
    else
    if(subtype == HistorySettings::SETTING_EV_AUTOSCRSEC){
        eventp->scroll_section = (long)extraData.toList()[1].toLongLong();
    }
    else
    if(subtype == HistorySettings::SETTING_EV_AUTOSCRX){
        eventp->move_camera_x = extraData.toList()[1].toDouble();
    }
    else
    if(subtype == HistorySettings::SETTING_EV_AUTOSCRY){
        eventp->move_camera_y = extraData.toList()[1].toDouble();
    }
    else
    if(subtype == HistorySettings::SETTING_EV_SECSIZE){
        eventp->sets[(int)extraData.toList()[0].toLongLong()].position_top = (long)extraData.toList()[5].toLongLong();
        eventp->sets[(int)extraData.toList()[0].toLongLong()].position_right = (long)extraData.toList()[6].toLongLong();
        eventp->sets[(int)extraData.toList()[0].toLongLong()].position_bottom = (long)extraData.toList()[7].toLongLong();
        eventp->sets[(int)extraData.toList()[0].toLongLong()].position_left = (long)extraData.toList()[8].toLongLong();
    }
    else
    if(subtype == HistorySettings::SETTING_EV_SECMUS){
        eventp->sets[(int)extraData.toList()[0].toLongLong()].music_id = (long)extraData.toList()[2].toLongLong();
    }
    else
    if(subtype == HistorySettings::SETTING_EV_SECBG){
        eventp->sets[(int)extraData.toList()[0].toLongLong()].background_id = (long)extraData.toList()[2].toLongLong();
    }
    else
    if(subtype == HistorySettings::SETTING_EV_MSG){
        eventp->msg = extraData.toList()[1].toString();
    }
    else
    if(subtype == HistorySettings::SETTING_EV_SOUND){
        eventp->sound_id = (long)extraData.toList()[1].toLongLong();
    }
    else
    if(subtype == HistorySettings::SETTING_EV_ENDGAME){
        eventp->end_game = (long)extraData.toList()[1].toLongLong();
    }
    else
    if(subtype == HistorySettings::SETTING_EV_KUP){
        eventp->ctrl_up = extraData.toBool();
    }
    else
    if(subtype == HistorySettings::SETTING_EV_KDOWN){
        eventp->ctrl_down = extraData.toBool();
    }
    else
    if(subtype == HistorySettings::SETTING_EV_KLEFT){
        eventp->ctrl_left = extraData.toBool();
    }
    else
    if(subtype == HistorySettings::SETTING_EV_KRIGHT){
        eventp->ctrl_right = extraData.toBool();
    }
    else
    if(subtype == HistorySettings::SETTING_EV_KRUN){
        eventp->ctrl_run = extraData.toBool();
    }
    else
    if(subtype == HistorySettings::SETTING_EV_KALTRUN){
        eventp->ctrl_altrun = extraData.toBool();
    }
    else
    if(subtype == HistorySettings::SETTING_EV_KJUMP){
        eventp->ctrl_jump = extraData.toBool();
    }
    else
    if(subtype == HistorySettings::SETTING_EV_KALTJUMP){
        eventp->ctrl_altjump = extraData.toBool();
    }
    else
    if(subtype == HistorySettings::SETTING_EV_KDROP){
        eventp->ctrl_drop = extraData.toBool();
    }
    else
    if(subtype == HistorySettings::SETTING_EV_KSTART){
        eventp->ctrl_start = extraData.toBool();
    }
    else
    if(subtype == HistorySettings::SETTING_EV_TRIACTIVATE){
        eventp->trigger = extraData.toList()[1].toString();
    }
    else
    if(subtype == HistorySettings::SETTING_EV_TRIDELAY){
        eventp->trigger_timer = (long)extraData.toList()[1].toLongLong();
    }


    MainWinConnect::pMainWin->dock_LvlEvents->setEventData(-2);
    MainWinConnect::pMainWin->dock_LvlEvents->setEventToolsLocked(false);
}
