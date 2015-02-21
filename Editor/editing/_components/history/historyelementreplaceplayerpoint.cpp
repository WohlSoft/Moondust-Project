#include "historyelementreplaceplayerpoint.h"
#include <editing/_scenes/level/lvl_scene.h>

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


}

void HistoryElementReplacePlayerPoint::redo()
{
    if(!m_scene)
        return;

    LvlScene* lvlScene;
    if(!(lvlScene = qobject_cast<LvlScene*>(m_scene)))
        return;



}
