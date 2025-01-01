/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2014-2025 Vitaly Novichkov <admin@wohlnet.ru>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "graphicsworkspace.h"
#include "logger.h"
#include <QElapsedTimer>
#include <QRubberBand>

#include <editing/_scenes/level/lvl_scene.h>
#include <editing/_scenes/world/wld_scene.h>
#include <pge_qt_compat.h>

#include <common_features/main_window_ptr.h>


PGEMouseEvent::PGEMouseEvent(const QMouseEvent *o)
{
    operator=(o);
}

PGEMouseEvent::PGEMouseEvent(QEvent::Type type,
                             const QPointF &localPos,
                             const QPointF &windowPos,
                             const QPointF &screenPos,
                             Qt::MouseButton button,
                             Qt::MouseButtons buttons,
                             Qt::KeyboardModifiers modifiers) :
    m_type(type),
    m_pos(localPos.toPoint()),
    m_localPos(localPos),
    m_globalPos(screenPos),
    m_windowPos(windowPos),
    m_screenPos(screenPos),
    m_button(button),
    m_buttons(buttons),
    m_modifiers(modifiers)
{}

PGEMouseEvent &PGEMouseEvent::operator=(const QMouseEvent *o)
{
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    m_device = o->device();
#endif
    m_type = o->type();
    m_pos = o->Q_EventPos();
    m_localPos = o->Q_EventLocalPos();
    m_globalPos = o->Q_EventGlobalPos();
    m_screenPos = o->Q_EventScreenPos();
    m_windowPos = o->Q_EventWindowPos();
    m_button = o->button();
    m_buttons = o->buttons();
    m_modifiers = o->modifiers();
    m_spontanous = o->spontaneous();
    return *this;
}


bool qt_sendSpontaneousEvent(QObject *receiver, QEvent *event);

GraphicsWorkspace::GraphicsWorkspace(QWidget *parent) :
    QGraphicsView(parent)
{
    useLastMouseEvent = true;
    handScrolling = false;
    rubberBanding = false;
    rubberBandRect = QRect(0, 0, 0, 0);
    originPoint = QPoint(0, 0);
    rubberBandX = new QRubberBand(QRubberBand::Rectangle, this->viewport());
    rubberBandX->setGeometry(QRect(this->viewport()->mapFromGlobal(originPoint), QSize()));
    hasStoredOriginalCursor = false;
    handScrollMotions = 0;
    originalCursor = this->cursor();
    rubberBandSelectionMode = Qt::IntersectsItemShape;
    this->setMouseTracking(true);
    this->setRubberBandSelectionMode(Qt::IntersectsItemShape);

    movement = MOVE_IDLE;

    Mover.setTimerType(Qt::PreciseTimer);

    step = 32;
    keyTime = 25;

    zoomValue = 1.0;
    scaleFactor = 1.15;

    scaleMin = 0.01;
    scaleMax = 20.0;

    connect(&Mover, SIGNAL(timeout()), this, SLOT(doMove()));
    connect(horizontalScrollBar(), SIGNAL(valueChanged(int)), this, SLOT(replayLastMouseEvent(int)));
    connect(verticalScrollBar(), SIGNAL(valueChanged(int)), this, SLOT(replayLastMouseEvent(int)));

    setTransformationAnchor(QGraphicsView::AnchorViewCenter);
}

GraphicsWorkspace::~GraphicsWorkspace()
{
    delete rubberBandX;
}

qreal GraphicsWorkspace::zoom()
{
    return zoomValue;
}

void GraphicsWorkspace::setZoom(qreal zoom)
{
    if(zoom >= scaleMax || zoom <= scaleMin) return;
    scale(1.0 / zoomValue, 1.0 / zoomValue);
    zoomValue = zoom;
    scale(zoom, zoom);
    emit zoomValueChanged(qRound(zoomValue * 100));
    emit zoomValueChanged(QString::number(qRound(zoomValue * 100)));
}

