#pragma once
#ifndef HISTORYELEMENTITEMSETTING_H
#define HISTORYELEMENTITEMSETTING_H

#include <QVariant>
#include "ihistoryelement.h"
#include "history_custom_setting.h"
#include <PGE_File_Formats/file_formats.h>
#include <defines.h>

class HistoryElementItemSetting : public QObject, public IHistoryElement
{
    Q_INTERFACES(IHistoryElement)
    Q_OBJECT

public:
    explicit HistoryElementItemSetting(const WorldData &data, HistorySettings::WorldSettingSubType wldSubType, QVariant extraData, QObject *parent = 0);
    explicit HistoryElementItemSetting(const LevelData &data, HistorySettings::LevelSettingSubType lvlSubType, QVariant extraData, QObject *parent = 0);
    explicit HistoryElementItemSetting(const WorldData &data, HistoryElementCustomSetting *setting, QVariant extraData, QObject *parent = 0);
    explicit HistoryElementItemSetting(const LevelData &data, HistoryElementCustomSetting *setting, QVariant extraData, QObject *parent = 0);
    virtual ~HistoryElementItemSetting();
    virtual QString getHistoryName();
    virtual void undo();
    virtual void redo();


    void processWorldUndo();
    void processLevelUndo();
    void processWorldRedo();
    void processLevelRedo();

signals:

public slots:
    //for world
    void historyUndoSettingPathBackgroundLevel(const WorldLevelTile &sourceLevel, QGraphicsItem* item);
    void historyRedoSettingPathBackgroundLevel(const WorldLevelTile &sourceLevel, QGraphicsItem* item);
    void historyUndoSettingBigPathBackgroundLevel(const WorldLevelTile &sourceLevel, QGraphicsItem* item);
    void historyRedoSettingBigPathBackgroundLevel(const WorldLevelTile &sourceLevel, QGraphicsItem* item);
    void historyUndoSettingAlwaysVisibleLevel(const WorldLevelTile &sourceLevel, QGraphicsItem* item);
    void historyRedoSettingAlwaysVisibleLevel(const WorldLevelTile &sourceLevel, QGraphicsItem* item);
    void historyUndoSettingGameStartPointLevel(const WorldLevelTile &sourceLevel, QGraphicsItem* item);
    void historyRedoSettingGameStartPointLevel(const WorldLevelTile &sourceLevel, QGraphicsItem* item);
    void historyUndoSettingLevelfileLevel(const WorldLevelTile &sourceLevel, QGraphicsItem* item);
    void historyRedoSettingLevelfileLevel(const WorldLevelTile &sourceLevel, QGraphicsItem* item);
    void historyUndoSettingLeveltitleLevel(const WorldLevelTile &sourceLevel, QGraphicsItem* item);
    void historyRedoSettingLeveltitleLevel(const WorldLevelTile &sourceLevel, QGraphicsItem* item);
    void historyUndoSettingDoorIDLevel(const WorldLevelTile &sourceLevel, QGraphicsItem* item);
    void historyRedoSettingDoorIDLevel(const WorldLevelTile &sourceLevel, QGraphicsItem* item);
    void historyUndoSettingPathByTopLevel(const WorldLevelTile &sourceLevel, QGraphicsItem* item);
    void historyRedoSettingPathByTopLevel(const WorldLevelTile &sourceLevel, QGraphicsItem* item);
    void historyUndoSettingPathByRightLevel(const WorldLevelTile &sourceLevel, QGraphicsItem* item);
    void historyRedoSettingPathByRightLevel(const WorldLevelTile &sourceLevel, QGraphicsItem* item);
    void historyUndoSettingPathByBottomLevel(const WorldLevelTile &sourceLevel, QGraphicsItem* item);
    void historyRedoSettingPathByBottomLevel(const WorldLevelTile &sourceLevel, QGraphicsItem* item);
    void historyUndoSettingPathByLeftLevel(const WorldLevelTile &sourceLevel, QGraphicsItem* item);
    void historyRedoSettingPathByLeftLevel(const WorldLevelTile &sourceLevel, QGraphicsItem* item);
    void historyUndoSettingGotoXLevel(const WorldLevelTile &sourceLevel, QGraphicsItem* item);
    void historyRedoSettingGotoXLevel(const WorldLevelTile &sourceLevel, QGraphicsItem* item);
    void historyUndoSettingGotoYLevel(const WorldLevelTile &sourceLevel, QGraphicsItem* item);
    void historyRedoSettingGotoYLevel(const WorldLevelTile &sourceLevel, QGraphicsItem* item);


