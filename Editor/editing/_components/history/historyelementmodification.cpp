#include "historyelementmodification.h"

#include <editing/_scenes/level/lvl_scene.h>
#include <editing/_scenes/world/wld_scene.h>


HistoryElementModification::HistoryElementModification(const LevelData &oldData, const LevelData &newData, QObject *parent) :
    QObject(parent), m_oldData(oldData), m_newData(newData)
{}

HistoryElementModification::~HistoryElementModification()
{}

QString HistoryElementModification::getHistoryName()
{
    if(m_customHistoryName.isEmpty()){
        return "Simple Modification History";
    }
    return m_customHistoryName;
}

void HistoryElementModification::undo()
{
    LvlScene* lvlScene = 0;
    WldScene* wldScene = 0;
    if((m_scene) && (lvlScene = qobject_cast<LvlScene*>(m_scene))){

    }
    if((m_scene) && (wldScene = qobject_cast<WldScene*>(m_scene))){

    }
}

void HistoryElementModification::redo()
{
    LvlScene* lvlScene = 0;
    WldScene* wldScene = 0;
    if((m_scene) && (lvlScene = qobject_cast<LvlScene*>(m_scene))){

    }
    if((m_scene) && (wldScene = qobject_cast<WldScene*>(m_scene))){

    }
}

LevelData HistoryElementModification::oldData() const
{
    return m_oldData;
}

void HistoryElementModification::setOldData(const LevelData &oldData)
{
    m_oldData = oldData;
}
LevelData HistoryElementModification::newData() const
{
    return m_newData;
}

void HistoryElementModification::setNewData(const LevelData &newData)
{
    m_newData = newData;
}
QString HistoryElementModification::customHistoryName() const
{
    return m_customHistoryName;
}

void HistoryElementModification::setCustomHistoryName(const QString &customHistoryName)
{
    m_customHistoryName = customHistoryName;
}