void GraphicsWorkspace::zoomIn()
{
    if(zoomValue * scaleFactor >= scaleMax) return;

    zoomValue *= scaleFactor;
    scale(scaleFactor, scaleFactor);
    emit zoomValueChanged(qRound(zoomValue * 100));
    emit zoomValueChanged(QString::number(qRound(zoomValue * 100)));
}

void GraphicsWorkspace::zoomOut()
{
    if(zoomValue * scaleFactor <= scaleMin) return;

    zoomValue *= 1.0 / scaleFactor;
    scale(1.0 / scaleFactor, 1.0 / scaleFactor);
    emit zoomValueChanged(qRound(zoomValue * 100));
    emit zoomValueChanged(QString::number(qRound(zoomValue * 100)));
}

QRubberBand *GraphicsWorkspace::rubberBand() const
{
    if(rubberBandX)
        return rubberBandX;
    else
        return nullptr;
}

void GraphicsWorkspace::moveLeft()
{
    bool rtl = (qApp->layoutDirection() == Qt::RightToLeft);
    horizontalScrollBar()->setValue(horizontalScrollBar()->value() - step * (rtl ? -1 : 1));
}

void GraphicsWorkspace::moveRight()
{
    bool rtl = (qApp->layoutDirection() == Qt::RightToLeft);
    horizontalScrollBar()->setValue(horizontalScrollBar()->value() + step * (rtl ? -1 : 1));
}

void GraphicsWorkspace::moveUp()
{
    verticalScrollBar()->setValue(verticalScrollBar()->value() - step);
}

void GraphicsWorkspace::moveDown()
{
    verticalScrollBar()->setValue(verticalScrollBar()->value() + step);
}

void GraphicsWorkspace::doMove()
{
    if((movement & MOVE_LEFT) != 0)
        moveLeft();
    if((movement & MOVE_RIGHT) != 0)
        moveRight();
    if((movement & MOVE_UP) != 0)
        moveUp();
    if((movement & MOVE_DOWN) != 0)
        moveDown();
}

void GraphicsWorkspace::keyPressEvent(QKeyEvent *event)
{
    if(event->isAutoRepeat())
    {
        event->ignore();
        return;
    }

    event->accept();
    replayLastMouseEvent();

    int lastMov = movement;
    switch(event->key())
    {
    case Qt::Key_Left:
        movement |= MOVE_LEFT;
        break;
    case Qt::Key_Right:
        movement |= MOVE_RIGHT;
        break;
    case Qt::Key_Up:
        movement |= MOVE_UP;
        break;
    case Qt::Key_Down:
        movement |= MOVE_DOWN;
        break;
    case Qt::Key_Shift:
        keyTime = 5;
        updateTimerInterval();
        break;
    default:
        break;
    }

    if((movement != MOVE_IDLE) && (!Mover.isActive()))
        Mover.start(keyTime);

    if(lastMov != movement) return;

    QGraphicsView::keyPressEvent(event);
}

void GraphicsWorkspace::keyReleaseEvent(QKeyEvent *event)
{
    if(event->isAutoRepeat())
    {
        event->ignore();
        return;
    }

    event->accept();
    replayLastMouseEvent();
    int lastMov = movement;
    switch(event->key())
    {
    case Qt::Key_Left:
        movement &= ~MOVE_LEFT;
        return;
    case Qt::Key_Right:
        movement &= ~MOVE_RIGHT;
        return;
    case Qt::Key_Up:
        movement &= ~MOVE_UP;
        return;
    case Qt::Key_Down:
        movement &= ~MOVE_DOWN;
        return;
    case Qt::Key_Shift:
        keyTime = 25;
        updateTimerInterval();
        break;
    default:
        break;
    }

    if((movement == MOVE_IDLE) && (Mover.isActive()))
        Mover.stop();

    if(lastMov != movement) return;

    QGraphicsView::keyReleaseEvent(event);
}

void GraphicsWorkspace::updateTimerInterval()
{
    Mover.setInterval(keyTime);
    replayLastMouseEvent();
}

void GraphicsWorkspace::focusOutEvent(QFocusEvent *event)
{
    movement = MOVE_IDLE;
    Mover.stop();
    QGraphicsView::focusOutEvent(event);
}


