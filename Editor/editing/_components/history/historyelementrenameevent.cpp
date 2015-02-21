#include "historyelementrenameevent.h"
#include <editing/_scenes/level/lvl_scene.h>


HistoryElementRenameEvent::HistoryElementRenameEvent(const int array_id, const QString &oldName, const QString &newName, QObject *parent) :
    QObject(parent),
    m_array_id(array_id),
    m_oldName(oldName),
    m_newName(newName)
{}

HistoryElementRenameEvent::~HistoryElementRenameEvent()
{}

QString HistoryElementRenameEvent::getHistoryName()
{
    return tr("Rename Event");
}

void HistoryElementRenameEvent::undo()
{
    if(!m_scene)
        return;

    LvlScene* lvlScene;
    if(!(lvlScene = qobject_cast<LvlScene*>(m_scene)))
        return;


}

void HistoryElementRenameEvent::redo()
{
    if(!m_scene)
        return;

    LvlScene* lvlScene;
    if(!(lvlScene = qobject_cast<LvlScene*>(m_scene)))
        return;


}
