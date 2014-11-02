#include "mode_hand.h"
#include "../lvlscene.h"
#include "../../common_features/mainwinconnect.h"

ModeHand::ModeHand(QGraphicsScene *parentScene, QObject *parent)
: EditMode("HandScroll", parentScene, parent)
{}

ModeHand::~ModeHand()
{}

void ModeHand::set()
{
    if(!scene) return;
    LvlScene *s = dynamic_cast<LvlScene *>(scene);

    s->EraserEnabled=false;
    s->PasteFromBuffer=false;
    s->DrawMode=false;
    s->disableMoveItems=false;

    s->clearSelection();
    s->resetCursor();
    s->resetResizers();

    s->_viewPort->setCursor(Qt::ArrowCursor);
    s->_viewPort->setInteractive(false);
    s->_viewPort->setDragMode(QGraphicsView::ScrollHandDrag);
}

void ModeHand::mousePress(QGraphicsSceneMouseEvent *mouseEvent)
{
    Q_UNUSED(mouseEvent);
}

void ModeHand::mouseMove(QGraphicsSceneMouseEvent *mouseEvent)
{
    Q_UNUSED(mouseEvent);
}

void ModeHand::mouseRelease(QGraphicsSceneMouseEvent *mouseEvent)
{
    Q_UNUSED(mouseEvent);
}

void ModeHand::keyPress(QKeyEvent *keyEvent)
{
    Q_UNUSED(keyEvent);
}

void ModeHand::keyRelease(QKeyEvent *keyEvent)
{
    Q_UNUSED(keyEvent);
}
