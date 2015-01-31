#include "historyelementitemsetting.h"

#include <editing/_scenes/world/items/item_level.h>
#include <editing/_scenes/level/lvl_scene.h>
#include <editing/_scenes/world/wld_scene.h>
#include <common_features/mainwinconnect.h>

#include <editing/_components/history/itemsearcher.h>

HistoryElementItemSetting::HistoryElementItemSetting(const WorldData &data, HistorySettings::WorldSettingSubType wldSubType, QVariant extraData, QObject *parent) :
    QObject(parent),
    m_modWorldSetting(wldSubType),
    m_modWorldData(data),
    m_modData(extraData)
{}

HistoryElementItemSetting::HistoryElementItemSetting(const LevelData &data, HistorySettings::LevelSettingSubType lvlSubType, QVariant extraData, QObject *parent) :
    QObject(parent),
    m_modLevelSetting(lvlSubType),
    m_modLevelData(data),
    m_modData(extraData)
{}

HistoryElementItemSetting::~HistoryElementItemSetting()
{}

QString HistoryElementItemSetting::getHistoryName()
{
    if(qobject_cast<LvlScene*>(m_scene)){
        return HistorySettings::settingToString(m_modLevelSetting);
    }else if(qobject_cast<WldScene*>(m_scene)){
        return HistorySettings::settingToString(m_modWorldSetting);
    }
    return QString("Error History");
}

void HistoryElementItemSetting::undo()
{
    LvlScene* lvlScene = 0;
    WldScene* wldScene = 0;
    if(!m_scene)
        return;
    if((lvlScene = qobject_cast<LvlScene*>(m_scene))){

    }
    if((wldScene = qobject_cast<WldScene*>(m_scene))){
        processWorldUndo();
    }
}

void HistoryElementItemSetting::redo()
{
    LvlScene* lvlScene = 0;
    WldScene* wldScene = 0;
    if(!m_scene)
        return;
    if((lvlScene = qobject_cast<LvlScene*>(m_scene))){

    }
    if((wldScene = qobject_cast<WldScene*>(m_scene))){
        processWorldRedo();
    }
}

void HistoryElementItemSetting::processWorldUndo()
{
    WldScene* wldScene = 0;
    if(!m_scene)
        return;
    if((wldScene = qobject_cast<WldScene*>(m_scene))){
        ItemSearcher worldSearcher(ItemTypes::WLD_S_Level);
        if(m_modWorldSetting == HistorySettings::SETTING_PATHBACKGROUND){
            connect(&worldSearcher, SIGNAL(foundLevel(WorldLevels,QGraphicsItem*)), this, SLOT(historyUndoSettingPathBackgroundLevel(WorldLevels,QGraphicsItem*)));
        }else if(m_modWorldSetting == HistorySettings::SETTING_BIGPATHBACKGROUND){
            connect(&worldSearcher, SIGNAL(foundLevel(WorldLevels,QGraphicsItem*)), this, SLOT(historyUndoSettingBigPathBackgroundLevel(WorldLevels,QGraphicsItem*)));
        }else if(m_modWorldSetting == HistorySettings::SETTING_ALWAYSVISIBLE){
            connect(&worldSearcher, SIGNAL(foundLevel(WorldLevels,QGraphicsItem*)), this, SLOT(historyUndoSettingAlwaysVisibleLevel(WorldLevels,QGraphicsItem*)));
        }else if(m_modWorldSetting == HistorySettings::SETTING_GAMESTARTPOINT){
            connect(&worldSearcher, SIGNAL(foundLevel(WorldLevels,QGraphicsItem*)), this, SLOT(historyUndoSettingGameStartPointLevel(WorldLevels,QGraphicsItem*)));
        }else if(m_modWorldSetting == HistorySettings::SETTING_DOORID){
            connect(&worldSearcher, SIGNAL(foundLevel(WorldLevels,QGraphicsItem*)), this, SLOT(historyUndoSettingDoorIDLevel(WorldLevels,QGraphicsItem*)));
        }else if(m_modWorldSetting == HistorySettings::SETTING_PATHBYTOP){
            connect(&worldSearcher, SIGNAL(foundLevel(WorldLevels,QGraphicsItem*)), this, SLOT(historyUndoSettingPathByTopLevel(WorldLevels,QGraphicsItem*)));
        }else if(m_modWorldSetting == HistorySettings::SETTING_PATHBYRIGHT){
            connect(&worldSearcher, SIGNAL(foundLevel(WorldLevels,QGraphicsItem*)), this, SLOT(historyUndoSettingPathByRightLevel(WorldLevels,QGraphicsItem*)));
        }else if(m_modWorldSetting == HistorySettings::SETTING_PATHBYBOTTOM){
            connect(&worldSearcher, SIGNAL(foundLevel(WorldLevels,QGraphicsItem*)), this, SLOT(historyUndoSettingPathByBottomLevel(WorldLevels,QGraphicsItem*)));
        }else if(m_modWorldSetting == HistorySettings::SETTING_PATHBYLEFT){
            connect(&worldSearcher, SIGNAL(foundLevel(WorldLevels,QGraphicsItem*)), this, SLOT(historyUndoSettingPathByLeftLevel(WorldLevels,QGraphicsItem*)));
        }else if(m_modWorldSetting == HistorySettings::SETTING_GOTOX){
            connect(&worldSearcher, SIGNAL(foundLevel(WorldLevels,QGraphicsItem*)), this, SLOT(historyUndoSettingGotoXLevel(WorldLevels,QGraphicsItem*)));
        }else if(m_modWorldSetting == HistorySettings::SETTING_GOTOY){
            connect(&worldSearcher, SIGNAL(foundLevel(WorldLevels,QGraphicsItem*)), this, SLOT(historyUndoSettingGotoYLevel(WorldLevels,QGraphicsItem*)));
        }else if(m_modWorldSetting == HistorySettings::SETTING_LEVELFILE){
            connect(&worldSearcher, SIGNAL(foundLevel(WorldLevels,QGraphicsItem*)), this, SLOT(historyUndoSettingLevelfileLevel(WorldLevels,QGraphicsItem*)));
        }else if(m_modWorldSetting == HistorySettings::SETTING_LEVELTITLE){
            connect(&worldSearcher, SIGNAL(foundLevel(WorldLevels,QGraphicsItem*)), this, SLOT(historyUndoSettingLeveltitleLevel(WorldLevels,QGraphicsItem*)));
        }
        worldSearcher.find(m_modWorldData, m_scene->items());
    }
}

