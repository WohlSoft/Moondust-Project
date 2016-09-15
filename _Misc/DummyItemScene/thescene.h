#ifndef THESCENE_H
#define THESCENE_H

#include <QWidget>
#include <QList>
#include <QRect>
#include <QMouseEvent>
#include <QPaintEvent>

class TheScene;

class Item
{
    friend class TheScene;
    TheScene* m_scene;
    bool m_selected;
public:
    explicit Item(TheScene* parent) :
        m_scene(parent),
        m_selected(false)
    {}

    Item(const Item&it) :
        m_scene(it.m_scene),
        m_selected(it.m_selected),
        m_posRect(it.m_posRect)
    {}

    void setSelected(bool selected)
    {
        m_selected = selected;
    }

    bool selected()
    {
        return m_selected;
    }

    QRect m_posRect;

    bool isTouches(int x, int y)
    {
        if(m_posRect.left() > x)
            return false;
        if(m_posRect.right() < x)
            return false;
        if(m_posRect.top() > y)
            return false;
        if(m_posRect.bottom() < y)
            return false;
        return true;
    }
};

class TheScene : public QWidget
{
    Q_OBJECT
public:
    explicit TheScene(QWidget *parent = 0);

    void addRect(int x, int y)
    {
        Item item(this);
        item.m_posRect.setRect(x, y, 32, 32);
        m_items.append(item);
    }

    void clearSelection();

    QList<Item>  m_items;
    QList<Item*> m_selectedItems;
    QPoint      m_mouseOld;

protected:
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void paintEvent(QPaintEvent *event);
};

#endif // THESCENE_H