    void historyUndoSettingsInvisibleBlock(const LevelBlock &sourceBlock, QGraphicsItem *item);
    void historyRedoSettingsInvisibleBlock(const LevelBlock &sourceBlock, QGraphicsItem *item);
    void historyUndoSettingsSlipperyBlock(const LevelBlock &sourceBlock, QGraphicsItem *item);
    void historyRedoSettingsSlipperyBlock(const LevelBlock &sourceBlock, QGraphicsItem *item);
    void historyUndoSettingsZLayerBgo(const LevelBGO &sourceBgo, QGraphicsItem *item);
    void historyRedoSettingsZLayerBgo(const LevelBGO &sourceBgo, QGraphicsItem *item);
    void historyUndoSettingsZOffsetBgo(const LevelBGO &sourceBgo, QGraphicsItem *item);
    void historyRedoSettingsZOffsetBgo(const LevelBGO &sourceBgo, QGraphicsItem *item);
    void historyUndoSettingsFriendlyNPC(const LevelNPC &sourceNPC, QGraphicsItem *item);
    void historyRedoSettingsFriendlyNPC(const LevelNPC &sourceNPC, QGraphicsItem *item);
    void historyUndoSettingsBossNPC(const LevelNPC &sourceNPC, QGraphicsItem *item);
    void historyRedoSettingsBossNPC(const LevelNPC &sourceNPC, QGraphicsItem *item);
    void historyUndoSettingsNoMoveableNPC(const LevelNPC &sourceNPC, QGraphicsItem *item);
    void historyRedoSettingsNoMoveableNPC(const LevelNPC &sourceNPC, QGraphicsItem *item);
    void historyUndoSettingsMessageNPC(const LevelNPC &sourceNPC, QGraphicsItem *item);
    void historyRedoSettingsMessageNPC(const LevelNPC &sourceNPC, QGraphicsItem *item);
    void historyUndoSettingsDirectionNPC(const LevelNPC &sourceNPC, QGraphicsItem *item);
    void historyRedoSettingsDirectionNPC(const LevelNPC &sourceNPC, QGraphicsItem *item);
    void historyUndoSettingsChangeNPCBlocks(const LevelBlock &sourceBlock, QGraphicsItem *item);
    void historyRedoSettingsChangeNPCBlocks(const LevelBlock &sourceBlock, QGraphicsItem *item);
    void historyUndoSettingsChangeNPCNPC(const LevelNPC &sourceNPC, QGraphicsItem *item);
    void historyRedoSettingsChangeNPCNPC(const LevelNPC &sourceNPC, QGraphicsItem *item);
    void historyUndoSettingsTypeWater(const LevelPhysEnv &sourcePhysEnv, QGraphicsItem *item);
    void historyRedoSettingsTypeWater(const LevelPhysEnv &sourcePhysEnv, QGraphicsItem *item);
    void historyUndoSettingsTwoWayDoors(const LevelDoor &sourceDoors, QGraphicsItem *item);
    void historyRedoSettingsTwoWayDoors(const LevelDoor &sourceDoors, QGraphicsItem *item);
    void historyUndoSettingsNoVehiclesDoors(const LevelDoor &sourceDoors, QGraphicsItem *item);
    void historyRedoSettingsNoVehiclesDoors(const LevelDoor &sourceDoors, QGraphicsItem *item);
    void historyUndoSettingsAllowNPCDoors(const LevelDoor &sourceDoors, QGraphicsItem *item);
    void historyRedoSettingsAllowNPCDoors(const LevelDoor &sourceDoors, QGraphicsItem *item);
    void historyUndoSettingsAllowNPCILDoors(const LevelDoor &sourceDoors, QGraphicsItem *item);
    void historyRedoSettingsAllowNPCILDoors(const LevelDoor &sourceDoors, QGraphicsItem *item);
    void historyUndoSettingsLockedDoors(const LevelDoor &sourceDoors, QGraphicsItem *item);
    void historyRedoSettingsLockedDoors(const LevelDoor &sourceDoors, QGraphicsItem *item);
    void historyUndoSettingsBombNeedDoors(const LevelDoor &sourceDoors, QGraphicsItem *item);
    void historyRedoSettingsBombNeedDoors(const LevelDoor &sourceDoors, QGraphicsItem *item);
    void historyUndoSettingsHideStarNumDoors(const LevelDoor &sourceDoors, QGraphicsItem *item);
    void historyRedoSettingsHideStarNumDoors(const LevelDoor &sourceDoors, QGraphicsItem *item);
    void historyUndoSettingsEnCannonDoors(const LevelDoor &sourceDoors, QGraphicsItem *item);
    void historyRedoSettingsEnCannonDoors(const LevelDoor &sourceDoors, QGraphicsItem *item);
    void historyUndoSettingsSpecialStateReqDoors(const LevelDoor &sourceDoors, QGraphicsItem *item);
    void historyRedoSettingsSpecialStateReqDoors(const LevelDoor &sourceDoors, QGraphicsItem *item);
    void historyUndoSettingsStoodReqDoors(const LevelDoor &sourceDoors, QGraphicsItem *item);
    void historyRedoSettingsStoodReqDoors(const LevelDoor &sourceDoors, QGraphicsItem *item);
    void historyUndoSettingsHideLvlEntSceneDoors(const LevelDoor &sourceDoors, QGraphicsItem *item);
    void historyRedoSettingsHideLvlEntSceneDoors(const LevelDoor &sourceDoors, QGraphicsItem *item);
    void historyUndoSettingsActivateGeneratorNPC(const LevelNPC &sourceNPC, QGraphicsItem *item);
    void historyRedoSettingsActivateGeneratorNPC(const LevelNPC &sourceNPC, QGraphicsItem *item);
    void historyUndoSettingsTypeGeneratorNPC(const LevelNPC &sourceNPC, QGraphicsItem *item);
    void historyRedoSettingsTypeGeneratorNPC(const LevelNPC &sourceNPC, QGraphicsItem *item);
    void historyUndoSettingsDirectionGeneratorNPC(const LevelNPC &sourceNPC, QGraphicsItem *item);
    void historyRedoSettingsDirectionGeneratorNPC(const LevelNPC &sourceNPC, QGraphicsItem *item);
    void historyUndoSettingsTimeGeneratorNPC(const LevelNPC &sourceNPC, QGraphicsItem *item);
    void historyRedoSettingsTimeGeneratorNPC(const LevelNPC &sourceNPC, QGraphicsItem *item);
    void historyUndoSettingsAttachLayerNPC(const LevelNPC &sourceNPC, QGraphicsItem *item);
    void historyRedoSettingsAttachLayerNPC(const LevelNPC &sourceNPC, QGraphicsItem *item);
    void historyUndoSettingsDestroyedEventBlocks(const LevelBlock &sourceBlock, QGraphicsItem *item);
    void historyRedoSettingsDestroyedEventBlocks(const LevelBlock &sourceBlock, QGraphicsItem *item);
    void historyUndoSettingsHitedEventBlocks(const LevelBlock &sourceBlock, QGraphicsItem *item);
    void historyRedoSettingsHitedEventBlocks(const LevelBlock &sourceBlock, QGraphicsItem *item);
    void historyUndoSettingsLayerEmptyEventBlocks(const LevelBlock &sourceBlock, QGraphicsItem *item);
    void historyRedoSettingsLayerEmptyEventBlocks(const LevelBlock &sourceBlock, QGraphicsItem *item);
    void historyUndoSettingsLayerEmptyEventNPC(const LevelNPC &sourceNPC, QGraphicsItem *item);
    void historyRedoSettingsLayerEmptyEventNPC(const LevelNPC &sourceNPC, QGraphicsItem *item);
    void historyUndoSettingsActivateEventNPC(const LevelNPC &sourceNPC, QGraphicsItem *item);
    void historyRedoSettingsActivateEventNPC(const LevelNPC &sourceNPC, QGraphicsItem *item);
    void historyUndoSettingsDeathEventNPC(const LevelNPC &sourceNPC, QGraphicsItem *item);
    void historyRedoSettingsDeathEventNPC(const LevelNPC &sourceNPC, QGraphicsItem *item);
    void historyUndoSettingsTalkEventNPC(const LevelNPC &sourceNPC, QGraphicsItem *item);
    void historyRedoSettingsTalkEventNPC(const LevelNPC &sourceNPC, QGraphicsItem *item);
    void historyUndoSettingsSpecialDataNPC(const LevelNPC &sourceNPC, QGraphicsItem *item);
    void historyRedoSettingsSpecialDataNPC(const LevelNPC &sourceNPC, QGraphicsItem *item);
    void historyUndoSettingsSortingBGO(const LevelBGO &sourceBGO, QGraphicsItem *item);
    void historyRedoSettingsSortingBGO(const LevelBGO &sourceBGO, QGraphicsItem *item);

