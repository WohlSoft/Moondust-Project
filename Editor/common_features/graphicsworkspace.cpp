/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2014 Vitaly Novichkov <admin@wohlnet.ru>
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

bool qt_sendSpontaneousEvent(QObject *receiver, QEvent *event);

GraphicsWorkspace::GraphicsWorkspace(QWidget *parent) :
    QGraphicsView(parent),
    lastMouseEvent(QEvent::None, QPointF(), QPointF(), QPointF(), Qt::NoButton, 0, 0)
{
    useLastMouseEvent=true;
    handScrolling=false;
    rubberBanding=false;
    rubberBandRect = QRect(0,0,0,0);
    originPoint = QPoint(0,0);
    rubberBandX = new QRubberBand(QRubberBand::Rectangle, this->viewport());
    rubberBandX->setGeometry(QRect(this->viewport()->mapFromGlobal(originPoint), QSize()));
    hasStoredOriginalCursor=false;
    handScrollMotions=0;
    originalCursor = this->cursor();
    rubberBandSelectionMode = Qt::IntersectsItemBoundingRect;
    this->setMouseTracking(true);
    this->setRubberBandSelectionMode(Qt::IntersectsItemBoundingRect);
    step=32;
    keyTime=25;

    zoomValue=1.0;
    scaleFactor = 1.15;

    scaleMin=0.01;
    scaleMax=20.0;

    connect(&lMover, SIGNAL(timeout()), this, SLOT(moveLeft()));
    connect(&rMover, SIGNAL(timeout()), this, SLOT(moveRight()));
    connect(&uMover, SIGNAL(timeout()), this, SLOT(moveUp()));
    connect(&dMover, SIGNAL(timeout()), this, SLOT(moveDown()));
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
    if(zoom>=scaleMax || zoom<=scaleMin) return;
    scale(1.0/zoomValue, 1.0/zoomValue);
    zoomValue = zoom;
    scale(zoom, zoom);
    emit zoomValueChanged(qRound(zoomValue*100));
    emit zoomValueChanged(QString::number(qRound(zoomValue*100)));
}

void GraphicsWorkspace::zoomIn()
{
    if(zoomValue*scaleFactor>=scaleMax) return;

    zoomValue *= scaleFactor;
    scale(scaleFactor, scaleFactor);
    emit zoomValueChanged(qRound(zoomValue*100));
    emit zoomValueChanged(QString::number(qRound(zoomValue*100)));
}

void GraphicsWorkspace::zoomOut()
{
    if(zoomValue*scaleFactor<=scaleMin) return;

    zoomValue *= 1.0/scaleFactor;
    scale(1.0 / scaleFactor, 1.0 / scaleFactor);
    emit zoomValueChanged(qRound(zoomValue*100));
    emit zoomValueChanged(QString::number(qRound(zoomValue*100)));
}

QRubberBand *GraphicsWorkspace::rubberBand() const
{
    if(rubberBandX)
        return rubberBandX;
    else
        return NULL;
}

int stepSumm=0;
void GraphicsWorkspace::moveLeft()
{
        QElapsedTimer t;
        t.start();
    horizontalScrollBar()->setValue(horizontalScrollBar()->value()-(step+stepSumm));
        int l = t.elapsed();
        if(l>keyTime){ l=l%keyTime; stepSumm=step<<2;} else stepSumm=0;
        lMover.setInterval(keyTime-l);
}

void GraphicsWorkspace::moveRight()
{
        QElapsedTimer t;
        t.start();
    horizontalScrollBar()->setValue(horizontalScrollBar()->value()+(step+stepSumm));
        int l = t.elapsed();
        if(l>keyTime){ l=l%keyTime; stepSumm=step<<2;} else stepSumm=0;
        rMover.setInterval(keyTime-l);
}

void GraphicsWorkspace::moveUp()
{
        QElapsedTimer t;
        t.start();
    verticalScrollBar()->setValue(verticalScrollBar()->value()-(step+stepSumm));
        int l = t.elapsed();
        if(l>keyTime){ l=l%keyTime; stepSumm=step<<2;} else stepSumm=0;
        uMover.setInterval(keyTime-l);
}