void GraphicsWorkspace::wheelEvent(QWheelEvent *event)
{
    int modS = 128;
    int modS_h = modS;
    bool rtl = (qApp->layoutDirection() == Qt::RightToLeft);

    if(rtl)
        modS_h *= -1;

    if((event->modifiers() & Qt::ShiftModifier) != 0)
    {
        modS *= 2;
        modS_h *= 2;
    }

    auto delta = event->angleDelta();
    int deltaX = delta.x();
    int deltaY = delta.y();

    if((event->modifiers() & Qt::ControlModifier) != 0)
    {
        // Scale the view / do the zoom
        if(deltaY > 0)
        {
            if(zoomValue * scaleFactor >= scaleMax) return;
            // Zoom in
            zoomValue *= scaleFactor;
            scale(scaleFactor, scaleFactor);
            emit zoomValueChanged(qRound(zoomValue * 100));
            emit zoomValueChanged(QString::number(qRound(zoomValue * 100)));
        }
        else if(deltaY < 0)
        {
            if(zoomValue * scaleFactor <= scaleMin) return;
            // Zooming out
            zoomValue *= 1.0 / scaleFactor;
            scale(1.0 / scaleFactor, 1.0 / scaleFactor);
            emit zoomValueChanged(qRound(zoomValue * 100));
            emit zoomValueChanged(QString::number(qRound(zoomValue * 100)));
        }

        replayLastMouseEvent();
        return;
    }

    auto *hBar = horizontalScrollBar();
    auto *vBar = verticalScrollBar();

    if((event->modifiers() & Qt::AltModifier) != 0)
    {
        // Ensure values aren't already swapped
        if(delta.x() == 0 && delta.y() != 0)
        {
            deltaY = delta.x();
            deltaX = delta.y();
        }
    }

    if(deltaX > 0)
        hBar->setValue(hBar->value() - modS);
    else if(deltaX < 0)
        hBar->setValue(hBar->value() + modS);

    if(deltaY > 0)
        vBar->setValue(vBar->value() - modS);
    else if(deltaY < 0)
        vBar->setValue(vBar->value() + modS);

    //event->accept();
    replayLastMouseEvent();

    if(scene())
        scene()->update();
}



