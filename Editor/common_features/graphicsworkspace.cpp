#include "graphicsworkspace.h"

GraphicsWorkspace::GraphicsWorkspace(QWidget *parent) :
    QGraphicsView(parent)
{
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

    setTransformationAnchor(QGraphicsView::AnchorUnderMouse);

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
}

void GraphicsWorkspace::zoomIn()
{
    if(zoomValue>=scaleMax) return;

    zoomValue *= scaleFactor;
    scale(scaleFactor, scaleFactor);
}

void GraphicsWorkspace::zoomOut()
{
    if(zoomValue<=scaleMin) return;

    zoomValue *= 1.0/scaleFactor;
    scale(1.0 / scaleFactor, 1.0 / scaleFactor);
}


void GraphicsWorkspace::moveLeft()
{
    horizontalScrollBar()->setValue(horizontalScrollBar()->value()-step);
}

void GraphicsWorkspace::moveRight()
{
    horizontalScrollBar()->setValue(horizontalScrollBar()->value()+step);
}

void GraphicsWorkspace::moveUp()
{
    verticalScrollBar()->setValue(verticalScrollBar()->value()-step);
}

void GraphicsWorkspace::moveDown()
{
    verticalScrollBar()->setValue(verticalScrollBar()->value()+step);
}

void GraphicsWorkspace::keyPressEvent(QKeyEvent *event)
{
    if(event->isAutoRepeat() )
           {
               event->ignore();
               return;
           }

    switch(event->key())
    {
    case Qt::Key_Left:
        lMover.start(keyTime);
        break;
    case Qt::Key_Right:
        rMover.start(keyTime);
        break;
    case Qt::Key_Up:
        uMover.start(keyTime);
        break;
    case Qt::Key_Down:
        dMover.start(keyTime);
        break;
    case Qt::Key_Shift:
        keyTime=5;
        updateTimerInterval();
        break;
    }
    event->accept();

    QGraphicsView::keyPressEvent(event);
}

void GraphicsWorkspace::keyReleaseEvent(QKeyEvent *event)
{
    if(event->isAutoRepeat() )
           {
               event->ignore();
               return;
           }

    switch(event->key())
    {
    case Qt::Key_Left:
        lMover.stop();
        break;
    case Qt::Key_Right:
        rMover.stop();
        break;
    case Qt::Key_Up:
        uMover.stop();
        break;
    case Qt::Key_Down:
        dMover.stop();
        break;
    case Qt::Key_Shift:
        keyTime=25;
        updateTimerInterval();
        break;
    }
    event->accept();
    QGraphicsView::keyReleaseEvent(event);
}

void GraphicsWorkspace::updateTimerInterval()
{
    lMover.setInterval(keyTime);
    rMover.setInterval(keyTime);
    uMover.setInterval(keyTime);
    dMover.setInterval(keyTime);
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
          if(zoomValue>=scaleMax) return;
          // Zoom in
          zoomValue *= scaleFactor;
          scale(scaleFactor, scaleFactor);
      } else {
          if(zoomValue<=scaleMin) return;
          // Zooming out
          zoomValue *= 1.0/scaleFactor;
          scale(1.0 / scaleFactor, 1.0 / scaleFactor);
      }
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
        return;
    }

    QGraphicsView::wheelEvent(event);
}
