#include "historyelementsettingssection.h"
#include <editing/_scenes/level/lvl_scene.h>

HistoryElementSettingsSection::HistoryElementSettingsSection(int sectionID, HistorySettings::LevelSettingSubType subtype, QVariant extraData, QObject *parent) :
    QObject(parent),
    m_sectionID(sectionID),
    m_subtype(subtype),
    m_extraData(extraData)
{}

HistoryElementSettingsSection::~HistoryElementSettingsSection()
{

}

QString HistoryElementSettingsSection::getHistoryName()
{
    return HistorySettings::settingToString(m_subtype);
}

void HistoryElementSettingsSection::undo()
{
    if(!m_scene)
        return;

    LvlScene* lvlScene;
    if(!(lvlScene = qobject_cast<LvlScene*>(m_scene)))
        return;


}

void HistoryElementSettingsSection::redo()
{
    if(!m_scene)
        return;

    LvlScene* lvlScene;
    if(!(lvlScene = qobject_cast<LvlScene*>(m_scene)))
        return;


}