void GraphicsWorkspace::mousePressEvent(QMouseEvent *event)
{
    //using namespace pge_graphicsworkspace;

    // Store this event for replaying, finding deltas, and for
    // scroll-dragging; even in non-interactive mode, scroll hand dragging is
    // allowed, so we store the event at the very top of this function.
    ///d->storeMouseEvent(event);
    useLastMouseEvent = true;
    bool multiSelect = (event->modifiers() & Qt::ShiftModifier) != 0;

    lastMouseEvent = PGEMouseEvent(QEvent::MouseMove,
                                   event->Q_EventLocalPos(),
                                   event->Q_EventWindowPos(),
                                   event->Q_EventScreenPos(),
                                   event->button(),
                                   event->buttons(),
                                   event->modifiers());

    lastMouseEvent.setAccepted(false);

    if(this->isInteractive())
    {
        // Store some of the event's button-down data.
        mousePressViewPoint = event->pos();
        mousePressScenePoint = mapToScene(mousePressViewPoint);
        mousePressScreenPoint = event->Q_EventGlobalPos();
        lastMouseMoveScenePoint = mousePressScenePoint;
        lastMouseMoveScreenPoint = mousePressScreenPoint;
        mousePressButton = event->button();

        if(this->scene())
        {
            // Convert and deliver the mouse event to the scene.
            QGraphicsSceneMouseEvent mouseEvent(QEvent::GraphicsSceneMousePress);
            mouseEvent.setWidget(viewport());
            mouseEvent.setButtonDownScenePos(mousePressButton, mousePressScenePoint);
            mouseEvent.setButtonDownScreenPos(mousePressButton, mousePressScreenPoint);
            mouseEvent.setScenePos(mousePressScenePoint);
            mouseEvent.setScreenPos(mousePressScreenPoint);
            mouseEvent.setLastScenePos(lastMouseMoveScenePoint);
            mouseEvent.setLastScreenPos(lastMouseMoveScreenPoint);
            mouseEvent.setButtons(event->buttons());
            mouseEvent.setButton(event->button());
            mouseEvent.setModifiers(event->modifiers());
            mouseEvent.setAccepted(false);
            if(!multiSelect)//Don't send event on ctrl-key
            {
                if(event->spontaneous())
                    qt_sendSpontaneousEvent(scene(), &mouseEvent);
                else
                    QApplication::sendEvent(scene(), &mouseEvent);
            }

            // Update the original mouse event accepted state.
            bool isAccepted = mouseEvent.isAccepted();
            event->setAccepted(isAccepted);

            // Update the last mouse event accepted state.
            lastMouseEvent.setAccepted(isAccepted);

            if(isAccepted)
                return;
        }
    }
    else
    {
        LvlScene *s = dynamic_cast<LvlScene *>(scene());
        WldScene *w = dynamic_cast<WldScene *>(scene());
        bool setSelect = false;
        if(s)
            setSelect = (s->m_editMode == LvlScene::MODE_HandScroll);
        else if(w)
            setSelect = ((!w->m_isSelectionDialog) && (w->m_editMode == WldScene::MODE_HandScroll));
        if(setSelect)
        {
            if(event->buttons() & Qt::RightButton)
                MainWinConnect::pMainWin->on_actionSelect_triggered();
        }
    }

#ifndef QT_NO_RUBBERBAND
    if(dragMode() == QGraphicsView::RubberBandDrag && !rubberBanding && (event->button()&Qt::LeftButton))
    {
        if(this->isInteractive())
        {
            // Rubberbanding is only allowed in interactive mode.
            event->accept();
            rubberBanding = true;
            rubberBandRect = QRect();
            if(scene())
            {
                if(multiSelect)
                    rubberBandExtendSelection = scene()->selectedItems();
                else
                    // Initiating a rubber band always clears the selection.
                    scene()->clearSelection();
            }
            //originPoint = mapToScene(mousePressViewPoint);
            rubberBandX->setGeometry(QRect(mapFromScene(mousePressScenePoint), QSize()));
            rubberBandX->show();
        }
    }
    else
#endif
    if(dragMode() == QGraphicsView::ScrollHandDrag && event->button() == Qt::LeftButton)
    {
        // Left-button press in scroll hand mode initiates hand scrolling.
        event->accept();
        handScrolling = true;
        handScrollMotions = 0;
        #ifndef QT_NO_CURSOR
        viewport()->setCursor(Qt::ClosedHandCursor);
        #endif
    }
}

void GraphicsWorkspace::mouseMoveEvent(QMouseEvent *event)
{
    if(dragMode() == QGraphicsView::ScrollHandDrag)
    {
        if(handScrolling)
        {
            QScrollBar *hBar = horizontalScrollBar();
            QScrollBar *vBar = verticalScrollBar();
            QPoint delta = event->pos() - lastMouseEvent.pos();
            hBar->setValue(hBar->value() + (isRightToLeft() ? delta.x() : -delta.x()));
            vBar->setValue(vBar->value() - delta.y());

            // Detect how much we've scrolled to disambiguate scrolling from
            // clicking.
            ++handScrollMotions;
        }
    }

    PGEMouseEvent pEvent(event);
    mouseMoveEventHandler(pEvent);
}

void GraphicsWorkspace::storeMouseEvent(const PGEMouseEvent &event)
{
    useLastMouseEvent = true;
    lastMouseEvent = event;
}

void GraphicsWorkspace::storeMouseEvent(const QMouseEvent *event)
{
    useLastMouseEvent = true;
    lastMouseEvent = PGEMouseEvent(event);
}

