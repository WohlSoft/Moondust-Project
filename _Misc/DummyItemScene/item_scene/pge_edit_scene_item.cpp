
#include "pge_edit_scene.h"
#include "pge_edit_scene_item.h"

PGE_EditSceneItem::PGE_EditSceneItem(PGE_EditScene* parent) :
    m_scene(parent),
    m_selected(false)
{}

PGE_EditSceneItem::PGE_EditSceneItem(const PGE_EditSceneItem &it) :
    m_scene(it.m_scene),
    m_selected(it.m_selected),
    m_posRect(it.m_posRect)
{}

void PGE_EditSceneItem::setSelected(bool selected)
{
    m_scene->setItemSelected(*this, selected);
}

bool PGE_EditSceneItem::selected()
{
    return m_selected;
}

bool PGE_EditSceneItem::isTouches(int x, int y)
{
    if(m_posRect.left() > x)
        return false;
    if((m_posRect.right()+1) < x)
        return false;
    if(m_posRect.top() > y)
        return false;
    if((m_posRect.bottom()+1) < y)
        return false;
    return true;
}

bool PGE_EditSceneItem::isTouches(QRect &rect)
{
    if(m_posRect.left() > (rect.right()+1))
        return false;
    if((m_posRect.right()+1) < rect.left())
        return false;
    if(m_posRect.top() > (rect.bottom()+1))
        return false;
    if((m_posRect.bottom()+1) < rect.top())
        return false;
    return true;
}

