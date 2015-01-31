#ifndef HISTORYELEMENTITEMSETTING_H
#define HISTORYELEMENTITEMSETTING_H

#include "ihistoryelement.h"
#include <file_formats/file_formats.h>
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
    void processWorldRedo();


signals:

public slots:
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


private:
    HistorySettings::WorldSettingSubType m_modWorldSetting;
    HistorySettings::LevelSettingSubType m_modLevelSetting;
    LevelData m_modLevelData;
    WorldData m_modWorldData;
    QVariant m_modData;

};

#endif // HISTORYELEMENTITEMSETTING_H