void HistoryElementItemSetting::processWorldRedo()
{
    WldScene* wldScene = 0;
    if(!m_scene)
        return;
    if((wldScene = qobject_cast<WldScene*>(m_scene))){
        ItemSearcher worldSearcher(ItemTypes::WLD_S_Level);
        if(m_modWorldSetting == HistorySettings::SETTING_PATHBACKGROUND){
            connect(&worldSearcher, SIGNAL(foundLevel(WorldLevels,QGraphicsItem*)), this, SLOT(historyRedoSettingPathBackgroundLevel(WorldLevels,QGraphicsItem*)));
        }else if(m_modWorldSetting == HistorySettings::SETTING_BIGPATHBACKGROUND){
            connect(&worldSearcher, SIGNAL(foundLevel(WorldLevels,QGraphicsItem*)), this, SLOT(historyRedoSettingBigPathBackgroundLevel(WorldLevels,QGraphicsItem*)));
        }else if(m_modWorldSetting == HistorySettings::SETTING_ALWAYSVISIBLE){
            connect(&worldSearcher, SIGNAL(foundLevel(WorldLevels,QGraphicsItem*)), this, SLOT(historyRedoSettingAlwaysVisibleLevel(WorldLevels,QGraphicsItem*)));
        }else if(m_modWorldSetting == HistorySettings::SETTING_GAMESTARTPOINT){
            connect(&worldSearcher, SIGNAL(foundLevel(WorldLevels,QGraphicsItem*)), this, SLOT(historyRedoSettingGameStartPointLevel(WorldLevels,QGraphicsItem*)));
        }else if(m_modWorldSetting == HistorySettings::SETTING_DOORID){
            connect(&worldSearcher, SIGNAL(foundLevel(WorldLevels,QGraphicsItem*)), this, SLOT(historyRedoSettingDoorIDLevel(WorldLevels,QGraphicsItem*)));
        }else if(m_modWorldSetting == HistorySettings::SETTING_PATHBYTOP){
            connect(&worldSearcher, SIGNAL(foundLevel(WorldLevels,QGraphicsItem*)), this, SLOT(historyRedoSettingPathByTopLevel(WorldLevels,QGraphicsItem*)));
        }else if(m_modWorldSetting == HistorySettings::SETTING_PATHBYRIGHT){
            connect(&worldSearcher, SIGNAL(foundLevel(WorldLevels,QGraphicsItem*)), this, SLOT(historyRedoSettingPathByRightLevel(WorldLevels,QGraphicsItem*)));
        }else if(m_modWorldSetting == HistorySettings::SETTING_PATHBYBOTTOM){
            connect(&worldSearcher, SIGNAL(foundLevel(WorldLevels,QGraphicsItem*)), this, SLOT(historyRedoSettingPathByBottomLevel(WorldLevels,QGraphicsItem*)));
        }else if(m_modWorldSetting == HistorySettings::SETTING_PATHBYLEFT){
            connect(&worldSearcher, SIGNAL(foundLevel(WorldLevels,QGraphicsItem*)), this, SLOT(historyRedoSettingPathByLeftLevel(WorldLevels,QGraphicsItem*)));
        }else if(m_modWorldSetting == HistorySettings::SETTING_GOTOX){
            connect(&worldSearcher, SIGNAL(foundLevel(WorldLevels,QGraphicsItem*)), this, SLOT(historyRedoSettingGotoXLevel(WorldLevels,QGraphicsItem*)));
        }else if(m_modWorldSetting == HistorySettings::SETTING_GOTOY){
            connect(&worldSearcher, SIGNAL(foundLevel(WorldLevels,QGraphicsItem*)), this, SLOT(historyRedoSettingGotoYLevel(WorldLevels,QGraphicsItem*)));
        }else if(m_modWorldSetting == HistorySettings::SETTING_LEVELFILE){
            connect(&worldSearcher, SIGNAL(foundLevel(WorldLevels,QGraphicsItem*)), this, SLOT(historyRedoSettingLevelfileLevel(WorldLevels,QGraphicsItem*)));
        }else if(m_modWorldSetting == HistorySettings::SETTING_LEVELTITLE){
            connect(&worldSearcher, SIGNAL(foundLevel(WorldLevels,QGraphicsItem*)), this, SLOT(historyRedoSettingLeveltitleLevel(WorldLevels,QGraphicsItem*)));
        }
        worldSearcher.find(m_modWorldData, m_scene->items());
    }
}

