#include "historyelementreplaceplayerpoint.h"
#include <editing/_scenes/level/lvl_scene.h>
#include <editing/_scenes/level/items/item_playerpoint.h>
#include "itemsearcher.h"


HistoryElementReplacePlayerPoint::HistoryElementReplacePlayerPoint(PlayerPoint plr, QVariant oldPos, QObject *parent) :
    QObject(parent),
    m_plr(plr),
    m_oldPos(oldPos)
{}

HistoryElementReplacePlayerPoint::~HistoryElementReplacePlayerPoint()
{}

QString HistoryElementReplacePlayerPoint::getHistoryName()
{
    return tr("Place Player Point");
}

void HistoryElementReplacePlayerPoint::undo()
{
    if(!m_scene)
        return;

    LvlScene* lvlScene;
    if(!(lvlScene = qobject_cast<LvlScene*>(m_scene)))
        return;

    //revert place
    LevelData placeData;
    placeData.players << m_plr;

    ItemSearcher* searcher = new ItemSearcher(ItemTypes::LVL_S_Player);
    connect(searcher, SIGNAL(foundPlayerPoint(PlayerPoint,QGraphicsItem*)), this, SLOT(historyRemovePlayerPoint(PlayerPoint,QGraphicsItem*)));
    searcher->find(placeData, m_scene->items());
    delete searcher;
}

void HistoryElementReplacePlayerPoint::redo()
{
    if(!m_scene)
        return;

    LvlScene* lvlScene;
    if(!(lvlScene = qobject_cast<LvlScene*>(m_scene)))
        return;

    lvlScene->placePlayerPoint(m_plr);


}


void HistoryElementReplacePlayerPoint::historyRemovePlayerPoint(const PlayerPoint &/*plr*/, QGraphicsItem* item)
{
    if(!m_scene)
        return;

    LvlScene* lvlScene;
    if(!(lvlScene = qobject_cast<LvlScene*>(m_scene)))
        return;

    dynamic_cast<ItemPlayerPoint *>(item)->removeFromArray();
    lvlScene->removeItem(item);
    delete item;

    bool wasPlaced = false;
    PlayerPoint oPoint;
    if(!m_oldPos.isNull())
    {
        if(m_oldPos.type() == QVariant::List)
        {
            QList<QVariant> mData = m_oldPos.toList();
            if(mData.size() == 5)
            {
                oPoint.id = (unsigned int)mData[0].toInt();
                oPoint.x = (long)mData[1].toLongLong();
                oPoint.y = (long)mData[2].toLongLong();
                oPoint.w = (long)mData[3].toLongLong();
                oPoint.h = (long)mData[4].toLongLong();
                if(oPoint.id>0 && !(oPoint.x == 0 && oPoint.y == 0)) wasPlaced = true;
            }
        }
    }

    if(wasPlaced) lvlScene->placePlayerPoint(oPoint);
}
