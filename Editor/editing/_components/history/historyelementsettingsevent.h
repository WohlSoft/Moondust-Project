#ifndef HISTORYELEMENTSETTINGSEVENT_H
#define HISTORYELEMENTSETTINGSEVENT_H

#include <QObject>

#include "ihistoryelement.h"
#include <PGE_File_Formats/file_formats.h>

class HistoryElementSettingsEvent : public QObject, public IHistoryElement
{
    Q_OBJECT
    Q_INTERFACES(IHistoryElement)

public:
    explicit HistoryElementSettingsEvent(int array_id, HistorySettings::LevelSettingSubType subtype, QVariant extraData, QObject* parent = 0);
    virtual ~HistoryElementSettingsEvent();
    virtual QString getHistoryName();
    virtual void undo();
    virtual void redo();

private:
    int m_array_id;
    HistorySettings::LevelSettingSubType m_subtype;
    QVariant m_modData;
};

#endif // HISTORYELEMENTSETTINGSEVENT_H