void HistoryElementItemSetting::historyUndoSettingPathBackgroundLevel(const WorldLevels &sourceLevel, QGraphicsItem *item)
{
    ((ItemLevel*)item)->setPath(sourceLevel.pathbg);
}

void HistoryElementItemSetting::historyRedoSettingPathBackgroundLevel(const WorldLevels &/*sourceLevel*/, QGraphicsItem *item)
{
    ((ItemLevel*)item)->setPath(m_modData.toBool());
}

void HistoryElementItemSetting::historyUndoSettingBigPathBackgroundLevel(const WorldLevels &sourceLevel, QGraphicsItem *item)
{
    ((ItemLevel*)item)->setbPath(sourceLevel.bigpathbg);
}

void HistoryElementItemSetting::historyRedoSettingBigPathBackgroundLevel(const WorldLevels &/*sourceLevel*/, QGraphicsItem *item)
{
    ((ItemLevel*)item)->setbPath(m_modData.toBool());
}

void HistoryElementItemSetting::historyUndoSettingAlwaysVisibleLevel(const WorldLevels &sourceLevel, QGraphicsItem *item)
{
    ((ItemLevel*)item)->alwaysVisible(sourceLevel.alwaysVisible);
}

void HistoryElementItemSetting::historyRedoSettingAlwaysVisibleLevel(const WorldLevels &/*sourceLevel*/, QGraphicsItem *item)
{
    ((ItemLevel*)item)->alwaysVisible(m_modData.toBool());
}

void HistoryElementItemSetting::historyUndoSettingGameStartPointLevel(const WorldLevels &sourceLevel, QGraphicsItem *item)
{
    ((ItemLevel*)item)->levelData.gamestart = sourceLevel.gamestart;
    ((ItemLevel*)item)->arrayApply();
}

void HistoryElementItemSetting::historyRedoSettingGameStartPointLevel(const WorldLevels &/*sourceLevel*/, QGraphicsItem *item)
{
    ((ItemLevel*)item)->levelData.gamestart = m_modData.toBool();
    ((ItemLevel*)item)->arrayApply();
}

void HistoryElementItemSetting::historyUndoSettingLevelfileLevel(const WorldLevels &sourceLevel, QGraphicsItem *item)
{
    ((ItemLevel*)item)->levelData.lvlfile = sourceLevel.lvlfile;
    ((ItemLevel*)item)->arrayApply();
}

void HistoryElementItemSetting::historyRedoSettingLevelfileLevel(const WorldLevels &/*sourceLevel*/, QGraphicsItem *item)
{
    ((ItemLevel*)item)->levelData.lvlfile = m_modData.toString();
    ((ItemLevel*)item)->arrayApply();
}

void HistoryElementItemSetting::historyUndoSettingLeveltitleLevel(const WorldLevels &sourceLevel, QGraphicsItem *item)
{
    ((ItemLevel*)item)->levelData.title = sourceLevel.title;
    ((ItemLevel*)item)->arrayApply();
}