void GraphicsWorkspace::mouseMoveEventHandler(PGEMouseEvent &event)
{
#if !defined(QT_NO_RUBBERBAND)
    updateRubberBand(event);
#endif

#ifdef _DEBUG_
    WriteToLog(QtDebugMsg, "GraphicsView -> MouseMoveHandler start");
#endif
    storeMouseEvent(event);
    lastMouseEvent.setAccepted(false);

    if(!isInteractive())
        return;
    if(handScrolling)
        return;
    if(!scene())
        return;

    QGraphicsSceneMouseEvent mouseEvent(QEvent::GraphicsSceneMouseMove);
    mouseEvent.setWidget(viewport());
    mouseEvent.setButtonDownScenePos(mousePressButton, mousePressScenePoint);
    mouseEvent.setButtonDownScreenPos(mousePressButton, mousePressScreenPoint);
    mouseEvent.setScenePos(mapToScene(event.pos()));
    mouseEvent.setScreenPos(event.globalPos().toPoint());
    mouseEvent.setLastScenePos(lastMouseMoveScenePoint);
    mouseEvent.setLastScreenPos(lastMouseMoveScreenPoint);
    mouseEvent.setButtons(event.buttons());
    mouseEvent.setButton(event.button());
    mouseEvent.setModifiers(event.modifiers());
    lastMouseMoveScenePoint = mouseEvent.scenePos();
    lastMouseMoveScreenPoint = mouseEvent.screenPos();
    mouseEvent.setAccepted(false);
    if(event.spontaneous())
        qt_sendSpontaneousEvent(scene(), &mouseEvent);
    else
        QApplication::sendEvent(scene(), &mouseEvent);

    // Remember whether the last event was accepted or not.
    lastMouseEvent.setAccepted(mouseEvent.isAccepted());

    if(mouseEvent.isAccepted() && mouseEvent.buttons() != 0)
    {
        // The event was delivered to a mouse grabber; the press is likely to
        // have set a cursor, and we must not change it.
        return;
    }

#ifndef QT_NO_CURSOR
    // If all the items ignore hover events, we don't look-up any items
    // in QGraphicsScenePrivate::dispatchHoverEvent, hence the
    // cachedItemsUnderMouse list will be empty. We therefore do the look-up
    // for cursor items here if not all items use the default cursor.
    if(/*scene()->d_func()->allItemsIgnoreHoverEvents && !scene()->d_func()->allItemsUseDefaultCursor
        && */ cachedItemsUnderMouse.isEmpty())
    {
        QRectF target(mapToScene(mouseEvent.screenPos()), mapToScene(mouseEvent.screenPos() + QPoint(1, 1)));
        LvlScene *lsc = qobject_cast<LvlScene * >(scene());
        WldScene *wsc = qobject_cast<WldScene * >(scene());
        if(lsc)
            lsc->queryItems(target, &cachedItemsUnderMouse);
        else if(wsc)
            wsc->queryItems(target, &cachedItemsUnderMouse);
        else
            cachedItemsUnderMouse = scene()->items(target, Qt::IntersectsItemBoundingRect);
    }

    // Find the topmost item under the mouse with a cursor.
    for(QGraphicsItem *item : cachedItemsUnderMouse)
    {
        if(item && item->hasCursor())
        {
            _q_setViewportCursor(item->cursor());
            return;
        }
    }
    cachedItemsUnderMouse.clear();

    // No items with cursors found; revert to the view cursor.
    if(hasStoredOriginalCursor)
    {
        // Restore the original viewport cursor.
        hasStoredOriginalCursor = false;
        viewport()->setCursor(originalCursor);
    }
#endif

#ifdef _DEBUG_
    WriteToLog(QtDebugMsg, "GraphicsView -> MouseMoveHandler End");
#endif
}


void GraphicsWorkspace::_q_setViewportCursor(const QCursor &cursor)
{
    if(!hasStoredOriginalCursor)
    {
        hasStoredOriginalCursor = true;
        originalCursor = viewport()->cursor();
    }
    viewport()->setCursor(cursor);
}

