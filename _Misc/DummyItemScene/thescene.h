#ifndef THESCENE_H
#define THESCENE_H

#include <QWidget>
#include <QList>
#include <QRect>
#include <QSet>
#include <QTimer>

class Item;
class TheScene : public QWidget
{
    Q_OBJECT
public:
    explicit TheScene(QWidget *parent = 0);

    void addRect(int x, int y);

    void clearSelection();
    void moveSelection(int deltaX, int deltaY);

    void select(Item& item);
    void deselect(Item& item);
    void toggleselect(Item& item);
    void setItemSelected(Item& item, bool selected);


    QList<Item>  m_items;
    typedef QSet<Item*> SelectionMap;
    SelectionMap m_selectedItems;
    QPoint       m_mouseOld;
    QPoint       m_mouseBegin;
    QPoint       m_mouseEnd;
    bool         m_ignoreMove;
    bool         m_ignoreRelease;
    bool         m_rectSelect;
    QPoint       m_cameraPos;
    double       m_zoom;

    QPoint       mapToWorld(const QPoint &mousePos);
    QRect applyZoom(const QRect &r);

    struct Mover
    {
        Mover(): speedX(0), speedY(0) {}
        QTimer  timer;
        int     speedX;
        int     speedY;
    } m_mover;
    void moveCamera();
    void moveCamera(int deltaX, int deltaY);

protected:
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void wheelEvent(QWheelEvent *event);
    void paintEvent(QPaintEvent *event);
    void keyPressEvent(QKeyEvent *event);
    void keyReleaseEvent(QKeyEvent *event);
};


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
        m_scene->setItemSelected(*this, selected);
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
        if((m_posRect.right()+1) < x)
            return false;
        if(m_posRect.top() > y)
            return false;
        if((m_posRect.bottom()+1) < y)
            return false;
        return true;
    }

    bool isTouches(QRect &rect)
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
};

#endif // THESCENE_H