void HistoryElementItemSetting::historyRedoSettingLeveltitleLevel(const WorldLevels &/*sourceLevel*/, QGraphicsItem *item)
{
    ((ItemLevel*)item)->levelData.title = m_modData.toString();
    ((ItemLevel*)item)->arrayApply();
}

void HistoryElementItemSetting::historyUndoSettingDoorIDLevel(const WorldLevels &sourceLevel, QGraphicsItem *item)
{
    ((ItemLevel*)item)->levelData.entertowarp = sourceLevel.entertowarp;
    ((ItemLevel*)item)->arrayApply();
}

void HistoryElementItemSetting::historyRedoSettingDoorIDLevel(const WorldLevels &/*sourceLevel*/, QGraphicsItem *item)
{
    ((ItemLevel*)item)->levelData.entertowarp = m_modData.toInt();
    ((ItemLevel*)item)->arrayApply();
}

void HistoryElementItemSetting::historyUndoSettingPathByTopLevel(const WorldLevels &sourceLevel, QGraphicsItem *item)
{
    ((ItemLevel*)item)->levelData.top_exit = sourceLevel.top_exit;
    ((ItemLevel*)item)->arrayApply();
}

void HistoryElementItemSetting::historyRedoSettingPathByTopLevel(const WorldLevels &/*sourceLevel*/, QGraphicsItem *item)
{
    ((ItemLevel*)item)->levelData.top_exit = m_modData.toInt();
    ((ItemLevel*)item)->arrayApply();
}

void HistoryElementItemSetting::historyUndoSettingPathByRightLevel(const WorldLevels &sourceLevel, QGraphicsItem *item)
{
    ((ItemLevel*)item)->levelData.right_exit = sourceLevel.right_exit;
    ((ItemLevel*)item)->arrayApply();
}

void HistoryElementItemSetting::historyRedoSettingPathByRightLevel(const WorldLevels &/*sourceLevel*/, QGraphicsItem *item)
{
    ((ItemLevel*)item)->levelData.right_exit = m_modData.toInt();
    ((ItemLevel*)item)->arrayApply();
}

void HistoryElementItemSetting::historyUndoSettingPathByBottomLevel(const WorldLevels &sourceLevel, QGraphicsItem *item)
{
    ((ItemLevel*)item)->levelData.bottom_exit = sourceLevel.bottom_exit;
    ((ItemLevel*)item)->arrayApply();
}

void HistoryElementItemSetting::historyRedoSettingPathByBottomLevel(const WorldLevels &/*sourceLevel*/, QGraphicsItem *item)
{
    ((ItemLevel*)item)->levelData.bottom_exit = m_modData.toInt();
    ((ItemLevel*)item)->arrayApply();
}

void HistoryElementItemSetting::historyUndoSettingPathByLeftLevel(const WorldLevels &sourceLevel, QGraphicsItem *item)
{
    ((ItemLevel*)item)->levelData.left_exit = sourceLevel.left_exit;
    ((ItemLevel*)item)->arrayApply();
}

void HistoryElementItemSetting::historyRedoSettingPathByLeftLevel(const WorldLevels &/*sourceLevel*/, QGraphicsItem *item)
{
    ((ItemLevel*)item)->levelData.left_exit = m_modData.toInt();
    ((ItemLevel*)item)->arrayApply();
}

void HistoryElementItemSetting::historyUndoSettingGotoXLevel(const WorldLevels &sourceLevel, QGraphicsItem *item)
{
    ((ItemLevel*)item)->levelData.gotox = sourceLevel.gotox;
    ((ItemLevel*)item)->arrayApply();
}

void HistoryElementItemSetting::historyRedoSettingGotoXLevel(const WorldLevels &/*sourceLevel*/, QGraphicsItem *item)
{
    ((ItemLevel*)item)->levelData.gotox = m_modData.toInt();
    ((ItemLevel*)item)->arrayApply();
}

void HistoryElementItemSetting::historyUndoSettingGotoYLevel(const WorldLevels &sourceLevel, QGraphicsItem *item)
{
    ((ItemLevel*)item)->levelData.gotoy = sourceLevel.gotoy;
    ((ItemLevel*)item)->arrayApply();
}

void HistoryElementItemSetting::historyRedoSettingGotoYLevel(const WorldLevels &/*sourceLevel*/, QGraphicsItem *item)
{
    ((ItemLevel*)item)->levelData.gotoy = m_modData.toInt();
    ((ItemLevel*)item)->arrayApply();
}
