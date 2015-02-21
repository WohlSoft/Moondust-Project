#include "historyelementresizewater.h"
#include <editing/_scenes/level/lvl_scene.h>


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


}

void HistoryElementResizeWater::redo()
{
    if(!m_scene)
        return;

    LvlScene* lvlScene;
    if(!(lvlScene = qobject_cast<LvlScene*>(m_scene)))
        return;


}
