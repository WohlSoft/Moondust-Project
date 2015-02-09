#include "historyelementmainsetting.h"

#include <editing/_scenes/level/lvl_scene.h>
#include <editing/_scenes/world/wld_scene.h>
#include <common_features/mainwinconnect.h>

HistoryElementMainSetting::HistoryElementMainSetting(HistorySettings::WorldSettingSubType wldSubType, QObject *parent) :
    QObject(parent),
    m_modWorldSetting(wldSubType)
{}

HistoryElementMainSetting::HistoryElementMainSetting(HistorySettings::LevelSettingSubType lvlSubType, QObject *parent) :
    QObject(parent),
    m_modLevelSetting(lvlSubType)
{}

HistoryElementMainSetting::HistoryElementMainSetting(HistorySettings::WorldSettingSubType wldSubType, QVariant extraData, QObject *parent) :
    QObject(parent),
    m_modWorldSetting(wldSubType),
    m_modData(extraData)
{}

HistoryElementMainSetting::HistoryElementMainSetting(HistorySettings::LevelSettingSubType lvlSubType, QVariant extraData, QObject *parent) :
    QObject(parent),
    m_modLevelSetting(lvlSubType),
    m_modData(extraData)
{}

HistoryElementMainSetting::~HistoryElementMainSetting()
{}

QString HistoryElementMainSetting::getHistoryName()
{
    if(qobject_cast<LvlScene*>(m_scene)){
        return HistorySettings::settingToString(m_modLevelSetting);
    }else if(qobject_cast<WldScene*>(m_scene)){
        return HistorySettings::settingToString(m_modWorldSetting);
    }
    return QString("Error History");
}

void HistoryElementMainSetting::undo()
{
    LvlScene* lvlScene = 0;
    WldScene* wldScene = 0;
    if(!m_scene)
        return;
    if((lvlScene = qobject_cast<LvlScene*>(m_scene))){
        processLevelUndo();
    }
    if((wldScene = qobject_cast<WldScene*>(m_scene))){
        processWorldUndo();
    }
}

void HistoryElementMainSetting::redo()
{
    LvlScene* lvlScene = 0;
    WldScene* wldScene = 0;
    if(!m_scene)
        return;
    if((lvlScene = qobject_cast<LvlScene*>(m_scene))){
        processLevelRedo();
    }
    if((wldScene = qobject_cast<WldScene*>(m_scene))){
        processWorldRedo();
    }
}

void HistoryElementMainSetting::processWorldUndo()
{
    if(!m_scene)
        return;

    WldScene* wldScene = 0;
    if(!(wldScene = qobject_cast<WldScene*>(m_scene)))
        return;

    if(m_modWorldSetting == HistorySettings::SETTING_HUB){
        wldScene->WldData->HubStyledWorld = !m_modData.toBool();
    }else if(m_modWorldSetting == HistorySettings::SETTING_RESTARTAFTERFAIL){
        wldScene->WldData->restartlevel = !m_modData.toBool();
    }else if(m_modWorldSetting == HistorySettings::SETTING_TOTALSTARS){
        wldScene->WldData->stars = m_modData.toList()[0].toInt();
    }else if(m_modWorldSetting == HistorySettings::SETTING_INTROLEVEL){
        wldScene->WldData->IntroLevel_file = m_modData.toList()[0].toString();
    }else if(m_modWorldSetting == HistorySettings::SETTING_CHARACTER){
        int ind = MainWinConnect::pMainWin->configs.getCharacterI(m_modData.toList()[0].toInt());
        if(ind!=-1)
            wldScene->WldData->nocharacter[ind] = !m_modData.toList()[1].toBool();
    }else if(m_modWorldSetting == HistorySettings::SETTING_WORLDTITLE){
        wldScene->WldData->EpisodeTitle = m_modData.toList()[0].toString();
        if(MainWinConnect::pMainWin->activeChildWindow()==3)
            MainWinConnect::pMainWin->activeWldEditWin()->setWindowTitle(
                        m_modData.toList()[0].toString() == "" ? MainWinConnect::pMainWin->activeWldEditWin()->userFriendlyCurrentFile() : m_modData.toList()[0].toString());
    }

    MainWinConnect::pMainWin->setCurrentWorldSettings();
}

void HistoryElementMainSetting::processLevelUndo()
{
    if(!m_scene)
        return;

    LvlScene* lvlScene = 0;
    if(!(lvlScene = qobject_cast<LvlScene*>(m_scene)))
        return;

    if(m_modLevelSetting == HistorySettings::SETTING_LEVELNAME){
        lvlScene->LvlData->LevelName = m_modData.toList()[0].toString();

        MainWinConnect::pMainWin->activeLvlEditWin()->setWindowTitle( lvlScene->LvlData->LevelName.isEmpty() ?
            MainWinConnect::pMainWin->activeLvlEditWin()->userFriendlyCurrentFile() : lvlScene->LvlData->LevelName );
        MainWinConnect::pMainWin->updateWindowMenu();
    }
}

void HistoryElementMainSetting::processWorldRedo()
{
    if(!m_scene)
        return;

    WldScene* wldScene = 0;
    if(!(wldScene = qobject_cast<WldScene*>(m_scene)))
        return;

    if(m_modWorldSetting == HistorySettings::SETTING_HUB){
        wldScene->WldData->HubStyledWorld = m_modData.toBool();
    }else if(m_modWorldSetting == HistorySettings::SETTING_RESTARTAFTERFAIL){
        wldScene->WldData->restartlevel = m_modData.toBool();
    }else if(m_modWorldSetting == HistorySettings::SETTING_TOTALSTARS){
        wldScene->WldData->stars = m_modData.toList()[1].toInt();
    }else if(m_modWorldSetting == HistorySettings::SETTING_INTROLEVEL){
        wldScene->WldData->IntroLevel_file = m_modData.toList()[1].toString();
    }else if(m_modWorldSetting == HistorySettings::SETTING_CHARACTER){
        int ind = MainWinConnect::pMainWin->configs.getCharacterI(m_modData.toList()[0].toInt());
        if(ind!=-1)
            wldScene->WldData->nocharacter[ind] = m_modData.toList()[1].toBool();
    }else if(m_modWorldSetting == HistorySettings::SETTING_WORLDTITLE){
        wldScene->WldData->EpisodeTitle = m_modData.toList()[1].toString();
        if(MainWinConnect::pMainWin->activeChildWindow()==3)
            MainWinConnect::pMainWin->activeWldEditWin()->setWindowTitle(
                        m_modData.toList()[1].toString() == "" ? MainWinConnect::pMainWin->activeWldEditWin()->userFriendlyCurrentFile() : m_modData.toList()[1].toString());
    }

    MainWinConnect::pMainWin->setCurrentWorldSettings();
}

void HistoryElementMainSetting::processLevelRedo()
{
    if(!m_scene)
        return;

    LvlScene* lvlScene = 0;
    if(!(lvlScene = qobject_cast<LvlScene*>(m_scene)))
        return;

    if(m_modLevelSetting == HistorySettings::SETTING_LEVELNAME){
        lvlScene->LvlData->LevelName = m_modData.toList()[1].toString();
        MainWinConnect::pMainWin->activeLvlEditWin()->setWindowTitle( lvlScene->LvlData->LevelName.isEmpty() ?
            MainWinConnect::pMainWin->activeLvlEditWin()->userFriendlyCurrentFile() : lvlScene->LvlData->LevelName );
        MainWinConnect::pMainWin->updateWindowMenu();
    }
}



