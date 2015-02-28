#include "historyelementsettingssection.h"
#include <editing/_scenes/level/lvl_scene.h>
#include <common_features/mainwinconnect.h>
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

    if(m_subtype == HistorySettings::SETTING_SECISWARP){
        lvlScene->LvlData->sections[sectionID].IsWarp = !extraData.toBool();
    }
    else
    if(m_subtype == HistorySettings::SETTING_SECOFFSCREENEXIT){
        lvlScene->LvlData->sections[sectionID].OffScreenEn = !extraData.toBool();
    }
    else
    if(m_subtype == HistorySettings::SETTING_SECNOBACK){
        lvlScene->LvlData->sections[sectionID].noback = !extraData.toBool();
    }
    else
    if(m_subtype == HistorySettings::SETTING_SECUNDERWATER){
        lvlScene->LvlData->sections[sectionID].underwater = !extraData.toBool();
    }
    else
    if(m_subtype == HistorySettings::SETTING_SECBACKGROUNDIMG){
        lvlScene->ChangeSectionBG(extraData.toList()[0].toInt(), m_sectionID);
    }
    else
    if(m_subtype == HistorySettings::SETTING_SECMUSIC){
        lvlScene->LvlData->sections[sectionID].music_id = extraData.toList()[0].toInt();
    }
    else
    if(m_subtype == HistorySettings::SETTING_SECCUSTOMMUSIC){
        lvlScene->LvlData->sections[sectionID].music_file = extraData.toList()[0].toString();
    }

    MainWinConnect::pMainWin->dock_LvlSectionProps->setLevelSectionData();
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

    if(m_subtype == HistorySettings::SETTING_SECISWARP){
        lvlScene->LvlData->sections[sectionID].IsWarp = extraData.toBool();
    }
    else
    if(m_subtype == HistorySettings::SETTING_SECOFFSCREENEXIT){
        lvlScene->LvlData->sections[sectionID].OffScreenEn = extraData.toBool();
    }
    else
    if(m_subtype == HistorySettings::SETTING_SECNOBACK){
        lvlScene->LvlData->sections[sectionID].noback = extraData.toBool();
    }
    else
    if(m_subtype == HistorySettings::SETTING_SECUNDERWATER){
        lvlScene->LvlData->sections[sectionID].underwater = extraData.toBool();
    }
    else
    if(m_subtype == HistorySettings::SETTING_SECBACKGROUNDIMG){
        lvlScene->ChangeSectionBG(extraData.toList()[1].toInt(), m_sectionID);
    }
    else
    if(m_subtype == HistorySettings::SETTING_SECMUSIC){
        lvlScene->LvlData->sections[sectionID].music_id = extraData.toList()[1].toInt();
    }
    else
    if(m_subtype == HistorySettings::SETTING_SECCUSTOMMUSIC){
        lvlScene->LvlData->sections[sectionID].music_file = extraData.toList()[1].toString();
    }
    MainWinConnect::pMainWin->dock_LvlSectionProps->setLevelSectionData();
    LvlMusPlay::updateMusic();
    MainWinConnect::pMainWin->setMusic(LvlMusPlay::musicButtonChecked);
}
