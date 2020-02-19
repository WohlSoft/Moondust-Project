#include "historyelementsettingssection.h"
#include <editing/_scenes/level/lvl_scene.h>
#include <common_features/main_window_ptr.h>
#include <main_window/dock/lvl_sctc_props.h>
#include <audio/music_player.h>

HistoryElementSettingsSection::HistoryElementSettingsSection(int sectionID, HistorySettings::LevelSettingSubType subtype, QVariant extraData, QObject *parent) :
    QObject(parent),
    m_sectionID(sectionID),
    m_subtype(subtype),
    m_extraData(extraData)
{}

HistoryElementSettingsSection::~HistoryElementSettingsSection()
{

}

QString HistoryElementSettingsSection::getHistoryName()
{
    return HistorySettings::settingToString(m_subtype);
}

void HistoryElementSettingsSection::undo()
{
    if(!m_scene)
        return;

    LvlScene* lvlScene;
    if(!(lvlScene = qobject_cast<LvlScene*>(m_scene)))
        return;

    int sectionID = m_sectionID;
    QVariant extraData = m_extraData;

    if(m_subtype == HistorySettings::SETTING_SECWRAPH){
        lvlScene->m_data->sections[sectionID].wrap_h = !extraData.toBool();
    }
    else
    if(m_subtype == HistorySettings::SETTING_SECWRAPV){
        lvlScene->m_data->sections[sectionID].wrap_v = !extraData.toBool();
    }
    else
    if(m_subtype == HistorySettings::SETTING_SECOFFSCREENEXIT){
        lvlScene->m_data->sections[sectionID].OffScreenEn = !extraData.toBool();
    }
    else
    if(m_subtype == HistorySettings::SETTING_SECNOBACK){
        lvlScene->m_data->sections[sectionID].lock_left_scroll = !extraData.toBool();
    }
    else
    if(m_subtype == HistorySettings::SETTING_SECUNDERWATER){
        lvlScene->m_data->sections[sectionID].underwater = !extraData.toBool();
    }
    else
    if(m_subtype == HistorySettings::SETTING_SECBACKGROUNDIMG){
        lvlScene->ChangeSectionBG(extraData.toList()[0].toInt(), m_sectionID);
    }
    else
    if(m_subtype == HistorySettings::SETTING_SECMUSIC){
        lvlScene->m_data->sections[sectionID].music_id = extraData.toList()[0].toInt();
    }
    else
    if(m_subtype == HistorySettings::SETTING_SECCUSTOMMUSIC){
        lvlScene->m_data->sections[sectionID].music_file = extraData.toList()[0].toString();
    }
    else
    if(m_subtype == HistorySettings::SETTING_SEC_XTRA){
        lvlScene->m_data->sections[sectionID].custom_params = extraData.toList()[0].toString();
    }

    MainWinConnect::pMainWin->dock_LvlSectionProps->refreshFileData();
    LvlMusPlay::updateMusic();
    MainWinConnect::pMainWin->setMusic(LvlMusPlay::musicButtonChecked);
}

void HistoryElementSettingsSection::redo()
{
    if(!m_scene)
        return;

    LvlScene* lvlScene;
    if(!(lvlScene = qobject_cast<LvlScene*>(m_scene)))
        return;

    int sectionID = m_sectionID;
    QVariant extraData = m_extraData;

    if(m_subtype == HistorySettings::SETTING_SECWRAPH){
        lvlScene->m_data->sections[sectionID].wrap_h = extraData.toBool();
    }
    else
    if(m_subtype == HistorySettings::SETTING_SECWRAPV){
        lvlScene->m_data->sections[sectionID].wrap_v = extraData.toBool();
    }
    else
    if(m_subtype == HistorySettings::SETTING_SECOFFSCREENEXIT){
        lvlScene->m_data->sections[sectionID].OffScreenEn = extraData.toBool();
    }
    else
    if(m_subtype == HistorySettings::SETTING_SECNOBACK){
        lvlScene->m_data->sections[sectionID].lock_left_scroll = extraData.toBool();
    }
    else
    if(m_subtype == HistorySettings::SETTING_SECUNDERWATER){
        lvlScene->m_data->sections[sectionID].underwater = extraData.toBool();
    }
    else
    if(m_subtype == HistorySettings::SETTING_SECBACKGROUNDIMG){
        lvlScene->ChangeSectionBG(extraData.toList()[1].toInt(), m_sectionID);
    }
    else
    if(m_subtype == HistorySettings::SETTING_SECMUSIC){
        lvlScene->m_data->sections[sectionID].music_id = extraData.toList()[1].toInt();
    }
    else
    if(m_subtype == HistorySettings::SETTING_SECCUSTOMMUSIC){
        lvlScene->m_data->sections[sectionID].music_file = extraData.toList()[1].toString();
    }
    else
    if(m_subtype == HistorySettings::SETTING_SEC_XTRA){
        lvlScene->m_data->sections[sectionID].custom_params = extraData.toList()[1].toString();
    }
    MainWinConnect::pMainWin->dock_LvlSectionProps->refreshFileData();
    LvlMusPlay::updateMusic();
    MainWinConnect::pMainWin->setMusic(LvlMusPlay::musicButtonChecked);
}

