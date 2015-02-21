#include "historyelementmergelayer.h"
#include <editing/_scenes/level/lvl_scene.h>


HistoryElementMergeLayer::HistoryElementMergeLayer(const LevelData &mergedData, const QString &newLayerName, QObject *parent) :
    QObject(parent),
    m_mergedData(mergedData),
    m_newLayerName(newLayerName)
{}

HistoryElementMergeLayer::~HistoryElementMergeLayer()
{}

QString HistoryElementMergeLayer::getHistoryName()
{
    return tr("Merge Layer");
}

void HistoryElementMergeLayer::undo()
{
    if(!m_scene)
        return;

    LvlScene* lvlScene;
    if(!(lvlScene = qobject_cast<LvlScene*>(m_scene)))
        return;


}

void HistoryElementMergeLayer::redo()
{
    if(!m_scene)
        return;

    LvlScene* lvlScene;
    if(!(lvlScene = qobject_cast<LvlScene*>(m_scene)))
        return;


}
