#include "historyelementrenamelayer.h"
#include <editing/_scenes/level/lvl_scene.h>

HistoryElementRenameLayer::HistoryElementRenameLayer(const int array_id, const QString oldName, const QString newName, QObject *parent) :
    QObject(parent),
    m_array_id(array_id),
    m_oldName(oldName),
    m_newName(newName)
{}

HistoryElementRenameLayer::~HistoryElementRenameLayer()
{}

QString HistoryElementRenameLayer::getHistoryName()
{
    return tr("Rename Layer");
}

void HistoryElementRenameLayer::undo()
{
    if(!m_scene)
        return;

    LvlScene* lvlScene;
    if(!(lvlScene = qobject_cast<LvlScene*>(m_scene)))
        return;


}

void HistoryElementRenameLayer::redo()
{
    if(!m_scene)
        return;

    LvlScene* lvlScene;
    if(!(lvlScene = qobject_cast<LvlScene*>(m_scene)))
        return;



}