void GraphicsWorkspace::moveDown()
{
        QElapsedTimer t;
        t.start();
    verticalScrollBar()->setValue(verticalScrollBar()->value()+(step+stepSumm));
        int l = t.elapsed();
        if(l>keyTime){ l=l%keyTime; stepSumm=step<<2;} else stepSumm=0;
        dMover.setInterval(keyTime-l);
}

void GraphicsWorkspace::keyPressEvent(QKeyEvent *event)
{
    if(event->isAutoRepeat() )
           {
               event->ignore();
               return;
           }

    event->accept();
    replayLastMouseEvent();

    switch(event->key())
    {
    case Qt::Key_Left:
        lMover.start(keyTime);
        return;
    case Qt::Key_Right:
        rMover.start(keyTime);
        return;
    case Qt::Key_Up:
        uMover.start(keyTime);
        return;
    case Qt::Key_Down:
        dMover.start(keyTime);
        return;
    case Qt::Key_Shift:
        keyTime=5;
        updateTimerInterval();
        break;
    default:
        break;
    }

    QGraphicsView::keyPressEvent(event);
}

void GraphicsWorkspace::keyReleaseEvent(QKeyEvent *event)
{
    if(event->isAutoRepeat() )
           {
               event->ignore();
               return;
           }

    event->accept();
    replayLastMouseEvent();
    stepSumm=0;
    switch(event->key())
    {
    case Qt::Key_Left:
        lMover.stop();
        return;
    case Qt::Key_Right:
        rMover.stop();
        return;
    case Qt::Key_Up:
        uMover.stop();
        return;
    case Qt::Key_Down:
        dMover.stop();
        return;
    case Qt::Key_Shift:
        keyTime=25;
        updateTimerInterval();
        break;
    default:
        break;
    }

    QGraphicsView::keyReleaseEvent(event);
}

void GraphicsWorkspace::updateTimerInterval()
{
    lMover.setInterval(keyTime);
    rMover.setInterval(keyTime);
    uMover.setInterval(keyTime);
    dMover.setInterval(keyTime);
    replayLastMouseEvent();
}

void GraphicsWorkspace::focusOutEvent(QFocusEvent *event)
{
    lMover.stop();
    rMover.stop();
    uMover.stop();
    dMover.stop();

    QGraphicsView::focusOutEvent(event);
}


void GraphicsWorkspace::wheelEvent(QWheelEvent *event)
{
    int modS=128;
    if(event->modifiers() & Qt::ShiftModifier) modS=256;

    if(event->modifiers() & Qt::AltModifier)
    {
      // Scale the view / do the zoom
      if(event->delta() > 0) {
          if(zoomValue*scaleFactor>=scaleMax) return;
          // Zoom in
          zoomValue *= scaleFactor;
          scale(scaleFactor, scaleFactor);
          emit zoomValueChanged(qRound(zoomValue*100));
          emit zoomValueChanged(QString::number(qRound(zoomValue*100)));
      } else {
          if(zoomValue*scaleFactor<=scaleMin) return;
          // Zooming out
          zoomValue *= 1.0/scaleFactor;
          scale(1.0 / scaleFactor, 1.0 / scaleFactor);
          emit zoomValueChanged(qRound(zoomValue*100));
          emit zoomValueChanged(QString::number(qRound(zoomValue*100)));
      }
      replayLastMouseEvent();
      return;
    }

    if(event->modifiers() & Qt::ControlModifier)
    {
        if(event->delta() > 0) {
            horizontalScrollBar()->setValue(horizontalScrollBar()->value()-modS);
        }
        else
        {
            horizontalScrollBar()->setValue(horizontalScrollBar()->value()+modS);
        }
        //event->accept();
        replayLastMouseEvent();
        if(scene())
        {
            scene()->update();
        }
        return;
    }
    else
    {
        if(event->delta() > 0) {
            verticalScrollBar()->setValue(verticalScrollBar()->value()-modS);
        }
        else
        {
            verticalScrollBar()->setValue(verticalScrollBar()->value()+modS);
        }
        //event->accept();
        replayLastMouseEvent();
        if(scene())
        {
            scene()->update();
        }
        return;
    }
    replayLastMouseEvent();
    QGraphicsView::wheelEvent(event);
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
    lastMouseEvent = QMouseEvent(QEvent::MouseMove, event->localPos(), event->windowPos(), event->screenPos(),
                                 event->button(), event->buttons(), event->modifiers());//d->storeMouseEvent(event);
    ///d->storeMouseEvent(event); //END
    lastMouseEvent.setAccepted(false);

    if (this->isInteractive())
    {
        // Store some of the event's button-down data.
        mousePressViewPoint = event->pos();
        mousePressScenePoint = mapToScene(mousePressViewPoint);
        mousePressScreenPoint = event->globalPos();
        lastMouseMoveScenePoint = mousePressScenePoint;
        lastMouseMoveScreenPoint = mousePressScreenPoint;
        mousePressButton = event->button();

        if (this->scene())
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
                if (event->spontaneous())
                    qt_sendSpontaneousEvent(scene(), &mouseEvent);
                else
                    QApplication::sendEvent(scene(), &mouseEvent);
            }

            // Update the original mouse event accepted state.
            bool isAccepted = mouseEvent.isAccepted();
            event->setAccepted(isAccepted);

            // Update the last mouse event accepted state.
            lastMouseEvent.setAccepted(isAccepted);

            if (isAccepted)
                return;
        }
    }

