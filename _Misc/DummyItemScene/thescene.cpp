
#include <QMenu>
#include <QAction>
#include <QPainter>
#include <QMouseEvent>
#include <QPaintEvent>
#include <QKeyEvent>
#include "thescene.h"

TheScene::TheScene(QWidget *parent) :
    QWidget(parent),
    m_ignoreMove(false),
    m_ignoreRelease(false),
    m_rectSelect(false)
{
    this->setFocusPolicy(Qt::StrongFocus);
}

void TheScene::clearSelection()
{
    for(SelectionMap::iterator it = m_selectedItems.begin(); it != m_selectedItems.end(); it++)
    {
        it.value()->setSelected(false);
    }
    m_selectedItems.clear();
}

void TheScene::select(Item &item)
{
    item.setSelected( true );
    m_selectedItems[intptr_t(&item)] = &item;
}

void TheScene::deselect(Item &item)
{
    item.setSelected( false );
    m_selectedItems.remove(intptr_t(&item));
}

void TheScene::toggleselect(Item &item)
{
    bool to = !item.selected();
    item.setSelected(to);
    if(to)
        m_selectedItems[intptr_t(&item)] = &item;
    else
        m_selectedItems.remove(intptr_t(&item));
}

void TheScene::moveSelection(int deltaX, int deltaY)
{
    for(SelectionMap::iterator it = m_selectedItems.begin(); it != m_selectedItems.end(); it++)
    {
        QRect&r = it.value()->m_posRect;
        int x= r.x();
        int y= r.y();
        r.moveTo(x+deltaX, y+deltaY);
    }
    repaint();
}

void TheScene::mousePressEvent(QMouseEvent *event)
{
    if(event->button() != Qt::LeftButton)
        return;

    m_mouseBegin = event->pos();
    m_mouseOld   = event->pos();

    bool isShift = (event->modifiers()&Qt::ShiftModifier) != 0;
    bool isCtrl = (event->modifiers()&Qt::ControlModifier) != 0;

    if( !isShift )
    {
        bool catched = false;
        for(int i=0; i<m_items.size(); i++)
        {
            if( m_items[i].isTouches(m_mouseOld.x(), m_mouseOld.y()) )
            {
                catched = true;
                if(isCtrl)
                {
                    toggleselect(m_items[i]);
                }
                else
                if(!m_items[i].selected())
                {
                    clearSelection();
                    select(m_items[i]);
                }
                break;
            }
        }
        if(!catched && !isCtrl)
            clearSelection();
    }

    if( (m_selectedItems.isEmpty() && !isCtrl) || isShift)
    {
        m_rectSelect=true;
    }

    if(isCtrl && !isShift)
    {
        m_ignoreMove = true;
        m_ignoreRelease = true;
    }

    repaint();
}

void TheScene::mouseMoveEvent(QMouseEvent *event)
{
    if((event->buttons()&Qt::LeftButton)==0)
        return;

    if(m_ignoreMove)
        return;

    QPoint delta = m_mouseOld - event->pos();
    if(!m_rectSelect)
    {
        for(SelectionMap::iterator it = m_selectedItems.begin(); it != m_selectedItems.end(); it++)
        {
            Item& item = *it.value();
            int x = item.m_posRect.x();
            int y = item.m_posRect.y();
            item.m_posRect.moveTo( x-delta.x(), y-delta.y() );
        }
    }
    m_mouseOld = event->pos();
    repaint();
}

void TheScene::mouseReleaseEvent(QMouseEvent *event)
{
    bool isShift = (event->modifiers()&Qt::ShiftModifier) != 0;
    bool isCtrl = (event->modifiers()&Qt::ControlModifier) != 0;

    if(event->button()==Qt::RightButton && (event->buttons()==0))
    {
        QMenu test;
        test.addAction("Meow  1");
        test.addAction("Meow  2");
        test.addAction("Meow :3");
        test.exec(mapToGlobal(event->pos()));
        return;
    }

    if(event->button() != Qt::LeftButton)
        return;

    bool skip = m_ignoreRelease;

    m_ignoreMove    = false;
    m_ignoreRelease = false;

    if(skip)
        return;

    m_mouseEnd = event->pos();
    if(m_rectSelect)
    {
        int left   = m_mouseBegin.x() < m_mouseEnd.x() ? m_mouseBegin.x() : m_mouseEnd.x();
        int right  = m_mouseBegin.x() > m_mouseEnd.x() ? m_mouseBegin.x() : m_mouseEnd.x();
        int top    = m_mouseBegin.y() < m_mouseEnd.y() ? m_mouseBegin.y() : m_mouseEnd.y();
        int bottom = m_mouseBegin.y() > m_mouseEnd.y() ? m_mouseBegin.y() : m_mouseEnd.y();
        QRect selZone;
        selZone.setCoords(left, top, right, bottom);
        for(int i=0; i<m_items.size(); i++)
        {
            if( m_items[i].isTouches(selZone) )
            {
                if(isShift && isCtrl)
                    toggleselect(m_items[i]);
                else
                    select(m_items[i]);
            }
        }
        m_rectSelect=false;
        repaint();
    }
}

void TheScene::paintEvent(QPaintEvent */*event*/)
{
    QPainter p(this);
    p.setBrush(QColor(Qt::white));
    p.setOpacity(1.0);

    for(int i=0; i<m_items.size(); i++)
    {
        if(m_items[i].selected())
            p.setPen(QColor(Qt::green));
        else
            p.setPen(QColor(Qt::black));
        p.drawRect(m_items[i].m_posRect);
    }
    if(m_rectSelect)
    {
        p.setBrush(QBrush(Qt::green));
        p.setPen(QPen(Qt::darkGreen));
        p.setOpacity(0.5);
        p.drawRect(QRect(m_mouseBegin, m_mouseOld));
    }
    p.end();
}

void TheScene::keyPressEvent(QKeyEvent *event)
{
    bool isCtrl = (event->modifiers()&Qt::ControlModifier) != 0;
    switch(event->key())
    {
    case Qt::Key_Left:
        if(isCtrl) moveSelection(-1, 0);
        break;
    case Qt::Key_Right:
        if(isCtrl) moveSelection(1, 0);
        break;
    case Qt::Key_Up:
        if(isCtrl) moveSelection(0, -1);
        break;
    case Qt::Key_Down:
        if(isCtrl) moveSelection(0, 1);
        break;
    }
}

void TheScene::keyReleaseEvent(QKeyEvent *event)
{
    switch(event->key())
    {
    case Qt::Key_Escape:
        clearSelection();
        m_rectSelect=false;
        repaint();
        break;
    }
}
