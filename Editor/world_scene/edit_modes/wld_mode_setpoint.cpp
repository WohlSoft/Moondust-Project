#include "wld_mode_setpoint.h"


WLD_ModeSetPoint::WLD_ModeSetPoint(QGraphicsScene *parentScene, QObject *parent)
    : EditMode("SetPoint", parentScene, parent)
{}

WLD_ModeSetPoint::~WLD_ModeSetPoint()
{

}

void WLD_ModeSetPoint::set()
{
    if(!scene) return;
    WldScene *s = dynamic_cast<WldScene *>(scene);

    s->EraserEnabled=false;
    s->PasteFromBuffer=false;
    s->DrawMode=true;
    s->disableMoveItems=false;

    s->clearSelection();
    s->resetResizers();

    s->_viewPort->setInteractive(true);
    s->_viewPort->setCursor(Qt::CrossCursor);
    s->_viewPort->setDragMode(QGraphicsView::NoDrag);
    s->_viewPort->setRenderHint(QPainter::Antialiasing, true);
    s->_viewPort->viewport()->setMouseTracking(true);
}

void WLD_ModeSetPoint::mousePress(QGraphicsSceneMouseEvent *mouseEvent)
{

}

void WLD_ModeSetPoint::mouseMove(QGraphicsSceneMouseEvent *mouseEvent)
{

}

void WLD_ModeSetPoint::mouseRelease(QGraphicsSceneMouseEvent *mouseEvent)
{

}

void WLD_ModeSetPoint::keyPress(QKeyEvent *keyEvent)
{

}

void WLD_ModeSetPoint::keyRelease(QKeyEvent *keyEvent)
{

}

