#ifndef HISTORYELEMENTSETTINGSWARP_H
#define HISTORYELEMENTSETTINGSWARP_H

#include "ihistoryelement.h"
#include <PGE_File_Formats/file_formats.h>
#include <defines.h>

class HistoryElementSettingsWarp : public QObject, public IHistoryElement
{
    Q_OBJECT
    Q_INTERFACES(IHistoryElement)

public:
    explicit HistoryElementSettingsWarp(int array_id, HistorySettings::LevelSettingSubType subtype, QVariant extraData, QObject *parent = 0);
    virtual ~HistoryElementSettingsWarp();
    virtual QString getHistoryName();
    virtual void undo();
    virtual void redo();

private:
    int m_array_id;
    HistorySettings::LevelSettingSubType m_subtype;
    QVariant m_modData;
};

#endif // HISTORYELEMENTSETTINGSWARP_H
