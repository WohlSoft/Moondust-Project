#include "historyelementitemsetting.h"

#include <editing/_scenes/world/items/item_level.h>

#include <editing/_scenes/level/items/item_block.h>
#include <editing/_scenes/level/items/item_bgo.h>
#include <editing/_scenes/level/items/item_npc.h>
#include <editing/_scenes/level/items/item_door.h>
#include <editing/_scenes/level/items/item_water.h>
#include <editing/_scenes/level/items/item_playerpoint.h>

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
        processLevelUndo();
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
        processLevelRedo();
    }
    if((wldScene = qobject_cast<WldScene*>(m_scene))){
        processWorldRedo();
    }
}

void HistoryElementItemSetting::processWorldUndo()
{
    if(!m_scene)
        return;

    WldScene* wldScene = 0;
    if(!(wldScene = qobject_cast<WldScene*>(m_scene)))
        return;

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

void HistoryElementItemSetting::processLevelUndo()
{
    if(!m_scene)
        return;

    LvlScene* lvlScene = 0;
    if(!(lvlScene = qobject_cast<LvlScene*>(m_scene)))
        return;

    ItemSearcher levelSearcher;

    if(m_modLevelSetting == HistorySettings::SETTING_INVISIBLE){
        levelSearcher.setFindFilter(ItemTypes::LVL_S_Block);
        connect(&levelSearcher, SIGNAL(foundBlock(LevelBlock,QGraphicsItem*)), this, SLOT(historyUndoSettingsInvisibleBlock(LevelBlock,QGraphicsItem*)));
    }
    else
    if(m_modLevelSetting == HistorySettings::SETTING_SLIPPERY){
        levelSearcher.setFindFilter(ItemTypes::LVL_S_Block);
        connect(&levelSearcher, SIGNAL(foundBlock(LevelBlock,QGraphicsItem*)), this, SLOT(historyUndoSettingsSlipperyBlock(LevelBlock,QGraphicsItem*)));
    }
    else
    if(m_modLevelSetting == HistorySettings::SETTING_FRIENDLY){
        levelSearcher.setFindFilter(ItemTypes::LVL_S_NPC);
        connect(&levelSearcher, SIGNAL(foundNPC(LevelNPC,QGraphicsItem*)), this, SLOT(historyUndoSettingsFriendlyNPC(LevelNPC,QGraphicsItem*)));
    }
    else
    if(m_modLevelSetting == HistorySettings::SETTING_BOSS){
        levelSearcher.setFindFilter(ItemTypes::LVL_S_NPC);
        connect(&levelSearcher, SIGNAL(foundNPC(LevelNPC,QGraphicsItem*)), this, SLOT(historyUndoSettingsBossNPC(LevelNPC,QGraphicsItem*)));
    }
    else
    if(m_modLevelSetting == HistorySettings::SETTING_NOMOVEABLE){
        levelSearcher.setFindFilter(ItemTypes::LVL_S_NPC);
        connect(&levelSearcher, SIGNAL(foundNPC(LevelNPC,QGraphicsItem*)), this, SLOT(historyUndoSettingsNoMoveableNPC(LevelNPC,QGraphicsItem*)));
    }
    else
    if(m_modLevelSetting == HistorySettings::SETTING_MESSAGE){
        levelSearcher.setFindFilter(ItemTypes::LVL_S_NPC);
        connect(&levelSearcher, SIGNAL(foundNPC(LevelNPC,QGraphicsItem*)), this, SLOT(historyUndoSettingsMessageNPC(LevelNPC,QGraphicsItem*)));
    }
    else
    if(m_modLevelSetting == HistorySettings::SETTING_DIRECTION){
        levelSearcher.setFindFilter(ItemTypes::LVL_S_NPC);
        connect(&levelSearcher, SIGNAL(foundNPC(LevelNPC,QGraphicsItem*)), this, SLOT(historyUndoSettingsDirectionNPC(LevelNPC,QGraphicsItem*)));
    }
    else
    if(m_modLevelSetting == HistorySettings::SETTING_CHANGENPC){
        levelSearcher.setFindFilter(static_cast<ItemTypes::itemTypesMultiSelectable>(ItemTypes::LVL_S_Block | ItemTypes::LVL_S_NPC));
        connect(&levelSearcher, SIGNAL(foundBlock(LevelBlock,QGraphicsItem*)), this, SLOT(historyUndoSettingsChangeNPCBlocks(LevelBlock,QGraphicsItem*)));
        connect(&levelSearcher, SIGNAL(foundNPC(LevelNPC,QGraphicsItem*)), this, SLOT(historyUndoSettingsChangeNPCNPC(LevelNPC,QGraphicsItem*)));
    }
    else
    if(m_modLevelSetting == HistorySettings::SETTING_WATERTYPE){
        levelSearcher.setFindFilter(ItemTypes::LVL_S_PhysEnv);
        connect(&levelSearcher, SIGNAL(foundPhysEnv(LevelPhysEnv,QGraphicsItem*)), this, SLOT(historyUndoSettingsTypeWater(LevelPhysEnv,QGraphicsItem*)));
    }
    else
    if(m_modLevelSetting == HistorySettings::SETTING_NOYOSHI){
        levelSearcher.setFindFilter(ItemTypes::LVL_S_Door);
        connect(&levelSearcher, SIGNAL(foundDoor(LevelDoor,QGraphicsItem*)), this, SLOT(historyUndoSettingsNoYoshiDoors(LevelDoors,QGraphicsItem*)));
    }
    else
    if(m_modLevelSetting == HistorySettings::SETTING_ALLOWNPC){
        levelSearcher.setFindFilter(ItemTypes::LVL_S_Door);
        connect(&levelSearcher, SIGNAL(foundDoor(LevelDoor,QGraphicsItem*)), this, SLOT(historyUndoSettingsAllowNPCDoors(LevelDoors,QGraphicsItem*)));
    }
    else
    if(m_modLevelSetting == HistorySettings::SETTING_LOCKED){
        levelSearcher.setFindFilter(ItemTypes::LVL_S_Door);
        connect(&levelSearcher, SIGNAL(foundDoor(LevelDoor,QGraphicsItem*)), this, SLOT(historyUndoSettingsLockedDoors(LevelDoors,QGraphicsItem*)));
    }
    else
    if(m_modLevelSetting == HistorySettings::SETTING_GENACTIVATE){
        levelSearcher.setFindFilter(ItemTypes::LVL_S_NPC);
        connect(&levelSearcher, SIGNAL(foundNPC(LevelNPC,QGraphicsItem*)), this, SLOT(historyUndoSettingsActivateGeneratorNPC(LevelNPC,QGraphicsItem*)));
    }
    else
    if(m_modLevelSetting == HistorySettings::SETTING_GENTYPE){
        levelSearcher.setFindFilter(ItemTypes::LVL_S_NPC);
        connect(&levelSearcher, SIGNAL(foundNPC(LevelNPC,QGraphicsItem*)), this, SLOT(historyUndoSettingsTypeGeneratorNPC(LevelNPC,QGraphicsItem*)));
    }
    else
    if(m_modLevelSetting == HistorySettings::SETTING_GENDIR){
        levelSearcher.setFindFilter(ItemTypes::LVL_S_NPC);
        connect(&levelSearcher, SIGNAL(foundNPC(LevelNPC,QGraphicsItem*)), this, SLOT(historyUndoSettingsDirectionGeneratorNPC(LevelNPC,QGraphicsItem*)));
    }
    else
    if(m_modLevelSetting == HistorySettings::SETTING_GENTIME){
        levelSearcher.setFindFilter(ItemTypes::LVL_S_NPC);
        connect(&levelSearcher, SIGNAL(foundNPC(LevelNPC,QGraphicsItem*)), this, SLOT(historyUndoSettingsTimeGeneratorNPC(LevelNPC,QGraphicsItem*)));
    }
    else
    if(m_modLevelSetting == HistorySettings::SETTING_ATTACHLAYER){
        levelSearcher.setFindFilter(ItemTypes::LVL_S_NPC);
        connect(&levelSearcher, SIGNAL(foundNPC(LevelNPC,QGraphicsItem*)), this, SLOT(historyUndoSettingsAttachLayerNPC(LevelNPC,QGraphicsItem*)));
    }
    else
    if(m_modLevelSetting == HistorySettings::SETTING_EV_DESTROYED){
        levelSearcher.setFindFilter(ItemTypes::LVL_S_Block);
        connect(&levelSearcher, SIGNAL(foundBlock(LevelBlock,QGraphicsItem*)), this, SLOT(historyUndoSettingsDestroyedEventBlocks(LevelBlock,QGraphicsItem*)));
    }
    else
    if(m_modLevelSetting == HistorySettings::SETTING_EV_HITED){
        levelSearcher.setFindFilter(ItemTypes::LVL_S_Block);
        connect(&levelSearcher, SIGNAL(foundBlock(LevelBlock,QGraphicsItem*)), this, SLOT(historyUndoSettingsHitedEventBlocks(LevelBlock,QGraphicsItem*)));
    }
    else
    if(m_modLevelSetting == HistorySettings::SETTING_EV_LAYER_EMP){
        levelSearcher.setFindFilter(static_cast<ItemTypes::itemTypesMultiSelectable>(ItemTypes::LVL_S_Block | ItemTypes::LVL_S_NPC));
        connect(&levelSearcher, SIGNAL(foundBlock(LevelBlock,QGraphicsItem*)), this, SLOT(historyUndoSettingsLayerEmptyEventBlocks(LevelBlock,QGraphicsItem*)));
        connect(&levelSearcher, SIGNAL(foundNPC(LevelNPC,QGraphicsItem*)), this, SLOT(historyUndoSettingsLayerEmptyEventNPC(LevelNPC,QGraphicsItem*)));
    }
    else
    if(m_modLevelSetting == HistorySettings::SETTING_EV_ACTIVATE){
        levelSearcher.setFindFilter(ItemTypes::LVL_S_NPC);
        connect(&levelSearcher, SIGNAL(foundNPC(LevelNPC,QGraphicsItem*)), this, SLOT(historyUndoSettingsActivateEventNPC(LevelNPC,QGraphicsItem*)));
    }
    else
    if(m_modLevelSetting == HistorySettings::SETTING_EV_DEATH){
        levelSearcher.setFindFilter(ItemTypes::LVL_S_NPC);
        connect(&levelSearcher, SIGNAL(foundNPC(LevelNPC,QGraphicsItem*)), this, SLOT(historyUndoSettingsDeathEventNPC(LevelNPC,QGraphicsItem*)));
    }
    else
    if(m_modLevelSetting == HistorySettings::SETTING_EV_TALK){
        levelSearcher.setFindFilter(ItemTypes::LVL_S_NPC);
        connect(&levelSearcher, SIGNAL(foundBlock(LevelNPC,QGraphicsItem*)), this, SLOT(historyUndoSettingsTalkEventNPC(LevelNPC,QGraphicsItem*)));
    }
    else
    if(m_modLevelSetting == HistorySettings::SETTING_SPECIAL_DATA){
        levelSearcher.setFindFilter(ItemTypes::LVL_S_NPC);
        connect(&levelSearcher, SIGNAL(foundNPC(LevelNPC,QGraphicsItem*)), this, SLOT(historyUndoSettingsSpecialDataNPC(LevelNPC,QGraphicsItem*)));
    }
    else
    if(m_modLevelSetting == HistorySettings::SETTING_BGOSORTING){
        levelSearcher.setFindFilter(ItemTypes::LVL_S_BGO);
        connect(&levelSearcher, SIGNAL(foundBGO(LevelBGO,QGraphicsItem*)), this, SLOT(historyUndoSettingsSortingBGO(LevelBGO,QGraphicsItem*)));
    }
    levelSearcher.find(m_modLevelData, m_scene->items());
}

void HistoryElementItemSetting::processWorldRedo()
{
    if(!m_scene)
        return;

    WldScene* wldScene = 0;
    if(!(wldScene = qobject_cast<WldScene*>(m_scene)))
        return;

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

void HistoryElementItemSetting::processLevelRedo()
{
    if(!m_scene)
        return;

    LvlScene* lvlScene = 0;
    if(!(lvlScene = qobject_cast<LvlScene*>(m_scene)))
        return;

    ItemSearcher levelSearcher;

    if(m_modLevelSetting == HistorySettings::SETTING_INVISIBLE){
        levelSearcher.setFindFilter(ItemTypes::LVL_S_Block);
        connect(&levelSearcher, SIGNAL(foundBlock(LevelBlock,QGraphicsItem*)), this, SLOT(historyRedoSettingsInvisibleBlock(LevelBlock,QGraphicsItem*)));
    }
    else
    if(m_modLevelSetting == HistorySettings::SETTING_SLIPPERY){
        levelSearcher.setFindFilter(ItemTypes::LVL_S_Block);
        connect(&levelSearcher, SIGNAL(foundBlock(LevelBlock,QGraphicsItem*)), this, SLOT(historyRedoSettingsSlipperyBlock(LevelBlock,QGraphicsItem*)));
    }
    else
    if(m_modLevelSetting == HistorySettings::SETTING_FRIENDLY){
        levelSearcher.setFindFilter(ItemTypes::LVL_S_NPC);
        connect(&levelSearcher, SIGNAL(foundNPC(LevelNPC,QGraphicsItem*)), this, SLOT(historyRedoSettingsFriendlyNPC(LevelNPC,QGraphicsItem*)));
    }
    else
    if(m_modLevelSetting == HistorySettings::SETTING_BOSS){
        levelSearcher.setFindFilter(ItemTypes::LVL_S_NPC);
        connect(&levelSearcher, SIGNAL(foundNPC(LevelNPC,QGraphicsItem*)), this, SLOT(historyRedoSettingsBossNPC(LevelNPC,QGraphicsItem*)));
    }
    else
    if(m_modLevelSetting == HistorySettings::SETTING_NOMOVEABLE){
        levelSearcher.setFindFilter(ItemTypes::LVL_S_NPC);
        connect(&levelSearcher, SIGNAL(foundNPC(LevelNPC,QGraphicsItem*)), this, SLOT(historyRedoSettingsNoMoveableNPC(LevelNPC,QGraphicsItem*)));
    }
    else
    if(m_modLevelSetting == HistorySettings::SETTING_MESSAGE){
        levelSearcher.setFindFilter(ItemTypes::LVL_S_NPC);
        connect(&levelSearcher, SIGNAL(foundNPC(LevelNPC,QGraphicsItem*)), this, SLOT(historyRedoSettingsMessageNPC(LevelNPC,QGraphicsItem*)));
    }
    else
    if(m_modLevelSetting == HistorySettings::SETTING_DIRECTION){
        levelSearcher.setFindFilter(ItemTypes::LVL_S_NPC);
        connect(&levelSearcher, SIGNAL(foundNPC(LevelNPC,QGraphicsItem*)), this, SLOT(historyRedoSettingsDirectionNPC(LevelNPC,QGraphicsItem*)));
    }
    else
    if(m_modLevelSetting == HistorySettings::SETTING_CHANGENPC){
        levelSearcher.setFindFilter(static_cast<ItemTypes::itemTypesMultiSelectable>(ItemTypes::LVL_S_Block | ItemTypes::LVL_S_NPC));
        connect(&levelSearcher, SIGNAL(foundBlock(LevelBlock,QGraphicsItem*)), this, SLOT(historyRedoSettingsChangeNPCBlocks(LevelBlock,QGraphicsItem*)));
        connect(&levelSearcher, SIGNAL(foundNPC(LevelNPC,QGraphicsItem*)), this, SLOT(historyRedoSettingsChangeNPCNPC(LevelNPC,QGraphicsItem*)));
    }
    else
    if(m_modLevelSetting == HistorySettings::SETTING_WATERTYPE){
        levelSearcher.setFindFilter(ItemTypes::LVL_S_PhysEnv);
        connect(&levelSearcher, SIGNAL(foundPhysEnv(LevelPhysEnv,QGraphicsItem*)), this, SLOT(historyRedoSettingsTypeWater(LevelPhysEnv,QGraphicsItem*)));
    }
    else
    if(m_modLevelSetting == HistorySettings::SETTING_NOYOSHI){
        levelSearcher.setFindFilter(ItemTypes::LVL_S_Door);
        connect(&levelSearcher, SIGNAL(foundDoor(LevelDoor,QGraphicsItem*)), this, SLOT(historyRedoSettingsNoYoshiDoors(LevelDoors,QGraphicsItem*)));
    }
    else
    if(m_modLevelSetting == HistorySettings::SETTING_ALLOWNPC){
        levelSearcher.setFindFilter(ItemTypes::LVL_S_Door);
        connect(&levelSearcher, SIGNAL(foundDoor(LevelDoor,QGraphicsItem*)), this, SLOT(historyRedoSettingsAllowNPCDoors(LevelDoors,QGraphicsItem*)));
    }
    else
    if(m_modLevelSetting == HistorySettings::SETTING_LOCKED){
        levelSearcher.setFindFilter(ItemTypes::LVL_S_Door);
        connect(&levelSearcher, SIGNAL(foundDoor(LevelDoor,QGraphicsItem*)), this, SLOT(historyRedoSettingsLockedDoors(LevelDoors,QGraphicsItem*)));
    }
    else
    if(m_modLevelSetting == HistorySettings::SETTING_GENACTIVATE){
        levelSearcher.setFindFilter(ItemTypes::LVL_S_NPC);
        connect(&levelSearcher, SIGNAL(foundNPC(LevelNPC,QGraphicsItem*)), this, SLOT(historyRedoSettingsActivateGeneratorNPC(LevelNPC,QGraphicsItem*)));
    }
    else
    if(m_modLevelSetting == HistorySettings::SETTING_GENTYPE){
        levelSearcher.setFindFilter(ItemTypes::LVL_S_NPC);
        connect(&levelSearcher, SIGNAL(foundNPC(LevelNPC,QGraphicsItem*)), this, SLOT(historyRedoSettingsTypeGeneratorNPC(LevelNPC,QGraphicsItem*)));
    }
    else
    if(m_modLevelSetting == HistorySettings::SETTING_GENDIR){
        levelSearcher.setFindFilter(ItemTypes::LVL_S_NPC);
        connect(&levelSearcher, SIGNAL(foundNPC(LevelNPC,QGraphicsItem*)), this, SLOT(historyRedoSettingsDirectionGeneratorNPC(LevelNPC,QGraphicsItem*)));
    }
    else
    if(m_modLevelSetting == HistorySettings::SETTING_GENTIME){
        levelSearcher.setFindFilter(ItemTypes::LVL_S_NPC);
        connect(&levelSearcher, SIGNAL(foundNPC(LevelNPC,QGraphicsItem*)), this, SLOT(historyRedoSettingsTimeGeneratorNPC(LevelNPC,QGraphicsItem*)));
    }
    else
    if(m_modLevelSetting == HistorySettings::SETTING_ATTACHLAYER){
        levelSearcher.setFindFilter(ItemTypes::LVL_S_NPC);
        connect(&levelSearcher, SIGNAL(foundNPC(LevelNPC,QGraphicsItem*)), this, SLOT(historyRedoSettingsAttachLayerNPC(LevelNPC,QGraphicsItem*)));
    }
    else
    if(m_modLevelSetting == HistorySettings::SETTING_EV_DESTROYED){
        levelSearcher.setFindFilter(ItemTypes::LVL_S_Block);
        connect(&levelSearcher, SIGNAL(foundBlock(LevelBlock,QGraphicsItem*)), this, SLOT(historyRedoSettingsDestroyedEventBlocks(LevelBlock,QGraphicsItem*)));
    }
    else
    if(m_modLevelSetting == HistorySettings::SETTING_EV_HITED){
        levelSearcher.setFindFilter(ItemTypes::LVL_S_Block);
        connect(&levelSearcher, SIGNAL(foundBlock(LevelBlock,QGraphicsItem*)), this, SLOT(historyRedoSettingsHitedEventBlocks(LevelBlock,QGraphicsItem*)));
    }
    else
    if(m_modLevelSetting == HistorySettings::SETTING_EV_LAYER_EMP){
        levelSearcher.setFindFilter(static_cast<ItemTypes::itemTypesMultiSelectable>(ItemTypes::LVL_S_Block | ItemTypes::LVL_S_NPC));
        connect(&levelSearcher, SIGNAL(foundBlock(LevelBlock,QGraphicsItem*)), this, SLOT(historyRedoSettingsLayerEmptyEventBlocks(LevelBlock,QGraphicsItem*)));
        connect(&levelSearcher, SIGNAL(foundNPC(LevelNPC,QGraphicsItem*)), this, SLOT(historyRedoSettingsLayerEmptyEventNPC(LevelNPC,QGraphicsItem*)));
    }
    else
    if(m_modLevelSetting == HistorySettings::SETTING_EV_ACTIVATE){
        levelSearcher.setFindFilter(ItemTypes::LVL_S_NPC);
        connect(&levelSearcher, SIGNAL(foundNPC(LevelNPC,QGraphicsItem*)), this, SLOT(historyRedoSettingsActivateEventNPC(LevelNPC,QGraphicsItem*)));
    }
    else
    if(m_modLevelSetting == HistorySettings::SETTING_EV_DEATH){
        levelSearcher.setFindFilter(ItemTypes::LVL_S_NPC);
        connect(&levelSearcher, SIGNAL(foundNPC(LevelNPC,QGraphicsItem*)), this, SLOT(historyRedoSettingsDeathEventNPC(LevelNPC,QGraphicsItem*)));
    }
    else
    if(m_modLevelSetting == HistorySettings::SETTING_EV_TALK){
        levelSearcher.setFindFilter(ItemTypes::LVL_S_NPC);
        connect(&levelSearcher, SIGNAL(foundBlock(LevelNPC,QGraphicsItem*)), this, SLOT(historyRedoSettingsTalkEventNPC(LevelNPC,QGraphicsItem*)));
    }
    else
    if(m_modLevelSetting == HistorySettings::SETTING_SPECIAL_DATA){
        levelSearcher.setFindFilter(ItemTypes::LVL_S_NPC);
        connect(&levelSearcher, SIGNAL(foundNPC(LevelNPC,QGraphicsItem*)), this, SLOT(historyRedoSettingsSpecialDataNPC(LevelNPC,QGraphicsItem*)));
    }
    else
    if(m_modLevelSetting == HistorySettings::SETTING_BGOSORTING){
        levelSearcher.setFindFilter(ItemTypes::LVL_S_BGO);
        connect(&levelSearcher, SIGNAL(foundBGO(LevelBGO,QGraphicsItem*)), this, SLOT(historyRedoSettingsSortingBGO(LevelBGO,QGraphicsItem*)));
    }
    levelSearcher.find(m_modLevelData, m_scene->items());
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



void HistoryElementItemSetting::historyUndoSettingsInvisibleBlock(const LevelBlock &sourceBlock, QGraphicsItem *item)
{
    ((ItemBlock*)item)->setInvisible(sourceBlock.invisible);
}

void HistoryElementItemSetting::historyRedoSettingsInvisibleBlock(const LevelBlock &/*sourceBlock*/, QGraphicsItem *item)
{
    ((ItemBlock*)item)->setInvisible(m_modData.toBool());
}

void HistoryElementItemSetting::historyUndoSettingsSlipperyBlock(const LevelBlock &sourceBlock, QGraphicsItem *item)
{
    ((ItemBlock*)item)->setSlippery(sourceBlock.slippery);
}

void HistoryElementItemSetting::historyRedoSettingsSlipperyBlock(const LevelBlock &/*sourceBlock*/, QGraphicsItem *item)
{
    ((ItemBlock*)item)->setSlippery(m_modData.toBool());
}

void HistoryElementItemSetting::historyUndoSettingsFriendlyNPC(const LevelNPC &sourceNPC, QGraphicsItem *item)
{
    ((ItemNPC*)item)->setFriendly(sourceNPC.friendly);
}

void HistoryElementItemSetting::historyRedoSettingsFriendlyNPC(const LevelNPC &/*sourceNPC*/, QGraphicsItem *item)
{
    ((ItemNPC*)item)->setFriendly(m_modData.toBool());
}

void HistoryElementItemSetting::historyUndoSettingsBossNPC(const LevelNPC &sourceNPC, QGraphicsItem *item)
{
    ((ItemNPC*)item)->setLegacyBoss(sourceNPC.legacyboss);
}

void HistoryElementItemSetting::historyRedoSettingsBossNPC(const LevelNPC &/*sourceNPC*/, QGraphicsItem *item)
{
    ((ItemNPC*)item)->setLegacyBoss(m_modData.toBool());
}

void HistoryElementItemSetting::historyUndoSettingsNoMoveableNPC(const LevelNPC &sourceNPC, QGraphicsItem *item)
{
    ((ItemNPC*)item)->setNoMovable(sourceNPC.nomove);
}

void HistoryElementItemSetting::historyRedoSettingsNoMoveableNPC(const LevelNPC &/*sourceNPC*/, QGraphicsItem *item)
{
    ((ItemNPC*)item)->setNoMovable(m_modData.toBool());
}

void HistoryElementItemSetting::historyUndoSettingsMessageNPC(const LevelNPC &sourceNPC, QGraphicsItem *item)
{
    ((ItemNPC*)item)->setMsg(sourceNPC.msg);
}

void HistoryElementItemSetting::historyRedoSettingsMessageNPC(const LevelNPC &/*sourceNPC*/, QGraphicsItem *item)
{
    ((ItemNPC*)item)->setMsg(m_modData.toString());
}

void HistoryElementItemSetting::historyUndoSettingsDirectionNPC(const LevelNPC &sourceNPC, QGraphicsItem *item)
{
    ((ItemNPC*)item)->changeDirection(sourceNPC.direct);
}

void HistoryElementItemSetting::historyRedoSettingsDirectionNPC(const LevelNPC &/*sourceNPC*/, QGraphicsItem *item)
{
    ((ItemNPC*)item)->changeDirection(m_modData.toInt());
}

void HistoryElementItemSetting::historyUndoSettingsChangeNPCBlocks(const LevelBlock &sourceBlock, QGraphicsItem *item)
{
    ItemBlock* targetItem = (ItemBlock*)item;
    int targetNPC_id = sourceBlock.npc_id;
    //targetItem->blockData.npc_id = (unsigned long)targetNPC_id;
    //targetItem->arrayApply();
    targetItem->setIncludedNPC((unsigned long)targetNPC_id);
}

void HistoryElementItemSetting::historyRedoSettingsChangeNPCBlocks(const LevelBlock &/*sourceBlock*/, QGraphicsItem *item)
{
    ItemBlock* targetItem = (ItemBlock*)item;
    int targetNPC_id = m_modData.toInt();
    //targetItem->blockData.npc_id = (unsigned long)targetNPC_id;
    //targetItem->arrayApply();
    targetItem->setIncludedNPC((unsigned long)targetNPC_id);
}

void HistoryElementItemSetting::historyUndoSettingsChangeNPCNPC(const LevelNPC &sourceNPC, QGraphicsItem *item)
{
    ((ItemNPC*)item)->setIncludedNPC(sourceNPC.special_data);
}

void HistoryElementItemSetting::historyRedoSettingsChangeNPCNPC(const LevelNPC &/*sourceNPC*/, QGraphicsItem *item)
{
    ((ItemNPC*)item)->setIncludedNPC(m_modData.toInt());
}

void HistoryElementItemSetting::historyUndoSettingsTypeWater(const LevelPhysEnv &sourcePhysEnv, QGraphicsItem *item)
{
    ((ItemWater*)item)->setType(sourcePhysEnv.quicksand ? 1 : 0);
}

void HistoryElementItemSetting::historyRedoSettingsTypeWater(const LevelPhysEnv &/*sourcePhysEnv*/, QGraphicsItem *item)
{
    ((ItemWater*)item)->setType(m_modData.toBool() ? 0 : 1);
}

void HistoryElementItemSetting::historyUndoSettingsNoYoshiDoors(const LevelDoors &/*sourceDoors*/, QGraphicsItem *item)
{
    ((ItemDoor*)item)->doorData.novehicles = !m_modData.toBool();
    ((ItemDoor*)item)->arrayApply();
}

void HistoryElementItemSetting::historyRedoSettingsNoYoshiDoors(const LevelDoors &/*sourceDoors*/, QGraphicsItem *item)
{
    ((ItemDoor*)item)->doorData.novehicles = m_modData.toBool();
    ((ItemDoor*)item)->arrayApply();
}

void HistoryElementItemSetting::historyUndoSettingsAllowNPCDoors(const LevelDoors &/*sourceDoors*/, QGraphicsItem *item)
{
    ((ItemDoor*)item)->doorData.allownpc = !m_modData.toBool();
    ((ItemDoor*)item)->arrayApply();
}

void HistoryElementItemSetting::historyRedoSettingsAllowNPCDoors(const LevelDoors &/*sourceDoors*/, QGraphicsItem *item)
{
    ((ItemDoor*)item)->doorData.allownpc = m_modData.toBool();
    ((ItemDoor*)item)->arrayApply();
}

void HistoryElementItemSetting::historyUndoSettingsLockedDoors(const LevelDoors &/*sourceDoors*/, QGraphicsItem *item)
{
    ((ItemDoor*)item)->doorData.locked = !m_modData.toBool();
    ((ItemDoor*)item)->arrayApply();
}

void HistoryElementItemSetting::historyRedoSettingsLockedDoors(const LevelDoors &/*sourceDoors*/, QGraphicsItem *item)
{
    ((ItemDoor*)item)->doorData.locked = m_modData.toBool();
    ((ItemDoor*)item)->arrayApply();
}

void HistoryElementItemSetting::historyUndoSettingsActivateGeneratorNPC(const LevelNPC &sourceNPC, QGraphicsItem *item)
{
    ((ItemNPC*)item)->setGenerator(sourceNPC.generator, sourceNPC.generator_direct, sourceNPC.generator_type);
}

void HistoryElementItemSetting::historyRedoSettingsActivateGeneratorNPC(const LevelNPC &sourceNPC, QGraphicsItem *item)
{
    ((ItemNPC*)item)->setGenerator(m_modData.toBool(), sourceNPC.generator_direct, sourceNPC.generator_type);
}

void HistoryElementItemSetting::historyUndoSettingsTypeGeneratorNPC(const LevelNPC &sourceNPC, QGraphicsItem *item)
{
    ((ItemNPC*)item)->setGenerator(sourceNPC.generator, sourceNPC.generator_direct, sourceNPC.generator_type);
}

void HistoryElementItemSetting::historyRedoSettingsTypeGeneratorNPC(const LevelNPC &sourceNPC, QGraphicsItem *item)
{
    ((ItemNPC*)item)->setGenerator(sourceNPC.generator, sourceNPC.generator_direct, m_modData.toInt());
}

void HistoryElementItemSetting::historyUndoSettingsDirectionGeneratorNPC(const LevelNPC &sourceNPC, QGraphicsItem *item)
{
    ((ItemNPC*)item)->setGenerator(sourceNPC.generator, sourceNPC.generator_direct, sourceNPC.generator_type);
}

void HistoryElementItemSetting::historyRedoSettingsDirectionGeneratorNPC(const LevelNPC &sourceNPC, QGraphicsItem *item)
{
    ((ItemNPC*)item)->setGenerator(sourceNPC.generator, m_modData.toInt(), sourceNPC.generator_type);
}

void HistoryElementItemSetting::historyUndoSettingsTimeGeneratorNPC(const LevelNPC &sourceNPC, QGraphicsItem *item)
{
    ((ItemNPC*)item)->npcData.generator_period = sourceNPC.generator_period;
    ((ItemNPC*)item)->arrayApply();
}

void HistoryElementItemSetting::historyRedoSettingsTimeGeneratorNPC(const LevelNPC &/*sourceNPC*/, QGraphicsItem *item)
{
    ((ItemNPC*)item)->npcData.generator_period = m_modData.toInt();
    ((ItemNPC*)item)->arrayApply();
}

void HistoryElementItemSetting::historyUndoSettingsAttachLayerNPC(const LevelNPC &sourceNPC, QGraphicsItem *item)
{
    ((ItemNPC*)item)->npcData.attach_layer = sourceNPC.attach_layer;
    ((ItemNPC*)item)->arrayApply();
}

void HistoryElementItemSetting::historyRedoSettingsAttachLayerNPC(const LevelNPC &/*sourceNPC*/, QGraphicsItem *item)
{
    ((ItemNPC*)item)->npcData.attach_layer = m_modData.toString();
    ((ItemNPC*)item)->arrayApply();
}

void HistoryElementItemSetting::historyUndoSettingsDestroyedEventBlocks(const LevelBlock &sourceBlock, QGraphicsItem *item)
{
    ((ItemBlock*)item)->blockData.event_destroy = sourceBlock.event_destroy;
    ((ItemBlock*)item)->arrayApply();
}

void HistoryElementItemSetting::historyRedoSettingsDestroyedEventBlocks(const LevelBlock &/*sourceBlock*/, QGraphicsItem *item)
{
    ((ItemBlock*)item)->blockData.event_destroy = m_modData.toString();
    ((ItemBlock*)item)->arrayApply();
}

void HistoryElementItemSetting::historyUndoSettingsHitedEventBlocks(const LevelBlock &sourceBlock, QGraphicsItem *item)
{
    ((ItemBlock*)item)->blockData.event_hit = sourceBlock.event_hit;
    ((ItemBlock*)item)->arrayApply();
}

void HistoryElementItemSetting::historyRedoSettingsHitedEventBlocks(const LevelBlock &/*sourceBlock*/, QGraphicsItem *item)
{
    ((ItemBlock*)item)->blockData.event_hit = m_modData.toString();
    ((ItemBlock*)item)->arrayApply();
}

void HistoryElementItemSetting::historyUndoSettingsLayerEmptyEventBlocks(const LevelBlock &sourceBlock, QGraphicsItem *item)
{
    ((ItemBlock*)item)->blockData.event_no_more = sourceBlock.event_no_more;
    ((ItemBlock*)item)->arrayApply();
}

void HistoryElementItemSetting::historyRedoSettingsLayerEmptyEventBlocks(const LevelBlock &/*sourceBlock*/, QGraphicsItem *item)
{
    ((ItemBlock*)item)->blockData.event_no_more = m_modData.toString();
    ((ItemBlock*)item)->arrayApply();
}

void HistoryElementItemSetting::historyUndoSettingsLayerEmptyEventNPC(const LevelNPC &sourceNPC, QGraphicsItem *item)
{
    ((ItemNPC*)item)->npcData.event_nomore = sourceNPC.event_nomore;
    ((ItemNPC*)item)->arrayApply();
}

void HistoryElementItemSetting::historyRedoSettingsLayerEmptyEventNPC(const LevelNPC &/*sourceNPC*/, QGraphicsItem *item)
{
    ((ItemNPC*)item)->npcData.event_nomore = m_modData.toString();
    ((ItemNPC*)item)->arrayApply();
}

void HistoryElementItemSetting::historyUndoSettingsActivateEventNPC(const LevelNPC &sourceNPC, QGraphicsItem *item)
{
    ((ItemNPC*)item)->npcData.event_activate = sourceNPC.event_activate;
    ((ItemNPC*)item)->arrayApply();
}

void HistoryElementItemSetting::historyRedoSettingsActivateEventNPC(const LevelNPC &/*sourceNPC*/, QGraphicsItem *item)
{
    ((ItemNPC*)item)->npcData.event_activate = m_modData.toString();
    ((ItemNPC*)item)->arrayApply();
}

void HistoryElementItemSetting::historyUndoSettingsDeathEventNPC(const LevelNPC &sourceNPC, QGraphicsItem *item)
{
    ((ItemNPC*)item)->npcData.event_die = sourceNPC.event_die;
    ((ItemNPC*)item)->arrayApply();
}

void HistoryElementItemSetting::historyRedoSettingsDeathEventNPC(const LevelNPC &/*sourceNPC*/, QGraphicsItem *item)
{
    ((ItemNPC*)item)->npcData.event_die = m_modData.toString();
    ((ItemNPC*)item)->arrayApply();
}

void HistoryElementItemSetting::historyUndoSettingsTalkEventNPC(const LevelNPC &sourceNPC, QGraphicsItem *item)
{
    ((ItemNPC*)item)->npcData.event_talk = sourceNPC.event_talk;
    ((ItemNPC*)item)->arrayApply();
}

void HistoryElementItemSetting::historyRedoSettingsTalkEventNPC(const LevelNPC &/*sourceNPC*/, QGraphicsItem *item)
{
    ((ItemNPC*)item)->npcData.event_talk = m_modData.toString();
    ((ItemNPC*)item)->arrayApply();
}

void HistoryElementItemSetting::historyUndoSettingsSpecialDataNPC(const LevelNPC &sourceNPC, QGraphicsItem *item)
{
    ((ItemNPC*)item)->npcData.special_data = sourceNPC.special_data;
    ((ItemNPC*)item)->arrayApply();
}

void HistoryElementItemSetting::historyRedoSettingsSpecialDataNPC(const LevelNPC &/*sourceNPC*/, QGraphicsItem *item)
{
    ((ItemNPC*)item)->npcData.special_data = m_modData.toInt();
    ((ItemNPC*)item)->arrayApply();
}

void HistoryElementItemSetting::historyUndoSettingsSortingBGO(const LevelBGO &sourceBGO, QGraphicsItem *item)
{
    ((ItemBGO*)item)->bgoData.smbx64_sp = sourceBGO.smbx64_sp;
    ((ItemBGO*)item)->arrayApply();
}

void HistoryElementItemSetting::historyRedoSettingsSortingBGO(const LevelBGO &/*sourceBGO*/, QGraphicsItem *item)
{
    ((ItemBGO*)item)->bgoData.smbx64_sp = (long)m_modData.toLongLong();
    ((ItemBGO*)item)->arrayApply();
}
