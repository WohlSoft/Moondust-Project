#ifndef HISTORYELEMENTMAINSETTING_H
#define HISTORYELEMENTMAINSETTING_H

#include "ihistoryelement.h"
#include <PGE_File_Formats/file_formats.h>
#include <defines.h>

class HistoryElementMainSetting : public QObject, public IHistoryElement
{
    Q_OBJECT

    Q_INTERFACES(IHistoryElement)
public:

    explicit HistoryElementMainSetting(HistorySettings::WorldSettingSubType wldSubType, QObject *parent = 0);
    explicit HistoryElementMainSetting(HistorySettings::LevelSettingSubType lvlSubType, QObject *parent = 0);
    explicit HistoryElementMainSetting(HistorySettings::WorldSettingSubType wldSubType, QVariant extraData, QObject *parent = 0);
    explicit HistoryElementMainSetting(HistorySettings::LevelSettingSubType lvlSubType, QVariant extraData, QObject *parent = 0);
    virtual ~HistoryElementMainSetting();
    virtual QString getHistoryName();
    virtual void undo();
    virtual void redo();


    void processWorldUndo();
    void processLevelUndo();

    void processWorldRedo();
    void processLevelRedo();


signals:

public slots:

private:
    HistorySettings::WorldSettingSubType m_modWorldSetting;
    HistorySettings::LevelSettingSubType m_modLevelSetting;
    QVariant m_modData;

};

#endif // HISTORYELEMENTMAINSETTING_H
