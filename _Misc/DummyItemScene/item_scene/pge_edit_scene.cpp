
#include <QMenu>
#include <QAction>
#include <QPainter>
#include <QMouseEvent>
#include <QWheelEvent>
#include <QPaintEvent>
#include <QKeyEvent>
#include <QMessageBox>
#include <QtConcurrent/QtConcurrentRun>

#include "pge_edit_scene.h"

PGE_EditScene::PGE_EditScene(QWidget *parent) :
    QWidget(parent),
    m_ignoreMove(false),
    m_ignoreRelease(false),
    m_moveInProcess(false),
    m_rectSelect(false),
    m_zoom(1.0),
    m_isBusy(false),
    m_abortThread(false)
{
    setFocusPolicy(Qt::StrongFocus);
    setMouseTracking(true);
    connect(&m_mover.timer,
            &QTimer::timeout,
            this,
            static_cast<void (PGE_EditScene::*)()>(&PGE_EditScene::moveCamera) );
}

PGE_EditScene::~PGE_EditScene()
{
    m_tree.RemoveAll();
    m_items.clear();
}

void PGE_EditScene::addRect(int x, int y)
{
    PGE_EditSceneItem item(this);
    item.m_posRect.setRect(x, y, 32, 32);
    m_items.append(item);
    registerElement(&m_items.last());
}

void PGE_EditScene::clearSelection()
{
    for(PGE_EditSceneItem* item : m_selectedItems)
    {
        item->m_selected = false;
    }
    m_selectedItems.clear();
}

void PGE_EditScene::moveSelection(int deltaX, int deltaY)
{
    for(PGE_EditSceneItem* item : m_selectedItems)
    {
        item->m_posRect.moveBy(deltaX, deltaY);
        unregisterElement(item);
        registerElement(item);
    }
}

void PGE_EditScene::select(PGE_EditSceneItem &item)
{
    item.m_selected = true;
    m_selectedItems.insert(&item);
}

void PGE_EditScene::deselect(PGE_EditSceneItem &item)
{
    item.m_selected = false;
    m_selectedItems.remove(&item);
}

void PGE_EditScene::toggleselect(PGE_EditSceneItem &item)
{
    item.m_selected = !item.m_selected;
    if(item.m_selected)
        m_selectedItems.insert(&item);
    else
        m_selectedItems.remove(&item);
}

void PGE_EditScene::setItemSelected(PGE_EditSceneItem &item, bool selected)
{
    item.m_selected = selected;
    if(item.m_selected)
        m_selectedItems.insert(&item);
    else
        m_selectedItems.remove(&item);
}

void PGE_EditScene::moveStart()
{
    m_moveInProcess = true;
}

void PGE_EditScene::moveEnd(bool /*esc*/)
{
    m_moveInProcess = false;
}

void PGE_EditScene::startInitAsync()
{
    m_isBusy = true;
    QtConcurrent::run<void>(this, &PGE_EditScene::initThread);
}

void PGE_EditScene::initThread()
{
    m_isBusy = true;
    bool offset = false;
    for(int y= -1024; y<32000; y+=32)
        for(int x= -1024; x<32000; x+=32)
        {
            if(m_abortThread) goto threadEnd;
            addRect(x,  y + (offset ? 16 : 0));
            offset = !offset;
        }
threadEnd:

    m_isBusy = false;
    metaObject()->invokeMethod(this, "repaint", Qt::QueuedConnection);
}



static bool _TreeSearchCallback(PGE_EditSceneItem* item, void* arg)
{
    PGE_EditScene::PGE_EditItemList* list = static_cast<PGE_EditScene::PGE_EditItemList* >(arg);
    if(list)
    {
        if(item) (*list).push_back(item);
    }
    return true;
}

void PGE_EditScene::queryItems(RRect &zone, PGE_EditScene::PGE_EditItemList *resultList)
{
    RPoint lt={zone.l, zone.t};
    RPoint rb={zone.r, zone.b};
    m_tree.Search(lt, rb, _TreeSearchCallback, (void*)resultList);
}

void PGE_EditScene::queryItems(int x, int y, PGE_EditScene::PGE_EditItemList *resultList)
{
    RRect zone = {x, y, x+1, y+1};
    queryItems(zone, resultList);
}

void PGE_EditScene::registerElement(PGE_EditSceneItem *item)
{
    PGE_Rect<int> &r = item->m_posRect;
    RPoint lt={ r.left(),  r.top() };
    RPoint rb={ r.right(), r.bottom() };
    if(rb[0] < lt[0]) { rb[0]=lt[0]+1; }
    if(rb[1] < lt[1]) { rb[1]=lt[1]+1; }
    m_tree.Insert(lt, rb, item);
    item->m_posRectTree = r;
}

void PGE_EditScene::unregisterElement(PGE_EditSceneItem *item)
{
    PGE_Rect<int> &r = item->m_posRectTree;
    RPoint lt={ r.left(),  r.top() };
    RPoint rb={ r.right(), r.bottom()};
    if(rb[0] < lt[0]) { rb[0]=lt[0]+1; }
    if(rb[1] < lt[1]) { rb[1]=lt[1]+1; }
    m_tree.Remove(lt, rb, item);
}






