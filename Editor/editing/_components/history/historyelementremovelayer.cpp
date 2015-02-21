#include "historyelementremovelayer.h"
#include <editing/_scenes/level/lvl_scene.h>

HistoryElementRemoveLayer::HistoryElementRemoveLayer(LevelData modData, QObject *parent) :
    QObject(parent),
    m_modData(modData)
{}

HistoryElementRemoveLayer::~HistoryElementRemoveLayer()
{}

QString HistoryElementRemoveLayer::getHistoryName()
{
    return tr("Remove Layer");
}

void HistoryElementRemoveLayer::undo()
{
    if(!m_scene)
        return;

    LvlScene* lvlScene;
    if(!(lvlScene = qobject_cast<LvlScene*>(m_scene)))
        return;


}

void HistoryElementRemoveLayer::redo()
{
    if(!m_scene)
        return;

    LvlScene* lvlScene;
    if(!(lvlScene = qobject_cast<LvlScene*>(m_scene)))
        return;


}
