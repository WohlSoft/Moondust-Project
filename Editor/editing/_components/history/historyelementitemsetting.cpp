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
#include <common_features/main_window_ptr.h>

#include <editing/_components/history/itemsearcher.h>


HistoryElementCustomSetting::HistoryElementCustomSetting()
{}

HistoryElementCustomSetting::~HistoryElementCustomSetting()
{}




HistoryElementItemSetting::HistoryElementItemSetting(const WorldData &data, HistorySettings::WorldSettingSubType wldSubType, QVariant extraData, QObject *parent) :
    QObject(parent),
    m_modWorldSetting(wldSubType),
    m_modWorldData(data),
    m_modData(extraData),
    m_customSetting(nullptr)
{}

HistoryElementItemSetting::HistoryElementItemSetting(const LevelData &data, HistorySettings::LevelSettingSubType lvlSubType, QVariant extraData, QObject *parent) :
    QObject(parent),
    m_modLevelSetting(lvlSubType),
    m_modLevelData(data),
    m_modData(extraData),
    m_customSetting(nullptr)
{}

HistoryElementItemSetting::HistoryElementItemSetting(const WorldData &data, HistoryElementCustomSetting *setting, QVariant extraData, QObject *parent) :
    QObject(parent),
    m_modWorldSetting(HistorySettings::SETTING_WLD_CUSTOM),
    m_modWorldData(data),
    m_modData(extraData),
    m_customSetting(setting)
{}

HistoryElementItemSetting::HistoryElementItemSetting(const LevelData &data, HistoryElementCustomSetting *setting, QVariant extraData, QObject *parent) :
    QObject(parent),
    m_modLevelSetting(HistorySettings::SETTING_LVL_CUSTOM),
    m_modLevelData(data),
    m_modData(extraData),
    m_customSetting(setting)
{}

HistoryElementItemSetting::~HistoryElementItemSetting()
{
    if(m_customSetting)
        delete m_customSetting;
    m_customSetting = nullptr;
}