void GraphicsWorkspace::_q_unsetViewportCursor()
{
    QList<QGraphicsItem *> theItems;
    QRectF target(lastMouseEvent.pos(), lastMouseEvent.pos() + QPoint(1, 1));
    LvlScene *lsc = qobject_cast<LvlScene * >(scene());
    WldScene *wsc = qobject_cast<WldScene * >(scene());
    if(lsc)
        lsc->queryItems(target, &theItems);
    else if(wsc)
        wsc->queryItems(target, &theItems);
    else
        theItems = scene()->items(target, Qt::IntersectsItemBoundingRect);

    for(QGraphicsItem *item : theItems)
    {
        if(item->hasCursor())
        {
            _q_setViewportCursor(item->cursor());
            return;
        }
    }

    // Restore the original viewport cursor.
    if(hasStoredOriginalCursor)
    {
        hasStoredOriginalCursor = false;
        if(dragMode() == QGraphicsView::ScrollHandDrag)
            viewport()->setCursor(Qt::OpenHandCursor);
        else
            viewport()->setCursor(originalCursor);
    }
}

void GraphicsWorkspace::replayLastMouseEvent(int x)
{
    Q_UNUSED(x);
    if(!useLastMouseEvent || !scene())
        return;

    mouseMoveEventHandler(lastMouseEvent);
}

QRegion GraphicsWorkspace::rubberBandRegion(const QWidget *widget, const QRect &rect) const
{
    QStyleHintReturnMask mask;
    QStyleOptionRubberBand option;
    option.initFrom(widget);
    option.rect = rect;
    option.opaque = false;
    option.shape = QRubberBand::Rectangle;

    QRegion tmp;
    tmp += rect;
    if(widget->style()->styleHint(QStyle::SH_RubberBand_Mask, &option, widget, &mask))
        tmp &= mask.region;
    return tmp;
}

void GraphicsWorkspace::updateRubberBand(const PGEMouseEvent &event)
{
#if !defined(QT_NO_RUBBERBAND)

    if(dragMode() != QGraphicsView::RubberBandDrag || !isInteractive() || !rubberBanding)
        return;

    // Check for enough drag distance
    if((mousePressViewPoint - event.pos()).manhattanLength() < QApplication::startDragDistance())
        return;

    // Update old rubberband
    if(viewportUpdateMode() != QGraphicsView::NoViewportUpdate && !rubberBandRect.isEmpty())
    {
        if(viewportUpdateMode() != QGraphicsView::FullViewportUpdate)
            viewport()->update(rubberBandRegion(viewport(), rubberBandRect));
        else
            update();
    }

    // Stop rubber banding if the user has let go of all buttons (even
    // if we didn't get the release events).
    if(!event.buttons())
    {
        rubberBanding = false;
        if(!rubberBandRect.isNull())
        {
            rubberBandRect = QRect();
#if QT_VERSION >= QT_VERSION_CHECK(5, 1, 0)
            emit rubberBandChanged(rubberBandRect, QPointF(), QPointF());
#endif
        }
        return;
    }

    QRect oldRubberband = rubberBandRect;

    // Update rubberband position
    const QPoint mp = mapFromScene(mousePressScenePoint);
    const QPoint ep = event.pos();
    rubberBandRect = QRect(qMin(mp.x(), ep.x()), qMin(mp.y(), ep.y()),
                           qAbs(mp.x() - ep.x()) + 1, qAbs(mp.y() - ep.y()) + 1);

    if(rubberBandRect != oldRubberband || lastRubberbandScenePoint != lastMouseMoveScenePoint)
    {
        lastRubberbandScenePoint = lastMouseMoveScenePoint;
        oldRubberband = rubberBandRect;
#if QT_VERSION >= QT_VERSION_CHECK(5, 1, 0)
        emit rubberBandChanged(rubberBandRect, mousePressScenePoint, lastRubberbandScenePoint);
#endif
    }

    // Update new rubberband
    if(viewportUpdateMode() != QGraphicsView::NoViewportUpdate)
    {
        if(viewportUpdateMode() != QGraphicsView::FullViewportUpdate)
            viewport()->update(rubberBandRegion(viewport(), rubberBandRect));
        else
            update();
    }

    // Set the new selection area
    QPainterPath selectionArea;
    selectionArea.addPolygon(mapToScene(rubberBandRect));
    selectionArea.closeSubpath();

    if(scene())
    {
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
        scene()->setSelectionArea(selectionArea,
                                  Qt::ReplaceSelection,
                                  rubberBandSelectionMode,
                                  viewportTransform());
#else
        scene()->setSelectionArea(selectionArea, rubberBandSelectionMode, viewportTransform());
#endif
        if(!rubberBandExtendSelection.isEmpty())
        {
            for(QGraphicsItem *item : rubberBandExtendSelection)
                item->setSelected(true);
        }
    }

    rubberBandX->setGeometry(rubberBandRect);
#else
    Q_UNUSED(event);
#endif
}

