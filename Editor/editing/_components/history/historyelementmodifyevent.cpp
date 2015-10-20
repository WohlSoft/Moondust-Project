#include "historyelementmodifyevent.h"

#include <common_features/mainwinconnect.h>
#include <main_window/dock/lvl_events_box.h>

HistoryElementModifyEvent::HistoryElementModifyEvent(const LevelSMBX64Event &event, bool didRemove, QObject *parent) :
    QObject(parent),
    m_event(event),
    m_didRemove(didRemove)
{}

HistoryElementModifyEvent::~HistoryElementModifyEvent()
{}

QString HistoryElementModifyEvent::getHistoryName()
{
    if(m_didRemove)
        return tr("Remove Event");
    else
        return tr("Add Event");
}

void HistoryElementModifyEvent::undo()
{
    if(m_didRemove)
        doEventPlace();
    else
        doEventRemove();
}

void HistoryElementModifyEvent::redo()
{
    if(m_didRemove)
        doEventRemove();
    else
        doEventPlace();
}

void HistoryElementModifyEvent::doEventRemove()
{
    if(!m_scene)
        return;

    LvlScene* lvlScene;
    if(!(lvlScene = qobject_cast<LvlScene*>(m_scene)))
        return;

    LevelSMBX64Event rmEvents = m_event;

    MainWinConnect::pMainWin->dock_LvlEvents->setEventToolsLocked(true);
    for (int i = 0; i < lvlScene->LvlData->events.size(); i++) {
        if(lvlScene->LvlData->events[i].array_id == (unsigned int)rmEvents.array_id){
            for(int j = 0; j < MainWinConnect::pMainWin->dock_LvlEvents->getEventList()->count(); j++){
                if(MainWinConnect::pMainWin->dock_LvlEvents->getEventList()->item(j)->data(ITEM_BLOCK_IS_SIZABLE).toInt() == (int)rmEvents.array_id){
                    delete MainWinConnect::pMainWin->dock_LvlEvents->getEventList()->item(j);
                }
            }
            MainWinConnect::pMainWin->dock_LvlEvents->ModifyEvent(lvlScene->LvlData->events[i].name, "");
            lvlScene->LvlData->events.removeAt(i);
        }
    }

    MainWinConnect::pMainWin->EventListsSync();
    MainWinConnect::pMainWin->dock_LvlEvents->setEventToolsLocked(false);
}

void HistoryElementModifyEvent::doEventPlace()
{
    if(!m_scene)
        return;

    LvlScene* lvlScene;
    if(!(lvlScene = qobject_cast<LvlScene*>(m_scene)))
        return;

    LevelSMBX64Event rmEvents = m_event;

    MainWinConnect::pMainWin->dock_LvlEvents->setEventToolsLocked(true);
    QListWidgetItem * item;
    item = new QListWidgetItem;
    item->setText(rmEvents.name);
    item->setFlags(Qt::ItemIsEditable);
    item->setFlags(item->flags() | Qt::ItemIsEnabled);
    item->setFlags(item->flags() | Qt::ItemIsEditable | Qt::ItemIsDragEnabled | Qt::ItemIsSelectable);
    item->setData(Qt::UserRole, QString::number(rmEvents.array_id) );
    QListWidget* evList = MainWinConnect::pMainWin->dock_LvlEvents->getEventList();
    LevelSMBX64Event NewEvent = rmEvents;


    lvlScene->LvlData->events.push_back(NewEvent);
    evList->addItem(item);

    lvlScene->LvlData->modified = true;

    MainWinConnect::pMainWin->EventListsSync();
    MainWinConnect::pMainWin->dock_LvlEvents->setEventToolsLocked(false);
}
