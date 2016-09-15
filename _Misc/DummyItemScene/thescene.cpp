
#include <QPainter>
#include "thescene.h"

TheScene::TheScene(QWidget *parent) :
    QWidget(parent)
{
    this->setFocusPolicy(Qt::StrongFocus);
}

void TheScene::clearSelection()
{
    for(int i=0; i<m_selectedItems.size(); i++)
    {
        m_selectedItems[i]->setSelected(false);
    }
    m_selectedItems.clear();
}

void TheScene::mousePressEvent(QMouseEvent *event)
{
    clearSelection();
    m_mouseOld = event->pos();
    for(int i=0; i<m_items.size(); i++)
    {
        if( m_items[i].isTouches(m_mouseOld.x(), m_mouseOld.y()) )
        {
            m_selectedItems.append(&m_items[i]);
            m_items[i].setSelected( true );
        }
    }
    repaint();
}

void TheScene::mouseMoveEvent(QMouseEvent *event)
{
    QPoint delta = m_mouseOld - event->pos();
    for(int i=0; i<m_selectedItems.size(); i++)
    {
        int x = m_selectedItems[i]->m_posRect.x();
        int y = m_selectedItems[i]->m_posRect.y();
        m_selectedItems[i]->m_posRect.moveTo( x-delta.x(), y-delta.y() );
    }
    m_mouseOld = event->pos();
    repaint();
}

void TheScene::mouseReleaseEvent(QMouseEvent *event)
{

}

void TheScene::paintEvent(QPaintEvent */*event*/)
{
    QPainter p(this);
    p.setBrush(QColor(Qt::white));

    for(int i=0; i<m_items.size(); i++)
    {
        if(m_items[i].selected())
            p.setPen(QColor(Qt::green));
        else
            p.setPen(QColor(Qt::black));
        p.drawRect(m_items[i].m_posRect);
    }
    p.end();
}
