#include "wld_mode_hand.h"
#include "../wld_scene.h"
#include "../../common_features/mainwinconnect.h"

WLD_ModeHand::WLD_ModeHand(QGraphicsScene *parentScene, QObject *parent)
: EditMode("HandScroll", parentScene, parent)
{}

WLD_ModeHand::~WLD_ModeHand()
{}

void WLD_ModeHand::set()
{
    if(!scene) return;
    WldScene *s = dynamic_cast<WldScene *>(scene);

    s->EraserEnabled=false;
    s->PasteFromBuffer=false;
    s->DrawMode=false;
    s->disableMoveItems=false;

    s->clearSelection();
    s->resetCursor();
    s->resetResizers();
    s->unserPointSelector();

    s->_viewPort->setCursor(Qt::ArrowCursor);
    s->_viewPort->setInteractive(false);
    s->_viewPort->setDragMode(QGraphicsView::ScrollHandDrag);
}

void WLD_ModeHand::mousePress(QGraphicsSceneMouseEvent *mouseEvent)
{
    Q_UNUSED(mouseEvent);
}

void WLD_ModeHand::mouseMove(QGraphicsSceneMouseEvent *mouseEvent)
{
    Q_UNUSED(mouseEvent);
}

void WLD_ModeHand::mouseRelease(QGraphicsSceneMouseEvent *mouseEvent)
{
    Q_UNUSED(mouseEvent);
}

void WLD_ModeHand::keyPress(QKeyEvent *keyEvent)
{
    Q_UNUSED(keyEvent);
}

void WLD_ModeHand::keyRelease(QKeyEvent *keyEvent)
{
    Q_UNUSED(keyEvent);
}
