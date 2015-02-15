#include "historyelementresizeblock.h"
#include <editing/_scenes/level/lvl_scene.h>
#include <editing/_components/history/itemsearcher.h>
#include <editing/_scenes/level/items/item_block.h>

HistoryElementResizeBlock::HistoryElementResizeBlock(const LevelBlock &block, const QRect &oldSize, const QRect &newSize, QObject *parent) :
    QObject(parent),
    m_oldSize(oldSize),
    m_newSize(newSize),
    m_block(block)
{}

HistoryElementResizeBlock::~HistoryElementResizeBlock()
{}

QString HistoryElementResizeBlock::getHistoryName()
{
    return tr("Resize Block");
}

void HistoryElementResizeBlock::undo()
{
    if(!m_scene)
        return;

    LvlScene* lvlScene;
    if(!(lvlScene = qobject_cast<LvlScene*>(m_scene)))
        return;

    ItemSearcher searcher(ItemTypes::LVL_S_Block);
    connect(&searcher, SIGNAL(foundBlock(LevelBlock,QGraphicsItem*)), this, SLOT(historyUndoResizeBlock(LevelBlock,QGraphicsItem*)));

    LevelData data;
    data.blocks << m_block;

    searcher.find(data, m_scene->items());
}

void HistoryElementResizeBlock::redo()
{
    if(!m_scene)
        return;

    LvlScene* lvlScene;
    if(!(lvlScene = qobject_cast<LvlScene*>(m_scene)))
        return;

    ItemSearcher searcher(ItemTypes::LVL_S_Block);
    connect(&searcher, SIGNAL(foundBlock(LevelBlock,QGraphicsItem*)), this, SLOT(historyRedoResizeBlock(LevelBlock,QGraphicsItem*)));

    LevelData data;
    data.blocks << m_block;

    searcher.find(data, m_scene->items());
}

void HistoryElementResizeBlock::historyUndoResizeBlock(const LevelBlock &/*orig*/, QGraphicsItem* item)
{
    ((ItemBlock *)item)->setBlockSize(m_oldSize);
}

void HistoryElementResizeBlock::historyRedoResizeBlock(const LevelBlock &/*orig*/, QGraphicsItem* item)
{
    ((ItemBlock *)item)->setBlockSize(m_newSize);
}
