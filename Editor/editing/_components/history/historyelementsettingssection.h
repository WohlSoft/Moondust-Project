#ifndef HISTORYELEMENTSETTINGSSECTION_H
#define HISTORYELEMENTSETTINGSSECTION_H

#include "ihistoryelement.h"
#include <PGE_File_Formats/file_formats.h>
#include <defines.h>


class HistoryElementSettingsSection : public QObject, public IHistoryElement
{
    Q_OBJECT
    Q_INTERFACES(IHistoryElement)

public:
    explicit HistoryElementSettingsSection(int sectionID, HistorySettings::LevelSettingSubType subtype, QVariant extraData, QObject *parent = 0);
    virtual ~HistoryElementSettingsSection();
    virtual QString getHistoryName();
    virtual void undo();
    virtual void redo();

private:
    int m_sectionID;
    HistorySettings::LevelSettingSubType m_subtype;
    QVariant m_extraData;
};

#endif // HISTORYELEMENTSETTINGSSECTION_H
