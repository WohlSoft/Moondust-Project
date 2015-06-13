#include "historyelementresizewater.h"
#include <editing/_scenes/level/lvl_scene.h>
#include "itemsearcher.h"
#include <editing/_scenes/level/items/item_water.h>

HistoryElementResizeWater::HistoryElementResizeWater(LevelPhysEnv wt, const QRect &oldSize, const QRect &newSize, QObject *parent) :
    QObject(parent),
    m_physEnv(wt),
    m_oldSize(oldSize),
    m_newSize(newSize)
{}

HistoryElementResizeWater::~HistoryElementResizeWater()
{}

QString HistoryElementResizeWater::getHistoryName()
{
    return tr("Resize Water");
}

void HistoryElementResizeWater::undo()
{
    if(!m_scene)
        return;

    LvlScene* lvlScene;
    if(!(lvlScene = qobject_cast<LvlScene*>(m_scene)))
        return;

    ItemSearcher searcher(ItemTypes::LVL_S_PhysEnv);
    connect(&searcher, SIGNAL(foundPhysEnv(LevelPhysEnv, QGraphicsItem*)), this, SLOT(historyUndoResizeWater(LevelPhysEnv, QGraphicsItem*)));

    LevelData data;
    data.physez << m_physEnv;

    searcher.find(data, m_scene->items());
}

void HistoryElementResizeWater::redo()
{
    if(!m_scene)
        return;

    LvlScene* lvlScene;
    if(!(lvlScene = qobject_cast<LvlScene*>(m_scene)))
        return;

    ItemSearcher searcher(ItemTypes::LVL_S_PhysEnv);
    connect(&searcher, SIGNAL(foundPhysEnv(LevelPhysEnv, QGraphicsItem*)), this, SLOT(historyRedoResizeWater(LevelPhysEnv, QGraphicsItem*)));

    LevelData data;
    data.physez << m_physEnv;

    searcher.find(data, m_scene->items());
}

void HistoryElementResizeWater::historyUndoResizeWater(const LevelPhysEnv &/*orig*/, QGraphicsItem *item)
{
    ((ItemWater *)item)->setRectSize(m_oldSize);
}

void HistoryElementResizeWater::historyRedoResizeWater(const LevelPhysEnv &/*orig*/, QGraphicsItem *item)
{
    ((ItemWater *)item)->setRectSize(m_newSize);
}
