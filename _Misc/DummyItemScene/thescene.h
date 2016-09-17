#ifndef THESCENE_H
#define THESCENE_H

#include <QWidget>
#include <QList>
#include <QRect>
#include <QHash>

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

    bool isTouches(QRect &rect)
    {
        if(m_posRect.left() > rect.right())
            return false;
        if(m_posRect.right() < rect.left())
            return false;
        if(m_posRect.top() > rect.bottom())
            return false;
        if(m_posRect.bottom() < rect.top())
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
    void select(Item& item);
    void deselect(Item& item);
    void toggleselect(Item& item);

    void moveSelection(int deltaX, int deltaY);

    QList<Item>  m_items;
    typedef QHash<intptr_t, Item*> SelectionMap;
    SelectionMap m_selectedItems;
    QPoint       m_mouseOld;
    QPoint       m_mouseBegin;
    QPoint       m_mouseEnd;
    bool         m_ignoreMove;
    bool         m_ignoreRelease;
    bool         m_rectSelect;
protected:
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void paintEvent(QPaintEvent *event);
    void keyPressEvent(QKeyEvent *event);
    void keyReleaseEvent(QKeyEvent *event);
};

#endif // THESCENE_H
