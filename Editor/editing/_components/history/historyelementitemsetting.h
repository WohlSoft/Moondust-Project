#ifndef HISTORYELEMENTITEMSETTING_H
#define HISTORYELEMENTITEMSETTING_H

#include "ihistoryelement.h"
#include <PGE_File_Formats/file_formats.h>
#include <defines.h>

class HistoryElementItemSetting : public QObject, public IHistoryElement
{
    Q_INTERFACES(IHistoryElement)
    Q_OBJECT

public:
    explicit HistoryElementItemSetting(const WorldData &data, HistorySettings::WorldSettingSubType wldSubType, QVariant extraData, QObject *parent = 0);
    explicit HistoryElementItemSetting(const LevelData &data, HistorySettings::LevelSettingSubType lvlSubType, QVariant extraData, QObject *parent = 0);
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
    void historyUndoSettingPathBackgroundLevel(const WorldLevels &sourceLevel, QGraphicsItem* item);
    void historyRedoSettingPathBackgroundLevel(const WorldLevels &sourceLevel, QGraphicsItem* item);
    void historyUndoSettingBigPathBackgroundLevel(const WorldLevels &sourceLevel, QGraphicsItem* item);
    void historyRedoSettingBigPathBackgroundLevel(const WorldLevels &sourceLevel, QGraphicsItem* item);
    void historyUndoSettingAlwaysVisibleLevel(const WorldLevels &sourceLevel, QGraphicsItem* item);
    void historyRedoSettingAlwaysVisibleLevel(const WorldLevels &sourceLevel, QGraphicsItem* item);
    void historyUndoSettingGameStartPointLevel(const WorldLevels &sourceLevel, QGraphicsItem* item);
    void historyRedoSettingGameStartPointLevel(const WorldLevels &sourceLevel, QGraphicsItem* item);
    void historyUndoSettingLevelfileLevel(const WorldLevels &sourceLevel, QGraphicsItem* item);
    void historyRedoSettingLevelfileLevel(const WorldLevels &sourceLevel, QGraphicsItem* item);
    void historyUndoSettingLeveltitleLevel(const WorldLevels &sourceLevel, QGraphicsItem* item);
    void historyRedoSettingLeveltitleLevel(const WorldLevels &sourceLevel, QGraphicsItem* item);
    void historyUndoSettingDoorIDLevel(const WorldLevels &sourceLevel, QGraphicsItem* item);
    void historyRedoSettingDoorIDLevel(const WorldLevels &sourceLevel, QGraphicsItem* item);
    void historyUndoSettingPathByTopLevel(const WorldLevels &sourceLevel, QGraphicsItem* item);
    void historyRedoSettingPathByTopLevel(const WorldLevels &sourceLevel, QGraphicsItem* item);
    void historyUndoSettingPathByRightLevel(const WorldLevels &sourceLevel, QGraphicsItem* item);
    void historyRedoSettingPathByRightLevel(const WorldLevels &sourceLevel, QGraphicsItem* item);
    void historyUndoSettingPathByBottomLevel(const WorldLevels &sourceLevel, QGraphicsItem* item);
    void historyRedoSettingPathByBottomLevel(const WorldLevels &sourceLevel, QGraphicsItem* item);
    void historyUndoSettingPathByLeftLevel(const WorldLevels &sourceLevel, QGraphicsItem* item);
    void historyRedoSettingPathByLeftLevel(const WorldLevels &sourceLevel, QGraphicsItem* item);
    void historyUndoSettingGotoXLevel(const WorldLevels &sourceLevel, QGraphicsItem* item);
    void historyRedoSettingGotoXLevel(const WorldLevels &sourceLevel, QGraphicsItem* item);
    void historyUndoSettingGotoYLevel(const WorldLevels &sourceLevel, QGraphicsItem* item);
    void historyRedoSettingGotoYLevel(const WorldLevels &sourceLevel, QGraphicsItem* item);


    void historyUndoSettingsInvisibleBlock(const LevelBlock &sourceBlock, QGraphicsItem *item);
    void historyRedoSettingsInvisibleBlock(const LevelBlock &sourceBlock, QGraphicsItem *item);
    void historyUndoSettingsSlipperyBlock(const LevelBlock &sourceBlock, QGraphicsItem *item);
    void historyRedoSettingsSlipperyBlock(const LevelBlock &sourceBlock, QGraphicsItem *item);
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
    void historyUndoSettingsNoYoshiDoors(const LevelDoor &sourceDoors, QGraphicsItem *item);
    void historyRedoSettingsNoYoshiDoors(const LevelDoor &sourceDoors, QGraphicsItem *item);
    void historyUndoSettingsAllowNPCDoors(const LevelDoor &sourceDoors, QGraphicsItem *item);
    void historyRedoSettingsAllowNPCDoors(const LevelDoor &sourceDoors, QGraphicsItem *item);
    void historyUndoSettingsLockedDoors(const LevelDoor &sourceDoors, QGraphicsItem *item);
    void historyRedoSettingsLockedDoors(const LevelDoor &sourceDoors, QGraphicsItem *item);
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

private:
    HistorySettings::WorldSettingSubType m_modWorldSetting;
    HistorySettings::LevelSettingSubType m_modLevelSetting;
    LevelData m_modLevelData;
    WorldData m_modWorldData;
    QVariant m_modData;

};

#endif // HISTORYELEMENTITEMSETTING_H
