#include "historyelementresizesection.h"
#include <editing/_scenes/level/lvl_scene.h>


HistoryElementResizeSection::HistoryElementResizeSection(int sectionID, const QRect &oldSize, const QRect &newSize, QObject *parent) :
    QObject(parent),
    m_oldSize(oldSize),
    m_newSize(newSize),
    m_sectionID(sectionID)
{}

HistoryElementResizeSection::~HistoryElementResizeSection()
{}

QString HistoryElementResizeSection::getHistoryName()
{
    return tr("Resize Section");
}

void HistoryElementResizeSection::undo()
{
    if(!m_scene)
        return;

    LvlScene* lvlScene;
    if(!(lvlScene = qobject_cast<LvlScene*>(m_scene)))
        return;

    lvlScene->LvlData->sections[m_sectionID].size_left = m_oldSize.left();
    lvlScene->LvlData->sections[m_sectionID].size_right = m_oldSize.right();
    lvlScene->LvlData->sections[m_sectionID].size_top = m_oldSize.top();
    lvlScene->LvlData->sections[m_sectionID].size_bottom = m_oldSize.bottom();

    lvlScene->ChangeSectionBG(lvlScene->LvlData->sections[m_sectionID].background, m_sectionID);
    if(m_sectionID == lvlScene->LvlData->CurSection){
        lvlScene->drawSpace();
    }
}

void HistoryElementResizeSection::redo()
{
    if(!m_scene)
        return;

    LvlScene* lvlScene;
    if(!(lvlScene = qobject_cast<LvlScene*>(m_scene)))
        return;

    lvlScene->LvlData->sections[m_sectionID].size_left = m_newSize.left();
    lvlScene->LvlData->sections[m_sectionID].size_right = m_newSize.right();
    lvlScene->LvlData->sections[m_sectionID].size_top = m_newSize.top();
    lvlScene->LvlData->sections[m_sectionID].size_bottom = m_newSize.bottom();

    lvlScene->ChangeSectionBG(lvlScene->LvlData->sections[m_sectionID].background, m_sectionID);
    if(m_sectionID == lvlScene->LvlData->CurSection){
        lvlScene->drawSpace();
    }
}