QPoint PGE_EditScene::mapToWorld(const QPoint &mousePos)
{
    QPoint w = mousePos;
    w.setX( qRound(qreal(w.x())/m_zoom) );
    w.setY( qRound(qreal(w.y())/m_zoom) );
    w += m_cameraPos;
    return w;
}

QRect PGE_EditScene::applyZoom(const QRect &r)
{
    QRect t = r;
    t.moveTo( t.topLeft() - m_cameraPos );
    t.moveTo( qRound(qreal(t.x())*m_zoom), qRound(qreal(t.y())*m_zoom));
    t.setWidth( qRound(qreal(t.width())*m_zoom) );
    t.setHeight( qRound(qreal(t.height())*m_zoom) );
    return t;
}

void PGE_EditScene::moveCamera()
{
    moveCamera(m_mover.speedX, m_mover.speedY);
    moveCameraUpdMouse(m_mover.speedX, m_mover.speedY);
    repaint();
}

void PGE_EditScene::moveCamera(int deltaX, int deltaY)
{
    m_cameraPos.setX( m_cameraPos.x() + deltaX );
    m_cameraPos.setY( m_cameraPos.y() + deltaY );
}

void PGE_EditScene::moveCameraUpdMouse(int deltaX, int deltaY)
{
    if(m_moveInProcess || m_rectSelect)
    {
        m_mouseOld.setX(m_mouseOld.x() + deltaX);
        m_mouseOld.setY(m_mouseOld.y() + deltaY);
        if(m_moveInProcess)
            moveSelection(deltaX, deltaY);
    }
}

void PGE_EditScene::closeEvent(QCloseEvent *event)
{
    m_abortThread = true;

    bool wasBusy = m_isBusy;
    while(m_isBusy)
        QThread::msleep(1);

    if(wasBusy)
        QMessageBox::information(this, "Closed", "Ouuuuch.... :-P");
    event->accept();
}

