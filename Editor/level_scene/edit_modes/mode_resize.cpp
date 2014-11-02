#include "mode_resize.h"

#include "../lvlscene.h"
#include "../../common_features/mainwinconnect.h"
#include "../../common_features/item_rectangles.h"

LVL_ModeResize::LVL_ModeResize(QGraphicsScene *parentScene, QObject *parent)
    : EditMode("Resize", parentScene, parent)
{}

LVL_ModeResize::~LVL_ModeResize()
{}

void LVL_ModeResize::set()
{
    if(!scene) return;
    LvlScene *s = dynamic_cast<LvlScene *>(scene);

    s->EraserEnabled=false;
    s->PasteFromBuffer=false;
    s->DrawMode=true;
    s->disableMoveItems=true;

    s->clearSelection();
    s->resetCursor();

    s->_viewPort->setInteractive(true);
    s->_viewPort->setCursor(Qt::ArrowCursor);
    s->_viewPort->setDragMode(QGraphicsView::NoDrag);
}

void LVL_ModeResize::mousePress(QGraphicsSceneMouseEvent *mouseEvent)
{
    Q_UNUSED(mouseEvent);
    if(!scene) return;
    LvlScene *s = dynamic_cast<LvlScene *>(scene);

    s->MousePressEventOnly = true;
    s->mousePressEvent(mouseEvent);
    dontCallEvent = true;
}

void LVL_ModeResize::mouseMove(QGraphicsSceneMouseEvent *mouseEvent)
{
    Q_UNUSED(mouseEvent);
    if(!scene) return;
    LvlScene *s = dynamic_cast<LvlScene *>(scene);
    s->clearSelection();

    s->MouseMoveEventOnly = true;
    s->mouseMoveEvent(mouseEvent);
    dontCallEvent = true;
}

void LVL_ModeResize::mouseRelease(QGraphicsSceneMouseEvent *mouseEvent)
{
    Q_UNUSED(mouseEvent);
}

void LVL_ModeResize::keyPress(QKeyEvent *keyEvent)
{
    Q_UNUSED(keyEvent);

}

void LVL_ModeResize::keyRelease(QKeyEvent *keyEvent)
{
    if(!scene) return;
    LvlScene *s = dynamic_cast<LvlScene *>(scene);

    switch(keyEvent->key())
    {
        case (Qt::Key_Escape):
            s->resetResizers();
            break;
        case (Qt::Key_Enter):
        case (Qt::Key_Return):
            s->applyResizers();
                //setSectionResizer(false, true);
            break;
        default:
            break;
    }
}