void GraphicsWorkspace::mouseReleaseEvent(QMouseEvent *event)
{
#if !defined(QT_NO_RUBBERBAND)
    if(dragMode() == QGraphicsView::RubberBandDrag && isInteractive() && !event->buttons())
    {
        if(rubberBanding)
        {
            if(viewportUpdateMode() != QGraphicsView::NoViewportUpdate)
            {
                if(viewportUpdateMode() != FullViewportUpdate)
                    viewport()->update(rubberBandRegion(viewport(), rubberBandRect));
                else
                    update();
            }

            rubberBanding = false;
            rubberBandX->hide();

            if(!rubberBandRect.isNull())
            {
                rubberBandRect = QRect();
                #if (QT_VERSION >= 0x050100)
                emit rubberBandChanged(rubberBandRect, QPointF(), QPointF());
                #endif
            }

            rubberBandExtendSelection.clear();
        }
    }
    else
#endif
    if(dragMode() == QGraphicsView::ScrollHandDrag && event->button() == Qt::LeftButton)
    {
        #ifndef QT_NO_CURSOR
        // Restore the open hand cursor. ### There might be items
        // under the mouse that have a valid cursor at this time, so
        // we could repeat the steps from mouseMoveEvent().
        viewport()->setCursor(Qt::OpenHandCursor);
        #endif
        handScrolling = false;

        if(scene() && isInteractive() && !lastMouseEvent.isAccepted() && handScrollMotions <= 6)
        {
            // If we've detected very little motion during the hand drag, and
            // no item accepted the last event, we'll interpret that as a
            // click to the scene, and reset the selection.
            scene()->clearSelection();
        }
    }

    storeMouseEvent(event);

    if(!isInteractive())
        return;

    if(!scene())
        return;

    QGraphicsSceneMouseEvent mouseEvent(QEvent::GraphicsSceneMouseRelease);
    mouseEvent.setWidget(viewport());
    mouseEvent.setButtonDownScenePos(mousePressButton, mousePressScenePoint);
    mouseEvent.setButtonDownScreenPos(mousePressButton, mousePressScreenPoint);
    mouseEvent.setScenePos(mapToScene(event->pos()));
    mouseEvent.setScreenPos(event->Q_EventGlobalPos());
    mouseEvent.setLastScenePos(lastMouseMoveScenePoint);
    mouseEvent.setLastScreenPos(lastMouseMoveScreenPoint);
    mouseEvent.setButtons(event->buttons());
    mouseEvent.setButton(event->button());
    mouseEvent.setModifiers(event->modifiers());
    mouseEvent.setAccepted(false);
    if(event->spontaneous())
        qt_sendSpontaneousEvent(scene(), &mouseEvent);
    else
        QApplication::sendEvent(scene(), &mouseEvent);

    // Update the last mouse event selected state.
    lastMouseEvent.setAccepted(mouseEvent.isAccepted());

    #ifndef QT_NO_CURSOR
    if(mouseEvent.isAccepted() && mouseEvent.buttons() == 0 && viewport()->testAttribute(Qt::WA_SetCursor))
    {
        // The last mouse release on the viewport will trigger clearing the cursor.
        _q_unsetViewportCursor();
    }
#endif
}

void GraphicsWorkspace::mouseDoubleClickEvent(QMouseEvent *event)
{
    mousePressEvent(event);
}

#ifdef QT_SHARED // avoid conflict with symbol in static lib
bool qt_sendSpontaneousEvent(QObject *receiver, QEvent *event)
{
    return QApplication::sendSpontaneousEvent(receiver, event);
}
#endif