    void historyUndoSettingCustom(const LevelBlock &sourceItem, QGraphicsItem *item);
    void historyRedoSettingCustom(const LevelBlock &sourceItem, QGraphicsItem *item);
    void historyUndoSettingCustom(const LevelBGO &sourceItem, QGraphicsItem *item);
    void historyRedoSettingCustom(const LevelBGO &sourceItem, QGraphicsItem *item);
    void historyUndoSettingCustom(const LevelNPC &sourceItem, QGraphicsItem *item);
    void historyRedoSettingCustom(const LevelNPC &sourceItem, QGraphicsItem *item);
    void historyUndoSettingCustom(const LevelDoor &sourceItem, QGraphicsItem *item);
    void historyRedoSettingCustom(const LevelDoor &sourceItem, QGraphicsItem *item);
    void historyUndoSettingCustom(const LevelPhysEnv &sourceItem, QGraphicsItem *item);
    void historyRedoSettingCustom(const LevelPhysEnv &sourceItem, QGraphicsItem *item);

    void historyUndoSettingCustom(const WorldTerrainTile &sourceItem, QGraphicsItem *item);
    void historyRedoSettingCustom(const WorldTerrainTile &sourceItem, QGraphicsItem *item);

    void historyUndoSettingCustom(const WorldScenery &sourceItem, QGraphicsItem *item);
    void historyRedoSettingCustom(const WorldScenery &sourceItem, QGraphicsItem *item);

    void historyUndoSettingCustom(const WorldPathTile &sourceItem, QGraphicsItem *item);
    void historyRedoSettingCustom(const WorldPathTile &sourceItem, QGraphicsItem *item);

    void historyUndoSettingCustom(const WorldLevelTile &sourceItem, QGraphicsItem *item);
    void historyRedoSettingCustom(const WorldLevelTile &sourceItem, QGraphicsItem *item);

    void historyUndoSettingCustom(const WorldMusicBox &sourceItem, QGraphicsItem *item);
    void historyRedoSettingCustom(const WorldMusicBox &sourceItem, QGraphicsItem *item);


private:
    HistorySettings::WorldSettingSubType m_modWorldSetting;
    HistorySettings::LevelSettingSubType m_modLevelSetting;
    LevelData m_modLevelData;
    WorldData m_modWorldData;
    QVariant  m_modData;
    HistoryElementCustomSetting *m_customSetting;
};

#endif // HISTORYELEMENTITEMSETTING_H