QString HistoryElementItemSetting::getHistoryName()
{
    if(m_modLevelSetting == HistorySettings::SETTING_LVL_CUSTOM || m_modWorldSetting == HistorySettings::SETTING_WLD_CUSTOM)
    {
        return m_customSetting->getHistoryName();
    }
    else if(qobject_cast<LvlScene*>(m_scene))
    {
        return HistorySettings::settingToString(m_modLevelSetting);
    }
    else if(qobject_cast<WldScene*>(m_scene))
    {
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
        connect(&worldSearcher, SIGNAL(foundLevel(WorldLevelTile,QGraphicsItem*)), this, SLOT(historyUndoSettingPathBackgroundLevel(WorldLevelTile,QGraphicsItem*)));
    }else if(m_modWorldSetting == HistorySettings::SETTING_BIGPATHBACKGROUND){
        connect(&worldSearcher, SIGNAL(foundLevel(WorldLevelTile,QGraphicsItem*)), this, SLOT(historyUndoSettingBigPathBackgroundLevel(WorldLevelTile,QGraphicsItem*)));
    }else if(m_modWorldSetting == HistorySettings::SETTING_ALWAYSVISIBLE){
        connect(&worldSearcher, SIGNAL(foundLevel(WorldLevelTile,QGraphicsItem*)), this, SLOT(historyUndoSettingAlwaysVisibleLevel(WorldLevelTile,QGraphicsItem*)));
    }else if(m_modWorldSetting == HistorySettings::SETTING_GAMESTARTPOINT){
        connect(&worldSearcher, SIGNAL(foundLevel(WorldLevelTile,QGraphicsItem*)), this, SLOT(historyUndoSettingGameStartPointLevel(WorldLevelTile,QGraphicsItem*)));
    }else if(m_modWorldSetting == HistorySettings::SETTING_DOORID){
        connect(&worldSearcher, SIGNAL(foundLevel(WorldLevelTile,QGraphicsItem*)), this, SLOT(historyUndoSettingDoorIDLevel(WorldLevelTile,QGraphicsItem*)));
    }else if(m_modWorldSetting == HistorySettings::SETTING_PATHBYTOP){
        connect(&worldSearcher, SIGNAL(foundLevel(WorldLevelTile,QGraphicsItem*)), this, SLOT(historyUndoSettingPathByTopLevel(WorldLevelTile,QGraphicsItem*)));
    }else if(m_modWorldSetting == HistorySettings::SETTING_PATHBYRIGHT){
        connect(&worldSearcher, SIGNAL(foundLevel(WorldLevelTile,QGraphicsItem*)), this, SLOT(historyUndoSettingPathByRightLevel(WorldLevelTile,QGraphicsItem*)));
    }else if(m_modWorldSetting == HistorySettings::SETTING_PATHBYBOTTOM){
        connect(&worldSearcher, SIGNAL(foundLevel(WorldLevelTile,QGraphicsItem*)), this, SLOT(historyUndoSettingPathByBottomLevel(WorldLevelTile,QGraphicsItem*)));
    }else if(m_modWorldSetting == HistorySettings::SETTING_PATHBYLEFT){
        connect(&worldSearcher, SIGNAL(foundLevel(WorldLevelTile,QGraphicsItem*)), this, SLOT(historyUndoSettingPathByLeftLevel(WorldLevelTile,QGraphicsItem*)));
    }else if(m_modWorldSetting == HistorySettings::SETTING_GOTOX){
        connect(&worldSearcher, SIGNAL(foundLevel(WorldLevelTile,QGraphicsItem*)), this, SLOT(historyUndoSettingGotoXLevel(WorldLevelTile,QGraphicsItem*)));
    }else if(m_modWorldSetting == HistorySettings::SETTING_GOTOY){
        connect(&worldSearcher, SIGNAL(foundLevel(WorldLevelTile,QGraphicsItem*)), this, SLOT(historyUndoSettingGotoYLevel(WorldLevelTile,QGraphicsItem*)));
    }else if(m_modWorldSetting == HistorySettings::SETTING_LEVELFILE){
        connect(&worldSearcher, SIGNAL(foundLevel(WorldLevelTile,QGraphicsItem*)), this, SLOT(historyUndoSettingLevelfileLevel(WorldLevelTile,QGraphicsItem*)));
    }else if(m_modWorldSetting == HistorySettings::SETTING_LEVELTITLE){
        connect(&worldSearcher, SIGNAL(foundLevel(WorldLevelTile,QGraphicsItem*)), this, SLOT(historyUndoSettingLeveltitleLevel(WorldLevelTile,QGraphicsItem*)));
    }else if(m_modWorldSetting == HistorySettings::SETTING_WLD_CUSTOM){
        worldSearcher.setFindFilter(ItemTypes::WLD_S_Tile|ItemTypes::WLD_S_Scenery|ItemTypes::WLD_S_Path|ItemTypes::WLD_S_Level|ItemTypes::WLD_S_MusicBox);

        connect(&worldSearcher, SIGNAL(foundTile(WorldTerrainTile,QGraphicsItem*)),
                this, SLOT(historyUndoSettingCustom(WorldTerrainTile,QGraphicsItem*)));

        connect(&worldSearcher, SIGNAL(foundScenery(WorldScenery,QGraphicsItem*)),
                this, SLOT(historyUndoSettingCustom(WorldScenery,QGraphicsItem*)));

        connect(&worldSearcher, SIGNAL(foundPath(WorldPathTile,QGraphicsItem*)),
                this, SLOT(historyUndoSettingCustom(WorldPathTile,QGraphicsItem*)));

        connect(&worldSearcher, SIGNAL(foundLevel(WorldLevelTile,QGraphicsItem*)),
                this, SLOT(historyUndoSettingCustom(WorldLevelTile,QGraphicsItem*)));

        connect(&worldSearcher, SIGNAL(foundMusicbox(WorldMusicBox,QGraphicsItem*)),
                this, SLOT(historyUndoSettingCustom(WorldMusicBox,QGraphicsItem*)));
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
    if(m_modLevelSetting == HistorySettings::SETTING_Z_LAYER){
        levelSearcher.setFindFilter(ItemTypes::LVL_S_BGO);
        connect(&levelSearcher, SIGNAL(foundBGO(LevelBGO,QGraphicsItem*)), this, SLOT(historyUndoSettingsZLayerBgo(LevelBGO,QGraphicsItem*)));
    }
    else
    if(m_modLevelSetting == HistorySettings::SETTING_Z_OFFSET){
        levelSearcher.setFindFilter(ItemTypes::LVL_S_BGO);
        connect(&levelSearcher, SIGNAL(foundBGO(LevelBGO,QGraphicsItem*)), this, SLOT(historyUndoSettingsZOffsetBgo(LevelBGO,QGraphicsItem*)));
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
    if(m_modLevelSetting == HistorySettings::SETTING_TWOWAY){
        levelSearcher.setFindFilter(ItemTypes::LVL_S_Door);
        connect(&levelSearcher, SIGNAL(foundDoor(LevelDoor,QGraphicsItem*)), this, SLOT(historyUndoSettingsTwoWayDoors(LevelDoor,QGraphicsItem*)));
    }
    else
    if(m_modLevelSetting == HistorySettings::SETTING_NOVEHICLE){
        levelSearcher.setFindFilter(ItemTypes::LVL_S_Door);
        connect(&levelSearcher, SIGNAL(foundDoor(LevelDoor,QGraphicsItem*)), this, SLOT(historyUndoSettingsNoVehiclesDoors(LevelDoor,QGraphicsItem*)));
    }
    else
    if(m_modLevelSetting == HistorySettings::SETTING_ALLOWNPC){
        levelSearcher.setFindFilter(ItemTypes::LVL_S_Door);
        connect(&levelSearcher, SIGNAL(foundDoor(LevelDoor,QGraphicsItem*)), this, SLOT(historyUndoSettingsAllowNPCDoors(LevelDoor,QGraphicsItem*)));
    }
    else
    if(m_modLevelSetting == HistorySettings::SETTING_ALLOWNPC_IL){
        levelSearcher.setFindFilter(ItemTypes::LVL_S_Door);
        connect(&levelSearcher, SIGNAL(foundDoor(LevelDoor,QGraphicsItem*)), this, SLOT(historyUndoSettingsAllowNPCILDoors(LevelDoor,QGraphicsItem*)));
    }
    else
    if(m_modLevelSetting == HistorySettings::SETTING_LOCKED){
        levelSearcher.setFindFilter(ItemTypes::LVL_S_Door);
        connect(&levelSearcher, SIGNAL(foundDoor(LevelDoor,QGraphicsItem*)), this, SLOT(historyUndoSettingsLockedDoors(LevelDoor,QGraphicsItem*)));
    }
    else
    if(m_modLevelSetting == HistorySettings::SETTING_NEED_A_BOMB){
        levelSearcher.setFindFilter(ItemTypes::LVL_S_Door);
        connect(&levelSearcher, SIGNAL(foundDoor(LevelDoor,QGraphicsItem*)), this, SLOT(historyUndoSettingsBombNeedDoors(LevelDoor,QGraphicsItem*)));
    }
    else
    if(m_modLevelSetting == HistorySettings::SETTING_HIDE_STAR_NUMBER){
        levelSearcher.setFindFilter(ItemTypes::LVL_S_Door);
        connect(&levelSearcher, SIGNAL(foundDoor(LevelDoor,QGraphicsItem*)), this, SLOT(historyUndoSettingsHideStarNumDoors(LevelDoor,QGraphicsItem*)));
    }
    else
    if(m_modLevelSetting == HistorySettings::SETTING_ENABLE_CANNON){
        levelSearcher.setFindFilter(ItemTypes::LVL_S_Door);
        connect(&levelSearcher, SIGNAL(foundDoor(LevelDoor,QGraphicsItem*)), this, SLOT(historyUndoSettingsEnCannonDoors(LevelDoor,QGraphicsItem*)));
    }
    else
    if(m_modLevelSetting == HistorySettings::SETTING_W_SPECIAL_STATE_REQUIRED){
        levelSearcher.setFindFilter(ItemTypes::LVL_S_Door);
        connect(&levelSearcher, SIGNAL(foundDoor(LevelDoor,QGraphicsItem*)), this, SLOT(historyUndoSettingsSpecialStateReqDoors(LevelDoor,QGraphicsItem*)));
    }
    else
    if(m_modLevelSetting == HistorySettings::SETTING_HIDE_LEVEL_ENTER_SCENE){
        levelSearcher.setFindFilter(ItemTypes::LVL_S_Door);
        connect(&levelSearcher, SIGNAL(foundDoor(LevelDoor,QGraphicsItem*)), this, SLOT(historyUndoSettingsHideLvlEntSceneDoors(LevelDoor,QGraphicsItem*)));
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
    else
    if(m_modLevelSetting == HistorySettings::SETTING_LVL_CUSTOM){
        levelSearcher.setFindFilter(ItemTypes::LVL_S_Block|ItemTypes::LVL_S_BGO|ItemTypes::LVL_S_NPC|ItemTypes::LVL_S_Door|ItemTypes::LVL_S_PhysEnv);

        connect(&levelSearcher, SIGNAL(foundBlock(LevelBlock,QGraphicsItem*)),
                this, SLOT(historyUndoSettingCustom(LevelBlock,QGraphicsItem*)));

        connect(&levelSearcher, SIGNAL(foundBGO(LevelBGO,QGraphicsItem*)),
                this, SLOT(historyUndoSettingCustom(LevelBGO,QGraphicsItem*)));

        connect(&levelSearcher, SIGNAL(foundNPC(LevelNPC,QGraphicsItem*)),
                this, SLOT(historyUndoSettingCustom(LevelNPC,QGraphicsItem*)));

        connect(&levelSearcher, SIGNAL(foundPhysEnv(LevelPhysEnv,QGraphicsItem*)),
                this, SLOT(historyUndoSettingCustom(LevelPhysEnv,QGraphicsItem*)));

        connect(&levelSearcher, SIGNAL(foundDoor(LevelDoor,QGraphicsItem*)),
                this, SLOT(historyUndoSettingCustom(LevelDoor,QGraphicsItem*)));
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
        connect(&worldSearcher, SIGNAL(foundLevel(WorldLevelTile,QGraphicsItem*)), this, SLOT(historyRedoSettingPathBackgroundLevel(WorldLevelTile,QGraphicsItem*)));
    }else if(m_modWorldSetting == HistorySettings::SETTING_BIGPATHBACKGROUND){
        connect(&worldSearcher, SIGNAL(foundLevel(WorldLevelTile,QGraphicsItem*)), this, SLOT(historyRedoSettingBigPathBackgroundLevel(WorldLevelTile,QGraphicsItem*)));
    }else if(m_modWorldSetting == HistorySettings::SETTING_ALWAYSVISIBLE){
        connect(&worldSearcher, SIGNAL(foundLevel(WorldLevelTile,QGraphicsItem*)), this, SLOT(historyRedoSettingAlwaysVisibleLevel(WorldLevelTile,QGraphicsItem*)));
    }else if(m_modWorldSetting == HistorySettings::SETTING_GAMESTARTPOINT){
        connect(&worldSearcher, SIGNAL(foundLevel(WorldLevelTile,QGraphicsItem*)), this, SLOT(historyRedoSettingGameStartPointLevel(WorldLevelTile,QGraphicsItem*)));
    }else if(m_modWorldSetting == HistorySettings::SETTING_DOORID){
        connect(&worldSearcher, SIGNAL(foundLevel(WorldLevelTile,QGraphicsItem*)), this, SLOT(historyRedoSettingDoorIDLevel(WorldLevelTile,QGraphicsItem*)));
    }else if(m_modWorldSetting == HistorySettings::SETTING_PATHBYTOP){
        connect(&worldSearcher, SIGNAL(foundLevel(WorldLevelTile,QGraphicsItem*)), this, SLOT(historyRedoSettingPathByTopLevel(WorldLevelTile,QGraphicsItem*)));
    }else if(m_modWorldSetting == HistorySettings::SETTING_PATHBYRIGHT){
        connect(&worldSearcher, SIGNAL(foundLevel(WorldLevelTile,QGraphicsItem*)), this, SLOT(historyRedoSettingPathByRightLevel(WorldLevelTile,QGraphicsItem*)));
    }else if(m_modWorldSetting == HistorySettings::SETTING_PATHBYBOTTOM){
        connect(&worldSearcher, SIGNAL(foundLevel(WorldLevelTile,QGraphicsItem*)), this, SLOT(historyRedoSettingPathByBottomLevel(WorldLevelTile,QGraphicsItem*)));
    }else if(m_modWorldSetting == HistorySettings::SETTING_PATHBYLEFT){
        connect(&worldSearcher, SIGNAL(foundLevel(WorldLevelTile,QGraphicsItem*)), this, SLOT(historyRedoSettingPathByLeftLevel(WorldLevelTile,QGraphicsItem*)));
    }else if(m_modWorldSetting == HistorySettings::SETTING_GOTOX){
        connect(&worldSearcher, SIGNAL(foundLevel(WorldLevelTile,QGraphicsItem*)), this, SLOT(historyRedoSettingGotoXLevel(WorldLevelTile,QGraphicsItem*)));
    }else if(m_modWorldSetting == HistorySettings::SETTING_GOTOY){
        connect(&worldSearcher, SIGNAL(foundLevel(WorldLevelTile,QGraphicsItem*)), this, SLOT(historyRedoSettingGotoYLevel(WorldLevelTile,QGraphicsItem*)));
    }else if(m_modWorldSetting == HistorySettings::SETTING_LEVELFILE){
        connect(&worldSearcher, SIGNAL(foundLevel(WorldLevelTile,QGraphicsItem*)), this, SLOT(historyRedoSettingLevelfileLevel(WorldLevelTile,QGraphicsItem*)));
    }else if(m_modWorldSetting == HistorySettings::SETTING_LEVELTITLE){
        connect(&worldSearcher, SIGNAL(foundLevel(WorldLevelTile,QGraphicsItem*)), this, SLOT(historyRedoSettingLeveltitleLevel(WorldLevelTile,QGraphicsItem*)));
    }else if(m_modWorldSetting == HistorySettings::SETTING_WLD_CUSTOM){
        worldSearcher.setFindFilter(ItemTypes::itemTypesMultiSelectable(ItemTypes::WLD_S_Tile|ItemTypes::WLD_S_Scenery|ItemTypes::WLD_S_Path|ItemTypes::WLD_S_Level|ItemTypes::WLD_S_MusicBox));

        connect(&worldSearcher, SIGNAL(foundTile(WorldTerrainTile,QGraphicsItem*)),
                this, SLOT(historyRedoSettingCustom(WorldTerrainTile,QGraphicsItem*)));

        connect(&worldSearcher, SIGNAL(foundScenery(WorldScenery,QGraphicsItem*)),
                this, SLOT(historyRedoSettingCustom(WorldScenery,QGraphicsItem*)));

        connect(&worldSearcher, SIGNAL(foundPath(WorldPathTile,QGraphicsItem*)),
                this, SLOT(historyRedoSettingCustom(WorldPathTile,QGraphicsItem*)));

        connect(&worldSearcher, SIGNAL(foundLevel(WorldLevelTile,QGraphicsItem*)),
                this, SLOT(historyRedoSettingCustom(WorldLevelTile,QGraphicsItem*)));

        connect(&worldSearcher, SIGNAL(foundMusicbox(WorldMusicBox,QGraphicsItem*)),
                this, SLOT(historyRedoSettingCustom(WorldMusicBox,QGraphicsItem*)));
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
    if(m_modLevelSetting == HistorySettings::SETTING_Z_LAYER){
        levelSearcher.setFindFilter(ItemTypes::LVL_S_BGO);
        connect(&levelSearcher, SIGNAL(foundBGO(LevelBGO,QGraphicsItem*)), this, SLOT(historyRedoSettingsZLayerBgo(LevelBGO,QGraphicsItem*)));
    }
    else
    if(m_modLevelSetting == HistorySettings::SETTING_Z_OFFSET){
        levelSearcher.setFindFilter(ItemTypes::LVL_S_BGO);
        connect(&levelSearcher, SIGNAL(foundBGO(LevelBGO,QGraphicsItem*)), this, SLOT(historyRedoSettingsZOffsetBgo(LevelBGO,QGraphicsItem*)));
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
    if(m_modLevelSetting == HistorySettings::SETTING_TWOWAY){
        levelSearcher.setFindFilter(ItemTypes::LVL_S_Door);
        connect(&levelSearcher, SIGNAL(foundDoor(LevelDoor,QGraphicsItem*)), this, SLOT(historyRedoSettingsTwoWayDoors(LevelDoor,QGraphicsItem*)));
    }
    else
    if(m_modLevelSetting == HistorySettings::SETTING_NOVEHICLE){
        levelSearcher.setFindFilter(ItemTypes::LVL_S_Door);
        connect(&levelSearcher, SIGNAL(foundDoor(LevelDoor,QGraphicsItem*)), this, SLOT(historyRedoSettingsNoVehiclesDoors(LevelDoor,QGraphicsItem*)));
    }
    else
    if(m_modLevelSetting == HistorySettings::SETTING_ALLOWNPC){
        levelSearcher.setFindFilter(ItemTypes::LVL_S_Door);
        connect(&levelSearcher, SIGNAL(foundDoor(LevelDoor,QGraphicsItem*)), this, SLOT(historyRedoSettingsAllowNPCDoors(LevelDoor,QGraphicsItem*)));
    }
    else
    if(m_modLevelSetting == HistorySettings::SETTING_ALLOWNPC_IL){
        levelSearcher.setFindFilter(ItemTypes::LVL_S_Door);
        connect(&levelSearcher, SIGNAL(foundDoor(LevelDoor,QGraphicsItem*)), this, SLOT(historyRedoSettingsAllowNPCILDoors(LevelDoor,QGraphicsItem*)));
    }
    else
    if(m_modLevelSetting == HistorySettings::SETTING_LOCKED){
        levelSearcher.setFindFilter(ItemTypes::LVL_S_Door);
        connect(&levelSearcher, SIGNAL(foundDoor(LevelDoor,QGraphicsItem*)), this, SLOT(historyRedoSettingsLockedDoors(LevelDoor,QGraphicsItem*)));
    }
    else
    if(m_modLevelSetting == HistorySettings::SETTING_NEED_A_BOMB){
        levelSearcher.setFindFilter(ItemTypes::LVL_S_Door);
        connect(&levelSearcher, SIGNAL(foundDoor(LevelDoor,QGraphicsItem*)), this, SLOT(historyRedoSettingsBombNeedDoors(LevelDoor,QGraphicsItem*)));
    }
    else
    if(m_modLevelSetting == HistorySettings::SETTING_HIDE_STAR_NUMBER){
        levelSearcher.setFindFilter(ItemTypes::LVL_S_Door);
        connect(&levelSearcher, SIGNAL(foundDoor(LevelDoor,QGraphicsItem*)), this, SLOT(historyRedoSettingsHideStarNumDoors(LevelDoor,QGraphicsItem*)));
    }
    else
    if(m_modLevelSetting == HistorySettings::SETTING_ENABLE_CANNON){
        levelSearcher.setFindFilter(ItemTypes::LVL_S_Door);
        connect(&levelSearcher, SIGNAL(foundDoor(LevelDoor,QGraphicsItem*)), this, SLOT(historyRedoSettingsEnCannonDoors(LevelDoor,QGraphicsItem*)));
    }
    else
    if(m_modLevelSetting == HistorySettings::SETTING_W_SPECIAL_STATE_REQUIRED){
        levelSearcher.setFindFilter(ItemTypes::LVL_S_Door);
        connect(&levelSearcher, SIGNAL(foundDoor(LevelDoor,QGraphicsItem*)), this, SLOT(historyRedoSettingsSpecialStateReqDoors(LevelDoor,QGraphicsItem*)));
    }
    else
    if(m_modLevelSetting == HistorySettings::SETTING_HIDE_LEVEL_ENTER_SCENE){
        levelSearcher.setFindFilter(ItemTypes::LVL_S_Door);
        connect(&levelSearcher, SIGNAL(foundDoor(LevelDoor,QGraphicsItem*)), this, SLOT(historyRedoSettingsHideLvlEntSceneDoors(LevelDoor,QGraphicsItem*)));
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
    else
    if(m_modLevelSetting == HistorySettings::SETTING_LVL_CUSTOM){
        levelSearcher.setFindFilter(ItemTypes::LVL_S_Block|ItemTypes::LVL_S_BGO|ItemTypes::LVL_S_NPC|ItemTypes::LVL_S_Door|ItemTypes::LVL_S_PhysEnv);

        connect(&levelSearcher, SIGNAL(foundBlock(LevelBlock,QGraphicsItem*)),
                this, SLOT(historyRedoSettingCustom(LevelBlock,QGraphicsItem*)));

        connect(&levelSearcher, SIGNAL(foundBGO(LevelBGO,QGraphicsItem*)),
                this, SLOT(historyRedoSettingCustom(LevelBGO,QGraphicsItem*)));

        connect(&levelSearcher, SIGNAL(foundNPC(LevelNPC,QGraphicsItem*)),
                this, SLOT(historyRedoSettingCustom(LevelNPC,QGraphicsItem*)));

        connect(&levelSearcher, SIGNAL(foundPhysEnv(LevelPhysEnv,QGraphicsItem*)),
                this, SLOT(historyRedoSettingCustom(LevelPhysEnv,QGraphicsItem*)));

        connect(&levelSearcher, SIGNAL(foundDoor(LevelDoor,QGraphicsItem*)),
                this, SLOT(historyRedoSettingCustom(LevelDoor,QGraphicsItem*)));
    }
    levelSearcher.find(m_modLevelData, m_scene->items());
}

void HistoryElementItemSetting::historyUndoSettingPathBackgroundLevel(const WorldLevelTile &sourceLevel, QGraphicsItem *item)
{
    ((ItemLevel*)item)->setShowSmallPathBG(sourceLevel.pathbg);
}

void HistoryElementItemSetting::historyRedoSettingPathBackgroundLevel(const WorldLevelTile &/*sourceLevel*/, QGraphicsItem *item)
{
    ((ItemLevel*)item)->setShowSmallPathBG(m_modData.toBool());
}

void HistoryElementItemSetting::historyUndoSettingBigPathBackgroundLevel(const WorldLevelTile &sourceLevel, QGraphicsItem *item)
{
    ((ItemLevel*)item)->setShowBigPathBG(sourceLevel.bigpathbg);
}

void HistoryElementItemSetting::historyRedoSettingBigPathBackgroundLevel(const WorldLevelTile &/*sourceLevel*/, QGraphicsItem *item)
{
    ((ItemLevel*)item)->setShowBigPathBG(m_modData.toBool());
}

void HistoryElementItemSetting::historyUndoSettingAlwaysVisibleLevel(const WorldLevelTile &sourceLevel, QGraphicsItem *item)
{
    ((ItemLevel*)item)->alwaysVisible(sourceLevel.alwaysVisible);
}

void HistoryElementItemSetting::historyRedoSettingAlwaysVisibleLevel(const WorldLevelTile &/*sourceLevel*/, QGraphicsItem *item)
{
    ((ItemLevel*)item)->alwaysVisible(m_modData.toBool());
}

void HistoryElementItemSetting::historyUndoSettingGameStartPointLevel(const WorldLevelTile &sourceLevel, QGraphicsItem *item)
{
    ((ItemLevel*)item)->m_data.gamestart = sourceLevel.gamestart;
    ((ItemLevel*)item)->arrayApply();
}

void HistoryElementItemSetting::historyRedoSettingGameStartPointLevel(const WorldLevelTile &/*sourceLevel*/, QGraphicsItem *item)
{
    ((ItemLevel*)item)->m_data.gamestart = m_modData.toBool();
    ((ItemLevel*)item)->arrayApply();
}

void HistoryElementItemSetting::historyUndoSettingLevelfileLevel(const WorldLevelTile &sourceLevel, QGraphicsItem *item)
{
    ((ItemLevel*)item)->m_data.lvlfile = sourceLevel.lvlfile;
    ((ItemLevel*)item)->arrayApply();
}

void HistoryElementItemSetting::historyRedoSettingLevelfileLevel(const WorldLevelTile &/*sourceLevel*/, QGraphicsItem *item)
{
    ((ItemLevel*)item)->m_data.lvlfile = m_modData.toString();
    ((ItemLevel*)item)->arrayApply();
}

void HistoryElementItemSetting::historyUndoSettingLeveltitleLevel(const WorldLevelTile &sourceLevel, QGraphicsItem *item)
{
    ((ItemLevel*)item)->m_data.title = sourceLevel.title;
    ((ItemLevel*)item)->arrayApply();
}

void HistoryElementItemSetting::historyRedoSettingLeveltitleLevel(const WorldLevelTile &/*sourceLevel*/, QGraphicsItem *item)
{
    ((ItemLevel*)item)->m_data.title = m_modData.toString();
    ((ItemLevel*)item)->arrayApply();
}

void HistoryElementItemSetting::historyUndoSettingDoorIDLevel(const WorldLevelTile &sourceLevel, QGraphicsItem *item)
{
    ((ItemLevel*)item)->m_data.entertowarp = sourceLevel.entertowarp;
    ((ItemLevel*)item)->arrayApply();
}

void HistoryElementItemSetting::historyRedoSettingDoorIDLevel(const WorldLevelTile &/*sourceLevel*/, QGraphicsItem *item)
{
    ((ItemLevel*)item)->m_data.entertowarp = m_modData.toInt();
    ((ItemLevel*)item)->arrayApply();
}

void HistoryElementItemSetting::historyUndoSettingPathByTopLevel(const WorldLevelTile &sourceLevel, QGraphicsItem *item)
{
    ((ItemLevel*)item)->m_data.top_exit = sourceLevel.top_exit;
    ((ItemLevel*)item)->arrayApply();
}

void HistoryElementItemSetting::historyRedoSettingPathByTopLevel(const WorldLevelTile &/*sourceLevel*/, QGraphicsItem *item)
{
    ((ItemLevel*)item)->m_data.top_exit = m_modData.toInt();
    ((ItemLevel*)item)->arrayApply();
}

void HistoryElementItemSetting::historyUndoSettingPathByRightLevel(const WorldLevelTile &sourceLevel, QGraphicsItem *item)
{
    ((ItemLevel*)item)->m_data.right_exit = sourceLevel.right_exit;
    ((ItemLevel*)item)->arrayApply();
}

void HistoryElementItemSetting::historyRedoSettingPathByRightLevel(const WorldLevelTile &/*sourceLevel*/, QGraphicsItem *item)
{
    ((ItemLevel*)item)->m_data.right_exit = m_modData.toInt();
    ((ItemLevel*)item)->arrayApply();
}

void HistoryElementItemSetting::historyUndoSettingPathByBottomLevel(const WorldLevelTile &sourceLevel, QGraphicsItem *item)
{
    ((ItemLevel*)item)->m_data.bottom_exit = sourceLevel.bottom_exit;
    ((ItemLevel*)item)->arrayApply();
}

void HistoryElementItemSetting::historyRedoSettingPathByBottomLevel(const WorldLevelTile &/*sourceLevel*/, QGraphicsItem *item)
{
    ((ItemLevel*)item)->m_data.bottom_exit = m_modData.toInt();
    ((ItemLevel*)item)->arrayApply();
}

void HistoryElementItemSetting::historyUndoSettingPathByLeftLevel(const WorldLevelTile &sourceLevel, QGraphicsItem *item)
{
    ((ItemLevel*)item)->m_data.left_exit = sourceLevel.left_exit;
    ((ItemLevel*)item)->arrayApply();
}

void HistoryElementItemSetting::historyRedoSettingPathByLeftLevel(const WorldLevelTile &/*sourceLevel*/, QGraphicsItem *item)
{
    ((ItemLevel*)item)->m_data.left_exit = m_modData.toInt();
    ((ItemLevel*)item)->arrayApply();
}

void HistoryElementItemSetting::historyUndoSettingGotoXLevel(const WorldLevelTile &sourceLevel, QGraphicsItem *item)
{
    ((ItemLevel*)item)->m_data.gotox = sourceLevel.gotox;
    ((ItemLevel*)item)->arrayApply();
}

void HistoryElementItemSetting::historyRedoSettingGotoXLevel(const WorldLevelTile &/*sourceLevel*/, QGraphicsItem *item)
{
    ((ItemLevel*)item)->m_data.gotox = m_modData.toInt();
    ((ItemLevel*)item)->arrayApply();
}

void HistoryElementItemSetting::historyUndoSettingGotoYLevel(const WorldLevelTile &sourceLevel, QGraphicsItem *item)
{
    ((ItemLevel*)item)->m_data.gotoy = sourceLevel.gotoy;
    ((ItemLevel*)item)->arrayApply();
}

void HistoryElementItemSetting::historyRedoSettingGotoYLevel(const WorldLevelTile &/*sourceLevel*/, QGraphicsItem *item)
{
    ((ItemLevel*)item)->m_data.gotoy = m_modData.toInt();
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







void HistoryElementItemSetting::historyUndoSettingsZLayerBgo(const LevelBGO &sourceBgo, QGraphicsItem *item)
{
    ((ItemBGO*)item)->setZMode(sourceBgo.z_mode, ((ItemBGO*)item)->m_data.z_offset);
}

void HistoryElementItemSetting::historyRedoSettingsZLayerBgo(const LevelBGO &/*sourceBgo*/, QGraphicsItem *item)
{
    ((ItemBGO*)item)->setZMode(m_modData.toInt(), ((ItemBGO*)item)->m_data.z_offset);
}

void HistoryElementItemSetting::historyUndoSettingsZOffsetBgo(const LevelBGO &sourceBgo, QGraphicsItem *item)
{
    ((ItemBGO*)item)->setZMode(((ItemBGO*)item)->m_data.z_mode, sourceBgo.z_offset);
}

void HistoryElementItemSetting::historyRedoSettingsZOffsetBgo(const LevelBGO &/*sourceBgo*/, QGraphicsItem *item)
{
    ((ItemBGO*)item)->setZMode(((ItemBGO*)item)->m_data.z_mode, m_modData.toReal());
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
    ((ItemNPC*)item)->setLegacyBoss(sourceNPC.is_boss);
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
    ((ItemNPC*)item)->setIncludedNPC(sourceNPC.contents);
}

void HistoryElementItemSetting::historyRedoSettingsChangeNPCNPC(const LevelNPC &/*sourceNPC*/, QGraphicsItem *item)
{
    ((ItemNPC*)item)->setIncludedNPC(m_modData.toInt());
}

void HistoryElementItemSetting::historyUndoSettingsTypeWater(const LevelPhysEnv &sourcePhysEnv, QGraphicsItem *item)
{
    ((ItemPhysEnv*)item)->setType(sourcePhysEnv.env_type);
}

void HistoryElementItemSetting::historyRedoSettingsTypeWater(const LevelPhysEnv &/*sourcePhysEnv*/, QGraphicsItem *item)
{
    ((ItemPhysEnv*)item)->setType(m_modData.toBool() ? 0 : 1);
}

void HistoryElementItemSetting::historyUndoSettingsTwoWayDoors(const LevelDoor &/*sourceDoors*/, QGraphicsItem *item)
{
    ((ItemDoor*)item)->m_data.two_way = !m_modData.toBool();
    ((ItemDoor*)item)->arrayApply();
}

void HistoryElementItemSetting::historyRedoSettingsTwoWayDoors(const LevelDoor &/*sourceDoors*/, QGraphicsItem *item)
{
    ((ItemDoor*)item)->m_data.two_way = m_modData.toBool();
    ((ItemDoor*)item)->arrayApply();
}

void HistoryElementItemSetting::historyUndoSettingsNoVehiclesDoors(const LevelDoor &/*sourceDoors*/, QGraphicsItem *item)
{
    ((ItemDoor*)item)->m_data.novehicles = !m_modData.toBool();
    ((ItemDoor*)item)->arrayApply();
}

void HistoryElementItemSetting::historyRedoSettingsNoVehiclesDoors(const LevelDoor &/*sourceDoors*/, QGraphicsItem *item)
{
    ((ItemDoor*)item)->m_data.novehicles = m_modData.toBool();
    ((ItemDoor*)item)->arrayApply();
}

void HistoryElementItemSetting::historyUndoSettingsAllowNPCDoors(const LevelDoor &/*sourceDoors*/, QGraphicsItem *item)
{
    ((ItemDoor*)item)->m_data.allownpc = !m_modData.toBool();
    ((ItemDoor*)item)->arrayApply();
}

void HistoryElementItemSetting::historyRedoSettingsAllowNPCDoors(const LevelDoor &/*sourceDoors*/, QGraphicsItem *item)
{
    ((ItemDoor*)item)->m_data.allownpc = m_modData.toBool();
    ((ItemDoor*)item)->arrayApply();
}

void HistoryElementItemSetting::historyUndoSettingsAllowNPCILDoors(const LevelDoor &/*sourceDoors*/, QGraphicsItem *item)
{
    ((ItemDoor*)item)->m_data.allownpc_interlevel = !m_modData.toBool();
    ((ItemDoor*)item)->arrayApply();
}

void HistoryElementItemSetting::historyRedoSettingsAllowNPCILDoors(const LevelDoor &/*sourceDoors*/, QGraphicsItem *item)
{
    ((ItemDoor*)item)->m_data.allownpc_interlevel = m_modData.toBool();
    ((ItemDoor*)item)->arrayApply();
}

void HistoryElementItemSetting::historyUndoSettingsLockedDoors(const LevelDoor &/*sourceDoors*/, QGraphicsItem *item)
{
    ((ItemDoor*)item)->m_data.locked = !m_modData.toBool();
    ((ItemDoor*)item)->arrayApply();
}

void HistoryElementItemSetting::historyRedoSettingsLockedDoors(const LevelDoor &/*sourceDoors*/, QGraphicsItem *item)
{
    ((ItemDoor*)item)->m_data.locked = m_modData.toBool();
    ((ItemDoor*)item)->arrayApply();
}

void HistoryElementItemSetting::historyUndoSettingsBombNeedDoors(const LevelDoor &/*sourceDoors*/, QGraphicsItem *item)
{
    ((ItemDoor*)item)->m_data.need_a_bomb = !m_modData.toBool();
    ((ItemDoor*)item)->arrayApply();
}

void HistoryElementItemSetting::historyRedoSettingsBombNeedDoors(const LevelDoor &/*sourceDoors*/, QGraphicsItem *item)
{
    ((ItemDoor*)item)->m_data.need_a_bomb = m_modData.toBool();
    ((ItemDoor*)item)->arrayApply();
}

void HistoryElementItemSetting::historyUndoSettingsHideStarNumDoors(const LevelDoor &/*sourceDoors*/, QGraphicsItem *item)
{
    ((ItemDoor*)item)->m_data.star_num_hide = !m_modData.toBool();
    ((ItemDoor*)item)->arrayApply();
}

void HistoryElementItemSetting::historyRedoSettingsHideStarNumDoors(const LevelDoor &/*sourceDoors*/, QGraphicsItem *item)
{
    ((ItemDoor*)item)->m_data.star_num_hide = m_modData.toBool();
    ((ItemDoor*)item)->arrayApply();
}

void HistoryElementItemSetting::historyUndoSettingsEnCannonDoors(const LevelDoor &/*sourceDoors*/, QGraphicsItem *item)
{
    ((ItemDoor*)item)->m_data.cannon_exit = !m_modData.toBool();
    ((ItemDoor*)item)->arrayApply();
}

void HistoryElementItemSetting::historyRedoSettingsEnCannonDoors(const LevelDoor &/*sourceDoors*/, QGraphicsItem *item)
{
    ((ItemDoor*)item)->m_data.cannon_exit = m_modData.toBool();
    ((ItemDoor*)item)->arrayApply();
}

void HistoryElementItemSetting::historyUndoSettingsSpecialStateReqDoors(const LevelDoor &/*sourceDoors*/, QGraphicsItem *item)
{
    ((ItemDoor*)item)->m_data.special_state_required = !m_modData.toBool();
    ((ItemDoor*)item)->arrayApply();
}

void HistoryElementItemSetting::historyRedoSettingsSpecialStateReqDoors(const LevelDoor &/*sourceDoors*/, QGraphicsItem *item)
{
    ((ItemDoor*)item)->m_data.special_state_required = m_modData.toBool();
    ((ItemDoor*)item)->arrayApply();
}

void HistoryElementItemSetting::historyUndoSettingsHideLvlEntSceneDoors(const LevelDoor &/*sourceDoors*/, QGraphicsItem *item)
{
    ((ItemDoor*)item)->m_data.hide_entering_scene = !m_modData.toBool();
    ((ItemDoor*)item)->arrayApply();
}

void HistoryElementItemSetting::historyRedoSettingsHideLvlEntSceneDoors(const LevelDoor &/*sourceDoors*/, QGraphicsItem *item)
{
    ((ItemDoor*)item)->m_data.hide_entering_scene = m_modData.toBool();
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
    ((ItemNPC*)item)->m_data.generator_period = sourceNPC.generator_period;
    ((ItemNPC*)item)->arrayApply();
}

void HistoryElementItemSetting::historyRedoSettingsTimeGeneratorNPC(const LevelNPC &/*sourceNPC*/, QGraphicsItem *item)
{
    ((ItemNPC*)item)->m_data.generator_period = m_modData.toInt();
    ((ItemNPC*)item)->arrayApply();
}

void HistoryElementItemSetting::historyUndoSettingsAttachLayerNPC(const LevelNPC &sourceNPC, QGraphicsItem *item)
{
    ((ItemNPC*)item)->m_data.attach_layer = sourceNPC.attach_layer;
    ((ItemNPC*)item)->arrayApply();
}

void HistoryElementItemSetting::historyRedoSettingsAttachLayerNPC(const LevelNPC &/*sourceNPC*/, QGraphicsItem *item)
{
    ((ItemNPC*)item)->m_data.attach_layer = m_modData.toString();
    ((ItemNPC*)item)->arrayApply();
}

void HistoryElementItemSetting::historyUndoSettingsDestroyedEventBlocks(const LevelBlock &sourceBlock, QGraphicsItem *item)
{
    ((ItemBlock*)item)->m_data.event_destroy = sourceBlock.event_destroy;
    ((ItemBlock*)item)->arrayApply();
}

void HistoryElementItemSetting::historyRedoSettingsDestroyedEventBlocks(const LevelBlock &/*sourceBlock*/, QGraphicsItem *item)
{
    ((ItemBlock*)item)->m_data.event_destroy = m_modData.toString();
    ((ItemBlock*)item)->arrayApply();
}

void HistoryElementItemSetting::historyUndoSettingsHitedEventBlocks(const LevelBlock &sourceBlock, QGraphicsItem *item)
{
    ((ItemBlock*)item)->m_data.event_hit = sourceBlock.event_hit;
    ((ItemBlock*)item)->arrayApply();
}

void HistoryElementItemSetting::historyRedoSettingsHitedEventBlocks(const LevelBlock &/*sourceBlock*/, QGraphicsItem *item)
{
    ((ItemBlock*)item)->m_data.event_hit = m_modData.toString();
    ((ItemBlock*)item)->arrayApply();
}

void HistoryElementItemSetting::historyUndoSettingsLayerEmptyEventBlocks(const LevelBlock &sourceBlock, QGraphicsItem *item)
{
    ((ItemBlock*)item)->m_data.event_emptylayer = sourceBlock.event_emptylayer;
    ((ItemBlock*)item)->arrayApply();
}

void HistoryElementItemSetting::historyRedoSettingsLayerEmptyEventBlocks(const LevelBlock &/*sourceBlock*/, QGraphicsItem *item)
{
    ((ItemBlock*)item)->m_data.event_emptylayer = m_modData.toString();
    ((ItemBlock*)item)->arrayApply();
}

void HistoryElementItemSetting::historyUndoSettingsLayerEmptyEventNPC(const LevelNPC &sourceNPC, QGraphicsItem *item)
{
    ((ItemNPC*)item)->m_data.event_emptylayer = sourceNPC.event_emptylayer;
    ((ItemNPC*)item)->arrayApply();
}

void HistoryElementItemSetting::historyRedoSettingsLayerEmptyEventNPC(const LevelNPC &/*sourceNPC*/, QGraphicsItem *item)
{
    ((ItemNPC*)item)->m_data.event_emptylayer = m_modData.toString();
    ((ItemNPC*)item)->arrayApply();
}

void HistoryElementItemSetting::historyUndoSettingsActivateEventNPC(const LevelNPC &sourceNPC, QGraphicsItem *item)
{
    ((ItemNPC*)item)->m_data.event_activate = sourceNPC.event_activate;
    ((ItemNPC*)item)->arrayApply();
}

void HistoryElementItemSetting::historyRedoSettingsActivateEventNPC(const LevelNPC &/*sourceNPC*/, QGraphicsItem *item)
{
    ((ItemNPC*)item)->m_data.event_activate = m_modData.toString();
    ((ItemNPC*)item)->arrayApply();
}

void HistoryElementItemSetting::historyUndoSettingsDeathEventNPC(const LevelNPC &sourceNPC, QGraphicsItem *item)
{
    ((ItemNPC*)item)->m_data.event_die = sourceNPC.event_die;
    ((ItemNPC*)item)->arrayApply();
}

void HistoryElementItemSetting::historyRedoSettingsDeathEventNPC(const LevelNPC &/*sourceNPC*/, QGraphicsItem *item)
{
    ((ItemNPC*)item)->m_data.event_die = m_modData.toString();
    ((ItemNPC*)item)->arrayApply();
}

void HistoryElementItemSetting::historyUndoSettingsTalkEventNPC(const LevelNPC &sourceNPC, QGraphicsItem *item)
{
    ((ItemNPC*)item)->m_data.event_talk = sourceNPC.event_talk;
    ((ItemNPC*)item)->arrayApply();
}

void HistoryElementItemSetting::historyRedoSettingsTalkEventNPC(const LevelNPC &/*sourceNPC*/, QGraphicsItem *item)
{
    ((ItemNPC*)item)->m_data.event_talk = m_modData.toString();
    ((ItemNPC*)item)->arrayApply();
}

void HistoryElementItemSetting::historyUndoSettingsSpecialDataNPC(const LevelNPC &sourceNPC, QGraphicsItem *item)
{
    ((ItemNPC*)item)->m_data.special_data = sourceNPC.special_data;
    ((ItemNPC*)item)->arrayApply();
}

void HistoryElementItemSetting::historyRedoSettingsSpecialDataNPC(const LevelNPC &/*sourceNPC*/, QGraphicsItem *item)
{
    ((ItemNPC*)item)->m_data.special_data = m_modData.toInt();
    ((ItemNPC*)item)->arrayApply();
}

void HistoryElementItemSetting::historyUndoSettingsSortingBGO(const LevelBGO &sourceBGO, QGraphicsItem *item)
{
    ((ItemBGO*)item)->m_data.smbx64_sp = sourceBGO.smbx64_sp;
    ((ItemBGO*)item)->arrayApply();
}

void HistoryElementItemSetting::historyRedoSettingsSortingBGO(const LevelBGO &/*sourceBGO*/, QGraphicsItem *item)
{
    ((ItemBGO*)item)->m_data.smbx64_sp = (long)m_modData.toLongLong();
    ((ItemBGO*)item)->arrayApply();
}

void HistoryElementItemSetting::historyUndoSettingCustom(const LevelBlock &sourceItem, QGraphicsItem *item)
{
    m_customSetting->undo(reinterpret_cast<const void*>(&sourceItem), &m_modData, item);
}

void HistoryElementItemSetting::historyRedoSettingCustom(const LevelBlock &sourceItem, QGraphicsItem *item)
{
    m_customSetting->redo(reinterpret_cast<const void*>(&sourceItem), &m_modData, item);
}

void HistoryElementItemSetting::historyUndoSettingCustom(const LevelBGO &sourceItem, QGraphicsItem *item)
{
    m_customSetting->undo(reinterpret_cast<const void*>(&sourceItem), &m_modData, item);
}

void HistoryElementItemSetting::historyRedoSettingCustom(const LevelBGO &sourceItem, QGraphicsItem *item)
{
    m_customSetting->redo(reinterpret_cast<const void*>(&sourceItem), &m_modData, item);
}

void HistoryElementItemSetting::historyUndoSettingCustom(const LevelNPC &sourceItem, QGraphicsItem *item)
{
    m_customSetting->undo(reinterpret_cast<const void*>(&sourceItem), &m_modData, item);
}

void HistoryElementItemSetting::historyRedoSettingCustom(const LevelNPC &sourceItem, QGraphicsItem *item)
{
    m_customSetting->redo(reinterpret_cast<const void*>(&sourceItem), &m_modData, item);
}

void HistoryElementItemSetting::historyUndoSettingCustom(const LevelDoor &sourceItem, QGraphicsItem *item)
{
    m_customSetting->undo(reinterpret_cast<const void*>(&sourceItem), &m_modData, item);
}

void HistoryElementItemSetting::historyRedoSettingCustom(const LevelDoor &sourceItem, QGraphicsItem *item)
{
    m_customSetting->redo(reinterpret_cast<const void*>(&sourceItem), &m_modData, item);
}

void HistoryElementItemSetting::historyUndoSettingCustom(const LevelPhysEnv &sourceItem, QGraphicsItem *item)
{
    m_customSetting->undo(reinterpret_cast<const void*>(&sourceItem), &m_modData, item);
}

void HistoryElementItemSetting::historyRedoSettingCustom(const LevelPhysEnv &sourceItem, QGraphicsItem *item)
{
    m_customSetting->redo(reinterpret_cast<const void*>(&sourceItem), &m_modData, item);
}

void HistoryElementItemSetting::historyUndoSettingCustom(const WorldTerrainTile &sourceItem, QGraphicsItem *item)
{
    m_customSetting->undo(reinterpret_cast<const void*>(&sourceItem), &m_modData, item);
}

void HistoryElementItemSetting::historyRedoSettingCustom(const WorldTerrainTile &sourceItem, QGraphicsItem *item)
{
    m_customSetting->redo(reinterpret_cast<const void*>(&sourceItem), &m_modData, item);
}

void HistoryElementItemSetting::historyUndoSettingCustom(const WorldScenery &sourceItem, QGraphicsItem *item)
{
    m_customSetting->undo(reinterpret_cast<const void*>(&sourceItem), &m_modData, item);
}

void HistoryElementItemSetting::historyRedoSettingCustom(const WorldScenery &sourceItem, QGraphicsItem *item)
{
    m_customSetting->redo(reinterpret_cast<const void*>(&sourceItem), &m_modData, item);
}

void HistoryElementItemSetting::historyUndoSettingCustom(const WorldPathTile &sourceItem, QGraphicsItem *item)
{
    m_customSetting->undo(reinterpret_cast<const void*>(&sourceItem), &m_modData, item);
}

void HistoryElementItemSetting::historyRedoSettingCustom(const WorldPathTile &sourceItem, QGraphicsItem *item)
{
    m_customSetting->redo(reinterpret_cast<const void*>(&sourceItem), &m_modData, item);
}

void HistoryElementItemSetting::historyUndoSettingCustom(const WorldLevelTile &sourceItem, QGraphicsItem *item)
{
    m_customSetting->undo(reinterpret_cast<const void*>(&sourceItem), &m_modData, item);
}

void HistoryElementItemSetting::historyRedoSettingCustom(const WorldLevelTile &sourceItem, QGraphicsItem *item)
{
    m_customSetting->redo(reinterpret_cast<const void*>(&sourceItem), &m_modData, item);
}

void HistoryElementItemSetting::historyUndoSettingCustom(const WorldMusicBox &sourceItem, QGraphicsItem *item)
{
    m_customSetting->undo(reinterpret_cast<const void*>(&sourceItem), &m_modData, item);
}

void HistoryElementItemSetting::historyRedoSettingCustom(const WorldMusicBox &sourceItem, QGraphicsItem *item)
{
    m_customSetting->redo(reinterpret_cast<const void*>(&sourceItem), &m_modData, item);
}
