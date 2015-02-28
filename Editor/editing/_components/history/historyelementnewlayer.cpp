#include "historyelementnewlayer.h"
#include <editing/_scenes/level/lvl_scene.h>

HistoryElementNewLayer::HistoryElementNewLayer(const int array_id, const QString &name, QObject *parent) :
    QObject(parent),
    m_array_id(array_id),
    m_name(name)
{}

HistoryElementNewLayer::~HistoryElementNewLayer()
{}

QString HistoryElementNewLayer::getHistoryName()
{
    return tr("New Layer");
}

void HistoryElementNewLayer::undo()
{
    if(!m_scene)
        return;

    LvlScene* lvlScene;
    if(!(lvlScene = qobject_cast<LvlScene*>(m_scene)))
        return;


}

void HistoryElementNewLayer::redo()
{
    if(!m_scene)
        return;

    LvlScene* lvlScene;
    if(!(lvlScene = qobject_cast<LvlScene*>(m_scene)))
        return;


}