#ifndef QT_NO_RUBBERBAND
    if (dragMode() == QGraphicsView::RubberBandDrag && !rubberBanding && (event->button()&Qt::LeftButton) )
    {
        if (this->isInteractive()) {
            // Rubberbanding is only allowed in interactive mode.
            event->accept();
            rubberBanding = true;
            rubberBandRect = QRect();
            if(scene())
            {
                if ( multiSelect )
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
        if (dragMode() == QGraphicsView::ScrollHandDrag && event->button() == Qt::LeftButton)
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
    if (dragMode() == QGraphicsView::ScrollHandDrag) {
        if (handScrolling) {
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

    mouseMoveEventHandler(event);
}

void GraphicsWorkspace::storeMouseEvent(QMouseEvent *event)
{
    useLastMouseEvent = true;
    lastMouseEvent = QMouseEvent(QEvent::MouseMove, event->localPos(), event->windowPos(), event->screenPos(),
                                 event->button(), event->buttons(), event->modifiers());//d->storeMouseEvent(event);
}

void GraphicsWorkspace::mouseMoveEventHandler(QMouseEvent *event)
{
#ifndef QT_NO_RUBBERBAND
    updateRubberBand(event);
#endif

    #ifdef _DEBUG_
    WriteToLog(QtDebugMsg, "GraphicsView -> MouseMoveHandler start");
    #endif
    storeMouseEvent(event);
    lastMouseEvent.setAccepted(false);

    if (!isInteractive())
        return;
    if (handScrolling)
        return;
    if (!scene())
        return;

    QGraphicsSceneMouseEvent mouseEvent(QEvent::GraphicsSceneMouseMove);
    mouseEvent.setWidget(viewport());
    mouseEvent.setButtonDownScenePos(mousePressButton, mousePressScenePoint);
    mouseEvent.setButtonDownScreenPos(mousePressButton, mousePressScreenPoint);
    mouseEvent.setScenePos(mapToScene(event->pos()));
    mouseEvent.setScreenPos(event->globalPos());
    mouseEvent.setLastScenePos(lastMouseMoveScenePoint);
    mouseEvent.setLastScreenPos(lastMouseMoveScreenPoint);
    mouseEvent.setButtons(event->buttons());
    mouseEvent.setButton(event->button());
    mouseEvent.setModifiers(event->modifiers());
    lastMouseMoveScenePoint = mouseEvent.scenePos();
    lastMouseMoveScreenPoint = mouseEvent.screenPos();
    mouseEvent.setAccepted(false);
    if (event->spontaneous())
        qt_sendSpontaneousEvent(scene(), &mouseEvent);
    else
        QApplication::sendEvent(scene(), &mouseEvent);

    // Remember whether the last event was accepted or not.
    lastMouseEvent.setAccepted(mouseEvent.isAccepted());

    if (mouseEvent.isAccepted() && mouseEvent.buttons() != 0) {
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
        cachedItemsUnderMouse = scene()->items(
                    QRectF(mapToScene(mouseEvent.screenPos()),
                           mapToScene(mouseEvent.screenPos()+QPoint(1,1))),
                    Qt::IntersectsItemBoundingRect);
    }
    // Find the topmost item under the mouse with a cursor.
    foreach (QGraphicsItem *item, cachedItemsUnderMouse)
    {
        if(item)
            if (item->hasCursor())
            {
                _q_setViewportCursor(item->cursor());
                return;
            }
    }
    cachedItemsUnderMouse.clear();

    // No items with cursors found; revert to the view cursor.
    if (hasStoredOriginalCursor) {
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
    if (!hasStoredOriginalCursor) {
        hasStoredOriginalCursor = true;
        originalCursor = viewport()->cursor();
    }
    viewport()->setCursor(cursor);
}

void GraphicsWorkspace::_q_unsetViewportCursor()
{
    foreach (QGraphicsItem *item, items(lastMouseEvent.pos())) {
        if (item->hasCursor())
        {
            _q_setViewportCursor(item->cursor());
            return;
        }
    }

    // Restore the original viewport cursor.
    if (hasStoredOriginalCursor)
    {
        hasStoredOriginalCursor = false;
        if (dragMode() == QGraphicsView::ScrollHandDrag)
            viewport()->setCursor(Qt::OpenHandCursor);
        else
            viewport()->setCursor(originalCursor);
    }
}

void GraphicsWorkspace::replayLastMouseEvent(int x)
{
    Q_UNUSED(x);
    if (!useLastMouseEvent || !scene())
        return;
    mouseMoveEventHandler(&lastMouseEvent);
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
    if (widget->style()->styleHint(QStyle::SH_RubberBand_Mask, &option, widget, &mask))
        tmp &= mask.region;
    return tmp;
}

void GraphicsWorkspace::updateRubberBand(const QMouseEvent *event)
{
    if (dragMode() != QGraphicsView::RubberBandDrag || !isInteractive() || !rubberBanding)
        return;
    // Check for enough drag distance
    if ((mousePressViewPoint - event->pos()).manhattanLength() < QApplication::startDragDistance())
        return;

    // Update old rubberband
    if (viewportUpdateMode() != QGraphicsView::NoViewportUpdate && !rubberBandRect.isEmpty()) {
        if (viewportUpdateMode() != QGraphicsView::FullViewportUpdate)
            viewport()->update(rubberBandRegion(viewport(), rubberBandRect));
        else
            update();
    }

    // Stop rubber banding if the user has let go of all buttons (even
    // if we didn't get the release events).
    if (!event->buttons()) {
        rubberBanding = false;
        if (!rubberBandRect.isNull()) {
            rubberBandRect = QRect();
            emit rubberBandChanged(rubberBandRect, QPointF(), QPointF());
        }
        return;
    }

    QRect oldRubberband = rubberBandRect;

    // Update rubberband position
    const QPoint mp = mapFromScene(mousePressScenePoint);
    const QPoint ep = event->pos();
    rubberBandRect = QRect(qMin(mp.x(), ep.x()), qMin(mp.y(), ep.y()),
                           qAbs(mp.x() - ep.x()) + 1, qAbs(mp.y() - ep.y()) + 1);

    if (rubberBandRect != oldRubberband || lastRubberbandScenePoint != lastMouseMoveScenePoint) {
        lastRubberbandScenePoint = lastMouseMoveScenePoint;
        oldRubberband = rubberBandRect;
        emit rubberBandChanged(rubberBandRect, mousePressScenePoint, lastRubberbandScenePoint);
    }

    // Update new rubberband
    if (viewportUpdateMode() != QGraphicsView::NoViewportUpdate) {
        if (viewportUpdateMode() != QGraphicsView::FullViewportUpdate)
            viewport()->update(rubberBandRegion(viewport(), rubberBandRect));
        else
            update();
    }
            // Set the new selection area
    QPainterPath selectionArea;
    selectionArea.addPolygon(mapToScene(rubberBandRect));
    selectionArea.closeSubpath();
    if (scene())
    {
        scene()->setSelectionArea(selectionArea, rubberBandSelectionMode, viewportTransform());
        if ( !rubberBandExtendSelection.isEmpty() )
        {
           foreach (QGraphicsItem *item, rubberBandExtendSelection) {
               item->setSelected(true);
           }
        }
    }

    rubberBandX->setGeometry(rubberBandRect);
}

void GraphicsWorkspace::mouseReleaseEvent(QMouseEvent *event)
{

#ifndef QT_NO_RUBBERBAND
    if (dragMode() == QGraphicsView::RubberBandDrag && isInteractive() && !event->buttons()) {
        if (rubberBanding) {
            if (viewportUpdateMode() != QGraphicsView::NoViewportUpdate){
                if (viewportUpdateMode() != FullViewportUpdate)
                    viewport()->update(rubberBandRegion(viewport(), rubberBandRect));
                else
                    update();
            }
            rubberBanding = false;
            rubberBandX->hide();
            if (!rubberBandRect.isNull())
            {
                rubberBandRect = QRect();
                emit rubberBandChanged(rubberBandRect, QPointF(), QPointF());
            }
            rubberBandExtendSelection.clear();
        }
    } else
#endif
    if (dragMode() == QGraphicsView::ScrollHandDrag && event->button() == Qt::LeftButton) {
#ifndef QT_NO_CURSOR
        // Restore the open hand cursor. ### There might be items
        // under the mouse that have a valid cursor at this time, so
        // we could repeat the steps from mouseMoveEvent().
        viewport()->setCursor(Qt::OpenHandCursor);
#endif
        handScrolling = false;

        if (scene() && isInteractive() && !lastMouseEvent.isAccepted() && handScrollMotions <= 6) {
            // If we've detected very little motion during the hand drag, and
            // no item accepted the last event, we'll interpret that as a
            // click to the scene, and reset the selection.
            scene()->clearSelection();
        }
    }

    storeMouseEvent(event);

    if (!isInteractive())
        return;

    if (!scene())
        return;

    QGraphicsSceneMouseEvent mouseEvent(QEvent::GraphicsSceneMouseRelease);
    mouseEvent.setWidget(viewport());
    mouseEvent.setButtonDownScenePos(mousePressButton, mousePressScenePoint);
    mouseEvent.setButtonDownScreenPos(mousePressButton, mousePressScreenPoint);
    mouseEvent.setScenePos(mapToScene(event->pos()));
    mouseEvent.setScreenPos(event->globalPos());
    mouseEvent.setLastScenePos(lastMouseMoveScenePoint);
    mouseEvent.setLastScreenPos(lastMouseMoveScreenPoint);
    mouseEvent.setButtons(event->buttons());
    mouseEvent.setButton(event->button());
    mouseEvent.setModifiers(event->modifiers());
    mouseEvent.setAccepted(false);
    if (event->spontaneous())
        qt_sendSpontaneousEvent(scene(), &mouseEvent);
    else
        QApplication::sendEvent(scene(), &mouseEvent);

    // Update the last mouse event selected state.
    lastMouseEvent.setAccepted(mouseEvent.isAccepted());

#ifndef QT_NO_CURSOR
    if (mouseEvent.isAccepted() && mouseEvent.buttons() == 0 && viewport()->testAttribute(Qt::WA_SetCursor))
    {
        // The last mouse release on the viewport will trigger clearing the cursor.
        _q_unsetViewportCursor();
    }
#endif

}

#ifdef QT_SHARED // avoid conflict with symbol in static lib
bool qt_sendSpontaneousEvent(QObject *receiver, QEvent *event)
{
    return QApplication::sendSpontaneousEvent(receiver, event);
}
#endif