void PGE_EditScene::mousePressEvent(QMouseEvent *event)
{
    if(m_isBusy)
        return;

    if(event->button() != Qt::LeftButton)
        return;

    QPoint pos = mapToWorld(event->pos());

    m_mouseBegin = pos;
    m_mouseOld   = pos;

    bool isShift =  (event->modifiers() & Qt::ShiftModifier) != 0;
    bool isCtrl =   (event->modifiers() & Qt::ControlModifier) != 0;

    if( !isShift )
    {
        bool catched = false;
        for(PGE_EditSceneItem &item : m_items)
        {
            if( item.isTouches(m_mouseOld.x(), m_mouseOld.y()) )
            {
                catched = true;
                if(isCtrl)
                {
                    toggleselect(item);
                }
                else
                if(!item.selected())
                {
                    clearSelection();
                    select(item);
                }
                break;
            }
        }
        if(!catched && !isCtrl)
            clearSelection();
        else
        if(catched)
        {
            moveStart();
        }
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

void PGE_EditScene::mouseMoveEvent(QMouseEvent *event)
{
    if(m_isBusy)
        return;

    if((event->buttons() & Qt::LeftButton) == 0)
        return;

    QPoint pos = mapToWorld(event->pos());

    if(m_ignoreMove)
        return;

    QPoint delta = m_mouseOld - pos;
    if(!m_rectSelect)
    {
        moveSelection(-delta.x(), -delta.y());
    }
    m_mouseOld = pos;
    repaint();
}

void PGE_EditScene::mouseReleaseEvent(QMouseEvent *event)
{
    if(m_isBusy)
        return;
    bool isShift =  (event->modifiers() & Qt::ShiftModifier) != 0;
    bool isCtrl  =  (event->modifiers() & Qt::ControlModifier) != 0;
    QPoint pos = mapToWorld(event->pos());

    if(m_moveInProcess)
        moveEnd(false);

    if( (event->button()==Qt::RightButton) && (event->buttons() == 0) )
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

    m_mouseEnd = pos;
    if(m_rectSelect)
    {
        int left   = m_mouseBegin.x() < m_mouseEnd.x() ? m_mouseBegin.x() : m_mouseEnd.x();
        int right  = m_mouseBegin.x() > m_mouseEnd.x() ? m_mouseBegin.x() : m_mouseEnd.x();
        int top    = m_mouseBegin.y() < m_mouseEnd.y() ? m_mouseBegin.y() : m_mouseEnd.y();
        int bottom = m_mouseBegin.y() > m_mouseEnd.y() ? m_mouseBegin.y() : m_mouseEnd.y();
        QRect selZone;
        selZone.setCoords(left, top, right, bottom);
        for(PGE_EditSceneItem &item : m_items)
        {
            if( item.isTouches(selZone) )
            {
                if(isShift && isCtrl)
                    toggleselect(item);
                else
                    select(item);
            }
        }
        m_rectSelect=false;
        repaint();
    }
}

void PGE_EditScene::wheelEvent(QWheelEvent *event)
{
    if(m_isBusy)
        return;

    bool isShift =  (event->modifiers() & Qt::ShiftModifier) != 0;
    bool isCtrl  =  (event->modifiers() & Qt::ControlModifier) != 0;
    bool isAlt   =  (event->modifiers() & Qt::AltModifier) != 0;

    if( isAlt )
    {
        if(event->delta() > 0)
            m_zoom += 0.1;
        else
            m_zoom -= 0.1;

        if(m_zoom <= 0.1)
            m_zoom = 0.1;

        repaint();
    }
    else
    if( isCtrl )
    {
        int delta = m_mover.scrollStep * (event->delta() < 0 ? 1 : -1) * (isShift ? 4 : 1);
        moveCamera(delta, 0);
        moveCameraUpdMouse(delta, 0);
        repaint();
    }
    else
    {
        int delta = m_mover.scrollStep * (event->delta() < 0 ? 1 : -1) * (isShift ? 4 : 1);
        moveCamera(0, delta);
        moveCameraUpdMouse(0, delta);
        repaint();
    }
}

void PGE_EditScene::paintEvent(QPaintEvent */*event*/)
{
    QPainter p(this);
    if(m_isBusy)
    {
        p.setBrush(QBrush(Qt::black));
        p.setPen(QPen(Qt::black));
        p.drawText(QPointF(20.0, 20.0), "Loading...");
        p.end();
        return;
    }

    PGE_EditItemList list;
    RRect vizArea = {m_cameraPos.x(),
                     m_cameraPos.y(),
                     m_cameraPos.x() + qRound(qreal(width()) / m_zoom),
                     m_cameraPos.y() + qRound(qreal(height()) / m_zoom) };
    queryItems(vizArea, &list);

    for(PGE_EditSceneItem* item : list)
    {
        item->paint(&p, m_cameraPos, m_zoom);
    }

    if(m_rectSelect)
    {
        p.setBrush(QBrush(Qt::green));
        p.setPen(QPen(Qt::darkGreen));
        p.setOpacity(0.5);
        QRect r = applyZoom(QRect(m_mouseBegin, m_mouseOld));
        p.drawRect(r);
    }
    p.end();
}

void PGE_EditScene::keyPressEvent(QKeyEvent *event)
{
    if(m_isBusy)
        return;

    bool isCtrl = (event->modifiers() & Qt::ControlModifier) != 0;
    switch(event->key())
    {
    case Qt::Key_Left:
        if(isCtrl)
        {
            moveSelection(-1, 0);
            repaint();
        }
        else
        {
            if(event->isAutoRepeat()) return;
            m_mover.setLeft(true);
        }
        break;
    case Qt::Key_Right:
        if(isCtrl)
        {
            moveSelection(1, 0);
            repaint();
        }
        else
        {
            if(event->isAutoRepeat()) return;
            m_mover.setRight(true);
        }
        break;
    case Qt::Key_Up:
        if(isCtrl)
        {
            moveSelection(0, -1);
            repaint();
        }
        else
        {
            if(event->isAutoRepeat()) return;
            m_mover.setUp(true);
        }
        break;
    case Qt::Key_Down:
        if(isCtrl)
        {
            moveSelection(0, 1);
            repaint();
        }
        else
        {
            if(event->isAutoRepeat()) return;
            m_mover.setDown(true);
        }
        break;
    case Qt::Key_Shift:
        if(event->isAutoRepeat()) return;
        m_mover.setFaster(true);
        break;
    default:
        QWidget::keyPressEvent(event);
        return;
    }
}

void PGE_EditScene::keyReleaseEvent(QKeyEvent *event)
{
    if(m_isBusy)
        return;

    switch(event->key())
    {
    case Qt::Key_Escape:
        clearSelection();
        m_rectSelect=false;
        repaint();
        break;
    case Qt::Key_Left:
        {
            if(event->isAutoRepeat()) return;
            m_mover.setLeft(false);
        }
        break;
    case Qt::Key_Right:
        {
            if(event->isAutoRepeat()) return;
            m_mover.setRight(false);
        }
        break;
    case Qt::Key_Up:
        {
            if(event->isAutoRepeat()) return;
            m_mover.setUp(false);
        }
        break;
    case Qt::Key_Down:
        {
            if(event->isAutoRepeat()) return;
            m_mover.setDown(false);
        }
        break;
    case Qt::Key_Shift:
        if(event->isAutoRepeat()) return;
        m_mover.setFaster(false);
        break;
    default:
        QWidget::keyReleaseEvent(event);
        return;
    }
}

void PGE_EditScene::focusInEvent(QFocusEvent *event)
{
    QWidget::focusInEvent(event);
}

void PGE_EditScene::focusOutEvent(QFocusEvent *event)
{
    //releaseKeyboard();
    m_mover.reset();
    QWidget::focusOutEvent(event);
}
